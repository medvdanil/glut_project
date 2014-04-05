int printText(float x, float y, const char* format, ...);
void shaderLog(unsigned int shader) ;
void checkOpenGLerror();
GLuint loadTexture(const char* filename);
unsigned int loadShaderFromFile(const char* filename);
GLuint loadShaderProgram(const char* vert, const char* frag);
GLint loadUniform(GLuint Program, const char* name);