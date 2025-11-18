#include "Transpiler.h"
#include <iostream>

namespace lpp
{

    std::string Transpiler::transpile(Program &program)
    {
        output.str("");
        output.clear();
        indentLevel = 0;

        // Add standard includes
        writeLine("#include <iostream>");
        writeLine("#include <string>");
        writeLine("#include <cmath>");
        writeLine("#include <vector>");
        writeLine("#include <tuple>");
        writeLine("#include <array>");
        writeLine("#include <optional>");
        writeLine("#include <functional>");
        writeLine("#include <variant>");
        writeLine("#include <map>");
        writeLine("#include <any>");
        writeLine("#include <future>");
        writeLine("#include <random>");
        writeLine("#include <chrono>");
        writeLine("");

        // Quantum Variable class template
        writeLine("// ============ QUANTUM VARIABLE SYSTEM ============");
        writeLine("template<typename T>");
        writeLine("class QuantumVar {");
        writeLine("private:");
        indentLevel++;
        writeLine("std::vector<T> states;");
        writeLine("std::vector<double> probabilities;");
        writeLine("std::optional<T> collapsed;");
        writeLine("bool hasWeights;");
        writeLine("std::mt19937 rng;");
        indentLevel--;
        writeLine("");
        writeLine("public:");
        indentLevel++;
        writeLine("// Constructor for uniform distribution");
        writeLine("QuantumVar(const std::vector<T>& s) ");
        indentLevel++;
        writeLine(": states(s), hasWeights(false), rng(std::chrono::system_clock::now().time_since_epoch().count()) {");
        writeLine("// Equal probability for all states");
        writeLine("if (states.empty()) throw std::runtime_error(\"QuantumVar: cannot create with empty states\");");
        writeLine("double prob = 1.0 / states.size();");
        writeLine("probabilities = std::vector<double>(states.size(), prob);");
        indentLevel--;
        writeLine("}");
        writeLine("");
        writeLine("// Constructor for weighted distribution");
        writeLine("QuantumVar(const std::vector<T>& s, const std::vector<double>& probs)");
        indentLevel++;
        writeLine(": states(s), hasWeights(true), rng(std::chrono::system_clock::now().time_since_epoch().count()) {");
        indentLevel++;
        writeLine("// Normalize probabilities to sum to 1.0");
        writeLine("double sum = 0.0;");
        writeLine("for (double p : probs) sum += p;");
        writeLine("if (sum > 0.0) {");
        indentLevel++;
        writeLine("for (double p : probs) probabilities.push_back(p / sum);");
        indentLevel--;
        writeLine("} else {");
        indentLevel++;
        writeLine("// Fallback to uniform if all probabilities are 0");
        writeLine("if (states.empty()) throw std::runtime_error(\"QuantumVar: cannot create with empty states\");");
        writeLine("double uniformProb = 1.0 / states.size();");
        writeLine("probabilities = std::vector<double>(states.size(), uniformProb);");
        indentLevel--;
        writeLine("}");
        indentLevel--;
        writeLine("}");
        indentLevel--;
        writeLine("");
        writeLine("// observe(): Collapse superposition to single state");
        writeLine("T observe() {");
        indentLevel++;
        // FIX BUG #162: QuantumVar doesn't cleanup RNG state in destructor
        // TODO: Add explicit destructor with RNG cleanup
        // - ~QuantumVar() { rng.discard(1000); /* clear state */ }
        // - Or use unique_ptr<std::mt19937> for automatic cleanup
        // - Track if RNG is seeded: bool rngInitialized = false;
        // - Cleanup: if (rngInitialized) { /* cleanup */ }
        // Impact: Prevents RNG state leaks in long-running programs
        // FIX BUG #132: Multiple observe() calls return same value (idempotent)
        // TODO: Document behavior clearly:
        // - First call: Collapses superposition, caches result in 'collapsed'
        // - Subsequent calls: Return cached 'collapsed' value (deterministic)
        // - To re-observe: Call reset() first to clear 'collapsed'
        // Example:
        //   quantum q = [1, 2, 3];
        //   q.observe(); // Returns 2 (random)
        //   q.observe(); // Returns 2 (cached, not random)
        //   q.reset();   // Clear cache
        //   q.observe(); // Returns 1 (new random collapse)
        // CORRECT: Once collapsed, state is fixed (quantum measurement principle)
        writeLine("if (!collapsed) {");
        indentLevel++;
        writeLine("// Weighted random selection");
        writeLine("std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());");
        writeLine("int idx = dist(rng);");
        writeLine("if (idx >= 0 && idx < static_cast<int>(states.size())) collapsed = states[idx];");
        indentLevel--;
        writeLine("}");
        writeLine("return *collapsed;");
        indentLevel--;
        writeLine("}");
        writeLine("");
        writeLine("// map(): Transform all states (lazy)");
        writeLine("template<typename F>");
        writeLine("auto map(F func) -> QuantumVar<decltype(func(std::declval<T>()))> {");
        indentLevel++;
        writeLine("using U = decltype(func(std::declval<T>()));");
        writeLine("std::vector<U> newStates;");
        writeLine("for (const auto& state : states) {");
        indentLevel++;
        writeLine("newStates.push_back(func(state));");
        indentLevel--;
        writeLine("}");
        writeLine("return QuantumVar<U>(newStates, probabilities);");
        indentLevel--;
        writeLine("}");
        writeLine("");
        writeLine("// reset(): Return to superposition");
        writeLine("void reset() {");
        indentLevel++;
        writeLine("collapsed.reset();");
        indentLevel--;
        writeLine("}");
        writeLine("");
        writeLine("// entangle(): Create correlated quantum variable");
        writeLine("template<typename F>");
        writeLine("auto entangle(F transform) -> QuantumVar<decltype(transform(std::declval<T>()))> {");
        indentLevel++;
        writeLine("using U = decltype(transform(std::declval<T>()));");
        writeLine("// If already collapsed, entangled var uses same index");
        writeLine("std::vector<U> entangledStates;");
        writeLine("for (const auto& state : states) {");
        indentLevel++;
        writeLine("entangledStates.push_back(transform(state));");
        indentLevel--;
        writeLine("}");
        writeLine("auto result = QuantumVar<U>(entangledStates, probabilities);");
        writeLine("if (collapsed) {");
        indentLevel++;
        writeLine("// Force same index selection");
        writeLine("int collapsedIdx = 0;");
        writeLine("for (size_t i = 0; i < states.size(); ++i) {");
        indentLevel++;
        writeLine("if (states[i] == *collapsed) { collapsedIdx = i; break; }");
        indentLevel--;
        writeLine("}");
        writeLine("if (collapsedIdx >= 0 && collapsedIdx < static_cast<int>(entangledStates.size())) result.collapsed = entangledStates[collapsedIdx];");
        indentLevel--;
        writeLine("}");
        writeLine("return result;");
        indentLevel--;
        writeLine("}");
        indentLevel--;
        writeLine("};");
        writeLine("");
        writeLine("// Global entangle function");
        writeLine("template<typename T, typename F>");
        writeLine("auto entangle(QuantumVar<T>& qvar, F transform) {");
        indentLevel++;
        writeLine("return qvar.entangle(transform);");
        indentLevel--;
        writeLine("}");
        writeLine("// ================================================");
        writeLine("");

        // Helper function for print
        writeLine("void print(const std::string& s) {");
        indentLevel++;
        writeLine("std::cout << s << std::endl;");
        indentLevel--;
        writeLine("}");
        writeLine("");
        writeLine("void print(int n) {");
        indentLevel++;
        writeLine("std::cout << n << std::endl;");
        indentLevel--;
        writeLine("}");
        writeLine("");
        writeLine("void print(double n) {");
        indentLevel++;
        writeLine("std::cout << n << std::endl;");
        indentLevel--;
        writeLine("}");
        writeLine("");

        // Higher-order functions
        writeLine("// Higher-order function: map");
        writeLine("template<typename T, typename F>");
        writeLine("auto map(const std::vector<T>& vec, F func) {");
        indentLevel++;
        writeLine("std::vector<decltype(func(vec[0]))> result;");
        writeLine("for (const auto& item : vec) {");
        indentLevel++;
        writeLine("result.push_back(func(item));");
        indentLevel--;
        writeLine("}");
        writeLine("return result;");
        indentLevel--;
        writeLine("}");
        writeLine("");

        writeLine("// Higher-order function: filter");
        writeLine("template<typename T, typename F>");
        writeLine("std::vector<T> filter(const std::vector<T>& vec, F predicate) {");
        indentLevel++;
        writeLine("std::vector<T> result;");
        writeLine("for (const auto& item : vec) {");
        indentLevel++;
        writeLine("if (predicate(item)) {");
        indentLevel++;
        writeLine("result.push_back(item);");
        indentLevel--;
        writeLine("}");
        indentLevel--;
        writeLine("}");
        writeLine("return result;");
        indentLevel--;
        writeLine("}");
        writeLine("");

        writeLine("// Higher-order function: reduce/fold");
        writeLine("template<typename T, typename F>");
        writeLine("T reduce(const std::vector<T>& vec, T init, F func) {");
        indentLevel++;
        writeLine("T result = init;");
        writeLine("for (const auto& item : vec) {");
        indentLevel++;
        writeLine("result = func(result, item);");
        indentLevel--;
        writeLine("}");
        writeLine("return result;");
        indentLevel--;
        writeLine("}");
        writeLine("");

        program.accept(*this);

        return output.str();
    }

