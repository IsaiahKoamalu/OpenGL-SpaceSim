#pragma once
#include <string>
#include <vector>
#include <cstdio>
#include "external/stb_image.h"

struct LoadedImage
{
    std::vector<unsigned char> pixels;
    int w=0, h=0;
    bool ok() const{ return !pixels.empty() && w>0 && h>0; }
};

inline LoadedImage loadImageRGBA(const std::string& path)
{
    LoadedImage out;
    int w,h,channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);
    if (!data)
    {
        std::fprintf(stderr, "[stb_image] Failed to load: %s\n", path.c_str());
        return out;
    }
    out.w = w;
    out.h = h;
    out.pixels.assign(data, data + (w * h * 4));
    stbi_image_free(data);
    return out;
}