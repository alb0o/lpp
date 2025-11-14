#include "StaticAnalyzer.h"
#include <iostream>
#include <algorithm>
#include <queue>

namespace lpp
{
    StaticAnalyzer::StaticAnalyzer() {}

    std::vector<AnalysisIssue> StaticAnalyzer::analyze(Program &program)
    {
        issues.clear();
        program.accept(*this);

        // Run global analyses
        checkMemoryLeak();
        checkDeadCode();

        return issues;
    }

    CFGNode *StaticAnalyzer::createNode(CFGNode::Type type)
    {
        auto node = std::make_unique<CFGNode>();
        node->type = type;
        node->id = cfg.size();
        CFGNode *ptr = node.get();
        cfg.push_back(std::move(node));
        return ptr;
    }

    void StaticAnalyzer::connectNodes(CFGNode *from, CFGNode *to)
    {
        if (from && to)
        {
            from->successors.push_back(to);
            to->predecessors.push_back(from);
        }
    }

    void StaticAnalyzer::buildCFG(std::vector<std::unique_ptr<Statement>> &statements)
    {
        entryBlock = createNode(CFGNode::Type::ENTRY);
        exitBlock = createNode(CFGNode::Type::EXIT);
        currentBlock = entryBlock;

        for (auto &stmt : statements)
        {
            auto stmtNode = createNode(CFGNode::Type::STATEMENT);
            stmtNode->stmt = stmt.get();
            connectNodes(currentBlock, stmtNode);
            currentBlock = stmtNode;
        }

        connectNodes(currentBlock, exitBlock);
    }

    void StaticAnalyzer::runDataFlowAnalysis()
    {
        // Worklist algorithm for data flow analysis
        std::queue<CFGNode *> worklist;
        std::set<CFGNode *> inWorklist;

        // Initialize
        for (auto &node : cfg)
        {
            worklist.push(node.get());
            inWorklist.insert(node.get());
        }

        // Iterate until fixpoint
        while (!worklist.empty())
        {
            CFGNode *node = worklist.front();
            worklist.pop();
            inWorklist.erase(node);

            // Merge incoming states
            mergeStates(node);

            // Apply transfer function
            bool changed = transferFunction(node);

            // If state changed, add successors to worklist
            if (changed)
            {
                for (auto *succ : node->successors)
                {
                    if (inWorklist.find(succ) == inWorklist.end())
                    {
                        worklist.push(succ);
                        inWorklist.insert(succ);
                    }
                }
            }
        }
    }

    bool StaticAnalyzer::transferFunction(CFGNode *node)
    {
        // Save old state
        auto oldState = node->stateOut;

        // Copy input state to output
        node->stateOut = node->stateIn;

        // Apply statement effects
        if (node->stmt)
        {
            // Simulate statement execution on symbolic state
            // This modifies stateOut based on what the statement does

            if (auto *varDecl = dynamic_cast<VarDecl *>(node->stmt))
            {
                SymbolicValue val;
                if (varDecl->initializer)
                {
                    val.state = SymbolicValue::State::INITIALIZED;
                }
                else
                {
                    val.state = SymbolicValue::State::UNINITIALIZED;
                }
                node->stateOut[varDecl->name] = val;
            }
            else if (auto *assign = dynamic_cast<Assignment *>(node->stmt))
            {
                SymbolicValue val;
                val.state = SymbolicValue::State::INITIALIZED;
                node->stateOut[assign->name] = val;
            }
        }

        // Check if state changed
        return oldState != node->stateOut;
    }

    void StaticAnalyzer::mergeStates(CFGNode *node)
    {
        if (node->predecessors.empty())
        {
            return;
        }

        // Merge all predecessor states
        node->stateIn.clear();

        for (auto *pred : node->predecessors)
        {
            for (auto &[varName, value] : pred->stateOut)
            {
                auto it = node->stateIn.find(varName);
                if (it == node->stateIn.end())
                {
                    node->stateIn[varName] = value;
                }
                else
                {
                    // Merge values conservatively
                    if (it->second.state != value.state)
                    {
                        it->second.state = SymbolicValue::State::UNKNOWN;
                    }
                    it->second.mayBeNull = it->second.mayBeNull || value.mayBeNull;
                }
            }
        }
    }

    void StaticAnalyzer::checkNullDereference(IdentifierExpr &node)
    {
        auto it = symbolTable.find(node.name);
        if (it != symbolTable.end())
        {
            if (it->second.definitelyNull)
            {
                reportIssue(IssueType::NULL_DEREFERENCE, Severity::ERROR,
                            "Null pointer dereference: '" + node.name + "' is definitely null",
                            {"Variable was set to null earlier"});
            }
            else if (it->second.mayBeNull)
            {
                reportIssue(IssueType::NULL_DEREFERENCE, Severity::WARNING,
                            "Potential null pointer dereference: '" + node.name + "' may be null",
                            {"Consider checking for null before use"});
            }
        }
    }

