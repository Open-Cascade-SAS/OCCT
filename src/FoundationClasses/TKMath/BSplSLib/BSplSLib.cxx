// Created on: 1991-08-26
// Created by: JCV
// Copyright (c) 1991-1999 Matra Datavision
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

// Modified RLE Aug 93 - Complete rewrite
// xab  21-Mar-95  implemented cache mechanism
// pmn  25-09-96   Interpolation
// jct  25-09-96 : Correction de l'alloc de LocalArray dans RationalDerivative.
// pmn  07-10-96 : Correction de DN dans le cas rationnal.
// pmn  06-02-97 : Correction des poids dans RationalDerivative. (PRO700)

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>
#include <NCollection_LocalArray.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>

// for null derivatives
static constexpr double BSplSLib_zero[3] = {0.0, 0.0, 0.0};

#ifndef M_SQRT2
  #define M_SQRT2 1.41421356237309504880168872420969808
#endif

namespace
{
//! Maximum supported degree for B-spline surfaces.
static constexpr int THE_MAX_DEGREE = BSplCLib::MaxDegree();

//! Validates that the given degrees do not exceed the maximum supported degree.
//! @param theUDegree degree in U direction
//! @param theVDegree degree in V direction
//! @throws Standard_OutOfRange if either degree exceeds MaxDegree()
void validateBSplineDegree([[maybe_unused]] int theUDegree, [[maybe_unused]] int theVDegree)
{
  Standard_OutOfRange_Raise_if(theUDegree > THE_MAX_DEGREE || theVDegree > THE_MAX_DEGREE,
                               "BSplSLib: bspline degree is greater than maximum supported");
}

//=======================================================================
// struct : BSplSLib_DataContainer
// purpose: Auxiliary structure providing buffers for poles and knots used in
//         evaluation of bspline (allocated in the stack)
//=======================================================================
struct BSplSLib_DataContainer
{
  double poles[4 * (THE_MAX_DEGREE + 1) * (THE_MAX_DEGREE + 1)];
  double knots1[2 * THE_MAX_DEGREE];
  double knots2[2 * THE_MAX_DEGREE];
  double ders[48];
};
} // namespace

//**************************************************************************
//                     Evaluation methods
//**************************************************************************

//=======================================================================
// function : RationalDerivative
// purpose  : computes the rational derivatives when we have the
//           the derivatives of the homogeneous numerator and the
//           the derivatives of the denominator
//=======================================================================

void BSplSLib::RationalDerivative(const int  UDeg,
                                  const int  VDeg,
                                  const int  N,
                                  const int  M,
                                  double&    HDerivatives,
                                  double&    RDerivatives,
                                  const bool All)
{
  //
  //  if All is True all derivatives are computed. if Not only
  //  the requested N, M is computed
  //
  //                                           Numerator(u,v)
  //   let f(u,v) be a rational function  = ------------------
  //                                         Denominator(u,v)
  //
  //
  //   Let (N,M) the order of the derivatives we want : then since
  //   we have :
  //
  //         Numerator = f * Denominator
  //
  //   we derive :
  //
  //   (N,M)         1           (         (N M)                  (p q)            (N -p M-q) )
  //  f       =  ------------    (  Numerator   -  SUM SUM  a   * f    * Denominator          )
  //                       (0,0) (                 p<N q<M   p q                              )
  //             Denominator
  //
  //   with :
  //
  //              ( N )  ( M )
  //    a      =  (   )  (   )
  //     p q      ( p )  ( q )
  //
  //
  //   HDerivatives is an array where derivatives are stored in the following form
  //   Numerator is assumee to have 3 functions that is a vector of dimension
  //   3
  //
  //             (0,0)           (0,0)                       (0, DegV)           (0, DegV)
  //     Numerator     Denominator      ...         Numerator          Denominator
  //
  //             (1,0)           (1,0)                       (1, DegV)           (1, DegV)
  //     Numerator     Denominator      ...         Numerator          Denominator
  //
  //         ...........................................................
  //
  //
  //            (DegU,0)        (DegU,0)                  (DegU, DegV)           (DegU, DegV)
  //     Numerator     Denominator      ...         Numerator          Denominator
  //
  //
  int ii, jj, pp, qq, index, index1, index2;
  int M1, M3, M4, N1, iiM1, iiM3, jjM1, ppM1, ppM3;
  int MinN, MinN1, MinM, MinM1;
  int index_u, index_u1, index_v, index_v1, index_w;

  M1 = M + 1;
  N1 = N + 1;
  ii = N1 * M1;
  M3 = (M1 << 1) + M1;
  M4 = (VDeg + 1) << 2;

  NCollection_LocalArray<double> StoreDerivatives(All ? 0 : ii * 3);
  double*                        RArray = (All ? &RDerivatives : (double*)StoreDerivatives);
  NCollection_LocalArray<double> StoreW(ii);
  double*                        HomogeneousArray = &HDerivatives;
  double                         denominator, Pii, Pip, Pjq;

  denominator = 1.0e0 / HomogeneousArray[3];
  index_u     = 0;
  index_u1    = 0;
  if (UDeg < N)
    MinN = UDeg;
  else
    MinN = N;
  if (VDeg < M)
    MinM = VDeg;
  else
    MinM = M;
  MinN1 = MinN + 1;
  MinM1 = MinM + 1;
  iiM1  = -M1;

  for (ii = 0; ii < MinN1; ii++)
  {
    iiM1 += M1;
    index_v  = index_u;
    index_v1 = index_u1;
    index_w  = iiM1;

    for (jj = 0; jj < MinM1; jj++)
    {
      RArray[index_v++] = HomogeneousArray[index_v1++];
      RArray[index_v++] = HomogeneousArray[index_v1++];
      RArray[index_v++] = HomogeneousArray[index_v1++];
      StoreW[index_w++] = HomogeneousArray[index_v1++];
    }

    for (jj = MinM1; jj < M1; jj++)
    {
      RArray[index_v++] = 0.;
      RArray[index_v++] = 0.;
      RArray[index_v++] = 0.;
      StoreW[index_w++] = 0.;
    }
    index_u1 += M4;
    index_u += M3;
  }
  index_v = MinN1 * M3;
  index_w = MinN1 * M1;

  for (ii = MinN1; ii < N1; ii++)
  {

    for (jj = 0; jj < M1; jj++)
    {
      RArray[index_v++] = 0.0e0;
      RArray[index_v++] = 0.0e0;
      RArray[index_v++] = 0.0e0;
      StoreW[index_w++] = 0.0e0;
    }
  }

  // ---------------  Calculation ----------------

  iiM1 = -M1;
  iiM3 = -M3;

  for (ii = 0; ii <= N; ii++)
  {
    iiM1 += M1;
    iiM3 += M3;
    index1 = iiM3 - 3;
    jjM1   = iiM1;

    for (jj = 0; jj <= M; jj++)
    {
      jjM1++;
      ppM1 = -M1;
      ppM3 = -M3;
      index1 += 3;

      for (pp = 0; pp < ii; pp++)
      {
        ppM1 += M1;
        ppM3 += M3;
        index  = ppM3;
        index2 = jjM1 - ppM1;
        Pip    = PLib::Bin(ii, pp);

        for (qq = 0; qq <= jj; qq++)
        {
          index2--;
          Pjq = Pip * PLib::Bin(jj, qq) * StoreW[index2];
          RArray[index1] -= Pjq * RArray[index];
          index++;
          index1++;
          RArray[index1] -= Pjq * RArray[index];
          index++;
          index1++;
          RArray[index1] -= Pjq * RArray[index];
          index++;
          index1 -= 2;
        }
      }
      index  = iiM3;
      index2 = jj + 1;
      Pii    = PLib::Bin(ii, ii);

      for (qq = 0; qq < jj; qq++)
      {
        index2--;
        Pjq = Pii * PLib::Bin(jj, qq) * StoreW[index2];
        RArray[index1] -= Pjq * RArray[index];
        index++;
        index1++;
        RArray[index1] -= Pjq * RArray[index];
        index++;
        index1++;
        RArray[index1] -= Pjq * RArray[index];
        index++;
        index1 -= 2;
      }
      RArray[index1] *= denominator;
      index1++;
      RArray[index1] *= denominator;
      index1++;
      RArray[index1] *= denominator;
      index1 -= 2;
    }
  }
  if (!All)
  {
    RArray    = &RDerivatives;
    index     = N * M1 + M;
    index     = (index << 1) + index;
    RArray[0] = StoreDerivatives[index];
    index++;
    RArray[1] = StoreDerivatives[index];
    index++;
    RArray[2] = StoreDerivatives[index];
  }
}

//=================================================================================================

//
// PrepareEval :
//
// Prepare all data for computing points :
//  local arrays of knots
//  local array  of poles (multiplied by the weights if rational)
//
//  The first direction to compute (smaller degree) is returned
//  and the poles are stored according to this direction.

static bool PrepareEval(const double                      U,
                        const double                      V,
                        const int                         Uindex,
                        const int                         Vindex,
                        const int                         UDegree,
                        const int                         VDegree,
                        const bool                        URat,
                        const bool                        VRat,
                        const bool                        UPer,
                        const bool                        VPer,
                        const NCollection_Array2<gp_Pnt>& Poles,
                        const NCollection_Array2<double>* Weights,
                        const NCollection_Array1<double>& UKnots,
                        const NCollection_Array1<double>& VKnots,
                        const NCollection_Array1<int>*    UMults,
                        const NCollection_Array1<int>*    VMults,
                        double&                           u1, // first  parameter to use
                        double&                           u2, // second parameter to use
                        int&                              d1, // first degree
                        int&                              d2, // second degree
                        bool&                             rational,
                        BSplSLib_DataContainer&           dc)
{
  rational    = URat || VRat;
  int uindex  = Uindex;
  int vindex  = Vindex;
  int UKLower = UKnots.Lower();
  int UKUpper = UKnots.Upper();
  int VKLower = VKnots.Lower();
  int VKUpper = VKnots.Upper();

  if (UDegree <= VDegree)
  {
    // compute the indices
    if (uindex < UKLower || uindex > UKUpper)
      BSplCLib::LocateParameter(UDegree, UKnots, UMults, U, UPer, uindex, u1);
    else
      u1 = U;

    if (vindex < VKLower || vindex > VKUpper)
      BSplCLib::LocateParameter(VDegree, VKnots, VMults, V, VPer, vindex, u2);
    else
      u2 = V;

    // get the knots
    d1 = UDegree;
    d2 = VDegree;
    BSplCLib::BuildKnots(UDegree, uindex, UPer, UKnots, UMults, *dc.knots1);
    BSplCLib::BuildKnots(VDegree, vindex, VPer, VKnots, VMults, *dc.knots2);

    if (UMults == nullptr)
      uindex -= UKLower + UDegree;
    else
      uindex = BSplCLib::PoleIndex(UDegree, uindex, UPer, *UMults);

    if (VMults == nullptr)
      vindex -= VKLower + VDegree;
    else
      vindex = BSplCLib::PoleIndex(VDegree, vindex, VPer, *VMults);

    // get the poles
    int    i, j, ip, jp;
    double w, *pole = dc.poles;
    d1            = UDegree;
    d2            = VDegree;
    int PLowerRow = Poles.LowerRow();
    int PUpperRow = Poles.UpperRow();
    int PLowerCol = Poles.LowerCol();
    int PUpperCol = Poles.UpperCol();

    // verify if locally non rational
    if (rational)
    {
      rational = false;
      ip       = PLowerRow + uindex;
      jp       = PLowerCol + vindex;

      if (ip < PLowerRow)
        ip = PUpperRow;
      if (jp < PLowerCol)
        jp = PUpperCol;

      w          = Weights->Value(ip, jp);
      double eps = Epsilon(w);
      double dw;

      for (i = 0; i <= UDegree && !rational; i++)
      {
        jp = PLowerCol + vindex;

        if (jp < PLowerCol)
          jp = PUpperCol;

        for (j = 0; j <= VDegree && !rational; j++)
        {
          dw = Weights->Value(ip, jp) - w;
          if (dw < 0)
            dw = -dw;

          rational = (dw > eps);

          jp++;

          if (jp > PUpperCol)
            jp = PLowerCol;
        }

        ip++;

        if (ip > PUpperRow)
          ip = PLowerRow;
      }
    }

    // copy the poles
    ip = PLowerRow + uindex;

    if (ip < PLowerRow)
      ip = PUpperRow;

    if (rational)
    {
      for (i = 0; i <= d1; i++)
      {
        jp = PLowerCol + vindex;

        if (jp < PLowerCol)
          jp = PUpperCol;

        for (j = 0; j <= d2; j++)
        {
          const gp_Pnt& P = Poles.Value(ip, jp);
          pole[3] = w = Weights->Value(ip, jp);
          pole[0]     = P.X() * w;
          pole[1]     = P.Y() * w;
          pole[2]     = P.Z() * w;
          pole += 4;
          jp++;

          if (jp > PUpperCol)
            jp = PLowerCol;
        }

        ip++;

        if (ip > PUpperRow)
          ip = PLowerRow;
      }
    }
    else
    {
      for (i = 0; i <= d1; i++)
      {
        jp = PLowerCol + vindex;

        if (jp < PLowerCol)
          jp = PUpperCol;

        for (j = 0; j <= d2; j++)
        {
          const gp_Pnt& P = Poles.Value(ip, jp);
          pole[0]         = P.X();
          pole[1]         = P.Y();
          pole[2]         = P.Z();
          pole += 3;
          jp++;

          if (jp > PUpperCol)
            jp = PLowerCol;
        }

        ip++;

        if (ip > PUpperRow)
          ip = PLowerRow;
      }
    }

    return true;
  }
  else
  {
    // compute the indices
    if (uindex < UKLower || uindex > UKUpper)
      BSplCLib::LocateParameter(UDegree, UKnots, UMults, U, UPer, uindex, u2);
    else
      u2 = U;

    if (vindex < VKLower || vindex > VKUpper)
      BSplCLib::LocateParameter(VDegree, VKnots, VMults, V, VPer, vindex, u1);
    else
      u1 = V;

    // get the knots

    d2 = UDegree;
    d1 = VDegree;

    BSplCLib::BuildKnots(UDegree, uindex, UPer, UKnots, UMults, *dc.knots2);
    BSplCLib::BuildKnots(VDegree, vindex, VPer, VKnots, VMults, *dc.knots1);

    if (UMults == nullptr)
      uindex -= UKLower + UDegree;
    else
      uindex = BSplCLib::PoleIndex(UDegree, uindex, UPer, *UMults);

    if (VMults == nullptr)
      vindex -= VKLower + VDegree;
    else
      vindex = BSplCLib::PoleIndex(VDegree, vindex, VPer, *VMults);

    // get the poles
    int    i, j, ip, jp;
    double w, *pole = dc.poles;
    d1            = VDegree;
    d2            = UDegree;
    int PLowerRow = Poles.LowerRow();
    int PUpperRow = Poles.UpperRow();
    int PLowerCol = Poles.LowerCol();
    int PUpperCol = Poles.UpperCol();

    // verify if locally non rational
    if (rational)
    {
      rational = false;
      ip       = PLowerRow + uindex;
      jp       = PLowerCol + vindex;

      if (ip < PLowerRow)
        ip = PUpperRow;

      if (jp < PLowerCol)
        jp = PUpperCol;

      w          = Weights->Value(ip, jp);
      double eps = Epsilon(w);
      double dw;

      for (i = 0; i <= UDegree && !rational; i++)
      {
        jp = PLowerCol + vindex;

        if (jp < PLowerCol)
          jp = PUpperCol;

        for (j = 0; j <= VDegree && !rational; j++)
        {
          dw = Weights->Value(ip, jp) - w;
          if (dw < 0)
            dw = -dw;
          rational = dw > eps;

          jp++;

          if (jp > PUpperCol)
            jp = PLowerCol;
        }

        ip++;

        if (ip > PUpperRow)
          ip = PLowerRow;
      }
    }

    // copy the poles
    jp = PLowerCol + vindex;

    if (jp < PLowerCol)
      jp = PUpperCol;

    if (rational)
    {
      for (i = 0; i <= d1; i++)
      {
        ip = PLowerRow + uindex;

        if (ip < PLowerRow)
          ip = PUpperRow;

        for (j = 0; j <= d2; j++)
        {
          const gp_Pnt& P = Poles.Value(ip, jp);
          pole[3] = w = Weights->Value(ip, jp);
          pole[0]     = P.X() * w;
          pole[1]     = P.Y() * w;
          pole[2]     = P.Z() * w;
          pole += 4;
          ip++;

          if (ip > PUpperRow)
            ip = PLowerRow;
        }

        jp++;

        if (jp > PUpperCol)
          jp = PLowerCol;
      }
    }
    else
    {
      for (i = 0; i <= d1; i++)
      {
        ip = PLowerRow + uindex;

        if (ip < PLowerRow)
          ip = PUpperRow;

        if (ip > PUpperRow)
          ip = PLowerRow;

        for (j = 0; j <= d2; j++)
        {
          const gp_Pnt& P = Poles.Value(ip, jp);
          pole[0]         = P.X();
          pole[1]         = P.Y();
          pole[2]         = P.Z();
          pole += 3;
          ip++;

          if (ip > PUpperRow)
            ip = PLowerRow;
        }

        jp++;

        if (jp > PUpperCol)
          jp = PLowerCol;
      }
    }

    return false;
  }
}

