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

    Tokenizer<lexer_type, const char*, size_t> tokenizer("keywords_reserved.txt");


    char const* first1 = f1.c_str();
    char const* last1 = &first1[f1.size()];
    char const* first2 = f2.c_str();
    char const* last2 = &first2[f2.size()];
    auto res1 = tokenizer.get_tokens(first1, last1);
    auto res2 = tokenizer.get_tokens(first2, last2);


}

bool Schleiermacher::has_flag(std::bitset<3> event, std::bitset<3> flag) {
    return false;
}


int Schleiermacher::match_score (std::string x, size_t type_x , std::string y, size_t type_y){

    if (type_x == type_y)
    {
        if (type_x == IDENTIFIER)
        {
            if (x==y) return id_matchScore;
            return id_misScore;
        }

        if (type_x == KEYWORD)
        {
            if (x==y) return kw_matchScore;
            return kw_misScore;
        }

        if (type_x == OPERATOR || type_x == PUNCTUATOR)
        {
            if (x==y) return op_matchScore;
            return op_misScore;
        }

        if (type_x == LITERAL)
        {
            if (x==y) return lit_matchScore;
            return lit_misScore;
        }

        if (type_x == NUMBER)
        {
            if (x==y) return num_matchScore;
            return num_misScore;
        }
    }
    return default_misScore;
}

int Schleiermacher::align() {

    int** scores = new int*[seq1.size()+1];
    for(size_t i = 0; i < seq1.size()+1; ++i)
        scores[i] = new int[seq2.size()+1];

    scores[0][0] = 0;

    auto** events = new std::bitset<3>*[seq1.size()+1];
    for(size_t i = 0; i < seq1.size()+1; ++i)
        events[i] = new std::bitset<3>[seq2.size()+1];



    unsigned long j;
    unsigned long i;

    for (i = 1; i <= seq1.size(); i++)
    {
        scores[i][0] = scores[i - 1][0] + nGap_score;
        events[i][0] = align_type.gap_y;
    }

    for (j = 1; j <= seq2.size(); j++)
    {
        scores[0][j] = scores[0][j - 1] + nGap_score;
        events[0][j] = align_type.gap_x;
    }


    for (i = 1; i <= seq1.size(); i++)
        for (j = 1; j <= seq2.size(); j++)
        {

            int m = scores[i-1][j-1] + match_score(seq1.at(i-1).value ,seq1.at(i-1).type,seq2.at(j-1).value,seq2.at(j-1).type);
            int g1 = scores[i-1][j] + nGap_score;
            int g2 = scores[i][j-1] + nGap_score;
            scores[i][j] = std::max(m,std::max(g1,g2));
            std::bitset<3> type = align_type.both;
            if (scores[i][j] == g1) type |= align_type.gap_y;
            if (scores[i][j] == g2) type |= align_type.gap_x;
            if (scores[i][j] > m ) type ^= align_type.both;
            events[i][j] = type;


        }

    i = seq1.size();
    j = seq2.size();

    while (i > 0 || j > 0)
    {
        if (has_flag(events[i][j],align_type.both) && i > 0 && j > 0)
        {
            seq1_aligned.push(seq1[--i].value);
            seq2_aligned.push(seq2[--j].value);
            if (seq1[i].value == seq2[j].value)
                str_match.push("=");
            else
                str_match.push("!");
        }
        else if (has_flag(events[i][j],align_type.gap_y) && i > 0)
        {
            seq1_aligned.push(seq1[--i].value);
            seq2_aligned.push(".");
            str_match.push("^");
        }
        else if (has_flag(events[i][j],align_type.gap_x) && j > 0)
        {
            seq1_aligned.push(".");
            seq2_aligned.push(seq2[--j].value);
            str_match.push("v");
        }

    }


    auto final_res = scores[seq1.size()][seq2.size()];
    for(size_t i = 0; i < seq1.size()+1; ++i)
    {
        delete[] scores[i];
        delete[] events[i];
    }

    delete[] scores;
    delete[] events;

    return final_res;
}

