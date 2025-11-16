#ifndef TRANSPILER_H
#define TRANSPILER_H

#include "AST.h"
#include <string>
#include <sstream>

namespace lpp
{

    class Transpiler : public ASTVisitor
    {
    public:
        std::string transpile(Program &program);

        void visit(NumberExpr &node) override;
        void visit(StringExpr &node) override;
        void visit(TemplateLiteralExpr &node) override;
        void visit(BoolExpr &node) override;
        void visit(IdentifierExpr &node) override;
        void visit(BinaryExpr &node) override;
        void visit(UnaryExpr &node) override;
        void visit(PostfixExpr &node) override;
        void visit(CallExpr &node) override;
        void visit(LambdaExpr &node) override;
        void visit(TernaryIfExpr &node) override;
        void visit(PipelineExpr &node) override;
        void visit(CompositionExpr &node) override;
        void visit(RangeExpr &node) override;
        void visit(MapExpr &node) override;
        void visit(FilterExpr &node) override;
        void visit(ReduceExpr &node) override;
        void visit(IterateWhileExpr &node) override;
        void visit(AutoIterateExpr &node) override;
        void visit(IterateStepExpr &node) override;
        void visit(ArrayExpr &node) override;
        void visit(ListComprehension &node) override;
        void visit(SpreadExpr &node) override;
        void visit(IndexExpr &node) override;
        void visit(ObjectExpr &node) override;
        void visit(MatchExpr &node) override;
        void visit(CastExpr &node) override;
        void visit(AwaitExpr &node) override;
        void visit(ThrowExpr &node) override;
        void visit(YieldExpr &node) override;
        void visit(TypeOfExpr &node) override;
        void visit(InstanceOfExpr &node) override;

        void visit(VarDecl &node) override;
        void visit(Assignment &node) override;
        void visit(IfStmt &node) override;
        void visit(WhileStmt &node) override;
        void visit(SwitchStmt &node) override;
        void visit(ForStmt &node) override;
        void visit(ForInStmt &node) override;
        void visit(DoWhileStmt &node) override;
        void visit(TryCatchStmt &node) override;
        void visit(DestructuringStmt &node) override;
        void visit(EnumDecl &node) override;
        void visit(BreakStmt &node) override;
        void visit(ContinueStmt &node) override;
        void visit(ReturnStmt &node) override;
        void visit(ImportStmt &node) override;
        void visit(ExportStmt &node) override;
        void visit(AutoPatternStmt &node) override;
        void visit(ExprStmt &node) override;

        void visit(Function &node) override;
        void visit(ClassDecl &node) override;
        void visit(InterfaceDecl &node) override;
        void visit(TypeDecl &node) override;
        void visit(Program &node) override;

    private:
        std::ostringstream output;
        int indentLevel = 0;
        int lambdaCounter = 0; // Per generare nomi univoci per lambda
        int matchCounter = 0;  // Per generare nomi univoci per match

        void indent();
        void writeLine(const std::string &line);
        std::string mapType(const std::string &lppType);
        std::string mapOperator(const std::string &op);
        std::string convertMethodSignature(const std::string &lppSignature);
    };

} // namespace lpp

#endif // TRANSPILER_H
