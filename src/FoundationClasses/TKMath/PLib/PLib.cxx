// Created on: 1995-08-28
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PLib.hxx>

#include <GeomAbs_Shape.hxx>
#include <math.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <BSplCLib.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_ConstructionError.hxx>

#include <array>

// To convert points array into Real ..
// *********************************
//=================================================================================================

void PLib::SetPoles(const NCollection_Array1<gp_Pnt2d>& Poles, NCollection_Array1<double>& FP)
{
  int j      = FP.Lower();
  int PLower = Poles.Lower();
  int PUpper = Poles.Upper();

  for (int i = PLower; i <= PUpper; i++)
  {
    const gp_Pnt2d& P = Poles(i);
    FP(j)             = P.Coord(1);
    j++;
    FP(j) = P.Coord(2);
    j++;
  }
}

//=================================================================================================

void PLib::SetPoles(const NCollection_Array1<gp_Pnt2d>& Poles,
                    const NCollection_Array1<double>&   Weights,
                    NCollection_Array1<double>&         FP)
{
  int j      = FP.Lower();
  int PLower = Poles.Lower();
  int PUpper = Poles.Upper();

  for (int i = PLower; i <= PUpper; i++)
  {
    double          w = Weights(i);
    const gp_Pnt2d& P = Poles(i);
    FP(j)             = P.Coord(1) * w;
    j++;
    FP(j) = P.Coord(2) * w;
    j++;
    FP(j) = w;
    j++;
  }
}

//=================================================================================================

void PLib::GetPoles(const NCollection_Array1<double>& FP, NCollection_Array1<gp_Pnt2d>& Poles)
{
  int j      = FP.Lower();
  int PLower = Poles.Lower();
  int PUpper = Poles.Upper();

  for (int i = PLower; i <= PUpper; i++)
  {
    gp_Pnt2d& P = Poles(i);
    P.SetCoord(1, FP(j));
    j++;
    P.SetCoord(2, FP(j));
    j++;
  }
}

//=================================================================================================

void PLib::GetPoles(const NCollection_Array1<double>& FP,
                    NCollection_Array1<gp_Pnt2d>&     Poles,
                    NCollection_Array1<double>&       Weights)
{
  int j      = FP.Lower();
  int PLower = Poles.Lower();
  int PUpper = Poles.Upper();

  for (int i = PLower; i <= PUpper; i++)
  {
    double w    = FP(j + 2);
    Weights(i)  = w;
    gp_Pnt2d& P = Poles(i);
    P.SetCoord(1, FP(j) / w);
    j++;
    P.SetCoord(2, FP(j) / w);
    j++;
    j++;
  }
}

//=================================================================================================

void PLib::SetPoles(const NCollection_Array1<gp_Pnt>& Poles, NCollection_Array1<double>& FP)
{
  int j      = FP.Lower();
  int PLower = Poles.Lower();
  int PUpper = Poles.Upper();

  for (int i = PLower; i <= PUpper; i++)
  {
    const gp_Pnt& P = Poles(i);
    FP(j)           = P.Coord(1);
    j++;
    FP(j) = P.Coord(2);
    j++;
    FP(j) = P.Coord(3);
    j++;
  }
}

//=================================================================================================

void PLib::SetPoles(const NCollection_Array1<gp_Pnt>& Poles,
                    const NCollection_Array1<double>& Weights,
                    NCollection_Array1<double>&       FP)
{
  int j      = FP.Lower();
  int PLower = Poles.Lower();
  int PUpper = Poles.Upper();

  for (int i = PLower; i <= PUpper; i++)
  {
    double        w = Weights(i);
    const gp_Pnt& P = Poles(i);
    FP(j)           = P.Coord(1) * w;
    j++;
    FP(j) = P.Coord(2) * w;
    j++;
    FP(j) = P.Coord(3) * w;
    j++;
    FP(j) = w;
    j++;
  }
}

//=================================================================================================

void PLib::GetPoles(const NCollection_Array1<double>& FP, NCollection_Array1<gp_Pnt>& Poles)
{
  int j      = FP.Lower();
  int PLower = Poles.Lower();
  int PUpper = Poles.Upper();

  for (int i = PLower; i <= PUpper; i++)
  {
    gp_Pnt& P = Poles(i);
    P.SetCoord(1, FP(j));
    j++;
    P.SetCoord(2, FP(j));
    j++;
    P.SetCoord(3, FP(j));
    j++;
  }
}

//=================================================================================================

void PLib::GetPoles(const NCollection_Array1<double>& FP,
                    NCollection_Array1<gp_Pnt>&       Poles,
                    NCollection_Array1<double>&       Weights)
{
  int j      = FP.Lower();
  int PLower = Poles.Lower();
  int PUpper = Poles.Upper();

  for (int i = PLower; i <= PUpper; i++)
  {
    double w   = FP(j + 3);
    Weights(i) = w;
    gp_Pnt& P  = Poles(i);
    P.SetCoord(1, FP(j) / w);
    j++;
    P.SetCoord(2, FP(j) / w);
    j++;
    P.SetCoord(3, FP(j) / w);
    j++;
    j++;
  }
}

// specialized allocator for binomial coefficients using compile-time computation
namespace
{

//! Compile-time Pascal's triangle allocator for binomial coefficients
//! @tparam MaxDegree Maximum degree N for which C(N,P) can be computed
template <int MaxDegree>
class BinomAllocator
{
public:
  //! Constructor - computes Pascal's triangle at compile time
  //! Uses the recurrence relation: C(n,k) = C(n-1,k-1) + C(n-1,k)
  constexpr BinomAllocator()
      : myBinom{}
  {
    // Initialize first row: C(0,0) = 1
    myBinom[0][0] = 1;

    // Build Pascal's triangle row by row
    for (int i = 1; i <= MaxDegree; ++i)
    {
      // First and last elements are always 1
      myBinom[i][0] = 1;
      myBinom[i][i] = 1;

      // Use recurrence relation for middle elements
      for (int j = 1; j < i; ++j)
      {
        myBinom[i][j] = myBinom[i - 1][j - 1] + myBinom[i - 1][j];
      }
    }
  }

  //! Returns the binomial coefficient C(N,P)
  //! @param N the degree (n in C(n,k))
  //! @param P the parameter (k in C(n,k))
  //! @return the value of C(N,P)
  //! @note Caller must ensure N and P are within valid range
  constexpr int Value(const int N, const int P) const { return myBinom[N][P]; }

private:
  int myBinom[MaxDegree + 1][MaxDegree + 1];
};

//! Thread-safe lazy initialization of compile-time computed binomial coefficients
//! @tparam MaxDegree Maximum degree supported (default 25)
template <int MaxDegree = BSplCLib::MaxDegree()>
inline const BinomAllocator<MaxDegree>& GetBinomAllocator()
{
  static constexpr BinomAllocator<MaxDegree> THE_ALLOCATOR{};
  return THE_ALLOCATOR;
}

} // namespace

//=================================================================================================

double PLib::Bin(const int N, const int P)
{
  const auto& aBinom = GetBinomAllocator();

  Standard_OutOfRange_Raise_if(N < 0 || N > BSplCLib::MaxDegree(),
                               "PLib::Bin: degree N is out of supported range [0, 25]");
  Standard_OutOfRange_Raise_if(P < 0 || P > N,
                               "PLib::Bin: parameter P is out of valid range [0, N]");

  return double(aBinom.Value(N, P));
}

//=================================================================================================

