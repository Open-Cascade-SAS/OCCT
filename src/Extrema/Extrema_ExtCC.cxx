// Created on: 1994-07-06
// Created by: Laurent PAINNOT
// Copyright (c) 1994-1999 Matra Datavision
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

// Modified by MPS (june 96) : correction du trap dans le cas droite/Bezier 
// Modified by MPS (mai 97) : PRO 7598 
//                            tri des solutions pour eviter de rendre plusieurs
//                            fois la meme solution 

#include <Adaptor3d_Curve.hxx>
#include <Bnd_Range.hxx>
#include <ElCLib.hxx>
#include <Extrema_CurveTool.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_ExtElC.hxx>
#include <Extrema_ExtPElC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_SequenceOfPOnCurv.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>

//=======================================================================
//function : Extrema_ExtCC
//purpose  : 
//=======================================================================
Extrema_ExtCC::Extrema_ExtCC (const Standard_Real TolC1,
                              const Standard_Real TolC2)
: myIsFindSingleSolution(Standard_False),
  myDone (Standard_False),
  myIsParallel(Standard_False)
{
  myC[0] = 0; myC[1] = 0;
  myInf[0] = myInf[1] = -Precision::Infinite();
  mySup[0] = mySup[1] = Precision::Infinite();
  myTol[0] = TolC1; myTol[1] = TolC2;
  mydist11 = mydist12 = mydist21 = mydist22 = RealFirst();
}

//=======================================================================
//function : Extrema_ExtCC
//purpose  : 
//=======================================================================

Extrema_ExtCC::Extrema_ExtCC(const Adaptor3d_Curve& C1,
                             const Adaptor3d_Curve& C2,
                             const Standard_Real      U1,
                             const Standard_Real      U2,
                             const Standard_Real      V1,
                             const Standard_Real      V2,
                             const Standard_Real      TolC1,
                             const Standard_Real      TolC2)
: myIsFindSingleSolution(Standard_False),
  myECC(C1, C2, U1, U2, V1, V2),
  myDone (Standard_False)
{
  SetCurve (1, C1, U1, U2);
  SetCurve (2, C2, V1, V2);
  SetTolerance (1, TolC1);
  SetTolerance (2, TolC2);
  mydist11 = mydist12 = mydist21 = mydist22 = RealFirst();
  Perform();
}


//=======================================================================
//function : Extrema_ExtCC
//purpose  : 
//=======================================================================

Extrema_ExtCC::Extrema_ExtCC(const Adaptor3d_Curve& C1, 
                             const Adaptor3d_Curve& C2,
                             const Standard_Real      TolC1,
                             const Standard_Real      TolC2)
: myIsFindSingleSolution(Standard_False),
  myECC(C1, C2),
  myDone (Standard_False)
{
  SetCurve (1, C1, C1.FirstParameter(), C1.LastParameter());
  SetCurve (2, C2, C2.FirstParameter(), C2.LastParameter());
  SetTolerance (1, TolC1);
  SetTolerance (2, TolC2);
  mydist11 = mydist12 = mydist21 = mydist22 = RealFirst();
  Perform();
}

//=======================================================================
//function : Initialize
//purpose  :
//=======================================================================
void Extrema_ExtCC::Initialize (const Adaptor3d_Curve& C1,
                                const Adaptor3d_Curve& C2,
                                const Standard_Real TolC1,
                                const Standard_Real TolC2)
{
  // myECC will be re-initialized by Perform()
  myDone = Standard_False;
  SetCurve (1, C1, C1.FirstParameter(), C1.LastParameter());
  SetCurve (2, C2, C2.FirstParameter(), C2.LastParameter());
  SetTolerance (1, TolC1);
  SetTolerance (2, TolC2);
  mydist11 = mydist12 = mydist21 = mydist22 = RealFirst();
}

