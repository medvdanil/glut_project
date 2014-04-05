#include <GL/glew.h>
#include <GL/glut.h>
#include <IL/il.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "loaders.h"

int printText(float x, float y, const char* format, ...){
	va_list arg;
	int done;
	va_start (arg, format);
	char* text=new char[1024];
	done = vsprintf (text, format, arg);
	va_end (arg); 

    glRasterPos2f(x, y);
    for(int i=0;text[i];i++)
        if(text[i]=='\n' || format[i]==13){
            y-=15;
            glRasterPos2f(x, y);
        }
        else
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
	delete[] text;
    return done;
}
void shaderLog(unsigned int shader) 
{ 
  int   infologLen   = 0;
  int   charsWritten = 0;
  char *infoLog;

  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);

  if(infologLen > 1)
  { 
    infoLog = new char[infologLen];
    if(infoLog == NULL)
    {
      std::cout<<"ERROR: Could not allocate InfoLog buffer\n";
    }
	else{
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
		std::cout<< "InfoLog: " << infoLog << "\n\n\n";
	}
    delete[] infoLog;
  }
}
void checkOpenGLerror()
{
  GLenum errCode;
  if((errCode=glGetError()) != GL_NO_ERROR)
	  std::cout << "OpenGl error! - " << gluErrorString(errCode) << std::endl;
}
GLuint loadTexture(const char* filename){
	GLuint img = ilGenImage();
	ilBindImage(img);
	if(ilLoadImage(filename))
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	else {
		printf("could not load image %s\n", filename);
		return -1;
	}
	GLuint ptex;
	glGenTextures(1,&ptex);
    glBindTexture(GL_TEXTURE_2D,ptex);
	if(filename[0] == 'e')
	gluBuild2DMipmaps(GL_TEXTURE_2D,ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, (void*)loadTexture);
	else
	gluBuild2DMipmaps(GL_TEXTURE_2D,ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);//GL_REPLACE) ;
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT) ;
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT) ;
	ilDeleteImage(img);
    //gluBuild2DMipmaps(GL_TEXTURE_2D,4, 4, 4,GL_RGBA, GL_UNSIGNED_BYTE, bits);
    return ptex;
}
GLuint loadShaderFromFile(const char* filename){
	bool vs=0, fs=0;
	GLuint shader;
	 for(int i = strlen(filename)-1;i>=0 && filename[i] != '.';i--)
		 vs |=filename[i] == 'v' || filename[i]=='V', 
		 fs |=filename[i] == 'f' || filename[i]=='F';
	 if(vs){
		 shader = glCreateShader(GL_VERTEX_SHADER);
		 std::cout << "vertex shader " << filename << ":\n";
	 }
	 else if(fs){
		 shader = glCreateShader(GL_FRAGMENT_SHADER);
		 std::cout << "fragment shader " << filename << ":\n";
	 }
	 else {
		 shader = glCreateShader(GL_GEOMETRY_SHADER);
		 std::cout << "geometry shader " << filename << ":\n";
	 }
	 FILE* f = fopen(filename, "r");
	 if(!f){
		 std::cout << "file " <<  filename << " not found\n"; 
		 return 0;
	 }
	 fseek(f,0,SEEK_END);
	 long flen = ftell(f),len;
	 GLchar *src = new GLchar[flen+2];
	 len=0;
	 rewind(f);
	 while(fgets(src+len, flen, f)){
		len+=strlen(src+len);
		//src[len++] = '\n';
		src[len]=0;
	 }
	 fclose(f);
	 const GLchar *usrc = (const GLchar*)src;
	 glShaderSource(shader, 1, (const GLchar**)(&src), NULL);
	 glCompileShader(shader);
	 shaderLog(shader);
	 delete[] src;
	 return shader;
}
GLuint loadShaderProgram(const char* vert, const char* frag){

	
  GLuint vShader, fShader, Program;
  vShader = loadShaderFromFile(vert);
  fShader = loadShaderFromFile(frag);
  Program = glCreateProgram();
  glAttachShader(Program, vShader);
  glAttachShader(Program, fShader);

  glLinkProgram(Program); 

  glDetachShader(Program, vShader);
  glDetachShader(Program, fShader);

  int link_ok;
  glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
  if(!link_ok)
  {
    std::cout << "error attach shaders \n";
    return 0;
  }
  return Program;
}
GLint loadUniform(GLuint Program, const char* name){	
	GLint res = glGetUniformLocation(Program, name);
	if(res == -1)
		printf("could not bind uniform %s\n", name);
	return res;
}