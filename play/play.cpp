

#include "pch.h"
#include <iostream>
#include "../dvector/dvector.h"

int main()
{
    dv::dvector<std::string> the_dvector;
    the_dvector.push_back("deep");
    the_dvector.push_back("dark");
    the_dvector.push_back("fantasies");

    std::cout << the_dvector[0] << "♂" << the_dvector[1] << "♂" << the_dvector[2] << std::endl;
    // deep♂dark♂fantasies

    the_dvector.erase(1);

    std::cout << the_dvector[0] << "♂" << the_dvector[1] << std::endl;
    // deep♂fantasies
}