//=======================================================================
//function : Initialize
//purpose  :
//=======================================================================
void Extrema_ExtCC::Initialize (const Adaptor3d_Curve& C1,
                                const Adaptor3d_Curve& C2,
                                const Standard_Real U1,
                                const Standard_Real U2,
                                const Standard_Real V1,
                                const Standard_Real V2,
                                const Standard_Real TolC1,
                                const Standard_Real TolC2)
{
  // myECC will be re-initialized by Perform()
  myDone = Standard_False;
  SetCurve (1, C1, U1, U2);
  SetCurve (2, C2, V1, V2);
  SetTolerance (1, TolC1);
  SetTolerance (2, TolC2);
  mydist11 = mydist12 = mydist21 = mydist22 = RealFirst();
}

//=======================================================================
//function : SetCurve
//purpose  :
//=======================================================================

void Extrema_ExtCC::SetCurve (const Standard_Integer theRank, const Adaptor3d_Curve& C)
{
  Standard_OutOfRange_Raise_if (theRank < 1 || theRank > 2, "Extrema_ExtCC::SetCurve()")
  Standard_Integer anInd = theRank - 1;
  myC[anInd] = &C;
}

//=======================================================================
//function : SetCurve
//purpose  : 
//=======================================================================

void Extrema_ExtCC::SetCurve (const Standard_Integer theRank, const Adaptor3d_Curve& C,
                               const Standard_Real Uinf, const Standard_Real Usup)
{
  SetCurve (theRank, C);
  SetRange (theRank, Uinf, Usup);
}

//=======================================================================
//function : SetRange
//purpose  : 
//=======================================================================

void Extrema_ExtCC::SetRange (const Standard_Integer theRank, 
                               const Standard_Real Uinf, const Standard_Real Usup)
{
  Standard_OutOfRange_Raise_if (theRank < 1 || theRank > 2, "Extrema_ExtCC::SetRange()")
  Standard_Integer anInd = theRank - 1;
  myInf[anInd] = Uinf;
  mySup[anInd] = Usup;
}

//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================

