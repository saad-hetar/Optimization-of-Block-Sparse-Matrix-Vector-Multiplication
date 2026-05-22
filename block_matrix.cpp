#include "block_matrix.h"
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <iomanip>
#include <immintrin.h>
#include <unordered_map>

block_matrix::block_matrix() : row_index(nullptr), col_index(nullptr), values(nullptr), nzb(0), old_matrix_size(0),
                               matrix_size_row(2), matrix_size_col(2), block_size_row(0), block_size_col(0)
{
}


block_matrix::block_matrix(const char* loc) : row_index(nullptr), col_index(nullptr), values(nullptr), nzb(0), old_matrix_size(0),
                                            matrix_size_row(2), matrix_size_col(2), block_size_row(0), block_size_col(0)
{
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, loc, "r");

    if (err != 0 || file == nullptr) {
        throw std::invalid_argument("Error: Could not open file");
    }

    char line[256];
    // Skip comments starting with '%'
    while (fgets(line, sizeof(line), file) && line[0] == '%');

    int rows, cols, nz;

    sscanf_s(line, "%d %d %d", &rows, &cols, &nz);

    // matrix from file
    int* file_row_index = new int[nz];
    int* file_col_index = new int[nz];
    double* file_values = new double[nz];

    for (int i = 0; i < nz; ++i) {
        if (fscanf_s(file, "%d %d %lf", &file_row_index[i], &file_col_index[i], &file_values[i]) != 3) {
            break;
        }
        --file_row_index[i]; // because the matrix is sorted by 1-base index
        --file_col_index[i];
    }

    if (rows >= cols)
    {
        while (matrix_size_row < rows)
            matrix_size_row *= 2;

        matrix_size_col = matrix_size_row;
        old_matrix_size = rows;
    }
    else
    {
        while (matrix_size_col < cols)
            matrix_size_col *= 2;

        matrix_size_row = matrix_size_col;
        old_matrix_size = cols;
    }

    block_size_row = matrix_size_row / b_size;
    block_size_col = matrix_size_col / b_size;


    //------------------------------

    long long int total_flag_size = static_cast<long long>(block_size_row) * block_size_col;

    bool* flag = nullptr;

    try
    {
        flag = new bool[total_flag_size](); // all false

        //std::cout << "Flag allocated successfully." << std::endl; 
    }
    catch (const std::bad_alloc& e)
    {
        std::cerr << "Allocation failed: " << e.what() << '\n';
    }
    
    long long index = 0;
    // to find out non zero blocks
    for (int i = 0; i < nz; ++i)
    {
        int b_row = (file_row_index[i] - (file_row_index[i] % b_size)) / b_size;
        int b_col = (file_col_index[i] - (file_col_index[i] % b_size)) / b_size;

        index = static_cast<long long>(b_row) * block_size_col + b_col;

        flag[index] = true;
    }

  
    int counter = 0;
    for (long long int i = 0; i < total_flag_size; ++i)
    {
        if (flag[i])
        {
            ++counter;
        }
    }
    

    nzb = counter;
    values = new double* [nzb];
    col_index = new int[nzb];
    row_index = new int[nzb];

    for (int n = 0; n < nzb; ++n)
    {
        values[n] = new double[b_size * b_size];

        for (int k = 0; k < b_size * b_size; ++k)
        {
            values[n][k] = 0;
        }
    }

    for (int i = 0; i < b_size; ++i)
    {
        for (int j = 0; j < b_size; ++j)
        {
            if (i == j)
            {
                values[nzb - 1][i * b_size + j] = 1.0;
            }
        }
    }

    std::unordered_map<long long, int> block_map;
    block_map.reserve(nzb);

    counter = 0;
    index = 0;
    for (int i = 0; i < block_size_row; ++i)
    {
        for (int j = 0; j < block_size_col; ++j)
        {
            index = static_cast<long long>(i) * block_size_col + j;

            if (flag[index])
            {
                row_index[counter] = i;
                col_index[counter] = j;
                block_map[index] = counter; // key is same formula as flag index
                ++counter;
            }
        }
    }

    delete[] flag;

    for (int m = 0; m < nz; ++m)
    {
        int k = file_row_index[m] % b_size;
        int l = file_col_index[m] % b_size;

        int block_row = file_row_index[m] / b_size;
        int block_col = file_col_index[m] / b_size;

        long long key = static_cast<long long>(block_row) * block_size_col + block_col;

        auto it = block_map.find(key);
        
        values[it->second][k * b_size + l] = file_values[m];
    }

    delete[] file_values;
    delete[] file_col_index;
    delete[] file_row_index;
    fclose(file);
}


