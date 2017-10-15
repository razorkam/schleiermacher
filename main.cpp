#include "Schleiermacher.h"


int main(int argc, const char** argv)
{

    if (argc < 3)
    {
        std::cerr << "Files isn't specified";
        return -1;
    }

    std::string fname1 = argv[1];
    std::string fname2 = argv[2];

    Schleiermacher sch(fname1, fname2);
    sch.tokenize();
    std::cout << sch.align();

    return 0;
}