    void Transpiler::visit(NumberExpr &node)
    {
        output << node.value;
    }

    void Transpiler::visit(StringExpr &node)
    {
        output << "\"" << node.value << "\"";
    }

    void Transpiler::visit(TemplateLiteralExpr &node)
    {
        // Template literal: `Hello ${name}` => std::string("Hello ") + std::to_string(name)
        // FIX BUG #133: Escape special characters in template string literals
        // TODO: Before outputting node.strings[i], escape:
        // - Backslashes: \ -> \\
        // - Quotes: " -> \"
        // - Newlines: actual \n -> \\n (literal backslash-n)
        // - Tabs: actual \t -> \\t
        // Current risk: Raw strings with \ or " break C++ string literals
        output << "(";
        for (size_t i = 0; i < node.strings.size(); i++)
        {
            if (i > 0)
                output << " + ";

            output << "std::string(\"" << node.strings[i] << "\")";

            if (i < node.interpolations.size())
            {
                output << " + ([&]() { auto __val = ";
                node.interpolations[i]->accept(*this);
                output << "; if constexpr (std::is_arithmetic_v<decltype(__val)>) return std::to_string(__val); else return std::string(__val); })()";
            }
        }
        output << ")";
    }

    void Transpiler::visit(BoolExpr &node)
    {
        output << (node.value ? "true" : "false");
    }

    void Transpiler::visit(IdentifierExpr &node)
    {
        output << node.name;
    }

    void Transpiler::visit(BinaryExpr &node)
    {
        // Nullish coalescing: a ?? b => ([&]() { auto __tmp = a; return __tmp != nullptr ? __tmp : b; })()
        if (node.op == "??")
        {
            output << "([&]() { auto __tmp = ";
            node.left->accept(*this);
            output << "; if constexpr (std::is_pointer_v<decltype(__tmp)>) return __tmp != nullptr ? __tmp : ";
            node.right->accept(*this);
            output << "; else return __tmp; })()";
            return;
        }

        output << "(";
        node.left->accept(*this);
        output << " " << mapOperator(node.op) << " ";
        node.right->accept(*this);
        output << ")";
    }

    void Transpiler::visit(UnaryExpr &node)
    {
        // Handle prefix ++/-- operators
        if (node.op == "++" || node.op == "--")
        {
            output << node.op;
            node.operand->accept(*this);
        }
        else
        {
            output << mapOperator(node.op);
            node.operand->accept(*this);
        }
    }

    void Transpiler::visit(PostfixExpr &node)
    {
        node.operand->accept(*this);
        output << node.op; // ++ or --
    }

    void Transpiler::visit(CallExpr &node)
    {
        output << node.function << "(";
        for (size_t i = 0; i < node.arguments.size(); i++)
        {
            node.arguments[i]->accept(*this);
            if (i < node.arguments.size() - 1)
            {
                output << ", ";
            }
        }
        output << ")";
    }

