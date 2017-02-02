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

// Modified: 28/02/1996 by PMN :  HermiteCoefficients added
// Modified: 18/06/1996 by PMN :  NULL reference.
// Modified: 19/02/1997 by JCT :  EvalPoly2Var added

#include <GeomAbs_Shape.hxx>
#include <math.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <NCollection_LocalArray.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>

// To convert points array into Real ..
// *********************************
//=======================================================================
//function : SetPoles
//purpose  : 
//=======================================================================
void PLib::SetPoles(const TColgp_Array1OfPnt2d& Poles,
		    TColStd_Array1OfReal& FP)
{
  Standard_Integer j      = FP   .Lower();
  Standard_Integer PLower = Poles.Lower();
  Standard_Integer PUpper = Poles.Upper();
    
  for (Standard_Integer i = PLower; i <= PUpper; i++) {
    const gp_Pnt2d& P = Poles(i);
    FP(j) = P.Coord(1); j++;
    FP(j) = P.Coord(2); j++;
  }
}

//=======================================================================
//function : SetPoles
//purpose  : 
//=======================================================================

void PLib::SetPoles(const TColgp_Array1OfPnt2d&       Poles,
		    const TColStd_Array1OfReal& Weights,
		    TColStd_Array1OfReal&       FP)
{
  Standard_Integer j      = FP   .Lower();
  Standard_Integer PLower = Poles.Lower();
  Standard_Integer PUpper = Poles.Upper();
    
  for (Standard_Integer i = PLower; i <= PUpper; i++) {
    Standard_Real w = Weights(i);
    const gp_Pnt2d& P = Poles(i);
    FP(j) = P.Coord(1) * w; j++;
    FP(j) = P.Coord(2) * w; j++;
    FP(j) =              w; j++;
  }
}

//=======================================================================
//function : GetPoles
//purpose  : 
//=======================================================================

void PLib::GetPoles(const TColStd_Array1OfReal& FP,
		    TColgp_Array1OfPnt2d& Poles)
{
  Standard_Integer j      = FP   .Lower();
  Standard_Integer PLower = Poles.Lower();
  Standard_Integer PUpper = Poles.Upper();
    
  for (Standard_Integer i = PLower; i <= PUpper; i++) {
    gp_Pnt2d& P = Poles(i);
    P.SetCoord(1,FP(j)); j++;
    P.SetCoord(2,FP(j)); j++;
  }
}

//=======================================================================
//function : GetPoles
//purpose  : 
//=======================================================================

void PLib::GetPoles(const TColStd_Array1OfReal& FP,
		    TColgp_Array1OfPnt2d&       Poles,
		    TColStd_Array1OfReal&       Weights)
{
  Standard_Integer j      = FP   .Lower();
  Standard_Integer PLower = Poles.Lower();
  Standard_Integer PUpper = Poles.Upper();
    
  for (Standard_Integer i = PLower; i <= PUpper; i++) {
    Standard_Real w = FP(j + 2);
    Weights(i) = w;
    gp_Pnt2d& P = Poles(i);
    P.SetCoord(1,FP(j) / w); j++;
    P.SetCoord(2,FP(j) / w); j++;
    j++;
  }
}

//=======================================================================
//function : SetPoles
//purpose  : 
//=======================================================================

void PLib::SetPoles(const TColgp_Array1OfPnt& Poles,
		    TColStd_Array1OfReal& FP)
{
  Standard_Integer j      = FP   .Lower();
  Standard_Integer PLower = Poles.Lower();
  Standard_Integer PUpper = Poles.Upper();

  for (Standard_Integer i = PLower; i <= PUpper; i++) {
    const gp_Pnt& P = Poles(i);
    FP(j) = P.Coord(1); j++;
    FP(j) = P.Coord(2); j++;
    FP(j) = P.Coord(3); j++;
  }
}

//=======================================================================
//function : SetPoles
//purpose  : 
//=======================================================================

void PLib::SetPoles(const TColgp_Array1OfPnt&   Poles,
		    const TColStd_Array1OfReal& Weights,
		    TColStd_Array1OfReal&       FP)
{
  Standard_Integer j      = FP   .Lower();
  Standard_Integer PLower = Poles.Lower();
  Standard_Integer PUpper = Poles.Upper();
    
  for (Standard_Integer i = PLower; i <= PUpper; i++) {
    Standard_Real w = Weights(i);
    const gp_Pnt& P = Poles(i);
    FP(j) = P.Coord(1) * w; j++;
    FP(j) = P.Coord(2) * w; j++;
    FP(j) = P.Coord(3) * w; j++;
    FP(j) =              w; j++;
  }
}

//=======================================================================
//function : GetPoles
//purpose  : 
//=======================================================================

void PLib::GetPoles(const TColStd_Array1OfReal& FP,
		    TColgp_Array1OfPnt&         Poles)
{
  Standard_Integer j      = FP   .Lower();
  Standard_Integer PLower = Poles.Lower();
  Standard_Integer PUpper = Poles.Upper();
    
  for (Standard_Integer i = PLower; i <= PUpper; i++) {
    gp_Pnt& P = Poles(i);
    P.SetCoord(1,FP(j)); j++;
    P.SetCoord(2,FP(j)); j++;
    P.SetCoord(3,FP(j)); j++;
  }
}

//=======================================================================
//function : GetPoles
//purpose  : 
//=======================================================================

void PLib::GetPoles(const TColStd_Array1OfReal& FP,
		    TColgp_Array1OfPnt&         Poles,
		    TColStd_Array1OfReal&       Weights)
{
  Standard_Integer j      = FP   .Lower();
  Standard_Integer PLower = Poles.Lower();
  Standard_Integer PUpper = Poles.Upper();
    
  for (Standard_Integer i = PLower; i <= PUpper; i++) {
    Standard_Real w = FP(j + 3);
    Weights(i) = w;
    gp_Pnt& P = Poles(i);
    P.SetCoord(1,FP(j) / w); j++;
    P.SetCoord(2,FP(j) / w); j++;
    P.SetCoord(3,FP(j) / w); j++;
    j++;
  }
}

// specialized allocator
namespace
{

class BinomAllocator
{
public:

  //! Main constructor
  BinomAllocator (const Standard_Integer theMaxBinom)
  : myBinom (NULL),
    myMaxBinom (theMaxBinom)
  {
    Standard_Integer i, im1, ip1, id2, md2, md3, j, k;
    Standard_Integer np1 = myMaxBinom + 1;
    myBinom = new Standard_Integer*[np1];
    myBinom[0] = new Standard_Integer[1];
    myBinom[0][0] = 1;
    for (i = 1; i < np1; ++i)
    {
      im1 = i - 1;
      ip1 = i + 1;
      id2 = i >> 1;
      md2 = im1 >> 1;
      md3 = ip1 >> 1;
      k   = 0;
      myBinom[i] = new Standard_Integer[ip1];

      for (j = 0; j < id2; ++j)
      {
        myBinom[i][j] = k + myBinom[im1][j];
        k = myBinom[im1][j];
      }
      j = id2;
      if (j > md2) j = im1 - j;
      myBinom[i][id2] = k + myBinom[im1][j];

      for (j = ip1 - md3; j < ip1; j++)
      {
        myBinom[i][j] = myBinom[i][i - j];
      }
    }
  }

  //! Destructor
  ~BinomAllocator()
  {
    // free memory
    for (Standard_Integer i = 0; i <= myMaxBinom; ++i)
    {
      delete[] myBinom[i];
    }
    delete[] myBinom;
  }

  Standard_Real Value (const Standard_Integer N,
                       const Standard_Integer P) const
  {
    Standard_OutOfRange_Raise_if (N > myMaxBinom,
      "PLib, BinomAllocator: requested degree is greater than maximum supported");
    return Standard_Real (myBinom[N][P]);
  }

private:
  BinomAllocator (const BinomAllocator&);
  BinomAllocator& operator= (const BinomAllocator&);

private:
  Standard_Integer**  myBinom;
  Standard_Integer myMaxBinom;

};

  // we do not call BSplCLib here to avoid Cyclic dependency detection by WOK
  //static BinomAllocator THE_BINOM (BSplCLib::MaxDegree() + 1);
  static BinomAllocator THE_BINOM (25 + 1);
}

//=======================================================================
//function : Bin
//purpose  : 
//=======================================================================

Standard_Real PLib::Bin(const Standard_Integer N,
                        const Standard_Integer P)
{
  return THE_BINOM.Value (N, P);
}

//=======================================================================
//function : RationalDerivative
//purpose  : 
//=======================================================================