void PLib::RationalDerivative(const int  Degree,
                              const int  DerivativeRequest,
                              const int  Dimension,
                              double&    Ders,
                              double&    RDers,
                              const bool All)
{
  //
  // Our purpose is to compute f = (u/v) derivated N = DerivativeRequest times
  //
  //  We Write  u = fv
  //  Let C(N,P) be the binomial
  //
  //        then we have
  //
  //         (q)                             (p)   (q-p)
  //        u    =     SUM          C (q,p) f     v
  //                p = 0 to q
  //
  //
  //        Therefore
  //
  //
  //         (q)         (   (q)                               (p)   (q-p)   )
  //        f    = (1/v) (  u    -     SUM            C (q,p) f     v        )
  //                     (          p = 0 to q-1                             )
  //
  //
  // make arrays for the binomial since computing it each time could raise a performance
  // issue
  // As oppose to the method below the <Der> array is organized in the following
  // fashion :
  //
  //  u (1)     u (2) ....   u  (Dimension)     v (1)
  //
  //   (1)       (1)          (1)                (1)
  //  u (1)     u (2) ....   u  (Dimension)     v (1)
  //
  //   ............................................
  //
  //   (Degree)  (Degree)     (Degree)           (Degree)
  //  u (1)     u (2) ....   u  (Dimension)     v (1)
  //
  //
  double                         Inverse;
  double*                        PolesArray    = &Ders;
  double*                        RationalArray = &RDers;
  double                         Factor;
  int                            ii, Index, OtherIndex, Index1, Index2, jj;
  NCollection_LocalArray<double> binomial_array;
  NCollection_LocalArray<double> derivative_storage;
  if (Dimension == 3)
  {
    int DeRequest1 = DerivativeRequest + 1;
    int MinDegRequ = DerivativeRequest;
    if (MinDegRequ > Degree)
      MinDegRequ = Degree;
    binomial_array.Allocate(DeRequest1);

    for (ii = 0; ii < DeRequest1; ii++)
    {
      binomial_array[ii] = 1.0e0;
    }
    if (!All)
    {
      int DimDeRequ1 = (DeRequest1 << 1) + DeRequest1;
      derivative_storage.Allocate(DimDeRequ1);
      RationalArray = derivative_storage;
    }

    Inverse    = 1.0e0 / PolesArray[3];
    Index      = 0;
    Index2     = -6;
    OtherIndex = 0;

    for (ii = 0; ii <= MinDegRequ; ii++)
    {
      Index2 += 3;
      Index1               = Index2;
      RationalArray[Index] = PolesArray[OtherIndex];
      Index++;
      OtherIndex++;
      RationalArray[Index] = PolesArray[OtherIndex];
      Index++;
      OtherIndex++;
      RationalArray[Index] = PolesArray[OtherIndex];
      Index -= 2;
      OtherIndex += 2;

      for (jj = ii - 1; jj >= 0; jj--)
      {
        Factor = binomial_array[jj] * PolesArray[((ii - jj) << 2) + 3];
        RationalArray[Index] -= Factor * RationalArray[Index1];
        Index++;
        Index1++;
        RationalArray[Index] -= Factor * RationalArray[Index1];
        Index++;
        Index1++;
        RationalArray[Index] -= Factor * RationalArray[Index1];
        Index -= 2;
        Index1 -= 5;
      }

      for (jj = ii; jj >= 1; jj--)
      {
        binomial_array[jj] += binomial_array[jj - 1];
      }
      RationalArray[Index] *= Inverse;
      Index++;
      RationalArray[Index] *= Inverse;
      Index++;
      RationalArray[Index] *= Inverse;
      Index++;
    }

    for (ii = MinDegRequ + 1; ii <= DerivativeRequest; ii++)
    {
      Index2 += 3;
      Index1               = Index2;
      RationalArray[Index] = 0.0e0;
      Index++;
      RationalArray[Index] = 0.0e0;
      Index++;
      RationalArray[Index] = 0.0e0;
      Index -= 2;

      for (jj = ii - 1; jj >= ii - MinDegRequ; jj--)
      {
        Factor = binomial_array[jj] * PolesArray[((ii - jj) << 2) + 3];
        RationalArray[Index] -= Factor * RationalArray[Index1];
        Index++;
        Index1++;
        RationalArray[Index] -= Factor * RationalArray[Index1];
        Index++;
        Index1++;
        RationalArray[Index] -= Factor * RationalArray[Index1];
        Index -= 2;
        Index1 -= 5;
      }

      for (jj = ii; jj >= 1; jj--)
      {
        binomial_array[jj] += binomial_array[jj - 1];
      }
      RationalArray[Index] *= Inverse;
      Index++;
      RationalArray[Index] *= Inverse;
      Index++;
      RationalArray[Index] *= Inverse;
      Index++;
    }

    if (!All)
    {
      RationalArray    = &RDers;
      int DimDeRequ    = (DerivativeRequest << 1) + DerivativeRequest;
      RationalArray[0] = derivative_storage[DimDeRequ];
      DimDeRequ++;
      RationalArray[1] = derivative_storage[DimDeRequ];
      DimDeRequ++;
      RationalArray[2] = derivative_storage[DimDeRequ];
    }
  }
  else
  {
    int kk;
    int Dimension1 = Dimension + 1;
    int Dimension2 = Dimension << 1;
    int DeRequest1 = DerivativeRequest + 1;
    int MinDegRequ = DerivativeRequest;
    if (MinDegRequ > Degree)
      MinDegRequ = Degree;
    binomial_array.Allocate(DeRequest1);

    for (ii = 0; ii < DeRequest1; ii++)
    {
      binomial_array[ii] = 1.0e0;
    }
    if (!All)
    {
      int DimDeRequ1 = Dimension * DeRequest1;
      derivative_storage.Allocate(DimDeRequ1);
      RationalArray = derivative_storage;
    }

    Inverse    = 1.0e0 / PolesArray[Dimension];
    Index      = 0;
    Index2     = -Dimension2;
    OtherIndex = 0;

    for (ii = 0; ii <= MinDegRequ; ii++)
    {
      Index2 += Dimension;
      Index1 = Index2;

      for (kk = 0; kk < Dimension; kk++)
      {
        RationalArray[Index] = PolesArray[OtherIndex];
        Index++;
        OtherIndex++;
      }
      Index -= Dimension;
      ++OtherIndex;

      for (jj = ii - 1; jj >= 0; jj--)
      {
        Factor = binomial_array[jj] * PolesArray[(ii - jj) * Dimension1 + Dimension];

        for (kk = 0; kk < Dimension; kk++)
        {
          RationalArray[Index] -= Factor * RationalArray[Index1];
          Index++;
          Index1++;
        }
        Index -= Dimension;
        Index1 -= Dimension2;
      }

      for (jj = ii; jj >= 1; jj--)
      {
        binomial_array[jj] += binomial_array[jj - 1];
      }

      for (kk = 0; kk < Dimension; kk++)
      {
        RationalArray[Index] *= Inverse;
        Index++;
      }
    }

    for (ii = MinDegRequ + 1; ii <= DerivativeRequest; ii++)
    {
      Index2 += Dimension;
      Index1 = Index2;

      for (kk = 0; kk < Dimension; kk++)
      {
        RationalArray[Index] = 0.0e0;
        Index++;
      }
      Index -= Dimension;

      for (jj = ii - 1; jj >= ii - MinDegRequ; jj--)
      {
        Factor = binomial_array[jj] * PolesArray[(ii - jj) * Dimension1 + Dimension];

        for (kk = 0; kk < Dimension; kk++)
        {
          RationalArray[Index] -= Factor * RationalArray[Index1];
          Index++;
          Index1++;
        }
        Index -= Dimension;
        Index1 -= Dimension2;
      }

      for (jj = ii; jj >= 1; jj--)
      {
        binomial_array[jj] += binomial_array[jj - 1];
      }

      for (kk = 0; kk < Dimension; kk++)
      {
        RationalArray[Index] *= Inverse;
        Index++;
      }
    }

    if (!All)
    {
      RationalArray = &RDers;
      int DimDeRequ = Dimension * DerivativeRequest;

      for (kk = 0; kk < Dimension; kk++)
      {
        RationalArray[kk] = derivative_storage[DimDeRequ];
        DimDeRequ++;
      }
    }
  }
}

//=======================================================================
// function : RationalDerivatives
// purpose  : Uses Homogeneous Poles Derivatives and Derivatives of Weights
//=======================================================================

