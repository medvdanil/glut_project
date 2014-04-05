#include<math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "primitive.h"
#include "common.h"
SinCos::SinCos(){
    Sin = 0;
    Cos = 1;
}
SinCos::SinCos(float ang){
    Sin = sin(ang);
    Cos = cos(ang);
}
SinCos::SinCos(float _Cos, float _Sin){
    Sin = _Sin;
    Cos = _Cos;
}
int SinCos::cmp(SinCos b){
        float r=Sin*b.Cos-Cos*b.Sin;
        return (int)(r*10000);
}
float* SinCos::vec2float(){
    return &Cos;
}
void SinCos::operator+=(SinCos b){
        float SinA=Sin;
        Sin=Sin*b.Cos+Cos*b.Sin;
        Cos=Cos*b.Cos-SinA*b.Sin;
}
SinCos SinCos::operator+(SinCos b){
       return SinCos(Cos*b.Cos-Sin*b.Sin, Sin*b.Cos+Cos*b.Sin);
}
void SinCos::operator-=(SinCos b){
        float SinA=Sin;
        Sin=Sin*b.Cos-Cos*b.Sin;
        Cos=Cos*b.Cos+SinA*b.Sin;
}
SinCos SinCos::operator-(){
	return SinCos(Cos,-Sin);
}

vec3_t::vec3_t(){
    x = y = z = 0;
}
vec3_t::vec3_t(float x,float y,float z):x(x),y(y),z(z){}
vec3_t::vec3_t(SinCos a){
    x = a.Cos;
    y = a.Sin;
    z = 0;
}
vec3_t  vec3_t::operator+(const vec3_t& b){
        vec3_t res;
        res.x = x+b.x;
        res.y = y+b.y;
        res.z = z+b.z;
        return res;
}
vec3_t  vec3_t::operator+=(const vec3_t& b){
        x+=b.x;  y+=b.y;  z+=b.z;
        return *this;
}
vec3_t  vec3_t::operator-(const vec3_t& b){
        vec3_t res;
        res.x = x-b.x;
        res.y = y-b.y;
        res.z = z-b.z;
        return res;
}
vec3_t  vec3_t::operator-=(const vec3_t& b){
        x-=b.x;  y-=b.y;  z-=b.z;
        return *this;
}
vec3_t  vec3_t::operator*(const float b){
        vec3_t res;
        res.x = x*b;
        res.y = y*b;
        res.z = z*b;
        return res;
}
vec3_t  vec3_t::operator*=(const float b){
        x*=b;  y*=b;  z*=b;
        return *this;
}
vec3_t  vec3_t::operator/(const float b){
        vec3_t res;
        res.x = x/b;
        res.y = y/b;
        res.z = z/b;
        return res;
}
vec3_t  vec3_t::operator/=(const float b){
        x/=b;  y/=b;  z/=b;
        return *this;
}
vec3_t  vec3_t::operator^(const vec3_t& b){
    vec3_t res=vec3_t(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);
    return res;
}
vec3_t  vec3_t::operator-(){
	return vec3_t(-this->x, -this->y, -this->z);
}
float&  vec3_t::operator[](int i){
    return ((float*)this)[i];
}
float   vec3_t::operator*(const vec3_t& b){
    return x*b.x+y*b.y+z*b.z;
}
float vec3_t::lensq(){
    return x*x+y*y+z*z;
}
float vec3_t::len(){
    return sqrt(x*x+y*y+z*z);
}
void vec3_t::normalize(){
	float invlen = 1/sqrt(this->x*this->x+this->y*this->y+this->z*this->z);
	this->x *= invlen;
	this->y *= invlen;
	this->z *= invlen;
}
void vec3_t::normfast(){
	float invlen = InvSqrt(this->x*this->x+this->y*this->y+this->z*this->z);
	this->x *= invlen;
	this->y *= invlen;
	this->z *= invlen;
}
SinCos  vec3_t::angle(const vec3_t& b){
    float ml=sqrt(DistSq(*this)*DistSq(b));
    SinCos r=SinCos(*this * b /ml, sqrt(DistSq(*this ^ b))/ml);
    return r;
}
SinCos* vec3_t::xyangptr(){
    return (SinCos*)this;
}
float* vec3_t::ptr(){
	return (float*)this;
}