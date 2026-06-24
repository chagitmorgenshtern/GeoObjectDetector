#pragma once

#include <string>
#include <map>
#include <stdexcept>
#include <cctype>

namespace SimpleJson {

struct Value {
    enum Type { Null, Number, String, Object } type;
    double number;
    std::string string;
    std::map<std::string, Value> object;

    Value() : type(Null), number(0.0) {}
    explicit Value(double v) : type(Number), number(v) {}
    explicit Value(const std::string& s) : type(String), number(0.0), string(s) {}

    bool isObject() const { return type == Object; }
    bool isString() const { return type == String; }
    bool isNumber() const { return type == Number; }
};

class Parser {
public:
    Parser(const std::string& s) : text(s), pos(0) {}

    Value parse() {
        skipWhitespace();
        Value value = parseValue();
        skipWhitespace();
        if (pos != text.size()) {
            throw std::runtime_error("Unexpected characters after JSON value");
        }
        return value;
    }

private:
    const std::string& text;
    size_t pos;

    void skipWhitespace() {
        while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
            pos++;
        }
    }

    Value parseValue() {
        if (pos >= text.size()) {
            throw std::runtime_error("Unexpected end of JSON");
        }
        char c = text[pos];
        if (c == '{') return parseObject();
        if (c == '"') return Value(parseString());
        if (c == 'n') return parseNull();
        if (c == '-' || (c >= '0' && c <= '9')) return parseNumber();
        throw std::runtime_error(std::string("Invalid JSON value at position ") + std::to_string(pos));
    }

    Value parseObject() {
        Value result;
        result.type = Value::Object;
        pos++; // skip '{'
        skipWhitespace();
        if (pos < text.size() && text[pos] == '}') {
            pos++;
            return result;
        }
        while (true) {
            skipWhitespace();
            std::string key = parseString();
            skipWhitespace();
            if (pos >= text.size() || text[pos] != ':') {
                throw std::runtime_error("Missing ':' in object");
            }
            pos++;
            skipWhitespace();
            Value value = parseValue();
            result.object[key] = value;
            skipWhitespace();
            if (pos >= text.size()) {
                throw std::runtime_error("Unterminated object");
            }
            if (text[pos] == '}') {
                pos++;
                break;
            }
            if (text[pos] != ',') {
                throw std::runtime_error("Expected ',' in object");
            }
            pos++;
        }
        return result;
    }

    std::string parseString() {
        if (text[pos] != '"') {
            throw std::runtime_error("Expected '\"' at start of string");
        }
        pos++;
        std::string result;
        while (pos < text.size()) {
            char c = text[pos++];
            if (c == '"') {
                return result;
            }
            if (c == '\\') {
                if (pos >= text.size()) {
                    break;
                }
                char esc = text[pos++];
                switch (esc) {
                    case '"': result.push_back('"'); break;
                    case '\\': result.push_back('\\'); break;
                    case '/': result.push_back('/'); break;
                    case 'b': result.push_back('\b'); break;
                    case 'f': result.push_back('\f'); break;
                    case 'n': result.push_back('\n'); break;
                    case 'r': result.push_back('\r'); break;
                    case 't': result.push_back('\t'); break;
                    default: result.push_back(esc); break;
                }
            } else {
                result.push_back(c);
            }
        }
        throw std::runtime_error("Unterminated string");
    }

    Value parseNull() {
        const std::string token = "null";
        if (text.compare(pos, token.size(), token) == 0) {
            pos += token.size();
            return Value();
        }
        throw std::runtime_error("Invalid token");
    }

    Value parseNumber() {
        size_t start = pos;
        if (text[pos] == '-') {
            pos++;
        }
        while (pos < text.size() && std::isdigit(static_cast<unsigned char>(text[pos]))) {
            pos++;
        }
        if (pos < text.size() && text[pos] == '.') {
            pos++;
            while (pos < text.size() && std::isdigit(static_cast<unsigned char>(text[pos]))) {
                pos++;
            }
        }
        std::string token = text.substr(start, pos - start);
        double value = std::stod(token);
        return Value(value);
    }
};

inline Value parse(const std::string& text) {
    Parser parser(text);
    return parser.parse();
}

inline bool hasKey(const Value& obj, const std::string& key) {
    if (!obj.isObject()) return false;
    return obj.object.find(key) != obj.object.end();
}

inline const Value& get(const Value& obj, const std::string& key) {
    auto it = obj.object.find(key);
    if (it == obj.object.end()) {
        throw std::runtime_error("Missing key: " + key);
    }
    return it->second;
}

inline std::string stringify(const std::string& value) {
    std::string result = "\"";
    for (char c : value) {
        if (c == '"' || c == '\\') {
            result.push_back('\\');
            result.push_back(c);
        } else if (c == '\n') {
            result += "\\n";
        } else {
            result.push_back(c);
        }
    }
    result.push_back('"');
    return result;
}

} // namespace SimpleJson
