#include "BorrowChecker.h"
#include <iostream>
#include <algorithm>

namespace lpp
{
    BorrowChecker::BorrowChecker()
    {
        // Global scope
        scopes.push_back({});
    }

    std::vector<BorrowError> BorrowChecker::check(Program &program)
    {
        errors.clear();
        program.accept(*this);
        return errors;
    }

    void BorrowChecker::enterScope()
    {
        scopeLevel++;
        scopes.push_back({});
    }

    void BorrowChecker::exitScope()
    {
        // Check for dangling references before exiting
        if (!scopes.empty())
        {
            auto &currentScope = scopes.back();
            for (auto &[name, info] : currentScope)
            {
                // Check if borrowed variables outlive their scope
                if (!info.borrowedBy.empty())
                {
                    reportError(BorrowError::Type::DANGLING_REFERENCE, name,
                                "Variable has active borrows when going out of scope");
                }

                // FIX BUG #97: Clear borrows of parent scope variables
                // When a scope exits, any variables from parent scopes that were
                // borrowed in this scope should have those borrows removed
                // TODO: Track which scope each borrow comes from
            }
            scopes.pop_back();
            scopeLevel--;
        }
        else
        {
            // Scope mismatch - log error but don't crash
            if (scopeLevel > 0)
                scopeLevel--;
        }
    }

