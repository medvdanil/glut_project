#include <GL/glew.h>
#include <GL/glut.h>
#include <IL/il.h>
#include "loaders.h"
#include "primitive.h"
#include "common.h"
#include "light.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#define log std::cout

pthread_t threadCompute;
const unsigned int timerDelay = 16;
timespec ts_delay={0,1000000*timerDelay};
float VisionRange = 2000;
int idxMainWindow;
const float skyeps=0;
GLuint Program, spShadow;
GLint  Unif_texture;
GLint  Unif_lightnum;
GLint 	Unif_lpos, Unif_ldir, Unif_lbright, Unif_lprojMatrix, Unif_depthtex;
GLuint VBO[16];
float camMat[16], invcamMat[16];
vec3_t camp;
SinCos camrtDelta(0.02);
float camvelc=0.1;
bool cursorCam = 0;
float mouseSpeed = 0.005;
int curx=0, cury=0;
const GLsizei texn = 16;
GLuint tex[texn];
GLuint samplerShadow;
const int lightnum=2;
Light light[lightnum];
//light[0](vec3_t(5, 1.1, 3), vec3_t(-5,-1.1,-3));
SinCos cubeang(0);
bool down[256];
int specoff = 128;
vec3_t block[256*8];
int blockn=0;
vec3_t cubeVert[] = {
	vec3_t( 0.5f, 0.5f, 0.5f),
	vec3_t( 0.5f, 0.5f,-0.5f),
	vec3_t( 0.5f,-0.5f, 0.5f),
	vec3_t( 0.5f,-0.5f,-0.5f),
	vec3_t(-0.5f, 0.5f, 0.5f),
	vec3_t(-0.5f, 0.5f,-0.5f),
	vec3_t(-0.5f,-0.5f, 0.5f),
	vec3_t(-0.5f,-0.5f,-0.5f)
};
vec3_t cubeNormal[] = { 
	vec3_t(-1, -1, -1), vec3_t(1, -1, -1), 
	vec3_t(1,  1, -1), vec3_t(-1,  1, -1), 
	vec3_t(-1, -1,  1), vec3_t(1, -1,  1), 
	vec3_t(1,  1,  1), vec3_t(-1,  1,  1)
};
float cubeTexCrd[][2] = { 
	{0,0}, {1, 0}, {0,0}, {1, 0},
	{0,1}, {1, 1}, {0,1}, {1, 1},
};
const unsigned int cubeNumIdx = 24;
const unsigned char skyidx[24]={0,1,3,2, 4,5,1,0, 6,7,5,4, 2,3,7,6, 2,6,4,0, 7,3,1,5};
const unsigned char cubeIdx[24]={0,2,3,1, 4,0,1,5, 6,4,5,7, 2,6,7,3, 2,0,4,6, 7,5,1,3};