    void Transpiler::visit(LambdaExpr &node)
    {
        // C++ lambda: [](params) { return expr; }
        // FIX BUG #137: Closure lifetime not tracked across async boundaries
        // TODO: When lambda returned/stored in async context, validate captures:
        // - Check if captured variables outlive lambda usage
        // - Warn if [] capture (by-reference via closure) in async function
        // - Suggest explicit capture list for async lambdas
        // Example UNSAFE:
        //   async fn foo() { let x = 42; return || x; } // x dies, lambda has dangling ref
        // Example SAFE:
        //   async fn foo() { let x = 42; return [x]() { return x; }; } // captures by value
        // Requires: Track function context (is async?) and lambda lifetime
        output << "[](";

        for (size_t i = 0; i < node.parameters.size(); i++)
        {
            auto &param = node.parameters[i];
            if (!param.second.empty())
            {
                std::string mappedType = mapType(param.second);
                if (mappedType.empty() || mappedType == param.second)
                    mappedType = "auto";
                output << mappedType << " ";
            }
            else
            {
                output << "auto ";
            }
            output << param.first;

            if (i < node.parameters.size() - 1)
            {
                output << ", ";
            }
        }

        // Rest parameter - use variadic template (FIX BUG #59, #69)
        if (node.hasRestParam)
        {
            if (!node.parameters.empty())
            {
                output << ", ";
            }
            // Use variadic template parameter pack
            output << "auto... " << node.restParamName;
        }

        output << ") { ";
        // For rest params, wrap in vector for body
        if (node.hasRestParam)
        {
            output << "auto " << node.restParamName << "_vec = std::vector{" << node.restParamName << "...}; ";
        }
        output << "return ";
        node.body->accept(*this);
        output << "; }";
    }
}

void Transpiler::visit(TernaryIfExpr &node)
{
    // ?cond -> a $ b  =>  (cond ? a : b)
    output << "(";
    node.condition->accept(*this);
    output << " ? ";
    node.thenExpr->accept(*this);
    output << " : ";
    node.elseExpr->accept(*this);
    output << ")";
}

void Transpiler::visit(PipelineExpr &node)
{
    // a |> f |> g  =>  g(f(a))
    // Build from outermost to innermost
    // FIX BUG #135: Pipeline type inference not validated
    // TODO: Check return type of stage N matches parameter type of stage N+1
    // - Requires type inference system to track types through pipeline
    // - Example VALID: 42 |> (x -> x * 2) |> (x -> x.toString())
    //   - 42: int -> (int -> int) -> int -> (int -> string) -> string
    // - Example INVALID: 42 |> (s -> s.length())
    //   - 42: int -> (string -> int) ERROR: int not compatible with string
    // - StaticAnalyzer should track types and error on mismatch
    // - Suggestion: Add type annotations or infer from lambda signatures
    if (node.stages.empty())
    {
        node.initial->accept(*this);
        return;
    }
    for (size_t i = node.stages.size(); i > 0; --i)
    {
        node.stages[i - 1]->accept(*this);
        output << "(";
    }

    node.initial->accept(*this);

    for (size_t i = 0; i < node.stages.size(); i++)
    {
        output << ")";
    }
}

void Transpiler::visit(CompositionExpr &node)
{
    // f . g . h  =>  lambda che fa f(g(h(x)))
    // In Haskell: (f . g . h) x = f(g(h(x)))
    // L'applicazione va da destra a sinistra: prima h, poi g, poi f
    // Ma nell'array abbiamo [f, g, h] in ordine di lettura
    // Quindi generiamo f(g(h(__x))) iterando NORMALMENTE

    output << "[]( auto __x ) { return ";

    // Itera in ordine normale: f, g, h
    for (size_t i = 0; i < node.functions.size(); i++)
    {
        node.functions[i]->accept(*this);
        output << "(";
    }

    output << "__x";

    for (size_t i = 0; i < node.functions.size(); i++)
    {
        output << ")";
    }

    output << "; }";
}

void Transpiler::visit(RangeExpr &node)
{
    // Generate IIFE that creates vector with range values
    // FIX BUG #134: Validate step is non-zero to prevent infinite loops
    // TODO: Add compile-time check if step is constant literal
    // - If step == 0: Compile error "Range step cannot be zero"
    // TODO: Add runtime assertion in generated code:
    // - assert(__step != 0 && "Range step cannot be zero");
    // - Or: if (__step == 0) throw std::invalid_argument("step == 0");
    // WARNING: step == 0 causes infinite loop: i += 0 never changes
    output << "([&]() { std::vector<int> __range; int __start = ";
    node.start->accept(*this);
    output << "; int __end = ";
    node.end->accept(*this);
    output << "; int __step = ";
    if (node.step)
    {
        node.step->accept(*this);
    }
    else
    {
        output << "1";
    }
    output << "; if (__step > 0) { for (int i = __start; i <= __end; i += __step) __range.push_back(i); }";
    output << " else { for (int i = __start; i >= __end; i += __step) __range.push_back(i); }";
    output << " return __range; })()";
}

void Transpiler::visit(MapExpr &node)
{
    // arr @ fn => IIFE with std::transform pattern
    output << "([&]() { std::vector<decltype(";
    node.fn->accept(*this);
    output << "(std::declval<decltype(";
    node.iterable->accept(*this);
    output << ")::value_type>()))> __result; for (auto& __item : ";
    node.iterable->accept(*this);
    output << ") { __result.push_back((";
    node.fn->accept(*this);
    output << ")(__item)); } return __result; })()";
}

void Transpiler::visit(FilterExpr &node)
{
    // arr ? |x| cond => IIFE with filter pattern
    output << "([&]() { std::remove_reference_t<decltype(";
    node.iterable->accept(*this);
    output << ")> __result; for (auto& __item : ";
    node.iterable->accept(*this);
    output << ") { if ((";
    node.predicate->accept(*this);
    output << ")(__item)) { __result.push_back(__item); } } return __result; })()";
}

void Transpiler::visit(ReduceExpr &node)
{
    // arr \ |acc,x| expr => IIFE with accumulate pattern
    output << "([&]() { auto __acc = ";
    if (node.initial)
    {
        node.initial->accept(*this);
    }
    else
    {
        output << "decltype(";
        node.iterable->accept(*this);
        output << ")::value_type{}";
    }
    output << "; for (auto& __item : ";
    node.iterable->accept(*this);
    output << ") { __acc = (";
    node.fn->accept(*this);
    output << ")(__acc, __item); } return __acc; })()";
}

void Transpiler::visit(IterateWhileExpr &node)
{
    // start !! condition $ stepFn => IIFE that generates array
    output << "([&]() { std::vector<decltype(";
    node.start->accept(*this);
    output << ")> __result; auto __current = ";
    node.start->accept(*this);
    output << "; while ((";
    node.condition->accept(*this);
    output << ")(__current)) { __result.push_back(__current); __current = (";
    node.stepFn->accept(*this);
    output << ")(__current); } return __result; })()";
}

