#ifndef STATIC_ANALYZER_H
#define STATIC_ANALYZER_H

#include "AST.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <optional>

namespace lpp
{
    // Analysis severity levels
    enum class Severity
    {
        WARNING,
        ERROR,
        NOTE
    };

    // Types of issues detected
    enum class IssueType
    {
        // Memory issues
        USE_AFTER_FREE,
        DOUBLE_FREE,
        MEMORY_LEAK,
        NULL_DEREFERENCE,
        UNINITIALIZED_VALUE,
        BUFFER_OVERFLOW,

        // Logic issues
        DIVISION_BY_ZERO,
        DEAD_CODE,
        INFINITE_LOOP,
        UNREACHABLE_CODE,

        // Data flow issues
        UNUSED_VARIABLE,
        UNINITIALIZED_READ,
        TAINTED_INPUT,

        // Type issues
        INTEGER_OVERFLOW,
        NARROWING_CONVERSION,
        SIGN_CONVERSION
    };

    // Analysis issue
    struct AnalysisIssue
    {
        IssueType type;
        Severity severity;
        std::string message;
        int line;
        int column;
        std::string function;
        std::vector<std::string> notes;
    };

    // Variable state in symbolic execution
    struct SymbolicValue
    {
        enum class State
        {
            UNINITIALIZED,
            INITIALIZED,
            NULL_PTR,
            NON_NULL,
            FREED,
            UNKNOWN
        };

        State state = State::UNKNOWN;
        bool mayBeNull = false;
        bool definitelyNull = false;
        std::optional<int64_t> constantValue;
        bool tainted = false; // For security analysis

        // Comparison operator needed for map equality checks
        bool operator==(const SymbolicValue &other) const
        {
            return state == other.state &&
                   mayBeNull == other.mayBeNull &&
                   definitelyNull == other.definitelyNull &&
                   constantValue == other.constantValue &&
                   tainted == other.tainted;
        }

        bool operator!=(const SymbolicValue &other) const
        {
            return !(*this == other);
        }
    };

    // Control Flow Graph Node
    struct CFGNode
    {
        enum class Type
        {
            ENTRY,
            EXIT,
            STATEMENT,
            BRANCH,
            MERGE,
            LOOP_HEAD,
            LOOP_BACK
        };

        Type type;
        Statement *stmt = nullptr;
        Expression *condition = nullptr;
        std::vector<CFGNode *> successors;
        std::vector<CFGNode *> predecessors;
        int id;

        // Data flow analysis state
        std::map<std::string, SymbolicValue> stateIn;
        std::map<std::string, SymbolicValue> stateOut;
    };

    // Path through the CFG (for path-sensitive analysis)
    struct ExecutionPath
    {
        std::vector<CFGNode *> nodes;
        std::map<std::string, SymbolicValue> currentState;
        bool isFeasible = true;
        std::vector<std::string> constraints;
    };

    class StaticAnalyzer : public ASTVisitor
    {
    public:
        StaticAnalyzer();

        // Run analysis on the AST
        std::vector<AnalysisIssue> analyze(Program &program);

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
        std::vector<AnalysisIssue> issues;
        int currentLine = 0;
        std::string currentFunction = "";

        // Control Flow Graph
        std::vector<std::unique_ptr<CFGNode>> cfg;
        CFGNode *currentBlock = nullptr;
        CFGNode *entryBlock = nullptr;
        CFGNode *exitBlock = nullptr;

        // Symbolic state
        std::map<std::string, SymbolicValue> symbolTable;

        // Memory tracking
        std::set<std::string> allocatedMemory;
        std::set<std::string> freedMemory;

        // Taint tracking (for security analysis)
        std::set<std::string> taintedVariables;

        // Dead code detection
        std::set<CFGNode *> visitedNodes;

        // Helper methods
        void buildCFG(std::vector<std::unique_ptr<Statement>> &statements);
        CFGNode *createNode(CFGNode::Type type);
        void connectNodes(CFGNode *from, CFGNode *to);

        // Data flow analysis
        void runDataFlowAnalysis();
        bool transferFunction(CFGNode *node);
        void mergeStates(CFGNode *node);

        // Path-sensitive analysis
        void exploreAllPaths(CFGNode *start, ExecutionPath &path);
        bool evaluateCondition(Expression *expr, ExecutionPath &path);

        // Issue detection
        void checkNullDereference(IdentifierExpr &node);
        void checkDivisionByZero(BinaryExpr &node);
        void checkBufferOverflow(IndexExpr &node);
        void checkUninitializedRead(IdentifierExpr &node);
        void checkMemoryLeak();
        void checkDeadCode();
        void checkInfiniteLoop(WhileStmt &node);
        void checkIntegerOverflow(BinaryExpr &node);
        void checkTaintedData(Expression &node);

        // Symbolic execution helpers
        SymbolicValue evaluateExpression(Expression *expr);
        bool canBeZero(Expression *expr);
        bool canBeNull(Expression *expr);
        bool isDefinitelyInitialized(const std::string &var);

        // Reporting
        void reportIssue(IssueType type, Severity severity,
                         const std::string &message,
                         const std::vector<std::string> &notes = {});
    };

} // namespace lpp

#endif // STATIC_ANALYZER_H