void initShadows(){
	for(int i=0;i<lightnum;i++)
		light[i].init();
	light[1].pos = vec3_t(5, 1.1, 3);
	light[1].dir = vec3_t(-5,-1.1,-3);
	light[0].pos = vec3_t(4, 2, 3);
	light[0].dir = vec3_t(-4,-2,-3);
	light[0].bright = vec3_t(1.8,1.8,1.8);	
	light[1].bright = vec3_t(2,2,2);
	
	glGenSamplers(1, &samplerShadow);
	glSamplerParameteri(samplerShadow, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(samplerShadow, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(samplerShadow, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(samplerShadow, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(samplerShadow, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(samplerShadow, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16); 
}
void initGL(){
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_POLYGON_SMOOTH);
	initShadows();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluLookAt(4,0,2,0,0,2,0,0,1);
	glGetFloatv(GL_PROJECTION_MATRIX, camMat);	
	memset(down,0,sizeof(down));
}
void initShader()
{

	Program = loadShaderProgram("test.vert", "test.frag");
	spShadow = loadShaderProgram("depth.vert", "depth.frag");
	Unif_texture = loadUniform(Program, "texture");
	Unif_lightnum = loadUniform(Program, "lightnum");
	Unif_lpos = loadUniform(Program, "lpos");
	Unif_ldir = loadUniform(Program, "ldir");
	Unif_lbright = loadUniform(Program, "lbright");
	Unif_lprojMatrix = loadUniform(Program, "lprojMatrix");
	Unif_depthtex = loadUniform(Program, "depthtex");
	checkOpenGLerror();
}
void initTextures(){
	tex[1]=loadTexture("skies/yavin_lf.jpg");
	tex[2]=loadTexture("skies/yavin_ft.jpg");
	tex[3]=loadTexture("skies/yavin_rt.jpg");
	tex[4]=loadTexture("skies/yavin_bk.jpg");
	tex[5]=loadTexture("skies/yavin_up.jpg");
	tex[6]=loadTexture("skies/yavin_dn.jpg");	
	tex[7]=loadTexture("empty.bmp");
	tex[8]=loadTexture("asphalt.jpg");
    checkOpenGLerror();
}
void initVBO()
{
  for(int i=0;i<8;i++)
	  cubeNormal[i].normfast();
  glGenBuffers(16, VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVert), cubeVert, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCrd), cubeTexCrd, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormal), cubeNormal, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glNewList(1, GL_COMPILE);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glVertexPointer(3,GL_FLOAT, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glTexCoordPointer(2,GL_FLOAT, 0, 0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
		glNormalPointer(GL_FLOAT, 0, 0);
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
		glDrawElements(GL_QUADS, cubeNumIdx, GL_UNSIGNED_BYTE, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEndList();
  checkOpenGLerror();
}

void freeShader()
{
  //! Передавая ноль, мы отключаем шейдрную программу
  glUseProgram(0); 
  //! Удаляем шейдерную программу
  glDeleteProgram(Program);
}

void freeBuffers()
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(2, VBO);
  for(int i=0;i<lightnum;i++){
	  glDeleteFramebuffers(1, (unsigned int*)&light[i].fbShadow);
	  glDeleteRenderbuffers(1, (unsigned int*)&light[i].rbShadow);
	  glDeleteTextures(1, (unsigned int*)&light[i].texShadow);
  }
  //glDeleteTextures(texn, tex);
}

void ResizeWindow(int width, int height)
{
	glViewport(0, 0, width, height);
}
void drawGeometry(bool shadowmap = 0){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(!shadowmap){
		glEnable (GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,tex[7]);
	}
	for(int i=0;i<blockn;i++){
		glPushMatrix();
		glTranslatef(block[i].x, block[i].y, block[i].z);
		//glScalef(1+float(((s>>0)&0x1F)-0xF)/0xFF, 1+float(((s>>5)&0x1F)-0xF)/0xFF, 1+float(((s>>10)&0x1F)-0xF)/0xFF);
		//glRotatef(0.4, (r&0x1F)-0xF,((r>>5)&0x1F)-0xF,((r>>10)&0x1F)-0xF);
	    //glRotatef(7.2, cubeang.Cos,cubeang.Sin,1);
		//glutSolidCube(2);
		glCallList(1);
		glPopMatrix();
	}
	glPushMatrix();
	glTranslatef(0, 0, 1);
	for(int i=0;i<50;i++){
		//glScalef(1+float(((s>>0)&0x1F)-0xF)/0xFF, 1+float(((s>>5)&0x1F)-0xF)/0xFF, 1+float(((s>>10)&0x1F)-0xF)/0xFF);
		//glRotatef(0.4, (r&0x1F)-0xF,((r>>5)&0x1F)-0xF,((r>>10)&0x1F)-0xF);
	    glRotatef(7.2, cubeang.Cos,cubeang.Sin,1);
		//glutSolidCube(2);
		glCallList(1);
	}
	glPopMatrix();



	glBindTexture(GL_TEXTURE_2D, tex[8]);
	glBegin(GL_QUADS);
	glNormal3f(0,0,1);
	glTexCoord2i(-100,-100);
	glVertex2i(-1000,-1000);
	glTexCoord2i(100,-100);
	glVertex2i(1000,-1000);
	glTexCoord2i(100,100);
	glVertex2i(1000,1000);
	glTexCoord2i(-100,100);
	glVertex2i(-1000,1000);
	glEnd();
	glPushMatrix();
	glTranslatef(2,0,0);	
	glRotatef(90, 1,0,0);
    glBindTexture(GL_TEXTURE_2D,tex[8]);
	glFrontFace(GL_CW);
	//glutSolidTeapot(1);
	glFrontFace(GL_CCW);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0,2,1.5);	
	glutSolidSphere(0.7, 50, 50);
	glTranslatef(2,-2,-1.5);
	glPopMatrix();
	if(!shadowmap)
		glDisable (GL_TEXTURE_2D);
}
void mapShadows(){
	
	
	for(int i=0;i<lightnum;i++){
		glBindFramebuffer(GL_FRAMEBUFFER, light[i].fbShadow);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
			std::cout << "framebuffer error " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
			return;
		}
		glViewport(0,0,light[i].sMapW, light[i].sMapH); 
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glUseProgram(spShadow);
		glPolygonOffset(2.0, 500.0);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float AspectRatio = float(light[i].sMapW)/ light[i].sMapH;
		gluPerspective(90,AspectRatio,0.1,light[i].shadowRange);
		gluLookAt(light[i].pos.x, light[i].pos.y, light[i].pos.z, 
			light[i].pos.x+light[i].dir.x,  light[i].pos.y+light[i].dir.y, light[i].pos.z+light[i].dir.z,
			light[i].dir.y, -light[i].dir.x, -light[i].dir.z);
		glGetFloatv(GL_PROJECTION_MATRIX, light[i].projMatrix);	
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(0.5, 0.5, 0.5); // + 0.5
		glScalef(0.5, 0.5, 0.5); // * 0.5
		glMultMatrixf(light[i].projMatrix);
		glGetFloatv(GL_PROJECTION_MATRIX, light[i].projMatrix);
		glPopMatrix();

		drawGeometry();
		glDisable(GL_POLYGON_OFFSET_FILL);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(0);
	}
}
void Render()
{
	unsigned tk = clock();
	mapShadows();
	printf("mapShadows() %dms\n", clock() - tk);tk = clock();
	glViewport(0,0,glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT)); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float AspectRatio = float(glutGet(GLUT_WINDOW_WIDTH))/glutGet(GLUT_WINDOW_HEIGHT);
	//glFrustum(-AspectRatio,AspectRatio,-1,1,1,VisionRange);
	gluPerspective(90,AspectRatio,0.1,VisionRange);	
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	vec3_t trl = -multPointMatrix(vec3_t(), camMat);
	float bufm[16];
	memcpy(bufm, camMat, sizeof(bufm));
	matTranslate(bufm, trl);
	glMultMatrixf(bufm);
	//glTranslatef(trl.x, trl.y, trl.z);
    glEnable (GL_TEXTURE_2D);
    for(int i=0;i<6;i++){
        glBindTexture(GL_TEXTURE_2D,tex[i+1]);
        glBegin(GL_QUADS);
            glTexCoord2f(0+skyeps,0+skyeps);
            glVertex3fv(cubeVert[skyidx[i*4]].ptr());
            glTexCoord2f(0+skyeps,1-skyeps);
            glVertex3fv(cubeVert[skyidx[i*4+1]].ptr());
            glTexCoord2f(1-skyeps,1-skyeps);
            glVertex3fv(cubeVert[skyidx[i*4+2]].ptr());
            glTexCoord2f(1-skyeps,0+skyeps);
            glVertex3fv(cubeVert[skyidx[i*4+3]].ptr());
        glEnd();
    }
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	
	glMultMatrixf(camMat);

	glEnable(GL_DEPTH_TEST);
	glUseProgram(Program);
	glUniform1i(Unif_texture, 0);
	for(int i=0;i<lightnum;i++){
		glUniform3fv(Unif_lpos+i, 1, light[i].pos.ptr());
		glUniform3fv(Unif_ldir+i, 1, light[i].dir.ptr());
		glUniform3fv(Unif_lbright+i, 1, light[i].bright.ptr());
		glUniformMatrix4fv(Unif_lprojMatrix+i, 1, false, light[i].projMatrix);
		//glUniform1i(Unif_light+7*i+4, light[i].sMapW);
		//glUniform1i(Unif_light+7*i+5, light[i].sMapH);
		
		glUniform1i(Unif_depthtex+i, i+1);
		glActiveTexture(GL_TEXTURE0+i+1);
		glBindTexture(GL_TEXTURE_2D, light[i].texShadow);
		glBindSampler(i+1, samplerShadow);
	}
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(Unif_lightnum, lightnum);
	
	printf("matrix&sky %dms\n", clock() - tk);tk = clock();
	drawGeometry();
	printf("drawGeometry() %dms\n", clock() - tk);tk = clock();
	puts("");
	glUseProgram(0);
	for(int i=0;i<lightnum;i++){
		glPushMatrix();
		glTranslatef(light[i].pos.x, light[i].pos.y, light[i].pos.z);
		glColor3fv(light[i].bright.ptr());
		glutSolidSphere(0.2,10,10);
		glPopMatrix();
	}
	checkOpenGLerror();
	glutSwapBuffers();
}

void SpecKeyUp(int key, int x, int y){
	down[key+specoff]=0;
}
void KeyboardUp(unsigned char key, int x, int y){
	down[key]=0;
}
void SpecKey(int key, int x, int y){
	down[key+specoff]=1;    
}
void Keyboard(unsigned char key, int x, int y){
	down[key]=1;
	switch(key){
		case 'A':
		case 'a':
			light[0].pos.xyangptr()->operator -=(camrtDelta);
			light[0].dir.xyangptr()->operator -=(camrtDelta);
			break;
		case 'D':
		case 'd':
			light[0].pos.xyangptr()->operator +=(camrtDelta);
			light[0].dir.xyangptr()->operator +=(camrtDelta);
			break;
		case 'Z':
		case 'z':
			light[1].pos.xyangptr()->operator -=(camrtDelta);
			light[1].dir.xyangptr()->operator -=(camrtDelta);
			break;
		case 'C':
		case 'c':
			light[1].pos.xyangptr()->operator +=(camrtDelta);
			light[1].dir.xyangptr()->operator +=(camrtDelta);
			break;
		case 'F':
		case 'f':
			if(glutGet(GLUT_SCREEN_HEIGHT) == glutGet(GLUT_WINDOW_HEIGHT) &&
				glutGet(GLUT_SCREEN_WIDTH) == glutGet(GLUT_WINDOW_WIDTH))
					glutReshapeWindow(800, 600);
			else 
				glutFullScreen();
			break;
		case 'P':
		case 'p':
			cursorCam = !cursorCam;
			if(cursorCam){
				glutSetCursor(GLUT_CURSOR_NONE);
				glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)/2);
			}
			else
				glutSetCursor(GLUT_CURSOR_INHERIT);
			break;
		case 'B':
		case 'b':{
			vec3_t pos = multPointMatrix(vec3_t(0,0,-2), invcamMat);
			pos.x = int(pos.x)+0.5, pos.y = int(pos.y)+0.5, pos.z = int(pos.z)+0.5;
			block[blockn++] = pos;	
		}
			break;
		case 27:
			glutDestroyWindow(idxMainWindow);
			break;
	}
	if(blockn){
		if(key == 't' || key == 'T')		
			block[blockn-1].y+=1;
		if(key == 'g' || key == 'G')		
			block[blockn-1].y-=1;
		if(key == 'h' || key == 'H')		
			block[blockn-1].x+=1;
		if(key == 'f' || key == 'F')		
			block[blockn-1].x-=1;		
		if(key == 'y' || key == 'Y')		
			block[blockn-1].z+=1;
		if(key == 'r' || key == 'R')		
			block[blockn-1].z-=1;
	}
		
	
    
}
void Mouse(int key, int down, int x, int y){

}
void MouseMotion(int x, int y){	
	curx = x, cury = y;
		
}
void* Compute(void* param){	
  while(1){
	int w=glutGet(GLUT_WINDOW_WIDTH)/2, h=glutGet(GLUT_WINDOW_HEIGHT)/2;
	cubeang+=SinCos(0.005);
	if(down[GLUT_KEY_LEFT+specoff])
		matRotate(camMat, -camrtDelta, vec3_t(0,1,0));
	if(down[GLUT_KEY_RIGHT+specoff])
		matRotate(camMat, camrtDelta, vec3_t(0,1,0));
	if(down[GLUT_KEY_UP+specoff])
		matTranslate(camMat, vec3_t(0,0,camvelc));
	if(down[GLUT_KEY_DOWN+specoff])
		matTranslate(camMat, vec3_t(0,0,-camvelc));
	if(down[GLUT_KEY_PAGE_UP+specoff])
		matRotate(camMat, -camrtDelta, vec3_t(1,0,0));
	if(down[GLUT_KEY_PAGE_DOWN+specoff])
		matRotate(camMat, camrtDelta, vec3_t(1,0,0));	
	if(cursorCam && !(curx == w && cury == h)){	
		glutWarpPointer(w, h);	
		matRotate(camMat, SinCos((curx-w)*mouseSpeed), vec3_t(0,1,0));
		matRotate(camMat, SinCos((cury-h)*mouseSpeed), vec3_t(1,0,0));
	}
	pthread_delay_np(&ts_delay);
  }
	inverseMatrix(invcamMat, camMat);

	return 0;
}
void Timer(int val){
	glutTimerFunc(timerDelay, Timer, 0);
    //SpecKey(GLUT_KEY_LEFT,0,0);
	//Keyboard('a', 0, 0);
	//Compute();
	glutPostRedisplay();
}
int main( int argc, char **argv )
{
	srand(time(0));
	freopen("out.log", "w", stdout);
	std::cout << "Starting\n";
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);// | GLUT_ALPHA
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	idxMainWindow = glutCreateWindow("Simple shaders");

	GLenum glew_status = glewInit();
	if(GLEW_OK != glew_status) 
	{
		std::cout << "Error: "  << "\n";
		return 1;
	}

	if(!GLEW_VERSION_2_0) 
	{
		std::cout << "No support for OpenGL 2.0 found\n";
		return 1;
	}
	initGL();
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		std::cout << "Need OpenIL ver." << IL_VERSION << "\n";
		return 1;
	}
	ilInit();
	pthread_create(&threadCompute, 0, Compute, 0);
	initTextures();
	initVBO();
	initShader();
	
	glutReshapeFunc(ResizeWindow);
	glutDisplayFunc(Render);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecKey);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialUpFunc(SpecKeyUp);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	glutTimerFunc(timerDelay, Timer, 0);			
	glutMainLoop();

	freeShader();
	freeBuffers();
	pthread_cancel(threadCompute);
}
