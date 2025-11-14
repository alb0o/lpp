#include "AST.h"

namespace lpp
{

    // Expression implementations
    void NumberExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void StringExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void TemplateLiteralExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void BoolExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void IdentifierExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void BinaryExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void UnaryExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void CallExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void LambdaExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void TernaryIfExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void PipelineExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void CompositionExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void RangeExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void ArrayExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void ListComprehension::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void SpreadExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void IndexExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void ObjectExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void MatchExpr::accept(ASTVisitor &visitor) { visitor.visit(*this); }

    // Statement implementations
    void VarDecl::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void Assignment::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void IfStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void WhileStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void ReturnStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void ExprStmt::accept(ASTVisitor &visitor) { visitor.visit(*this); }

    // Function, Class, and Program implementations
    void Function::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void ClassDecl::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void InterfaceDecl::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void TypeDecl::accept(ASTVisitor &visitor) { visitor.visit(*this); }
    void Program::accept(ASTVisitor &visitor) { visitor.visit(*this); }

} // namespace lpp
