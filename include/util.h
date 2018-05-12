/*
 *
 * Written by Michael Mattioli
 *
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <iostream>
#include <fstream>

namespace util {

    /*
     * Used to load kernel source files.
     */
    inline std::string LoadProgram(std::string fileName) {
        std::ifstream f(fileName.c_str());
        if (!f.is_open()) {
            std::cerr << "Cannot open file: " << fileName << std::endl;
            exit(1);
        }
        return std::string(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
    }

}

#endif
