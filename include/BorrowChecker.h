#ifndef BORROW_CHECKER_H
#define BORROW_CHECKER_H

#include "AST.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>

namespace lpp
{
    // Ownership states
    enum class Ownership
    {
        OWNED,        // Variable owns the data
        BORROWED,     // Immutable borrow (&T)
        MUT_BORROWED, // Mutable borrow (&mut T)
        MOVED         // Ownership transferred
    };

    // Lifetime annotation
    struct Lifetime
    {
        std::string name;      // 'a, 'b, etc.
        int scopeLevel;        // Block depth
        bool isStatic = false; // 'static lifetime
    };

    // Variable information
    struct VarInfo
    {
        std::string name;
        Ownership ownership;
        std::vector<Lifetime> lifetimes;
        int declarationLine;
        int lastUseLine;
        bool isMutable;
        std::set<std::string> borrowedBy; // Who borrows this
    };

    // Borrow checker errors
    struct BorrowError
    {
        enum class Type
        {
            USE_AFTER_MOVE,
            DOUBLE_BORROW_MUT,
            BORROW_AFTER_MOVE,
            DANGLING_REFERENCE,
            LIFETIME_MISMATCH,
            IMMUTABLE_ASSIGN
        };

        Type type;
        std::string variable;
        int line;
        int column;
        std::string message;
    };

    class BorrowChecker : public ASTVisitor
    {
    public:
        BorrowChecker();

        // Run borrow checking on the AST
        std::vector<BorrowError> check(Program &program);

        // AST Visitor methods
        void visit(NumberExpr &node) override;
        void visit(StringExpr &node) override;
        void visit(TemplateLiteralExpr &node) override;
        void visit(BoolExpr &node) override;
        void visit(IdentifierExpr &node) override;
        void visit(BinaryExpr &node) override;
        void visit(UnaryExpr &node) override;
        void visit(CallExpr &node) override;
        void visit(LambdaExpr &node) override;
        void visit(TernaryIfExpr &node) override;
        void visit(PipelineExpr &node) override;
        void visit(CompositionExpr &node) override;
        void visit(RangeExpr &node) override;
        void visit(ArrayExpr &node) override;
        void visit(ListComprehension &node) override;
        void visit(SpreadExpr &node) override;
        void visit(IndexExpr &node) override;
        void visit(ObjectExpr &node) override;
        void visit(MatchExpr &node) override;

        void visit(VarDecl &node) override;
        void visit(Assignment &node) override;
        void visit(IfStmt &node) override;
        void visit(WhileStmt &node) override;
        void visit(ReturnStmt &node) override;
        void visit(ExprStmt &node) override;

        void visit(Function &node) override;
        void visit(ClassDecl &node) override;
        void visit(InterfaceDecl &node) override;
        void visit(TypeDecl &node) override;
        void visit(Program &node) override;

    private:
        // Symbol table per scope
        std::vector<std::map<std::string, VarInfo>> scopes;
        std::vector<BorrowError> errors;
        int currentLine = 0;
        int currentColumn = 0;
        int scopeLevel = 0;

        // Helper methods
        void enterScope();
        void exitScope();
        VarInfo *findVariable(const std::string &name);
        void declareVariable(const std::string &name, bool isMutable);
        void useVariable(const std::string &name);
        void moveVariable(const std::string &name);
        void borrowVariable(const std::string &name, bool mutable_borrow);
        void checkLifetimes();
        void reportError(BorrowError::Type type, const std::string &var, const std::string &msg);
    };

} // namespace lpp

#endif // BORROW_CHECKER_H