//=================================================================================================

void BSplSLib::D0(const double                      U,
                  const double                      V,
                  const int                         UIndex,
                  const int                         VIndex,
                  const NCollection_Array2<gp_Pnt>& Poles,
                  const NCollection_Array2<double>* Weights,
                  const NCollection_Array1<double>& UKnots,
                  const NCollection_Array1<double>& VKnots,
                  const NCollection_Array1<int>*    UMults,
                  const NCollection_Array1<int>*    VMults,
                  const int                         UDegree,
                  const int                         VDegree,
                  const bool                        URat,
                  const bool                        VRat,
                  const bool                        UPer,
                  const bool                        VPer,
                  gp_Pnt&                           P)
{
  //  int k ;
  double W;
  HomogeneousD0(U,
                V,
                UIndex,
                VIndex,
                Poles,
                Weights,
                UKnots,
                VKnots,
                UMults,
                VMults,
                UDegree,
                VDegree,
                URat,
                VRat,
                UPer,
                VPer,
                W,
                P);
  P.SetX(P.X() / W);
  P.SetY(P.Y() / W);
  P.SetZ(P.Z() / W);
}

//=================================================================================================

void BSplSLib::HomogeneousD0(const double                      U,
                             const double                      V,
                             const int                         UIndex,
                             const int                         VIndex,
                             const NCollection_Array2<gp_Pnt>& Poles,
                             const NCollection_Array2<double>* Weights,
                             const NCollection_Array1<double>& UKnots,
                             const NCollection_Array1<double>& VKnots,
                             const NCollection_Array1<int>*    UMults,
                             const NCollection_Array1<int>*    VMults,
                             const int                         UDegree,
                             const int                         VDegree,
                             const bool                        URat,
                             const bool                        VRat,
                             const bool                        UPer,
                             const bool                        VPer,
                             double&                           W,
                             gp_Pnt&                           P)
{
  bool rational;
  //  int k,dim;
  int    dim;
  double u1, u2;
  int    d1, d2;
  W = 1.0e0;

  validateBSplineDegree(UDegree, VDegree);
  BSplSLib_DataContainer dc;
  PrepareEval(U,
              V,
              UIndex,
              VIndex,
              UDegree,
              VDegree,
              URat,
              VRat,
              UPer,
              VPer,
              Poles,
              Weights,
              UKnots,
              VKnots,
              UMults,
              VMults,
              u1,
              u2,
              d1,
              d2,
              rational,
              dc);
  if (rational)
  {
    dim = 4;
    BSplCLib::Eval(u1, d1, *dc.knots1, dim * (d2 + 1), *dc.poles);
    BSplCLib::Eval(u2, d2, *dc.knots2, dim, *dc.poles);
    W = dc.poles[3];
    P.SetX(dc.poles[0]);
    P.SetY(dc.poles[1]);
    P.SetZ(dc.poles[2]);
  }
  else
  {
    dim = 3;
    BSplCLib::Eval(u1, d1, *dc.knots1, dim * (d2 + 1), *dc.poles);
    BSplCLib::Eval(u2, d2, *dc.knots2, dim, *dc.poles);
    P.SetX(dc.poles[0]);
    P.SetY(dc.poles[1]);
    P.SetZ(dc.poles[2]);
  }
}

//=================================================================================================

void BSplSLib::D1(const double                      U,
                  const double                      V,
                  const int                         UIndex,
                  const int                         VIndex,
                  const NCollection_Array2<gp_Pnt>& Poles,
                  const NCollection_Array2<double>* Weights,
                  const NCollection_Array1<double>& UKnots,
                  const NCollection_Array1<double>& VKnots,
                  const NCollection_Array1<int>*    UMults,
                  const NCollection_Array1<int>*    VMults,
                  const int                         UDegree,
                  const int                         VDegree,
                  const bool                        URat,
                  const bool                        VRat,
                  const bool                        UPer,
                  const bool                        VPer,
                  gp_Pnt&                           P,
                  gp_Vec&                           Vu,
                  gp_Vec&                           Vv)
{
  bool rational;
  //  int k,dim,dim2;
  int     dim, dim2;
  double  u1, u2;
  int     d1, d2;
  double *result, *resVu, *resVv;
  validateBSplineDegree(UDegree, VDegree);
  BSplSLib_DataContainer dc;
  if (PrepareEval(U,
                  V,
                  UIndex,
                  VIndex,
                  UDegree,
                  VDegree,
                  URat,
                  VRat,
                  UPer,
                  VPer,
                  Poles,
                  Weights,
                  UKnots,
                  VKnots,
                  UMults,
                  VMults,
                  u1,
                  u2,
                  d1,
                  d2,
                  rational,
                  dc))
  {
    if (rational)
    {
      dim  = 4;
      dim2 = (d2 + 1) << 2;
      BSplCLib::Bohm(u1, d1, 1, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *dc.poles);
      BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + dim2));
      BSplSLib::RationalDerivative(d1, d2, 1, 1, *dc.poles, *dc.ders);
      result = dc.ders;
      resVu  = result + 6;
      resVv  = result + 3;
    }
    else
    {
      dim  = 3;
      dim2 = d2 + 1;
      dim2 = (dim2 << 1) + dim2;
      BSplCLib::Bohm(u1, d1, 1, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *dc.poles);
      BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + dim2));
      result = dc.poles;
      resVu  = result + dim2;
      resVv  = result + 3;
    }
  }
  else
  {
    if (rational)
    {
      dim  = 4;
      dim2 = (d2 + 1) << 2;
      BSplCLib::Bohm(u1, d1, 1, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *dc.poles);
      BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + dim2));
      BSplSLib::RationalDerivative(d1, d2, 1, 1, *dc.poles, *dc.ders);
      result = dc.ders;
      resVu  = result + 3;
      resVv  = result + 6;
    }
    else
    {
      dim  = 3;
      dim2 = d2 + 1;
      dim2 = (dim2 << 1) + dim2;
      BSplCLib::Bohm(u1, d1, 1, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *dc.poles);
      BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + dim2));
      result = dc.poles;
      resVu  = result + 3;
      resVv  = result + dim2;
    }
  }

  P.SetX(result[0]);
  Vu.SetX(resVu[0]);
  Vv.SetX(resVv[0]);

  P.SetY(result[1]);
  Vu.SetY(resVu[1]);
  Vv.SetY(resVv[1]);

  P.SetZ(result[2]);
  Vu.SetZ(resVu[2]);
  Vv.SetZ(resVv[2]);
}

//=================================================================================================

void BSplSLib::HomogeneousD1(const double                      U,
                             const double                      V,
                             const int                         UIndex,
                             const int                         VIndex,
                             const NCollection_Array2<gp_Pnt>& Poles,
                             const NCollection_Array2<double>* Weights,
                             const NCollection_Array1<double>& UKnots,
                             const NCollection_Array1<double>& VKnots,
                             const NCollection_Array1<int>*    UMults,
                             const NCollection_Array1<int>*    VMults,
                             const int                         UDegree,
                             const int                         VDegree,
                             const bool                        URat,
                             const bool                        VRat,
                             const bool                        UPer,
                             const bool                        VPer,
                             gp_Pnt&                           N,
                             gp_Vec&                           Nu,
                             gp_Vec&                           Nv,
                             double&                           D,
                             double&                           Du,
                             double&                           Dv)
{
  bool rational;
  //  int k,dim;
  int    dim;
  double u1, u2;
  int    d1, d2;

  D  = 1.0e0;
  Du = 0.0e0;
  Dv = 0.0e0;
  validateBSplineDegree(UDegree, VDegree);
  BSplSLib_DataContainer dc;
  bool                   ufirst = PrepareEval(U,
                            V,
                            UIndex,
                            VIndex,
                            UDegree,
                            VDegree,
                            URat,
                            VRat,
                            UPer,
                            VPer,
                            Poles,
                            Weights,
                            UKnots,
                            VKnots,
                            UMults,
                            VMults,
                            u1,
                            u2,
                            d1,
                            d2,
                            rational,
                            dc);
  dim                           = rational ? 4 : 3;

  BSplCLib::Bohm(u1, d1, 1, *dc.knots1, dim * (d2 + 1), *dc.poles);
  BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *dc.poles);
  BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + dim * (d2 + 1)));

  double *result, *resVu, *resVv;
  result = dc.poles;
  resVu  = result + (ufirst ? dim * (d2 + 1) : dim);
  resVv  = result + (ufirst ? dim : dim * (d2 + 1));

  N.SetX(result[0]);
  Nu.SetX(resVu[0]);
  Nv.SetX(resVv[0]);

  N.SetY(result[1]);
  Nu.SetY(resVu[1]);
  Nv.SetY(resVv[1]);

  N.SetZ(result[2]);
  Nu.SetZ(resVu[2]);
  Nv.SetZ(resVv[2]);

  if (rational)
  {
    D  = result[3];
    Du = resVu[3];
    Dv = resVv[3];
  }
}

//=================================================================================================

void BSplSLib::D2(const double                      U,
                  const double                      V,
                  const int                         UIndex,
                  const int                         VIndex,
                  const NCollection_Array2<gp_Pnt>& Poles,
                  const NCollection_Array2<double>* Weights,
                  const NCollection_Array1<double>& UKnots,
                  const NCollection_Array1<double>& VKnots,
                  const NCollection_Array1<int>*    UMults,
                  const NCollection_Array1<int>*    VMults,
                  const int                         UDegree,
                  const int                         VDegree,
                  const bool                        URat,
                  const bool                        VRat,
                  const bool                        UPer,
                  const bool                        VPer,
                  gp_Pnt&                           P,
                  gp_Vec&                           Vu,
                  gp_Vec&                           Vv,
                  gp_Vec&                           Vuu,
                  gp_Vec&                           Vvv,
                  gp_Vec&                           Vuv)
{
  bool rational;
  //  int k,dim,dim2;
  int           dim, dim2;
  double        u1, u2;
  int           d1, d2;
  double*       result;
  const double *resVu, *resVv, *resVuu, *resVvv, *resVuv;
  validateBSplineDegree(UDegree, VDegree);
  BSplSLib_DataContainer dc;
  if (PrepareEval(U,
                  V,
                  UIndex,
                  VIndex,
                  UDegree,
                  VDegree,
                  URat,
                  VRat,
                  UPer,
                  VPer,
                  Poles,
                  Weights,
                  UKnots,
                  VKnots,
                  UMults,
                  VMults,
                  u1,
                  u2,
                  d1,
                  d2,
                  rational,
                  dc))
  {
    if (rational)
    {
      dim  = 4;
      dim2 = (d2 + 1) << 2;
      BSplCLib::Bohm(u1, d1, 2, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 2, *dc.knots2, dim, *dc.poles);
      BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *(dc.poles + dim2));
      if (d1 > 1)
        BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + (dim2 << 1)));
      BSplSLib::RationalDerivative(d1, d2, 2, 2, *dc.poles, *dc.ders);
      result = dc.ders;
      resVu  = result + 9;
      resVv  = result + 3;
      resVuu = result + 18;
      resVvv = result + 6;
      resVuv = result + 12;
    }
    else
    {
      dim  = 3;
      dim2 = d2 + 1;
      dim2 = (dim2 << 1) + dim2;
      BSplCLib::Bohm(u1, d1, 2, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 2, *dc.knots2, dim, *dc.poles);
      BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *(dc.poles + dim2));
      if (d1 > 1)
        BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + (dim2 << 1)));
      result = dc.poles;
      resVu  = result + dim2;
      resVv  = result + 3;
      if (UDegree <= 1)
        resVuu = BSplSLib_zero;
      else
        resVuu = result + (dim2 << 1);
      if (VDegree <= 1)
        resVvv = BSplSLib_zero;
      else
        resVvv = result + 6;
      resVuv = result + (d2 << 1) + d2 + 6;
    }
  }
  else
  {
    if (rational)
    {
      dim  = 4;
      dim2 = (d2 + 1) << 2;
      BSplCLib::Bohm(u1, d1, 2, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 2, *dc.knots2, dim, *dc.poles);
      BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *(dc.poles + dim2));
      if (d1 > 1)
        BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + (dim2 << 1)));
      BSplSLib::RationalDerivative(d1, d2, 2, 2, *dc.poles, *dc.ders);
      result = dc.ders;
      resVu  = result + 3;
      resVv  = result + 9;
      resVuu = result + 6;
      resVvv = result + 18;
      resVuv = result + 12;
    }
    else
    {
      dim  = 3;
      dim2 = d2 + 1;
      dim2 = (dim2 << 1) + dim2;
      BSplCLib::Bohm(u1, d1, 2, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 2, *dc.knots2, dim, *dc.poles);
      BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *(dc.poles + dim2));
      if (d1 > 1)
        BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + (dim2 << 1)));
      result = dc.poles;
      resVu  = result + 3;
      resVv  = result + dim2;
      if (UDegree <= 1)
        resVuu = BSplSLib_zero;
      else
        resVuu = result + 6;
      if (VDegree <= 1)
        resVvv = BSplSLib_zero;
      else
        resVvv = result + (dim2 << 1);
      resVuv = result + (d2 << 1) + d2 + 6;
    }
  }

  P.SetX(result[0]);
  Vu.SetX(resVu[0]);
  Vv.SetX(resVv[0]);
  Vuu.SetX(resVuu[0]);
  Vvv.SetX(resVvv[0]);
  Vuv.SetX(resVuv[0]);

  P.SetY(result[1]);
  Vu.SetY(resVu[1]);
  Vv.SetY(resVv[1]);
  Vuu.SetY(resVuu[1]);
  Vvv.SetY(resVvv[1]);
  Vuv.SetY(resVuv[1]);

  P.SetZ(result[2]);
  Vu.SetZ(resVu[2]);
  Vv.SetZ(resVv[2]);
  Vuu.SetZ(resVuu[2]);
  Vvv.SetZ(resVvv[2]);
  Vuv.SetZ(resVuv[2]);
}

