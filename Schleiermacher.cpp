//
// Created by uroboros on 12.10.17.
//
#include "Tokenizer.h"
#include "Schleiermacher.h"

Schleiermacher::Schleiermacher(const std::string& fname1, const std::string& fname2)
{
    std::ifstream file1(fname1), file2(fname2);
    std::stringstream buf1, buf2;
    buf1 << file1.rdbuf();
    buf2 << file2.rdbuf();
    f1 = buf1.str();
    f2 = buf2.str();
    file1.close();
    file2.close();
}

void Schleiermacher::tokenize() {
    typedef lex::lexertl::token<const char*, lex::omit, boost::mpl::false_>
            token_type;

    typedef lex::lexertl::actor_lexer<token_type> lexer_type;

    Tokenizer<lexer_type> tokenizer("keywords_reserved.txt");



    char const* first = f1.c_str();
    char const* last = &first[f1.size()];
    lex::tokenize(first, last, tokenizer);
//    tokenizer.get_tokens(first, last);


}
