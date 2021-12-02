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

#include <ApproxInt_KnotTools.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <math_Vector.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <NCollection_Sequence.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <NCollection_Vector.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <GeomInt_WLApprox.hxx>
#include <GeomInt_TheMultiLineOfWLApprox.hxx>

// (Sqrt(5.0) - 1.0) / 4.0
//static const Standard_Real aSinCoeff = 0.30901699437494742410229341718282;
static const Standard_Real aSinCoeff2 = 0.09549150281252627; // aSinCoeff^2 = (3. - Sqrt(5.)) / 8.
static const Standard_Integer aMaxPntCoeff = 15;

//=======================================================================
//function : EvalCurv
//purpose  : Evaluate curvature in dim-dimension point.
//=======================================================================
static Standard_Real EvalCurv(const Standard_Real dim, 
                              const Standard_Real* V1,
                              const Standard_Real* V2)
{
  // Really V1 and V2 are arrays of size dim;
  // V1 is first derivative, V2 is second derivative
  // of n-dimension curve
  // Curvature is curv = |V1^V2|/|V1|^3
  // V1^V2 is outer product of two vectors:
  // P(i,j) = V1(i)*V2(j) - V1(j)*V2(i);
  Standard_Real mp = 0.;
  Standard_Integer i, j;
  Standard_Real p;
  for(i = 1; i < dim; ++i)
  {
    for(j = 0; j < i; ++j)
    {
      p = V1[i]*V2[j] - V1[j]*V2[i];
      mp += p*p;
    }
  }
  //
  Standard_Real q = 0.;
  for(i = 0; i < dim; ++i)
  {
    q += V1[i]*V1[i];
  }

  if (q < 1 / Precision::Infinite())
  {
    // Indeed, if q is small then we can
    // obtain equivocation of "0/0" type.
    // In this case, local curvature can be
    // not equal to 0 or Infinity.
    // However, it is good solution to insert
    // knot in the place with such singularity.
    // Therefore, we need imitation of curvature
    // jumping. Return of Precision::Infinite() is
    // enough for it.

    return Precision::Infinite();
  }

  q = Min(q, Precision::Infinite());
  q *= q*q;

  //
  Standard_Real curv = Sqrt(mp / q);

  return curv;
}

//=======================================================================
//function : BuildCurvature
//purpose  : 
//=======================================================================

void ApproxInt_KnotTools::BuildCurvature(
  const NCollection_LocalArray<Standard_Real>& theCoords,
  const Standard_Integer theDim,
  const math_Vector& thePars,
  TColStd_Array1OfReal& theCurv,
  Standard_Real& theMaxCurv)
{
  // Arrays are allocated for max theDim = 7: 1 3d curve + 2 2d curves.
  Standard_Real Val[21], Par[3], Res[21];
  Standard_Integer i, j, m, ic;
  Standard_Integer dim = theDim;
  //
  theMaxCurv = 0.;
  if (theCurv.Length() < 3)
  {
    theCurv.Init(0.);
    return;
  }
  i = theCurv.Lower();
  for (j = 0; j < 3; ++j)
  {
    Standard_Integer k = i + j;
    ic = (k - theCurv.Lower()) * dim;
    Standard_Integer l = dim*j;
    for (m = 0; m < dim; ++m)
    {
      Val[l + m] = theCoords[ic + m];
    }
    Par[j] = thePars(k);
  }
  PLib::EvalLagrange(Par[0], 2, 2, dim, *Val, *Par, *Res);
  //
  theCurv(i) = EvalCurv(dim, &Res[dim], &Res[2 * dim]);
  //
  if (theCurv(i) > theMaxCurv)
  {
    theMaxCurv = theCurv(i);
  }
  //
  for (i = theCurv.Lower() + 1; i < theCurv.Upper(); ++i)
  {
    for (j = 0; j < 3; ++j)
    {
      Standard_Integer k = i + j - 1;
      ic = (k - theCurv.Lower()) * dim;
      Standard_Integer l = dim*j;
      for (m = 0; m < dim; ++m)
      {
        Val[l + m] = theCoords[ic + m];
      }
      Par[j] = thePars(k);
    }
    PLib::EvalLagrange(Par[1], 2, 2, dim, *Val, *Par, *Res);
    //
    theCurv(i) = EvalCurv(dim, &Res[dim], &Res[2 * dim]);
    if (theCurv(i) > theMaxCurv)
    {
      theMaxCurv = theCurv(i);
    }
  }
  //
  i = theCurv.Upper();
  for (j = 0; j < 3; ++j)
  {
    Standard_Integer k = i + j - 2;
    ic = (k - theCurv.Lower()) * dim;
    Standard_Integer l = dim*j;
    for (m = 0; m < dim; ++m)
    {
      Val[l + m] = theCoords[ic + m];
    }
    Par[j] = thePars(k);
  }
  PLib::EvalLagrange(Par[2], 2, 2, dim, *Val, *Par, *Res);
  //
  theCurv(i) = EvalCurv(dim, &Res[dim], &Res[2 * dim]);
  if (theCurv(i) > theMaxCurv)
  {
    theMaxCurv = theCurv(i);
  }

}

