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
        // FIX BUG #131: Report when optimization is skipped
        // TODO: Emit compiler warnings to stderr or log:
        // - "Warning: Constant folding not fully implemented (placeholder)"
        // - Track stats.optimizationsSkipped counter
        // - Display at end of compilation: "X optimizations were unavailable"
        // - Help users understand why code isn't optimized
        // Useful for debugging performance issues

        // Placeholder implementation
        stats.constantsFolded = 0;
    }

    void Optimizer::deadCodeElimination(Program &ast)
    {
        // Remove unreachable code
        // - Code after return statements
        // - Branches that are never taken (if(false) {...})
        // FIX BUG #83: Currently not implemented, placeholder only
        // TODO: Implement proper unreachable code detection
        // FIX BUG #131: Warn when dead code elimination skipped
        // TODO: Add compiler warning: "Dead code elimination not implemented"

        for (auto &func : ast.functions)
        {
            // Analyze control flow and remove dead code
            // TODO: Use CFG from StaticAnalyzer
        }

        stats.deadCodeRemoved = 0;
    }

    void Optimizer::inlineExpansion(Program &ast)
    {
        // Inline small functions at call sites
        // Benefits: eliminates function call overhead
        // Criteria: function size < threshold, not recursive
        // FIX BUG #82: TODO - Add recursion detection to prevent infinite inlining

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
            // FIX BUG #80: Check for overflow
            if ((rightVal > 0 && leftVal > INT_MAX - rightVal) ||
                (rightVal < 0 && leftVal < INT_MIN - rightVal))
            {
                return nullptr; // Overflow, don't fold
            }
            result = leftVal + rightVal;
        }
        else if (expr->op == "-")
        {
            // FIX BUG #80: Check for overflow
            if ((rightVal < 0 && leftVal > INT_MAX + rightVal) ||
                (rightVal > 0 && leftVal < INT_MIN + rightVal))
            {
                return nullptr; // Overflow, don't fold
            }
            result = leftVal - rightVal;
        }
        else if (expr->op == "*")
        {
            // FIX BUG #80: Check for multiplication overflow
            if (leftVal != 0 && (result / leftVal) != rightVal)
            {
                return nullptr; // Potential overflow
            }
            result = leftVal * rightVal;
        }
        else if (expr->op == "/")
        {
            if (rightVal != 0)
            {
                // Use double division to preserve precision
                double resultDouble = static_cast<double>(leftVal) / static_cast<double>(rightVal);
                result = resultDouble;
            }
            else
            {
                // FIX BUG #79: Report division by zero
                // TODO: Report warning through proper channel
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
            // FIX BUG #81: Check for INT_MIN overflow
            if (val == INT_MIN)
            {
                return nullptr; // -INT_MIN overflows, don't fold
            }
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
        // FIX BUG #89: Include null in constant detection
        return dynamic_cast<NumberExpr *>(expr) != nullptr ||
               dynamic_cast<StringExpr *>(expr) != nullptr ||
               dynamic_cast<BoolExpr *>(expr) != nullptr;
        // TODO: Add NullExpr when implemented
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
        // FIX BUG #90: Warn about fallback
        // TODO: Add proper error reporting for unevaluable constants
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