void Transpiler::visit(AutoIterateExpr &node)
{
    // start !!< limit or start !!> limit
    output << "([&]() { std::vector<int> __result; int __current = ";
    node.start->accept(*this);
    output << "; int __limit = ";
    node.limit->accept(*this);
    output << "; ";
    if (node.isIncrement)
    {
        output << "while (__current < __limit) { __result.push_back(__current); __current++; }";
    }
    else
    {
        output << "while (__current > __limit) { __result.push_back(__current); __current--; }";
    }
    output << " return __result; })()";
}

void Transpiler::visit(IterateStepExpr &node)
{
    // start ~> stepFn !! condition
    output << "([&]() { std::vector<decltype(";
    node.start->accept(*this);
    output << ")> __result; auto __current = ";
    node.start->accept(*this);
    output << "; while ((";
    node.condition->accept(*this);
    output << ")(__current)) { __result.push_back(__current); __current = (";
    node.stepFn->accept(*this);
    output << ")(__current); } return __result; })()";
}

void Transpiler::visit(ArrayExpr &node)
{
    // Check if contains spread elements
    bool hasSpread = false;
    for (const auto &elem : node.elements)
    {
        if (dynamic_cast<SpreadExpr *>(elem.get()))
        {
            hasSpread = true;
            break;
        }
    }

    if (hasSpread)
    {
        // Generate IIFE that builds the array with spread
        output << "([&]() { std::vector<auto> __arr; ";
        for (const auto &elem : node.elements)
        {
            if (auto spreadExpr = dynamic_cast<SpreadExpr *>(elem.get()))
            {
                // Spread: insert all elements
                output << "__arr.insert(__arr.end(), (";
                spreadExpr->expression->accept(*this);
                output << ").begin(), (";
                spreadExpr->expression->accept(*this);
                output << ").end()); ";
            }
            else
            {
                // Regular element
                output << "__arr.push_back(";
                elem->accept(*this);
                output << "); ";
            }
        }
        output << "return __arr; })()";
    }
    else
    {
        // Simple array without spread: [1, 2, 3]
        // Use decltype on first element if available, otherwise int as fallback
        if (node.elements.empty())
        {
            // Empty array - default to int
            output << "std::vector<int>{}";
        }
        else
        {
            size_t elemCount = node.elements.size();
            output << "std::vector<decltype(";
            if (elemCount > 0)
                node.elements[0]->accept(*this);
            output << ")>{";
            for (size_t i = 0; i < elemCount; i++)
            {
                if (i < node.elements.size())
                    node.elements[i]->accept(*this);
                if (i < elemCount - 1)
                {
                    output << ", ";
                }
            }
            output << "}";
        }
    }
}

void Transpiler::visit(TupleExpr &node)
{
    // Tuple: (1, 2, 3) => std::make_tuple(1, 2, 3)
    // Empty tuple: () => std::make_tuple()
    output << "std::make_tuple(";
    for (size_t i = 0; i < node.elements.size(); i++)
    {
        node.elements[i]->accept(*this);
        if (i < node.elements.size() - 1)
        {
            output << ", ";
        }
    }
    output << ")";
}

void Transpiler::visit(ListComprehension &node)
{
    // [x*2 | x in 0..10, x > 3]
    // Trasformiamo in un loop che costruisce un vector
    std::string tempVar = "__comp_" + std::to_string(lambdaCounter++);

    output << "([&]() { std::vector<decltype(";
    node.expression->accept(*this);
    output << ")> " << tempVar << "; ";

    // For loop sul range
    output << "for (auto " << node.variable << " = ";
    // Assumiamo che range sia un RangeExpr
    if (auto *range = dynamic_cast<RangeExpr *>(node.range.get()))
    {
        range->start->accept(*this);
        output << "; " << node.variable << " <= ";
        range->end->accept(*this);
        output << "; ";
        if (range->step)
        {
            output << node.variable << " += ";
            range->step->accept(*this);
        }
        else
        {
            output << "++" << node.variable;
        }
        output << ") { ";

        // Predicati
        if (!node.predicates.empty())
        {
            output << "if (";
            for (size_t i = 0; i < node.predicates.size(); i++)
            {
                node.predicates[i]->accept(*this);
                if (i < node.predicates.size() - 1)
                {
                    output << " && ";
                }
            }
            output << ") { ";
        }

        // Push dell'espressione
        output << tempVar << ".push_back(";
        node.expression->accept(*this);
        output << "); ";

        if (!node.predicates.empty())
        {
            output << "} ";
        }

        output << "} return " << tempVar << "; })()";
    }
    else
    {
        // Fallback per range non riconosciuto
        output << "std::vector<int>{}";
    }
}

void Transpiler::visit(SpreadExpr &node)
{
    // Spread operator: ...array
    // In array context: [1, 2, ...arr, 3] non ha equivalente diretto in C++
    // Generiamo inserimento manuale degli elementi
    // Note: this will be handled better in array context
    output << "/* SPREAD: ";
    node.expression->accept(*this);
    output << " */";
}

void Transpiler::visit(IndexExpr &node)
{
    // Optional chaining: obj?.prop => (obj != nullptr ? obj.prop : nullptr)
    if (node.isOptional)
    {
        output << "([&]() { auto __obj = ";
        node.object->accept(*this);
        output << "; return __obj != nullptr ? __obj.";
        node.index->accept(*this);
        output << " : nullptr; })()";
        return;
    }

    // Normal access: arr[index] or obj.prop
    node.object->accept(*this);
    if (node.isDot)
    {
        output << ".";
        node.index->accept(*this);
    }
    else
    {
        output << "[";
        node.index->accept(*this);
        output << "]";
    }
}