void Extrema_ExtCC::SetTolerance (const Standard_Integer theRank, const Standard_Real theTol)
{
  Standard_OutOfRange_Raise_if (theRank < 1 || theRank > 2, "Extrema_ExtCC::SetTolerance()")
  Standard_Integer anInd = theRank - 1;
  myTol[anInd] = theTol;
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Extrema_ExtCC::Perform()
{  
  Standard_NullObject_Raise_if (!myC[0] || !myC[1], "Extrema_ExtCC::Perform()")
  myECC.SetParams(*myC[0], *myC[1], myInf[0], mySup[0], myInf[1], mySup[1]);
  myECC.SetTolerance(Min(myTol[0], myTol[1]));
  myECC.SetSingleSolutionFlag(GetSingleSolutionFlag());
  myDone = Standard_False;
  mypoints.Clear();
  mySqDist.Clear();
  myIsParallel = Standard_False;

  GeomAbs_CurveType type1 = myC[0]->GetType();
  GeomAbs_CurveType type2 = myC[1]->GetType();
  Standard_Real U11, U12, U21, U22, Tol = Min(myTol[0], myTol[1]);

  U11 = myInf[0];
  U12 = mySup[0];
  U21 = myInf[1];
  U22 = mySup[1];

  if (!Precision::IsInfinite(U11)) myP1f = Extrema_CurveTool::Value(*myC[0], U11);
  if (!Precision::IsInfinite(U12)) myP1l = Extrema_CurveTool::Value(*myC[0], U12);
  if (!Precision::IsInfinite(U21)) myP2f = Extrema_CurveTool::Value(*myC[1], U21);
  if (!Precision::IsInfinite(U22)) myP2l = Extrema_CurveTool::Value(*myC[1], U22);
  

  if (Precision::IsInfinite(U11) || Precision::IsInfinite(U21)) mydist11 = RealLast();
  else mydist11 = myP1f.SquareDistance(myP2f);
  if (Precision::IsInfinite(U11) || Precision::IsInfinite(U22)) mydist12 = RealLast();
  else mydist12 = myP1f.SquareDistance(myP2l);
  if (Precision::IsInfinite(U12) || Precision::IsInfinite(U21)) mydist21 = RealLast();
  else mydist21 = myP1l.SquareDistance(myP2f);
  if (Precision::IsInfinite(U12) || Precision::IsInfinite(U22)) mydist22 = RealLast();
  else mydist22 = myP1l.SquareDistance(myP2l);

  //Depending on the types of curves, the algorithm is chosen:
  //- _ExtElC, when one of the curves is a line and the other is elementary,
  //   or there are two circles;
  //- _GenExtCC, in all other cases
  if ( (type1 == GeomAbs_Line && type2 <= GeomAbs_Parabola) ||
       (type2 == GeomAbs_Line && type1 <= GeomAbs_Parabola) ) {
    //analytical case - one curve is always a line
    Standard_Integer anInd1 = 0, anInd2 = 1;
    GeomAbs_CurveType aType2 = type2;
    Standard_Boolean isInverse = (type1 > type2);
    if (isInverse)
    {
      //algorithm uses inverse order of arguments
      anInd1 = 1;
      anInd2 = 0;
      aType2 = type1;
    }
    switch (aType2) {
    case GeomAbs_Line: {
      Extrema_ExtElC Xtrem (myC[anInd1]->Line(), myC[anInd2]->Line(), Tol);
      PrepareResults(Xtrem, isInverse, U11, U12, U21, U22);
      break;
    }
    case GeomAbs_Circle: {
      Extrema_ExtElC Xtrem (myC[anInd1]->Line(), myC[anInd2]->Circle(), Tol);
      PrepareResults(Xtrem, isInverse, U11, U12, U21, U22);
      break;
    }
    case GeomAbs_Ellipse: {
      Extrema_ExtElC Xtrem (myC[anInd1]->Line(), myC[anInd2]->Ellipse());
      PrepareResults(Xtrem, isInverse, U11, U12, U21, U22);
      break;
    }
    case GeomAbs_Hyperbola: {
      Extrema_ExtElC Xtrem (myC[anInd1]->Line(), myC[anInd2]->Hyperbola());
      PrepareResults(Xtrem, isInverse, U11, U12, U21, U22);
      break;
    }
    case GeomAbs_Parabola: {
      Extrema_ExtElC Xtrem (myC[anInd1]->Line(), myC[anInd2]->Parabola());
      PrepareResults(Xtrem, isInverse, U11, U12, U21, U22);
      break;
    }
    default: break;
    }
  } else if (type1 == GeomAbs_Circle && type2 == GeomAbs_Circle) {
    //analytical case - two circles
    Standard_Boolean bIsDone;
    Extrema_ExtElC CCXtrem (myC[0]->Circle(), myC[1]->Circle());
    bIsDone = CCXtrem.IsDone();
    if(bIsDone) {
      PrepareResults(CCXtrem, Standard_False, U11, U12, U21, U22);
    }
    else {
      myECC.Perform();
      PrepareResults(myECC, U11, U12, U21, U22);
    }
  } else {
    myECC.Perform();
    PrepareResults(myECC, U11, U12, U21, U22);
  }
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_ExtCC::IsDone() const
{
  return myDone;
}

//=======================================================================
//function : IsParallel
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_ExtCC::IsParallel() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }

  return myIsParallel;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Extrema_ExtCC::SquareDistance(const Standard_Integer N) const 
{
  if ((N < 1) || (N > NbExt())) throw Standard_OutOfRange();
  return mySqDist.Value(N);
}


//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================

Standard_Integer Extrema_ExtCC::NbExt() const
{
  if(!myDone) throw StdFail_NotDone();
  return mySqDist.Length();
}


//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

void Extrema_ExtCC::Points(const Standard_Integer N, 
			    Extrema_POnCurv& P1,
			    Extrema_POnCurv& P2) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  P1 = mypoints.Value(2 * N - 1);
  P2 = mypoints.Value(2 * N);
}



//=======================================================================
//function : TrimmedDistances
//purpose  : 
//=======================================================================

