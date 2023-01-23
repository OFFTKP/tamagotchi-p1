#ifndef FRONTEND_HXX
#define FRONTEND_HXX
#include <string>
#include <GL/glew.h>
#include "tama.hxx"

class Frontend {
public:
    Frontend();
    void Draw();
    void Load(const std::string&);
private:
    Tama tama_;
    GLuint texture_;
};
#endif