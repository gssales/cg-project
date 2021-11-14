#include "graphics/texture.h"

texture_t ReadTextureFile(const char* filename)
{
  texture_t tex;
  stbi_set_flip_vertically_on_load(true);
  tex.data = stbi_load(filename, &tex.width, &tex.height, &tex.channels, 4);
  
  if ( tex.data == NULL )
  {
    std::cerr << "ERROR: Cannot open image file \"" << filename << "\"." << std::endl;
    throw std::runtime_error("Error cannot open image file");
  }

  return tex;
}