    void StaticAnalyzer::checkDivisionByZero(BinaryExpr &node)
    {
        if (node.op == "/")
        {
            if (canBeZero(node.right.get()))
            {
                reportIssue(IssueType::DIVISION_BY_ZERO, Severity::ERROR,
                            "Division by zero detected",
                            {"Right operand can be zero"});
            }
        }
    }

    void StaticAnalyzer::checkUninitializedRead(IdentifierExpr &node)
    {
        auto it = symbolTable.find(node.name);
        if (it != symbolTable.end())
        {
            if (it->second.state == SymbolicValue::State::UNINITIALIZED)
            {
                reportIssue(IssueType::UNINITIALIZED_READ, Severity::ERROR,
                            "Use of uninitialized variable: '" + node.name + "'",
                            {"Variable declared but never assigned a value"});
            }
        }
    }

    void StaticAnalyzer::checkIntegerOverflow(BinaryExpr &node)
    {
        if (node.op == "+" || node.op == "*")
        {
            // Check if operation can overflow
            auto leftVal = evaluateExpression(node.left.get());
            auto rightVal = evaluateExpression(node.right.get());

            if (leftVal.constantValue && rightVal.constantValue)
            {
                int64_t result;
                if (node.op == "+")
                {
                    result = *leftVal.constantValue + *rightVal.constantValue;
                }
                else
                {
                    result = *leftVal.constantValue * *rightVal.constantValue;
                }

                // Check for overflow (simplified)
                if (result > INT32_MAX || result < INT32_MIN)
                {
                    reportIssue(IssueType::INTEGER_OVERFLOW, Severity::WARNING,
                                "Integer overflow in arithmetic operation",
                                {"Result exceeds 32-bit integer bounds"});
                }
            }
        }
    }

    void StaticAnalyzer::checkMemoryLeak()
    {
        for (const auto &allocated : allocatedMemory)
        {
            if (freedMemory.find(allocated) == freedMemory.end())
            {
                reportIssue(IssueType::MEMORY_LEAK, Severity::WARNING,
                            "Potential memory leak: allocated memory never freed",
                            {"Variable: " + allocated});
            }
        }
    }

    void StaticAnalyzer::checkDeadCode()
    {
        for (auto &node : cfg)
        {
            if (visitedNodes.find(node.get()) == visitedNodes.end())
            {
                if (node->type == CFGNode::Type::STATEMENT)
                {
                    reportIssue(IssueType::DEAD_CODE, Severity::WARNING,
                                "Dead code detected: statement is unreachable",
                                {});
                }
            }
        }
    }

    bool StaticAnalyzer::canBeZero(Expression *expr)
    {
        if (auto *num = dynamic_cast<NumberExpr *>(expr))
        {
            return num->value == 0.0;
        }

        if (auto *ident = dynamic_cast<IdentifierExpr *>(expr))
        {
            auto it = symbolTable.find(ident->name);
            if (it != symbolTable.end() && it->second.constantValue)
            {
                return *it->second.constantValue == 0;
            }
        }

        return true; // Conservative: assume it might be zero
    }

    bool StaticAnalyzer::canBeNull(Expression *expr)
    {
        if (auto *ident = dynamic_cast<IdentifierExpr *>(expr))
        {
            auto it = symbolTable.find(ident->name);
            if (it != symbolTable.end())
            {
                return it->second.mayBeNull;
            }
        }
        return false;
    }

    SymbolicValue StaticAnalyzer::evaluateExpression(Expression *expr)
    {
        SymbolicValue result;

        if (auto *num = dynamic_cast<NumberExpr *>(expr))
        {
            result.state = SymbolicValue::State::INITIALIZED;
            result.constantValue = static_cast<int64_t>(num->value);
        }
        else if (auto *ident = dynamic_cast<IdentifierExpr *>(expr))
        {
            auto it = symbolTable.find(ident->name);
            if (it != symbolTable.end())
            {
                result = it->second;
            }
        }

        return result;
    }

    void StaticAnalyzer::reportIssue(IssueType type, Severity severity,
                                     const std::string &message,
                                     const std::vector<std::string> &notes)
    {
        AnalysisIssue issue;
        issue.type = type;
        issue.severity = severity;
        issue.message = message;
        issue.line = currentLine;
        issue.function = currentFunction;
        issue.notes = notes;
        issues.push_back(issue);
    }

    // Visitor implementations
    void StaticAnalyzer::visit(NumberExpr &node) {}
    void StaticAnalyzer::visit(StringExpr &node) {}
    void StaticAnalyzer::visit(TemplateLiteralExpr &node) {}
    void StaticAnalyzer::visit(BoolExpr &node) {}

    void StaticAnalyzer::visit(IdentifierExpr &node)
    {
        checkNullDereference(node);
        checkUninitializedRead(node);
    }