void  PLib::RationalDerivative(const Standard_Integer Degree,
			       const Standard_Integer DerivativeRequest,
			       const Standard_Integer Dimension,
			       Standard_Real& Ders,
			       Standard_Real& RDers,
			       const Standard_Boolean All)
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
  Standard_Real Inverse;
  Standard_Real *PolesArray = &Ders;
  Standard_Real *RationalArray = &RDers;
  Standard_Real Factor ;
  Standard_Integer ii, Index, OtherIndex, Index1, Index2, jj;
  NCollection_LocalArray<Standard_Real> binomial_array;
  NCollection_LocalArray<Standard_Real> derivative_storage;
  if (Dimension == 3) {
    Standard_Integer DeRequest1 = DerivativeRequest + 1;
    Standard_Integer MinDegRequ = DerivativeRequest;
    if (MinDegRequ > Degree) MinDegRequ = Degree;
    binomial_array.Allocate (DeRequest1);
    
    for (ii = 0 ; ii < DeRequest1 ; ii++) {
      binomial_array[ii] = 1.0e0 ;
    }
    if (!All) {
      Standard_Integer DimDeRequ1 = (DeRequest1 << 1) + DeRequest1;
      derivative_storage.Allocate (DimDeRequ1);
      RationalArray = derivative_storage ;
    }
    
    Inverse = 1.0e0 / PolesArray[3]  ;
    Index = 0 ;
    Index2 = - 6;
    OtherIndex = 0 ;
    
    for (ii = 0 ; ii <= MinDegRequ ; ii++) {
      Index2 += 3;
      Index1  = Index2;
      RationalArray[Index] = PolesArray[OtherIndex]; Index++; OtherIndex++;
      RationalArray[Index] = PolesArray[OtherIndex]; Index++; OtherIndex++;
      RationalArray[Index] = PolesArray[OtherIndex];
      Index      -= 2;
      OtherIndex += 2;
      
      for (jj = ii - 1 ; jj >= 0 ; jj--) {
	Factor = binomial_array[jj] * PolesArray[((ii-jj) << 2) + 3]; 
	RationalArray[Index] -=  Factor * RationalArray[Index1]; Index++; Index1++;
	RationalArray[Index] -=  Factor * RationalArray[Index1]; Index++; Index1++;
	RationalArray[Index] -=  Factor * RationalArray[Index1];
	Index  -= 2;
	Index1 -= 5;
      }
      
      for (jj = ii ; jj >=  1 ; jj--) {
	binomial_array[jj] += binomial_array[jj - 1] ;
      }
      RationalArray[Index] *= Inverse ; Index++;
      RationalArray[Index] *= Inverse ; Index++;
      RationalArray[Index] *= Inverse ; Index++;
    } 
    
    for (ii= MinDegRequ + 1; ii <= DerivativeRequest ; ii++){
      Index2 += 3;
      Index1 = Index2;
      RationalArray[Index] = 0.0e0; Index++;
      RationalArray[Index] = 0.0e0; Index++;
      RationalArray[Index] = 0.0e0;
      Index -= 2;
      
      for (jj = ii - 1 ; jj >= ii - MinDegRequ ; jj--) {
	Factor = binomial_array[jj] * PolesArray[((ii-jj) << 2) + 3]; 
	RationalArray[Index] -=  Factor * RationalArray[Index1]; Index++; Index1++;
	RationalArray[Index] -=  Factor * RationalArray[Index1]; Index++; Index1++;
	RationalArray[Index] -=  Factor * RationalArray[Index1];
	Index  -= 2;
	Index1 -= 5;
      }
      
      for (jj = ii ; jj >=  1 ; jj--) {
	binomial_array[jj] += binomial_array[jj - 1] ;
      }
      RationalArray[Index] *= Inverse; Index++;
      RationalArray[Index] *= Inverse; Index++;
      RationalArray[Index] *= Inverse; Index++;
    }
    
    if (!All) {
      RationalArray = &RDers ;
      Standard_Integer DimDeRequ = (DerivativeRequest << 1) + DerivativeRequest;
      RationalArray[0] = derivative_storage[DimDeRequ]; DimDeRequ++;
      RationalArray[1] = derivative_storage[DimDeRequ]; DimDeRequ++;
      RationalArray[2] = derivative_storage[DimDeRequ];
    }
  }
  else {
    Standard_Integer kk;
    Standard_Integer Dimension1 = Dimension + 1;
    Standard_Integer Dimension2 = Dimension << 1;
    Standard_Integer DeRequest1 = DerivativeRequest + 1;
    Standard_Integer MinDegRequ = DerivativeRequest;
    if (MinDegRequ > Degree) MinDegRequ = Degree;
    binomial_array.Allocate (DeRequest1);
    
    for (ii = 0 ; ii < DeRequest1 ; ii++) {
      binomial_array[ii] = 1.0e0 ;
    }
    if (!All) {
      Standard_Integer DimDeRequ1 = Dimension * DeRequest1;
      derivative_storage.Allocate (DimDeRequ1);
      RationalArray = derivative_storage ;
    }
    
    Inverse = 1.0e0 / PolesArray[Dimension]  ;
    Index = 0 ;
    Index2 = - Dimension2;
    OtherIndex = 0 ;
    
    for (ii = 0 ; ii <= MinDegRequ ; ii++) {
      Index2 += Dimension;
      Index1  = Index2;
      
      for (kk = 0 ; kk < Dimension ; kk++) {
	RationalArray[Index] = PolesArray[OtherIndex]; Index++; OtherIndex++;
      }
      Index -= Dimension;
      OtherIndex ++;;
      
      for (jj = ii - 1 ; jj >= 0 ; jj--) {
	Factor = binomial_array[jj] * PolesArray[(ii-jj) * Dimension1 + Dimension]; 
	
	for (kk = 0 ; kk < Dimension ; kk++) {
	  RationalArray[Index] -=  Factor * RationalArray[Index1]; Index++; Index1++;
	}
	Index  -= Dimension ;
	Index1 -= Dimension2 ;
      }
      
      for (jj = ii ; jj >=  1 ; jj--) {
	binomial_array[jj] += binomial_array[jj - 1] ;
      }
      
      for (kk = 0 ; kk < Dimension ; kk++) {
	RationalArray[Index] *= Inverse ; Index++;
      }
    } 
    
    for (ii= MinDegRequ + 1; ii <= DerivativeRequest ; ii++){
      Index2 += Dimension;
      Index1  = Index2;
      
      for (kk = 0 ; kk < Dimension ; kk++) {
	RationalArray[Index] = 0.0e0 ; Index++;
      }
      Index -= Dimension;
      
      for (jj = ii - 1 ; jj >= ii - MinDegRequ ; jj--) {
	Factor = binomial_array[jj] * PolesArray[(ii-jj) * Dimension1 + Dimension]; 
	
	for (kk = 0 ; kk < Dimension ; kk++) {
	  RationalArray[Index] -=  Factor * RationalArray[Index1]; Index++; Index1++;
	}
	Index  -= Dimension ;
	Index1 -= Dimension2 ;
      }
      
      for (jj = ii ; jj >=  1 ; jj--) {
	binomial_array[jj] += binomial_array[jj - 1] ;
      }
      
      for (kk = 0 ; kk < Dimension ; kk++) {
	RationalArray[Index] *= Inverse; Index++;
      }
    }
    
    if (!All) {
      RationalArray = &RDers ;
      Standard_Integer DimDeRequ = Dimension * DerivativeRequest;
      
      for (kk = 0 ; kk < Dimension ; kk++) {
	RationalArray[kk] = derivative_storage[DimDeRequ]; DimDeRequ++;
      }
    }
  }
} 

//=======================================================================
//function : RationalDerivatives
//purpose  : Uses Homogeneous Poles Derivatives and Deivatives of Weights
//=======================================================================