//=================================================================================================

void BSplSLib::D3(const double                      U,
                  const double                      V,
                  const int                         UIndex,
                  const int                         VIndex,
                  const NCollection_Array2<gp_Pnt>& Poles,
                  const NCollection_Array2<double>* Weights,
                  const NCollection_Array1<double>& UKnots,
                  const NCollection_Array1<double>& VKnots,
                  const NCollection_Array1<int>*    UMults,
                  const NCollection_Array1<int>*    VMults,
                  const int                         UDegree,
                  const int                         VDegree,
                  const bool                        URat,
                  const bool                        VRat,
                  const bool                        UPer,
                  const bool                        VPer,
                  gp_Pnt&                           P,
                  gp_Vec&                           Vu,
                  gp_Vec&                           Vv,
                  gp_Vec&                           Vuu,
                  gp_Vec&                           Vvv,
                  gp_Vec&                           Vuv,
                  gp_Vec&                           Vuuu,
                  gp_Vec&                           Vvvv,
                  gp_Vec&                           Vuuv,
                  gp_Vec&                           Vuvv)
{
  bool rational;
  //  int k,dim,dim2;
  int           dim, dim2;
  double        u1, u2;
  int           d1, d2;
  double*       result;
  const double *resVu, *resVv, *resVuu, *resVvv, *resVuv, *resVuuu, *resVvvv, *resVuuv, *resVuvv;
  validateBSplineDegree(UDegree, VDegree);
  BSplSLib_DataContainer dc;
  if (PrepareEval(U,
                  V,
                  UIndex,
                  VIndex,
                  UDegree,
                  VDegree,
                  URat,
                  VRat,
                  UPer,
                  VPer,
                  Poles,
                  Weights,
                  UKnots,
                  VKnots,
                  UMults,
                  VMults,
                  u1,
                  u2,
                  d1,
                  d2,
                  rational,
                  dc))
  {
    if (rational)
    {
      dim  = 4;
      dim2 = (d2 + 1) << 2;
      BSplCLib::Bohm(u1, d1, 3, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 3, *dc.knots2, dim, *dc.poles);
      BSplCLib::Bohm(u2, d2, 2, *dc.knots2, dim, *(dc.poles + dim2));
      if (d1 > 1)
        BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *(dc.poles + (dim2 << 1)));
      if (d1 > 2)
        BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + (dim2 << 1) + dim2));
      BSplSLib::RationalDerivative(d1, d2, 3, 3, *dc.poles, *dc.ders);
      result  = dc.ders;
      resVu   = result + 12;
      resVv   = result + 3;
      resVuu  = result + 24;
      resVvv  = result + 6;
      resVuv  = result + 15;
      resVuuu = result + 36;
      resVvvv = result + 9;
      resVuuv = result + 27;
      resVuvv = result + 18;
    }
    else
    {
      dim  = 3;
      dim2 = (d2 + 1);
      dim2 = (dim2 << 1) + dim2;
      BSplCLib::Bohm(u1, d1, 3, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 3, *dc.knots2, dim, *dc.poles);
      BSplCLib::Bohm(u2, d2, 2, *dc.knots2, dim, *(dc.poles + dim2));
      if (d1 > 1)
        BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *(dc.poles + (dim2 << 1)));
      if (d1 > 2)
        BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + (dim2 << 1) + dim2));
      result = dc.poles;
      resVu  = result + dim2;
      resVv  = result + 3;
      if (UDegree <= 1)
      {
        resVuu  = BSplSLib_zero;
        resVuuv = BSplSLib_zero;
      }
      else
      {
        resVuu  = result + (dim2 << 1);
        resVuuv = result + (dim2 << 1) + 3;
      }
      if (VDegree <= 1)
      {
        resVvv  = BSplSLib_zero;
        resVuvv = BSplSLib_zero;
      }
      else
      {
        resVvv  = result + 6;
        resVuvv = result + dim2 + 6;
      }
      resVuv = result + (d2 << 1) + d2 + 6;
      if (UDegree <= 2)
        resVuuu = BSplSLib_zero;
      else
        resVuuu = result + (dim2 << 1) + dim2;
      if (VDegree <= 2)
        resVvvv = BSplSLib_zero;
      else
        resVvvv = result + 9;
    }
  }
  else
  {
    if (rational)
    {
      dim  = 4;
      dim2 = (d2 + 1) << 2;
      BSplCLib::Bohm(u1, d1, 3, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 3, *dc.knots2, dim, *dc.poles);
      BSplCLib::Bohm(u2, d2, 2, *dc.knots2, dim, *(dc.poles + dim2));
      if (d1 > 1)
        BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *(dc.poles + (dim2 << 1)));
      if (d1 > 2)
        BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + (dim2 << 1) + dim2));
      BSplSLib::RationalDerivative(d1, d2, 3, 3, *dc.poles, *dc.ders);
      result  = dc.ders;
      resVu   = result + 3;
      resVv   = result + 12;
      resVuu  = result + 6;
      resVvv  = result + 24;
      resVuv  = result + 15;
      resVuuu = result + 9;
      resVvvv = result + 36;
      resVuuv = result + 18;
      resVuvv = result + 27;
    }
    else
    {
      dim  = 3;
      dim2 = (d2 + 1);
      dim2 = (dim2 << 1) + dim2;
      BSplCLib::Bohm(u1, d1, 3, *dc.knots1, dim2, *dc.poles);
      BSplCLib::Bohm(u2, d2, 3, *dc.knots2, dim, *dc.poles);
      BSplCLib::Bohm(u2, d2, 2, *dc.knots2, dim, *(dc.poles + dim2));
      if (d1 > 1)
        BSplCLib::Bohm(u2, d2, 1, *dc.knots2, dim, *(dc.poles + (dim2 << 1)));
      if (d1 > 2)
        BSplCLib::Eval(u2, d2, *dc.knots2, dim, *(dc.poles + (dim2 << 1) + dim2));
      result = dc.poles;
      resVu  = result + 3;
      resVv  = result + dim2;
      if (UDegree <= 1)
      {
        resVuu  = BSplSLib_zero;
        resVuuv = BSplSLib_zero;
      }
      else
      {
        resVuu  = result + 6;
        resVuuv = result + dim2 + 6;
      }
      if (VDegree <= 1)
      {
        resVvv  = BSplSLib_zero;
        resVuvv = BSplSLib_zero;
      }
      else
      {
        resVvv  = result + (dim2 << 1);
        resVuvv = result + (dim2 << 1) + 3;
      }
      resVuv = result + (d2 << 1) + d2 + 6;
      if (UDegree <= 2)
        resVuuu = BSplSLib_zero;
      else
        resVuuu = result + 9;
      if (VDegree <= 2)
        resVvvv = BSplSLib_zero;
      else
        resVvvv = result + (dim2 << 1) + dim2;
    }
  }

  P.SetX(result[0]);
  Vu.SetX(resVu[0]);
  Vv.SetX(resVv[0]);
  Vuu.SetX(resVuu[0]);
  Vvv.SetX(resVvv[0]);
  Vuv.SetX(resVuv[0]);
  Vuuu.SetX(resVuuu[0]);
  Vvvv.SetX(resVvvv[0]);
  Vuuv.SetX(resVuuv[0]);
  Vuvv.SetX(resVuvv[0]);

  P.SetY(result[1]);
  Vu.SetY(resVu[1]);
  Vv.SetY(resVv[1]);
  Vuu.SetY(resVuu[1]);
  Vvv.SetY(resVvv[1]);
  Vuv.SetY(resVuv[1]);
  Vuuu.SetY(resVuuu[1]);
  Vvvv.SetY(resVvvv[1]);
  Vuuv.SetY(resVuuv[1]);
  Vuvv.SetY(resVuvv[1]);

  P.SetZ(result[2]);
  Vu.SetZ(resVu[2]);
  Vv.SetZ(resVv[2]);
  Vuu.SetZ(resVuu[2]);
  Vvv.SetZ(resVvv[2]);
  Vuv.SetZ(resVuv[2]);
  Vuuu.SetZ(resVuuu[2]);
  Vvvv.SetZ(resVvvv[2]);
  Vuuv.SetZ(resVuuv[2]);
  Vuvv.SetZ(resVuvv[2]);
}

//=================================================================================================

void BSplSLib::DN(const double                      U,
                  const double                      V,
                  const int                         Nu,
                  const int                         Nv,
                  const int                         UIndex,
                  const int                         VIndex,
                  const NCollection_Array2<gp_Pnt>& Poles,
                  const NCollection_Array2<double>* Weights,
                  const NCollection_Array1<double>& UKnots,
                  const NCollection_Array1<double>& VKnots,
                  const NCollection_Array1<int>*    UMults,
                  const NCollection_Array1<int>*    VMults,
                  const int                         UDegree,
                  const int                         VDegree,
                  const bool                        URat,
                  const bool                        VRat,
                  const bool                        UPer,
                  const bool                        VPer,
                  gp_Vec&                           Vn)
{
  bool   rational;
  int    k, dim;
  double u1, u2;
  int    d1, d2;

  validateBSplineDegree(UDegree, VDegree);
  BSplSLib_DataContainer dc;
  bool                   ufirst = PrepareEval(U,
                            V,
                            UIndex,
                            VIndex,
                            UDegree,
                            VDegree,
                            URat,
                            VRat,
                            UPer,
                            VPer,
                            Poles,
                            Weights,
                            UKnots,
                            VKnots,
                            UMults,
                            VMults,
                            u1,
                            u2,
                            d1,
                            d2,
                            rational,
                            dc);
  dim                           = rational ? 4 : 3;

  if (!rational)
  {
    if ((Nu > UDegree) || (Nv > VDegree))
    {
      Vn.SetX(0.);
      Vn.SetY(0.);
      Vn.SetZ(0.);
      return;
    }
  }

  int n1 = ufirst ? Nu : Nv;
  int n2 = ufirst ? Nv : Nu;

  BSplCLib::Bohm(u1, d1, n1, *dc.knots1, dim * (d2 + 1), *dc.poles);

  for (k = 0; k <= std::min(n1, d1); k++)
    BSplCLib::Bohm(u2, d2, n2, *dc.knots2, dim, *(dc.poles + k * dim * (d2 + 1)));

  double* result;
  if (rational)
  {
    BSplSLib::RationalDerivative(d1, d2, n1, n2, *dc.poles, *dc.ders, false);
    result = dc.ders; // because false ci-dessus.
  }
  else
  {
    result = dc.poles + (n1 * (d2 + 1) + n2) * dim;
  }

  Vn.SetX(result[0]);
  Vn.SetY(result[1]);
  Vn.SetZ(result[2]);
}

//
// Surface modifications
//
// a surface is processed as a curve of curves.
// i.e : a curve of parameter u where the current point is the set of poles
//       of the iso.
//

//=================================================================================================

void BSplSLib::Iso(const double                      Param,
                   const bool                        IsU,
                   const NCollection_Array2<gp_Pnt>& Poles,
                   const NCollection_Array2<double>* Weights,
                   const NCollection_Array1<double>& Knots,
                   const NCollection_Array1<int>*    Mults,
                   const int                         Degree,
                   const bool                        Periodic,
                   NCollection_Array1<gp_Pnt>&       CPoles,
                   NCollection_Array1<double>*       CWeights)
{
  int    index    = 0;
  double u        = Param;
  bool   rational = Weights != nullptr;
  int    dim      = rational ? 4 : 3;

  // compute local knots

  NCollection_LocalArray<double> locknots1(2 * Degree);
  BSplCLib::LocateParameter(Degree, Knots, Mults, u, Periodic, index, u);
  BSplCLib::BuildKnots(Degree, index, Periodic, Knots, Mults, *locknots1);
  if (Mults == nullptr)
    index -= Knots.Lower() + Degree;
  else
    index = BSplCLib::PoleIndex(Degree, index, Periodic, *Mults);

  // copy the local poles

  //  int f1,l1,f2,l2,i,j,k;
  int f1, l1, f2, l2, i, j;

  if (IsU)
  {
    f1 = Poles.LowerRow();
    l1 = Poles.UpperRow();
    f2 = Poles.LowerCol();
    l2 = Poles.UpperCol();
  }
  else
  {
    f1 = Poles.LowerCol();
    l1 = Poles.UpperCol();
    f2 = Poles.LowerRow();
    l2 = Poles.UpperRow();
  }

  NCollection_LocalArray<double> locpoles((Degree + 1) * (l2 - f2 + 1) * dim);

  double w, *pole = locpoles;
  index += f1;

  for (i = 0; i <= Degree; i++)
  {

    for (j = f2; j <= l2; j++)
    {

      const gp_Pnt& P = IsU ? Poles(index, j) : Poles(j, index);
      if (rational)
      {
        pole[3] = w = IsU ? (*Weights)(index, j) : (*Weights)(j, index);
        pole[0]     = P.X() * w;
        pole[1]     = P.Y() * w;
        pole[2]     = P.Z() * w;
      }
      else
      {
        pole[0] = P.X();
        pole[1] = P.Y();
        pole[2] = P.Z();
      }
      pole += dim;
    }
    index++;
    if (index > l1)
      index = f1;
  }

  // compute the iso
  BSplCLib::Eval(u, Degree, *locknots1, (l2 - f2 + 1) * dim, *locpoles);

  // get the result
  pole = locpoles;

  for (i = CPoles.Lower(); i <= CPoles.Upper(); i++)
  {
    gp_Pnt& P = CPoles(i);
    if (rational)
    {
      (*CWeights)(i) = w = pole[3];
      P.SetX(pole[0] / w);
      P.SetY(pole[1] / w);
      P.SetZ(pole[2] / w);
    }
    else
    {
      P.SetX(pole[0]);
      P.SetY(pole[1]);
      P.SetZ(pole[2]);
    }
    pole += dim;
  }

  // if the input is not rational but weights are wanted
  if (!rational && (CWeights != nullptr))
  {

    for (i = CWeights->Lower(); i <= CWeights->Upper(); i++)
      (*CWeights)(i) = 1.;
  }
}

//=================================================================================================

void BSplSLib::Reverse(NCollection_Array2<gp_Pnt>& Poles, const int Last, const bool UDirection)
{
  int i, j, l = Last;
  if (UDirection)
  {
    l = Poles.LowerRow() + (l - Poles.LowerRow()) % (Poles.ColLength());
    NCollection_Array2<gp_Pnt> temp(0, Poles.ColLength() - 1, Poles.LowerCol(), Poles.UpperCol());

    for (i = Poles.LowerRow(); i <= l; i++)
    {

      for (j = Poles.LowerCol(); j <= Poles.UpperCol(); j++)
      {
        temp(l - i, j) = Poles(i, j);
      }
    }

    for (i = l + 1; i <= Poles.UpperRow(); i++)
    {

      for (j = Poles.LowerCol(); j <= Poles.UpperCol(); j++)
      {
        temp(l + Poles.ColLength() - i, j) = Poles(i, j);
      }
    }

    for (i = Poles.LowerRow(); i <= Poles.UpperRow(); i++)
    {

      for (j = Poles.LowerCol(); j <= Poles.UpperCol(); j++)
      {
        Poles(i, j) = temp(i - Poles.LowerRow(), j);
      }
    }
  }
  else
  {
    l = Poles.LowerCol() + (l - Poles.LowerCol()) % (Poles.RowLength());
    NCollection_Array2<gp_Pnt> temp(Poles.LowerRow(), Poles.UpperRow(), 0, Poles.RowLength() - 1);

    for (j = Poles.LowerCol(); j <= l; j++)
    {

      for (i = Poles.LowerRow(); i <= Poles.UpperRow(); i++)
      {
        temp(i, l - j) = Poles(i, j);
      }
    }

    for (j = l + 1; j <= Poles.UpperCol(); j++)
    {

      for (i = Poles.LowerRow(); i <= Poles.UpperRow(); i++)
      {
        temp(i, l + Poles.RowLength() - j) = Poles(i, j);
      }
    }

    for (i = Poles.LowerRow(); i <= Poles.UpperRow(); i++)
    {

      for (j = Poles.LowerCol(); j <= Poles.UpperCol(); j++)
      {
        Poles(i, j) = temp(i, j - Poles.LowerCol());
      }
    }
  }
}

