#include <algorithm>
#include <cassert>
#include <iostream>
#include <thread>
#if defined(_OPENMP)
#include <omp.h>
#endif
#include "ProdMatMat.hpp"

namespace {
void prodSubBlocks(int iRowBlkA, int iColBlkB, int iColBlkA, int szBlock,
                   const Matrix& A, const Matrix& B, Matrix& C) {
//---- Séquentielle ----
  /*
  for (int j = iColBlkB; j < std::min(B.nbCols, iColBlkB + szBlock); j++)
    for (int k = iColBlkA; k < std::min(A.nbCols, iColBlkA + szBlock); k++)
      for (int i = iRowBlkA; i < std::min(A.nbRows, iRowBlkA + szBlock); i++)
        C(i, j) += A(i, k) * B(k, j);
  */

// ---- OpenMP ----
  
  int i,j,k;
  #pragma omp parallel for private(i,j,k) shared (A,B,C) schedule(dynamic) num_threads(2)
    for (j = iColBlkB; j < std::min(B.nbCols, iColBlkB + szBlock); j++)
      for (k = iColBlkA; k < std::min(A.nbCols, iColBlkA + szBlock); k++)
        for (i = iRowBlkA; i < std::min(A.nbRows, iRowBlkA + szBlock); i++)
          C(i, j) += A(i, k) * B(k, j);
  

}
const int szBlock = 256;
}  // namespace

Matrix operator*(const Matrix& A, const Matrix& B) {
  Matrix C(A.nbRows, B.nbCols, 0.0);
  //prodSubBlocks(0, 0, 0, std::max({A.nbRows, B.nbCols, A.nbCols}), A, B, C);

  int N = A.nbRows / szBlock + 1;
  for (int k = 0; k < N; k++) {
    for (int j = 0; j < N; j++) {
      for (int i = 0; i < N; i++) {
        prodSubBlocks(i*szBlock,j*szBlock,k*szBlock,szBlock,A,B,C);
      }
    }
   }
    
  return C;
}