void  PLib::RationalDerivatives(const Standard_Integer DerivativeRequest,
				const Standard_Integer  Dimension,
				Standard_Real& PolesDerivates, 
				// must be an array with 
				// (DerivativeRequest + 1) * Dimension slots
				Standard_Real& WeightsDerivates,
				// must be an array with 
				// (DerivativeRequest + 1) slots
				Standard_Real& RationalDerivates) 
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
  Standard_Real Inverse;
  Standard_Real *PolesArray = &PolesDerivates;
  Standard_Real *WeightsArray = &WeightsDerivates;
  Standard_Real *RationalArray = &RationalDerivates;
  Standard_Real Factor ;
  
  Standard_Integer ii, Index, Index1, Index2, jj;
  Standard_Integer DeRequest1 = DerivativeRequest + 1;
  
  NCollection_LocalArray<Standard_Real> binomial_array (DeRequest1);
  NCollection_LocalArray<Standard_Real> derivative_storage;

  for (ii = 0 ; ii < DeRequest1 ; ii++) {
    binomial_array[ii] = 1.0e0 ;
  }
  Inverse = 1.0e0 / WeightsArray[0]  ;
  if (Dimension == 3) {
    Index  = 0 ;
    Index2 = - 6 ;

    for (ii = 0 ; ii < DeRequest1 ; ii++) {
      Index2 += 3;
      Index1 = Index2;
      RationalArray[Index] = PolesArray[Index] ; Index++;
      RationalArray[Index] = PolesArray[Index] ; Index++;
      RationalArray[Index] = PolesArray[Index] ;
      Index -= 2;
      
      for (jj = ii - 1 ; jj >= 0 ; jj--) {
	Factor = binomial_array[jj] * WeightsArray[ii - jj] ; 
	RationalArray[Index] -=  Factor * RationalArray[Index1]; Index++; Index1++;
	RationalArray[Index] -=  Factor * RationalArray[Index1]; Index++; Index1++;
	RationalArray[Index] -=  Factor * RationalArray[Index1];
	Index  -= 2;
	Index1 -= 5;
      }
      
      for (jj = ii ; jj >=  1 ; jj--) {
	binomial_array[jj] += binomial_array[jj - 1] ;
      }
      RationalArray[Index] *= Inverse ; Index++;
      RationalArray[Index] *= Inverse ; Index++;
      RationalArray[Index] *= Inverse ; Index++;
    }
  }
  else {
    Standard_Integer kk;
    Standard_Integer Dimension2 = Dimension << 1;
    Index  = 0 ;
    Index2 = - Dimension2;

    for (ii = 0 ; ii < DeRequest1 ; ii++) {
      Index2 += Dimension;
      Index1  = Index2;
      
      for (kk = 0 ; kk < Dimension ; kk++) {
	RationalArray[Index] = PolesArray[Index]; Index++;
      }
      Index  -= Dimension;
      
      for (jj = ii - 1 ; jj >= 0 ; jj--) {
	Factor = binomial_array[jj] * WeightsArray[ii - jj] ; 
	
	for (kk = 0 ; kk < Dimension ; kk++) {
	  RationalArray[Index] -=  Factor * RationalArray[Index1]; Index++; Index1++;
	}
	Index  -= Dimension;
	Index1 -= Dimension2;
      }
      
      for (jj = ii ; jj >=  1 ; jj--) {
	binomial_array[jj] += binomial_array[jj - 1] ;
      }
      
      for (kk = 0 ; kk < Dimension ; kk++) {
	RationalArray[Index] *= Inverse ; Index++;
      }
    }
  }
}

//=======================================================================
// Auxiliary template functions used for optimized evaluation of polynome
// and its derivatives for smaller dimensions of the polynome
//=======================================================================

namespace {
  // recursive template for evaluating value or first derivative
  template<int dim> 
  inline void eval_step1 (double* poly, double par, double* coef)
  {
    eval_step1<dim - 1> (poly, par, coef);
    poly[dim] = poly[dim] * par + coef[dim];
  }

  // recursion end
  template<>
  inline void eval_step1<-1> (double*, double, double*)
  {
  }

  // recursive template for evaluating second derivative
  template<int dim> 
  inline void eval_step2 (double* poly, double par, double* coef)
  {
    eval_step2<dim - 1> (poly, par, coef);
    poly[dim] = poly[dim] * par + coef[dim] * 2.;
  }

  // recursion end
  template<>
  inline void eval_step2<-1> (double*, double, double*)
  {
  }

  // evaluation of only value
  template<int dim>
  inline void eval_poly0 (double* aRes, double* aCoeffs, int Degree, double Par)
  {
    Standard_Real* aRes0 = aRes;
    memcpy(aRes0, aCoeffs, sizeof(Standard_Real) * dim);

    for (Standard_Integer aDeg = 0; aDeg < Degree; aDeg++)
    {
      aCoeffs -= dim;
      // Calculating the value of the polynomial
      eval_step1<dim-1> (aRes0, Par, aCoeffs);
    }
  }

  // evaluation of value and first derivative
  template<int dim>
  inline void eval_poly1 (double* aRes, double* aCoeffs, int Degree, double Par)
  {
    Standard_Real* aRes0 = aRes;
    Standard_Real* aRes1 = aRes + dim;

    memcpy(aRes0, aCoeffs, sizeof(Standard_Real) * dim);
    memset(aRes1, 0, sizeof(Standard_Real) * dim);

    for (Standard_Integer aDeg = 0; aDeg < Degree; aDeg++)
    {
      aCoeffs -= dim;
      // Calculating derivatives of the polynomial
      eval_step1<dim-1> (aRes1, Par, aRes0);
      // Calculating the value of the polynomial
      eval_step1<dim-1> (aRes0, Par, aCoeffs);
    }
  }

  // evaluation of value and first and second derivatives
  template<int dim>
  inline void eval_poly2 (double* aRes, double* aCoeffs, int Degree, double Par)
  {
    Standard_Real* aRes0 = aRes;
    Standard_Real* aRes1 = aRes + dim;
    Standard_Real* aRes2 = aRes + 2 * dim;

    memcpy(aRes0, aCoeffs, sizeof(Standard_Real) * dim);
    memset(aRes1, 0, sizeof(Standard_Real) * dim);
    memset(aRes2, 0, sizeof(Standard_Real) * dim);

    for (Standard_Integer aDeg = 0; aDeg < Degree; aDeg++)
    {
      aCoeffs -= dim;
      // Calculating second derivatives of the polynomial
      eval_step2<dim-1> (aRes2, Par, aRes1);
      // Calculating derivatives of the polynomial
      eval_step1<dim-1> (aRes1, Par, aRes0);
      // Calculating the value of the polynomial
      eval_step1<dim-1> (aRes0, Par, aCoeffs);
    }
  }
}

//=======================================================================
//function : This evaluates a polynomial and its derivatives 
//purpose  : up to the requested order 
//=======================================================================