    void StaticAnalyzer::visit(BinaryExpr &node)
    {
        node.left->accept(*this);
        node.right->accept(*this);

        checkDivisionByZero(node);
        checkIntegerOverflow(node);
    }

    void StaticAnalyzer::visit(UnaryExpr &node)
    {
        node.operand->accept(*this);
    }

    void StaticAnalyzer::visit(CallExpr &node)
    {
        for (auto &arg : node.arguments)
        {
            arg->accept(*this);
        }
    }

    void StaticAnalyzer::visit(LambdaExpr &node)
    {
        node.body->accept(*this);
    }

    void StaticAnalyzer::visit(TernaryIfExpr &node)
    {
        node.condition->accept(*this);
        node.thenExpr->accept(*this);
        if (node.elseExpr)
        {
            node.elseExpr->accept(*this);
        }
    }

    void StaticAnalyzer::visit(PipelineExpr &node)
    {
        node.initial->accept(*this);
        for (auto &stage : node.stages)
        {
            stage->accept(*this);
        }
    }

    void StaticAnalyzer::visit(CompositionExpr &node)
    {
        for (auto &func : node.functions)
        {
            func->accept(*this);
        }
    }

    void StaticAnalyzer::visit(RangeExpr &node)
    {
        node.start->accept(*this);
        node.end->accept(*this);
        if (node.step)
        {
            node.step->accept(*this);
        }
    }

    void StaticAnalyzer::visit(ArrayExpr &node)
    {
        for (auto &elem : node.elements)
        {
            elem->accept(*this);
        }
    }

    void StaticAnalyzer::visit(ListComprehension &node)
    {
        node.range->accept(*this);
        for (auto &pred : node.predicates)
        {
            pred->accept(*this);
        }
        node.expression->accept(*this);
    }

    void StaticAnalyzer::visit(SpreadExpr &node)
    {
        node.expression->accept(*this);
    }

    void StaticAnalyzer::visit(IndexExpr &node)
    {
        node.object->accept(*this);
        node.index->accept(*this);
    }

    void StaticAnalyzer::visit(ObjectExpr &node)
    {
        for (auto &[key, value] : node.properties)
        {
            value->accept(*this);
        }
    }

    void StaticAnalyzer::visit(MatchExpr &node)
    {
        node.expression->accept(*this);
        for (auto &[pattern, result] : node.cases)
        {
            pattern->accept(*this);
            result->accept(*this);
        }
    }

    void StaticAnalyzer::visit(VarDecl &node)
    {
        currentLine++;

        SymbolicValue val;
        if (node.initializer)
        {
            node.initializer->accept(*this);
            val.state = SymbolicValue::State::INITIALIZED;
        }
        else
        {
            val.state = SymbolicValue::State::UNINITIALIZED;
        }

        symbolTable[node.name] = val;
    }

    void StaticAnalyzer::visit(Assignment &node)
    {
        currentLine++;

        node.value->accept(*this);

        SymbolicValue val;
        val.state = SymbolicValue::State::INITIALIZED;
        symbolTable[node.name] = val;
    }

    void StaticAnalyzer::visit(IfStmt &node)
    {
        node.condition->accept(*this);

        for (auto &stmt : node.thenBranch)
        {
            stmt->accept(*this);
        }

        for (auto &stmt : node.elseBranch)
        {
            stmt->accept(*this);
        }
    }

    void StaticAnalyzer::visit(WhileStmt &node)
    {
        node.condition->accept(*this);

        for (auto &stmt : node.body)
        {
            stmt->accept(*this);
        }
    }

    void StaticAnalyzer::visit(ReturnStmt &node)
    {
        currentLine++;
        if (node.value)
        {
            node.value->accept(*this);
        }
    }

    void StaticAnalyzer::visit(ExprStmt &node)
    {
        currentLine++;
        node.expression->accept(*this);
    }

    void StaticAnalyzer::visit(Function &node)
    {
        currentFunction = node.name;
        symbolTable.clear();

        // Initialize parameters as initialized
        for (auto &[paramName, paramType] : node.parameters)
        {
            SymbolicValue val;
            val.state = SymbolicValue::State::INITIALIZED;
            symbolTable[paramName] = val;
        }

        // Build CFG for function
        buildCFG(node.body);

        // Run data flow analysis
        runDataFlowAnalysis();

        // Visit statements
        for (auto &stmt : node.body)
        {
            stmt->accept(*this);
        }
    }

    void StaticAnalyzer::visit(ClassDecl &node)
    {
        for (auto &method : node.methods)
        {
            method->accept(*this);
        }
    }

    void StaticAnalyzer::visit(InterfaceDecl &node) {}
    void StaticAnalyzer::visit(TypeDecl &node) {}

    void StaticAnalyzer::visit(Program &node)
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

} // namespace lpp