void PLib::RationalDerivatives(const int DerivativeRequest,
                               const int Dimension,
                               double&   PolesDerivates,
                               // must be an array with
                               // (DerivativeRequest + 1) * Dimension slots
                               double& WeightsDerivates,
                               // must be an array with
                               // (DerivativeRequest + 1) slots
                               double& RationalDerivates)
{
  //
  // Our purpose is to compute f = (u/v) derivated N times
  //
  //  We Write  u = fv
  //  Let C(N,P) be the binomial
  //
  //        then we have
  //
  //         (q)                             (p)   (q-p)
  //        u    =     SUM          C (q,p) f     v
  //                p = 0 to q
  //
  //
  //        Therefore
  //
  //
  //         (q)         (   (q)                               (p)   (q-p)   )
  //        f    = (1/v) (  u    -     SUM            C (q,p) f     v        )
  //                     (          p = 0 to q-1                             )
  //
  //
  // make arrays for the binomial since computing it each time could
  // raize a performance issue
  //
  double  Inverse;
  double* PolesArray    = &PolesDerivates;
  double* WeightsArray  = &WeightsDerivates;
  double* RationalArray = &RationalDerivates;
  double  Factor;

  int ii, Index, Index1, Index2, jj;
  int DeRequest1 = DerivativeRequest + 1;

  NCollection_LocalArray<double> binomial_array(DeRequest1);
  NCollection_LocalArray<double> derivative_storage;

  for (ii = 0; ii < DeRequest1; ii++)
  {
    binomial_array[ii] = 1.0e0;
  }
  Inverse = 1.0e0 / WeightsArray[0];
  if (Dimension == 3)
  {
    Index  = 0;
    Index2 = -6;

    for (ii = 0; ii < DeRequest1; ii++)
    {
      Index2 += 3;
      Index1               = Index2;
      RationalArray[Index] = PolesArray[Index];
      Index++;
      RationalArray[Index] = PolesArray[Index];
      Index++;
      RationalArray[Index] = PolesArray[Index];
      Index -= 2;

      for (jj = ii - 1; jj >= 0; jj--)
      {
        Factor = binomial_array[jj] * WeightsArray[ii - jj];
        RationalArray[Index] -= Factor * RationalArray[Index1];
        Index++;
        Index1++;
        RationalArray[Index] -= Factor * RationalArray[Index1];
        Index++;
        Index1++;
        RationalArray[Index] -= Factor * RationalArray[Index1];
        Index -= 2;
        Index1 -= 5;
      }

      for (jj = ii; jj >= 1; jj--)
      {
        binomial_array[jj] += binomial_array[jj - 1];
      }
      RationalArray[Index] *= Inverse;
      Index++;
      RationalArray[Index] *= Inverse;
      Index++;
      RationalArray[Index] *= Inverse;
      Index++;
    }
  }
  else
  {
    int kk;
    int Dimension2 = Dimension << 1;
    Index          = 0;
    Index2         = -Dimension2;

    for (ii = 0; ii < DeRequest1; ii++)
    {
      Index2 += Dimension;
      Index1 = Index2;

      for (kk = 0; kk < Dimension; kk++)
      {
        RationalArray[Index] = PolesArray[Index];
        Index++;
      }
      Index -= Dimension;

      for (jj = ii - 1; jj >= 0; jj--)
      {
        Factor = binomial_array[jj] * WeightsArray[ii - jj];

        for (kk = 0; kk < Dimension; kk++)
        {
          RationalArray[Index] -= Factor * RationalArray[Index1];
          Index++;
          Index1++;
        }
        Index -= Dimension;
        Index1 -= Dimension2;
      }

      for (jj = ii; jj >= 1; jj--)
      {
        binomial_array[jj] += binomial_array[jj - 1];
      }

      for (kk = 0; kk < Dimension; kk++)
      {
        RationalArray[Index] *= Inverse;
        Index++;
      }
    }
  }
}

//=======================================================================
// Auxiliary template functions used for optimized evaluation of polynome
// and its derivatives for smaller dimensions of the polynome.
// Uses local arrays to enable register allocation and avoids memcpy/memset.
//=======================================================================

namespace
{
// Maximum dimension for optimized template dispatch
constexpr int THE_MAX_OPT_DIM = 15;

// Evaluation of only value using local array (register-friendly).
// Writes to output only at the end.
template <int dim>
inline void eval_poly0(double* theResult, const double* theCoeffs, int theDegree, double thePar)
{
  std::array<double, dim> aLocal;
  for (int i = 0; i < dim; ++i)
  {
    aLocal[i] = theCoeffs[i];
  }

  const double* aCoeffs = theCoeffs;
  for (int aDeg = 0; aDeg < theDegree; ++aDeg)
  {
    aCoeffs -= dim;
    for (int i = 0; i < dim; ++i)
    {
      aLocal[i] = aLocal[i] * thePar + aCoeffs[i];
    }
  }

  for (int i = 0; i < dim; ++i)
  {
    theResult[i] = aLocal[i];
  }
}

// Evaluation of value and first derivative using local arrays.
// Fuses derivative and value loops to read aLocal0 only once per iteration.
template <int dim>
inline void eval_poly1(double* theResult, const double* theCoeffs, int theDegree, double thePar)
{
  std::array<double, dim> aLocal0;
  std::array<double, dim> aLocal1;

  for (int i = 0; i < dim; ++i)
  {
    aLocal0[i] = theCoeffs[i];
    aLocal1[i] = 0.0;
  }

  const double* aCoeffs = theCoeffs;
  for (int aDeg = 0; aDeg < theDegree; ++aDeg)
  {
    aCoeffs -= dim;
    for (int i = 0; i < dim; ++i)
    {
      const double aVal = aLocal0[i];
      aLocal1[i]        = aLocal1[i] * thePar + aVal;
      aLocal0[i]        = aVal * thePar + aCoeffs[i];
    }
  }

  for (int i = 0; i < dim; ++i)
  {
    theResult[i]       = aLocal0[i];
    theResult[dim + i] = aLocal1[i];
  }
}

// Evaluation of value and first and second derivatives using local arrays.
// Fuses all three loops for better cache utilization.
template <int dim>
inline void eval_poly2(double* theResult, const double* theCoeffs, int theDegree, double thePar)
{
  std::array<double, dim> aLocal0;
  std::array<double, dim> aLocal1;
  std::array<double, dim> aLocal2;

  for (int i = 0; i < dim; ++i)
  {
    aLocal0[i] = theCoeffs[i];
    aLocal1[i] = 0.0;
    aLocal2[i] = 0.0;
  }

  const double* aCoeffs = theCoeffs;
  for (int aDeg = 0; aDeg < theDegree; ++aDeg)
  {
    aCoeffs -= dim;
    for (int i = 0; i < dim; ++i)
    {
      const double aD1  = aLocal1[i];
      const double aVal = aLocal0[i];
      aLocal2[i]        = aLocal2[i] * thePar + aD1 * 2.0;
      aLocal1[i]        = aD1 * thePar + aVal;
      aLocal0[i]        = aVal * thePar + aCoeffs[i];
    }
  }

  for (int i = 0; i < dim; ++i)
  {
    theResult[i]           = aLocal0[i];
    theResult[dim + i]     = aLocal1[i];
    theResult[2 * dim + i] = aLocal2[i];
  }
}

// Runtime evaluation for dimension > THE_MAX_OPT_DIM (value only)
inline void eval_poly0_runtime(double*       theResult,
                               const double* theCoeffs,
                               int           theDegree,
                               double        thePar,
                               int           theDimension)
{
  for (int i = 0; i < theDimension; ++i)
  {
    theResult[i] = theCoeffs[i];
  }

  const double* aCoeffs = theCoeffs;
  for (int aDeg = 0; aDeg < theDegree; ++aDeg)
  {
    aCoeffs -= theDimension;
    for (int i = 0; i < theDimension; ++i)
    {
      theResult[i] = theResult[i] * thePar + aCoeffs[i];
    }
  }
}

// Runtime evaluation for dimension > THE_MAX_OPT_DIM (value + 1st derivative)
inline void eval_poly1_runtime(double*       theResult,
                               const double* theCoeffs,
                               int           theDegree,
                               double        thePar,
                               int           theDimension)
{
  double* aRes0 = theResult;
  double* aRes1 = theResult + theDimension;

  for (int i = 0; i < theDimension; ++i)
  {
    aRes0[i] = theCoeffs[i];
    aRes1[i] = 0.0;
  }

  const double* aCoeffs = theCoeffs;
  for (int aDeg = 0; aDeg < theDegree; ++aDeg)
  {
    aCoeffs -= theDimension;
    for (int i = 0; i < theDimension; ++i)
    {
      const double aVal = aRes0[i];
      aRes1[i]          = aRes1[i] * thePar + aVal;
      aRes0[i]          = aVal * thePar + aCoeffs[i];
    }
  }
}

// Runtime evaluation for dimension > THE_MAX_OPT_DIM (value + 1st + 2nd derivatives)
inline void eval_poly2_runtime(double*       theResult,
                               const double* theCoeffs,
                               int           theDegree,
                               double        thePar,
                               int           theDimension)
{
  double* aRes0 = theResult;
  double* aRes1 = theResult + theDimension;
  double* aRes2 = theResult + 2 * theDimension;

  for (int i = 0; i < theDimension; ++i)
  {
    aRes0[i] = theCoeffs[i];
    aRes1[i] = 0.0;
    aRes2[i] = 0.0;
  }

  const double* aCoeffs = theCoeffs;
  for (int aDeg = 0; aDeg < theDegree; ++aDeg)
  {
    aCoeffs -= theDimension;
    for (int i = 0; i < theDimension; ++i)
    {
      const double aD1  = aRes1[i];
      const double aVal = aRes0[i];
      aRes2[i]          = aRes2[i] * thePar + aD1 * 2.0;
      aRes1[i]          = aD1 * thePar + aVal;
      aRes0[i]          = aVal * thePar + aCoeffs[i];
    }
  }
}

// Function pointer type for dispatch tables
using EvalPolyFunc = void (*)(double*, const double*, int, double);

// Helper to generate dispatch tables at compile time
template <template <int> class EvalFunc, typename FuncPtr, int... Is>
constexpr std::array<FuncPtr, sizeof...(Is)> makeDispatchTable(std::integer_sequence<int, Is...>)
{
  return {{&EvalFunc<Is + 1>::call...}};
}

// Wrapper structs to allow template parameter deduction
template <int Dim>
struct EvalPoly0Wrapper
{
  static void call(double* r, const double* c, int d, double p) { eval_poly0<Dim>(r, c, d, p); }
};

template <int Dim>
struct EvalPoly1Wrapper
{
  static void call(double* r, const double* c, int d, double p) { eval_poly1<Dim>(r, c, d, p); }
};

template <int Dim>
struct EvalPoly2Wrapper
{
  static void call(double* r, const double* c, int d, double p) { eval_poly2<Dim>(r, c, d, p); }
};

// Dispatch tables for dimensions 1..15
constexpr std::array<EvalPolyFunc, THE_MAX_OPT_DIM> THE_EVAL_POLY0_TABLE =
  makeDispatchTable<EvalPoly0Wrapper, EvalPolyFunc>(
    std::make_integer_sequence<int, THE_MAX_OPT_DIM>{});

constexpr std::array<EvalPolyFunc, THE_MAX_OPT_DIM> THE_EVAL_POLY1_TABLE =
  makeDispatchTable<EvalPoly1Wrapper, EvalPolyFunc>(
    std::make_integer_sequence<int, THE_MAX_OPT_DIM>{});

constexpr std::array<EvalPolyFunc, THE_MAX_OPT_DIM> THE_EVAL_POLY2_TABLE =
  makeDispatchTable<EvalPoly2Wrapper, EvalPolyFunc>(
    std::make_integer_sequence<int, THE_MAX_OPT_DIM>{});

} // namespace

