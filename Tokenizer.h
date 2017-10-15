//
// Created by uroboros on 11.10.17.
//

#ifndef SCHLEIERMACHER_TOKENIZER_H
#define SCHLEIERMACHER_TOKENIZER_H



#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_algorithm.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <set>
#include <fstream>
#include <string>
#include <utility>

namespace lex = boost::spirit::lex;


enum tok_type {IDENTIFIER = lex::min_token_id , KEYWORD,  OPERATOR , PUNCTUATOR , LITERAL, NUMBER};

template <typename T>
struct token
{
    token(std::string value, const T& type): value(std::move(value)), type(type) {}
    std::string value;
    T type;
};


struct token_add_action
{
    template <typename TokenStorage>
    struct result
    {
        typedef void type;
    };

    template <typename TokenStorage, typename IdType>
    void operator()(TokenStorage& s, const IdType& id, const std::set<std::string>& kw_set) const
    {
        using boost::spirit::lex::_start;
        using boost::spirit::lex::_end;
        using boost::spirit::lex::_tokenid;
        std::string value;
        IdType token_id = id;
        boost::phoenix::ref(value) = boost::phoenix::construct<std::string>(_start+1, _end);
        if (token_id == IDENTIFIER && (kw_set.find(value) != kw_set.end())){
            token_id = KEYWORD;
        }
        token<IdType> tok(value, token_id);
        s.push_back(tok);
    }

    void operator()() const // discard generated token
    {

    }
};


template <typename Lexer, typename IterType, typename IdType>
class Tokenizer : lex::lexer<Lexer> {

public:
    explicit Tokenizer(const std::string& kw_filename):
            IDENTIFIER("[a-zA-Z_][a-zA-Z0-9_]*"),
            OPERATOR("[\\(\\)\\{\\}\\[\\]]|[\\+-\\*/=<>.:%!&\\|~\\^\\?]{1,2}"), //simplified (for now)
            PUNCTUATOR("[,#;]"),
            LITERAL("[\\\"'<].*[\\\"'>]"),
            NUMBER("[\\+-\\.0-9].*[a-fA-FuUlL0-9]"),  //leave testing number literal correctness to C++ compiler
            //  , just detect it
            COMMENT("\\/\\*(.*?)\\*\\/|\\/\\/.*"),
            WHITESPACE("\\s+")
    {
        using boost::spirit::lex::_tokenid;
        read_keywords(kw_filename);
        this->self =
                IDENTIFIER [add(result, _tokenid, kw_set)]
                | OPERATOR [add(result, _tokenid, kw_set)]
                | PUNCTUATOR [add(result, _tokenid, kw_set)]
                | LITERAL [add(result, _tokenid, kw_set)]
                | NUMBER [add(result, _tokenid, kw_set)]
                | COMMENT [add()]
                | WHITESPACE [add()];
    }

    lex::token_def<> IDENTIFIER, OPERATOR, PUNCTUATOR, LITERAL,
            NUMBER, COMMENT, WHITESPACE;

private:
    std::set<std::string> kw_set;
    std::vector<token<IdType>> result;
    boost::phoenix::function<token_add_action> add;

    void read_keywords(const std::string& filename){
        std::ifstream kw_list;
        std::string extracted;
        kw_list.open(filename);
        while (std::getline(kw_list,extracted))
            kw_set.insert(extracted);

        kw_list.close();
    }

public:
    std::vector<token<IdType>> get_tokens(IterType first,
                                                      IterType last){
        auto iter = this->begin(first, last);
        auto end = this->end();

        while (iter != end && token_is_valid(*iter))
            ++iter;

        return result;
    }
};


#endif //SCHLEIERMACHER_TOKENIZER_H