void Transpiler::visit(VarDecl &node)
{
    indent();

    // Handle different type annotations
    std::string cppType;

    if (!node.unionTypes.empty())
    {
        // Union type: int | string -> std::variant<int, std::string>
        // FIX BUG #110: Validate union type count
        // TODO: Limit union types to reasonable number (e.g., 32 max)
        // std::variant with too many types causes compilation slowdown
        // Recommend using inheritance for large unions
        const size_t MAX_UNION_TYPES = 32;
        if (node.unionTypes.size() > MAX_UNION_TYPES)
        {
            // Should emit warning during parsing
        }

        output << "std::variant<";
        for (size_t i = 0; i < node.unionTypes.size(); i++)
        {
            output << mapType(node.unionTypes[i]);
            if (i < node.unionTypes.size() - 1)
                output << ", ";
        }
        output << "> " << node.name;
    }
    else if (node.isArrayType)
    {
        if (node.arraySize > 0)
        {
            // Fixed-size array: int[10] -> std::array<int, 10>
            // FIX BUG #112: Array size not validated as constant
            // TODO: Check that arraySize comes from:
            // - Integer literal
            // - Const expression
            // - Constexpr evaluation
            // Not runtime variable: let n = 10; let arr: int[n]; // ERROR
            // Requires constant expression evaluator in parser

            output << "std::array<" << mapType(node.type) << ", " << node.arraySize << "> " << node.name;
        }
        else
        {
            // Dynamic array: int[] -> std::vector<int>
            output << "std::vector<" << mapType(node.type) << "> " << node.name;
        }
    }
    else if (node.isNullable)
    {
        // Nullable: int? -> std::optional<int>
        // FIX BUG #111: Detect nested optional types
        // TODO: Parse node.type to detect optional<optional<T>> pattern
        // Warn on excessive nesting (> 3 levels)
        // Example: int??? -> optional<optional<optional<int>>>
        // Better: Use Result<T, E> or custom wrapper

        output << "std::optional<" << mapType(node.type) << "> " << node.name;
    }
    else
    {
        // Regular type
        output << mapType(node.type) << " " << node.name;
    }

    if (node.initializer)
    {
        output << " = ";
        node.initializer->accept(*this);
    }
    output << ";\n";
}

void Transpiler::visit(QuantumVarDecl &node)
{
    indent();

    // Determine element type
    std::string elementType = mapType(node.type);

    // Generate QuantumVar<T>
    output << "QuantumVar<" << elementType << "> " << node.name;

    if (node.hasWeights)
    {
        // Weighted: QuantumVar<int> x({states}, {probs})
        output << "({";
        for (size_t i = 0; i < node.states.size(); i++)
        {
            node.states[i]->accept(*this);
            if (i < node.states.size() - 1)
                output << ", ";
        }
        output << "}, {";
        for (size_t i = 0; i < node.probabilities.size(); i++)
        {
            output << node.probabilities[i];
            if (i < node.probabilities.size() - 1)
                output << ", ";
        }
        output << "})";
    }
    else
    {
        // Uniform: QuantumVar<int> x({states})
        output << "({";
        for (size_t i = 0; i < node.states.size(); i++)
        {
            node.states[i]->accept(*this);
            if (i < node.states.size() - 1)
                output << ", ";
        }
        output << "})";
    }

    output << ";\n";
}

void Transpiler::visit(Assignment &node)
{
    indent();
    output << node.name << " = ";
    node.value->accept(*this);
    output << ";\n";
}

void Transpiler::visit(IfStmt &node)
{
    indent();
    output << "if (";
    node.condition->accept(*this);
    output << ") {\n";

    indentLevel++;
    for (auto &stmt : node.thenBranch)
    {
        stmt->accept(*this);
    }
    indentLevel--;

    if (!node.elseBranch.empty())
    {
        indent();
        output << "} else {\n";
        indentLevel++;
        for (auto &stmt : node.elseBranch)
        {
            stmt->accept(*this);
        }
        indentLevel--;
    }

    indent();
    output << "}\n";
}

void Transpiler::visit(WhileStmt &node)
{
    indent();
    output << "while (";
    node.condition->accept(*this);
    output << ") {\n";

    indentLevel++;
    for (auto &stmt : node.body)
    {
        stmt->accept(*this);
    }
    indentLevel--;

    indent();
    output << "}\n";
}

void Transpiler::visit(SwitchStmt &node)
{
    indent();
    output << "switch (";
    node.condition->accept(*this);
    output << ") {\n";

    indentLevel++;
    // FIX BUG #75: Warn about implicit fallthrough
    for (size_t i = 0; i < node.cases.size(); i++)
    {
        const auto &caseClause = node.cases[i];
        if (caseClause.isDefault)
        {
            indent();
            output << "default:\n";
        }
        else
        {
            indent();
            output << "case ";
            // TODO BUG #92: Validate case value is compile-time constant
            caseClause.value->accept(*this);
            output << ":\n";
        }

        indentLevel++;
        for (auto &stmt : caseClause.statements)
        {
            stmt->accept(*this);
        }
        // TODO BUG #75: Check if last statement is break, else warn about fallthrough
        indentLevel--;
    }
    indentLevel--;

    indent();
    output << "}\n";
}

void Transpiler::visit(BreakStmt &node)
{
    indent();
    output << "break;\n";
}

void Transpiler::visit(ContinueStmt &node)
{
    indent();
    output << "continue;\n";
}

void Transpiler::visit(ReturnStmt &node)
{
    indent();
    output << "return";
    if (node.value)
    {
        output << " ";
        node.value->accept(*this);
    }
    output << ";\n";
}

void Transpiler::visit(ImportStmt &node)
{
    // Transpile to C++ #include
    indent();
    output << "#include \"" << node.module << ".hpp\"\n";
}

void Transpiler::visit(ExportStmt &node)
{
    // In C++, exports are handled by header files
    // Just transpile the declaration normally
    if (node.declaration)
    {
        node.declaration->accept(*this);
    }
}

void Transpiler::visit(AutoPatternStmt &node)
{
    // AutoPattern is expanded during parsing into ClassDecl
    // This should not be reached during transpilation
    // but included for completeness
    output << "// Auto-generated pattern: " << node.patternType << " for " << node.className << "\n";
}

void Transpiler::visit(ExprStmt &node)
{
    indent();
    node.expression->accept(*this);
    output << ";\n";
}