//=======================================================================
// function : This evaluates a polynomial and its derivatives
// purpose  : up to the requested order
//=======================================================================

void PLib::EvalPolynomial(const double  Par,
                          const int     DerivativeRequest,
                          const int     Degree,
                          const int     Dimension,
                          const double& PolynomialCoeff,
                          double&       Results)
//
// the polynomial coefficients are assumed to be stored as follows :
//                                                      0
// [0]                  [Dimension -1]                 X     coefficient
//                                                      1
// [Dimension]          [Dimension + Dimension -1]     X     coefficient
//                                                      2
// [2 * Dimension]      [2 * Dimension + Dimension-1]  X     coefficient
//
//   ...................................................
//
//
//                                                      d
// [d * Dimension]      [d * Dimension + Dimension-1]  X     coefficient
//
//  where d is the Degree
//
{
  const double* aCoeffs = &PolynomialCoeff + Degree * Dimension;
  double*       aRes    = &Results;

  switch (DerivativeRequest)
  {
    case 1: {
      if (Dimension >= 1 && Dimension <= THE_MAX_OPT_DIM)
      {
        THE_EVAL_POLY1_TABLE[Dimension - 1](aRes, aCoeffs, Degree, Par);
      }
      else
      {
        eval_poly1_runtime(aRes, aCoeffs, Degree, Par, Dimension);
      }
      break;
    }
    case 2: {
      if (Dimension >= 1 && Dimension <= THE_MAX_OPT_DIM)
      {
        THE_EVAL_POLY2_TABLE[Dimension - 1](aRes, aCoeffs, Degree, Par);
      }
      else
      {
        eval_poly2_runtime(aRes, aCoeffs, Degree, Par, Dimension);
      }
      break;
    }
    default: {
      // General case for DerivativeRequest > 2
      const int aResSize = (1 + DerivativeRequest) * Dimension;
      for (int i = 0; i < aResSize; ++i)
      {
        aRes[i] = 0.0;
      }

      for (int aDeg = 0; aDeg <= Degree; ++aDeg)
      {
        double* aPtr = aRes + aResSize - Dimension;
        // Calculating derivatives of the polynomial
        for (int aDeriv = DerivativeRequest; aDeriv > 0; --aDeriv)
        {
          double* anOriginal = aPtr - Dimension;
          for (int ind = 0; ind < Dimension; ++ind)
          {
            aPtr[ind] = aPtr[ind] * Par + anOriginal[ind] * aDeriv;
          }
          aPtr = anOriginal;
        }
        // Calculating the value of the polynomial
        for (int ind = 0; ind < Dimension; ++ind)
        {
          aPtr[ind] = aPtr[ind] * Par + aCoeffs[ind];
        }
        aCoeffs -= Dimension;
      }
    }
  }
}

//=================================================================================================

void PLib::NoDerivativeEvalPolynomial(const double  Par,
                                      const int     Degree,
                                      const int     Dimension,
                                      const int     DegreeDimension,
                                      const double& PolynomialCoeff,
                                      double&       Results)
{
  const double* aCoeffs = &PolynomialCoeff + DegreeDimension;
  double*       aRes    = &Results;

  if (Dimension >= 1 && Dimension <= THE_MAX_OPT_DIM)
  {
    THE_EVAL_POLY0_TABLE[Dimension - 1](aRes, aCoeffs, Degree, Par);
  }
  else
  {
    eval_poly0_runtime(aRes, aCoeffs, Degree, Par, Dimension);
  }
}

//=======================================================================
// function : This evaluates a polynomial of 2 variables
// purpose  : or its derivative at the requested orders
//=======================================================================

void PLib::EvalPoly2Var(const double UParameter,
                        const double VParameter,
                        const int    UDerivativeRequest,
                        const int    VDerivativeRequest,
                        const int    UDegree,
                        const int    VDegree,
                        const int    Dimension,
                        double&      PolynomialCoeff,
                        double&      Results)
//
// the polynomial coefficients are assumed to be stored as follows :
//                                                      0 0
// [0]                  [Dimension -1]                 U V    coefficient
//                                                      1 0
// [Dimension]          [Dimension + Dimension -1]     U V    coefficient
//                                                      2 0
// [2 * Dimension]      [2 * Dimension + Dimension-1]  U V    coefficient
//
//   ...................................................
//
//
//                                                      m 0
// [m * Dimension]      [m * Dimension + Dimension-1]  U V    coefficient
//
//  where m = UDegree
//
//                                                           0 1
// [(m+1) * Dimension]  [(m+1) * Dimension + Dimension-1]   U V   coefficient
//
//   ...................................................
//
//                                                          m 1
// [2*m * Dimension]      [2*m * Dimension + Dimension-1]  U V    coefficient
//
//   ...................................................
//
//                                                          m n
// [m*n * Dimension]      [m*n * Dimension + Dimension-1]  U V    coefficient
//
//  where n = VDegree
{
  int                        Udim = (VDegree + 1) * Dimension, index = Udim * UDerivativeRequest;
  NCollection_Array1<double> Curve(1, Udim * (UDerivativeRequest + 1));
  NCollection_Array1<double> Point(1, Dimension * (VDerivativeRequest + 1));
  double*                    Result = (double*)&Curve.ChangeValue(1);
  double*                    Digit  = (double*)&Point.ChangeValue(1);
  double*                    ResultArray;
  ResultArray = &Results;

  PLib::EvalPolynomial(UParameter, UDerivativeRequest, UDegree, Udim, PolynomialCoeff, Result[0]);

  PLib::EvalPolynomial(VParameter, VDerivativeRequest, VDegree, Dimension, Result[index], Digit[0]);

  index = Dimension * VDerivativeRequest;

  for (int i = 0; i < Dimension; i++)
  {
    ResultArray[i] = Digit[index + i];
  }
}

//=======================================================================
// function : This evaluates the lagrange polynomial and its derivatives
// purpose  : up to the requested order that interpolates a series of
// points of dimension <Dimension> with given assigned parameters
//=======================================================================