//=======================================================================
//function : ComputeKnotInds
//purpose  : 
//=======================================================================
void ApproxInt_KnotTools::ComputeKnotInds(const NCollection_LocalArray<Standard_Real>& theCoords,
                                          const Standard_Integer theDim, 
                                          const math_Vector& thePars,
                                          NCollection_Sequence<Standard_Integer>& theInds)
{
  //I: Create discrete curvature.
  NCollection_Sequence<Standard_Integer> aFeatureInds;
  TColStd_Array1OfReal aCurv(thePars.Lower(), thePars.Upper());
  Standard_Real aMaxCurv = 0.;
  BuildCurvature(theCoords, theDim, thePars, aCurv, aMaxCurv);
  //  
  Standard_Integer i, j, dim = theDim;
#ifdef APPROXINT_KNOTTOOLS_DEBUG
  std::cout << "Discrete curvature array is" << std::endl;
  for(i = aCurv.Lower(); i <= aCurv.Upper(); ++i)
  {
    std::cout << i << " " << aCurv(i) << std::endl;
  }
#endif

  theInds.Append(aCurv.Lower());
  if(aMaxCurv <= Precision::Confusion())
  {
    // Linear case.
    theInds.Append(aCurv.Upper());
    return;
  }

  // II: Find extremas of curvature.
  // Not used Precision::PConfusion, by different from "param space" eps nature.
  Standard_Real eps  = 1.0e-9,
                eps1 = 1.0e3 * eps;
  for(i = aCurv.Lower() + 1; i < aCurv.Upper(); ++i)
  {
    Standard_Real d1 = aCurv(i) - aCurv(i - 1),
                  d2 = aCurv(i) - aCurv(i + 1),
                  ad1 = Abs(d1), ad2 = Abs(d2);

    if(d1*d2 > 0. && ad1 > eps && ad2 > eps)
    {
      if(i != theInds.Last())
      {
        theInds.Append(i);
        aFeatureInds.Append(i);
      }
    }
    else if((ad1 < eps && ad2 > eps1) || (ad1 > eps1 && ad2 < eps))
    {
      if(i != theInds.Last())
      {
        theInds.Append(i);
        aFeatureInds.Append(i);
      }
    }
  }
  if(aCurv.Upper() != theInds.Last())
  {
    theInds.Append(aCurv.Upper());
  }

#if defined(APPROXINT_KNOTTOOLS_DEBUG)
  {
    std::cout << "Feature indices new: " << std::endl;
    i;
    for(i = theInds.Lower(); i <= theInds.Upper();  ++i)
    {
      std::cout << i << " : " << theInds(i) << std::endl;
    }
  }
#endif

  //III: Put knots in monotone intervals of curvature.
  Standard_Boolean Ok;
  i = 1;
  do
  {
    i++;
    //
    Ok = InsKnotBefI(i, aCurv, theCoords, dim, theInds, Standard_True); 
    if(Ok)
    {
      i--;
    }
  }
  while(i < theInds.Length());

  //IV: Checking feature points.
  j = 2;
  for(i = 1; i <= aFeatureInds.Length(); ++i)
  {
    Standard_Integer anInd = aFeatureInds(i);
    for(; j <= theInds.Length() - 1;)
    {
      if(theInds(j) == anInd)
      {
        Standard_Integer anIndPrev = theInds(j-1);
        Standard_Integer anIndNext = theInds(j+1);
        Standard_Integer ici = (anIndPrev - aCurv.Lower()) * theDim,
          ici1 = (anIndNext - aCurv.Lower()) * theDim,
          icm = (anInd - aCurv.Lower()) * theDim;
        NCollection_LocalArray<Standard_Real> V1(theDim), V2(theDim);
        Standard_Real mp = 0., m1 = 0., m2 = 0.;
        Standard_Real p;
        for(Standard_Integer k = 0; k < theDim; ++k)
        {
          V1[k] = theCoords[icm + k] - theCoords[ici + k];
          m1 += V1[k]*V1[k];
          V2[k] = theCoords[ici1 + k] - theCoords[icm + k];
          m2 += V2[k]*V2[k];
        }
        for(Standard_Integer k = 1; k < theDim; ++k)
        {
          for(Standard_Integer l = 0; l < k; ++l)
          {
            p = V1[k]*V2[l] - V1[l]*V2[k];
            mp += p*p;
          }
        }
        //mp *= 2.; //P(j,i) = -P(i,j);
        //

        if(mp > aSinCoeff2 * m1 * m2) // Sqrt (mp/(m1*m2)) > aSinCoeff
        {
          //Insert new knots
          Standard_Real d1 = Abs(aCurv(anInd) - aCurv(anIndPrev));
          Standard_Real d2 = Abs(aCurv(anInd) - aCurv(anIndNext));
          if(d1 > d2)
          {
            Ok = InsKnotBefI(j, aCurv, theCoords, dim, theInds, Standard_False);
            if(Ok)
            {
              j++;
            }
            else
            {
              break;
            }
          }
          else
          {
            Ok = InsKnotBefI(j+1, aCurv, theCoords, dim, theInds, Standard_False);
            if(!Ok)
            {
              break;
            }
          }
        }
        else
        {
          j++;
          break;
        }
      }
      else
      {
        j++;
      }
    }
  }
  //
}


