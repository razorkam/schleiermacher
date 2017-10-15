//
// Created by uroboros on 12.10.17.
//

#ifndef SCHLEIERMACHER_SCHLEIERMACHER_H
#define SCHLEIERMACHER_SCHLEIERMACHER_H

#include "Tokenizer.h"
#include <bitset>


struct alignment

{

    std::bitset<3> both;
    std::bitset<3> gap_x;
    std::bitset<3> gap_y;

    alignment()
    {
        both = 1;
        gap_x = 2;
        gap_y = 4;
    }

};


class Schleiermacher {

public:
    Schleiermacher(const std::string& fname1, const std::string& fname2);

private:
    std::string f1, f2;
    alignment align_type;
    int nGap_score;
    int id_matchScore,id_misScore,kw_matchScore,kw_misScore,
            op_matchScore,op_misScore,lit_matchScore,lit_misScore,
            num_matchScore,num_misScore;
    int default_misScore;
    std::stack<std::string> seq1_aligned, seq2_aligned,str_match;
    std::vector<token<size_t>> seq1, seq2;
    bool has_flag(std::bitset<3> event, std::bitset<3> flag);
    int match_score (std::string x, size_t type_x , std::string y, size_t type_y);


public:
    void tokenize();
    int align();


};


#endif //SCHLEIERMACHER_SCHLEIERMACHER_H
