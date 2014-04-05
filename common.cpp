#include <string.h>
#include <math.h>
#include "primitive.h"
#include "common.h"

float InvSqrt (float x){
    float xhalf = 0.5f*x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i>>1);
    x = *(float*)&i;
    x = x*(1.5f - xhalf*x*x);
    return x;
}
vec3_t normalize(const vec3_t &v){
	float invlen = 1/sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
	return vec3_t(v.x*invlen, v.y*invlen, v.z*invlen);
}
vec3_t normfast(const vec3_t &v){
	float invlen = InvSqrt(v.x*v.x+v.y*v.y+v.z*v.z);
	return vec3_t(v.x*invlen, v.y*invlen, v.z*invlen);
}
float DistSq(vec3_t v1, vec3_t v2 ){
    return (v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)*(v1.z-v2.z);
}
float buffmat[16],buffmat2[16];
void multMatrix(float *a, float *b, float *c){
	int i, j, k;
	if(a == c || b == c){
		for(i=0;i<16;i+=4)
			for(j=0;j<4;j++)
				buffmat[i+j] = a[i]*b[j]+a[i+1]*b[4+j]+a[i+2]*b[8+j]+a[i+3]*b[12+j];
		memcpy(c, buffmat, sizeof(buffmat));
	}
	else
		for(i=0;i<16;i+=4)
			for(j=0;j<4;j++)
				c[i+j] = a[i]*b[j]+a[i+1]*b[4+j]+a[i+2]*b[8+j]+a[i+3]*b[12+j];
}
vec3_t multPointMatrix(vec3_t p, float* mat){
	return vec3_t(p.x*mat[0]+p.y*mat[4]+p.z*mat[8]+mat[12], 
		p.x*mat[1]+p.y*mat[5]+p.z*mat[9]+mat[13],
		p.x*mat[2]+p.y*mat[6]+p.z*mat[10]+mat[14])/(p.x*mat[3]+p.y*mat[7]+p.z*mat[11]+mat[15]);
}

void buildRotationMatrix(SinCos a, float x, float y, float z, float *matrix) {	
	float t = 1.0 - a.Cos;
	float tx = t * x;
	float ty = t * y;
	float tz = t * z;	
	float sz = a.Sin * z;
	float sy = a.Sin * y;
	float sx = a.Sin * x;
	
	matrix[0]  = tx * x + a.Cos;
	matrix[1]  = tx * y + sz;
	matrix[2]  = tx * z - sy;
	matrix[3]  = 0;

	matrix[4]  = tx * y - sz;
	matrix[5]  = ty * y + a.Cos;
	matrix[6]  = ty * z + sx;
	matrix[7]  = 0;

	matrix[8]  = tx * z + sy;
	matrix[9]  = ty * z - sx;
	matrix[10] = tz * z + a.Cos;
	matrix[11] = 0;

	matrix[12] = 0;
	matrix[13] = 0; 
	matrix[14] = 0;
	matrix[15] = 1; 
}
void matRotate(float* mat, SinCos a, vec3_t v){
	buildRotationMatrix(a, v.x, v.y, v.z, buffmat2);
	multMatrix(mat, buffmat2, mat);
}
void matPreRotate(float* mat, SinCos a, vec3_t v){
	buildRotationMatrix(a, v.x, v.y, v.z, buffmat2);
	multMatrix(buffmat2, mat, mat);
}
void matTranslate(float* mat, vec3_t v){
	buffmat2[0] = 1, buffmat2[1] = 0, buffmat2[2] = 0, buffmat2[3] = 0;
	buffmat2[4] = 0, buffmat2[5] = 1, buffmat2[6] = 0, buffmat2[7] = 0;
	buffmat2[8] = 0, buffmat2[9] = 0, buffmat2[10] = 1, buffmat2[11] = 0;
	buffmat2[12] = v.x, buffmat2[13] = v.y, buffmat2[14] = v.z, buffmat2[15] = 1;
	multMatrix(mat, buffmat2, mat);
}
void inverseMatrix(float dst[16], float src[16]){
	dst[0] = src[0];
	dst[1] = src[4];
	dst[2] = src[8];
	dst[3] = 0.0;
	dst[4] = src[1];
	dst[5] = src[5];
	dst[6]  = src[9];
	dst[7] = 0.0;
	dst[8] = src[2];
	dst[9] = src[6];
	dst[10] = src[10];
	dst[11] = 0.0;
	dst[12] = -(src[12] * src[0]) - (src[13] * src[1]) - (src[14] * src[2]);
	dst[13] = -(src[12] * src[4]) - (src[13] * src[5]) - (src[14] * src[6]);
	dst[14] = -(src[12] * src[8]) - (src[13] * src[9]) - (src[14] * src[10]);
	dst[15] = 1.0;
}