//=======================================================================
//function : FilterKnots
//purpose  :
//=======================================================================
void ApproxInt_KnotTools::FilterKnots(NCollection_Sequence<Standard_Integer>& theInds, 
                                      const Standard_Integer theMinNbPnts,
                                      NCollection_Vector<Standard_Integer>& theLKnots)
{
  // Maximum number of points per knot interval.
  Standard_Integer aMaxNbPnts = aMaxPntCoeff*theMinNbPnts;
  Standard_Integer i = 1;
  Standard_Integer aMinNbStep = theMinNbPnts / 2;

  // I: Filter too big number of points per knot interval.
  while(i < theInds.Length())
  {
    Standard_Integer nbint = theInds(i + 1) - theInds(i) + 1;
    if(nbint <= aMaxNbPnts)
    {
      ++i;
      continue;
    }
    else
    {
      Standard_Integer ind = theInds(i) + nbint / 2;
      theInds.InsertAfter(i, ind);     
    }
  }

  // II: Filter points with too small amount of points per knot interval.
  i = 1;
  theLKnots.Append(theInds(i));
  Standard_Integer anIndsPrev = theInds(i);
  for(i = 2; i <= theInds.Length(); ++i)
  {
    if(theInds(i) - anIndsPrev <= theMinNbPnts)
    {
      if (i != theInds.Length())
      {
        Standard_Integer anIdx = i + 1;
        for( ; anIdx <= theInds.Length(); ++anIdx)
        {
          if (theInds(anIdx) - anIndsPrev >= theMinNbPnts)
            break;
        }
        anIdx--;

        Standard_Integer aMidIdx = (theInds(anIdx) + anIndsPrev) / 2;
        if (aMidIdx - anIndsPrev        < theMinNbPnts &&
            aMidIdx - theInds(anIdx)    < theMinNbPnts &&
            theInds(anIdx) - anIndsPrev >= aMinNbStep)
        {
          if (theInds(anIdx) - anIndsPrev > 2 * theMinNbPnts)
          {
            // Bad distribution points merge into one knot interval.
            theLKnots.Append(anIndsPrev + theMinNbPnts);
            anIndsPrev = anIndsPrev + theMinNbPnts;
            i = anIdx - 1;
          }
          else
          {
            if (theInds(anIdx - 1) - anIndsPrev >= theMinNbPnts / 2)
            {
              // Bad distribution points merge into one knot interval.
              theLKnots.Append(theInds(anIdx - 1));
              anIndsPrev = theInds(anIdx - 1);
              i = anIdx - 1;
              if (theInds(anIdx) - theInds(anIdx - 1) <= theMinNbPnts / 2)
              {
                theLKnots.SetValue(theLKnots.Upper(), theInds(anIdx));
                anIndsPrev = theInds(anIdx );
                i = anIdx;
              }
            }
            else
            {
              // Bad distribution points merge into one knot interval.
              theLKnots.Append(theInds(anIdx));
              anIndsPrev = theInds(anIdx);
              i = anIdx;
            }
          }
        }
        else if (anIdx == theInds.Upper() && // Last point obtained.
                 theLKnots.Length() >= 2) // It is possible to modify last item.
        {
          // Current bad interval from i to last.
          // Trying to add knot to divide sequence on two parts:
          // Last good index -> Last index - theMinNbPnts -> Last index
          Standard_Integer aLastGoodIdx = theLKnots.Value(theLKnots.Upper() - 1);
          if ( theInds.Last() - 2 * theMinNbPnts >= aLastGoodIdx)
          {
            theLKnots(theLKnots.Upper()) = theInds.Last() - theMinNbPnts;
            theLKnots.Append(theInds.Last());
            anIndsPrev = theInds(anIdx);
            i = anIdx;
          }
        }
      } // if (i != theInds.Length())
      continue;
    }
    else
    {
      theLKnots.Append(theInds(i));
      anIndsPrev = theInds(i);
    }
  }

  // III: Fill Last Knot.
  if(theLKnots.Length() < 2)
  {
    theLKnots.Append(theInds.Last());
  }
  else
  {
    if(theLKnots.Last() < theInds.Last())
    {
      theLKnots(theLKnots.Upper()) = theInds.Last();
    }
  }
}
//=======================================================================
//function : InsKnotBefI
//purpose  :
//=======================================================================
Standard_Boolean ApproxInt_KnotTools::InsKnotBefI(const Standard_Integer theI,
                                                  const TColStd_Array1OfReal& theCurv,
                                                  const NCollection_LocalArray<Standard_Real>& theCoords,
                                                  const Standard_Integer theDim, 
                                                  NCollection_Sequence<Standard_Integer>& theInds,
                                                  const Standard_Boolean ChkCurv)
{
  Standard_Integer anInd1 = theInds(theI);
  Standard_Integer anInd = theInds(theI - 1);
  //
  if((anInd1-anInd) == 1)
  {
    return Standard_False;
  }
  //
  Standard_Real curv = 0.5*(theCurv(anInd) + theCurv(anInd1));
  Standard_Integer mid = 0, j, jj;
  const Standard_Real aLimitCurvatureChange = 3.0;
  for(j = anInd+1; j < anInd1; ++j)
  {
    mid = 0;

    // I: Curvature change criteria:
    // Non-null curvature.
    if (theCurv(j)     > Precision::Confusion() && 
        theCurv(anInd) > Precision::Confusion() )
    {
      if (theCurv(j) / theCurv(anInd) > aLimitCurvatureChange || 
          theCurv(j) / theCurv(anInd) < 1.0 / aLimitCurvatureChange)
      {
        // Curvature on current interval changed more than 3 times.
        mid = j;
        theInds.InsertBefore(theI, mid);
        return Standard_True;
      }
    }

    // II: Angular criteria:
    Standard_Real ac = theCurv(j - 1), ac1 = theCurv(j);
    if((curv >= ac && curv <= ac1) || (curv >= ac1 && curv <= ac))
    {
      if(Abs(curv - ac) < Abs(curv - ac1))
      {
        mid = j - 1;
      }
      else
      {
        mid = j;
      }
    }
    if(mid == anInd)
    {
      mid++;
    }
    if(mid == anInd1)
    {
      mid--;
    }
    if(mid > 0)
    {
      if(ChkCurv)
      {
        Standard_Integer ici = (anInd - theCurv.Lower()) * theDim,
          ici1 = (anInd1 - theCurv.Lower()) * theDim,
          icm = (mid - theCurv.Lower()) * theDim;
        NCollection_LocalArray<Standard_Real> V1(theDim), V2(theDim);
        Standard_Integer i;
        Standard_Real mp = 0., m1 = 0., m2 = 0.;
        Standard_Real p;
        for(i = 0; i < theDim; ++i)
        {
          V1[i] = theCoords[icm + i] - theCoords[ici + i];
          m1 += V1[i]*V1[i];
          V2[i] = theCoords[ici1 + i] - theCoords[icm + i];
          m2 += V2[i]*V2[i];
        }
        for(i = 1; i < theDim; ++i)
        {
          for(jj = 0; jj < i; ++jj)
          {
            p = V1[i]*V2[jj] - V1[jj]*V2[i];
            mp += p*p;
          }
        }
        //mp *= 2.; //P(j,i) = -P(i,j);
        //

        if (mp > aSinCoeff2 * m1 * m2) // Sqrt (mp / m1m2) > aSinCoeff
        {
          theInds.InsertBefore(theI, mid);
          return Standard_True;
        }
      }
      else
      {
        theInds.InsertBefore(theI, mid);
        return Standard_True;
      }
    }
  }

  return Standard_False;
}