block_matrix::~block_matrix()
{
    delete[] row_index;
    delete[] col_index;

    for (int i = 0; i < nzb; i++)
    {
        delete[] values[i];
    }

    delete[] values;
}

block_matrix& block_matrix::operator=(const block_matrix& other)
{
    if (this == &other)
    {
        return *this;
    }

    delete[] row_index;
    delete[] col_index;

    if (values) {
        for (int i = 0; i < nzb; ++i)
        {
            delete[] values[i];
        }

        delete[] values;
    }

    nzb = other.nzb;
    matrix_size_row = other.matrix_size_row;
    matrix_size_col = other.matrix_size_col;
    block_size_row = other.block_size_row;
    block_size_col = other.block_size_col;

    row_index = new int[nzb];

    for (int i = 0; i < nzb; ++i)
    {
        row_index[i] = other.row_index[i];
    }

    col_index = new int[nzb];

    for (int i = 0; i < nzb; ++i)
    {
        col_index[i] = other.col_index[i];
    }

    values = new double* [nzb];
    for (int i = 0; i < nzb; ++i) {
        values[i] = new double[block_size_row * block_size_col];
        for (int j = 0; j < block_size_row * block_size_col; ++j) {
            values[i][j] = other.values[i][j];
        }
    }

    return *this;
}


vec block_matrix::operator*(const vec& b) const
{
    if (matrix_size_row != b.size)
    {
        throw std::invalid_argument("Invalid matrix size");
    }

    vec r(b.size);

    for (int i = 0; i < b.size; ++i)
    {
        r.values[i] = 0;
    }

#pragma omp parallel for
    for (int n = 0; n < nzb; ++n)
    {
        for (int i = 0; i < b_size; ++i)
        {
            __m256d rowSumQuad = _mm256_setzero_pd();
            for (int j = 0; j < b_size; j += 4)
            {
                //r.values[row_index[n] * b_size + i] += values[n][i * b_size + j] * b.values[col_index[n] * b_size + j];

                __m256d matQuad = _mm256_loadu_pd(&values[n][i * b_size + j]);

                __m256d vecQuad = _mm256_loadu_pd(&b.values[col_index[n] * b_size + j]);

                __m256d prod = _mm256_mul_pd(matQuad, vecQuad);
                rowSumQuad = _mm256_add_pd(rowSumQuad, prod);
            }

            __m128d lowLane = _mm256_extractf128_pd(rowSumQuad, 0);
            __m128d highLane = _mm256_extractf128_pd(rowSumQuad, 1);

            __m128d sumLane = _mm_add_pd(lowLane, highLane);
            __m128d finalSum = _mm_hadd_pd(sumLane, sumLane);

            // Store the final scalar result into our result array
            double sum;
            _mm_storel_pd(&sum, finalSum);

            r.values[row_index[n] * b_size + i] += sum;
        }
    }

    return r;
}


vec block_matrix::mult_AT(const vec& b) const
{
    if (matrix_size_row != b.size)
    {
        throw std::invalid_argument("Invalid matrix size");
    }

    vec r(b.size);

    for (int i = 0; i < b.size; ++i)
    {
        r.values[i] = 0;
    }
    #pragma omp parallel for
    for (int n = 0; n < nzb; ++n)
    {
        for (int i = 0; i < b_size; ++i)
        {
            for (int j = 0; j < b_size; ++j)
            {
                r.values[col_index[n] * b_size + j] += values[n][i * b_size + j] * b.values[col_index[n] * b_size + i]; //vector is like blocks
            }
        }
    }

    return r;
}

