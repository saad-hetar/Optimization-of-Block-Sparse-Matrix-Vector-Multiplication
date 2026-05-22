#pragma once
#ifndef BLOCK_MATRIX_H
#define BLOCK_MATRIX_H

#include "C:\Users\saad_hetar\OneDrive\Desktop\c++\Lab 2\Lab 2\vec.h"
// SIMD YMM registers = 16 * (256 bits = 32 bytes)  = 512 bytes = 64 double 8*8

struct block_matrix
{
    int* row_index;
    int* col_index;
    double** values;
    const short int b_size = 64;
    int nzb; // non-zero elements for block
    int matrix_size_row;
    int matrix_size_col;
    int block_size_row;
    int block_size_col;
    int old_matrix_size;

    block_matrix();
    block_matrix(const char* loc);
    ~block_matrix();

    block_matrix& operator=(const block_matrix& other);
    vec operator*(const vec& b) const;

    vec mult_AT(const vec& b) const;

    void print_matrix() const;

};


#endif




//for (int i = 0; i < block_size_row; ++i) // row of matrix
//{
//    for (int k = 0; k < b_size; ++k) // row of block
//    {
//        for (int j = 0; j < block_size_col; ++j) // col of matrix
//        {
//            for (int l = 0; l < b_size; ++l) // col of block
//            {
//                if ((values_in < nz) && (file_col_index[values_in] == col_in) &&
//                    (file_row_index[values_in] == row_in))
//                {
//                    values_ptr[i * block_size_col + j][k * b_size + l] = file_values[values_in];
//                    flag[i * block_size_col + j] = true;
//                    ++values_in;
//                }
//                else
//                {
//                    values_ptr[i * block_size_col + j][k * b_size + l] = 0;
//                }
//
//                ++row_in;
//            }
//        }
//
//        ++col_in;
//        row_in = 0;
//    }
//}