//=================================================================================================

void BSplSLib::Reverse(NCollection_Array2<double>& Weights, const int Last, const bool UDirection)
{
  int i, j, l = Last;
  if (UDirection)
  {
    l = Weights.LowerRow() + (l - Weights.LowerRow()) % (Weights.ColLength());
    NCollection_Array2<double> temp(0,
                                    Weights.ColLength() - 1,
                                    Weights.LowerCol(),
                                    Weights.UpperCol());

    for (i = Weights.LowerRow(); i <= l; i++)
    {

      for (j = Weights.LowerCol(); j <= Weights.UpperCol(); j++)
      {
        temp(l - i, j) = Weights(i, j);
      }
    }

    for (i = l + 1; i <= Weights.UpperRow(); i++)
    {

      for (j = Weights.LowerCol(); j <= Weights.UpperCol(); j++)
      {
        temp(l + Weights.ColLength() - i, j) = Weights(i, j);
      }
    }

    for (i = Weights.LowerRow(); i <= Weights.UpperRow(); i++)
    {

      for (j = Weights.LowerCol(); j <= Weights.UpperCol(); j++)
      {
        Weights(i, j) = temp(i - Weights.LowerRow(), j);
      }
    }
  }
  else
  {
    l = Weights.LowerCol() + (l - Weights.LowerCol()) % (Weights.RowLength());
    NCollection_Array2<double> temp(Weights.LowerRow(),
                                    Weights.UpperRow(),
                                    0,
                                    Weights.RowLength() - 1);

    for (j = Weights.LowerCol(); j <= l; j++)
    {

      for (i = Weights.LowerRow(); i <= Weights.UpperRow(); i++)
      {
        temp(i, l - j) = Weights(i, j);
      }
    }

    for (j = l + 1; j <= Weights.UpperCol(); j++)
    {

      for (i = Weights.LowerRow(); i <= Weights.UpperRow(); i++)
      {
        temp(i, l + Weights.RowLength() - j) = Weights(i, j);
      }
    }

    for (i = Weights.LowerRow(); i <= Weights.UpperRow(); i++)
    {

      for (j = Weights.LowerCol(); j <= Weights.UpperCol(); j++)
      {
        Weights(i, j) = temp(i, j - Weights.LowerCol());
      }
    }
  }
}

//=================================================================================================

bool BSplSLib::IsRational(const NCollection_Array2<double>& Weights,
                          const int                         I1,
                          const int                         I2,
                          const int                         J1,
                          const int                         J2,
                          const double                      Epsi)
{
  double eps = (Epsi > 0.0) ? Epsi : Epsilon(Weights(I1, I2));
  int    i, j;
  int    fi = Weights.LowerRow(), li = Weights.ColLength();
  int    fj = Weights.LowerCol(), lj = Weights.RowLength();

  for (i = I1 - fi; i < I2 - fi; i++)
  {

    for (j = J1 - fj; j < J2 - fj; j++)
    {
      if (std::abs(Weights(fi + i % li, fj + j % lj) - Weights(fi + (i + 1) % li, fj + j % lj))
          > eps)
        return true;
    }
  }
  return false;
}

//=================================================================================================

void BSplSLib::SetPoles(const NCollection_Array2<gp_Pnt>& Poles,
                        NCollection_Array1<double>&       FP,
                        const bool                        UDirection)
{
  int i, j, l = FP.Lower();
  int PLowerRow = Poles.LowerRow();
  int PUpperRow = Poles.UpperRow();
  int PLowerCol = Poles.LowerCol();
  int PUpperCol = Poles.UpperCol();
  if (UDirection)
  {

    for (i = PLowerRow; i <= PUpperRow; i++)
    {

      for (j = PLowerCol; j <= PUpperCol; j++)
      {
        const gp_Pnt& P = Poles.Value(i, j);
        FP(l)           = P.X();
        l++;
        FP(l) = P.Y();
        l++;
        FP(l) = P.Z();
        l++;
      }
    }
  }
  else
  {

    for (j = PLowerCol; j <= PUpperCol; j++)
    {

      for (i = PLowerRow; i <= PUpperRow; i++)
      {
        const gp_Pnt& P = Poles.Value(i, j);
        FP(l)           = P.X();
        l++;
        FP(l) = P.Y();
        l++;
        FP(l) = P.Z();
        l++;
      }
    }
  }
}

//=================================================================================================

void BSplSLib::SetPoles(const NCollection_Array2<gp_Pnt>& Poles,
                        const NCollection_Array2<double>& Weights,
                        NCollection_Array1<double>&       FP,
                        const bool                        UDirection)
{
  int i, j, l = FP.Lower();
  int PLowerRow = Poles.LowerRow();
  int PUpperRow = Poles.UpperRow();
  int PLowerCol = Poles.LowerCol();
  int PUpperCol = Poles.UpperCol();
  if (UDirection)
  {

    for (i = PLowerRow; i <= PUpperRow; i++)
    {

      for (j = PLowerCol; j <= PUpperCol; j++)
      {
        const gp_Pnt& P = Poles.Value(i, j);
        double        w = Weights.Value(i, j);
        FP(l)           = P.X() * w;
        l++;
        FP(l) = P.Y() * w;
        l++;
        FP(l) = P.Z() * w;
        l++;
        FP(l) = w;
        l++;
      }
    }
  }
  else
  {

    for (j = PLowerCol; j <= PUpperCol; j++)
    {

      for (i = PLowerRow; i <= PUpperRow; i++)
      {
        const gp_Pnt& P = Poles.Value(i, j);
        double        w = Weights.Value(i, j);
        FP(l)           = P.X() * w;
        l++;
        FP(l) = P.Y() * w;
        l++;
        FP(l) = P.Z() * w;
        l++;
        FP(l) = w;
        l++;
      }
    }
  }
}

//=================================================================================================

void BSplSLib::GetPoles(const NCollection_Array1<double>& FP,
                        NCollection_Array2<gp_Pnt>&       Poles,
                        const bool                        UDirection)
{
  int i, j, l = FP.Lower();
  int PLowerRow = Poles.LowerRow();
  int PUpperRow = Poles.UpperRow();
  int PLowerCol = Poles.LowerCol();
  int PUpperCol = Poles.UpperCol();
  if (UDirection)
  {

    for (i = PLowerRow; i <= PUpperRow; i++)
    {

      for (j = PLowerCol; j <= PUpperCol; j++)
      {
        gp_Pnt& P = Poles.ChangeValue(i, j);
        P.SetX(FP(l));
        l++;
        P.SetY(FP(l));
        l++;
        P.SetZ(FP(l));
        l++;
      }
    }
  }
  else
  {

    for (j = PLowerCol; j <= PUpperCol; j++)
    {

      for (i = PLowerRow; i <= PUpperRow; i++)
      {
        gp_Pnt& P = Poles.ChangeValue(i, j);
        P.SetX(FP(l));
        l++;
        P.SetY(FP(l));
        l++;
        P.SetZ(FP(l));
        l++;
      }
    }
  }
}

//=================================================================================================

void BSplSLib::GetPoles(const NCollection_Array1<double>& FP,
                        NCollection_Array2<gp_Pnt>&       Poles,
                        NCollection_Array2<double>&       Weights,
                        const bool                        UDirection)
{
  int i, j, l = FP.Lower();
  int PLowerRow = Poles.LowerRow();
  int PUpperRow = Poles.UpperRow();
  int PLowerCol = Poles.LowerCol();
  int PUpperCol = Poles.UpperCol();
  if (UDirection)
  {

    for (i = PLowerRow; i <= PUpperRow; i++)
    {

      for (j = PLowerCol; j <= PUpperCol; j++)
      {
        double w      = FP(l + 3);
        Weights(i, j) = w;
        gp_Pnt& P     = Poles.ChangeValue(i, j);
        P.SetX(FP(l) / w);
        l++;
        P.SetY(FP(l) / w);
        l++;
        P.SetZ(FP(l) / w);
        l++;
        l++;
      }
    }
  }
  else
  {

    for (j = PLowerCol; j <= PUpperCol; j++)
    {

      for (i = PLowerRow; i <= PUpperRow; i++)
      {
        double w      = FP(l + 3);
        Weights(i, j) = w;
        gp_Pnt& P     = Poles.ChangeValue(i, j);
        P.SetX(FP(l) / w);
        l++;
        P.SetY(FP(l) / w);
        l++;
        P.SetZ(FP(l) / w);
        l++;
        l++;
      }
    }
  }
}

//=================================================================================================

void BSplSLib::InsertKnots(const bool                        UDirection,
                           const int                         Degree,
                           const bool                        Periodic,
                           const NCollection_Array2<gp_Pnt>& Poles,
                           const NCollection_Array2<double>* Weights,
                           const NCollection_Array1<double>& Knots,
                           const NCollection_Array1<int>&    Mults,
                           const NCollection_Array1<double>& AddKnots,
                           const NCollection_Array1<int>*    AddMults,
                           NCollection_Array2<gp_Pnt>&       NewPoles,
                           NCollection_Array2<double>*       NewWeights,
                           NCollection_Array1<double>&       NewKnots,
                           NCollection_Array1<int>&          NewMults,
                           const double                      Epsilon,
                           const bool                        Add)
{
  bool rational = Weights != nullptr;
  int  dim      = 3;
  if (rational)
    dim++;

  NCollection_Array1<double> poles(1, dim * Poles.RowLength() * Poles.ColLength());
  NCollection_Array1<double> newpoles(1, dim * NewPoles.RowLength() * NewPoles.ColLength());

  if (rational)
    SetPoles(Poles, *Weights, poles, UDirection);
  else
    SetPoles(Poles, poles, UDirection);

  if (UDirection)
  {
    dim *= Poles.RowLength();
  }
  else
  {
    dim *= Poles.ColLength();
  }
  BSplCLib::InsertKnots(Degree,
                        Periodic,
                        dim,
                        poles,
                        Knots,
                        Mults,
                        AddKnots,
                        AddMults,
                        newpoles,
                        NewKnots,
                        NewMults,
                        Epsilon,
                        Add);

  if (rational)
    GetPoles(newpoles, NewPoles, *NewWeights, UDirection);
  else
    GetPoles(newpoles, NewPoles, UDirection);
}

//=================================================================================================

bool BSplSLib::RemoveKnot(const bool                        UDirection,
                          const int                         Index,
                          const int                         Mult,
                          const int                         Degree,
                          const bool                        Periodic,
                          const NCollection_Array2<gp_Pnt>& Poles,
                          const NCollection_Array2<double>* Weights,
                          const NCollection_Array1<double>& Knots,
                          const NCollection_Array1<int>&    Mults,
                          NCollection_Array2<gp_Pnt>&       NewPoles,
                          NCollection_Array2<double>*       NewWeights,
                          NCollection_Array1<double>&       NewKnots,
                          NCollection_Array1<int>&          NewMults,
                          const double                      Tolerance)
{
  bool rational = Weights != nullptr;
  int  dim      = 3;
  if (rational)
    dim++;

  NCollection_Array1<double> poles(1, dim * Poles.RowLength() * Poles.ColLength());
  NCollection_Array1<double> newpoles(1, dim * NewPoles.RowLength() * NewPoles.ColLength());

  if (rational)
    SetPoles(Poles, *Weights, poles, UDirection);
  else
    SetPoles(Poles, poles, UDirection);

  if (UDirection)
  {
    dim *= Poles.RowLength();
  }
  else
  {
    dim *= Poles.ColLength();
  }

  if (!BSplCLib::RemoveKnot(Index,
                            Mult,
                            Degree,
                            Periodic,
                            dim,
                            poles,
                            Knots,
                            Mults,
                            newpoles,
                            NewKnots,
                            NewMults,
                            Tolerance))
    return false;

  if (rational)
    GetPoles(newpoles, NewPoles, *NewWeights, UDirection);
  else
    GetPoles(newpoles, NewPoles, UDirection);
  return true;
}

//=================================================================================================

void BSplSLib::IncreaseDegree(const bool                        UDirection,
                              const int                         Degree,
                              const int                         NewDegree,
                              const bool                        Periodic,
                              const NCollection_Array2<gp_Pnt>& Poles,
                              const NCollection_Array2<double>* Weights,
                              const NCollection_Array1<double>& Knots,
                              const NCollection_Array1<int>&    Mults,
                              NCollection_Array2<gp_Pnt>&       NewPoles,
                              NCollection_Array2<double>*       NewWeights,
                              NCollection_Array1<double>&       NewKnots,
                              NCollection_Array1<int>&          NewMults)
{
  bool rational = Weights != nullptr;
  int  dim      = 3;
  if (rational)
    dim++;

  NCollection_Array1<double> poles(1, dim * Poles.RowLength() * Poles.ColLength());
  NCollection_Array1<double> newpoles(1, dim * NewPoles.RowLength() * NewPoles.ColLength());

  if (rational)
    SetPoles(Poles, *Weights, poles, UDirection);
  else
    SetPoles(Poles, poles, UDirection);

  if (UDirection)
  {
    dim *= Poles.RowLength();
  }
  else
  {
    dim *= Poles.ColLength();
  }

  BSplCLib::IncreaseDegree(Degree,
                           NewDegree,
                           Periodic,
                           dim,
                           poles,
                           Knots,
                           Mults,
                           newpoles,
                           NewKnots,
                           NewMults);

  if (rational)
    GetPoles(newpoles, NewPoles, *NewWeights, UDirection);
  else
    GetPoles(newpoles, NewPoles, UDirection);
}

//=================================================================================================

void BSplSLib::Unperiodize(const bool                        UDirection,
                           const int                         Degree,
                           const NCollection_Array1<int>&    Mults,
                           const NCollection_Array1<double>& Knots,
                           const NCollection_Array2<gp_Pnt>& Poles,
                           const NCollection_Array2<double>* Weights,
                           NCollection_Array1<int>&          NewMults,
                           NCollection_Array1<double>&       NewKnots,
                           NCollection_Array2<gp_Pnt>&       NewPoles,
                           NCollection_Array2<double>*       NewWeights)
{
  bool rational = Weights != nullptr;
  int  dim      = 3;
  if (rational)
    dim++;

  NCollection_Array1<double> poles(1, dim * Poles.RowLength() * Poles.ColLength());
  NCollection_Array1<double> newpoles(1, dim * NewPoles.RowLength() * NewPoles.ColLength());

  if (rational)
    SetPoles(Poles, *Weights, poles, UDirection);
  else
    SetPoles(Poles, poles, UDirection);

  if (UDirection)
  {
    dim *= Poles.RowLength();
  }
  else
  {
    dim *= Poles.ColLength();
  }

  BSplCLib::Unperiodize(Degree, dim, Mults, Knots, poles, NewMults, NewKnots, newpoles);

  if (rational)
    GetPoles(newpoles, NewPoles, *NewWeights, UDirection);
  else
    GetPoles(newpoles, NewPoles, UDirection);
}

//=======================================================================
// function : BuildCache
// purpose  : Stores theTaylor expansion normalized between 0,1 in the
//           Cache : in case of  a rational function the Poles are
//           stored in homogeneous form
//=======================================================================