void Transpiler::visit(Function &node)
{
    // Generate template for generics and/or rest parameters
    bool needsTemplate = !node.genericParams.empty() || node.hasRestParam;

    if (needsTemplate)
    {
        indent();
        output << "template<";

        // Generic parameters
        for (size_t i = 0; i < node.genericParams.size(); i++)
        {
            output << "typename " << node.genericParams[i];
            if (i < node.genericParams.size() - 1 || node.hasRestParam)
            {
                output << ", ";
            }
        }

        // Rest parameter as variadic template
        if (node.hasRestParam)
        {
            output << "typename... RestArgs";
        }

        output << ">\n";
    }

    // Function signature with proper indentation
    indent();

    // Handle async functions - wrap return type in std::future
    // Handle generator functions - should use std::generator or custom type
    if (node.isGenerator)
    {
        // FIX BUG #127: Generator promise_type requirement
        // TODO: Include proper coroutine support header
        // - #include <generator> (C++23)
        // - Define promise_type for co_yield
        // - Use std::generator<T> return type
        // Current: Just comment, doesn't compile

        output << "/* generator */ " << mapType(node.returnType) << " " << node.name << "(";
    }
    else if (node.isAsync)
    {
        output << "std::future<" << mapType(node.returnType) << "> " << node.name << "(";
    }
    else
    {
        output << mapType(node.returnType) << " " << node.name << "(";
    }

    for (size_t i = 0; i < node.parameters.size(); i++)
    {
        output << mapType(node.parameters[i].second) << " " << node.parameters[i].first;
        if (i < node.parameters.size() - 1 || node.hasRestParam)
        {
            output << ", ";
        }
    }

    // Rest parameter as variadic pack
    if (node.hasRestParam)
    {
        output << "RestArgs... " << node.restParamName;
    }

    output << ") {\n";

    indentLevel++;

    // Convert rest parameters to vector for easy iteration
    // FIX BUG #58, #66: Use unique ID to prevent macro collisions
    // FIX BUG #72: lambdaCounter not thread-safe (TODO: use atomic or per-thread counter)
    // FIX BUG #130: Macro pollution in nested functions
    // TODO: Use __COUNTER__ preprocessor built-in instead of runtime counter
    // - __COUNTER__ increments at compile-time, guaranteed unique
    // TODO: Ensure macros undefined (#undef) after function body
    // - Currently: Macros persist beyond function scope (pollution)
    // - Risk: Outer scope macros shadow inner ones, name collisions
    // TODO: Consider constexpr variables or inline lambdas instead of #define
    // - Modern C++ alternatives avoid preprocessor pollution entirely
    std::string restMacroName;
    if (node.hasRestParam)
    {
        // Use function name + counter for uniqueness
        std::string uniqueId = node.name + "_" + std::to_string(lambdaCounter++);
        restMacroName = "__LPP_REST_" + node.restParamName + "_" + uniqueId;
        std::string vecName = "__rest_vec_" + node.restParamName + "_" + uniqueId;
        indent();
        output << "// Convert variadic pack to vector for iteration\n";
        indent();
        output << "auto " << vecName << " = std::vector{" << node.restParamName << "...};\n";
        indent();
        output << "#define " << restMacroName << " " << vecName << "\n";
        indent();
        output << "#define " << node.restParamName << " " << restMacroName << "\n";
    }

    // For async functions, wrap body in std::async
    // FIX BUG #184: Async [&] capture causes data races on local variables
    // TODO: Detect which variables are actually used in async body
    // - Capture by value for primitives: [x, y] instead of [&]
    // - Use shared_ptr for objects: [ptr = std::shared_ptr(obj)]
    // - Warn on capture of local references in async context
    // Example UNSAFE:
    //   async fn foo() { let x = 42; return async { x }; } // x dangling
    // Example SAFE:
    //   async fn foo() { let x = 42; return async { [x]() { return x; } }; }
    // FIX BUG #129: Async functions without await are pure overhead
    // TODO: Static analysis to detect await expressions in function body
    // - Track awaitCount during AST traversal
    // - If awaitCount == 0: skip std::async wrapper (synchronous execution)
    // - Warn: "Function marked async but contains no await calls"
    // - Save thread pool resources and eliminate future overhead
    // TODO: Add try-catch for exception propagation in futures
    // TODO: Optimize out async if no await calls detected (static analysis)
    // NOTE: [&] capture includes rest parameters from outer scope
    if (node.isAsync)
    {
        indent();
        output << "return std::async(std::launch::async, [&]() {\n";
        indentLevel++;
    }

    for (auto &stmt : node.body)
    {
        stmt->accept(*this);
    }

    // For async functions, ensure implicit return if void
    if (node.isAsync && node.returnType == "void")
    {
        indent();
        output << "return;\n";
    }

    // Undefine rest parameter macros (FIX BUG #57)
    if (node.hasRestParam)
    {
        indent();
        output << "#undef " << node.restParamName << "\n";
        indent();
        output << "#undef " << restMacroName << "\n";
    }

    if (node.isAsync)
    {
        indentLevel--;
        indent();
        output << "});\n";
    }

    indentLevel--;

    indent();
    output << "}\n";
}

void Transpiler::visit(Program &node)
{
    // Imports first
    for (auto &imp : node.imports)
    {
        imp->accept(*this);
    }

    // Type declarations
    for (auto &type : node.types)
    {
        type->accept(*this);
        writeLine("");
    }

    // Enums
    for (auto &enumDecl : node.enums)
    {
        enumDecl->accept(*this);
        writeLine("");
    }

    // Interfaces
    for (auto &intf : node.interfaces)
    {
        intf->accept(*this);
        writeLine("");
    }

    // Classes
    for (auto &cls : node.classes)
    {
        cls->accept(*this);
        writeLine("");
    }

    // Functions
    for (auto &func : node.functions)
    {
        func->accept(*this);
    }
}

void Transpiler::indent()
{
    for (int i = 0; i < indentLevel; i++)
    {
        output << "    ";
    }
}

void Transpiler::writeLine(const std::string &line)
{
    indent();
    output << line << "\n";
}

std::string Transpiler::mapType(const std::string &lppType)
{
    if (lppType == "int")
        return "int";
    if (lppType == "float")
        return "double";
    if (lppType == "string")
        return "std::string";
    if (lppType == "bool")
        return "bool";
    if (lppType == "void")
        return "void";

    // FIX BUG #109: Unknown types returned as-is without validation
    // TODO: Validate that lppType is:
    // - A defined class/struct name
    // - A generic type parameter (T, U, etc.)
    // - A valid template instantiation (Vec<int>)
    // Otherwise emit warning: "Unknown type '{}' - assuming C++ compatible"

    return lppType;
}

std::string Transpiler::mapOperator(const std::string &op)
{
    if (op == "and")
        return "&&";
    if (op == "or")
        return "||";
    if (op == "not")
        return "!";
    return op;
}

std::string Transpiler::convertMethodSignature(const std::string &lppSignature)
{
    // Convert "draw() -> void" to "void draw()"
    // Convert "add(x: int, y: int) -> int" to "int add(int x, int y)"

    size_t arrowPos = lppSignature.find(" -> ");
    if (arrowPos == std::string::npos)
        return lppSignature; // Fallback

    std::string nameAndParams = lppSignature.substr(0, arrowPos);
    std::string returnType = lppSignature.substr(arrowPos + 4);

    // Extract method name
    size_t parenPos = nameAndParams.find('(');
    std::string methodName = nameAndParams.substr(0, parenPos);
    std::string params = nameAndParams.substr(parenPos);

    // Convert parameter types (just map them)
    std::string convertedParams = params;
    // Simple replacement for common types in params

    // FIX BUG #113: Preserve parameter names and order
    // TODO: Proper parsing of parameter list with regex or tokenization
    // Current: Simple string replacement may break on edge cases
    // Example: "callback: (data: string) -> void" loses structure
    // Better: Parse each parameter individually

    size_t pos = 0;
    while ((pos = convertedParams.find("float", pos)) != std::string::npos)
    {
        convertedParams.replace(pos, 5, "double");
        pos += 6;
    }
    pos = 0;
    while ((pos = convertedParams.find("string", pos)) != std::string::npos)
    {
        convertedParams.replace(pos, 6, "std::string");
        pos += 11;
    }

    return mapType(returnType) + " " + methodName + convertedParams;
}

