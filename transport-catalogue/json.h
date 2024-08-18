#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

    class Node;

    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        using Type = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;

        Node() = default;

        template <typename Type>
        Node(Type type)
            : node_(std::move(type)) {
        }

        Type GetNode() const {
            return node_;
        }

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

    private:
        Type node_;
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void PrintString(const std::string& value, std::ostream& out);

    void Print(const Document& doc, std::ostream& output);

    struct NodePrinter {
        std::ostream& out;

        void operator()(std::nullptr_t) {
            out << "null";
        }
        void operator()(int num) {
            out << num;
        }
        void operator()(double num) {
            out << num;
        }
        void operator()(std::string str) {
            out.put('"');
            PrintString(str, out);
            out.put('"');
        }
        void operator()(bool elem) {
            out << std::boolalpha << elem;
        }
        void operator()(Array arr) {
            out.put('[');
            for (size_t i = 0; i != arr.size(); ++i) {
                std::visit(NodePrinter{ out }, arr[i].GetNode());
                if (i != arr.size() - 1) {
                    out.put(',');
                }
            }
            out.put(']');
        }
        void operator()(Dict dict) {
            size_t i = 0;
            out.put('{');
            for (const auto& [key, value] : dict) {
                out.put('"');
                out << key << "\":";
                std::visit(NodePrinter{ out }, value.GetNode());
                if (i != dict.size() - 1) {
                    out.put(',');
                }
                ++i;
            }
            out.put('}');
        }
    };

    inline std::ostream& operator<<(std::ostream& out, Node node) {
        std::visit(NodePrinter{ out }, node.GetNode());
        return out;
    }

    inline bool operator==(const Node& lhs, const Node& rhs) {
        return lhs.GetNode() == rhs.GetNode();
    }

    inline bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    inline bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    inline bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

}  // namespace json