void BSplSLib::BuildCache(const double                      U,
                          const double                      V,
                          const double                      USpanDomain,
                          const double                      VSpanDomain,
                          const bool                        UPeriodic,
                          const bool                        VPeriodic,
                          const int                         UDegree,
                          const int                         VDegree,
                          const int                         UIndex,
                          const int                         VIndex,
                          const NCollection_Array1<double>& UFlatKnots,
                          const NCollection_Array1<double>& VFlatKnots,
                          const NCollection_Array2<gp_Pnt>& Poles,
                          const NCollection_Array2<double>* Weights,
                          NCollection_Array2<gp_Pnt>&       CachePoles,
                          NCollection_Array2<double>*       CacheWeights)
{
  bool   rational, rational_u, rational_v, flag_u_or_v;
  int    kk, d1, d1p1, d2, d2p1, ii, jj, iii, jjj, Index;
  double u1, min_degree_domain, max_degree_domain, f, factor[2], u2;
  if (Weights != nullptr)
    rational_u = rational_v = true;
  else
    rational_u = rational_v = false;
  validateBSplineDegree(UDegree, VDegree);
  BSplSLib_DataContainer dc;
  flag_u_or_v = PrepareEval(U,
                            V,
                            UIndex,
                            VIndex,
                            UDegree,
                            VDegree,
                            rational_u,
                            rational_v,
                            UPeriodic,
                            VPeriodic,
                            Poles,
                            Weights,
                            UFlatKnots,
                            VFlatKnots,
                            (BSplCLib::NoMults()),
                            (BSplCLib::NoMults()),
                            u1,
                            u2,
                            d1,
                            d2,
                            rational,
                            dc);
  d1p1        = d1 + 1;
  d2p1        = d2 + 1;
  if (rational)
  {
    BSplCLib::Bohm(u1, d1, d1, *dc.knots1, 4 * d2p1, *dc.poles);

    for (kk = 0; kk <= d1; kk++)
      BSplCLib::Bohm(u2, d2, d2, *dc.knots2, 4, *(dc.poles + kk * 4 * d2p1));
    if (flag_u_or_v)
    {
      min_degree_domain = USpanDomain;
      max_degree_domain = VSpanDomain;
    }
    else
    {
      min_degree_domain = VSpanDomain;
      max_degree_domain = USpanDomain;
    }
    factor[0] = 1.0e0;

    for (ii = 0; ii <= d2; ii++)
    {
      iii       = ii + 1;
      factor[1] = 1.0e0;

      for (jj = 0; jj <= d1; jj++)
      {
        jjj       = jj + 1;
        Index     = jj * d2p1 + ii;
        Index     = Index << 2;
        gp_Pnt& P = CachePoles(iii, jjj);
        f         = factor[0] * factor[1];
        P.SetX(f * dc.poles[Index]);
        Index++;
        P.SetY(f * dc.poles[Index]);
        Index++;
        P.SetZ(f * dc.poles[Index]);
        Index++;
        (*CacheWeights)(iii, jjj) = f * dc.poles[Index];
        factor[1] *= min_degree_domain / (double)(jjj);
      }
      factor[0] *= max_degree_domain / (double)(iii);
    }
  }
  else
  {
    BSplCLib::Bohm(u1, d1, d1, *dc.knots1, 3 * d2p1, *dc.poles);

    for (kk = 0; kk <= d1; kk++)
      BSplCLib::Bohm(u2, d2, d2, *dc.knots2, 3, *(dc.poles + kk * 3 * d2p1));
    if (flag_u_or_v)
    {
      min_degree_domain = USpanDomain;
      max_degree_domain = VSpanDomain;
    }
    else
    {
      min_degree_domain = VSpanDomain;
      max_degree_domain = USpanDomain;
    }
    factor[0] = 1.0e0;

    for (ii = 0; ii <= d2; ii++)
    {
      iii       = ii + 1;
      factor[1] = 1.0e0;

      for (jj = 0; jj <= d1; jj++)
      {
        jjj       = jj + 1;
        Index     = jj * d2p1 + ii;
        Index     = (Index << 1) + Index;
        gp_Pnt& P = CachePoles(iii, jjj);
        f         = factor[0] * factor[1];
        P.SetX(f * dc.poles[Index]);
        Index++;
        P.SetY(f * dc.poles[Index]);
        Index++;
        P.SetZ(f * dc.poles[Index]);
        factor[1] *= min_degree_domain / (double)(jjj);
      }
      factor[0] *= max_degree_domain / (double)(iii);
    }
    if (Weights != nullptr)
    {
      //
      // means that PrepareEval did found out that the surface was
      // locally polynomial but since the surface is constructed
      // with some weights we need to set the weight polynomial to constant
      //

      for (ii = 1; ii <= d2p1; ii++)
      {

        for (jj = 1; jj <= d1p1; jj++)
        {
          (*CacheWeights)(ii, jj) = 0.0e0;
        }
      }
      (*CacheWeights)(1, 1) = 1.0e0;
    }
  }
}

void BSplSLib::BuildCache(const double                      theU,
                          const double                      theV,
                          const double                      theUSpanDomain,
                          const double                      theVSpanDomain,
                          const bool                        theUPeriodicFlag,
                          const bool                        theVPeriodicFlag,
                          const int                         theUDegree,
                          const int                         theVDegree,
                          const int                         theUIndex,
                          const int                         theVIndex,
                          const NCollection_Array1<double>& theUFlatKnots,
                          const NCollection_Array1<double>& theVFlatKnots,
                          const NCollection_Array2<gp_Pnt>& thePoles,
                          const NCollection_Array2<double>* theWeights,
                          NCollection_Array2<double>&       theCacheArray)
{
  bool   flag_u_or_v;
  int    d1, d2;
  double u1, u2;
  bool   isRationalOnParam = (theWeights != nullptr);
  bool   isRational;

  validateBSplineDegree(theUDegree, theVDegree);
  BSplSLib_DataContainer dc;
  flag_u_or_v = PrepareEval(theU,
                            theV,
                            theUIndex,
                            theVIndex,
                            theUDegree,
                            theVDegree,
                            isRationalOnParam,
                            isRationalOnParam,
                            theUPeriodicFlag,
                            theVPeriodicFlag,
                            thePoles,
                            theWeights,
                            theUFlatKnots,
                            theVFlatKnots,
                            (BSplCLib::NoMults()),
                            (BSplCLib::NoMults()),
                            u1,
                            u2,
                            d1,
                            d2,
                            isRational,
                            dc);

  int d2p1        = d2 + 1;
  int aDimension  = isRational ? 4 : 3;
  int aCacheShift = // helps to store weights when PrepareEval did not found that the
                    // surface is locally polynomial
    (isRationalOnParam && !isRational) ? aDimension + 1 : aDimension;

  double aDomains[2];
  // aDomains[0] corresponds to variable with minimal degree
  // aDomains[1] corresponds to variable with maximal degree
  if (flag_u_or_v)
  {
    aDomains[0] = theUSpanDomain;
    aDomains[1] = theVSpanDomain;
  }
  else
  {
    aDomains[0] = theVSpanDomain;
    aDomains[1] = theUSpanDomain;
  }

  BSplCLib::Bohm(u1, d1, d1, *dc.knots1, aDimension * d2p1, *dc.poles);
  for (int kk = 0; kk <= d1; kk++)
    BSplCLib::Bohm(u2, d2, d2, *dc.knots2, aDimension, *(dc.poles + kk * aDimension * d2p1));

  double* aCache = (double*)&(theCacheArray(theCacheArray.LowerRow(), theCacheArray.LowerCol()));

  double aFactors[2];
  // aFactors[0] corresponds to variable with minimal degree
  // aFactors[1] corresponds to variable with maximal degree
  aFactors[1] = 1.0;
  int    aRow, aCol, i;
  double aCoeff;
  for (aRow = 0; aRow <= d2; aRow++)
  {
    aFactors[0] = 1.0;
    for (aCol = 0; aCol <= d1; aCol++)
    {
      double* aPolyCoeffs = dc.poles + (aCol * d2p1 + aRow) * aDimension;
      aCoeff              = aFactors[0] * aFactors[1];
      for (i = 0; i < aDimension; i++)
        aCache[i] = aPolyCoeffs[i] * aCoeff;
      aCache += aCacheShift;
      aFactors[0] *= aDomains[0] / (aCol + 1);
    }
    aFactors[1] *= aDomains[1] / (aRow + 1);
  }

  // Fill the weights for the surface which is not locally polynomial
  if (aCacheShift > aDimension)
  {
    aCache = (double*)&(theCacheArray(theCacheArray.LowerRow(), theCacheArray.LowerCol()));
    aCache += aCacheShift - 1;
    for (aRow = 0; aRow <= d2; aRow++)
      for (aCol = 0; aCol <= d1; aCol++)
      {
        *aCache = 0.0;
        aCache += aCacheShift;
      }
    theCacheArray.SetValue(theCacheArray.LowerRow(),
                           theCacheArray.LowerCol() + aCacheShift - 1,
                           1.0);
  }
}

//=======================================================================
// function : CacheD0
// purpose  : Evaluates the polynomial cache of the Bspline Curve
//
//=======================================================================

void BSplSLib::CacheD0(const double                      UParameter,
                       const double                      VParameter,
                       const int                         UDegree,
                       const int                         VDegree,
                       const double                      UCacheParameter,
                       const double                      VCacheParameter,
                       const double                      USpanLenght,
                       const double                      VSpanLenght,
                       const NCollection_Array2<gp_Pnt>& PolesArray,
                       const NCollection_Array2<double>* WeightsArray,
                       gp_Pnt&                           aPoint)
{
  //
  // the CacheParameter is where the cache polynomial was evaluated in homogeneous
  // form
  // the SpanLenght     is the normalizing factor so that the polynomial is between
  // 0 and 1
  int
    //    ii,
    dimension,
    min_degree, max_degree;

  double new_parameter[2], inverse;

  double* PArray  = (double*)&(PolesArray(PolesArray.LowerCol(), PolesArray.LowerRow()));
  double* myPoint = (double*)&aPoint;
  if (UDegree <= VDegree)
  {
    min_degree       = UDegree;
    max_degree       = VDegree;
    new_parameter[1] = (UParameter - UCacheParameter) / USpanLenght;
    new_parameter[0] = (VParameter - VCacheParameter) / VSpanLenght;
    dimension        = 3 * (UDegree + 1);
  }
  else
  {
    min_degree       = VDegree;
    max_degree       = UDegree;
    new_parameter[0] = (UParameter - UCacheParameter) / USpanLenght;
    new_parameter[1] = (VParameter - VCacheParameter) / VSpanLenght;
    dimension        = 3 * (VDegree + 1);
  }
  NCollection_LocalArray<double> locpoles(dimension);

  PLib::NoDerivativeEvalPolynomial(new_parameter[0],
                                   max_degree,
                                   dimension,
                                   max_degree * dimension,
                                   PArray[0],
                                   locpoles[0]);

  PLib::NoDerivativeEvalPolynomial(new_parameter[1],
                                   min_degree,
                                   3,
                                   (min_degree << 1) + min_degree,
                                   locpoles[0],
                                   myPoint[0]);
  if (WeightsArray != nullptr)
  {
    dimension                                    = min_degree + 1;
    const NCollection_Array2<double>& refWeights = *WeightsArray;
    double* WArray = (double*)&refWeights(refWeights.LowerCol(), refWeights.LowerRow());
    PLib::NoDerivativeEvalPolynomial(new_parameter[0],
                                     max_degree,
                                     dimension,
                                     max_degree * dimension,
                                     WArray[0],
                                     locpoles[0]);

    PLib::NoDerivativeEvalPolynomial(new_parameter[1],
                                     min_degree,
                                     1,
                                     min_degree,
                                     locpoles[0],
                                     inverse);
    inverse = 1.0e0 / inverse;

    myPoint[0] *= inverse;
    myPoint[1] *= inverse;
    myPoint[2] *= inverse;
  }
}

//=======================================================================
// function : CacheD1
// purpose  : Evaluates the polynomial cache of the Bspline Curve
//
//=======================================================================

