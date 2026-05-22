#include <iostream>
#include <cstdio>
#include <cmath>
#include <omp.h>
#include <random>
#include <fstream>
#include "block_matrix.h"

// cd "C:\Users\saad_hetar\OneDrive\Desktop\c++"
// Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
// .\benchmark.ps1

int main()
{
    block_matrix A("C:/Users/saad_hetar/OneDrive/Desktop/c++/matrix file/torso3.mtx"); //pwtk econ 
    // worked bcsstm39    torso3  Spielman_k100_A_03  BenElechi1

    //std::cout << "finished sorting" << std::endl;
    //std::cout << "Size of full matrix's rows: " << A.matrix_size_row << std::endl;
    //std::cout << "Size of full matrix's columns: " << A.matrix_size_col << std::endl;
    //std::cout << "Size of blocks' rows: " << A.block_size_row << std::endl;
    //std::cout << "Size of blocks' columns: " << A.block_size_col << std::endl;
    //std::cout << "number of non-zero blocks: " << A.nzb << std::endl << std::endl;

    // BiCGSTAB Vectors
    vec b(A.matrix_size_col);
    vec x(b.size);
    vec r(b.size);
    vec p(b.size);
    vec v(b.size);       //v = A * p

    vec r_tilde(b.size); 
    vec s(b.size);       
    vec t(b.size);
    vec tmp(b.size);

    // BiCGSTAB Scalars
    double a = 1.0;      // alpha
    double b_0 = 0.0;    // beta
    double omega = 1.0;  // omega
    double rho = 1.0;
    double rho_new = 1.0;
    double denom = 0.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    for (int m = 0; m < b.size; ++m) {
        if (m < A.old_matrix_size)
        {
            b.values[m] = dist(gen);
            x.values[m] = 0.0;
            //r.values[m] = 2.0;
            //r_tilde.values[m] = 2.0;
            p.values[m] = 2.0;  // p_0 = r_0
            v.values[m] = 0.0;
            s.values[m] = 0.0;
            t.values[m] = 0.0;
            tmp.values[m] = 0.0;
        }
        else
        {
            b.values[m] = 0.0;
            x.values[m] = 0.0;
            r.values[m] = 0.0;
            r_tilde.values[m] = 0.0;
            p.values[m] = 0.0;
            v.values[m] = 0.0;
            s.values[m] = 0.0;
            t.values[m] = 0.0;
            tmp.values[m] = 0.0;
        }
    }

    r = b;
    r_tilde = b;

    std::ofstream file("b.txt");

    if (file.is_open()) {
        b.print_vec(file);   // write into txt file
        file.close();
    }

    //std::cout << "started calculating" << std::endl;

    double epsilon = 1e-8;
    double start = omp_get_wtime();

    double rr = r * r;
    //int iterations = 0;

    while (rr > epsilon)
    {
        rho_new = r_tilde * r;

        b_0 = (rho_new / rho) * (a / omega);
        p -= (v * omega);          // p_temp = p - omega * v
        p = r + (p * b_0);         // p_new = r + beta * p_temp

        v = A * p;

        denom = r_tilde * v;
        

        a = rho_new / denom;

        s = r;
        s -= (v * a);

        // t = A * s
        t = A * s;

        omega = (t * s) / (t * t);

        x += (p * a);
        x += (s * omega);

        r = s;
        r -= (t * omega);

        rho = rho_new;
        //-------------------
        tmp = A * x;
        tmp -= b;
        rr = std::sqrt(tmp * tmp);
        //-------------------

        //if (iterations % 50 == 0) {
        //    std::cout << "Iter: " << iterations
        //        << " | Residual: " << std::sqrt(rr) << std::endl;
        //}
        //iterations++;
    }
    std::cout << "Final residual norm (rr): " << rr << std::endl;

    double end = omp_get_wtime();

    std::cout << (end - start) << std::endl;

    //std::cout << rr << std::endl;
    b = A * x;
    //b.print_vec();

    //x.print_vec();
    std::ofstream result_file("result.txt");

    if (result_file.is_open()) {
        b.print_vec(result_file);   // write into txt file
        result_file.close();
    }

    return 0;
}




//if (std::abs(denom) < 1e-18) {
        //    std::cout << "BiCGSTAB Breakdown (denominator too small) at iter " << iterations << std::endl;
        //    break;
        //}

//if (std::abs(omega) < 1e-18) {
        //    std::cout << "BiCGSTAB breakdown (omega) at iter " << iterations << std::endl;
        //    break;
        //}



        