void  PLib::EvalPolynomial(const Standard_Real    Par,
                           const Standard_Integer DerivativeRequest,
                           const Standard_Integer Degree,
                           const Standard_Integer Dimension,
                                 Standard_Real&   PolynomialCoeff,
                                 Standard_Real&   Results)
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
  Standard_Real* aCoeffs = &PolynomialCoeff + Degree * Dimension;
  Standard_Real* aRes = &Results;
  Standard_Real* anOriginal;
  Standard_Integer ind = 0;
  switch (DerivativeRequest)
  {
  case 1:
  {
    switch (Dimension)
    {
    case 1:  eval_poly1<1>  (aRes, aCoeffs, Degree, Par); break;
    case 2:  eval_poly1<2>  (aRes, aCoeffs, Degree, Par); break;
    case 3:  eval_poly1<3>  (aRes, aCoeffs, Degree, Par); break;
    case 4:  eval_poly1<4>  (aRes, aCoeffs, Degree, Par); break;
    case 5:  eval_poly1<5>  (aRes, aCoeffs, Degree, Par); break;
    case 6:  eval_poly1<6>  (aRes, aCoeffs, Degree, Par); break;
    case 7:  eval_poly1<7>  (aRes, aCoeffs, Degree, Par); break;
    case 8:  eval_poly1<8>  (aRes, aCoeffs, Degree, Par); break;
    case 9:  eval_poly1<9>  (aRes, aCoeffs, Degree, Par); break;
    case 10: eval_poly1<10> (aRes, aCoeffs, Degree, Par); break;
    case 11: eval_poly1<11> (aRes, aCoeffs, Degree, Par); break;
    case 12: eval_poly1<12> (aRes, aCoeffs, Degree, Par); break;
    case 13: eval_poly1<13> (aRes, aCoeffs, Degree, Par); break;
    case 14: eval_poly1<14> (aRes, aCoeffs, Degree, Par); break;
    case 15: eval_poly1<15> (aRes, aCoeffs, Degree, Par); break;
    default:
      {
        Standard_Real* aRes0 = aRes;
        Standard_Real* aRes1 = aRes + Dimension;

        memcpy(aRes0, aCoeffs, sizeof(Standard_Real) * Dimension);
        memset(aRes1, 0, sizeof(Standard_Real) * Dimension);

        for (Standard_Integer aDeg = 0; aDeg < Degree; aDeg++)
        {
          aCoeffs -= Dimension;
          // Calculating derivatives of the polynomial
          for (ind = 0; ind < Dimension; ind++)
            aRes1[ind] = aRes1[ind] * Par + aRes0[ind];
          // Calculating the value of the polynomial
          for (ind = 0; ind < Dimension; ind++)
            aRes0[ind] = aRes0[ind] * Par + aCoeffs[ind];
        }
      }
    }
    break;
  }
  case 2:
  {
    switch (Dimension)
    {
    case 1:  eval_poly2<1>  (aRes, aCoeffs, Degree, Par); break;
    case 2:  eval_poly2<2>  (aRes, aCoeffs, Degree, Par); break;
    case 3:  eval_poly2<3>  (aRes, aCoeffs, Degree, Par); break;
    case 4:  eval_poly2<4>  (aRes, aCoeffs, Degree, Par); break;
    case 5:  eval_poly2<5>  (aRes, aCoeffs, Degree, Par); break;
    case 6:  eval_poly2<6>  (aRes, aCoeffs, Degree, Par); break;
    case 7:  eval_poly2<7>  (aRes, aCoeffs, Degree, Par); break;
    case 8:  eval_poly2<8>  (aRes, aCoeffs, Degree, Par); break;
    case 9:  eval_poly2<9>  (aRes, aCoeffs, Degree, Par); break;
    case 10: eval_poly2<10> (aRes, aCoeffs, Degree, Par); break;
    case 11: eval_poly2<11> (aRes, aCoeffs, Degree, Par); break;
    case 12: eval_poly2<12> (aRes, aCoeffs, Degree, Par); break;
    case 13: eval_poly2<13> (aRes, aCoeffs, Degree, Par); break;
    case 14: eval_poly2<14> (aRes, aCoeffs, Degree, Par); break;
    case 15: eval_poly2<15> (aRes, aCoeffs, Degree, Par); break;
    default: 
      {
        Standard_Real* aRes0 = aRes;
        Standard_Real* aRes1 = aRes + Dimension;
        Standard_Real* aRes2 = aRes1 + Dimension;

        // Nullify the results
        Standard_Integer aSize = 2 * Dimension;
        memcpy(aRes, aCoeffs, sizeof(Standard_Real) * Dimension);
        memset(aRes1, 0, sizeof(Standard_Real) * aSize);

        for (Standard_Integer aDeg = 0; aDeg < Degree; aDeg++)
        {
          aCoeffs -= Dimension;
          // Calculating derivatives of the polynomial
          for (ind = 0; ind < Dimension; ind++)
            aRes2[ind] = aRes2[ind] * Par + aRes1[ind] * 2.0;
          for (ind = 0; ind < Dimension; ind++)
            aRes1[ind] = aRes1[ind] * Par + aRes0[ind];
          // Calculating the value of the polynomial
          for (ind = 0; ind < Dimension; ind++)
            aRes0[ind] = aRes0[ind] * Par + aCoeffs[ind];
        }
        break;
      }
    }
    break;
  }
  default:
    {
      // Nullify the results
      Standard_Integer aResSize = (1 + DerivativeRequest) * Dimension;
      memset(aRes, 0, sizeof(Standard_Real) * aResSize);

      for (Standard_Integer aDeg = 0; aDeg <= Degree; aDeg++)
      {
        aRes = &Results + aResSize - Dimension;
        // Calculating derivatives of the polynomial
        for (Standard_Integer aDeriv = DerivativeRequest; aDeriv > 0; aDeriv--)
        {
          anOriginal = aRes - Dimension; // pointer to the derivative minus 1
          for (ind = 0; ind < Dimension; ind++)
            aRes[ind] = aRes[ind] * Par + anOriginal[ind] * aDeriv;
          aRes = anOriginal;
        }
        // Calculating the value of the polynomial
        for (ind = 0; ind < Dimension; ind++)
          aRes[ind] = aRes[ind] * Par + aCoeffs[ind];
        aCoeffs -= Dimension;
      }
    }
  }
}

//=======================================================================
//function : This evaluates a polynomial without derivative 
//purpose  :
//=======================================================================

void  PLib::NoDerivativeEvalPolynomial(const Standard_Real    Par,
                                       const Standard_Integer Degree,
                                       const Standard_Integer Dimension,
                                       const Standard_Integer DegreeDimension,
                                       Standard_Real&         PolynomialCoeff,
                                       Standard_Real&         Results)
{
  Standard_Real* aCoeffs = &PolynomialCoeff + DegreeDimension;
  Standard_Real* aRes = &Results;

  switch (Dimension)
  {
  case 1:  eval_poly0<1>  (aRes, aCoeffs, Degree, Par); break;
  case 2:  eval_poly0<2>  (aRes, aCoeffs, Degree, Par); break;
  case 3:  eval_poly0<3>  (aRes, aCoeffs, Degree, Par); break;
  case 4:  eval_poly0<4>  (aRes, aCoeffs, Degree, Par); break;
  case 5:  eval_poly0<5>  (aRes, aCoeffs, Degree, Par); break;
  case 6:  eval_poly0<6>  (aRes, aCoeffs, Degree, Par); break;
  case 7:  eval_poly0<7>  (aRes, aCoeffs, Degree, Par); break;
  case 8:  eval_poly0<8>  (aRes, aCoeffs, Degree, Par); break;
  case 9:  eval_poly0<9>  (aRes, aCoeffs, Degree, Par); break;
  case 10: eval_poly0<10> (aRes, aCoeffs, Degree, Par); break;
  case 11: eval_poly0<11> (aRes, aCoeffs, Degree, Par); break;
  case 12: eval_poly0<12> (aRes, aCoeffs, Degree, Par); break;
  case 13: eval_poly0<13> (aRes, aCoeffs, Degree, Par); break;
  case 14: eval_poly0<14> (aRes, aCoeffs, Degree, Par); break;
  case 15: eval_poly0<15> (aRes, aCoeffs, Degree, Par); break;
  default:
    {
      memcpy(aRes, aCoeffs, sizeof(Standard_Real) * Dimension);
      for (Standard_Integer aDeg = 0; aDeg < Degree; aDeg++)
      {
        aCoeffs -= Dimension;
        for (Standard_Integer ind = 0; ind < Dimension; ind++)
          aRes[ind] = aRes[ind] * Par + aCoeffs[ind];
      }
    }
  }
}

//=======================================================================
//function : This evaluates a polynomial of 2 variables 
//purpose  : or its derivative at the requested orders 
//=======================================================================

void  PLib::EvalPoly2Var(const Standard_Real    UParameter,
			 const Standard_Real    VParameter,
			 const Standard_Integer UDerivativeRequest,
			 const Standard_Integer VDerivativeRequest,
			 const Standard_Integer UDegree,
			 const Standard_Integer VDegree,
			 const Standard_Integer Dimension, 
			 Standard_Real&         PolynomialCoeff,
			 Standard_Real&         Results)
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
  Standard_Integer Udim = (VDegree+1)*Dimension,
  index = Udim*UDerivativeRequest;
  TColStd_Array1OfReal Curve(1, Udim*(UDerivativeRequest+1));
  TColStd_Array1OfReal Point(1, Dimension*(VDerivativeRequest+1)); 
  Standard_Real * Result =  (Standard_Real *) &Curve.ChangeValue(1);
  Standard_Real * Digit  =  (Standard_Real *) &Point.ChangeValue(1);
  Standard_Real * ResultArray ;
  ResultArray = &Results ;
  
  PLib::EvalPolynomial(UParameter,
		       UDerivativeRequest,
		       UDegree,
		       Udim,
		       PolynomialCoeff,
		       Result[0]);
  
  PLib::EvalPolynomial(VParameter,
		       VDerivativeRequest,
		       VDegree,  
		       Dimension,
		       Result[index],
		       Digit[0]);

  index = Dimension*VDerivativeRequest;

  for (Standard_Integer i=0;i<Dimension;i++) {
    ResultArray[i] = Digit[index+i];
  }
}



//=======================================================================
//function : This evaluates the lagrange polynomial and its derivatives 
//purpose  : up to the requested order that interpolates a series of
//points of dimension <Dimension> with given assigned parameters
//=======================================================================

