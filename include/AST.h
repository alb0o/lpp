#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

namespace lpp
{

    // Paradigm modes for file-level enforcement
    enum class ParadigmMode
    {
        HYBRID,     // Default: all features allowed
        FUNCTIONAL, // Immutability, pure functions, no classes
        IMPERATIVE, // Performance-oriented, explicit control flow
        OOP,        // Object-oriented with classes and inheritance
        GOLFED,     // Code golf: minimal syntax, shortest possible code
        NONE        // No pragma specified (error state)
    };

    // Forward declarations
    class ASTVisitor;

    // Base AST Node
    class ASTNode
    {
    public:
        // FIX BUG #160: AST destructors don't validate resource cleanup
        // TODO: Add validation hooks in destructor
        // - Track active resources in constructor (files, memory, handles)
        // - Virtual cleanup() method called before ~ASTNode()
        // - Assert all resources released: assert(openFiles.empty())
        // - Use RAII wrappers: std::unique_ptr, file_handle (custom RAII)
        // Example:
        //   class ASTNode {
        //     std::vector<std::unique_ptr<Resource>> resources;
        //     virtual void cleanup() { /* subclass-specific */ }
        //     virtual ~ASTNode() { cleanup(); assert(resources.empty()); }
        //   };
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

        // FIX BUG #168: Constructor takes by-value then moves (double-move inefficiency)
        // TODO: Change to rvalue references for true zero-copy
        // - Constructor: TemplateLiteralExpr(std::vector<std::string>&& strs, ...)
        // - Direct init: : strings(strs), interpolations(interp) {} // No std::move needed
        // - Caller must: TemplateLiteralExpr(std::move(myStrings), ...)
        // - Benefit: One move instead of two (parameter + member init)
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

    // Postfix expression: x++ or x--
    class PostfixExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> operand;
        std::string op; // "++" or "--"

        PostfixExpr(std::unique_ptr<Expression> operand, const std::string &o)
            : operand(std::move(operand)), op(o) {}
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

    // Iterate-while operator: start !! predicate $ stepFn
    // Generates array by iterating while condition is true
    class IterateWhileExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> start;
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Expression> stepFn;

