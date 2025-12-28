// Created on: 1999-04-14
// Created by: Roman LYGIN
// Copyright (c) 1999-1999 Matra Datavision
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

#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade.hxx>
#include <ShapeUpgrade_SplitCurve2dContinuity.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_SplitCurve2dContinuity, ShapeUpgrade_SplitCurve2d)

//=================================================================================================

ShapeUpgrade_SplitCurve2dContinuity::ShapeUpgrade_SplitCurve2dContinuity()
{
  myCriterion = GeomAbs_C1;
  myTolerance = Precision::PConfusion();
  myCont      = 1;
}

//=================================================================================================

void ShapeUpgrade_SplitCurve2dContinuity::SetCriterion(const GeomAbs_Shape Criterion)
{
  myCriterion = Criterion;
  switch (myCriterion)
  {
    case GeomAbs_C0:
      myCont = 0;
      break;
    case GeomAbs_C1:
      myCont = 1;
      break;
    case GeomAbs_C2:
      myCont = 2;
      break;
    case GeomAbs_C3:
      myCont = 3;
      break;
    case GeomAbs_CN:
      myCont = 4;
      break;
    default:
      myCont = 1;
  }
}

//=================================================================================================

void ShapeUpgrade_SplitCurve2dContinuity::SetTolerance(const double Tol)
{
  myTolerance = Tol;
}

//=================================================================================================

void ShapeUpgrade_SplitCurve2dContinuity::Compute()
{
  if (myCurve->Continuity() < myCriterion)
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
  if (mySplitValues->Length() > 2)
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  constexpr double precision = Precision::PConfusion();
  double           First     = mySplitValues->Value(1);
  double           Last      = mySplitValues->Value(mySplitValues->Length());
  if (myCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    occ::handle<Geom2d_TrimmedCurve>    tmp      = occ::down_cast<Geom2d_TrimmedCurve>(myCurve);
    occ::handle<Geom2d_Curve>           BasCurve = tmp->BasisCurve();
    ShapeUpgrade_SplitCurve2dContinuity spc;
    //    spc.Init(BasCurve,Max(First,tmp->FirstParameter()),Min(Last,tmp->LastParameter()));
    spc.Init(BasCurve, First, Last);
    spc.SetSplitValues(mySplitValues);
    spc.SetTolerance(myTolerance);
    spc.SetCriterion(myCriterion);
    spc.Compute();
    mySplitValues->Clear();
    mySplitValues->ChangeSequence() = spc.SplitValues()->Sequence();
    // mySplitValues = spc.SplitValues();
    myStatus |= spc.myStatus;
    return;
  }
  else if (myCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
  {
    GeomAbs_Shape BasCriterion;
    switch (myCriterion)
    {
      default:
      case GeomAbs_C1:
        BasCriterion = GeomAbs_C2;
        break;
      case GeomAbs_C2:
        BasCriterion = GeomAbs_C3;
        break;
      case GeomAbs_C3: // if (ShapeUpgrade::Debug()) std::cout<<". this criterion is not suitable
                       // for a Offset curve"<<std::endl;
#ifdef OCCT_DEBUG
        std::cout << "Warning: ShapeUpgrade_SplitCurve2dContinuity: criterion C3 for Offset curve"
                  << std::endl;
#endif
      case GeomAbs_CN:
        BasCriterion = GeomAbs_CN;
        break;
    }
    occ::handle<Geom2d_OffsetCurve> tmp      = occ::down_cast<Geom2d_OffsetCurve>(myCurve);
    occ::handle<Geom2d_Curve>       BasCurve = tmp->BasisCurve();
    // double Offset = tmp->Offset(); // Offset not used (skl)
    ShapeUpgrade_SplitCurve2dContinuity spc;
    //    spc.Init(BasCurve,Max(tmp->FirstParameter(),First),Min(tmp->LastParameter(),Last));
    spc.Init(BasCurve, First, Last);
    spc.SetSplitValues(mySplitValues);
    spc.SetTolerance(myTolerance);
    spc.SetCriterion(BasCriterion);
    spc.Compute();
    mySplitValues->Clear();
    mySplitValues->ChangeSequence() = spc.SplitValues()->Sequence();
    myStatus |= spc.myStatus;
    return;
  }

  occ::handle<Geom2d_BSplineCurve> MyBSpline = occ::down_cast<Geom2d_BSplineCurve>(myCurve);
  if (MyBSpline.IsNull())
  {
    //    if (ShapeUpgrade::Debug()) std::cout<<". curve is not a Bspline"<<std::endl;
    return;
  }

  myNbCurves  = 1;
  int Deg     = MyBSpline->Degree();
  int NbKnots = MyBSpline->NbKnots();
  //  if (ShapeUpgrade::Debug()) std::cout<<". NbKnots="<<NbKnots<<std::endl;
  if (NbKnots <= 2)
  {
    return;
  }
  int FirstInd = MyBSpline->FirstUKnotIndex() + 1, LastInd = MyBSpline->LastUKnotIndex() - 1;
  int iknot = FirstInd;
  for (int j = 2; j <= mySplitValues->Length(); j++)
  {
    Last = mySplitValues->Value(j);
    for (; iknot <= LastInd; iknot++)
    {
      double valknot = MyBSpline->Knot(iknot);
      if (valknot <= First + precision)
        continue;
      if (valknot >= Last - precision)
        break;
      int Continuity = Deg - MyBSpline->Multiplicity(iknot);
      // double tt = MyBSpline->Knot(iknot); // tt not used (skl)
      if (Continuity < myCont)
      {
        // At this knot, the curve is C0; try to remove Knot.
        bool corrected       = false;
        int  newMultiplicity = Deg - myCont;
        if (newMultiplicity < 0)
          newMultiplicity = 0;
        {
          try
          {
            OCC_CATCH_SIGNALS
            corrected = MyBSpline->RemoveKnot(iknot, newMultiplicity, myTolerance);
          }
          catch (Standard_Failure const&)
          {
            corrected = false;
          }
        }
        if (corrected && newMultiplicity > 0)
        {
          Continuity = Deg - MyBSpline->Multiplicity(iknot);
          corrected  = (Continuity >= myCont);
        }
        if (corrected)
        {
          // at this knot, the continuity is now C1. Nothing else to do.
          //	  if (ShapeUpgrade::Debug()) std::cout<<". Correction at Knot "<<iknot<<std::endl;
          myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
        }
        else
        {
          // impossible to force C1 within the tolerance:
          // this knot will be a splitting value.
          mySplitValues->InsertBefore(j++, MyBSpline->Knot(iknot));
          myNbCurves++;
          //	  if (ShapeUpgrade::Debug()) std::cout<<". Splitting at Knot "<<iknot<<std::endl;
        }
      }
    }

    First = Last;
  }

  if (mySplitValues->Length() > 2)
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
}
