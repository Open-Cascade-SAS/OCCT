// Created on: 1993-10-20
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BSplCLib.hxx>
#include <Convert_CompBezierCurvesToBSplineCurve.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TColgp_HArray1OfPnt.hxx>

//=================================================================================================

Convert_CompBezierCurvesToBSplineCurve::Convert_CompBezierCurvesToBSplineCurve(
  const Standard_Real AngularTolerance)
    : myDegree(0),
      myAngular(AngularTolerance),
      myDone(Standard_False)
{
}

//=================================================================================================

void Convert_CompBezierCurvesToBSplineCurve::AddCurve(const TColgp_Array1OfPnt& Poles)
{
  if (!mySequence.IsEmpty())
  {
    gp_Pnt P1, P2;
    P1 = mySequence.Last()->Value(mySequence.Last()->Upper());
    P2 = Poles(Poles.Lower());

#ifdef OCCT_DEBUG
    if (!P1.IsEqual(P2, Precision::Confusion()))
      std::cout << "Convert_CompBezierCurvesToBSplineCurve::Addcurve" << std::endl;
#endif
  }
  myDone                             = Standard_False;
  Handle(TColgp_HArray1OfPnt) HPoles = new TColgp_HArray1OfPnt(Poles.Lower(), Poles.Upper());
  HPoles->ChangeArray1()             = Poles;
  mySequence.Append(HPoles);
}

//=================================================================================================

Standard_Integer Convert_CompBezierCurvesToBSplineCurve::Degree() const
{
  return myDegree;
}

//=================================================================================================

Standard_Integer Convert_CompBezierCurvesToBSplineCurve::NbPoles() const
{
  return CurvePoles.Length();
}

//=================================================================================================

void Convert_CompBezierCurvesToBSplineCurve::Poles(TColgp_Array1OfPnt& Poles) const
{
  Standard_Integer i, Lower = Poles.Lower(), Upper = Poles.Upper();
  Standard_Integer k = 1;
  for (i = Lower; i <= Upper; i++)
  {
    Poles(i) = CurvePoles(k++);
  }
}

//=================================================================================================

Standard_Integer Convert_CompBezierCurvesToBSplineCurve::NbKnots() const
{
  return CurveKnots.Length();
}

//=================================================================================================

void Convert_CompBezierCurvesToBSplineCurve::KnotsAndMults(TColStd_Array1OfReal&    Knots,
                                                           TColStd_Array1OfInteger& Mults) const
{
  Standard_Integer i, LowerK = Knots.Lower(), UpperK = Knots.Upper();
  Standard_Integer LowerM = Mults.Lower(), UpperM = Mults.Upper();
  Standard_Integer k = 1;
  for (i = LowerK; i <= UpperK; i++)
  {
    Knots(i) = CurveKnots(k++);
  }
  k = 1;
  for (i = LowerM; i <= UpperM; i++)
  {
    Mults(i) = KnotsMultiplicities(k++);
  }
}

//=================================================================================================

void Convert_CompBezierCurvesToBSplineCurve::Perform()
{
  myDone = Standard_True;
  CurvePoles.Clear();
  CurveKnots.Clear();
  KnotsMultiplicities.Clear();
  Standard_Integer LowerI  = 1;
  Standard_Integer UpperI  = mySequence.Length();
  Standard_Integer NbrCurv = UpperI - LowerI + 1;
  //  Standard_Integer NbKnotsSpl = NbrCurv + 1 ;
  TColStd_Array1OfReal CurveKnVals(1, NbrCurv);

  Standard_Integer i;
  myDegree = 0;
  for (i = 1; i <= mySequence.Length(); i++)
  {
    myDegree = Max(myDegree, (mySequence(i))->Length() - 1);
  }

  Standard_Real      Det = 0;
  gp_Pnt             P1, P2, P3;
  Standard_Integer   Deg, Inc, MaxDegree = myDegree;
  TColgp_Array1OfPnt Points(1, myDegree + 1);

  for (i = LowerI; i <= UpperI; i++)
  {
    // 1- Raise the Bezier curve to the maximum degree.
    Deg = mySequence(i)->Length() - 1;
    Inc = myDegree - Deg;
    if (Inc > 0)
    {
      BSplCLib::IncreaseDegree(myDegree,
                               mySequence(i)->Array1(),
                               BSplCLib::NoWeights(),
                               Points,
                               BSplCLib::NoWeights());
    }
    else
    {
      Points = mySequence(i)->Array1();
    }

    // 2- Process the node of junction between 2 Bezier curves.
    if (i == LowerI)
    {
      // Processing of the initial node of the BSpline.
      for (Standard_Integer j = 1; j <= MaxDegree; j++)
      {
        CurvePoles.Append(Points(j));
      }
      CurveKnVals(1) = 1.; // To begin the series.
      KnotsMultiplicities.Append(MaxDegree + 1);
      Det = 1.;
    }

    if (i != LowerI)
    {
      P2 = Points(1);
      P3 = Points(2);
      gp_Vec V1(P1, P2), V2(P2, P3);

      // Processing of the tangency between Bezier and the previous.
      // This allows to guarantee at least a C1 continuity if the tangents are
      // coherent.

      Standard_Real D1 = V1.SquareMagnitude();
      Standard_Real D2 = V2.SquareMagnitude();
      if (MaxDegree > 1 && // rln 20.06.99 work-around
          D1 > gp::Resolution() && D2 > gp::Resolution() && V1.IsParallel(V2, myAngular))
      {
        Standard_Real Lambda = Sqrt(D2 / D1);
        if (CurveKnVals(i - 1) * Lambda > 10. * Epsilon(Det))
        {
          KnotsMultiplicities.Append(MaxDegree - 1);
          CurveKnVals(i) = CurveKnVals(i - 1) * Lambda;
        }
        else
        {
          CurvePoles.Append(Points(1));
          KnotsMultiplicities.Append(MaxDegree);
          CurveKnVals(i) = 1.0;
        }
      }
      else
      {
        CurvePoles.Append(Points(1));
        KnotsMultiplicities.Append(MaxDegree);
        CurveKnVals(i) = 1.0;
      }
      Det += CurveKnVals(i);

      // Store the poles.
      for (Standard_Integer j = 2; j <= MaxDegree; j++)
      {
        CurvePoles.Append(Points(j));
      }
    }

    if (i == UpperI)
    {
      // Processing of the end node of the BSpline.
      CurvePoles.Append(Points(MaxDegree + 1));
      KnotsMultiplicities.Append(MaxDegree + 1);
    }
    P1 = Points(MaxDegree);
  }

  // Correct nodal values to make them variable within [0.,1.].
  CurveKnots.Append(0.0);
  //  std::cout << "Convert : Det = " << Det << std::endl;
  for (i = 2; i <= NbrCurv; i++)
  {
    CurveKnots.Append(CurveKnots(i - 1) + (CurveKnVals(i - 1) / Det));
  }
  CurveKnots.Append(1.0);
}