//=======================================================================
//function : BuildKnots
//purpose  :
//=======================================================================
void ApproxInt_KnotTools::BuildKnots(const TColgp_Array1OfPnt& thePntsXYZ,
                                     const TColgp_Array1OfPnt2d& thePntsU1V1,
                                     const TColgp_Array1OfPnt2d& thePntsU2V2,
                                     const math_Vector& thePars,
                                     const Standard_Boolean theApproxXYZ,
                                     const Standard_Boolean theApproxU1V1,
                                     const Standard_Boolean theApproxU2V2,
                                     const Standard_Integer theMinNbPnts,
                                     NCollection_Vector<Standard_Integer>& theKnots)
{
  NCollection_Sequence<Standard_Integer> aKnots;
  Standard_Integer aDim = 0;

  // I: Convert input data to the corresponding format.
  if(theApproxXYZ)
    aDim += 3;
  if(theApproxU1V1)
    aDim += 2;
  if(theApproxU2V2)
    aDim += 2;

  NCollection_LocalArray<Standard_Real> aCoords(thePars.Length()*aDim);
  Standard_Integer i, j;
  for(i = thePars.Lower(); i <= thePars.Upper(); ++i)
  {
    j = (i - thePars.Lower()) * aDim;
    if(theApproxXYZ)
    {
      aCoords[j] = thePntsXYZ.Value(i).X();
      ++j;
      aCoords[j] = thePntsXYZ.Value(i).Y();
      ++j;
      aCoords[j] = thePntsXYZ.Value(i).Z();
      ++j;
    }
    if(theApproxU1V1)
    {
      aCoords[j] = thePntsU1V1.Value(i).X();
      ++j;
      aCoords[j] = thePntsU1V1.Value(i).Y();
      ++j;
    }
    if(theApproxU2V2)
    {
      aCoords[j] = thePntsU2V2.Value(i).X();
      ++j;
      aCoords[j] = thePntsU2V2.Value(i).Y();
      ++j;
    }
  }

  // II: Build draft knot sequence.
  ComputeKnotInds(aCoords, aDim, thePars, aKnots);

#if defined(APPROXINT_KNOTTOOLS_DEBUG)
    std::cout << "Draft knot sequence: " << std::endl;
    for(i = aKnots.Lower(); i <= aKnots.Upper();  ++i)
    {
      std::cout << i << " : " << aKnots(i) << std::endl;
    }
#endif

  // III: Build output knot sequence.
  FilterKnots(aKnots, theMinNbPnts, theKnots);

#if defined(APPROXINT_KNOTTOOLS_DEBUG)
    std::cout << "Result knot sequence: " << std::endl;
    for(i = theKnots.Lower(); i <= theKnots.Upper();  ++i)
    {
      std::cout << i << " : " << theKnots(i) << std::endl;
    }
#endif

}
//=======================================================================
//function : MaxParamRatio
//purpose  :
//=======================================================================
static Standard_Real MaxParamRatio(const math_Vector& thePars)
{
  Standard_Integer i;
  Standard_Real aMaxRatio = 0.;
  //
  for (i = thePars.Lower() + 1; i < thePars.Upper(); ++i)
  {
    Standard_Real aRat = (thePars(i + 1) - thePars(i)) / (thePars(i) - thePars(i - 1));
    if (aRat < 1.)
      aRat = 1. / aRat;

    aMaxRatio = Max(aMaxRatio, aRat);
  }
  return aMaxRatio;
}
//=======================================================================
//function : DefineParType
//purpose  :
//=======================================================================
Approx_ParametrizationType ApproxInt_KnotTools::DefineParType(
  const Handle(IntPatch_WLine)& theWL,
  const Standard_Integer theFpar, const Standard_Integer theLpar,
  const Standard_Boolean theApproxXYZ,
  const Standard_Boolean theApproxU1V1,
  const Standard_Boolean theApproxU2V2
)
{
  if (theLpar - theFpar == 1)
    return Approx_IsoParametric;

  const Standard_Integer  nbp3d = theApproxXYZ ? 1 : 0,
    nbp2d = (theApproxU1V1 ? 1 : 0) + (theApproxU2V2 ? 1 : 0);

  GeomInt_TheMultiLineOfWLApprox aTestLine(theWL, nbp3d, nbp2d, theApproxU1V1, theApproxU2V2,
    0., 0., 0., 0., 0., 0., 0., theApproxU1V1, theFpar, theLpar);

  TColgp_Array1OfPnt aTabPnt3d(1, Max(1, nbp3d));
  TColgp_Array1OfPnt2d aTabPnt2d(1, Max(1, nbp2d));
  TColgp_Array1OfPnt aPntXYZ(theFpar, theLpar);
  TColgp_Array1OfPnt2d aPntU1V1(theFpar, theLpar);
  TColgp_Array1OfPnt2d aPntU2V2(theFpar, theLpar);

  Standard_Integer i, j;

  for (i = theFpar; i <= theLpar; ++i)
  {
    if (nbp3d != 0 && nbp2d != 0) aTestLine.Value(i, aTabPnt3d, aTabPnt2d);
    else if (nbp2d != 0)          aTestLine.Value(i, aTabPnt2d);
    else if (nbp3d != 0)          aTestLine.Value(i, aTabPnt3d);
    //
    if (nbp3d > 0)
    {
      aPntXYZ(i) = aTabPnt3d(1);
    }
    if (nbp2d > 1)
    {
      aPntU1V1(i) = aTabPnt2d(1);
      aPntU2V2(i) = aTabPnt2d(2);
    }
    else if (nbp2d > 0)
    {
      if (theApproxU1V1)
      {
        aPntU1V1(i) = aTabPnt2d(1);
      }
      else
      {
        aPntU2V2(i) = aTabPnt2d(1);
      }
    }
  }

  Standard_Integer aDim = 0;

  if (theApproxXYZ)
    aDim += 3;
  if (theApproxU1V1)
    aDim += 2;
  if (theApproxU2V2)
    aDim += 2;

  Standard_Integer aLength = theLpar - theFpar + 1;
  NCollection_LocalArray<Standard_Real> aCoords(aLength * aDim);
  for (i = theFpar; i <= theLpar; ++i)
  {
    j = (i - theFpar) * aDim;
    if (theApproxXYZ)
    {
      aCoords[j] = aPntXYZ.Value(i).X();
      ++j;
      aCoords[j] = aPntXYZ.Value(i).Y();
      ++j;
      aCoords[j] = aPntXYZ.Value(i).Z();
      ++j;
    }
    if (theApproxU1V1)
    {
      aCoords[j] = aPntU1V1.Value(i).X();
      ++j;
      aCoords[j] = aPntU1V1.Value(i).Y();
      ++j;
    }
    if (theApproxU2V2)
    {
      aCoords[j] = aPntU2V2.Value(i).X();
      ++j;
      aCoords[j] = aPntU2V2.Value(i).Y();
      ++j;
    }
  }

  //Analysis of curvature
  const Standard_Real aCritRat = 500.;
  const Standard_Real aCritParRat = 100.;
  math_Vector aPars(theFpar, theLpar);
  Approx_ParametrizationType aParType = Approx_ChordLength;
  GeomInt_WLApprox::Parameters(aTestLine, theFpar, theLpar, aParType, aPars);
  TColStd_Array1OfReal aCurv(aPars.Lower(), aPars.Upper());
  Standard_Real aMaxCurv = 0.;
  BuildCurvature(aCoords, aDim, aPars, aCurv, aMaxCurv);

  if (aMaxCurv < Precision::PConfusion() 
      || Precision::IsPositiveInfinite(aMaxCurv))
  {
    //Linear case
    return aParType;
  }

  Standard_Real aMidCurv = 0.;
  Standard_Real eps = Epsilon(1.);
  j = 0;
  for (i = aCurv.Lower(); i <= aCurv.Upper(); ++i)
  {
    if (aMaxCurv - aCurv(i) < eps)
    {
      continue;
    }
    ++j;
    aMidCurv += aCurv(i);
  }

  if (j > 1)
  {
    aMidCurv /= j;
  }

  if (aMidCurv <= eps)
    return aParType;

  Standard_Real aRat = aMaxCurv / aMidCurv;
  
  if (aRat > aCritRat)
  {
    if(aRat > 5.*aCritRat)
      aParType = Approx_Centripetal;
    else
    {
      Standard_Real aParRat = MaxParamRatio(aPars);
      if (aParRat > aCritParRat)
        aParType = Approx_Centripetal;
    }
  }

  return aParType;
}
