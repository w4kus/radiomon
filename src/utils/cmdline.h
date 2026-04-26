// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#pragma once

#include <string>
#include <vector>
#include <complex>
#include <stdio.h>
#include "rm-math.h"

namespace util {

char *getCmdOption(char ** begin, char ** end, const std::string& option);
bool cmdOptionExists(char** begin, char** end, const std::string& option);

void printComplex(FILE *f, std::vector<rm_math::complex_f> &vals);
void printComplex(FILE *f, size_t num, const rm_math::complex_f *vals);
void printReal(FILE *f, std::vector<float>  &vals);
void printReal(FILE *f, size_t num, const float *vals);
}
