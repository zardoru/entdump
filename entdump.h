#pragma once

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif

#include <filesystem>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include "bsp.h"

enum errortype_t {
    ERR_CONTINUE = 0,
    ERR_DROP = 1
};

void Com_Error(errortype_t code, const char *fmt, ...);

void print_bsp_entities(lump_t lump, std::vector<uint8_t> &buf, const char *filename);

void print_bsp_textures(lump_t lump, std::vector<uint8_t> &buf, const char *filename);

int read_bsp(FILE *in, dheader_t &header, std::vector<uint8_t> &buf, const char *filename);

int print_textures_of_maps_in_directory(char *filepath);