int PLib::EvalLagrange(const double Parameter,
                       const int    DerivativeRequest,
                       const int    Degree,
                       const int    Dimension,
                       double&      Values,
                       double&      Parameters,
                       double&      Results)
{
  //
  // the points  are assumed to be stored as follows in the Values array :
  //
  // [0]              [Dimension -1]                first  point   coefficients
  //
  // [Dimension]      [Dimension + Dimension -1]    second point   coefficients
  //
  // [2 * Dimension]  [2 * Dimension + Dimension-1] third  point   coefficients
  //
  //   ...................................................
  //
  //
  //
  // [d * Dimension]  [d * Dimension + Dimension-1] d + 1 point   coefficients
  //
  //  where d is the Degree
  //
  //  The ParameterArray stores the parameter value assign to each point in
  //  order described above, that is
  //  [0]   is assign to first  point
  //  [1]   is assign to second point
  //
  int     ii, jj, kk, Index, Index1, ReturnCode = 0;
  int     local_request = DerivativeRequest;
  double* ParameterArray;
  double  difference;
  double* PointsArray;
  double* ResultArray;

  PointsArray    = &Values;
  ParameterArray = &Parameters;
  ResultArray    = &Results;
  if (local_request >= Degree)
  {
    local_request = Degree;
  }
  NCollection_LocalArray<double> divided_differences_array((Degree + 1) * Dimension);
  //
  //  Build the divided differences array
  //

  for (ii = 0; ii < (Degree + 1) * Dimension; ii++)
  {
    divided_differences_array[ii] = PointsArray[ii];
  }

  for (ii = Degree; ii >= 0; ii--)
  {

    for (jj = Degree; jj > Degree - ii; jj--)
    {
      Index  = jj * Dimension;
      Index1 = Index - Dimension;

      for (kk = 0; kk < Dimension; kk++)
      {
        divided_differences_array[Index + kk] -= divided_differences_array[Index1 + kk];
      }
      difference = ParameterArray[jj] - ParameterArray[jj - Degree - 1 + ii];
      if (std::abs(difference) < RealSmall())
      {
        ReturnCode = 1;
        goto FINISH;
      }
      difference = 1.0e0 / difference;

      for (kk = 0; kk < Dimension; kk++)
      {
        divided_differences_array[Index + kk] *= difference;
      }
    }
  }
  //
  //
  // Evaluate the divided difference array polynomial which expresses as
  //
  //  P(t) = [t1] P + (t - t1) [t1,t2] P + (t - t1)(t - t2)[t1,t2,t3] P + ...
  //         + (t - t1)(t - t2)(t - t3)...(t - td) [t1,t2,...,td+1] P
  //
  // The ith slot in the divided_differences_array is [t1,t2,...,ti+1]
  //
  //
  Index = Degree * Dimension;

  for (kk = 0; kk < Dimension; kk++)
  {
    ResultArray[kk] = divided_differences_array[Index + kk];
  }

  for (ii = Dimension; ii < (local_request + 1) * Dimension; ii++)
  {
    ResultArray[ii] = 0.0e0;
  }

  for (ii = Degree; ii >= 1; ii--)
  {
    difference = Parameter - ParameterArray[ii - 1];

    for (jj = local_request; jj > 0; jj--)
    {
      Index  = jj * Dimension;
      Index1 = Index - Dimension;

      for (kk = 0; kk < Dimension; kk++)
      {
        ResultArray[Index + kk] *= difference;
        ResultArray[Index + kk] += ResultArray[Index1 + kk] * (double)jj;
      }
    }
    Index = (ii - 1) * Dimension;

    for (kk = 0; kk < Dimension; kk++)
    {
      ResultArray[kk] *= difference;
      ResultArray[kk] += divided_differences_array[Index + kk];
    }
  }
FINISH:
  return (ReturnCode);
}

//=======================================================================
// function : This evaluates the hermite polynomial and its derivatives
// purpose  : up to the requested order that interpolates a series of
// points of dimension <Dimension> with given assigned parameters
//=======================================================================

int PLib::EvalCubicHermite(const double Parameter,
                           const int    DerivativeRequest,
                           const int    Dimension,
                           double&      Values,
                           double&      Derivatives,
                           double&      theParameters,
                           double&      Results)
{
  //
  // the points  are assumed to be stored as follows in the Values array :
  //
  // [0]            [Dimension -1]             first  point   coefficients
  //
  // [Dimension]    [Dimension + Dimension -1] last point   coefficients
  //
  //
  // the derivatives  are assumed to be stored as follows in
  // the Derivatives array :
  //
  // [0]            [Dimension -1]             first  point   coefficients
  //
  // [Dimension]    [Dimension + Dimension -1] last point   coefficients
  //
  //  The ParameterArray stores the parameter value assign to each point in
  //  order described above, that is
  //  [0]   is assign to first  point
  //  [1]   is assign to last   point
  //
  int ii, jj, kk, pp, Index, Index1, Degree, ReturnCode;
  int local_request = DerivativeRequest;

  ReturnCode = 0;
  Degree     = 3;
  double  ParametersArray[4];
  double  difference;
  double  inverse;
  double* FirstLast;
  double* PointsArray;
  double* DerivativesArray;
  double* ResultArray;

  DerivativesArray = &Derivatives;
  PointsArray      = &Values;
  FirstLast        = &theParameters;
  ResultArray      = &Results;
  if (local_request >= Degree)
  {
    local_request = Degree;
  }
  NCollection_LocalArray<double> divided_differences_array((Degree + 1) * Dimension);

  for (ii = 0, jj = 0; ii < 2; ii++, jj += 2)
  {
    ParametersArray[jj] = ParametersArray[jj + 1] = FirstLast[ii];
  }
  //
  //  Build the divided differences array
  //
  //
  // initialise it at the stage 2 of the building algorithm
  // for divided differences
  //
  inverse = FirstLast[1] - FirstLast[0];
  inverse = 1.0e0 / inverse;

  for (ii = 0, jj = Dimension, kk = 2 * Dimension, pp = 3 * Dimension; ii < Dimension;
       ii++, jj++, kk++, pp++)
  {
    divided_differences_array[ii] = PointsArray[ii];
    divided_differences_array[kk] = inverse * (PointsArray[jj] - PointsArray[ii]);
    divided_differences_array[jj] = DerivativesArray[ii];
    divided_differences_array[pp] = DerivativesArray[jj];
  }

  for (ii = 1; ii <= Degree; ii++)
  {

    for (jj = Degree; jj >= ii + 1; jj--)
    {
      Index  = jj * Dimension;
      Index1 = Index - Dimension;

      for (kk = 0; kk < Dimension; kk++)
      {
        divided_differences_array[Index + kk] -= divided_differences_array[Index1 + kk];
      }

      for (kk = 0; kk < Dimension; kk++)
      {
        divided_differences_array[Index + kk] *= inverse;
      }
    }
  }
  //
  //
  // Evaluate the divided difference array polynomial which expresses as
  //
  //  P(t) = [t1] P + (t - t1) [t1,t2] P + (t - t1)(t - t2)[t1,t2,t3] P + ...
  //         + (t - t1)(t - t2)(t - t3)...(t - td) [t1,t2,...,td+1] P
  //
  // The ith slot in the divided_differences_array is [t1,t2,...,ti+1]
  //
  //
  Index = Degree * Dimension;

  for (kk = 0; kk < Dimension; kk++)
  {
    ResultArray[kk] = divided_differences_array[Index + kk];
  }

  for (ii = Dimension; ii < (local_request + 1) * Dimension; ii++)
  {
    ResultArray[ii] = 0.0e0;
  }

  for (ii = Degree; ii >= 1; ii--)
  {
    difference = Parameter - ParametersArray[ii - 1];

    for (jj = local_request; jj > 0; jj--)
    {
      Index  = jj * Dimension;
      Index1 = Index - Dimension;

      for (kk = 0; kk < Dimension; kk++)
      {
        ResultArray[Index + kk] *= difference;
        ResultArray[Index + kk] += ResultArray[Index1 + kk] * (double)jj;
      }
    }
    Index = (ii - 1) * Dimension;

    for (kk = 0; kk < Dimension; kk++)
    {
      ResultArray[kk] *= difference;
      ResultArray[kk] += divided_differences_array[Index + kk];
    }
  }
  //  FINISH :
  return (ReturnCode);
}

//=======================================================================
// function : HermiteCoefficients
// purpose  : calcul des polynomes d'Hermite
//=======================================================================

bool PLib::HermiteCoefficients(const double FirstParameter,
                               const double LastParameter,
                               const int    FirstOrder,
                               const int    LastOrder,
                               math_Matrix& MatrixCoefs)
{
  int         NbCoeff = FirstOrder + LastOrder + 2, Ordre[2];
  int         ii, jj, pp, cote, iof = 0;
  double      Prod, TBorne = FirstParameter;
  math_Vector Coeff(1, NbCoeff), B(1, NbCoeff, 0.0);
  math_Matrix MAT(1, NbCoeff, 1, NbCoeff, 0.0);

  // Test de validites

  if ((FirstOrder < 0) || (LastOrder < 0))
    return false;
  double D1 = fabs(FirstParameter), D2 = fabs(LastParameter);
  if (D1 > 100 || D2 > 100)
    return false;
  D2 += D1;
  if (D2 < 0.01)
    return false;
  if (fabs(LastParameter - FirstParameter) / D2 < 0.01)
    return false;

  // Calcul de la matrice a inverser (MAT)

  Ordre[0] = FirstOrder + 1;
  Ordre[1] = LastOrder + 1;

  for (cote = 0; cote <= 1; cote++)
  {
    Coeff.Init(1);

    for (pp = 1; pp <= Ordre[cote]; pp++)
    {
      ii   = pp + iof;
      Prod = 1;

      for (jj = pp; jj <= NbCoeff; jj++)
      {
        //        tout se passe dans les 3 lignes suivantes
        MAT(ii, jj) = Coeff(jj) * Prod;
        Coeff(jj) *= jj - pp;
        Prod *= TBorne;
      }
    }
    TBorne = LastParameter;
    iof    = Ordre[0];
  }

  // resolution du systemes
  math_Gauss ResolCoeff(MAT, 1.0e-10);
  if (!ResolCoeff.IsDone())
    return false;

  for (ii = 1; ii <= NbCoeff; ii++)
  {
    B(ii) = 1;
    ResolCoeff.Solve(B, Coeff);
    MatrixCoefs.SetRow(ii, Coeff);
    B(ii) = 0;
  }
  return true;
}

