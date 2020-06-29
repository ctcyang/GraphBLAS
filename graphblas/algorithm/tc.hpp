#ifndef GRAPHBLAS_ALGORITHM_TC_HPP_
#define GRAPHBLAS_ALGORITHM_TC_HPP_

#include <limits>
#include <vector>
#include <string>

#include "graphblas/algorithm/test_tc.hpp"
#include "graphblas/backend/cuda/util.hpp"

namespace graphblas {
namespace algorithm {

// Use int for ntris and A
float tc(int*                 ntris,
         const Matrix<float>* A,     // upper triangular matrix
         Descriptor*          desc) {
  // Get number of vertices
  Index A_nrows;
  CHECK(A->nrows(&A_nrows));

  // Pagerank vector (p)
  CHECK(p->clear());
  CHECK(p->fill(1.f/A_nrows));

  // Previous pagerank vector (p_tcev)
  Vector<float> p_tcev(A_nrows);

  // Temporary pagerank (p_temp)
  Vector<float> p_swap(A_nrows);

  // Residual vector (r)
  Vector<float> r(A_nrows);
  r.fill(1.f);

  // Temporary residual (r_temp)
  Vector<float> r_temp(A_nrows);

  int iter;
  float error_last = 0.f;
  float error = 1.f;
  Index unvisited = A_nrows;

  backend::GpuTimer gpu_tight;
  float gpu_tight_time = 0.f;
  gpu_tight.Start();

  for (iter = 1; error > eps && iter <= desc->descriptor_.max_niter_; ++iter) {
    if (desc->descriptor_.debug())
      std::cout << "=====TC Iteration " << iter - 1 << "=====\n";
    gpu_tight.Stop();
    if (iter > 1) {
      std::string vxm_mode = (desc->descriptor_.lastmxv_ == GrB_PUSHONLY) ?
          "push" : "pull";
      if (desc->descriptor_.timing_ == 1)
        std::cout << iter - 1 << ", " << error << "/" << A_nrows << ", "
            << unvisited << ", " << vxm_mode << ", "
            << gpu_tight.ElapsedMillis() << "\n";
      gpu_tight_time += gpu_tight.ElapsedMillis();
    }
    unvisited -= static_cast<int>(error);
    gpu_tight.Start();
    error_last = error;
    p_tcev = *p;

    // p = A*p + (1-alpha)*1
    vxm<float, float, float, float>(&p_swap, GrB_NULL, GrB_NULL,
        PlusMultipliesSemiring<float>(), &p_tcev, A, desc);
    eWiseAdd<float, float, float, float>(p, GrB_NULL, GrB_NULL,
        PlusMultipliesSemiring<float>(), &p_swap, (1.f-alpha)/A_nrows, desc);

    // error = l2loss(p, p_tcev)
    eWiseMult<float, float, float, float>(&r, GrB_NULL, GrB_NULL,
        PlusMinusSemiring<float>(), p, &p_tcev, desc);
    eWiseAdd<float, float, float, float>(&r_temp, GrB_NULL, GrB_NULL,
        MultipliesMultipliesSemiring<float>(), &r, &r, desc);
    reduce<float, float>(&error, GrB_NULL, PlusMonoid<float>(), &r_temp, desc);
    error = sqrt(error);

    if (desc->descriptor_.debug())
      std::cout << "error: " << error_last << std::endl;
  }
  gpu_tight.Stop();
  std::string vxm_mode = (desc->descriptor_.lastmxv_ == GrB_PUSHONLY) ?
      "push" : "pull";
  if (desc->descriptor_.timing_ > 0)
    std::cout << iter - 1 << ", " << error << "/" << A_nrows << ", "
        << unvisited << ", " << vxm_mode << ", "
        << gpu_tight.ElapsedMillis() << "\n";
  gpu_tight_time += gpu_tight.ElapsedMillis();
  return gpu_tight_time;
}

template <typename T, typename a>
int tcCpu(int*       ntris,
          Matrix<a>* A,
          bool       transpose = false) {
  if (transpose)
    max_depth = SimpleReferenceTc<T>(A->matrix_.nrows_,
        A->matrix_.sparse_.h_cscColPtr_, A->matrix_.sparse_.h_cscRowInd_,
        A->matrix_.sparse_.h_cscVal_, ntris);
  else
    max_depth = SimpleReferenceTc<T>(A->matrix_.nrows_,
        A->matrix_.sparse_.h_csrRowPtr_, A->matrix_.sparse_.h_csrColInd_,
        A->matrix_.sparse_.h_csrVal_, ntris);

  return 0;
}
}  // namespace algorithm
}  // namespace graphblas

#endif  // GRAPHBLAS_ALGORITHM_TC_HPP_