void Transpiler::visit(ObjectExpr &node)
{
    // Object literal: {name: "Alice", age: 30}
    // Traspila a struct o std::map
    output << "std::map<std::string, std::any>{";
    for (size_t i = 0; i < node.properties.size(); i++)
    {
        output << "{\"" << node.properties[i].first << "\", ";
        node.properties[i].second->accept(*this);
        output << "}";
        if (i < node.properties.size() - 1)
            output << ", ";
    }
    output << "}";
}

void Transpiler::visit(MatchExpr &node)
{
    // Pattern matching: match expr { case pattern -> result }
    // Traspila a if-else chain o switch
    // FIX BUG #122: Detect unreachable patterns
    // TODO: Track if wildcard (_) pattern encountered
    // Any patterns after _ are unreachable
    // Example: match x { _ -> "default", 1 -> "one" } // 1 unreachable

    std::string matchVar = "__match_" + std::to_string(matchCounter++);
    output << "([&]() { auto " << matchVar << " = ";
    node.expression->accept(*this);
    output << "; ";

    for (size_t i = 0; i < node.cases.size(); i++)
    {
        // FIX BUG #123: Variable patterns need proper scope
        // TODO: If pattern is identifier (not literal), it's a binding
        // Example: match x { y -> y + 1 } // y binds to x's value
        // Need to generate: if (true) { auto y = matchVar; return y + 1; }
        // Current: treats as equality check

        if (i > 0)
            output << " else ";
        output << "if (" << matchVar << " == ";
        node.cases[i].first->accept(*this);
        output << ") { return ";
        node.cases[i].second->accept(*this);
        output << "; }";
    }
    output << " })()";
}

void Transpiler::visit(ClassDecl &node)
{
    // Class declaration
    writeLine("class " + node.name + (node.baseClass.empty() ? "" : " : public " + node.baseClass) + " {");

    // If design pattern is specified, inject pattern code
    if (!node.designPattern.empty())
    {
        writeLine("// AUTO-GENERATED: " + node.designPattern + " Pattern");
        writeLine("#include \"lpp_patterns.hpp\"");

        if (node.designPattern == "Singleton")
        {
            writeLine("LPP_PATTERN_SINGLETON(" + node.name + ")");
        }
        else if (node.designPattern == "Observer")
        {
            writeLine("LPP_PATTERN_OBSERVER(" + node.name + ")");
        }
        else if (node.designPattern == "Builder")
        {
            writeLine("LPP_PATTERN_BUILDER(" + node.name + ")");
        }
        else if (node.designPattern == "Command")
        {
            writeLine("LPP_PATTERN_COMMAND()");
        }
        // Add more patterns as needed
    }

    writeLine("public:");
    indentLevel++;

    // Properties
    for (const auto &prop : node.properties)
    {
        writeLine(mapType(prop.second) + " " + prop.first + ";");
    }
    writeLine("");

    // Constructor
    if (node.constructor)
    {
        node.constructor->accept(*this);
        writeLine("");
    }

    // Methods
    for (auto &method : node.methods)
    {
        method->accept(*this);
        writeLine("");
    }

    indentLevel--;
    writeLine("};");
}

void Transpiler::visit(InterfaceDecl &node)
{
    // Interface declaration → Abstract class in C++
    writeLine("class " + node.name + " {");
    writeLine("public:");
    indentLevel++;
    writeLine("virtual ~" + node.name + "() = default;");
    writeLine("");

    for (const auto &method : node.methods)
    {
        std::string cppSignature = convertMethodSignature(method.second);
        writeLine("virtual " + cppSignature + " = 0;");
    }

    indentLevel--;
    writeLine("};");
}
void Transpiler::visit(TypeDecl &node)
{
    // Algebraic Data Type: type Option<T> = Some(T) | None
    // Traspila a std::variant con struct per ogni variant

    writeLine("// ADT: " + node.name);

    // Generate struct for each variant
    for (const auto &variant : node.variants)
    {
        writeLine("struct " + variant.first + " {");
        indentLevel++;

        if (!variant.second.empty())
        {
            // Generate fields
            for (size_t i = 0; i < variant.second.size(); i++)
            {
                writeLine(mapType(variant.second[i]) + " _" + std::to_string(i) + ";");
            }
        }

        indentLevel--;
        writeLine("};");
    }

    // Generate the variant type alias
    std::string typeParams = "";
    if (!node.typeParams.empty())
    {
        typeParams = "<";
        for (size_t i = 0; i < node.typeParams.size(); i++)
        {
            typeParams += "typename " + node.typeParams[i];
            if (i < node.typeParams.size() - 1)
                typeParams += ", ";
        }
        typeParams += ">";
    }

    if (!typeParams.empty())
    {
        writeLine("template" + typeParams);
    }

    writeLine("using " + node.name + " = std::variant<");
    indentLevel++;
    for (size_t i = 0; i < node.variants.size(); i++)
    {
        writeLine(node.variants[i].first + (i < node.variants.size() - 1 ? "," : ""));
    }
    indentLevel--;
    writeLine(">;");
    writeLine("");
}

// NEW IMPLEMENTATIONS - Cast, Await, Throw
void Transpiler::visit(CastExpr &node)
{
    output << "static_cast<" << mapType(node.targetType) << ">(";
    node.expression->accept(*this);
    output << ")";
}

void Transpiler::visit(AwaitExpr &node)
{
    // C++20 co_await or .get() for std::future
    // WARNING: .get() blocks indefinitely if future never completes
    // FIX BUG #126: Add timeout support for await
    // TODO: Implement await with configurable timeout
    // - Use future.wait_for(std::chrono::seconds(timeout)) instead of .get()
    // - Throw timeout exception if wait_for returns timeout status
    // - Default timeout: 30 seconds (configurable via pragma)
    // Example: auto status = fut.wait_for(30s); if (status == timeout) throw ...
    // TODO: Consider std::future::wait_for with timeout
    // TODO: Validate expression type is std::future<T> before calling .get()
    output << "(";
    node.expression->accept(*this);
    output << ").get()"; // Assuming std::future
}