void BSplSLib::CacheD1(const double                      UParameter,
                       const double                      VParameter,
                       const int                         UDegree,
                       const int                         VDegree,
                       const double                      UCacheParameter,
                       const double                      VCacheParameter,
                       const double                      USpanLenght,
                       const double                      VSpanLenght,
                       const NCollection_Array2<gp_Pnt>& PolesArray,
                       const NCollection_Array2<double>* WeightsArray,
                       gp_Pnt&                           aPoint,
                       gp_Vec&                           aVecU,
                       gp_Vec&                           aVecV)
{
  //
  // the CacheParameter is where the cache polynomial was evaluated in homogeneous
  // form
  // the SpanLenght     is the normalizing factor so that the polynomial is between
  // 0 and 1
  int
    //    ii,
    //  jj,
    //  kk,
    dimension,
    min_degree, max_degree;

  double inverse_min, inverse_max, new_parameter[2];

  double* PArray = (double*)&(PolesArray(PolesArray.LowerCol(), PolesArray.LowerRow()));
  double  local_poles_array[2][2][3], local_poles_and_weights_array[2][2][4],
    local_weights_array[2][2];
  double *my_vec_min, *my_vec_max, *my_point;
  my_point = (double*)&aPoint;
  //
  // initialize in case of rational evaluation
  // because RationalDerivative will use all
  // the coefficients
  //
  //
  if (WeightsArray != nullptr)
  {

    local_poles_array[0][0][0]             = 0.0e0;
    local_poles_array[0][0][1]             = 0.0e0;
    local_poles_array[0][0][2]             = 0.0e0;
    local_weights_array[0][0]              = 0.0e0;
    local_poles_and_weights_array[0][0][0] = 0.0e0;
    local_poles_and_weights_array[0][0][1] = 0.0e0;
    local_poles_and_weights_array[0][0][2] = 0.0e0;
    local_poles_and_weights_array[0][0][3] = 0.0e0;

    local_poles_array[0][1][0]             = 0.0e0;
    local_poles_array[0][1][1]             = 0.0e0;
    local_poles_array[0][1][2]             = 0.0e0;
    local_weights_array[0][1]              = 0.0e0;
    local_poles_and_weights_array[0][1][0] = 0.0e0;
    local_poles_and_weights_array[0][1][1] = 0.0e0;
    local_poles_and_weights_array[0][1][2] = 0.0e0;
    local_poles_and_weights_array[0][1][3] = 0.0e0;

    local_poles_array[1][0][0]             = 0.0e0;
    local_poles_array[1][0][1]             = 0.0e0;
    local_poles_array[1][0][2]             = 0.0e0;
    local_weights_array[1][0]              = 0.0e0;
    local_poles_and_weights_array[1][0][0] = 0.0e0;
    local_poles_and_weights_array[1][0][1] = 0.0e0;
    local_poles_and_weights_array[1][0][2] = 0.0e0;
    local_poles_and_weights_array[1][0][3] = 0.0e0;

    local_poles_array[1][1][0]             = 0.0e0;
    local_poles_array[1][1][1]             = 0.0e0;
    local_poles_array[1][1][2]             = 0.0e0;
    local_weights_array[1][1]              = 0.0e0;
    local_poles_and_weights_array[1][1][0] = 0.0e0;
    local_poles_and_weights_array[1][1][1] = 0.0e0;
    local_poles_and_weights_array[1][1][2] = 0.0e0;
    local_poles_and_weights_array[1][1][3] = 0.0e0;
  }

  if (UDegree <= VDegree)
  {
    min_degree       = UDegree;
    max_degree       = VDegree;
    inverse_min      = 1.0e0 / USpanLenght;
    inverse_max      = 1.0e0 / VSpanLenght;
    new_parameter[0] = (VParameter - VCacheParameter) * inverse_max;
    new_parameter[1] = (UParameter - UCacheParameter) * inverse_min;

    dimension  = 3 * (UDegree + 1);
    my_vec_min = (double*)&aVecU;
    my_vec_max = (double*)&aVecV;
  }
  else
  {
    min_degree       = VDegree;
    max_degree       = UDegree;
    inverse_min      = 1.0e0 / VSpanLenght;
    inverse_max      = 1.0e0 / USpanLenght;
    new_parameter[0] = (UParameter - UCacheParameter) * inverse_max;
    new_parameter[1] = (VParameter - VCacheParameter) * inverse_min;
    dimension        = 3 * (VDegree + 1);
    my_vec_min       = (double*)&aVecV;
    my_vec_max       = (double*)&aVecU;
  }

  NCollection_LocalArray<double> locpoles(2 * dimension);

  PLib::EvalPolynomial(new_parameter[0], 1, max_degree, dimension, PArray[0], locpoles[0]);

  PLib::EvalPolynomial(new_parameter[1], 1, min_degree, 3, locpoles[0], local_poles_array[0][0][0]);
  PLib::NoDerivativeEvalPolynomial(new_parameter[1],
                                   min_degree,
                                   3,
                                   (min_degree << 1) + min_degree,
                                   locpoles[dimension],
                                   local_poles_array[1][0][0]);

  if (WeightsArray != nullptr)
  {
    dimension                                    = min_degree + 1;
    const NCollection_Array2<double>& refWeights = *WeightsArray;
    double* WArray = (double*)&refWeights(refWeights.LowerCol(), refWeights.LowerRow());
    PLib::EvalPolynomial(new_parameter[0], 1, max_degree, dimension, WArray[0], locpoles[0]);

    PLib::EvalPolynomial(new_parameter[1],
                         1,
                         min_degree,
                         1,
                         locpoles[0],
                         local_weights_array[0][0]);
    PLib::NoDerivativeEvalPolynomial(new_parameter[1],
                                     min_degree,
                                     1,
                                     min_degree,
                                     locpoles[dimension],
                                     local_weights_array[1][0]);

    local_poles_and_weights_array[0][0][0] = local_poles_array[0][0][0];
    local_poles_and_weights_array[0][0][1] = local_poles_array[0][0][1];
    local_poles_and_weights_array[0][0][2] = local_poles_array[0][0][2];
    local_poles_and_weights_array[0][0][3] = local_weights_array[0][0];

    local_poles_and_weights_array[0][1][0] = local_poles_array[0][1][0];
    local_poles_and_weights_array[0][1][1] = local_poles_array[0][1][1];
    local_poles_and_weights_array[0][1][2] = local_poles_array[0][1][2];
    local_poles_and_weights_array[0][1][3] = local_weights_array[0][1];

    local_poles_and_weights_array[1][0][0] = local_poles_array[1][0][0];
    local_poles_and_weights_array[1][0][1] = local_poles_array[1][0][1];
    local_poles_and_weights_array[1][0][2] = local_poles_array[1][0][2];
    local_poles_and_weights_array[1][0][3] = local_weights_array[1][0];

    local_poles_and_weights_array[1][1][0] = local_poles_array[1][1][0];
    local_poles_and_weights_array[1][1][1] = local_poles_array[1][1][1];
    local_poles_and_weights_array[1][1][2] = local_poles_array[1][1][2];
    local_poles_and_weights_array[1][1][3] = local_weights_array[1][1];

    BSplSLib::RationalDerivative(1,
                                 1,
                                 1,
                                 1,
                                 local_poles_and_weights_array[0][0][0],
                                 local_poles_array[0][0][0]);
  }

  my_point[0]   = local_poles_array[0][0][0];
  my_vec_min[0] = inverse_min * local_poles_array[0][1][0];
  my_vec_max[0] = inverse_max * local_poles_array[1][0][0];

  my_point[1]   = local_poles_array[0][0][1];
  my_vec_min[1] = inverse_min * local_poles_array[0][1][1];
  my_vec_max[1] = inverse_max * local_poles_array[1][0][1];

  my_point[2]   = local_poles_array[0][0][2];
  my_vec_min[2] = inverse_min * local_poles_array[0][1][2];
  my_vec_max[2] = inverse_max * local_poles_array[1][0][2];
}

//=======================================================================
// function : CacheD2
// purpose  : Evaluates the polynomial cache of the Bspline Curve
//
//=======================================================================

void BSplSLib::CacheD2(const double                      UParameter,
                       const double                      VParameter,
                       const int                         UDegree,
                       const int                         VDegree,
                       const double                      UCacheParameter,
                       const double                      VCacheParameter,
                       const double                      USpanLenght,
                       const double                      VSpanLenght,
                       const NCollection_Array2<gp_Pnt>& PolesArray,
                       const NCollection_Array2<double>* WeightsArray,
                       gp_Pnt&                           aPoint,
                       gp_Vec&                           aVecU,
                       gp_Vec&                           aVecV,
                       gp_Vec&                           aVecUU,
                       gp_Vec&                           aVecUV,
                       gp_Vec&                           aVecVV)
{
  //
  // the CacheParameter is where the cache polynomial was evaluated in homogeneous
  // form
  // the SpanLenght     is the normalizing factor so that the polynomial is between
  // 0 and 1
  int ii,
    //  jj,
    kk, index, dimension, min_degree, max_degree;

  double inverse_min, inverse_max, new_parameter[2];

  double* PArray = (double*)&(PolesArray(PolesArray.LowerCol(), PolesArray.LowerRow()));
  double  local_poles_array[3][3][3], local_poles_and_weights_array[3][3][4],
    local_weights_array[3][3];
  double *my_vec_min, *my_vec_max, *my_vec_min_min, *my_vec_max_max, *my_vec_min_max, *my_point;
  my_point = (double*)&aPoint;

  //
  // initialize in case the min and max degree are less than 2
  //
  local_poles_array[0][0][0] = 0.0e0;
  local_poles_array[0][0][1] = 0.0e0;
  local_poles_array[0][0][2] = 0.0e0;
  local_poles_array[0][1][0] = 0.0e0;
  local_poles_array[0][1][1] = 0.0e0;
  local_poles_array[0][1][2] = 0.0e0;
  local_poles_array[0][2][0] = 0.0e0;
  local_poles_array[0][2][1] = 0.0e0;
  local_poles_array[0][2][2] = 0.0e0;

  local_poles_array[1][0][0] = 0.0e0;
  local_poles_array[1][0][1] = 0.0e0;
  local_poles_array[1][0][2] = 0.0e0;
  local_poles_array[1][1][0] = 0.0e0;
  local_poles_array[1][1][1] = 0.0e0;
  local_poles_array[1][1][2] = 0.0e0;
  local_poles_array[1][2][0] = 0.0e0;
  local_poles_array[1][2][1] = 0.0e0;
  local_poles_array[1][2][2] = 0.0e0;

  local_poles_array[2][0][0] = 0.0e0;
  local_poles_array[2][0][1] = 0.0e0;
  local_poles_array[2][0][2] = 0.0e0;
  local_poles_array[2][1][0] = 0.0e0;
  local_poles_array[2][1][1] = 0.0e0;
  local_poles_array[2][1][2] = 0.0e0;
  local_poles_array[2][2][0] = 0.0e0;
  local_poles_array[2][2][1] = 0.0e0;
  local_poles_array[2][2][2] = 0.0e0;
  //
  // initialize in case of rational evaluation
  // because RationalDerivative will use all
  // the coefficients
  //
  //
  if (WeightsArray != nullptr)
  {

    local_poles_and_weights_array[0][0][0] = 0.0e0;
    local_poles_and_weights_array[0][0][1] = 0.0e0;
    local_poles_and_weights_array[0][0][2] = 0.0e0;
    local_poles_and_weights_array[0][1][0] = 0.0e0;
    local_poles_and_weights_array[0][1][1] = 0.0e0;
    local_poles_and_weights_array[0][1][2] = 0.0e0;
    local_poles_and_weights_array[0][2][0] = 0.0e0;
    local_poles_and_weights_array[0][2][1] = 0.0e0;
    local_poles_and_weights_array[0][2][2] = 0.0e0;

    local_poles_and_weights_array[1][0][0] = 0.0e0;
    local_poles_and_weights_array[1][0][1] = 0.0e0;
    local_poles_and_weights_array[1][0][2] = 0.0e0;
    local_poles_and_weights_array[1][1][0] = 0.0e0;
    local_poles_and_weights_array[1][1][1] = 0.0e0;
    local_poles_and_weights_array[1][1][2] = 0.0e0;
    local_poles_and_weights_array[1][2][0] = 0.0e0;
    local_poles_and_weights_array[1][2][1] = 0.0e0;
    local_poles_and_weights_array[1][2][2] = 0.0e0;

    local_poles_and_weights_array[2][0][0] = 0.0e0;
    local_poles_and_weights_array[2][0][1] = 0.0e0;
    local_poles_and_weights_array[2][0][2] = 0.0e0;
    local_poles_and_weights_array[2][1][0] = 0.0e0;
    local_poles_and_weights_array[2][1][1] = 0.0e0;
    local_poles_and_weights_array[2][1][2] = 0.0e0;
    local_poles_and_weights_array[2][2][0] = 0.0e0;
    local_poles_and_weights_array[2][2][1] = 0.0e0;
    local_poles_and_weights_array[2][2][2] = 0.0e0;

    local_poles_and_weights_array[0][0][3] = local_weights_array[0][0] = 0.0e0;
    local_poles_and_weights_array[0][1][3] = local_weights_array[0][1] = 0.0e0;
    local_poles_and_weights_array[0][2][3] = local_weights_array[0][2] = 0.0e0;
    local_poles_and_weights_array[1][0][3] = local_weights_array[1][0] = 0.0e0;
    local_poles_and_weights_array[1][1][3] = local_weights_array[1][1] = 0.0e0;
    local_poles_and_weights_array[1][2][3] = local_weights_array[1][2] = 0.0e0;
    local_poles_and_weights_array[2][0][3] = local_weights_array[2][0] = 0.0e0;
    local_poles_and_weights_array[2][1][3] = local_weights_array[2][1] = 0.0e0;
    local_poles_and_weights_array[2][2][3] = local_weights_array[2][2] = 0.0e0;
  }

  if (UDegree <= VDegree)
  {
    min_degree       = UDegree;
    max_degree       = VDegree;
    inverse_min      = 1.0e0 / USpanLenght;
    inverse_max      = 1.0e0 / VSpanLenght;
    new_parameter[0] = (VParameter - VCacheParameter) * inverse_max;
    new_parameter[1] = (UParameter - UCacheParameter) * inverse_min;

    dimension      = 3 * (UDegree + 1);
    my_vec_min     = (double*)&aVecU;
    my_vec_max     = (double*)&aVecV;
    my_vec_min_min = (double*)&aVecUU;
    my_vec_min_max = (double*)&aVecUV;
    my_vec_max_max = (double*)&aVecVV;
  }
  else
  {
    min_degree       = VDegree;
    max_degree       = UDegree;
    inverse_min      = 1.0e0 / VSpanLenght;
    inverse_max      = 1.0e0 / USpanLenght;
    new_parameter[0] = (UParameter - UCacheParameter) * inverse_max;
    new_parameter[1] = (VParameter - VCacheParameter) * inverse_min;
    dimension        = 3 * (VDegree + 1);
    my_vec_min       = (double*)&aVecV;
    my_vec_max       = (double*)&aVecU;
    my_vec_min_min   = (double*)&aVecVV;
    my_vec_min_max   = (double*)&aVecUV;
    my_vec_max_max   = (double*)&aVecUU;
  }

  NCollection_LocalArray<double> locpoles(3 * dimension);

  //
  // initialize in case min or max degree are less than 2
  //
  int MinIndMax = 2;
  if (max_degree < 2)
    MinIndMax = max_degree;
  int MinIndMin = 2;
  if (min_degree < 2)
    MinIndMin = min_degree;

  index = MinIndMax * dimension;

  for (ii = MinIndMax; ii < 3; ii++)
  {

    for (kk = 0; kk < dimension; kk++)
    {
      locpoles[index] = 0.0e0;
      index += 1;
    }
  }

  PLib::EvalPolynomial(new_parameter[0], MinIndMax, max_degree, dimension, PArray[0], locpoles[0]);

  PLib::EvalPolynomial(new_parameter[1],
                       MinIndMin,
                       min_degree,
                       3,
                       locpoles[0],
                       local_poles_array[0][0][0]);
  PLib::EvalPolynomial(new_parameter[1],
                       1,
                       min_degree,
                       3,
                       locpoles[dimension],
                       local_poles_array[1][0][0]);

  PLib::NoDerivativeEvalPolynomial(new_parameter[1],
                                   min_degree,
                                   3,
                                   (min_degree << 1) + min_degree,
                                   locpoles[dimension + dimension],
                                   local_poles_array[2][0][0]);

  if (WeightsArray != nullptr)
  {
    dimension                                    = min_degree + 1;
    const NCollection_Array2<double>& refWeights = *WeightsArray;
    double* WArray = (double*)&refWeights(refWeights.LowerCol(), refWeights.LowerRow());
    PLib::EvalPolynomial(new_parameter[0],
                         MinIndMax,
                         max_degree,
                         dimension,
                         WArray[0],
                         locpoles[0]);

    PLib::EvalPolynomial(new_parameter[1],
                         MinIndMin,
                         min_degree,
                         1,
                         locpoles[0],
                         local_weights_array[0][0]);
    PLib::EvalPolynomial(new_parameter[1],
                         1,
                         min_degree,
                         1,
                         locpoles[dimension],
                         local_weights_array[1][0]);
    PLib::NoDerivativeEvalPolynomial(new_parameter[1],
                                     min_degree,
                                     1,
                                     min_degree,
                                     locpoles[dimension + dimension],
                                     local_weights_array[2][0]);

    local_poles_and_weights_array[0][0][0] = local_poles_array[0][0][0];
    local_poles_and_weights_array[0][0][1] = local_poles_array[0][0][1];
    local_poles_and_weights_array[0][0][2] = local_poles_array[0][0][2];
    local_poles_and_weights_array[0][1][0] = local_poles_array[0][1][0];
    local_poles_and_weights_array[0][1][1] = local_poles_array[0][1][1];
    local_poles_and_weights_array[0][1][2] = local_poles_array[0][1][2];
    local_poles_and_weights_array[0][2][0] = local_poles_array[0][2][0];
    local_poles_and_weights_array[0][2][1] = local_poles_array[0][2][1];
    local_poles_and_weights_array[0][2][2] = local_poles_array[0][2][2];

    local_poles_and_weights_array[1][0][0] = local_poles_array[1][0][0];
    local_poles_and_weights_array[1][0][1] = local_poles_array[1][0][1];
    local_poles_and_weights_array[1][0][2] = local_poles_array[1][0][2];
    local_poles_and_weights_array[1][1][0] = local_poles_array[1][1][0];
    local_poles_and_weights_array[1][1][1] = local_poles_array[1][1][1];
    local_poles_and_weights_array[1][1][2] = local_poles_array[1][1][2];
    local_poles_and_weights_array[1][2][0] = local_poles_array[1][2][0];
    local_poles_and_weights_array[1][2][1] = local_poles_array[1][2][1];
    local_poles_and_weights_array[1][2][2] = local_poles_array[1][2][2];

    local_poles_and_weights_array[2][0][0] = local_poles_array[2][0][0];
    local_poles_and_weights_array[2][0][1] = local_poles_array[2][0][1];
    local_poles_and_weights_array[2][0][2] = local_poles_array[2][0][2];
    local_poles_and_weights_array[2][1][0] = local_poles_array[2][1][0];
    local_poles_and_weights_array[2][1][1] = local_poles_array[2][1][1];
    local_poles_and_weights_array[2][1][2] = local_poles_array[2][1][2];
    local_poles_and_weights_array[2][2][0] = local_poles_array[2][2][0];
    local_poles_and_weights_array[2][2][1] = local_poles_array[2][2][1];
    local_poles_and_weights_array[2][2][2] = local_poles_array[2][2][2];

    local_poles_and_weights_array[0][0][3] = local_weights_array[0][0];
    local_poles_and_weights_array[0][1][3] = local_weights_array[0][1];
    local_poles_and_weights_array[0][2][3] = local_weights_array[0][2];
    local_poles_and_weights_array[1][0][3] = local_weights_array[1][0];
    local_poles_and_weights_array[1][1][3] = local_weights_array[1][1];
    local_poles_and_weights_array[1][2][3] = local_weights_array[1][2];
    local_poles_and_weights_array[2][0][3] = local_weights_array[2][0];
    local_poles_and_weights_array[2][1][3] = local_weights_array[2][1];
    local_poles_and_weights_array[2][2][3] = local_weights_array[2][2];

    BSplSLib::RationalDerivative(2,
                                 2,
                                 2,
                                 2,
                                 local_poles_and_weights_array[0][0][0],
                                 local_poles_array[0][0][0]);
  }

  double minmin = inverse_min * inverse_min;
  double minmax = inverse_min * inverse_max;
  double maxmax = inverse_max * inverse_max;

  my_point[0]       = local_poles_array[0][0][0];
  my_vec_min[0]     = inverse_min * local_poles_array[0][1][0];
  my_vec_max[0]     = inverse_max * local_poles_array[1][0][0];
  my_vec_min_min[0] = minmin * local_poles_array[0][2][0];
  my_vec_min_max[0] = minmax * local_poles_array[1][1][0];
  my_vec_max_max[0] = maxmax * local_poles_array[2][0][0];

  my_point[1]       = local_poles_array[0][0][1];
  my_vec_min[1]     = inverse_min * local_poles_array[0][1][1];
  my_vec_max[1]     = inverse_max * local_poles_array[1][0][1];
  my_vec_min_min[1] = minmin * local_poles_array[0][2][1];
  my_vec_min_max[1] = minmax * local_poles_array[1][1][1];
  my_vec_max_max[1] = maxmax * local_poles_array[2][0][1];

  my_point[2]       = local_poles_array[0][0][2];
  my_vec_min[2]     = inverse_min * local_poles_array[0][1][2];
  my_vec_max[2]     = inverse_max * local_poles_array[1][0][2];
  my_vec_min_min[2] = minmin * local_poles_array[0][2][2];
  my_vec_min_max[2] = minmax * local_poles_array[1][1][2];
  my_vec_max_max[2] = maxmax * local_poles_array[2][0][2];
}

