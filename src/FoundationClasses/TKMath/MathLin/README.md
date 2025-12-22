# MathLin Package

The MathLin package provides modern C++ implementations of linear algebra solvers for the Open CASCADE Technology (OCCT) library. These solvers are designed for solving linear systems, matrix decompositions, eigenvalue problems, and least squares computations.

## Components

### MathLin_Gauss.hxx
LU decomposition with partial pivoting for solving general linear systems Ax = b.
- `LU()` - LU decomposition of a square matrix
- `Solve()` - Solve linear system using LU decomposition
- `SolveMultiple()` - Solve multiple right-hand sides
- `Determinant()` - Compute matrix determinant
- `Invert()` - Compute matrix inverse

### MathLin_Crout.hxx
Crout (LDL^T) decomposition optimized for symmetric matrices.
- `Crout()` - LDL^T decomposition for symmetric matrices
- `SolveCrout()` - Solve symmetric linear system
- `InvertCrout()` - Invert symmetric matrix

### MathLin_SVD.hxx
Singular Value Decomposition for general and ill-conditioned matrices.
- `SVD()` - Compute singular value decomposition A = U * S * V^T
- `SolveSVD()` - Solve linear system using SVD (handles rank-deficient systems)
- `PseudoInverse()` - Compute Moore-Penrose pseudo-inverse
- `ConditionNumber()` - Compute matrix condition number
- `NumericalRank()` - Determine numerical rank

### MathLin_Householder.hxx
QR decomposition using Householder reflections.
- `QR()` - QR decomposition of a matrix
- `SolveQR()` - Solve overdetermined system (least squares)
- `SolveQRMultiple()` - Solve multiple right-hand sides

### MathLin_Jacobi.hxx
Jacobi method for eigenvalue decomposition of symmetric matrices.
- `Jacobi()` - Compute eigenvalues and eigenvectors
- `EigenValues()` - Compute only eigenvalues
- `SpectralDecomposition()` - Compute spectral decomposition A = V * D * V^T
- `MatrixPower()` - Compute matrix power using eigendecomposition
- `MatrixSqrt()` - Compute matrix square root
- `MatrixInvSqrt()` - Compute inverse square root

### MathLin_LeastSquares.hxx
Least squares solvers with multiple methods.
- `LeastSquares()` - Solve overdetermined systems with choice of method
- `WeightedLeastSquares()` - Weighted least squares
- `RegularizedLeastSquares()` - Tikhonov/Ridge regression
- `OptimalRegularization()` - Find optimal regularization parameter

### MathLin_EigenSearch.hxx
QL algorithm for eigenvalue decomposition of tridiagonal matrices.
- `EigenTridiagonal()` - Eigenvalues and eigenvectors of tridiagonal matrix
- `GetEigenVector()` - Extract single eigenvector from result

## Usage

All functions are in the `MathLin` namespace and return result structures with status information:

```cpp
#include <MathLin_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

math_Matrix A(1, 3, 1, 3);
math_Vector b(1, 3);
// ... initialize A and b ...

auto result = MathLin::Solve(A, b);
if (result.IsDone())
{
  const math_Vector& x = *result.Solution;
  // Use solution x
}
```

## Dependencies

The MathLin package depends on:
- MathUtils package for common types and utilities
- math_Matrix, math_Vector, math_IntegerVector from the math package
- math_Recipes for some low-level algorithms (SVD_Decompose, Jacobi)
