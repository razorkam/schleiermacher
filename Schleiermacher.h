//
// Created by uroboros on 12.10.17.
//

#ifndef SCHLEIERMACHER_SCHLEIERMACHER_H
#define SCHLEIERMACHER_SCHLEIERMACHER_H

#include "Tokenizer.h"





class Schleiermacher {

    Schleiermacher(const std::string& fname1, const std::string& fname2);
private:
    std::string f1, f2;

    void tokenize();


};


#endif //SCHLEIERMACHER_SCHLEIERMACHER_H
