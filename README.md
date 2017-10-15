# schleiermacher
Schleiermacher is an automatic source code plagiarism detection tool

It's using Needleman-Wunsch DP algorithm to compute plagiarism
coefficient of two source code snippets. The main idea is to define
different values for penalties adding if mismatching of different type
language tokens was occured and then align two snippets tokens
according to general alignment procedure doing for tokens.

Old version was created in 2015 and was successfully tested on my
costudent's code written by them passing Algorithms course assignments.
For now this version (which is very naive and dirty) is storing in
*old/*
directory.

Modern version is under development in root directory. It includes Boost
to define simplified version of C++ grammar and it's tokens
and effectively tokenize it.
