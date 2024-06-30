#ifndef     VK_LOAD_SHADER_H
#define     VK_LOAD_SHADER_H

#include    <vector>
#include    <string>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
std::vector<char> readSPV(const std::string &filename);

#endif
