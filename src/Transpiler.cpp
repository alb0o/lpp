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
        writeLine("#include <functional>");
        writeLine("#include <variant>");
        writeLine("#include <map>");
        writeLine("#include <any>");
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
        output << "(";
        for (size_t i = 0; i < node.strings.size(); i++)
        {
            if (i > 0)
                output << " + ";

            output << "std::string(\"" << node.strings[i] << "\")";

            if (i < node.interpolations.size())
            {
                output << " + std::to_string(";
                node.interpolations[i]->accept(*this);
                output << ")";
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
            output << "; return __tmp != nullptr && __tmp != 0 ? __tmp : ";
            node.right->accept(*this);
            output << "; })()";
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
        output << mapOperator(node.op);
        node.operand->accept(*this);
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
        output << "[](";

        for (size_t i = 0; i < node.parameters.size(); i++)
        {
            auto &param = node.parameters[i];
            if (!param.second.empty())
            {
                output << mapType(param.second) << " ";
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

        // Rest parameter
        if (node.hasRestParam)
        {
            if (!node.parameters.empty())
            {
                output << ", ";
            }
            output << "std::vector<auto> " << node.restParamName;
        }

        output << ") { return ";
        node.body->accept(*this);
        output << "; }";
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
        for (int i = node.stages.size() - 1; i >= 0; i--)
        {
            node.stages[i]->accept(*this);
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
        // Range viene usato tipicamente in for loops o list comprehension
        // Per ora generiamo una struct temporanea con begin/end
        // Implementazione semplificata: generiamo direttamente i valori
        output << "/* range: ";
        node.start->accept(*this);
        output << "..";
        node.end->accept(*this);
        if (node.step)
        {
            output << "..";
            node.step->accept(*this);
        }
        output << " */";
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
            // Simple array without spread: [1, 2, 3]  =>  std::vector<auto>{1, 2, 3}
            output << "std::vector<auto>{";
            for (size_t i = 0; i < node.elements.size(); i++)
            {
                node.elements[i]->accept(*this);
                if (i < node.elements.size() - 1)
                {
                    output << ", ";
                }
            }
            output << "}";
        }
    }

    void Transpiler::visit(ListComprehension &node)
    {
        // [x*2 | x in 0..10, x > 3]
        // Trasformiamo in un loop che costruisce un vector
        std::string tempVar = "__comp_" + std::to_string(lambdaCounter++);

        output << "([&]() { std::vector<auto> " << tempVar << "; ";

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
            output << "std::vector<auto>{}";
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
        output << mapType(node.type) << " " << node.name;
        if (node.initializer)
        {
            output << " = ";
            node.initializer->accept(*this);
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

    void Transpiler::visit(ExprStmt &node)
    {
        indent();
        node.expression->accept(*this);
        output << ";\n";
    }

    void Transpiler::visit(Function &node)
    {
        // Function signature with proper indentation
        indent();
        output << mapType(node.returnType) << " " << node.name << "(";

        for (size_t i = 0; i < node.parameters.size(); i++)
        {
            output << mapType(node.parameters[i].second) << " " << node.parameters[i].first;
            if (i < node.parameters.size() - 1)
            {
                output << ", ";
            }
        }

        // Rest parameter
        if (node.hasRestParam)
        {
            if (!node.parameters.empty())
            {
                output << ", ";
            }
            output << "std::vector<auto> " << node.restParamName;
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

    void Transpiler::visit(Program &node)
    {
        // Type declarations first
        for (auto &type : node.types)
        {
            type->accept(*this);
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
        size_t pos = 0;
        while ((pos = convertedParams.find("int", pos)) != std::string::npos)
        {
            convertedParams.replace(pos, 3, "int");
            pos += 3;
        }
        pos = 0;
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
        std::string matchVar = "__match_" + std::to_string(matchCounter++);
        output << "([&]() { auto " << matchVar << " = ";
        node.expression->accept(*this);
        output << "; ";

        for (size_t i = 0; i < node.cases.size(); i++)
        {
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

} // namespace lpp
