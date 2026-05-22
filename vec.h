#pragma once
#ifndef VEC_H
#define VEC_H
#include <ostream>
struct vec
{
    double* values = nullptr;
    int size = 0;

    vec(const int& size);
    ~vec();

    vec& operator=(const vec& other);
    vec& operator*=(const double& scalar);
    vec& operator+=(const vec& other);
    vec& operator-=(const vec& other);

    vec operator*(const double& scalar) const;
    vec operator+(const vec& other) const;
    vec operator-(const vec& other) const;

    double operator*(const vec& other) const;

    void print_vec(std::ostream& out) const;
};




#endif