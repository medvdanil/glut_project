#ifndef COMMON_H
#define COMMON_H
float InvSqrt (float x);
vec3_t normalize(const vec3_t &v);
vec3_t normfast(const vec3_t &v);
float DistSq(vec3_t v1, vec3_t v2 = vec3_t());
void multMatrix(float *a, float *b, float *c);
void matRotate(float* mat, SinCos a, vec3_t v);
void matPreRotate(float* mat, SinCos a, vec3_t v);
void matTranslate(float* mat, vec3_t v);
vec3_t multPointMatrix(vec3_t p, float* mat);
vec3_t multVectorMatrix(vec3_t v, float* mat);
void buildRotationMatrix(SinCos a, float x, float y, float z, float *matrix);
void inverseMatrix(float dst[16], float src[16]);
#endif