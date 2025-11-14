#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

namespace lpp
{

    // Forward declarations
    class ASTVisitor;

    // Base AST Node
    class ASTNode
    {
    public:
        virtual ~ASTNode() = default;
        virtual void accept(ASTVisitor &visitor) = 0;
    };

    // Expressions
    class Expression : public ASTNode
    {
    };

    class NumberExpr : public Expression
    {
    public:
        double value;
        explicit NumberExpr(double val) : value(val) {}
        void accept(ASTVisitor &visitor) override;
    };

    class StringExpr : public Expression
    {
    public:
        std::string value;
        explicit StringExpr(const std::string &val) : value(val) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Template literal: `Hello ${name}`
    class TemplateLiteralExpr : public Expression
    {
    public:
        std::vector<std::string> strings;                        // static parts
        std::vector<std::unique_ptr<Expression>> interpolations; // ${expr}

        TemplateLiteralExpr(std::vector<std::string> strs,
                            std::vector<std::unique_ptr<Expression>> interp)
            : strings(std::move(strs)), interpolations(std::move(interp)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class BoolExpr : public Expression
    {
    public:
        bool value;
        explicit BoolExpr(bool val) : value(val) {}
        void accept(ASTVisitor &visitor) override;
    };

    class IdentifierExpr : public Expression
    {
    public:
        std::string name;
        explicit IdentifierExpr(const std::string &n) : name(n) {}
        void accept(ASTVisitor &visitor) override;
    };

    class BinaryExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> left;
        std::string op;
        std::unique_ptr<Expression> right;

        BinaryExpr(std::unique_ptr<Expression> l, const std::string &o, std::unique_ptr<Expression> r)
            : left(std::move(l)), op(o), right(std::move(r)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class UnaryExpr : public Expression
    {
    public:
        std::string op;
        std::unique_ptr<Expression> operand;

        UnaryExpr(const std::string &o, std::unique_ptr<Expression> operand)
            : op(o), operand(std::move(operand)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Range expression: 0~5
    class RangeExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> start;
        std::unique_ptr<Expression> end;
        std::unique_ptr<Expression> step; // optional, can be nullptr

        RangeExpr(std::unique_ptr<Expression> s, std::unique_ptr<Expression> e, std::unique_ptr<Expression> st = nullptr)
            : start(std::move(s)), end(std::move(e)), step(std::move(st)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Map/forEach operator: arr @ fn
    class MapExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> iterable;
        std::unique_ptr<Expression> fn;

        MapExpr(std::unique_ptr<Expression> iter, std::unique_ptr<Expression> f)
            : iterable(std::move(iter)), fn(std::move(f)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Filter operator: arr ? |x| condition
    class FilterExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> iterable;
        std::unique_ptr<Expression> predicate;

        FilterExpr(std::unique_ptr<Expression> iter, std::unique_ptr<Expression> pred)
            : iterable(std::move(iter)), predicate(std::move(pred)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Reduce/fold operator: arr \ |acc, x| expr
    class ReduceExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> iterable;
        std::unique_ptr<Expression> fn;
        std::unique_ptr<Expression> initial; // optional

        ReduceExpr(std::unique_ptr<Expression> iter, std::unique_ptr<Expression> f, std::unique_ptr<Expression> init = nullptr)
            : iterable(std::move(iter)), fn(std::move(f)), initial(std::move(init)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class CallExpr : public Expression
    {
    public:
        std::string function;
        std::vector<std::unique_ptr<Expression>> arguments;

        CallExpr(const std::string &fn, std::vector<std::unique_ptr<Expression>> args)
            : function(fn), arguments(std::move(args)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Lambda expression: x -> expr or (a, b) -> expr
    class LambdaExpr : public Expression
    {
    public:
        std::vector<std::pair<std::string, std::string>> parameters; // (name, type) - type can be empty
        std::unique_ptr<Expression> body;
        std::string returnType;         // opzionale
        bool hasRestParam = false;      // true if last param is ...rest
        std::string restParamName = ""; // name of rest parameter

        LambdaExpr(std::vector<std::pair<std::string, std::string>> params,
                   std::unique_ptr<Expression> b,
                   const std::string &retType = "",
                   bool rest = false,
                   const std::string &restName = "")
            : parameters(std::move(params)), body(std::move(b)), returnType(retType),
              hasRestParam(rest), restParamName(restName) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Ternary if: ?cond -> thenExpr $ elseExpr
    class TernaryIfExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Expression> thenExpr;
        std::unique_ptr<Expression> elseExpr;

        TernaryIfExpr(std::unique_ptr<Expression> cond,
                      std::unique_ptr<Expression> thenE,
                      std::unique_ptr<Expression> elseE)
            : condition(std::move(cond)), thenExpr(std::move(thenE)), elseExpr(std::move(elseE)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Pipeline: expr |> fn |> fn
    class PipelineExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> initial;
        std::vector<std::unique_ptr<Expression>> stages;

        PipelineExpr(std::unique_ptr<Expression> init, std::vector<std::unique_ptr<Expression>> stgs)
            : initial(std::move(init)), stages(std::move(stgs)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Function composition: f . g . h
    class CompositionExpr : public Expression
    {
    public:
        std::vector<std::unique_ptr<Expression>> functions;

        explicit CompositionExpr(std::vector<std::unique_ptr<Expression>> funcs)
            : functions(std::move(funcs)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Array literal: [1, 2, 3]
    class ArrayExpr : public Expression
    {
    public:
        std::vector<std::unique_ptr<Expression>> elements;

        explicit ArrayExpr(std::vector<std::unique_ptr<Expression>> elems)
            : elements(std::move(elems)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // List comprehension: [expr | var in range, predicates]
    class ListComprehension : public Expression
    {
    public:
        std::unique_ptr<Expression> expression;
        std::string variable;
        std::unique_ptr<Expression> range;
        std::vector<std::unique_ptr<Expression>> predicates;

        ListComprehension(std::unique_ptr<Expression> expr,
                          const std::string &var,
                          std::unique_ptr<Expression> rng,
                          std::vector<std::unique_ptr<Expression>> preds)
            : expression(std::move(expr)), variable(var), range(std::move(rng)), predicates(std::move(preds)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Spread expression: ...array
    class SpreadExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> expression;

        explicit SpreadExpr(std::unique_ptr<Expression> expr)
            : expression(std::move(expr)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Index/member access: arr[index] or obj.property
    class IndexExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> object;
        std::unique_ptr<Expression> index;
        bool isDot;      // true for obj.prop, false for arr[index]
        bool isOptional; // true for obj?.prop (optional chaining)

        IndexExpr(std::unique_ptr<Expression> obj, std::unique_ptr<Expression> idx, bool dot = false, bool optional = false)
            : object(std::move(obj)), index(std::move(idx)), isDot(dot), isOptional(optional) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Object literal: {name: "Alice", age: 30}
    class ObjectExpr : public Expression
    {
    public:
        std::vector<std::pair<std::string, std::unique_ptr<Expression>>> properties; // (key, value)

        explicit ObjectExpr(std::vector<std::pair<std::string, std::unique_ptr<Expression>>> props)
            : properties(std::move(props)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Pattern matching: match expr { case pattern -> result }
    class MatchExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> expression;
        std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> cases; // (pattern, result)

        MatchExpr(std::unique_ptr<Expression> expr,
                  std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> cs)
            : expression(std::move(expr)), cases(std::move(cs)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Statements
    class Statement : public ASTNode
    {
    };

    class VarDecl : public Statement
    {
    public:
        std::string name;
        std::string type;
        std::unique_ptr<Expression> initializer;

        VarDecl(const std::string &n, const std::string &t, std::unique_ptr<Expression> init)
            : name(n), type(t), initializer(std::move(init)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class Assignment : public Statement
    {
    public:
        std::string name;
        std::unique_ptr<Expression> value;

        Assignment(const std::string &n, std::unique_ptr<Expression> val)
            : name(n), value(std::move(val)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class IfStmt : public Statement
    {
    public:
        std::unique_ptr<Expression> condition;
        std::vector<std::unique_ptr<Statement>> thenBranch;
        std::vector<std::unique_ptr<Statement>> elseBranch;

        IfStmt(std::unique_ptr<Expression> cond,
               std::vector<std::unique_ptr<Statement>> thenB,
               std::vector<std::unique_ptr<Statement>> elseB)
            : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class WhileStmt : public Statement
    {
    public:
        std::unique_ptr<Expression> condition;
        std::vector<std::unique_ptr<Statement>> body;

        WhileStmt(std::unique_ptr<Expression> cond, std::vector<std::unique_ptr<Statement>> b)
            : condition(std::move(cond)), body(std::move(b)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class ReturnStmt : public Statement
    {
    public:
        std::unique_ptr<Expression> value;

        explicit ReturnStmt(std::unique_ptr<Expression> val) : value(std::move(val)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class ExprStmt : public Statement
    {
    public:
        std::unique_ptr<Expression> expression;

        explicit ExprStmt(std::unique_ptr<Expression> expr) : expression(std::move(expr)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class Function : public ASTNode
    {
    public:
        std::string name;
        std::vector<std::pair<std::string, std::string>> parameters; // (name, type)
        std::string returnType;
        std::vector<std::unique_ptr<Statement>> body;
        bool hasRestParam = false;      // true if last param is ...rest
        std::string restParamName = ""; // name of rest parameter

        Function(const std::string &n,
                 std::vector<std::pair<std::string, std::string>> params,
                 const std::string &retType,
                 std::vector<std::unique_ptr<Statement>> b,
                 bool rest = false,
                 const std::string &restName = "")
            : name(n), parameters(std::move(params)), returnType(retType), body(std::move(b)),
              hasRestParam(rest), restParamName(restName) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Class declaration
    class ClassDecl : public ASTNode
    {
    public:
        std::string name;
        std::string baseClass;                                       // for inheritance (optional)
        std::vector<std::pair<std::string, std::string>> properties; // (name, type)
        std::vector<std::unique_ptr<Function>> methods;
        std::unique_ptr<Function> constructor;

        ClassDecl(const std::string &n,
                  const std::string &base,
                  std::vector<std::pair<std::string, std::string>> props,
                  std::vector<std::unique_ptr<Function>> meths,
                  std::unique_ptr<Function> ctor = nullptr)
            : name(n), baseClass(base), properties(std::move(props)),
              methods(std::move(meths)), constructor(std::move(ctor)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Interface declaration
    class InterfaceDecl : public ASTNode
    {
    public:
        std::string name;
        std::vector<std::pair<std::string, std::string>> methods; // (name, signature)

        InterfaceDecl(const std::string &n,
                      std::vector<std::pair<std::string, std::string>> meths)
            : name(n), methods(std::move(meths)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Type alias / ADT: type Option<T> = Some(T) | None
    class TypeDecl : public ASTNode
    {
    public:
        std::string name;
        std::vector<std::string> typeParams;                                    // generics
        std::vector<std::pair<std::string, std::vector<std::string>>> variants; // (constructor, fields)

        TypeDecl(const std::string &n,
                 std::vector<std::string> params,
                 std::vector<std::pair<std::string, std::vector<std::string>>> vars)
            : name(n), typeParams(std::move(params)), variants(std::move(vars)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class Program : public ASTNode
    {
    public:
        std::vector<std::unique_ptr<Function>> functions;
        std::vector<std::unique_ptr<ClassDecl>> classes;
        std::vector<std::unique_ptr<InterfaceDecl>> interfaces;
        std::vector<std::unique_ptr<TypeDecl>> types;

        Program(std::vector<std::unique_ptr<Function>> funcs,
                std::vector<std::unique_ptr<ClassDecl>> cls = {},
                std::vector<std::unique_ptr<InterfaceDecl>> intfs = {},
                std::vector<std::unique_ptr<TypeDecl>> tps = {})
            : functions(std::move(funcs)), classes(std::move(cls)),
              interfaces(std::move(intfs)), types(std::move(tps)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Visitor pattern for traversing AST
    class ASTVisitor
    {
    public:
        virtual ~ASTVisitor() = default;

        virtual void visit(NumberExpr &node) = 0;
        virtual void visit(StringExpr &node) = 0;
        virtual void visit(TemplateLiteralExpr &node) = 0;
        virtual void visit(BoolExpr &node) = 0;
        virtual void visit(IdentifierExpr &node) = 0;
        virtual void visit(BinaryExpr &node) = 0;
        virtual void visit(UnaryExpr &node) = 0;
        virtual void visit(CallExpr &node) = 0;
        virtual void visit(LambdaExpr &node) = 0;
        virtual void visit(TernaryIfExpr &node) = 0;
        virtual void visit(PipelineExpr &node) = 0;
        virtual void visit(CompositionExpr &node) = 0;
        virtual void visit(RangeExpr &node) = 0;
        virtual void visit(MapExpr &node) = 0;
        virtual void visit(FilterExpr &node) = 0;
        virtual void visit(ReduceExpr &node) = 0;
        virtual void visit(ArrayExpr &node) = 0;
        virtual void visit(ListComprehension &node) = 0;
        virtual void visit(SpreadExpr &node) = 0;
        virtual void visit(IndexExpr &node) = 0;
        virtual void visit(ObjectExpr &node) = 0;
        virtual void visit(MatchExpr &node) = 0;

        virtual void visit(VarDecl &node) = 0;
        virtual void visit(Assignment &node) = 0;
        virtual void visit(IfStmt &node) = 0;
        virtual void visit(WhileStmt &node) = 0;
        virtual void visit(ReturnStmt &node) = 0;
        virtual void visit(ExprStmt &node) = 0;

        virtual void visit(Function &node) = 0;
        virtual void visit(ClassDecl &node) = 0;
        virtual void visit(InterfaceDecl &node) = 0;
        virtual void visit(TypeDecl &node) = 0;
        virtual void visit(Program &node) = 0;
    };

} // namespace lpp

#endif // AST_H