void Extrema_ExtCC::TrimmedSquareDistances(Standard_Real& dist11,
				      Standard_Real& dist12,
				      Standard_Real& dist21,
				      Standard_Real& dist22,
				      gp_Pnt&        P11   ,
				      gp_Pnt&        P12   ,
				      gp_Pnt&        P21   ,
				      gp_Pnt&        P22    ) const {
					
  dist11 = mydist11;
  dist12 = mydist12;
  dist21 = mydist21;
  dist22 = mydist22;
  P11 = myP1f;
  P12 = myP1l;
  P21 = myP2f;
  P22 = myP2l;
}

//=======================================================================
//function : ParallelResult
//purpose  : 
//=======================================================================
void Extrema_ExtCC::PrepareParallelResult(const Standard_Real theUt11,
                                          const Standard_Real theUt12,
                                          const Standard_Real theUt21,
                                          const Standard_Real theUt22,
                                          const Standard_Real theSqDist)
{
  if (!myIsParallel)
    return;

  const GeomAbs_CurveType aType1 = Extrema_CurveTool::GetType (*myC[0]);
  const GeomAbs_CurveType aType2 = Extrema_CurveTool::GetType (*myC[1]);
  if (((aType1 != GeomAbs_Line) && (aType1 != GeomAbs_Circle)) ||
      ((aType2 != GeomAbs_Line) && (aType2 != GeomAbs_Circle)))
  {
    mySqDist.Append(theSqDist);
    myDone = Standard_True;
    myIsParallel = Standard_True;
    return;
  }
  
  // Parallel case is only for line-line, circle-circle and circle-line!!!
  // But really for trimmed curves extremas can not exist!
  if (aType1 != aType2)
  {
    //The projection of the circle's location to the trimmed line must exist.
    const Standard_Boolean isReversed = (aType1 != GeomAbs_Circle);
    const gp_Pnt aPonC = !isReversed ?
                      Extrema_CurveTool::Value (*myC[0], theUt11) :
                      Extrema_CurveTool::Value (*myC[1], theUt21);

    const gp_Lin aL = myC[!isReversed ? 1 : 0]->Line();
    const Extrema_ExtPElC ExtPLin(aPonC, aL, Precision::Confusion(),
                                  !isReversed ? theUt21 : theUt11,
                                  !isReversed ? theUt22 : theUt12);

    if (ExtPLin.IsDone())
    {
      mySqDist.Append(theSqDist);
    }
    else
    {
      myIsParallel = Standard_False;
    }

    return;
  }

  if (aType1 == GeomAbs_Line)
  {
    // Line - Line

    const Standard_Real isFirstInfinite = (Precision::IsInfinite(theUt11) &&
                                           Precision::IsInfinite(theUt12));
    const Standard_Real isLastInfinite = (Precision::IsInfinite(theUt21) &&
                                          Precision::IsInfinite(theUt22));

    if (isFirstInfinite || isLastInfinite)
    {
      // Infinite number of solution

      mySqDist.Append(theSqDist);
    }
    else
    {
      // The range created by projection of both ends of the 1st line
      // to the 2nd one must intersect the (native) trimmed range of
      // the 2nd line.

      myIsParallel = Standard_False;

      const gp_Lin aLin1 = myC[0]->Line();
      const gp_Lin aLin2 = myC[1]->Line();
      const Standard_Boolean isOpposite(aLin1.Direction().Dot(aLin2.Direction()) < 0.0);

      Bnd_Range aRange2(theUt21, theUt22);
      Bnd_Range aProjRng12;

      if (Precision::IsInfinite(theUt11))
      {
        if (isOpposite)
          aProjRng12.Add(Precision::Infinite());
        else
          aProjRng12.Add(-Precision::Infinite());
      }
      else
      {
        const gp_Pnt aPonC1 = ElCLib::Value(theUt11, aLin1);
        const Standard_Real aPar = ElCLib::Parameter(aLin2, aPonC1);
        aProjRng12.Add(aPar);
      }

      if (Precision::IsInfinite(theUt12))
      {
        if (isOpposite)
          aProjRng12.Add(-Precision::Infinite());
        else
          aProjRng12.Add(Precision::Infinite());
      }
      else
      {
        const gp_Pnt aPonC1 = ElCLib::Value(theUt12, aLin1);
        const Standard_Real aPar = ElCLib::Parameter(aLin2, aPonC1);
        aProjRng12.Add(aPar);
      }

      aRange2.Common(aProjRng12);
      if (aRange2.Delta() > Precision::Confusion())
      {
        ClearSolutions();
        mySqDist.Append(theSqDist);
        myIsParallel = Standard_True;
      }
      else if (!aRange2.IsVoid())
      {
        //Case like this:

        //  **************     aLin1
        //               o
        //               o
        //               ***************  aLin2

        ClearSolutions();
        Standard_Real aPar1 = 0.0, aPar2 = 0.0;
        aRange2.GetBounds(aPar1, aPar2);
        aPar2 = 0.5*(aPar1 + aPar2);
        gp_Pnt aP = ElCLib::Value(aPar2, aLin2);
        const Extrema_POnCurv aP2(aPar2, aP);
        aPar1 = ElCLib::Parameter(aLin1, aP);
        aP = ElCLib::Value(aPar1, aLin1);
        const Extrema_POnCurv aP1(aPar1, aP);
        mypoints.Append(aP1);
        mypoints.Append(aP2);
        mySqDist.Append(theSqDist);
      }
      else
      {
        //Case like this:

        //  **************     aLin1
        //                 o
        //                  o
        //                   ***********  aLin2
        // 
        //Take minimal trimmed distance
        Standard_Real aDmin, aDists[4] = {mydist11, mydist12, mydist21, mydist22};
        Extrema_POnCurv aP1, aP2;
        aDmin = aDists[0];
        Standard_Integer i, imin = 0;
        for (i = 1; i < 4; ++i)
        {
          if (aDmin > aDists[i])
          {
            aDmin = aDists[i];
            imin = i;
          }
        }
        if (imin == 0)
        {
          aP1.SetValues(myInf[0], myP1f);
          aP2.SetValues(myInf[1], myP2f);
        }
        else if (imin == 1)
        {
          aP1.SetValues(myInf[0], myP1f);
          aP2.SetValues(mySup[1], myP2l);
        }
        else if (imin == 2)
        {
          aP1.SetValues(mySup[0], myP1l);
          aP2.SetValues(myInf[1], myP2f);
        }
        else 
        {
          aP1.SetValues(mySup[0], myP1l);
          aP2.SetValues(mySup[1], myP2l);
        }
        ClearSolutions();
        mypoints.Append(aP1);
        mypoints.Append(aP2);
        mySqDist.Append(aDmin);
      }
    }   
  }
  else
  {
    // Circle - Circle
    myIsParallel = Standard_False;

    //Two arcs with ranges [U1, U2] and [V1, V2] correspondingly are
    //considered to be parallel in the following case:
    //  The range created by projection both points U1 and U2 of the
    //  1st circle to the 2nd one intersects either the range [V1, V2] or
    //  the range [V1-PI, V2-PI]. All ranges must be adjusted to correspond
    //  periodic range before checking of intersection.

    const gp_Circ aWorkCirc = myC[1]->Circle();
    const Standard_Real aPeriod = M_PI + M_PI;
    gp_Vec aVTg1;
    gp_Pnt aP11;
    const gp_Pnt aP12 = Extrema_CurveTool::Value (*myC[0], theUt12);
    Extrema_CurveTool::D1 (*myC[0], theUt11, aP11, aVTg1);

    const Bnd_Range aRange(theUt21, theUt22);
    Bnd_Range aProjRng1;

    // Precision of the calculation depends on circles radii
    const Standard_Real aPrecision = Max(Epsilon(myC[0]->Circle().Radius()), Epsilon(myC[1]->Circle().Radius()));

    // Project arc of the 1st circle between points theUt11 and theUt12 to the
    // 2nd circle. It is necessary to chose correct arc from two possible ones.

    Standard_Real aPar1 = ElCLib::InPeriod(ElCLib::Parameter(aWorkCirc, aP11),
                                           theUt21, theUt21 + aPeriod);
    const gp_Vec aVTg2 = Extrema_CurveTool::DN (*myC[1], aPar1, 1);
    
    // Check if circles have same/opposite directions
    const Standard_Boolean isOpposite(aVTg1.Dot(aVTg2) < 0.0);

    Standard_Real aPar2 = ElCLib::InPeriod(ElCLib::Parameter(aWorkCirc, aP12),
                                           theUt21, theUt21 + aPeriod);

    if (isOpposite)
    {
      // Must be aPar2 < aPar1
      if ((aRange.Delta() > Precision::Angular()) &&
          ((aPar1 - aPar2) < Precision::Angular()))
      {
        aPar2 -= aPeriod;
      }
    }
    else
    {
      // Must be aPar2 > aPar1
      if ((aRange.Delta() > Precision::Angular()) &&
          ((aPar2 - aPar1) < Precision::Angular()))
      {
        aPar2 += aPeriod;
      }
    }

    // Now the projection result is the range [aPar1, aPar2]
    // if aPar1 < aPar2 or the range [aPar2, aPar1], otherwise.

    Standard_Real aMinSquareDist = RealLast();

    aProjRng1.Add(aPar1 - aPeriod);
    aProjRng1.Add(aPar2 - aPeriod);
    for (Standard_Integer i = 0; i < 3; i++)
    {
      // Repeat computation three times, shifting the range to PI on each step,
      // to be able to find if the concentric arcs ranges are intersected in just one parameter
      // (lower or upper boundary).

      Bnd_Range aRng = aProjRng1;
      aRng.Common(aRange);

      //Cases are possible and processed below:
      //1. Extrema does not exist. In this case all common ranges are VOID.
      //2. Arcs are parallel and distance between them is equal to sqrt(theSqDist).
      //    In this case myIsParallel = TRUE definitely.
      //3. Arcs are parallel and distance between them is equal to (sqrt(theSqDist) + R),
      //    where R is the least radius of the both circles. In this case myIsParallel flag
      //    will temporary be set to TRUE but check will be continued until less
      //    distance will be found. At that, region with the least distance can be
      //    either a local point or continuous range. In 1st case myIsParallel = FALSE and
      //    several (or single) extremas will be returned. In the 2nd one
      //    myIsParallel = TRUE and only the least distance will be returned.
      //4. Arcs are not parallel. Then several (or single) extremas will be returned.

      if (aRng.Delta() > Precision::Confusion())
      {
        Standard_Real aPar = 0.0;
        aRng.GetIntermediatePoint(0.5, aPar);
        const gp_Pnt aPCirc2 = ElCLib::Value(aPar, aWorkCirc);
        Extrema_ExtPElC ExtPCir(aPCirc2,
                                Extrema_CurveTool::Circle (*myC[0]),
                                Precision::Confusion(), theUt11, theUt12);
        if (ExtPCir.NbExt() < 1)
        {
          continue;
        }
        Standard_Real aMinSqD = ExtPCir.SquareDistance(1);
        for (Standard_Integer anExtID = 2; anExtID <= ExtPCir.NbExt(); anExtID++)
        {
          aMinSqD = Min(aMinSqD, ExtPCir.SquareDistance(anExtID));
        }

        if (aMinSqD <= aMinSquareDist + (1. + aMinSqD) * aPrecision)
        {
          ClearSolutions();
          mySqDist.Append(aMinSqD);
          myIsParallel = Standard_True;

          const Standard_Real aDeltaSqDist = aMinSqD - theSqDist;
          const Standard_Real aSqD = Max(aMinSqD, theSqDist);

          //  0 <= Dist1-Dist2 <= Eps
          //  0 <= Dist1^2 - Dist2^2 < Eps*(Dist1+Dist2)

          //If Dist1 ~= Dist2 ==> Dist1+Dist2 ~= 2*Dist2.
          //Consequently,
          //  0 <= Dist1^2 - Dist2^2 <= 2*Dist2*Eps

          //Or
          //  (Dist1^2 - Dist2^2)^2 <= 4*Dist2^2*Eps^2

          if (aDeltaSqDist*aDeltaSqDist < 4.0*aSqD*Precision::SquareConfusion())
          {
            // New solution is found
            break;
          }
        }
        //Nearer solution can be found
      }
      else if (!aRng.IsVoid())
      {
        //Check cases like this:

        //  **************     aCirc1
        //               o
        //               o
        //               ***************  aCirc2

        Standard_Real aPar = 0.0;
        aRng.GetIntermediatePoint(0.5, aPar);
        const gp_Pnt aPCirc2 = ElCLib::Value(aPar, aWorkCirc);
        const Extrema_POnCurv aP2(aPar, aPCirc2);

        Extrema_ExtPElC ExtPCir(aPCirc2,
                                Extrema_CurveTool::Circle (*myC[0]),
                                Precision::Confusion(), theUt11, theUt12);

        Standard_Boolean isFound = !myIsParallel;

        if (!isFound)
        {
          //If the flag myIsParallel was set earlier then it does not mean that
          //we have found the minimal distance. Here we check it. If there is
          //a pair of points, which are in less distance then myIsParallel flag
          //was unset and the algorithm will return these nearest points.

          for (Standard_Integer anExtID = 1; anExtID <= ExtPCir.NbExt(); anExtID++)
          {
            if (ExtPCir.SquareDistance(anExtID) < aMinSquareDist)
            {
              isFound = Standard_True;
              break;
            }
          }
        }

        if (isFound)
        {
          ClearSolutions();
          myIsParallel = Standard_False;
          for (Standard_Integer anExtID = 1; anExtID <= ExtPCir.NbExt(); anExtID++)
          {
            mypoints.Append(ExtPCir.Point(anExtID));
            mypoints.Append(aP2);
            mySqDist.Append(ExtPCir.SquareDistance(anExtID));
            aMinSquareDist = Min(aMinSquareDist, ExtPCir.SquareDistance(anExtID));
          }
        }
      }
      else
      {
        //Case like this:

        //  **************     Cir1
        //                 o
        //                  o
        //                   ***********  Cir2
        // 
        //Take minimal trimmed distance
        myIsParallel = Standard_False;
        Standard_Real aDmin, aDists[4] = { mydist11, mydist12, mydist21, mydist22 };
        Extrema_POnCurv aP1, aP2;
        aDmin = aDists[0];
        Standard_Integer k, imin = 0;
        for (k = 1; k < 4; ++k)
        {
          if (aDmin > aDists[k])
          {
            aDmin = aDists[k];
            imin = k;
          }
        }
        if (aDmin <= aMinSquareDist + (1. + aDmin) * aPrecision)
        {
          if (imin == 0)
          {
            aP1.SetValues(myInf[0], myP1f);
            aP2.SetValues(myInf[1], myP2f);
          }
          else if (imin == 1)
          {
            aP1.SetValues(myInf[0], myP1f);
            aP2.SetValues(mySup[1], myP2l);
          }
          else if (imin == 2)
          {
            aP1.SetValues(mySup[0], myP1l);
            aP2.SetValues(myInf[1], myP2f);
          }
          else
          {
            aP1.SetValues(mySup[0], myP1l);
            aP2.SetValues(mySup[1], myP2l);
          }
          ClearSolutions();
          mypoints.Append(aP1);
          mypoints.Append(aP2);
          mySqDist.Append(aDmin);
          aMinSquareDist = Min(aMinSquareDist, aDmin);
        }
      }
      aProjRng1.Shift(M_PI);
    }
  }
}

