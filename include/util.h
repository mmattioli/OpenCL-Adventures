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
    inline std::string loadProgram(std::string input) {
        std::ifstream stream(input.c_str());
        if (!stream.is_open()) {
            std::cerr << "Cannot open file: " << input << std::endl;
            exit(1);
        }
        return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
    }
    
}

#endif