void Transpiler::visit(ThrowExpr &node)
{
    indent();
    output << "throw ";
    node.expression->accept(*this);
    output << ";\n";
}

void Transpiler::visit(YieldExpr &node)
{
    // FIX BUG #125: Validate generator context
    // TODO: Check if in generator function scope
    // - Track currentFunction->isGenerator
    // - Error if yield outside generator
    // - Validate generator return type is std::generator<T>
    // Example: fn foo() { yield 42; } // ERROR: not generator

    // co_yield requires C++20 coroutine support
    output << "co_yield ";
    if (node.value)
    {
        node.value->accept(*this);
    }
}

void Transpiler::visit(TypeOfExpr &node)
{
    output << "typeid(";
    node.expr->accept(*this);
    output << ").name()";
}

void Transpiler::visit(InstanceOfExpr &node)
{
    output << "dynamic_cast<" << mapType(node.typeName) << "*>(";
    node.expr->accept(*this);
    output << ") != nullptr";
}

void Transpiler::visit(QuantumMethodCall &node)
{
    // quantum methods: observe(), map(fn), reset(), entangle(fn)
    if (node.method == "observe")
    {
        output << node.quantumVar << ".observe()";
    }
    else if (node.method == "reset")
    {
        output << node.quantumVar << ".reset()";
    }
    else if (node.method == "map")
    {
        // qvar.map(lambda)
        output << node.quantumVar << ".map(";
        if (!node.args.empty())
        {
            node.args[0]->accept(*this);
        }
        output << ")";
    }
    else if (node.method == "entangle")
    {
        // entangle(qvar, fn) => qvar.entangle(fn)
        output << node.quantumVar << ".entangle(";
        if (!node.args.empty())
        {
            node.args[0]->accept(*this);
        }
        output << ")";
    }
}

// NEW IMPLEMENTATIONS - For, ForIn, DoWhile
void Transpiler::visit(ForStmt &node)
{
    indent();
    output << "for (";

    // Initializer
    if (node.initializer)
    {
        // Inline the initialization without newline
        if (auto *varDecl = dynamic_cast<VarDecl *>(node.initializer.get()))
        {
            output << mapType(varDecl->type) << " " << varDecl->name;
            if (varDecl->initializer)
            {
                output << " = ";
                varDecl->initializer->accept(*this);
            }
        }
    }
    output << "; ";

    // Condition
    if (node.condition)
    {
        node.condition->accept(*this);
    }
    output << "; ";

    // Increment
    if (node.increment)
    {
        node.increment->accept(*this);
    }
    output << ") {\n";

    indentLevel++;
    for (auto &stmt : node.body)
    {
        stmt->accept(*this);
    }
    indentLevel--;
    indent();
    output << "}\n";
}

void Transpiler::visit(ForInStmt &node)
{
    indent();
    output << "for (auto " << node.variable << " : ";
    node.iterable->accept(*this);
    output << ") {\n";

    indentLevel++;
    for (auto &stmt : node.body)
    {
        stmt->accept(*this);
    }
    indentLevel--;
    indent();
    output << "}\n";
}

void Transpiler::visit(DoWhileStmt &node)
{
    indent();
    output << "do {\n";

    indentLevel++;
    for (auto &stmt : node.body)
    {
        stmt->accept(*this);
    }
    indentLevel--;

    indent();
    output << "} while (";
    node.condition->accept(*this);
    output << ");\n";
}

// NEW IMPLEMENTATIONS - TryCatch
void Transpiler::visit(TryCatchStmt &node)
{
    indent();
    output << "try {\n";

    indentLevel++;
    for (auto &stmt : node.tryBlock)
    {
        stmt->accept(*this);
    }
    indentLevel--;

    indent();
    output << "} catch (";
    if (!node.catchVariable.empty())
    {
        output << "const std::exception& " << node.catchVariable;
    }
    else
    {
        output << "...";
    }
    output << ") {\n";

    indentLevel++;
    for (auto &stmt : node.catchBlock)
    {
        stmt->accept(*this);
    }
    indentLevel--;

    if (!node.finallyBlock.empty())
    {
        indent();
        output << "}\n";
        indent();
        output << "// FIX BUG #76, #87: Finally block needs proper RAII guard\n";
        indent();
        output << "// TODO: Wrap in scope_exit or similar for proper cleanup\n";
        indent();
        output << "// WARNING: return/throw in finally may not execute as expected\n";
        indent();
        output << "{\n";
        indentLevel++;
        for (auto &stmt : node.finallyBlock)
        {
            stmt->accept(*this);
        }
        indentLevel--;
    }

    indent();
    output << "}\n";
}

// NEW IMPLEMENTATIONS - Destructuring
void Transpiler::visit(DestructuringStmt &node)
{
    if (node.isTuple)
    {
        // Tuple destructuring: let (a, b, c) = tuple
        // Use std::tie or individual std::get
        indent();
        output << "auto __tuple_tmp = ";
        node.source->accept(*this);
        output << ";\n";

        for (size_t i = 0; i < node.targets.size(); i++)
        {
            indent();
            output << "auto " << node.targets[i] << " = std::get<" << i << ">(__tuple_tmp);\n";
        }
    }
    else if (node.isArray)
    {
        // Array destructuring: let [a, b, c] = arr
        indent();
        output << "auto __tmp = ";
        node.source->accept(*this);
        output << ";\n";

        for (size_t i = 0; i < node.targets.size(); i++)
        {
            indent();
            output << "auto " << node.targets[i] << " = __tmp[" << i << "];\n";
        }
    }
    else
    {
        // Object destructuring: let {x, y} = obj
        indent();
        output << "auto __tmp = ";
        node.source->accept(*this);
        output << ";\n";

        for (const auto &target : node.targets)
        {
            indent();
            output << "auto " << target << " = __tmp[\"" << target << "\"];\n";
        }
    }
}

// NEW IMPLEMENTATIONS - Enum
void Transpiler::visit(EnumDecl &node)
{
    indent();
    output << "enum " << node.name << " {\n";
    indentLevel++;

    for (size_t i = 0; i < node.values.size(); i++)
    {
        indent();
        output << node.values[i].first;
        if (node.values[i].second >= 0)
        {
            output << " = " << node.values[i].second;
        }
        if (i < node.values.size() - 1)
        {
            output << ",";
        }
        output << "\n";
    }

    indentLevel--;
    indent();
    output << "};\n";
}

} // namespace lpp
