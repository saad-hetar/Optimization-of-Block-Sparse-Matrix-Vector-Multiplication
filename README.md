# Optimization-of-Block-Sparse-Matrix-Vector-Multiplication
This is a lab for the course Operating Systems. The code was written to save the matrix as blocks, so we can multiply the matrix as fast as possible by assigning the size of the block to the size of the registers in the CPU. Libraries <omp.h> and <immintrin.h> were used to parallel running the code by the cores.

The link of the matrix: https://sparse.tamu.edu/Norris/torso3
The method that was used to solve Ax = b was: Biconjugate gradient stabilized method

To run the project:
1) Extract the file torso3.tar.gz to get the .mtx file of the matrix and paste its path to the constructor so you can save the matrix as blocks. note that you can get anthor matrix with .mtx format if you want.
2) uncomment the code inside the while loop and the code that prints the info in the fisrt so you can track the sorting and solving process. Note that it will take time to get the result.
3) After that you can find the b.txt vector and the result.txt vector in the directory. You can compare to see if the result is correct or not.
4) You can run the project by .ps1 file as well. it will run for 10 threads and then make csv file so you can see the time that was spent to get the result for different numbers of threads. Amdahl's law was also used to see the how good parallelization was.

Notes:
1) You can change the size of the blocks from block_matrix.h by changing the value of b_size. b_size should be 2^n.
2) For CPU you need to change the <immintrin.h> code of multiplying because in files it was written for AVX 256.
3) You can change the matrix to smaller one if you search in http://sparse.tamu.edu/.
4) Not all matrix can be solved by this code. if you noticed that the residual get bigger then that's mean you won't get an answer by Biconjugate gradient stabilized method.
5) The matrix should be squared.
