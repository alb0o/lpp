#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "AST.h"
#include <memory>

namespace lpp
{

    class Optimizer
    {
    public:
        // Optimization passes
        void optimize(Program &ast);

        // Individual optimizations
        void constantFolding(Program &ast);
        void deadCodeElimination(Program &ast);
        void inlineExpansion(Program &ast);
        void strengthReduction(Program &ast);
        void commonSubexpressionElimination(Program &ast);

        // Statistics
        struct OptimizationStats
        {
            int constantsFolded = 0;
            int deadCodeRemoved = 0;
            int functionsInlined = 0;
            int expressionsSimplified = 0;
        };

        const OptimizationStats &getStats() const { return stats; }

    private:
        OptimizationStats stats;

        // Helper methods
        std::unique_ptr<Expression> foldBinaryExpression(BinaryExpr *expr);
        std::unique_ptr<Expression> foldUnaryExpression(UnaryExpr *expr);
        bool isConstant(Expression *expr);
        int evaluateConstant(Expression *expr);
        bool hasNoSideEffects(Expression *expr);
    };

} // namespace lpp

#endif // OPTIMIZER_H