void block_matrix::print_matrix() const
{
    std::cout << "Size of full matrix's rows: " << matrix_size_row << std::endl;
    std::cout << "Size of full matrix's columns: " << matrix_size_col << std::endl;
    std::cout << "Size of blocks' rows: " << block_size_row << std::endl;
    std::cout << "Size of blocks' columns: " << block_size_col << std::endl;
    std::cout << "number of non-zero blocks: " << nzb << std::endl << std::endl;

    int index = 0;
    for (int i = 0; i < block_size_row; ++i)
    {
        for (int k = 0; k < b_size; ++k)
        {
            for (int j = 0; j < block_size_col; ++j)
            {
                // search for block (i,j) in stored blocks
                int found_idx = -1;
                for (int m = 0; m < nzb; ++m)
                {
                    if (row_index[m] == i && col_index[m] == j)
                    {
                        found_idx = m;
                        break;
                    }
                }

                for (int l = 0; l < b_size; ++l)
                {
                    if (found_idx != -1)
                        std::cout << std::setw(5) << values[found_idx][k * b_size + l];
                    else
                        std::cout << std::setw(5) << 0;
                }
                std::cout << "|";
            }
            std::cout << std::endl;
        }
        std::cout << "------" << std::endl;
    }
}






//vec block_matrix::operator*(const vec& b) const
//{
//    if (matrix_size_row != b.size)
//    {
//        throw std::invalid_argument("Invalid matrix size");
//    }
//
//    vec r(b.size);
//
//    for (int i = 0; i < b.size; ++i)
//    {
//        r.values[i] = 0;
//    }
//
//#pragma omp parallel for
//    for (int n = 0; n < nzb; ++n)
//    {
//        for (int i = 0; i < b_size; ++i)
//        {
//            for (int j = 0; j < b_size; ++j)
//            {
//                r.values[row_index[n] * b_size + i] += values[n][i * b_size + j] * b.values[col_index[n] * b_size + j];
//            }
//        }
//    }
//
//    return r;
//}


//vec block_matrix::operator*(const vec& b) const
//{
//    if (matrix_size_row != b.size)
//    {
//        throw std::invalid_argument("Invalid matrix size");
//    }
//
//    vec r(b.size);
//
//    for (int i = 0; i < b.size; ++i)
//    {
//        r.values[i] = 0.0;
//    }
//
//    // Hardcoding or enforcing the block size to 8 allows the compiler 
//    // to optimize loop unrolling flawlessly.
//    const int STRIDE = 8;
//
//#pragma omp parallel for
//    for (int n = 0; n < nzb; ++n)
//    {
//        int row_base = row_index[n] * STRIDE;
//        int col_base = col_index[n] * STRIDE;
//
//        // REG_REUSE OPTIMIZATION: Load the 8 elements of vector b once.
//        // It remains inside a ZMM register for all 8 iterations of the row loop.
//        __m512d b_vec = _mm512_loadu_pd(&b.values[col_base]);
//
//        for (int i = 0; i < STRIDE; ++i)
//        {
//            // 1. Load a full row of 8 doubles from your matrix block
//            __m512d mat_row = _mm512_loadu_pd(&values[n][i * STRIDE]);
//
//            // 2. Perform element-wise parallel multiplication
//            __m512d prod = _mm512_mul_pd(mat_row, b_vec);
//
//            // 3. Horizontally sum the 8 elements inside the ZMM register 
//            double sum = _mm512_reduce_add_pd(prod);
//
//            // 4. Accumulate scalar result into the target vector element
//            r.values[row_base + i] += sum;
//        }
//    }
//
//    return r;
//}