Standard_Integer  
PLib::EvalLagrange(const Standard_Real                   Parameter,
		   const Standard_Integer                DerivativeRequest,
		   const Standard_Integer                Degree,
		   const Standard_Integer                Dimension, 
		   Standard_Real&                        Values,
		   Standard_Real&                        Parameters,
		   Standard_Real&                        Results)
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
  Standard_Integer ii, jj, kk, Index, Index1, ReturnCode=0;
  Standard_Integer local_request = DerivativeRequest;
  Standard_Real  *ParameterArray;
  Standard_Real  difference;
  Standard_Real  *PointsArray;
  Standard_Real  *ResultArray ;
  
  PointsArray    = &Values ;
  ParameterArray = &Parameters ;
  ResultArray = &Results ;
  if (local_request >= Degree) {
    local_request = Degree ;
  }
  NCollection_LocalArray<Standard_Real> divided_differences_array ((Degree + 1) * Dimension);
  //
  //  Build the divided differences array
  //
  
  for (ii = 0 ; ii <  (Degree + 1) * Dimension  ; ii++) {
    divided_differences_array[ii] = PointsArray[ii] ;  
  }

  for (ii = Degree ; ii >= 0   ; ii--) {

    for (jj = Degree  ; jj > Degree - ii  ; jj--) {
      Index = jj * Dimension ;
      Index1 = Index - Dimension ;

      for (kk = 0 ; kk < Dimension ; kk++) {
	divided_differences_array[Index + kk] -=
	  divided_differences_array[Index1 + kk] ;
      }
      difference = 
	ParameterArray[jj] - ParameterArray[jj - Degree -1 +  ii] ; 
      if (Abs(difference) < RealSmall()) {
	ReturnCode = 1 ;
	goto FINISH ;
      }
      difference = 1.0e0 / difference ;

      for (kk = 0 ; kk < Dimension ; kk++) {
	divided_differences_array[Index + kk] *= difference ;
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
  Index = Degree * Dimension ;

  for (kk = 0 ; kk < Dimension ; kk++) {
    ResultArray[kk] = divided_differences_array[Index + kk] ;
  }  

  for (ii = Dimension ; ii < (local_request + 1)  * Dimension ; ii++) {
    ResultArray[ii] = 0.0e0 ;
  }

  for (ii = Degree ; ii >= 1 ; ii--) {
    difference =  Parameter - ParameterArray[ii - 1] ;

    for (jj = local_request ; jj > 0 ; jj--) {
      Index = jj * Dimension ;
      Index1 = Index - Dimension ;
      
      for (kk = 0 ; kk < Dimension ; kk++) {
	ResultArray[Index + kk] *= difference ;
	ResultArray[Index + kk] += ResultArray[Index1+kk]*(Standard_Real) jj ;
      }
    }
    Index = (ii -1) * Dimension ;

    for (kk = 0 ; kk < Dimension ; kk++) {
      ResultArray[kk] *= difference ;
      ResultArray[kk] += divided_differences_array[Index+kk] ;
    }
  }
  FINISH : 
    return (ReturnCode) ;
}

//=======================================================================
//function : This evaluates the hermite polynomial and its derivatives 
//purpose  : up to the requested order that interpolates a series of
//points of dimension <Dimension> with given assigned parameters
//=======================================================================

Standard_Integer PLib::EvalCubicHermite
(const Standard_Real          Parameter,
 const Standard_Integer       DerivativeRequest,
 const Standard_Integer       Dimension, 
 Standard_Real&               Values,
 Standard_Real&               Derivatives,
 Standard_Real&               theParameters,
 Standard_Real&               Results)
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
  Standard_Integer ii, jj, kk, pp, Index, Index1, Degree, ReturnCode;
  Standard_Integer local_request = DerivativeRequest ;
  
  ReturnCode = 0 ;
  Degree = 3 ;
  Standard_Real  ParametersArray[4];
  Standard_Real  difference;
  Standard_Real  inverse;
  Standard_Real  *FirstLast;
  Standard_Real  *PointsArray;
  Standard_Real  *DerivativesArray;
  Standard_Real  *ResultArray ;

  DerivativesArray = &Derivatives ;
  PointsArray    = &Values ;
  FirstLast = &theParameters ;
  ResultArray = &Results ;
  if (local_request >= Degree) {
    local_request = Degree ;
  }   
  NCollection_LocalArray<Standard_Real> divided_differences_array ((Degree + 1) * Dimension);

  for (ii = 0, jj = 0  ; ii < 2 ; ii++, jj+= 2) {
    ParametersArray[jj] =
      ParametersArray[jj+1] = FirstLast[ii] ;
  }
  //
  //  Build the divided differences array
  //
  //
  // initialise it at the stage 2 of the building algorithm
  // for devided differences
  //
  inverse = FirstLast[1] - FirstLast[0] ;
  inverse = 1.0e0 / inverse ;

  for (ii = 0, jj = Dimension, kk = 2 * Dimension, pp = 3 * Dimension  ; 
       ii <  Dimension  ; 
       ii++, jj++, kk++, pp++) {
    divided_differences_array[ii] = PointsArray[ii] ;  
    divided_differences_array[kk] = inverse * 
      (PointsArray[jj] - PointsArray[ii]) ; 
    divided_differences_array[jj] = DerivativesArray[ii] ;
    divided_differences_array[pp] = DerivativesArray[jj] ;
  }

  for (ii = 1 ; ii <= Degree   ; ii++) {

    for (jj = Degree  ; jj >=  ii+1  ; jj--) {
      Index = jj * Dimension ;
      Index1 = Index - Dimension ;

      for (kk = 0 ; kk < Dimension ; kk++) {
	divided_differences_array[Index + kk] -=
	  divided_differences_array[Index1 + kk] ;
      }

      for (kk = 0 ; kk < Dimension ; kk++) {
	divided_differences_array[Index + kk] *= inverse ;
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
  Index = Degree * Dimension ;

  for (kk = 0 ; kk < Dimension ; kk++) {
    ResultArray[kk] = divided_differences_array[Index + kk] ;
  }  

  for (ii = Dimension ; ii < (local_request + 1)  * Dimension ; ii++) {
    ResultArray[ii] = 0.0e0 ;
  }

  for (ii = Degree ; ii >= 1 ; ii--) {
    difference =  Parameter - ParametersArray[ii - 1] ;

    for (jj = local_request ; jj > 0 ; jj--) {
      Index = jj * Dimension ;
      Index1 = Index - Dimension ;

      for (kk = 0 ; kk < Dimension ; kk++) {
	ResultArray[Index + kk] *= difference ;
	ResultArray[Index + kk] += ResultArray[Index1+kk]*(Standard_Real) jj;
      }
    }
    Index = (ii -1) * Dimension ;

    for (kk = 0 ; kk < Dimension ; kk++) {
      ResultArray[kk] *= difference ;
      ResultArray[kk] += divided_differences_array[Index+kk] ;
    }
  }
//  FINISH : 
    return (ReturnCode) ;
}

//=======================================================================
//function : HermiteCoefficients
//purpose  : calcul des polynomes d'Hermite
//=======================================================================

Standard_Boolean PLib::HermiteCoefficients(const Standard_Real FirstParameter, 
					   const Standard_Real LastParameter,
					   const Standard_Integer FirstOrder,
					   const Standard_Integer LastOrder, 
					   math_Matrix& MatrixCoefs)
{
  Standard_Integer NbCoeff =  FirstOrder +  LastOrder + 2, Ordre[2];
  Standard_Integer ii, jj, pp, cote, iof=0;
  Standard_Real Prod, TBorne = FirstParameter;
  math_Vector Coeff(1,NbCoeff), B(1, NbCoeff, 0.0);
  math_Matrix MAT(1,NbCoeff, 1,NbCoeff, 0.0);

  // Test de validites

  if ((FirstOrder < 0) || (LastOrder < 0)) return Standard_False;
  Standard_Real D1 = fabs(FirstParameter), D2 = fabs(LastParameter);
  if (D1 > 100 || D2 > 100) return Standard_False;
  D2 += D1;
  if (D2 < 0.01) return Standard_False;
  if (fabs(LastParameter - FirstParameter) / D2 < 0.01) return Standard_False; 

  // Calcul de la matrice a inverser (MAT)

  Ordre[0] = FirstOrder+1;
  Ordre[1] = LastOrder+1;

  for (cote=0; cote<=1; cote++) {
    Coeff.Init(1);

    for (pp=1; pp<=Ordre[cote]; pp++) {
      ii = pp + iof;
      Prod = 1;

      for (jj=pp; jj<=NbCoeff; jj++) {
	//        tout se passe dans les 3 lignes suivantes
	MAT(ii, jj) = Coeff(jj) * Prod;
	Coeff(jj) *= jj - pp;
	Prod      *= TBorne;
      }
    }
    TBorne = LastParameter;
    iof = Ordre[0];
  }

  // resolution du systemes
  math_Gauss ResolCoeff(MAT, 1.0e-10);
  if (!ResolCoeff.IsDone()) return Standard_False;
  
  for (ii=1; ii<=NbCoeff; ii++) {
    B(ii) = 1;
    ResolCoeff.Solve(B, Coeff);
    MatrixCoefs.SetRow( ii, Coeff);
    B(ii) = 0;
  }
  return Standard_True;      
}

//=======================================================================
//function : CoefficientsPoles
//purpose  : 
//=======================================================================

void PLib::CoefficientsPoles (const TColgp_Array1OfPnt&   Coefs,
			      const TColStd_Array1OfReal* WCoefs,
			      TColgp_Array1OfPnt&         Poles, 
			      TColStd_Array1OfReal*       Weights)
{
  TColStd_Array1OfReal tempC(1,3*Coefs.Length());
  PLib::SetPoles(Coefs,tempC);
  TColStd_Array1OfReal tempP(1,3*Poles.Length());
  PLib::SetPoles(Coefs,tempP);
  PLib::CoefficientsPoles(3,tempC,WCoefs,tempP,Weights);
  PLib::GetPoles(tempP,Poles);
}

//=======================================================================
//function : CoefficientsPoles
//purpose  : 
//=======================================================================

void PLib::CoefficientsPoles (const TColgp_Array1OfPnt2d& Coefs,
			      const TColStd_Array1OfReal* WCoefs,
			      TColgp_Array1OfPnt2d&       Poles, 
			      TColStd_Array1OfReal*       Weights)
{
  TColStd_Array1OfReal tempC(1,2*Coefs.Length());
  PLib::SetPoles(Coefs,tempC);
  TColStd_Array1OfReal tempP(1,2*Poles.Length());
  PLib::SetPoles(Coefs,tempP);
  PLib::CoefficientsPoles(2,tempC,WCoefs,tempP,Weights);
  PLib::GetPoles(tempP,Poles);
}

//=======================================================================
//function : CoefficientsPoles
//purpose  : 
//=======================================================================

void PLib::CoefficientsPoles (const TColStd_Array1OfReal& Coefs,
			      const TColStd_Array1OfReal* WCoefs,
			      TColStd_Array1OfReal&       Poles, 
			      TColStd_Array1OfReal*       Weights)
{
  PLib::CoefficientsPoles(1,Coefs,WCoefs,Poles,Weights);
}

//=======================================================================
//function : CoefficientsPoles
//purpose  : 
//=======================================================================

void PLib::CoefficientsPoles (const Standard_Integer      dim,
			      const TColStd_Array1OfReal& Coefs,
			      const TColStd_Array1OfReal* WCoefs,
			      TColStd_Array1OfReal&       Poles, 
			      TColStd_Array1OfReal*       Weights)
{
  Standard_Boolean rat = WCoefs != NULL;
  Standard_Integer loc = Coefs.Lower();
  Standard_Integer lop = Poles.Lower();
  Standard_Integer lowc=0;
  Standard_Integer lowp=0;
  Standard_Integer upc = Coefs.Upper();
  Standard_Integer upp = Poles.Upper();
  Standard_Integer upwc=0;
  Standard_Integer upwp=0;
  Standard_Integer reflen = Coefs.Length()/dim;
  Standard_Integer i,j,k; 
  //Les Extremites.
  if (rat) { 
    lowc = WCoefs->Lower(); lowp = Weights->Lower();
    upwc = WCoefs->Upper(); upwp = Weights->Upper();
  }

  for (i = 0; i < dim; i++){
    Poles (lop + i) = Coefs (loc + i);
    Poles (upp - i) = Coefs (upc - i);
  }
  if (rat) {
    (*Weights) (lowp) = (*WCoefs) (lowc);
    (*Weights) (upwp) = (*WCoefs) (upwc);
  }
  
  Standard_Real Cnp;
  for (i = 2; i < reflen; i++ ) {
    Cnp = PLib::Bin(reflen - 1, i - 1);
    if (rat) (*Weights)(lowp + i - 1) = (*WCoefs)(lowc + i - 1) / Cnp;

    for(j = 0; j < dim; j++){
      Poles(lop + dim * (i-1) + j)= Coefs(loc + dim * (i-1) + j) / Cnp;
    }
  }
  
  for (i = 1; i <= reflen - 1; i++) {

    for (j = reflen - 1; j >= i; j--) {
      if (rat) (*Weights)(lowp + j) += (*Weights)(lowp + j - 1);

      for(k = 0; k < dim; k++){
	Poles(lop + dim * j + k) += Poles(lop + dim * (j - 1) + k);
      }
    }
  }
  if (rat) {

    for (i = 1; i <= reflen; i++) {

      for(j = 0; j < dim; j++){
	Poles(lop + dim * (i-1) + j) /= (*Weights)(lowp + i -1);
      }
    }
  }
}

//=======================================================================
//function : Trimming
//purpose  : 
//=======================================================================

void PLib::Trimming(const Standard_Real   U1, 
		    const Standard_Real   U2, 
		    TColgp_Array1OfPnt&   Coefs,
		    TColStd_Array1OfReal* WCoefs)
{
  TColStd_Array1OfReal temp(1,3*Coefs.Length());
  PLib::SetPoles(Coefs,temp);
  PLib::Trimming(U1,U2,3,temp,WCoefs);
  PLib::GetPoles(temp,Coefs);
}

//=======================================================================
//function : Trimming
//purpose  : 
//=======================================================================

void PLib::Trimming(const Standard_Real   U1, 
		    const Standard_Real   U2, 
		    TColgp_Array1OfPnt2d& Coefs,
		    TColStd_Array1OfReal* WCoefs)
{
  TColStd_Array1OfReal temp(1,2*Coefs.Length());
  PLib::SetPoles(Coefs,temp);
  PLib::Trimming(U1,U2,2,temp,WCoefs);
  PLib::GetPoles(temp,Coefs);
}

//=======================================================================
//function : Trimming
//purpose  : 
//=======================================================================

void PLib::Trimming(const Standard_Real   U1, 
		    const Standard_Real   U2, 
		    TColStd_Array1OfReal& Coefs,
		    TColStd_Array1OfReal* WCoefs)
{
  PLib::Trimming(U1,U2,1,Coefs,WCoefs);
}

//=======================================================================
//function : Trimming
//purpose  : 
//=======================================================================

void PLib::Trimming(const Standard_Real U1, 
		    const Standard_Real U2, 
		    const Standard_Integer dim, 
		    TColStd_Array1OfReal& Coefs,
		    TColStd_Array1OfReal* WCoefs)
{

  // principe :
  // on fait le changement de variable v = (u-U1) / (U2-U1)
  // on exprime u = f(v) que l'on remplace dans l'expression polynomiale
  // decomposee sous la forme du schema iteratif de horner.

  Standard_Real lsp = U2 - U1;
  Standard_Integer indc, indw=0;
  Standard_Integer upc = Coefs.Upper() - dim + 1, upw=0;
  Standard_Integer len = Coefs.Length()/dim;
  Standard_Boolean rat = WCoefs != NULL;

  if (rat) {
    if(len != WCoefs->Length())
      throw Standard_Failure("PLib::Trimming : nbcoefs/dim != nbweights !!!");
    upw = WCoefs->Upper();
  }
  len --;

  for (Standard_Integer i = 1; i <= len; i++) {
    Standard_Integer j ;
    indc = upc - dim*(i-1);
    if (rat) indw = upw - i + 1;
    //calcul du coefficient de degre le plus faible a l'iteration i

    for( j = 0; j < dim; j++){
      Coefs(indc - dim + j) += U1 * Coefs(indc + j);
    }
    if (rat) (*WCoefs)(indw - 1) += U1 * (*WCoefs)(indw);
    
    //calcul des coefficients intermediaires :

    while (indc < upc){
      indc += dim;

      for(Standard_Integer k = 0; k < dim; k++){
	Coefs(indc - dim + k) = 
	  U1 * Coefs(indc + k) + lsp * Coefs(indc - dim + k);
      }
      if (rat) {
	indw ++;
        (*WCoefs)(indw - 1) = U1 * (*WCoefs)(indw) + lsp * (*WCoefs)(indw - 1);
      }
    }

    //calcul du coefficient de degre le plus eleve :

    for(j = 0; j < dim; j++){
      Coefs(upc + j) *= lsp;
    }
    if (rat) (*WCoefs)(upw) *= lsp;
  }
}

//=======================================================================
//function : CoefficientsPoles
//purpose  : 
// Modified: 21/10/1996 by PMN :  PolesCoefficient (PRO5852).
// on ne bidouille plus les u et v c'est a l'appelant de savoir ce qu'il
// fait avec BuildCache ou plus simplement d'utiliser PolesCoefficients
//=======================================================================

void PLib::CoefficientsPoles (const TColgp_Array2OfPnt&   Coefs,
			      const TColStd_Array2OfReal* WCoefs,
			      TColgp_Array2OfPnt&         Poles,
			      TColStd_Array2OfReal*       Weights) 
{
  Standard_Boolean rat = (WCoefs != NULL);
  Standard_Integer LowerRow  = Poles.LowerRow();
  Standard_Integer UpperRow  = Poles.UpperRow();
  Standard_Integer LowerCol  = Poles.LowerCol();
  Standard_Integer UpperCol  = Poles.UpperCol();
  Standard_Integer ColLength = Poles.ColLength();
  Standard_Integer RowLength = Poles.RowLength();

  // Bidouille pour retablir u et v pour les coefs calcules 
  // par buildcache
//  Standard_Boolean inv = Standard_False; //ColLength != Coefs.ColLength();

  Standard_Integer Row, Col;
  Standard_Real W, Cnp;

  Standard_Integer I1, I2;
  Standard_Integer NPoleu , NPolev;
  gp_XYZ Temp;

  for (NPoleu = LowerRow; NPoleu <= UpperRow; NPoleu++){
    Poles (NPoleu, LowerCol) =  Coefs (NPoleu, LowerCol);
    if (rat) {
      (*Weights) (NPoleu, LowerCol) =  (*WCoefs) (NPoleu, LowerCol);
    }

    for (Col = LowerCol + 1; Col <= UpperCol - 1; Col++) {
      Cnp = PLib::Bin(RowLength - 1,Col - LowerCol);
      Temp = Coefs (NPoleu, Col).XYZ();
      Temp.Divide (Cnp);
      Poles (NPoleu, Col).SetXYZ (Temp);
      if (rat) {
	(*Weights) (NPoleu, Col) = (*WCoefs) (NPoleu, Col) /  Cnp;
      }
    }
    Poles (NPoleu, UpperCol) = Coefs (NPoleu, UpperCol);
    if (rat) {
      (*Weights) (NPoleu, UpperCol) = (*WCoefs) (NPoleu, UpperCol);
    }

    for (I1 = 1; I1 <= RowLength - 1; I1++) {

      for (I2 = UpperCol; I2 >= LowerCol + I1; I2--) {
        Temp.SetLinearForm 
	  (Poles (NPoleu, I2).XYZ(), Poles (NPoleu, I2-1).XYZ());
	Poles (NPoleu, I2).SetXYZ (Temp);
	if (rat) (*Weights)(NPoleu, I2) += (*Weights)(NPoleu, I2-1);
      }
    } 
  }
  
  for (NPolev = LowerCol; NPolev <= UpperCol; NPolev++){

    for (Row = LowerRow + 1; Row <= UpperRow - 1; Row++) {
      Cnp = PLib::Bin(ColLength - 1,Row - LowerRow);
      Temp = Poles (Row, NPolev).XYZ();
      Temp.Divide (Cnp);
      Poles (Row, NPolev).SetXYZ (Temp);
      if (rat) (*Weights)(Row, NPolev) /= Cnp;
    }

    for (I1 = 1; I1 <= ColLength - 1; I1++) {

      for (I2 = UpperRow; I2 >= LowerRow + I1; I2--) {
        Temp.SetLinearForm 
	  (Poles (I2, NPolev).XYZ(), Poles (I2-1, NPolev).XYZ());
	Poles (I2, NPolev).SetXYZ (Temp);
	if (rat) (*Weights)(I2, NPolev) += (*Weights)(I2-1, NPolev);
      }
    }
  }
  if (rat) {

    for (Row = LowerRow; Row <= UpperRow; Row++) {

      for (Col = LowerCol; Col <= UpperCol; Col++) {
	W = (*Weights) (Row, Col);
	Temp = Poles(Row, Col).XYZ();
	Temp.Divide (W);
	Poles(Row, Col).SetXYZ (Temp);
      }
    }
  }
}  

//=======================================================================
//function : UTrimming
//purpose  : 
//=======================================================================

void PLib::UTrimming(const Standard_Real U1, 
		     const Standard_Real U2, 
		     TColgp_Array2OfPnt& Coeffs, 
		     TColStd_Array2OfReal* WCoeffs)
{
  Standard_Boolean rat = WCoeffs != NULL;
  Standard_Integer lr = Coeffs.LowerRow();
  Standard_Integer ur = Coeffs.UpperRow();
  Standard_Integer lc = Coeffs.LowerCol();
  Standard_Integer uc = Coeffs.UpperCol();
  TColgp_Array1OfPnt  Temp (lr,ur);
  TColStd_Array1OfReal Temw (lr,ur);

  for (Standard_Integer icol = lc; icol <= uc; icol++) {
    Standard_Integer irow ;
    for ( irow = lr; irow <= ur; irow++) {
      Temp (irow) = Coeffs  (irow, icol);
      if (rat) Temw (irow) = (*WCoeffs) (irow, icol);
    }
    if (rat) PLib::Trimming (U1, U2, Temp, &Temw);
    else PLib::Trimming (U1, U2, Temp, PLib::NoWeights());

    for (irow = lr; irow <= ur; irow++) {
      Coeffs  (irow, icol) = Temp (irow);
      if (rat) (*WCoeffs) (irow, icol) = Temw (irow);
    }
  }
}

//=======================================================================
//function : VTrimming
//purpose  : 
//=======================================================================

void PLib::VTrimming(const Standard_Real V1, 
		     const Standard_Real V2, 
		     TColgp_Array2OfPnt& Coeffs, 
		     TColStd_Array2OfReal* WCoeffs)
{
  Standard_Boolean rat = WCoeffs != NULL;
  Standard_Integer lr = Coeffs.LowerRow();
  Standard_Integer ur = Coeffs.UpperRow();
  Standard_Integer lc = Coeffs.LowerCol();
  Standard_Integer uc = Coeffs.UpperCol();
  TColgp_Array1OfPnt  Temp (lc,uc);
  TColStd_Array1OfReal Temw (lc,uc);

  for (Standard_Integer irow = lr; irow <= ur; irow++) {
    Standard_Integer icol ;
    for ( icol = lc; icol <= uc; icol++) {
      Temp (icol) = Coeffs  (irow, icol);
      if (rat) Temw (icol) = (*WCoeffs) (irow, icol);
    }
    if (rat) PLib::Trimming (V1, V2, Temp, &Temw);
    else PLib::Trimming (V1, V2, Temp, PLib::NoWeights());

    for (icol = lc; icol <= uc; icol++) {
      Coeffs  (irow, icol) = Temp (icol);
      if (rat) (*WCoeffs) (irow, icol) = Temw (icol);
    }
  }
}

//=======================================================================
//function : HermiteInterpolate
//purpose  : 
//=======================================================================

Standard_Boolean PLib::HermiteInterpolate
(const Standard_Integer Dimension, 
 const Standard_Real FirstParameter, 
 const Standard_Real LastParameter, 
 const Standard_Integer FirstOrder, 
 const Standard_Integer LastOrder, 
 const TColStd_Array2OfReal& FirstConstr,
 const TColStd_Array2OfReal& LastConstr,
 TColStd_Array1OfReal& Coefficients)
{
  Standard_Real Pattern[3][6];

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

  math_Matrix A(0,FirstOrder+LastOrder+1, 0,FirstOrder+LastOrder+1);
  //  The initialisation of the matrix A
  Standard_Integer irow ;
  for ( irow=0; irow<=FirstOrder; irow++) {
    Standard_Real FirstVal = 1.;

    for (Standard_Integer icol=0; icol<=FirstOrder+LastOrder+1; icol++) {
      A(irow,icol) = Pattern[irow][icol]*FirstVal;
      if (irow <= icol) FirstVal *= FirstParameter;
    }
  }

  for (irow=0; irow<=LastOrder; irow++) {
    Standard_Real LastVal  = 1.;

    for (Standard_Integer icol=0; icol<=FirstOrder+LastOrder+1; icol++) {
      A(irow+FirstOrder+1,icol) = Pattern[irow][icol]*LastVal;
      if (irow <= icol) LastVal *= LastParameter;
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
  //  cout << "A=" << A << endl;

  for (Standard_Integer idim=1; idim<=Dimension; idim++) {
    //  cout << "idim=" << idim << endl;

    math_Vector B(0,FirstOrder+LastOrder+1);
    Standard_Integer icol ;
    for ( icol=0; icol<=FirstOrder; icol++) 
      B(icol) = FirstConstr(idim,icol);

    for (icol=0; icol<=LastOrder; icol++) 
      B(FirstOrder+1+icol) = LastConstr(idim,icol);
    //  cout << "B=" << B << endl;

    //  The solving of equations system A * X = B. Then B = X
    Equations.Solve(B);         
    //  cout << "After Solving" << endl << "B=" << B << endl;

    if (Equations.IsDone()==Standard_False) return Standard_False;
    
    //  the filling of the Coefficients

    for (icol=0; icol<=FirstOrder+LastOrder+1; icol++) 
      Coefficients(Dimension*icol+idim-1) = B(icol);
  } 
  return Standard_True;
}

//=======================================================================
//function : JacobiParameters
//purpose  : 
//=======================================================================

void PLib::JacobiParameters(const GeomAbs_Shape ConstraintOrder, 
			    const Standard_Integer MaxDegree, 
			    const Standard_Integer Code, 
			    Standard_Integer& NbGaussPoints, 
			    Standard_Integer& WorkDegree)
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

  const Standard_Integer NDEG8=8,   NDEG10=10, NDEG15=15, NDEG20=20, NDEG25=25, 
  NDEG30=30, NDEG40=40, NDEG50=50, NDEG61=61;

  Standard_Integer NivConstr=0;
  switch (ConstraintOrder) {
  case GeomAbs_C0: NivConstr = 0; break;
  case GeomAbs_C1: NivConstr = 1; break;
  case GeomAbs_C2: NivConstr = 2; break;
  default: 
    throw Standard_ConstructionError("Invalid ConstraintOrder");
  }
  if (MaxDegree < 2*NivConstr+1)
    throw Standard_ConstructionError("Invalid MaxDegree");
  
  if (Code >= 1)
    WorkDegree = MaxDegree + 9;
  else 
    WorkDegree = MaxDegree + 6;
  
  //---> Nbre mini de points necessaires.
  Standard_Integer IPMIN=0;
  if (WorkDegree < NDEG8) 
    IPMIN=NDEG8;
  else if (WorkDegree < NDEG10)
    IPMIN=NDEG10;
  else if (WorkDegree < NDEG15) 
    IPMIN=NDEG15;
  else if (WorkDegree < NDEG20) 
    IPMIN=NDEG20;
  else if (WorkDegree < NDEG25)
    IPMIN=NDEG25;
  else if (WorkDegree < NDEG30) 
    IPMIN=NDEG30;
  else if (WorkDegree < NDEG40) 
    IPMIN=NDEG40;
  else if (WorkDegree < NDEG50) 
    IPMIN=NDEG50;
  else if (WorkDegree < NDEG61) 
    IPMIN=NDEG61;
  else
    throw Standard_ConstructionError("Invalid MaxDegree");
  // ---> Nbre de points voulus.
  Standard_Integer IWANT=0;
  switch (Code) {
  case -5: IWANT=NDEG8;  break;
  case -4: IWANT=NDEG10; break;
  case -3: IWANT=NDEG15; break;
  case -2: IWANT=NDEG20; break;
  case -1: IWANT=NDEG25; break;
  case  1: IWANT=NDEG30; break;
  case  2: IWANT=NDEG40; break;
  case  3: IWANT=NDEG50; break;
  case  4: IWANT=NDEG61; break;
  default: 
    throw Standard_ConstructionError("Invalid Code");
  }      
  //-->  NbGaussPoints est le nombre de points de discretisation de la fonction,
  //     il ne peut prendre que les valeurs 8,10,15,20,25,30,40,50 ou 61.
  //     NbGaussPoints doit etre superieur strictement a WorkDegree.
  NbGaussPoints = Max(IPMIN,IWANT);
  //  NbGaussPoints +=2;
}

//=======================================================================
//function : NivConstr
//purpose  : translates from GeomAbs_Shape to Integer
//=======================================================================

 Standard_Integer PLib::NivConstr(const GeomAbs_Shape ConstraintOrder) 
{
  Standard_Integer NivConstr=0;
  switch (ConstraintOrder) {
    case GeomAbs_C0: NivConstr = 0; break;
    case GeomAbs_C1: NivConstr = 1; break;
    case GeomAbs_C2: NivConstr = 2; break;
    default: 
      throw Standard_ConstructionError("Invalid ConstraintOrder");
  }
  return NivConstr;
}

//=======================================================================
//function : ConstraintOrder
//purpose  : translates from Integer to GeomAbs_Shape
//=======================================================================

 GeomAbs_Shape PLib::ConstraintOrder(const Standard_Integer NivConstr) 
{
  GeomAbs_Shape ConstraintOrder=GeomAbs_C0;
  switch (NivConstr) {
    case 0: ConstraintOrder = GeomAbs_C0; break;
    case 1: ConstraintOrder = GeomAbs_C1; break;
    case 2: ConstraintOrder = GeomAbs_C2; break;
    default: 
      throw Standard_ConstructionError("Invalid NivConstr");
  }
  return ConstraintOrder;
}

//=======================================================================
//function : EvalLength
//purpose  : 
//=======================================================================

 void PLib::EvalLength(const Standard_Integer Degree, const Standard_Integer Dimension, 
                      Standard_Real& PolynomialCoeff,
		      const Standard_Real U1, const Standard_Real U2,
		      Standard_Real& Length)
{
  Standard_Integer i,j,idim, degdim;
  Standard_Real C1,C2,Sum,Tran,X1,X2,Der1,Der2,D1,D2,DD;

  Standard_Real *PolynomialArray = &PolynomialCoeff ;

  Standard_Integer NbGaussPoints = 4 * Min((Degree/4)+1,10);

  math_Vector GaussPoints(1,NbGaussPoints);
  math::GaussPoints(NbGaussPoints,GaussPoints);

  math_Vector GaussWeights(1,NbGaussPoints);
  math::GaussWeights(NbGaussPoints,GaussWeights);

  C1 = (U2 + U1) / 2.;
  C2 = (U2 - U1) / 2.;

//-----------------------------------------------------------
//****** Integration - Boucle sur les intervalles de GAUSS **
//-----------------------------------------------------------

  Sum = 0;

  for (j=1; j<=NbGaussPoints/2; j++) {
    // Integration en tenant compte de la symetrie 
    Tran  = C2 * GaussPoints(j);
    X1 = C1 + Tran;
    X2 = C1 - Tran;

    //****** Derivation sur la dimension de l'espace **

    degdim =  Degree*Dimension;
    Der1 = Der2 = 0.;
    for (idim=0; idim<Dimension; idim++) {
      D1 = D2 = Degree * PolynomialArray [idim + degdim];
      for (i=Degree-1; i>=1; i--) {
	DD = i * PolynomialArray [idim + i*Dimension];
	D1 = D1 * X1 + DD;
	D2 = D2 * X2 + DD;
      }
      Der1 += D1 * D1;
      Der2 += D2 * D2;
    }

    //****** Integration **

    Sum += GaussWeights(j) * C2 * (Sqrt(Der1) + Sqrt(Der2));

//****** Fin de boucle dur les intervalles de GAUSS **
  }
  Length = Sum;
}


//=======================================================================
//function : EvalLength
//purpose  : 
//=======================================================================

 void PLib::EvalLength(const Standard_Integer Degree, const Standard_Integer Dimension, 
                      Standard_Real& PolynomialCoeff,
		      const Standard_Real U1, const Standard_Real U2,
		      const Standard_Real Tol, 
		      Standard_Real& Length, Standard_Real& Error)
{
  Standard_Integer i;
  Standard_Integer NbSubInt = 1,    // Current number of subintervals
                   MaxNbIter = 13,  // Max number of iterations
                   NbIter    = 1;   // Current number of iterations
  Standard_Real    dU,OldLen,LenI;

  PLib::EvalLength(Degree,Dimension, PolynomialCoeff, U1,U2, Length);

  do {
    OldLen = Length;
    Length = 0.;
    NbSubInt *= 2;
    dU = (U2-U1)/NbSubInt;    
    for (i=1; i<=NbSubInt; i++) {
      PLib::EvalLength(Degree,Dimension, PolynomialCoeff, U1+(i-1)*dU,U1+i*dU, LenI);
      Length += LenI;
    }
    NbIter++;
    Error = Abs(OldLen-Length);
  }    
  while (Error > Tol && NbIter <= MaxNbIter);
}  