//=======================================================================
//function : Results
//purpose  : 
//=======================================================================

void Extrema_ExtCC::PrepareResults(const Extrema_ExtElC&  AlgExt,
                                   const Standard_Boolean theIsInverse,
                                   const Standard_Real    Ut11,
                                   const Standard_Real    Ut12,
                                   const Standard_Real    Ut21,
                                   const Standard_Real    Ut22)
{
  Standard_Integer i, NbExt;
  Standard_Real Val, U, U2;
  Extrema_POnCurv P1, P2;

  myDone = AlgExt.IsDone();
  if (myDone) {
    myIsParallel = AlgExt.IsParallel();
    if (myIsParallel) {
      PrepareParallelResult(Ut11, Ut12, Ut21, Ut22, AlgExt.SquareDistance());
    }
    else {
      NbExt = AlgExt.NbExt();
      for (i = 1; i <= NbExt; i++) {
	// Verification de la validite des parametres
	AlgExt.Points(i, P1, P2);
        if (!theIsInverse)
        {
	  U = P1.Parameter();
	  U2 = P2.Parameter();
	}
	else {
	  U2 = P1.Parameter();
	  U = P2.Parameter();
	}

	if (Extrema_CurveTool::IsPeriodic (*myC[0]))
	{
	  U = ElCLib::InPeriod(U, Ut11, Ut11+Extrema_CurveTool::Period (*myC[0]));
	}
	if (Extrema_CurveTool::IsPeriodic (*myC[1]))
	{
	  U2 = ElCLib::InPeriod(U2, Ut21, Ut21+Extrema_CurveTool::Period (*myC[1]));
	}

	if ((U  >= Ut11 - RealEpsilon())  && 
	    (U  <= Ut12 + RealEpsilon())  &&
	    (U2 >= Ut21 - RealEpsilon())  &&
	    (U2 <= Ut22 + RealEpsilon())) {
	  Val = AlgExt.SquareDistance(i);
	  mySqDist.Append(Val);
          if (!theIsInverse)
          {
	    P1.SetValues(U, P1.Value());
	    P2.SetValues(U2, P2.Value());
	    mypoints.Append(P1);
	    mypoints.Append(P2);
	  }
	  else {
	    P1.SetValues(U2, P1.Value());
	    P2.SetValues(U, P2.Value());
	    mypoints.Append(P2);
	    mypoints.Append(P1);
	  }
	}
      }
    }
  }

}