//=================================================================================================

void PLib::CoefficientsPoles(const NCollection_Array1<gp_Pnt>& Coefs,
                             const NCollection_Array1<double>* WCoefs,
                             NCollection_Array1<gp_Pnt>&       Poles,
                             NCollection_Array1<double>*       Weights)
{
  NCollection_Array1<double> tempC(1, 3 * Coefs.Length());
  PLib::SetPoles(Coefs, tempC);
  NCollection_Array1<double> tempP(1, 3 * Poles.Length());
  PLib::SetPoles(Coefs, tempP);
  PLib::CoefficientsPoles(3, tempC, WCoefs, tempP, Weights);
  PLib::GetPoles(tempP, Poles);
}

//=================================================================================================

void PLib::CoefficientsPoles(const NCollection_Array1<gp_Pnt2d>& Coefs,
                             const NCollection_Array1<double>*   WCoefs,
                             NCollection_Array1<gp_Pnt2d>&       Poles,
                             NCollection_Array1<double>*         Weights)
{
  NCollection_Array1<double> tempC(1, 2 * Coefs.Length());
  PLib::SetPoles(Coefs, tempC);
  NCollection_Array1<double> tempP(1, 2 * Poles.Length());
  PLib::SetPoles(Coefs, tempP);
  PLib::CoefficientsPoles(2, tempC, WCoefs, tempP, Weights);
  PLib::GetPoles(tempP, Poles);
}

//=================================================================================================

void PLib::CoefficientsPoles(const NCollection_Array1<double>& Coefs,
                             const NCollection_Array1<double>* WCoefs,
                             NCollection_Array1<double>&       Poles,
                             NCollection_Array1<double>*       Weights)
{
  PLib::CoefficientsPoles(1, Coefs, WCoefs, Poles, Weights);
}

//=================================================================================================

void PLib::CoefficientsPoles(const int                         dim,
                             const NCollection_Array1<double>& Coefs,
                             const NCollection_Array1<double>* WCoefs,
                             NCollection_Array1<double>&       Poles,
                             NCollection_Array1<double>*       Weights)
{
  bool rat    = WCoefs != NULL;
  int  loc    = Coefs.Lower();
  int  lop    = Poles.Lower();
  int  lowc   = 0;
  int  lowp   = 0;
  int  upc    = Coefs.Upper();
  int  upp    = Poles.Upper();
  int  upwc   = 0;
  int  upwp   = 0;
  int  reflen = Coefs.Length() / dim;
  int  i, j, k;
  // Les Extremites.
  if (rat)
  {
    lowc = WCoefs->Lower();
    lowp = Weights->Lower();
    upwc = WCoefs->Upper();
    upwp = Weights->Upper();
  }

  for (i = 0; i < dim; i++)
  {
    Poles(lop + i) = Coefs(loc + i);
    Poles(upp - i) = Coefs(upc - i);
  }
  if (rat)
  {
    (*Weights)(lowp) = (*WCoefs)(lowc);
    (*Weights)(upwp) = (*WCoefs)(upwc);
  }

  double Cnp;
  for (i = 2; i < reflen; i++)
  {
    Cnp = PLib::Bin(reflen - 1, i - 1);
    if (rat)
      (*Weights)(lowp + i - 1) = (*WCoefs)(lowc + i - 1) / Cnp;

    for (j = 0; j < dim; j++)
    {
      Poles(lop + dim * (i - 1) + j) = Coefs(loc + dim * (i - 1) + j) / Cnp;
    }
  }

  for (i = 1; i <= reflen - 1; i++)
  {

    for (j = reflen - 1; j >= i; j--)
    {
      if (rat)
        (*Weights)(lowp + j) += (*Weights)(lowp + j - 1);

      for (k = 0; k < dim; k++)
      {
        Poles(lop + dim * j + k) += Poles(lop + dim * (j - 1) + k);
      }
    }
  }
  if (rat)
  {

    for (i = 1; i <= reflen; i++)
    {

      for (j = 0; j < dim; j++)
      {
        Poles(lop + dim * (i - 1) + j) /= (*Weights)(lowp + i - 1);
      }
    }
  }
}

//=================================================================================================

void PLib::Trimming(const double                U1,
                    const double                U2,
                    NCollection_Array1<gp_Pnt>& Coefs,
                    NCollection_Array1<double>* WCoefs)
{
  NCollection_Array1<double> temp(1, 3 * Coefs.Length());
  PLib::SetPoles(Coefs, temp);
  PLib::Trimming(U1, U2, 3, temp, WCoefs);
  PLib::GetPoles(temp, Coefs);
}

//=================================================================================================

void PLib::Trimming(const double                  U1,
                    const double                  U2,
                    NCollection_Array1<gp_Pnt2d>& Coefs,
                    NCollection_Array1<double>*   WCoefs)
{
  NCollection_Array1<double> temp(1, 2 * Coefs.Length());
  PLib::SetPoles(Coefs, temp);
  PLib::Trimming(U1, U2, 2, temp, WCoefs);
  PLib::GetPoles(temp, Coefs);
}

//=================================================================================================

void PLib::Trimming(const double                U1,
                    const double                U2,
                    NCollection_Array1<double>& Coefs,
                    NCollection_Array1<double>* WCoefs)
{
  PLib::Trimming(U1, U2, 1, Coefs, WCoefs);
}

//=================================================================================================

void PLib::Trimming(const double                U1,
                    const double                U2,
                    const int                   dim,
                    NCollection_Array1<double>& Coefs,
                    NCollection_Array1<double>* WCoefs)
{

  // principe :
  // on fait le changement de variable v = (u-U1) / (U2-U1)
  // on exprime u = f(v) que l'on remplace dans l'expression polynomiale
  // decomposee sous la forme du schema iteratif de horner.

  double lsp = U2 - U1;
  int    indc, indw = 0;
  int    upc = Coefs.Upper() - dim + 1, upw = 0;
  int    len = Coefs.Length() / dim;
  bool   rat = WCoefs != NULL;

  if (rat)
  {
    if (len != WCoefs->Length())
      throw Standard_Failure("PLib::Trimming : nbcoefs/dim != nbweights !!!");
    upw = WCoefs->Upper();
  }
  len--;

  for (int i = 1; i <= len; i++)
  {
    int j;
    indc = upc - dim * (i - 1);
    if (rat)
      indw = upw - i + 1;
    // calcul du coefficient de degre le plus faible a l'iteration i

    for (j = 0; j < dim; j++)
    {
      Coefs(indc - dim + j) += U1 * Coefs(indc + j);
    }
    if (rat)
      (*WCoefs)(indw - 1) += U1 * (*WCoefs)(indw);

    // calcul des coefficients intermediaires :

    while (indc < upc)
    {
      indc += dim;

      for (int k = 0; k < dim; k++)
      {
        Coefs(indc - dim + k) = U1 * Coefs(indc + k) + lsp * Coefs(indc - dim + k);
      }
      if (rat)
      {
        indw++;
        (*WCoefs)(indw - 1) = U1 * (*WCoefs)(indw) + lsp * (*WCoefs)(indw - 1);
      }
    }

    // calcul du coefficient de degre le plus eleve :

    for (j = 0; j < dim; j++)
    {
      Coefs(upc + j) *= lsp;
    }
    if (rat)
      (*WCoefs)(upw) *= lsp;
  }
}

//=======================================================================
// function : CoefficientsPoles
// purpose  :
// Modified: 21/10/1996 by PMN :  PolesCoefficient (PRO5852).
// on ne bidouille plus les u et v c'est a l'appelant de savoir ce qu'il
// fait avec BuildCache ou plus simplement d'utiliser PolesCoefficients
//=======================================================================

