#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <iostream>
#include <math.h>
#include <stdint.h>
#include <stb\stb_image.h>

typedef struct
{
  uint8_t *data;
  int width, height;
  int channels;
} texture_t;

texture_t ReadTextureFile(const char* filename);

texture_t* GenerateMipmaps(texture_t texture);

#endif // _TEXTURE_H