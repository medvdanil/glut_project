#ifndef LIGHT_H
#define LIGHT_H
#include"primitive.h"
class Light{
public:
	vec3_t pos, dir, vup;
	int sMapW, sMapH;
	float projMatrix[16];
	vec3_t bright;
	float shadowRange;
	int texShadow, rbShadow, fbShadow;
	Light(vec3_t _pos = vec3_t(0,0,0), vec3_t _dir = vec3_t(1,0,0), vec3_t _bright = vec3_t(1,1,1), 
	    int _sMapW = 1024, int _sMapH = 1024, float _shadowRange = 200)
	{
		pos = _pos;
		dir = _dir;
		sMapW = _sMapW;
		sMapH = _sMapH;
		bright = _bright;
		shadowRange = _shadowRange;
	}
	void init(){			
		glGenTextures(1, (unsigned int*)&texShadow);
		glBindTexture(GL_TEXTURE_2D, texShadow);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, sMapW, sMapH, 
						  0, GL_RED, GL_UNSIGNED_SHORT, 0);
		glGenRenderbuffers(1, (unsigned int*)&rbShadow);
		glBindRenderbuffer(GL_RENDERBUFFER, rbShadow);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, sMapW, sMapH);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glGenFramebuffers(1, (unsigned int*)&fbShadow);
		glBindFramebuffer(GL_FRAMEBUFFER, fbShadow);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texShadow, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbShadow);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);		
	}
};
#endif
