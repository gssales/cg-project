#include "loaders.h"

std::string ReadFileContent(const char* filename)
{
  std::ifstream file;
  try {
    file.exceptions(std::ifstream::failbit);
    file.open(filename);
  } catch ( std::exception& e ) {
    std::cerr << "ERROR: Cannot open file \"" << filename << "\"." << std::endl;
    throw e;
  }
  std::stringstream file_stream;
  file_stream << file.rdbuf();
  return file_stream.str();
}