//=======================================================================
// function : MovePoint
// purpose  : Find the new poles which allows  an old point (with a
//           given  u and v as parameters) to reach a new position
//=======================================================================

void BSplSLib::MovePoint(const double                      U,
                         const double                      V,
                         const gp_Vec&                     Displ,
                         const int                         UIndex1,
                         const int                         UIndex2,
                         const int                         VIndex1,
                         const int                         VIndex2,
                         const int                         UDegree,
                         const int                         VDegree,
                         const bool                        Rational,
                         const NCollection_Array2<gp_Pnt>& Poles,
                         const NCollection_Array2<double>& Weights,
                         const NCollection_Array1<double>& UFlatKnots,
                         const NCollection_Array1<double>& VFlatKnots,
                         int&                              UFirstIndex,
                         int&                              ULastIndex,
                         int&                              VFirstIndex,
                         int&                              VLastIndex,
                         NCollection_Array2<gp_Pnt>&       NewPoles)
{
  // calculate the UBSplineBasis in the parameter U
  int         UFirstNonZeroBsplineIndex;
  math_Matrix UBSplineBasis(1, 1, 1, UDegree + 1);
  int         ErrorCod1 = BSplCLib::EvalBsplineBasis(0,
                                             UDegree + 1,
                                             UFlatKnots,
                                             U,
                                             UFirstNonZeroBsplineIndex,
                                             UBSplineBasis);
  // calculate the VBSplineBasis in the parameter V
  int         VFirstNonZeroBsplineIndex;
  math_Matrix VBSplineBasis(1, 1, 1, VDegree + 1);
  int         ErrorCod2 = BSplCLib::EvalBsplineBasis(0,
                                             VDegree + 1,
                                             VFlatKnots,
                                             V,
                                             VFirstNonZeroBsplineIndex,
                                             VBSplineBasis);
  if (ErrorCod1 || ErrorCod2)
  {
    UFirstIndex = 0;
    ULastIndex  = 0;
    VFirstIndex = 0;
    VLastIndex  = 0;
    return;
  }

  // find the span which is predominant for parameter U
  UFirstIndex = UFirstNonZeroBsplineIndex;
  ULastIndex  = UFirstNonZeroBsplineIndex + UDegree;
  if (UFirstIndex < UIndex1)
    UFirstIndex = UIndex1;
  if (ULastIndex > UIndex2)
    ULastIndex = UIndex2;

  double maxValue = 0.0;
  int    ukk1     = 0, ukk2;

  for (int i = UFirstIndex - UFirstNonZeroBsplineIndex + 1;
       i <= ULastIndex - UFirstNonZeroBsplineIndex + 1;
       i++)
  {
    if (UBSplineBasis(1, i) > maxValue)
    {
      ukk1     = i + UFirstNonZeroBsplineIndex - 1;
      maxValue = UBSplineBasis(1, i);
    }
  }

  // find a ukk2 if symmetry
  ukk2 = ukk1;
  if ((ukk1 + 1) <= ULastIndex)
  {
    if (std::abs(UBSplineBasis(1, ukk1 - UFirstNonZeroBsplineIndex + 2) - maxValue) < 1.e-10)
    {
      ukk2 = ukk1 + 1;
    }
  }

  // find the span which is predominant for parameter V
  VFirstIndex = VFirstNonZeroBsplineIndex;
  VLastIndex  = VFirstNonZeroBsplineIndex + VDegree;

  if (VFirstIndex < VIndex1)
    VFirstIndex = VIndex1;
  if (VLastIndex > VIndex2)
    VLastIndex = VIndex2;

  maxValue = 0.0;
  int vkk1 = 0, vkk2;

  for (int j = VFirstIndex - VFirstNonZeroBsplineIndex + 1;
       j <= VLastIndex - VFirstNonZeroBsplineIndex + 1;
       j++)
  {
    if (VBSplineBasis(1, j) > maxValue)
    {
      vkk1     = j + VFirstNonZeroBsplineIndex - 1;
      maxValue = VBSplineBasis(1, j);
    }
  }

  // find a vkk2 if symmetry
  vkk2 = vkk1;
  if ((vkk1 + 1) <= VLastIndex)
  {
    if (std::abs(VBSplineBasis(1, vkk1 - VFirstNonZeroBsplineIndex + 2) - maxValue) < 1.e-10)
    {
      vkk2 = vkk1 + 1;
    }
  }

  // compute the vector of displacement
  double D1 = 0.0;
  double D2 = 0.0;
  double hN, Coef, DvalU, DvalV;

  for (int i = 1; i <= UDegree + 1; i++)
  {
    const int ii = i + UFirstNonZeroBsplineIndex - 1;
    if (ii < ukk1)
    {
      DvalU = ukk1 - ii;
    }
    else if (ii > ukk2)
    {
      DvalU = ii - ukk2;
    }
    else
    {
      DvalU = 0.0;
    }

    for (int j = 1; j <= VDegree + 1; j++)
    {
      const int jj = j + VFirstNonZeroBsplineIndex - 1;
      if (Rational)
      {
        hN = Weights(ii, jj) * UBSplineBasis(1, i) * VBSplineBasis(1, j);
        D2 += hN;
      }
      else
      {
        hN = UBSplineBasis(1, i) * VBSplineBasis(1, j);
      }
      if (ii >= UFirstIndex && ii <= ULastIndex && jj >= VFirstIndex && jj <= VLastIndex)
      {
        if (jj < vkk1)
        {
          DvalV = vkk1 - jj;
        }
        else if (jj > vkk2)
        {
          DvalV = jj - vkk2;
        }
        else
        {
          DvalV = 0.0;
        }
        D1 += 1. / (DvalU + DvalV + 1.) * hN;
      }
    }
  }

  if (Rational)
  {
    Coef = D2 / D1;
  }
  else
  {
    Coef = 1. / D1;
  }

  // compute the new poles

  for (int i = Poles.LowerRow(); i <= Poles.UpperRow(); i++)
  {
    if (i < ukk1)
    {
      DvalU = ukk1 - i;
    }
    else if (i > ukk2)
    {
      DvalU = i - ukk2;
    }
    else
    {
      DvalU = 0.0;
    }

    for (int j = Poles.LowerCol(); j <= Poles.UpperCol(); j++)
    {
      if (i >= UFirstIndex && i <= ULastIndex && j >= VFirstIndex && j <= VLastIndex)
      {
        if (j < vkk1)
        {
          DvalV = vkk1 - j;
        }
        else if (j > vkk2)
        {
          DvalV = j - vkk2;
        }
        else
        {
          DvalV = 0.0;
        }
        NewPoles(i, j) = Poles(i, j).Translated((Coef / (DvalU + DvalV + 1.)) * Displ);
      }
      else
      {
        NewPoles(i, j) = Poles(i, j);
      }
    }
  }
}

//=======================================================================
// function : Resolution
// purpose  : this computes an estimate for the maximum of the
// partial derivatives both in U and in V
//
//
// The calculation resembles at the calculation of curves with
// additional index for the control point. Let Si,j be the
// control points for ls surface  and  Di,j  the weights.
// The checking of upper bounds for the partial derivatives
// will be omitted and Su is the next upper bound in the polynomial case :
//
//
//
//                        |  Si,j - Si-1,j  |
//          d *   Max     |  -------------  |
//                i = 2,n |     ti+d - ti   |
//                i=1.m
//
//
// and in the rational case :
//
//
//
//                         Di,j * (Si,j - Sk,j) - Di-1,j * (Si-1,j - Sk,j)
//   Max   Max       d  *  -----------------------------------------------
// k=1,n  i dans Rj                   ti+d  - ti
// j=1,m
//  ----------------------------------------------------------------------
//
//               Min    Di,j
//              i=1,n
//              j=1,m
//
//
//
// with Rj = {j-d, ....,  j+d+d+1}.
//
//
//=======================================================================

