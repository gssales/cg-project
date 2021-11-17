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

texture_t* GenerateMipmaps(texture_t texture)
{
  int max_level = std::floor(std::log2(texture.width));
  texture_t *mipmaps = new texture_t[max_level];
  mipmaps[0] = texture;
  int size = texture.width;
  for (int i = 1; i < max_level; i++)
  {
    size /= 2;
    texture_t new_level;
    new_level.data = new uint8_t[4 * size * size];
    new_level.height = size;
    new_level.width = size;
    new_level.channels = texture.channels;

    uint8_t *window;
    for (int l = 0; l < size; l++)
      for (int c = 0; c < size; c++)
      {
        int index = l*size + c;
        double value[3] = { 0.0, 0.0, 0.0 };
        for (int ol = 0; ol < 2; ol++)
          for (int oc = 0; oc < 2; oc++)
          {
            int w_index = (2*l+ol) * mipmaps[i-1].width + (2*c+oc);
            window = mipmaps[i-1].data + w_index * 4;
            for (int g = 0; g < 3; g++)
              value[g] += (double)window[g];
          }
        for (int g = 0; g < 3; g++)
          new_level.data[4*index + g] = std::floor(value[g] / 4.0);
        new_level.data[4*index + 3] = 255;
      }
    
    mipmaps[i] = new_level;
  }
  return mipmaps;
}