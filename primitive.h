#ifndef PRIMITIVE_H
#define PRIMITIVE_H

class SinCos{
public:
        float Cos,Sin;
        SinCos();
        SinCos(float ang);
        SinCos(float _Cos, float _Sin);
        int cmp(SinCos b);
        float* vec2float();
        void operator+=(SinCos b);
        SinCos operator+(SinCos b);
        void operator-=(SinCos b);
        SinCos operator-();
};

struct vec3_t{
    float x,y,z;
    vec3_t();
    vec3_t(float,float,float);
    vec3_t(SinCos a);
    vec3_t  operator+(const vec3_t&);
    vec3_t  operator+=(const vec3_t&);
    vec3_t  operator-(const vec3_t&);
    vec3_t  operator-=(const vec3_t&);
    vec3_t  operator*(float);
    vec3_t  operator*=(float);
    vec3_t  operator/(float);
    vec3_t  operator/=(float);
    vec3_t  operator^(const vec3_t&);
    float&  operator[](int i);
    float   operator*(const vec3_t&);
    vec3_t  operator-();
    float   lensq();
    float   len();
	void	normalize();
	void	normfast();
    SinCos  angle(const vec3_t&);
	SinCos* xyangptr();
	float* ptr();
};
#endif