void PLib::CoefficientsPoles(const NCollection_Array2<gp_Pnt>& Coefs,
                             const NCollection_Array2<double>* WCoefs,
                             NCollection_Array2<gp_Pnt>&       Poles,
                             NCollection_Array2<double>*       Weights)
{
  bool rat       = (WCoefs != NULL);
  int  LowerRow  = Poles.LowerRow();
  int  UpperRow  = Poles.UpperRow();
  int  LowerCol  = Poles.LowerCol();
  int  UpperCol  = Poles.UpperCol();
  int  ColLength = Poles.ColLength();
  int  RowLength = Poles.RowLength();

  // Bidouille pour retablir u et v pour les coefs calcules
  // par buildcache
  //  bool inv = false; //ColLength != Coefs.ColLength();

  int    Row, Col;
  double W, Cnp;

  int    I1, I2;
  int    NPoleu, NPolev;
  gp_XYZ Temp;

  for (NPoleu = LowerRow; NPoleu <= UpperRow; NPoleu++)
  {
    Poles(NPoleu, LowerCol) = Coefs(NPoleu, LowerCol);
    if (rat)
    {
      (*Weights)(NPoleu, LowerCol) = (*WCoefs)(NPoleu, LowerCol);
    }

    for (Col = LowerCol + 1; Col <= UpperCol - 1; Col++)
    {
      Cnp  = PLib::Bin(RowLength - 1, Col - LowerCol);
      Temp = Coefs(NPoleu, Col).XYZ();
      Temp.Divide(Cnp);
      Poles(NPoleu, Col).SetXYZ(Temp);
      if (rat)
      {
        (*Weights)(NPoleu, Col) = (*WCoefs)(NPoleu, Col) / Cnp;
      }
    }
    Poles(NPoleu, UpperCol) = Coefs(NPoleu, UpperCol);
    if (rat)
    {
      (*Weights)(NPoleu, UpperCol) = (*WCoefs)(NPoleu, UpperCol);
    }

    for (I1 = 1; I1 <= RowLength - 1; I1++)
    {

      for (I2 = UpperCol; I2 >= LowerCol + I1; I2--)
      {
        Temp.SetLinearForm(Poles(NPoleu, I2).XYZ(), Poles(NPoleu, I2 - 1).XYZ());
        Poles(NPoleu, I2).SetXYZ(Temp);
        if (rat)
          (*Weights)(NPoleu, I2) += (*Weights)(NPoleu, I2 - 1);
      }
    }
  }

  for (NPolev = LowerCol; NPolev <= UpperCol; NPolev++)
  {

    for (Row = LowerRow + 1; Row <= UpperRow - 1; Row++)
    {
      Cnp  = PLib::Bin(ColLength - 1, Row - LowerRow);
      Temp = Poles(Row, NPolev).XYZ();
      Temp.Divide(Cnp);
      Poles(Row, NPolev).SetXYZ(Temp);
      if (rat)
        (*Weights)(Row, NPolev) /= Cnp;
    }

    for (I1 = 1; I1 <= ColLength - 1; I1++)
    {

      for (I2 = UpperRow; I2 >= LowerRow + I1; I2--)
      {
        Temp.SetLinearForm(Poles(I2, NPolev).XYZ(), Poles(I2 - 1, NPolev).XYZ());
        Poles(I2, NPolev).SetXYZ(Temp);
        if (rat)
          (*Weights)(I2, NPolev) += (*Weights)(I2 - 1, NPolev);
      }
    }
  }
  if (rat)
  {

    for (Row = LowerRow; Row <= UpperRow; Row++)
    {

      for (Col = LowerCol; Col <= UpperCol; Col++)
      {
        W    = (*Weights)(Row, Col);
        Temp = Poles(Row, Col).XYZ();
        Temp.Divide(W);
        Poles(Row, Col).SetXYZ(Temp);
      }
    }
  }
}

//=================================================================================================

void PLib::UTrimming(const double                U1,
                     const double                U2,
                     NCollection_Array2<gp_Pnt>& Coeffs,
                     NCollection_Array2<double>* WCoeffs)
{
  bool                       rat = WCoeffs != NULL;
  int                        lr  = Coeffs.LowerRow();
  int                        ur  = Coeffs.UpperRow();
  int                        lc  = Coeffs.LowerCol();
  int                        uc  = Coeffs.UpperCol();
  NCollection_Array1<gp_Pnt> Temp(lr, ur);
  NCollection_Array1<double> Temw(lr, ur);

  for (int icol = lc; icol <= uc; icol++)
  {
    int irow;
    for (irow = lr; irow <= ur; irow++)
    {
      Temp(irow) = Coeffs(irow, icol);
      if (rat)
        Temw(irow) = (*WCoeffs)(irow, icol);
    }
    if (rat)
      PLib::Trimming(U1, U2, Temp, &Temw);
    else
      PLib::Trimming(U1, U2, Temp, PLib::NoWeights());

    for (irow = lr; irow <= ur; irow++)
    {
      Coeffs(irow, icol) = Temp(irow);
      if (rat)
        (*WCoeffs)(irow, icol) = Temw(irow);
    }
  }
}

//=================================================================================================

void PLib::VTrimming(const double                V1,
                     const double                V2,
                     NCollection_Array2<gp_Pnt>& Coeffs,
                     NCollection_Array2<double>* WCoeffs)
{
  bool                       rat = WCoeffs != NULL;
  int                        lr  = Coeffs.LowerRow();
  int                        ur  = Coeffs.UpperRow();
  int                        lc  = Coeffs.LowerCol();
  int                        uc  = Coeffs.UpperCol();
  NCollection_Array1<gp_Pnt> Temp(lc, uc);
  NCollection_Array1<double> Temw(lc, uc);

  for (int irow = lr; irow <= ur; irow++)
  {
    int icol;
    for (icol = lc; icol <= uc; icol++)
    {
      Temp(icol) = Coeffs(irow, icol);
      if (rat)
        Temw(icol) = (*WCoeffs)(irow, icol);
    }
    if (rat)
      PLib::Trimming(V1, V2, Temp, &Temw);
    else
      PLib::Trimming(V1, V2, Temp, PLib::NoWeights());

    for (icol = lc; icol <= uc; icol++)
    {
      Coeffs(irow, icol) = Temp(icol);
      if (rat)
        (*WCoeffs)(irow, icol) = Temw(icol);
    }
  }
}

//=================================================================================================

bool PLib::HermiteInterpolate(const int                         Dimension,
                              const double                      FirstParameter,
                              const double                      LastParameter,
                              const int                         FirstOrder,
                              const int                         LastOrder,
                              const NCollection_Array2<double>& FirstConstr,
                              const NCollection_Array2<double>& LastConstr,
                              NCollection_Array1<double>&       Coefficients)
{
  double Pattern[3][6];

  // portage HP : il faut les initialiser 1 par 1

  Pattern[0][0] = 1;
  Pattern[0][1] = 1;
  Pattern[0][2] = 1;
  Pattern[0][3] = 1;
  Pattern[0][4] = 1;
  Pattern[0][5] = 1;
  Pattern[1][0] = 0;
  Pattern[1][1] = 1;
  Pattern[1][2] = 2;
  Pattern[1][3] = 3;
  Pattern[1][4] = 4;
  Pattern[1][5] = 5;
  Pattern[2][0] = 0;
  Pattern[2][1] = 0;
  Pattern[2][2] = 2;
  Pattern[2][3] = 6;
  Pattern[2][4] = 12;
  Pattern[2][5] = 20;

  math_Matrix A(0, FirstOrder + LastOrder + 1, 0, FirstOrder + LastOrder + 1);
  //  The initialisation of the matrix A
  int irow;
  for (irow = 0; irow <= FirstOrder; irow++)
  {
    double FirstVal = 1.;

    for (int icol = 0; icol <= FirstOrder + LastOrder + 1; icol++)
    {
      A(irow, icol) = Pattern[irow][icol] * FirstVal;
      if (irow <= icol)
        FirstVal *= FirstParameter;
    }
  }

  for (irow = 0; irow <= LastOrder; irow++)
  {
    double LastVal = 1.;

    for (int icol = 0; icol <= FirstOrder + LastOrder + 1; icol++)
    {
      A(irow + FirstOrder + 1, icol) = Pattern[irow][icol] * LastVal;
      if (irow <= icol)
        LastVal *= LastParameter;
    }
  }
  //
  //  The filled matrix A for FirstOrder=LastOrder=2 is:
  //
  //      1   FP  FP**2   FP**3    FP**4     FP**5
  //      0   1   2*FP    3*FP**2  4*FP**3   5*FP**4        FP - FirstParameter
  //      0   0   2       6*FP     12*FP**2  20*FP**3
  //      1   LP  LP**2   LP**3    LP**4     LP**5
  //      0   1   2*LP    3*LP**2  4*LP**3   5*LP**4        LP - LastParameter
  //      0   0   2       6*LP     12*LP**2  20*LP**3
  //
  //  If FirstOrder or LastOrder <=2 then some rows and columns are missing.
  //  For example:
  //  If FirstOrder=1 then 3th row and 6th column are missing
  //  If FirstOrder=LastOrder=0 then 2,3,5,6th rows and 3,4,5,6th columns are missing

  math_Gauss Equations(A);
  //  std::cout << "A=" << A << std::endl;

  for (int idim = 1; idim <= Dimension; idim++)
  {
    //  std::cout << "idim=" << idim << std::endl;

    math_Vector B(0, FirstOrder + LastOrder + 1);
    int         icol;
    for (icol = 0; icol <= FirstOrder; icol++)
      B(icol) = FirstConstr(idim, icol);

    for (icol = 0; icol <= LastOrder; icol++)
      B(FirstOrder + 1 + icol) = LastConstr(idim, icol);
    //  std::cout << "B=" << B << std::endl;

    //  The solving of equations system A * X = B. Then B = X
    Equations.Solve(B);
    //  std::cout << "After Solving" << std::endl << "B=" << B << std::endl;

    if (Equations.IsDone() == false)
      return false;

    //  the filling of the Coefficients

    for (icol = 0; icol <= FirstOrder + LastOrder + 1; icol++)
      Coefficients(Dimension * icol + idim - 1) = B(icol);
  }
  return true;
}

