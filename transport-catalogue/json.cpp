#include "json.h"

using namespace std;

namespace json {

    namespace {
        std::string LoadWord(std::istream& input) {
            std::string word;

            while (std::isalpha(input.peek())) {
                word.push_back(static_cast<char>(input.get()));
            }
            return word;
        }


        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;

            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }

            if (!input) {
                throw ParsingError("LoadArray"s);
            }

            return Node(move(result));
        }

        using Number = std::variant<int, double>;

        Number LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
                };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
                };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // Считывает содержимое строкового литерала JSON-документа
        // Функцию следует использовать после считывания открывающего символа ":
        std::string LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return s;
        }

        Node LoadDict(istream& input) {
            Dict result;

            for (char c; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input);
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }

            if (!input) {
                throw ParsingError("LoadArray"s);
            }

            return Node(move(result));
        }

        bool LoadBool(std::istream& input) {
            const auto str = LoadWord(input);

            if (str == "true") {
                return true;
            }
            if (str == "false") {
                return false;
            }

            throw ParsingError("LoadBool");
        }

        std::nullptr_t LoadNull(std::istream& input) {
            if (LoadWord(input) == "null") {
                return nullptr;
            }

            throw ParsingError("LoadNull");
        }

        Node LoadNode(istream& input) {
            char c;
            if (!(input >> c)) {
                throw ParsingError(""s);
            }

            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return Node(LoadString(input));
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return Node(LoadBool(input));
            }
            else if (c == 'n') {
                input.putback(c);
                return Node(LoadNull(input));
            }
            else {
                input.putback(c);
                Number num = LoadNumber(input);
                if (std::holds_alternative<double>(num)) {
                    return Node(std::get<double>(num));
                }
                return Node(std::get<int>(num));
            }
        }

    }  // namespace


    int Node::AsInt() const {
        if (IsInt()) {
            return std::get<int>(node_);
        }
        throw std::logic_error("Error");
    }

    bool Node::AsBool() const
    {
        if (IsBool()) {
            return std::get<bool>(node_);
        }
        throw std::logic_error("Error");
    }

    double Node::AsDouble() const
    {
        if (IsInt()) {
            return std::get<int>(node_) * 1.0;
        }
        if (IsDouble()) {
            return std::get<double>(node_);
        }
        throw std::logic_error("Error");
    }

    const string& Node::AsString() const {
        if (IsString()) {
            return std::get<std::string>(node_);
        }
        throw std::logic_error("Error");
    }

    const Array& Node::AsArray() const {
        if (IsArray()) {
            return std::get<Array>(node_);
        }
        throw std::logic_error("Error");
    }

    const Dict& Node::AsMap() const {
        if (IsMap()) {
            return std::get<Dict>(node_);
        }
        throw std::logic_error("Error");
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void PrintString(const std::string& value, std::ostream& out) {
        for (const char ch : value) {
            switch (ch) {
            case '\r':
                out << R"(\r)";
                break;
            case '\n':
                out << R"(\n)";
                break;
            case '\t':
                out << R"(\t)";
                break;
            case '"':
                out << R"(\")";
                break;
            case '\\':
                out << R"(\\)";
                break;
            default:
                out.put(ch);
                break;
            }
        }
    }

    void Print(const Document& doc, std::ostream& output) {
        output << doc.GetRoot();
    }

    bool Node::IsInt() const {
        return std::holds_alternative<int>(node_);
    }

    bool Node::IsDouble() const {
        return std::holds_alternative<double>(node_) || IsInt();
    }

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(node_);
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(node_);
    }

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(node_);
    }

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(node_);
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(node_);
    }

    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(node_);
    }

}  // namespace json