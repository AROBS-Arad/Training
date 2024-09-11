#ifndef UTIL_H
#define UTIL_H

#include <string>

void checkShaderCompilation(unsigned int shader, const std::string& type);

void checkProgramLinking(unsigned int program);

#endif