//=======================================================================
//function : Results
//purpose  : 
//=======================================================================

void Extrema_ExtCC::PrepareResults(const Extrema_ECC&   AlgExt,
                                   const Standard_Real  Ut11,
                                   const Standard_Real  Ut12,
                                   const Standard_Real  Ut21,
                                   const Standard_Real  Ut22)
{
  Standard_Integer i, NbExt;
  Standard_Real Val, U, U2;
  Extrema_POnCurv P1, P2;

  myDone = AlgExt.IsDone();
  if (myDone)
  {
    myIsParallel = AlgExt.IsParallel();
    if (myIsParallel)
    {
      PrepareParallelResult(Ut11, Ut12, Ut21, Ut22, AlgExt.SquareDistance());
    }
    else
    {
      NbExt = AlgExt.NbExt();
      for (i = 1; i <= NbExt; i++)
      {
        AlgExt.Points(i, P1, P2);
        U = P1.Parameter();
        U2 = P2.Parameter();

        // Check points to be into param space.
        if (Extrema_CurveTool::IsPeriodic (*myC[0]))
        {
          U = ElCLib::InPeriod(U, Ut11, Ut11 + Extrema_CurveTool::Period (*myC[0]));
        }
        if (Extrema_CurveTool::IsPeriodic (*myC[1]))
        {
          U2 = ElCLib::InPeriod(U2, Ut21, Ut21 + Extrema_CurveTool::Period (*myC[1]));
        }

        if ((U >= Ut11 - RealEpsilon()) &&
            (U <= Ut12 + RealEpsilon()) &&
            (U2 >= Ut21 - RealEpsilon()) &&
            (U2 <= Ut22 + RealEpsilon()))
        {
          Val = AlgExt.SquareDistance(i);
          mySqDist.Append(Val);
          P1.SetValues(U, P1.Value());
          P2.SetValues(U2, P2.Value());
          mypoints.Append(P1);
          mypoints.Append(P2);
        }
      }
    }
  }
}

//=======================================================================
//function : SetSingleSolutionFlag
//purpose  : 
//=======================================================================
void Extrema_ExtCC::SetSingleSolutionFlag(const Standard_Boolean theFlag)
{
  myIsFindSingleSolution = theFlag;
}

//=======================================================================
//function : GetSingleSolutionFlag
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_ExtCC::GetSingleSolutionFlag() const
{
  return myIsFindSingleSolution;
}