void BSplSLib::Resolution(const NCollection_Array2<gp_Pnt>& Poles,
                          const NCollection_Array2<double>* Weights,
                          const NCollection_Array1<double>& UKnots,
                          const NCollection_Array1<double>& VKnots,
                          const NCollection_Array1<int>&    UMults,
                          const NCollection_Array1<int>&    VMults,
                          const int                         UDegree,
                          const int                         VDegree,
                          const bool                        URational,
                          const bool                        VRational,
                          const bool                        UPeriodic,
                          const bool                        VPeriodic,
                          const double                      Tolerance3D,
                          double&                           UTolerance,
                          double&                           VTolerance)
{
  double Wij, Wmj, Wji, Wjm;
  double Xij, Xmj, Xji, Xjm, Xpq, Xqp;
  double Yij, Ymj, Yji, Yjm, Ypq, Yqp;
  double Zij, Zmj, Zji, Zjm, Zpq, Zqp;
  double factor, value, min, min_weights = 0, inverse, max_derivative[2];

  max_derivative[0] = max_derivative[1] = 0.0e0;

  int PRowLength, PColLength;
  int ii, jj, pp, qq, ii_index, jj_index, pp_index, qq_index;
  int ii_minus, upper[2], lower[2], poles_length[2];
  int num_poles[2], num_flat_knots[2];

  num_flat_knots[0] = BSplCLib::KnotSequenceLength(UMults, UDegree, UPeriodic);
  num_flat_knots[1] = BSplCLib::KnotSequenceLength(VMults, VDegree, VPeriodic);
  NCollection_Array1<double> flat_knots_in_u(1, num_flat_knots[0]);
  NCollection_Array1<double> flat_knots_in_v(1, num_flat_knots[1]);
  BSplCLib::KnotSequence(UKnots, UMults, UDegree, UPeriodic, flat_knots_in_u);
  BSplCLib::KnotSequence(VKnots, VMults, VDegree, VPeriodic, flat_knots_in_v);
  PRowLength = Poles.RowLength();
  PColLength = Poles.ColLength();
  if (URational || VRational)
  {
    int                               Wsize     = PRowLength * PColLength;
    const NCollection_Array2<double>& refWights = *Weights;
    const double*                     WG = &refWights(refWights.LowerRow(), refWights.LowerCol());
    min_weights                          = WG[0];

    for (ii = 1; ii < Wsize; ii++)
    {
      min = WG[ii];
      if (min_weights > min)
        min_weights = min;
    }
  }
  int UD1         = UDegree + 1;
  int VD1         = VDegree + 1;
  num_poles[0]    = num_flat_knots[0] - UD1;
  num_poles[1]    = num_flat_knots[1] - VD1;
  poles_length[0] = PColLength;
  poles_length[1] = PRowLength;
  if (URational)
  {
    int UD2 = UDegree << 1;
    int VD2 = VDegree << 1;

    for (ii = 2; ii <= num_poles[0]; ii++)
    {
      ii_index = (ii - 1) % poles_length[0] + 1;
      ii_minus = (ii - 2) % poles_length[0] + 1;
      inverse  = flat_knots_in_u(ii + UDegree) - flat_knots_in_u(ii);
      inverse  = 1.0e0 / inverse;
      lower[0] = ii - UD1;
      if (lower[0] < 1)
        lower[0] = 1;
      upper[0] = ii + UD2 + 1;
      if (upper[0] > num_poles[0])
        upper[0] = num_poles[0];

      for (jj = 1; jj <= num_poles[1]; jj++)
      {
        jj_index = (jj - 1) % poles_length[1] + 1;
        lower[1] = jj - VD1;
        if (lower[1] < 1)
          lower[1] = 1;
        upper[1] = jj + VD2 + 1;
        if (upper[1] > num_poles[1])
          upper[1] = num_poles[1];
        const gp_Pnt& Pij = Poles.Value(ii_index, jj_index);
        Wij               = Weights->Value(ii_index, jj_index);
        const gp_Pnt& Pmj = Poles.Value(ii_minus, jj_index);
        Wmj               = Weights->Value(ii_minus, jj_index);
        Xij               = Pij.X();
        Yij               = Pij.Y();
        Zij               = Pij.Z();
        Xmj               = Pmj.X();
        Ymj               = Pmj.Y();
        Zmj               = Pmj.Z();

        for (pp = lower[0]; pp <= upper[0]; pp++)
        {
          pp_index = (pp - 1) % poles_length[0] + 1;

          for (qq = lower[1]; qq <= upper[1]; qq++)
          {
            value             = 0.0e0;
            qq_index          = (qq - 1) % poles_length[1] + 1;
            const gp_Pnt& Ppq = Poles.Value(pp_index, qq_index);
            Xpq               = Ppq.X();
            Ypq               = Ppq.Y();
            Zpq               = Ppq.Z();
            factor            = (Xpq - Xij) * Wij;
            factor -= (Xpq - Xmj) * Wmj;
            if (factor < 0)
              factor = -factor;
            value += factor;
            factor = (Ypq - Yij) * Wij;
            factor -= (Ypq - Ymj) * Wmj;
            if (factor < 0)
              factor = -factor;
            value += factor;
            factor = (Zpq - Zij) * Wij;
            factor -= (Zpq - Zmj) * Wmj;
            if (factor < 0)
              factor = -factor;
            value += factor;
            value *= inverse;
            if (max_derivative[0] < value)
              max_derivative[0] = value;
          }
        }
      }
    }
    max_derivative[0] /= min_weights;
  }
  else
  {

    for (ii = 2; ii <= num_poles[0]; ii++)
    {
      ii_index = (ii - 1) % poles_length[0] + 1;
      ii_minus = (ii - 2) % poles_length[0] + 1;
      inverse  = flat_knots_in_u(ii + UDegree) - flat_knots_in_u(ii);
      inverse  = 1.0e0 / inverse;

      for (jj = 1; jj <= num_poles[1]; jj++)
      {
        jj_index          = (jj - 1) % poles_length[1] + 1;
        value             = 0.0e0;
        const gp_Pnt& Pij = Poles.Value(ii_index, jj_index);
        const gp_Pnt& Pmj = Poles.Value(ii_minus, jj_index);
        factor            = Pij.X() - Pmj.X();
        if (factor < 0)
          factor = -factor;
        value += factor;
        factor = Pij.Y() - Pmj.Y();
        if (factor < 0)
          factor = -factor;
        value += factor;
        factor = Pij.Z() - Pmj.Z();
        if (factor < 0)
          factor = -factor;
        value += factor;
        value *= inverse;
        if (max_derivative[0] < value)
          max_derivative[0] = value;
      }
    }
  }
  max_derivative[0] *= UDegree;
  if (VRational)
  {
    int UD2 = UDegree << 1;
    int VD2 = VDegree << 1;

    for (ii = 2; ii <= num_poles[1]; ii++)
    {
      ii_index = (ii - 1) % poles_length[1] + 1;
      ii_minus = (ii - 2) % poles_length[1] + 1;
      inverse  = flat_knots_in_v(ii + VDegree) - flat_knots_in_v(ii);
      inverse  = 1.0e0 / inverse;
      lower[0] = ii - VD1;
      if (lower[0] < 1)
        lower[0] = 1;
      upper[0] = ii + VD2 + 1;
      if (upper[0] > num_poles[1])
        upper[0] = num_poles[1];

      for (jj = 1; jj <= num_poles[0]; jj++)
      {
        jj_index = (jj - 1) % poles_length[0] + 1;
        lower[1] = jj - UD1;
        if (lower[1] < 1)
          lower[1] = 1;
        upper[1] = jj + UD2 + 1;
        if (upper[1] > num_poles[0])
          upper[1] = num_poles[0];
        const gp_Pnt& Pji = Poles.Value(jj_index, ii_index);
        Wji               = Weights->Value(jj_index, ii_index);
        const gp_Pnt& Pjm = Poles.Value(jj_index, ii_minus);
        Wjm               = Weights->Value(jj_index, ii_minus);
        Xji               = Pji.X();
        Yji               = Pji.Y();
        Zji               = Pji.Z();
        Xjm               = Pjm.X();
        Yjm               = Pjm.Y();
        Zjm               = Pjm.Z();

        for (pp = lower[1]; pp <= upper[1]; pp++)
        {
          pp_index = (pp - 1) % poles_length[1] + 1;

          for (qq = lower[0]; qq <= upper[0]; qq++)
          {
            value             = 0.0e0;
            qq_index          = (qq - 1) % poles_length[0] + 1;
            const gp_Pnt& Pqp = Poles.Value(qq_index, pp_index);
            Xqp               = Pqp.X();
            Yqp               = Pqp.Y();
            Zqp               = Pqp.Z();
            factor            = (Xqp - Xji) * Wji;
            factor -= (Xqp - Xjm) * Wjm;
            if (factor < 0)
              factor = -factor;
            value += factor;
            factor = (Yqp - Yji) * Wji;
            factor -= (Yqp - Yjm) * Wjm;
            if (factor < 0)
              factor = -factor;
            value += factor;
            factor = (Zqp - Zji) * Wji;
            factor -= (Zqp - Zjm) * Wjm;
            if (factor < 0)
              factor = -factor;
            value += factor;
            value *= inverse;
            if (max_derivative[1] < value)
              max_derivative[1] = value;
          }
        }
      }
    }
    max_derivative[1] /= min_weights;
  }
  else
  {

    for (ii = 2; ii <= num_poles[1]; ii++)
    {
      ii_index = (ii - 1) % poles_length[1] + 1;
      ii_minus = (ii - 2) % poles_length[1] + 1;
      inverse  = flat_knots_in_v(ii + VDegree) - flat_knots_in_v(ii);
      inverse  = 1.0e0 / inverse;

      for (jj = 1; jj <= num_poles[0]; jj++)
      {
        jj_index          = (jj - 1) % poles_length[0] + 1;
        value             = 0.0e0;
        const gp_Pnt& Pji = Poles.Value(jj_index, ii_index);
        const gp_Pnt& Pjm = Poles.Value(jj_index, ii_minus);
        factor            = Pji.X() - Pjm.X();
        if (factor < 0)
          factor = -factor;
        value += factor;
        factor = Pji.Y() - Pjm.Y();
        if (factor < 0)
          factor = -factor;
        value += factor;
        factor = Pji.Z() - Pjm.Z();
        if (factor < 0)
          factor = -factor;
        value += factor;
        value *= inverse;
        if (max_derivative[1] < value)
          max_derivative[1] = value;
      }
    }
  }
  max_derivative[1] *= VDegree;
  max_derivative[0] *= M_SQRT2;
  max_derivative[1] *= M_SQRT2;
  if (max_derivative[0] && max_derivative[1])
  {
    UTolerance = Tolerance3D / max_derivative[0];
    VTolerance = Tolerance3D / max_derivative[1];
  }
  else
  {
    UTolerance = VTolerance = 0.0;
#ifdef OCCT_DEBUG
    std::cout << "ElSLib.cxx : maxderivative = 0.0 " << std::endl;
#endif
  }
}

//=================================================================================================

void BSplSLib::Interpolate(const int                         UDegree,
                           const int                         VDegree,
                           const NCollection_Array1<double>& UFlatKnots,
                           const NCollection_Array1<double>& VFlatKnots,
                           const NCollection_Array1<double>& UParameters,
                           const NCollection_Array1<double>& VParameters,
                           NCollection_Array2<gp_Pnt>&       Poles,
                           NCollection_Array2<double>&       Weights,
                           int&                              InversionProblem)
{
  int     ii, jj, ll, kk, dimension;
  int     ULength = UParameters.Length();
  int     VLength = VParameters.Length();
  double* poles_array;

  // extraction of iso u
  dimension = 4 * ULength;
  NCollection_Array2<double> Points(1, VLength, 1, dimension);

  occ::handle<NCollection_HArray1<int>> ContactOrder = new (NCollection_HArray1<int>)(1, VLength);
  ContactOrder->Init(0);

  for (ii = 1; ii <= VLength; ii++)
  {

    for (jj = 1, ll = 1; jj <= ULength; jj++, ll += 4)
    {
      Points(ii, ll)     = Poles(jj, ii).X();
      Points(ii, ll + 1) = Poles(jj, ii).Y();
      Points(ii, ll + 2) = Poles(jj, ii).Z();
      Points(ii, ll + 3) = Weights(jj, ii);
    }
  }

  // interpolation of iso u
  poles_array = (double*)&Points.ChangeValue(1, 1);
  BSplCLib::Interpolate(VDegree,
                        VFlatKnots,
                        VParameters,
                        ContactOrder->Array1(),
                        dimension,
                        poles_array[0],
                        InversionProblem);
  if (InversionProblem != 0)
    return;

  // extraction of iso v

  dimension = VLength * 4;
  NCollection_Array2<double> IsoPoles(1, ULength, 1, dimension);

  ContactOrder = new (NCollection_HArray1<int>)(1, ULength);
  ContactOrder->Init(0);
  poles_array = (double*)&IsoPoles.ChangeValue(1, 1);

  for (ii = 1, kk = 1; ii <= ULength; ii++, kk += 4)
  {

    for (jj = 1, ll = 1; jj <= VLength; jj++, ll += 4)
    {
      IsoPoles(ii, ll)     = Points(jj, kk);
      IsoPoles(ii, ll + 1) = Points(jj, kk + 1);
      IsoPoles(ii, ll + 2) = Points(jj, kk + 2);
      IsoPoles(ii, ll + 3) = Points(jj, kk + 3);
    }
  }
  // interpolation of iso v
  BSplCLib::Interpolate(UDegree,
                        UFlatKnots,
                        UParameters,
                        ContactOrder->Array1(),
                        dimension,
                        poles_array[0],
                        InversionProblem);

  // return results

  for (ii = 1; ii <= ULength; ii++)
  {

    for (jj = 1, ll = 1; jj <= VLength; jj++, ll += 4)
    {
      gp_Pnt Pnt(IsoPoles(ii, ll), IsoPoles(ii, ll + 1), IsoPoles(ii, ll + 2));
      Poles.SetValue(ii, jj, Pnt);
      Weights.SetValue(ii, jj, IsoPoles(ii, ll + 3));
    }
  }
}

//=================================================================================================

void BSplSLib::Interpolate(const int                         UDegree,
                           const int                         VDegree,
                           const NCollection_Array1<double>& UFlatKnots,
                           const NCollection_Array1<double>& VFlatKnots,
                           const NCollection_Array1<double>& UParameters,
                           const NCollection_Array1<double>& VParameters,
                           NCollection_Array2<gp_Pnt>&       Poles,
                           int&                              InversionProblem)
{
  int     ii, jj, ll, kk, dimension;
  int     ULength = UParameters.Length();
  int     VLength = VParameters.Length();
  double* poles_array;

  // extraction of iso u
  dimension = 3 * ULength;
  NCollection_Array2<double> Points(1, VLength, 1, dimension);

  occ::handle<NCollection_HArray1<int>> ContactOrder = new (NCollection_HArray1<int>)(1, VLength);
  ContactOrder->Init(0);

  for (ii = 1; ii <= VLength; ii++)
  {

    for (jj = 1, ll = 1; jj <= ULength; jj++, ll += 3)
    {
      Points(ii, ll)     = Poles(jj, ii).X();
      Points(ii, ll + 1) = Poles(jj, ii).Y();
      Points(ii, ll + 2) = Poles(jj, ii).Z();
    }
  }

  // interpolation of iso u
  poles_array = (double*)&Points.ChangeValue(1, 1);
  BSplCLib::Interpolate(VDegree,
                        VFlatKnots,
                        VParameters,
                        ContactOrder->Array1(),
                        dimension,
                        poles_array[0],
                        InversionProblem);
  if (InversionProblem != 0)
    return;

  // extraction of iso v

  dimension = VLength * 3;
  NCollection_Array2<double> IsoPoles(1, ULength, 1, dimension);

  ContactOrder = new (NCollection_HArray1<int>)(1, ULength);
  ContactOrder->Init(0);
  poles_array = (double*)&IsoPoles.ChangeValue(1, 1);

  for (ii = 1, kk = 1; ii <= ULength; ii++, kk += 3)
  {

    for (jj = 1, ll = 1; jj <= VLength; jj++, ll += 3)
    {
      IsoPoles(ii, ll)     = Points(jj, kk);
      IsoPoles(ii, ll + 1) = Points(jj, kk + 1);
      IsoPoles(ii, ll + 2) = Points(jj, kk + 2);
    }
  }
  // interpolation of iso v
  BSplCLib::Interpolate(UDegree,
                        UFlatKnots,
                        UParameters,
                        ContactOrder->Array1(),
                        dimension,
                        poles_array[0],
                        InversionProblem);

  // return results

  for (ii = 1; ii <= ULength; ii++)
  {

    for (jj = 1, ll = 1; jj <= VLength; jj++, ll += 3)
    {
      gp_Pnt Pnt(IsoPoles(ii, ll), IsoPoles(ii, ll + 1), IsoPoles(ii, ll + 2));
      Poles.SetValue(ii, jj, Pnt);
    }
  }
}

//=================================================================================================

void BSplSLib::FunctionMultiply(const BSplSLib_EvaluatorFunction& Function,
                                const int                         UBSplineDegree,
                                const int                         VBSplineDegree,
                                const NCollection_Array1<double>& UBSplineKnots,
                                const NCollection_Array1<double>& VBSplineKnots,
                                const NCollection_Array1<int>*    UMults,
                                const NCollection_Array1<int>*    VMults,
                                const NCollection_Array2<gp_Pnt>& Poles,
                                const NCollection_Array2<double>* Weights,
                                const NCollection_Array1<double>& UFlatKnots,
                                const NCollection_Array1<double>& VFlatKnots,
                                const int                         UNewDegree,
                                const int                         VNewDegree,
                                NCollection_Array2<gp_Pnt>&       NewNumerator,
                                NCollection_Array2<double>&       NewDenominator,
                                int&                              theStatus)
{
  int num_uparameters,
    //  ii,jj,kk,
    ii, jj, error_code, num_vparameters;
  double result;

  num_uparameters = UFlatKnots.Length() - UNewDegree - 1;
  num_vparameters = VFlatKnots.Length() - VNewDegree - 1;
  NCollection_Array1<double> UParameters(1, num_uparameters);
  NCollection_Array1<double> VParameters(1, num_vparameters);

  if ((NewNumerator.ColLength() == num_uparameters) && (NewNumerator.RowLength() == num_vparameters)
      && (NewDenominator.ColLength() == num_uparameters)
      && (NewDenominator.RowLength() == num_vparameters))
  {

    BSplCLib::BuildSchoenbergPoints(UNewDegree, UFlatKnots, UParameters);

    BSplCLib::BuildSchoenbergPoints(VNewDegree, VFlatKnots, VParameters);

    for (ii = 1; ii <= num_uparameters; ii++)
    {

      for (jj = 1; jj <= num_vparameters; jj++)
      {
        HomogeneousD0(UParameters(ii),
                      VParameters(jj),
                      0,
                      0,
                      Poles,
                      Weights,
                      UBSplineKnots,
                      VBSplineKnots,
                      UMults,
                      VMults,
                      UBSplineDegree,
                      VBSplineDegree,
                      true,
                      true,
                      false,
                      false,
                      NewDenominator(ii, jj),
                      NewNumerator(ii, jj));

        Function.Evaluate(0, UParameters(ii), VParameters(jj), result, error_code);
        if (error_code)
        {
          throw Standard_ConstructionError();
        }
        gp_Pnt& P = NewNumerator(ii, jj);
        P.SetX(P.X() * result);
        P.SetY(P.Y() * result);
        P.SetZ(P.Z() * result);
        NewDenominator(ii, jj) *= result;
      }
    }
    Interpolate(UNewDegree,
                VNewDegree,
                UFlatKnots,
                VFlatKnots,
                UParameters,
                VParameters,
                NewNumerator,
                NewDenominator,
                theStatus);
  }
  else
  {
    throw Standard_ConstructionError();
  }
}

//==================================================================================================

NCollection_Array2<double> BSplSLib::UnitWeights(const int theNbUPoles, const int theNbVPoles)
{
  const int aTotal = theNbUPoles * theNbVPoles;
  if (aTotal <= BSplCLib::MaxUnitWeightsSize())
  {
    // Non-owning view over global static array (zero allocation).
    return NCollection_Array2<double>(BSplCLib::UnitWeightsData()[0],
                                      1,
                                      theNbUPoles,
                                      1,
                                      theNbVPoles);
  }
  // Rare: allocate fresh array.
  NCollection_Array2<double> aResult(1, theNbUPoles, 1, theNbVPoles);
  aResult.Init(1.0);
  return aResult;
}
