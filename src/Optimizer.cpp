#include "Optimizer.h"
#include <iostream>

namespace lpp
{

    void Optimizer::optimize(Program &ast)
    {
        std::cout << "Running optimizer passes...\n";

        constantFolding(ast);
        deadCodeElimination(ast);
        inlineExpansion(ast);
        strengthReduction(ast);
        commonSubexpressionElimination(ast);

        std::cout << "Optimization complete:\n";
        std::cout << "  Constants folded: " << stats.constantsFolded << "\n";
        std::cout << "  Dead code removed: " << stats.deadCodeRemoved << "\n";
        std::cout << "  Functions inlined: " << stats.functionsInlined << "\n";
        std::cout << "  Expressions simplified: " << stats.expressionsSimplified << "\n";
    }

    void Optimizer::constantFolding(Program &ast)
    {
        // Fold constant expressions at compile time
        // Example: 2 + 3 -> 5
        // This requires traversing the AST and replacing BinaryExpr with constant operands

        // Placeholder implementation
        stats.constantsFolded = 0;
    }

    void Optimizer::deadCodeElimination(Program &ast)
    {
        // Remove unreachable code
        // - Code after return statements
        // - Branches that are never taken (if(false) {...})

        for (auto &func : ast.functions)
        {
            // Analyze control flow and remove dead code
        }

        stats.deadCodeRemoved = 0;
    }

    void Optimizer::inlineExpansion(Program &ast)
    {
        // Inline small functions at call sites
        // Benefits: eliminates function call overhead
        // Criteria: function size < threshold, not recursive

        stats.functionsInlined = 0;
    }

    void Optimizer::strengthReduction(Program &ast)
    {
        // Replace expensive operations with cheaper equivalents
        // Examples:
        // - x * 2 -> x << 1
        // - x / 2 -> x >> 1
        // - x * 0 -> 0
        // - x * 1 -> x

        stats.expressionsSimplified = 0;
    }

    void Optimizer::commonSubexpressionElimination(Program &ast)
    {
        // Eliminate redundant calculations
        // Example:
        //   let a = x + y;
        //   let b = x + y;  // Reuse result from 'a'

        stats.expressionsSimplified = 0;
    }

    std::unique_ptr<Expression> Optimizer::foldBinaryExpression(BinaryExpr *expr)
    {
        // Check if both operands are constants
        if (!isConstant(expr->left.get()) || !isConstant(expr->right.get()))
        {
            return nullptr;
        }

        int leftVal = evaluateConstant(expr->left.get());
        int rightVal = evaluateConstant(expr->right.get());
        int result = 0;

        if (expr->op == "+")
        {
            result = leftVal + rightVal;
        }
        else if (expr->op == "-")
        {
            result = leftVal - rightVal;
        }
        else if (expr->op == "*")
        {
            result = leftVal * rightVal;
        }
        else if (expr->op == "/")
        {
            if (rightVal != 0)
            {
                result = leftVal / rightVal;
            }
            else
            {
                return nullptr; // Division by zero
            }
        }

        stats.constantsFolded++;
        return std::make_unique<NumberExpr>(result);
    }

    std::unique_ptr<Expression> Optimizer::foldUnaryExpression(UnaryExpr *expr)
    {
        if (!isConstant(expr->operand.get()))
        {
            return nullptr;
        }

        int val = evaluateConstant(expr->operand.get());

        if (expr->op == "-")
        {
            stats.constantsFolded++;
            return std::make_unique<NumberExpr>(-val);
        }
        else if (expr->op == "!")
        {
            stats.constantsFolded++;
            return std::make_unique<BoolExpr>(val == 0);
        }

        return nullptr;
    }

    bool Optimizer::isConstant(Expression *expr)
    {
        return dynamic_cast<NumberExpr *>(expr) != nullptr ||
               dynamic_cast<StringExpr *>(expr) != nullptr ||
               dynamic_cast<BoolExpr *>(expr) != nullptr;
    }

    int Optimizer::evaluateConstant(Expression *expr)
    {
        if (auto num = dynamic_cast<NumberExpr *>(expr))
        {
            return num->value;
        }
        if (auto boolean = dynamic_cast<BoolExpr *>(expr))
        {
            return boolean->value ? 1 : 0;
        }
        return 0;
    }

    bool Optimizer::hasNoSideEffects(Expression *expr)
    {
        // Check if expression has no side effects (pure)
        // - No function calls
        // - No assignments
        // - No I/O operations

        return isConstant(expr);
    }

} // namespace lpp