//=================================================================================================

void PLib::JacobiParameters(const GeomAbs_Shape ConstraintOrder,
                            const int           MaxDegree,
                            const int           Code,
                            int&                NbGaussPoints,
                            int&                WorkDegree)
{
  // ConstraintOrder: Ordre de contrainte aux extremites :
  //            C0 = contraintes de passage aux bornes;
  //            C1 = C0 + contraintes de derivees 1eres;
  //            C2 = C1 + contraintes de derivees 2ndes.
  // MaxDegree: Nombre maxi de coeff de la "courbe" polynomiale
  //            d' approximation (doit etre superieur ou egal a
  //            2*NivConstr+2 et inferieur ou egal a 50).
  // Code:      Code d' init. des parametres de discretisation.
  //            (choix de NBPNTS et de NDGJAC de MAPF1C,MAPFXC).
  //            = -5 Calcul tres rapide mais peu precis (8pts)
  //            = -4    '     '    '      '   '    '    (10pts)
  //            = -3    '     '    '      '   '    '    (15pts)
  //            = -2    '     '    '      '   '    '    (20pts)
  //            = -1    '     '    '      '   '    '    (25pts)
  //            = 1 calcul rapide avec precision moyenne (30pts).
  //            = 2 calcul rapide avec meilleure precision (40pts).
  //            = 3 calcul un peu plus lent avec bonne precision (50 pts).
  //            = 4 calcul lent avec la meilleure precision possible
  //             (61pts).

  // The possible values of NbGaussPoints

  const int NDEG8 = 8, NDEG10 = 10, NDEG15 = 15, NDEG20 = 20, NDEG25 = 25, NDEG30 = 30, NDEG40 = 40,
            NDEG50 = 50, NDEG61 = 61;

  int NivConstr = 0;
  switch (ConstraintOrder)
  {
    case GeomAbs_C0:
      NivConstr = 0;
      break;
    case GeomAbs_C1:
      NivConstr = 1;
      break;
    case GeomAbs_C2:
      NivConstr = 2;
      break;
    default:
      throw Standard_ConstructionError("Invalid ConstraintOrder");
  }
  if (MaxDegree < 2 * NivConstr + 1)
    throw Standard_ConstructionError("Invalid MaxDegree");

  if (Code >= 1)
    WorkDegree = MaxDegree + 9;
  else
    WorkDegree = MaxDegree + 6;

  //---> Nbre mini de points necessaires.
  int IPMIN = 0;
  if (WorkDegree < NDEG8)
    IPMIN = NDEG8;
  else if (WorkDegree < NDEG10)
    IPMIN = NDEG10;
  else if (WorkDegree < NDEG15)
    IPMIN = NDEG15;
  else if (WorkDegree < NDEG20)
    IPMIN = NDEG20;
  else if (WorkDegree < NDEG25)
    IPMIN = NDEG25;
  else if (WorkDegree < NDEG30)
    IPMIN = NDEG30;
  else if (WorkDegree < NDEG40)
    IPMIN = NDEG40;
  else if (WorkDegree < NDEG50)
    IPMIN = NDEG50;
  else if (WorkDegree < NDEG61)
    IPMIN = NDEG61;
  else
    throw Standard_ConstructionError("Invalid MaxDegree");
  // ---> Nbre de points voulus.
  int IWANT = 0;
  switch (Code)
  {
    case -5:
      IWANT = NDEG8;
      break;
    case -4:
      IWANT = NDEG10;
      break;
    case -3:
      IWANT = NDEG15;
      break;
    case -2:
      IWANT = NDEG20;
      break;
    case -1:
      IWANT = NDEG25;
      break;
    case 1:
      IWANT = NDEG30;
      break;
    case 2:
      IWANT = NDEG40;
      break;
    case 3:
      IWANT = NDEG50;
      break;
    case 4:
      IWANT = NDEG61;
      break;
    default:
      throw Standard_ConstructionError("Invalid Code");
  }
  //-->  NbGaussPoints est le nombre de points de discretisation de la fonction,
  //     il ne peut prendre que les valeurs 8,10,15,20,25,30,40,50 ou 61.
  //     NbGaussPoints doit etre superieur strictement a WorkDegree.
  NbGaussPoints = std::max(IPMIN, IWANT);
  //  NbGaussPoints +=2;
}

//=======================================================================
// function : NivConstr
// purpose  : translates from GeomAbs_Shape to Integer
//=======================================================================

int PLib::NivConstr(const GeomAbs_Shape ConstraintOrder)
{
  int NivConstr = 0;
  switch (ConstraintOrder)
  {
    case GeomAbs_C0:
      NivConstr = 0;
      break;
    case GeomAbs_C1:
      NivConstr = 1;
      break;
    case GeomAbs_C2:
      NivConstr = 2;
      break;
    default:
      throw Standard_ConstructionError("Invalid ConstraintOrder");
  }
  return NivConstr;
}

//=======================================================================
// function : ConstraintOrder
// purpose  : translates from Integer to GeomAbs_Shape
//=======================================================================

GeomAbs_Shape PLib::ConstraintOrder(const int NivConstr)
{
  GeomAbs_Shape ConstraintOrder = GeomAbs_C0;
  switch (NivConstr)
  {
    case 0:
      ConstraintOrder = GeomAbs_C0;
      break;
    case 1:
      ConstraintOrder = GeomAbs_C1;
      break;
    case 2:
      ConstraintOrder = GeomAbs_C2;
      break;
    default:
      throw Standard_ConstructionError("Invalid NivConstr");
  }
  return ConstraintOrder;
}

//=================================================================================================

void PLib::EvalLength(const int    Degree,
                      const int    Dimension,
                      double&      PolynomialCoeff,
                      const double U1,
                      const double U2,
                      double&      Length)
{
  int    i, j, idim, degdim;
  double C1, C2, Sum, Tran, X1, X2, Der1, Der2, D1, D2, DD;

  double* PolynomialArray = &PolynomialCoeff;

  int NbGaussPoints = 4 * std::min((Degree / 4) + 1, 10);

  math_Vector GaussPoints(1, NbGaussPoints);
  math::GaussPoints(NbGaussPoints, GaussPoints);

  math_Vector GaussWeights(1, NbGaussPoints);
  math::GaussWeights(NbGaussPoints, GaussWeights);

  C1 = (U2 + U1) / 2.;
  C2 = (U2 - U1) / 2.;

  //-----------------------------------------------------------
  //****** Integration - Boucle sur les intervalles de GAUSS **
  //-----------------------------------------------------------

  Sum = 0;

  for (j = 1; j <= NbGaussPoints / 2; j++)
  {
    // Integration en tenant compte de la symetrie
    Tran = C2 * GaussPoints(j);
    X1   = C1 + Tran;
    X2   = C1 - Tran;

    //****** Derivation sur la dimension de l'espace **

    degdim = Degree * Dimension;
    Der1 = Der2 = 0.;
    for (idim = 0; idim < Dimension; idim++)
    {
      D1 = D2 = Degree * PolynomialArray[idim + degdim];
      for (i = Degree - 1; i >= 1; i--)
      {
        DD = i * PolynomialArray[idim + i * Dimension];
        D1 = D1 * X1 + DD;
        D2 = D2 * X2 + DD;
      }
      Der1 += D1 * D1;
      Der2 += D2 * D2;
    }

    //****** Integration **

    Sum += GaussWeights(j) * C2 * (std::sqrt(Der1) + std::sqrt(Der2));

    //****** Fin de boucle dur les intervalles de GAUSS **
  }
  Length = Sum;
}

//=================================================================================================

void PLib::EvalLength(const int    Degree,
                      const int    Dimension,
                      double&      PolynomialCoeff,
                      const double U1,
                      const double U2,
                      const double Tol,
                      double&      Length,
                      double&      Error)
{
  int i;
  int NbSubInt = 1,  // Current number of subintervals
    MaxNbIter  = 13, // Max number of iterations
    NbIter     = 1;  // Current number of iterations
  double dU, OldLen, LenI;

  PLib::EvalLength(Degree, Dimension, PolynomialCoeff, U1, U2, Length);

  do
  {
    OldLen = Length;
    Length = 0.;
    NbSubInt *= 2;
    dU = (U2 - U1) / NbSubInt;
    for (i = 1; i <= NbSubInt; i++)
    {
      PLib::EvalLength(Degree, Dimension, PolynomialCoeff, U1 + (i - 1) * dU, U1 + i * dU, LenI);
      Length += LenI;
    }
    NbIter++;
    Error = std::abs(OldLen - Length);
  } while (Error > Tol && NbIter <= MaxNbIter);
}
