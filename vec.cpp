#include "vec.h"
#include <iostream>
#include <stdexcept>
#include <ostream>
#include <omp.h>
#include <immintrin.h>

vec::vec(const int& b_size) : size(b_size)
{
	values = new double[size];
}

vec::~vec()
{
	delete[] values;
}

vec& vec::operator=(const vec& other)
{
	if (this == &other)
	{
		return *this;
	}

	delete[] values;

	size = other.size;
	values = new double[size];

	for (int i = 0; i < size; ++i)
	{
		values[i] = other.values[i];
	}

	return *this;
}

vec& vec::operator*=(const double& scalar)
{
	for (int i = 0; i < size; ++i)
	{
		values[i] *= scalar;
	}

	return *this;
}

vec& vec::operator+=(const vec& other)
{
	if (size != other.size)
	{
		throw std::invalid_argument("Invalid matrix size");
	}

	for (int i = 0; i < size; ++i)
	{
		values[i] += other.values[i];
	}

	return *this;
}

vec& vec::operator-=(const vec& other)
{
	if (size != other.size)
	{
		throw std::invalid_argument("Invalid matrix size");
	}

	for (int i = 0; i < size; ++i)
	{
		values[i] -= other.values[i];
	}

	return *this;
}


vec vec::operator+(const vec& other) const
{
	if (size != other.size)
	{
		throw std::invalid_argument("Invalid matrix size");
	}

	vec r(size);

	for (int i = 0; i < size; ++i)
	{
		r.values[i] = values[i] + other.values[i];
	}

	return r;
}

vec vec::operator-(const vec& other) const
{
	if (size != other.size)
	{
		throw std::invalid_argument("Invalid matrix size");
	}

	vec r(size);

	for (int i = 0; i < size; ++i)
	{
		r.values[i] = values[i] - other.values[i];
	}

	return r;
}

vec vec::operator*(const double& scalar) const
{
	vec r(size);

	for (int i = 0; i < size; ++i)
	{
		r.values[i] = values[i] * scalar;
	}

	return r;
}


double vec::operator*(const vec& other) const
{
	if (size != other.size)
		throw std::invalid_argument("Invalid matrix size");

	double r = 0;
	__m256d sum = _mm256_setzero_pd();

	int i = 0;
	for (; i + 3 < size; i += 4)
	{
		__m256d a = _mm256_loadu_pd(&values[i]);
		__m256d b = _mm256_loadu_pd(&other.values[i]);
		sum = _mm256_fmadd_pd(a, b, sum);
	}

	// horizontal sum of 4 doubles in sum register
	__m128d low = _mm256_castpd256_pd128(sum);
	__m128d high = _mm256_extractf128_pd(sum, 1);
	__m128d val = _mm_add_pd(low, high);
	val = _mm_hadd_pd(val, val);
	r += _mm_cvtsd_f64(val);

	// handle remaining elements
	for (; i < size; ++i)
		r += values[i] * other.values[i];

	return r;
}



void vec::print_vec(std::ostream& out) const
{
	out << "Size of vector: " << size << std::endl << std::endl;

	for (int i = 0; i < size; ++i)
	{
		out << values[i] << std::endl;
	}
}


//void vec::print_vec() const
//{
//	std::cout << "Size of vector: " << size << std::endl << std::endl;
//
//	for (int i = 0; i < size; ++i)
//	{
//		std::cout << values[i] << std::endl; //std::endl
//	}
//}


//double vec::operator*(const vec& other) const
//{
//	if (size != other.size)
//	{
//		throw std::invalid_argument("Invalid matrix size");
//	}
//
//	double r = 0;
//
//	for (int i = 0; i < size; ++i)
//	{
//
//		r += values[i] * other.values[i];
//	}
//
//	return r;
//}