    VarInfo *BorrowChecker::findVariable(const std::string &name)
    {
        // Search from innermost to outermost scope
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            auto found = it->find(name);
            if (found != it->end())
            {
                return &found->second;
            }
        }
        return nullptr;
    }

    void BorrowChecker::declareVariable(const std::string &name, bool isMutable)
    {
        if (!scopes.empty())
        {
            VarInfo info;
            info.name = name;
            info.ownership = Ownership::OWNED;
            info.declarationLine = currentLine;
            info.lastUseLine = currentLine;
            info.isMutable = isMutable;

            scopes.back()[name] = info;
        }
    }

    void BorrowChecker::useVariable(const std::string &name)
    {
        VarInfo *var = findVariable(name);
        if (!var)
        {
            reportError(BorrowError::Type::USE_AFTER_MOVE, name,
                        "Variable not found or already moved");
            return;
        }

        // Check if variable was moved
        if (var->ownership == Ownership::MOVED)
        {
            reportError(BorrowError::Type::USE_AFTER_MOVE, name,
                        "Use of moved value '" + name + "'");
            return;
        }

        var->lastUseLine = currentLine;
    }

    void BorrowChecker::moveVariable(const std::string &name)
    {
        VarInfo *var = findVariable(name);
        if (!var)
        {
            reportError(BorrowError::Type::USE_AFTER_MOVE, name,
                        "Cannot move undefined variable");
            return;
        }

        // Check if variable is borrowed
        if (!var->borrowedBy.empty())
        {
            reportError(BorrowError::Type::BORROW_AFTER_MOVE, name,
                        "Cannot move '" + name + "' while it is borrowed");
            return;
        }

        var->ownership = Ownership::MOVED;
    }

    void BorrowChecker::borrowVariable(const std::string &name, bool mutable_borrow)
    {
        VarInfo *var = findVariable(name);
        if (!var)
        {
            reportError(BorrowError::Type::USE_AFTER_MOVE, name,
                        "Cannot borrow undefined variable");
            return;
        }

        // Check if already moved
        if (var->ownership == Ownership::MOVED)
        {
            reportError(BorrowError::Type::BORROW_AFTER_MOVE, name,
                        "Cannot borrow moved value '" + name + "'");
            return;
        }

        // FIX BUG #96: Track borrow site to detect aliasing
        // TODO: Store source location (line, column) in borrowedBy
        // to provide better error messages showing conflicting borrows

        // Mutable borrow rules
        if (mutable_borrow)
        {
            // Cannot have any other borrows (mutable or immutable)
            if (var->ownership == Ownership::BORROWED ||
                var->ownership == Ownership::MUT_BORROWED ||
                !var->borrowedBy.empty())
            {
                reportError(BorrowError::Type::DOUBLE_BORROW_MUT, name,
                            "Cannot borrow '" + name + "' as mutable, already borrowed");
                return;
            }
            var->ownership = Ownership::MUT_BORROWED;
            var->borrowedBy.insert("<mutable>");
        }
        else
        {
            // Cannot borrow immutably if already mutably borrowed
            if (var->ownership == Ownership::MUT_BORROWED ||
                var->borrowedBy.count("<mutable>") > 0)
            {
                reportError(BorrowError::Type::DOUBLE_BORROW_MUT, name,
                            "Cannot borrow '" + name + "' as immutable, already mutably borrowed");
                return;
            }
            var->ownership = Ownership::BORROWED;
            var->borrowedBy.insert("<immutable>");
        }
    }

    void BorrowChecker::reportError(BorrowError::Type type, const std::string &var,
                                    const std::string &msg)
    {
        BorrowError error;
        error.type = type;
        error.variable = var;
        error.line = currentLine;
        error.column = currentColumn;
        error.message = msg;
        errors.push_back(error);
    }

    // Visitor implementations
    void BorrowChecker::visit(NumberExpr &node) {}
    void BorrowChecker::visit(StringExpr &node) {}
    void BorrowChecker::visit(TemplateLiteralExpr &node) {}
    void BorrowChecker::visit(BoolExpr &node) {}

    void BorrowChecker::visit(IdentifierExpr &node)
    {
        useVariable(node.name);

        // FIX BUG #101: Track nested borrow chains
        // TODO: When borrowing &x, track that new reference depends on x's lifetime
        // Example: let y = &x; let z = &y; // z depends on x
        // Implement borrow graph to track transitive dependencies
    }

    void BorrowChecker::visit(BinaryExpr &node)
    {
        node.left->accept(*this);
        node.right->accept(*this);
    }

    void BorrowChecker::visit(UnaryExpr &node)
    {
        // Check for move operator (e.g., move(x))
        if (node.op == "move")
        {
            if (auto *ident = dynamic_cast<IdentifierExpr *>(node.operand.get()))
            {
                moveVariable(ident->name);
            }
        }
        node.operand->accept(*this);
    }

    void BorrowChecker::visit(CallExpr &node)
    {
        // Function calls might move arguments
        for (auto &arg : node.arguments)
        {
            arg->accept(*this);
        }
    }

    void BorrowChecker::visit(LambdaExpr &node)
    {
        // FIX BUG #95: Closure capture analysis missing
        // TODO: Track captured variables from outer scopes
        // - Detect which variables are captured by closure
        // - Mark captured variables as borrowed for lambda lifetime
        // - Prevent move of captured variables while closure exists
        // - Validate capture semantics (move vs borrow)

        enterScope();
        for (auto &[paramName, paramType] : node.parameters)
        {
            declareVariable(paramName, false);
        }
        node.body->accept(*this);
        exitScope();
    }

    void BorrowChecker::visit(TernaryIfExpr &node)
    {
        node.condition->accept(*this);
        node.thenExpr->accept(*this);
        if (node.elseExpr)
        {
            node.elseExpr->accept(*this);
        }
    }

    void BorrowChecker::visit(PipelineExpr &node)
    {
        node.initial->accept(*this);
        for (auto &stage : node.stages)
        {
            stage->accept(*this);
        }
    }

    void BorrowChecker::visit(CompositionExpr &node)
    {
        for (auto &func : node.functions)
        {
            func->accept(*this);
        }
    }

    void BorrowChecker::visit(RangeExpr &node)
    {
        node.start->accept(*this);
        node.end->accept(*this);
        if (node.step)
        {
            node.step->accept(*this);
        }
    }

    void BorrowChecker::visit(MapExpr &node)
    {
        node.iterable->accept(*this);
        node.fn->accept(*this);
    }

    void BorrowChecker::visit(FilterExpr &node)
    {
        node.iterable->accept(*this);
        node.predicate->accept(*this);
    }

    void BorrowChecker::visit(ReduceExpr &node)
    {
        node.iterable->accept(*this);
        node.fn->accept(*this);
        if (node.initial)
        {
            node.initial->accept(*this);
        }
    }

    void BorrowChecker::visit(IterateWhileExpr &node)
    {
        node.start->accept(*this);
        node.condition->accept(*this);
        node.stepFn->accept(*this);
    }

    void BorrowChecker::visit(AutoIterateExpr &node)
    {
        node.start->accept(*this);
        node.limit->accept(*this);
    }

    void BorrowChecker::visit(IterateStepExpr &node)
    {
        node.start->accept(*this);
        node.stepFn->accept(*this);
        node.condition->accept(*this);
    }

    void BorrowChecker::visit(ArrayExpr &node)
    {
        for (auto &elem : node.elements)
        {
            elem->accept(*this);
        }
    }

    void BorrowChecker::visit(ListComprehension &node)
    {
        enterScope();
        declareVariable(node.variable, false);
        node.range->accept(*this);
        for (auto &pred : node.predicates)
        {
            pred->accept(*this);
        }
        node.expression->accept(*this);
        exitScope();
    }

    void BorrowChecker::visit(SpreadExpr &node)
    {
        node.expression->accept(*this);
    }

    void BorrowChecker::visit(IndexExpr &node)
    {
        node.object->accept(*this);
        node.index->accept(*this);

        // FIX BUG #100: Field/index access should check borrow state
        // TODO: If object is borrowed mutably, indexing should also be mutable borrow
        // If object is moved, accessing fields/indices should error
        // Track field-level borrows for partial moves (obj.field moved, obj.other_field still accessible)
    }

    void BorrowChecker::visit(ObjectExpr &node)
    {
        for (auto &[key, value] : node.properties)
        {
            value->accept(*this);
        }
    }

    void BorrowChecker::visit(MatchExpr &node)
    {
        node.expression->accept(*this);
        for (auto &[pattern, result] : node.cases)
        {
            pattern->accept(*this);
            result->accept(*this);
        }
    }

    void BorrowChecker::visit(VarDecl &node)
    {
        currentLine++;

        // Check if it's a mutable declaration (let mut x)
        bool isMutable = (node.type.find("mut") != std::string::npos);

        if (node.initializer)
        {
            node.initializer->accept(*this);
        }

        declareVariable(node.name, isMutable);
    }

    void BorrowChecker::visit(Assignment &node)
    {
        currentLine++;

        VarInfo *var = findVariable(node.name);
        if (!var)
        {
            reportError(BorrowError::Type::USE_AFTER_MOVE, node.name,
                        "Cannot assign to undefined variable");
            return;
        }

        // Check if variable is immutable
        if (!var->isMutable)
        {
            reportError(BorrowError::Type::IMMUTABLE_ASSIGN, node.name,
                        "Cannot assign to immutable variable '" + node.name + "'");
            return;
        }

        // Check if variable was moved
        if (var->ownership == Ownership::MOVED)
        {
            reportError(BorrowError::Type::USE_AFTER_MOVE, node.name,
                        "Cannot assign to moved value '" + node.name + "'");
            return;
        }

        node.value->accept(*this);
    }

    void BorrowChecker::visit(IfStmt &node)
    {
        node.condition->accept(*this);

        enterScope();
        for (auto &stmt : node.thenBranch)
        {
            stmt->accept(*this);
        }
        exitScope();

        if (!node.elseBranch.empty())
        {
            enterScope();
            for (auto &stmt : node.elseBranch)
            {
                stmt->accept(*this);
            }
            exitScope();
        }
    }

    void BorrowChecker::visit(WhileStmt &node)
    {
        node.condition->accept(*this);

        enterScope();
        for (auto &stmt : node.body)
        {
            stmt->accept(*this);
        }
        exitScope();
    }

    void BorrowChecker::visit(ReturnStmt &node)
    {
        currentLine++;
        if (node.value)
        {
            node.value->accept(*this);

            // FIX BUG #98: Validate return value lifetime
            // TODO: Check if returning reference to local variable
            // - If return type is &T or &mut T, ensure referent outlives function
            // - Detect "return &local_var" pattern
            // - Validate lifetime annotations on return type
            // Example error: "Cannot return reference to local variable"
        }
    }

    void BorrowChecker::visit(ExprStmt &node)
    {
        currentLine++;
        node.expression->accept(*this);
    }

    void BorrowChecker::visit(Function &node)
    {
        enterScope();

        for (auto &[paramName, paramType] : node.parameters)
        {
            // Check if parameter is &mut or &
            bool isMutable = (paramType.find("&mut") != std::string::npos);
            bool isBorrow = (paramType.find("&") != std::string::npos);

            declareVariable(paramName, isMutable);

            // FIX BUG #99: Reference parameters should be marked as borrowed
            // TODO: If paramType is &T or &mut T, set ownership to BORROWED/MUT_BORROWED
            // This ensures borrow checker rules apply to parameters
            if (isBorrow)
            {
                // Should set var->ownership = isMutable ? MUT_BORROWED : BORROWED
            }
        }

        for (auto &stmt : node.body)
        {
            stmt->accept(*this);
        }

        exitScope();
    }

    void BorrowChecker::visit(ClassDecl &node)
    {
        for (auto &method : node.methods)
        {
            method->accept(*this);
        }
    }

    void BorrowChecker::visit(InterfaceDecl &node) {}
    void BorrowChecker::visit(TypeDecl &node) {}

    void BorrowChecker::visit(Program &node)
    {
        for (auto &func : node.functions)
        {
            func->accept(*this);
        }

        for (auto &cls : node.classes)
        {
            cls->accept(*this);
        }
    }

    // FIX BUG #94: Lifetime tracking not implemented
    // TODO: Implement lifetime validation
    // - Check that borrowed references don't outlive their referents
    // - Validate lifetime annotations ('a, 'b, 'static)
    // - Detect lifetime elision failures
    // - Track lifetime relationships across function boundaries
    void BorrowChecker::checkLifetimes()
    {
        // Placeholder - needs full implementation (~500 lines)
        // Current: No lifetime validation performed
    }

} // namespace lpp