        IterateWhileExpr(std::unique_ptr<Expression> s, std::unique_ptr<Expression> cond, std::unique_ptr<Expression> step)
            : start(std::move(s)), condition(std::move(cond)), stepFn(std::move(step)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Auto-iterate operators: start !!< limit or start !!> limit
    // Auto-increment/decrement until limit is reached
    class AutoIterateExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> start;
        std::unique_ptr<Expression> limit;
        bool isIncrement; // true for !!<, false for !!>

        AutoIterateExpr(std::unique_ptr<Expression> s, std::unique_ptr<Expression> lim, bool inc)
            : start(std::move(s)), limit(std::move(lim)), isIncrement(inc) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Iterate-step operator: start ~> stepFn !! condition
    // Alternative syntax for iterate-while
    class IterateStepExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> start;
        std::unique_ptr<Expression> stepFn;
        std::unique_ptr<Expression> condition;

        IterateStepExpr(std::unique_ptr<Expression> s, std::unique_ptr<Expression> step, std::unique_ptr<Expression> cond)
            : start(std::move(s)), stepFn(std::move(step)), condition(std::move(cond)) {}
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

    // Tuple literal: (1, 2, 3) or ("hello", 42, true)
    class TupleExpr : public Expression
    {
    public:
        std::vector<std::unique_ptr<Expression>> elements;

        explicit TupleExpr(std::vector<std::unique_ptr<Expression>> elems)
            : elements(std::move(elems)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // List comprehension: [x * 2 | x in range(10), x > 5]
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

    // Cast expression: x as int
    class CastExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> expression;
        std::string targetType;

        CastExpr(std::unique_ptr<Expression> expr, const std::string &type)
            : expression(std::move(expr)), targetType(type) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Await expression: await promise
    class AwaitExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> expression;

        explicit AwaitExpr(std::unique_ptr<Expression> expr)
            : expression(std::move(expr)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Throw expression: throw error
    class ThrowExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> expression;

        explicit ThrowExpr(std::unique_ptr<Expression> expr)
            : expression(std::move(expr)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Yield expression for generators
    class YieldExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> value;

        explicit YieldExpr(std::unique_ptr<Expression> val = nullptr)
            : value(std::move(val)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Type guard expressions
    class TypeOfExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> expr;

        explicit TypeOfExpr(std::unique_ptr<Expression> e)
            : expr(std::move(e)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class InstanceOfExpr : public Expression
    {
    public:
        std::unique_ptr<Expression> expr;
        std::string typeName;

        InstanceOfExpr(std::unique_ptr<Expression> e, std::string tn)
            : expr(std::move(e)), typeName(std::move(tn)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Quantum method call: x.observe(), x.map(fn), x.reset(), entangle(a, fn)
    class QuantumMethodCall : public Expression
    {
    public:
        std::string quantumVar;                        // variable name
        std::string method;                            // observe, map, reset, entangle
        std::vector<std::unique_ptr<Expression>> args; // arguments for map/entangle

        QuantumMethodCall(const std::string &var, const std::string &meth)
            : quantumVar(var), method(meth) {}

        QuantumMethodCall(const std::string &var, const std::string &meth,
                          std::vector<std::unique_ptr<Expression>> arguments)
            : quantumVar(var), method(meth), args(std::move(arguments)) {}

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
        bool isArrayType = false;            // true for int[], string[], etc.
        int arraySize = -1;                  // -1 for dynamic, > 0 for fixed size
        bool isNullable = false;             // true for int?, string?
        std::vector<std::string> unionTypes; // for union types: int | string

        VarDecl(const std::string &n, const std::string &t, std::unique_ptr<Expression> init)
            : name(n), type(t), initializer(std::move(init)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Quantum variable declaration: quantum let x = [states]
    class QuantumVarDecl : public Statement
    {
    public:
        std::string name;
        std::string type;                                // element type
        std::vector<std::unique_ptr<Expression>> states; // superposition states
        std::vector<double> probabilities;               // optional weighted probabilities
        bool hasWeights = false;

        QuantumVarDecl(const std::string &n, const std::string &t,
                       std::vector<std::unique_ptr<Expression>> st)
            : name(n), type(t), states(std::move(st)) {}

        QuantumVarDecl(const std::string &n, const std::string &t,
                       std::vector<std::unique_ptr<Expression>> st,
                       std::vector<double> probs)
            : name(n), type(t), states(std::move(st)), probabilities(std::move(probs)), hasWeights(true) {}

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

    class BreakStmt : public Statement
    {
    public:
        BreakStmt() {}
        void accept(ASTVisitor &visitor) override;
    };

    class ContinueStmt : public Statement
    {
    public:
        ContinueStmt() {}
        void accept(ASTVisitor &visitor) override;
    };

    // Switch case clause
    struct CaseClause
    {
        std::unique_ptr<Expression> value; // nullptr for default case
        std::unique_ptr<Expression> guard; // when condition (optional)
        std::vector<std::unique_ptr<Statement>> statements;
        bool isDefault;

        CaseClause(std::unique_ptr<Expression> val, std::vector<std::unique_ptr<Statement>> stmts, bool isDef = false, std::unique_ptr<Expression> grd = nullptr)
            : value(std::move(val)), statements(std::move(stmts)), isDefault(isDef), guard(std::move(grd)) {}
    };

    class SwitchStmt : public Statement
    {
    public:
        std::unique_ptr<Expression> condition;
        std::vector<CaseClause> cases;

        SwitchStmt(std::unique_ptr<Expression> cond, std::vector<CaseClause> cs)
            : condition(std::move(cond)), cases(std::move(cs)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // For loop: for (init; cond; update) { body }
    class ForStmt : public Statement
    {
    public:
        std::unique_ptr<Statement> initializer;
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Expression> increment;
        std::vector<std::unique_ptr<Statement>> body;

        ForStmt(std::unique_ptr<Statement> init,
                std::unique_ptr<Expression> cond,
                std::unique_ptr<Expression> inc,
                std::vector<std::unique_ptr<Statement>> b)
            : initializer(std::move(init)), condition(std::move(cond)),
              increment(std::move(inc)), body(std::move(b)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // For-in loop: for (var in array) { body }
    class ForInStmt : public Statement
    {
    public:
        std::string variable;
        std::unique_ptr<Expression> iterable;
        std::vector<std::unique_ptr<Statement>> body;

        ForInStmt(const std::string &var,
                  std::unique_ptr<Expression> iter,
                  std::vector<std::unique_ptr<Statement>> b)
            : variable(var), iterable(std::move(iter)), body(std::move(b)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Do-while loop: do { body } while (cond);
    class DoWhileStmt : public Statement
    {
    public:
        std::vector<std::unique_ptr<Statement>> body;
        std::unique_ptr<Expression> condition;

        DoWhileStmt(std::vector<std::unique_ptr<Statement>> b,
                    std::unique_ptr<Expression> cond)
            : body(std::move(b)), condition(std::move(cond)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Try-catch-finally: try { } catch (e) { } finally { }
    class TryCatchStmt : public Statement
    {
    public:
        std::vector<std::unique_ptr<Statement>> tryBlock;
        std::string catchVariable;
        std::vector<std::unique_ptr<Statement>> catchBlock;
        std::vector<std::unique_ptr<Statement>> finallyBlock;

        TryCatchStmt(std::vector<std::unique_ptr<Statement>> tryB,
                     const std::string &catchVar,
                     std::vector<std::unique_ptr<Statement>> catchB,
                     std::vector<std::unique_ptr<Statement>> finallyB = {})
            : tryBlock(std::move(tryB)), catchVariable(catchVar),
              catchBlock(std::move(catchB)), finallyBlock(std::move(finallyB)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Destructuring assignment: let [a, b] = arr or let {x, y} = obj or let (a, b) = tuple
    class DestructuringStmt : public Statement
    {
    public:
        std::vector<std::string> targets; // variable names
        std::unique_ptr<Expression> source;
        bool isArray;         // true for array, false for object
        bool isTuple = false; // true for tuple

        DestructuringStmt(std::vector<std::string> tgts,
                          std::unique_ptr<Expression> src,
                          bool arr,
                          bool tup = false)
            : targets(std::move(tgts)), source(std::move(src)), isArray(arr), isTuple(tup) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Enum declaration: enum Color { Red, Green, Blue }
    class EnumDecl : public Statement
    {
    public:
        std::string name;
        std::vector<std::pair<std::string, int>> values; // (name, value)

        EnumDecl(const std::string &n, std::vector<std::pair<std::string, int>> vals)
            : name(n), values(std::move(vals)) {}
        void accept(ASTVisitor &visitor) override;
    };

    class ReturnStmt : public Statement
    {
    public:
        std::unique_ptr<Expression> value;

        explicit ReturnStmt(std::unique_ptr<Expression> val) : value(std::move(val)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Import statement: import { foo, bar } from "module"
    class ImportStmt : public Statement
    {
    public:
        std::vector<std::string> imports; // names to import
        std::string module;               // module path
        bool importAll = false;           // true for import * as name

        ImportStmt(std::vector<std::string> imps, const std::string &mod, bool all = false)
            : imports(std::move(imps)), module(mod), importAll(all) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Export statement: export fn foo() { }
    class ExportStmt : public Statement
    {
    public:
        std::unique_ptr<Statement> declaration; // function, class, etc.

        explicit ExportStmt(std::unique_ptr<Statement> decl) : declaration(std::move(decl)) {}
        void accept(ASTVisitor &visitor) override;
    };

    // Auto-generated design pattern statement
    class AutoPatternStmt : public Statement
    {
    public:
        std::string problemType; // e.g., "Configuration", "Logging", "DataAccess"
        std::string className;
        std::string patternType; // auto-detected: Singleton, Factory, Observer, etc.

        AutoPatternStmt(const std::string &problem, const std::string &name)
            : problemType(problem), className(name) {}
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
        bool hasRestParam = false;              // true if last param is ...rest
        std::string restParamName = "";         // name of rest parameter
        bool isAsync = false;                   // true for async functions
        bool isGenerator = false;               // true for generator functions (uses yield)
        bool isPrototype = false;               // true for forward declarations
        bool isGetter = false;                  // true for getter methods
        bool isSetter = false;                  // true for setter methods
        std::vector<std::string> genericParams; // for generics: <T, U>

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
        std::string designPattern = ""; // for @pattern: Singleton, Factory, Observer, etc.

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
        ParadigmMode paradigm;
        std::vector<std::unique_ptr<Statement>> imports;
        std::vector<std::unique_ptr<Statement>> exports;
        std::vector<std::unique_ptr<Function>> functions;
        std::vector<std::unique_ptr<ClassDecl>> classes;
        std::vector<std::unique_ptr<InterfaceDecl>> interfaces;
        std::vector<std::unique_ptr<TypeDecl>> types;
        std::vector<std::unique_ptr<Statement>> enums;

        Program(ParadigmMode pm,
                std::vector<std::unique_ptr<Function>> funcs,
                std::vector<std::unique_ptr<ClassDecl>> cls = {},
                std::vector<std::unique_ptr<InterfaceDecl>> intfs = {},
                std::vector<std::unique_ptr<TypeDecl>> tps = {},
                std::vector<std::unique_ptr<Statement>> enms = {},
                std::vector<std::unique_ptr<Statement>> imps = {},
                std::vector<std::unique_ptr<Statement>> exps = {})
            : paradigm(pm), imports(std::move(imps)), exports(std::move(exps)),
              functions(std::move(funcs)), classes(std::move(cls)),
              interfaces(std::move(intfs)), types(std::move(tps)), enums(std::move(enms)) {}
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
        virtual void visit(PostfixExpr &node) = 0;
        virtual void visit(CallExpr &node) = 0;
        virtual void visit(LambdaExpr &node) = 0;
        virtual void visit(TernaryIfExpr &node) = 0;
        virtual void visit(PipelineExpr &node) = 0;
        virtual void visit(CompositionExpr &node) = 0;
        virtual void visit(RangeExpr &node) = 0;
        virtual void visit(MapExpr &node) = 0;
        virtual void visit(FilterExpr &node) = 0;
        virtual void visit(ReduceExpr &node) = 0;
        virtual void visit(IterateWhileExpr &node) = 0;
        virtual void visit(AutoIterateExpr &node) = 0;
        virtual void visit(IterateStepExpr &node) = 0;
        virtual void visit(ArrayExpr &node) = 0;
        virtual void visit(TupleExpr &node) = 0;
        virtual void visit(ListComprehension &node) = 0;
        virtual void visit(SpreadExpr &node) = 0;
        virtual void visit(IndexExpr &node) = 0;
        virtual void visit(ObjectExpr &node) = 0;
        virtual void visit(MatchExpr &node) = 0;
        virtual void visit(CastExpr &node) = 0;
        virtual void visit(AwaitExpr &node) = 0;
        virtual void visit(ThrowExpr &node) = 0;
        virtual void visit(YieldExpr &node) = 0;
        virtual void visit(TypeOfExpr &node) = 0;
        virtual void visit(InstanceOfExpr &node) = 0;
        virtual void visit(QuantumMethodCall &node) = 0;

        virtual void visit(VarDecl &node) = 0;
        virtual void visit(QuantumVarDecl &node) = 0;
        virtual void visit(Assignment &node) = 0;
        virtual void visit(IfStmt &node) = 0;
        virtual void visit(WhileStmt &node) = 0;
        virtual void visit(SwitchStmt &node) = 0;
        virtual void visit(ForStmt &node) = 0;
        virtual void visit(ForInStmt &node) = 0;
        virtual void visit(DoWhileStmt &node) = 0;
        virtual void visit(TryCatchStmt &node) = 0;
        virtual void visit(DestructuringStmt &node) = 0;
        virtual void visit(EnumDecl &node) = 0;
        virtual void visit(BreakStmt &node) = 0;
        virtual void visit(ContinueStmt &node) = 0;
        virtual void visit(ReturnStmt &node) = 0;
        virtual void visit(ImportStmt &node) = 0;
        virtual void visit(ExportStmt &node) = 0;
        virtual void visit(AutoPatternStmt &node) = 0;
        virtual void visit(ExprStmt &node) = 0;

        virtual void visit(Function &node) = 0;
        virtual void visit(ClassDecl &node) = 0;
        virtual void visit(InterfaceDecl &node) = 0;
        virtual void visit(TypeDecl &node) = 0;
        virtual void visit(Program &node) = 0;
    };

} // namespace lpp

#endif // AST_H
