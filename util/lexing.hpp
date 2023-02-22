#pragma once

#include "common.h"

#include <map>

_UTIL_API

enum class default_token_types {
    left_paren,
    right_paren,
    left_brace,
    right_brace,
    comma,
    identifier,
    keyword,
    string,
    number
};

template<class TokenType>
struct token {
    TokenType type;
    struct {
        size_t start, end;
    } span;
    std::string lexeme;
};

template<typename Token = utl::default_token_types>
class lexer {
private:
    std::string_view source;
    std::vector<struct utl::token<Token>> tokens{};
    size_t cursor{ -1 };

    std::map<char, Token> matches{};
public:
    lexer(std::string_view _Src)
        : source(_Src)
    {}

    auto register_match(char _Match, Token _Arb) noexcept {
        matches
            .try_emplace(std::make_pair(_Match, _Arb));
    }

private:
    optional<char> current() const noexcept {
        if (cursor >= source.size()) {
            return {};
        }
        return source.at(cursor);
    }

    struct utl::token<Token> single() const noexcept {
        auto maybe_character = this->current();
        auto character = unwrap_or_panic(maybe_character);

        if (!matches.contains(character)) {
            vpanic("no match for character: {}", character);
        }

        auto tt = matches.at(character);
        auto tok = token<Token> {
            tt,
            {cursor, ++cursor},
            std::string(character)
        };

        return tok;
    }
};

_UTIL_API_END