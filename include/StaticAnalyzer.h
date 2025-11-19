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
        // FIX BUG #143: Missing severity levels for type errors
        // TODO: Add TYPE_ERROR, TYPE_WARNING for type mismatches
        // - ERROR: Cannot convert string to int (explicit cast required)
        // - Hint: Use std::stoi(str) or explicit cast (int)value
        // - NOTE: Implicit conversion may lose precision (double -> int)
        // FIX BUG #159: No configurable warning levels
        // TODO: Add command-line flags for warning control
        // - -Werror: Treat all warnings as errors
        // - -Wno-unused: Suppress unused variable warnings
        // - -Wall: Enable all warnings
        // - -Wextra: Enable extra pedantic warnings
        // - Requires: WarningConfig class with enabled flags
        WARNING,
        ERROR,
        NOTE,
        INFO // BUG #144: No INFO level for non-critical messages
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
        SIGN_CONVERSION,
        TYPE_MISMATCH, // BUG #146: No TYPE_MISMATCH category
                       // TODO: Show type context in error messages
                       // Example: "Expected 'int' but got 'string'"
                       // - Show where types come from: "Function foo() expects int (line 5)"
                       // - Suggest conversions: "Use std::stoi() or cast with (int)"

        // Paradigm violations
        PARADIGM_MUTATION_IN_FUNCTIONAL,
        PARADIGM_CLASS_IN_FUNCTIONAL,
        PARADIGM_CLASS_IN_IMPERATIVE,
        PARADIGM_GOLF_DISCOURAGED,
        PARADIGM_GOLF_ENCOURAGED,

        // Control flow (BUG #150: Missing categories)
        CONTROL_FLOW_ERROR, // break/continue outside loop
        INTERNAL_ERROR      // Compiler internal errors
    };

    // Analysis issue
    struct AnalysisIssue
    {
        // FIX BUG #145: No fix-it hints for common issues
        // TODO: Add fixItHint field with suggested code change
        // Example: "Unused variable 'x' at line 10"
        // - fixItHint: "Prefix with underscore: '_x' or remove"
        // - fixItCode: "let _x = 42;" (show corrected version)
        IssueType type;
        Severity severity;
        std::string message;
        int line;
        int column;
        std::string function;
        std::vector<std::string> notes;
        std::string fixItHint; // BUG #145: Suggested fix
    };

    // Variable state in symbolic execution
    struct SymbolicValue
    {
        // FIX BUG #164: No tracking for moved-from state (use-after-move)
        // TODO: Add MOVED_FROM state to prevent use-after-move bugs
        // - State::MOVED_FROM: Variable moved, access is error
        // - Detect: x = move(y); use(y); // ERROR: y is moved-from
        // - Reset: y = newValue; // OK, y is reinitialized
        // - Error: "Use of moved-from variable 'y' at line 10"
        // Example:
        //   enum class State {
        //     UNINITIALIZED, INITIALIZED, MOVED_FROM, UNKNOWN
        //   };
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
        void visit(TupleExpr &node) override;
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
        void visit(QuantumMethodCall &node) override;

        void visit(VarDecl &node) override;
        void visit(QuantumVarDecl &node) override;
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
        std::vector<AnalysisIssue> issues;
        int currentLine = 0;
        std::string currentFunction = "";
        ParadigmMode currentParadigm = ParadigmMode::NONE;

        // Context tracking for break/continue validation
        int loopDepth = 0;
        int switchDepth = 0;

        // Control Flow Graph
        std::vector<std::unique_ptr<CFGNode>> cfg;
        CFGNode *currentBlock = nullptr;
        CFGNode *entryBlock = nullptr;
        CFGNode *exitBlock = nullptr;

        // Symbolic state
        // FIX BUG #183: symbolTable accessed without synchronization\n        // TODO: Add mutex for thread-safe access in parallel analysis
        // - std::mutex symbolTableMutex;
        // - Lock on read/write: std::lock_guard<std::mutex> lock(symbolTableMutex);
        // - Or use concurrent data structure: tbb::concurrent_hash_map
        // - Impact: Safe parallel function analysis
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
        void traverseCFG(CFGNode *node, int depth = 0); // DFS traversal to mark reachable nodes

        // Advanced CFG building for control flow
        CFGNode *buildCFGForStatement(Statement *stmt, CFGNode *breakTarget = nullptr, CFGNode *continueTarget = nullptr);

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

        // Paradigm validation
        void checkParadigmViolation(VarDecl &node);
        void checkParadigmViolation(Assignment &node);
        void checkParadigmViolation(ClassDecl &node);
        void checkParadigmViolation(MapExpr &node);
        void checkParadigmViolation(FilterExpr &node);
        void checkParadigmViolation(ReduceExpr &node);
        void checkParadigmViolation(Function &node);
    };

} // namespace lpp

#endif // STATIC_ANALYZER_H
