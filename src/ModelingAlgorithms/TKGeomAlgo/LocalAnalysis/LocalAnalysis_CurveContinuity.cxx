// Copyright (c) 1996-1999 Matra Datavision
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

#include <Geom_Curve.hxx>
#include <GeomProp_Curve.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <LocalAnalysis_CurveContinuity.hxx>
#include <LocalAnalysis_StatusErrorType.hxx>
#include <StdFail_NotDone.hxx>

/***********************************************************************/
void LocalAnalysis_CurveContinuity::CurvC0(const gp_Pnt& theP1, const gp_Pnt& theP2)
{
  myContC0 = theP1.Distance(theP2);
}

/****************************************************************************/
void LocalAnalysis_CurveContinuity::CurvC1(const gp_Vec& theD1_1, const gp_Vec& theD1_2)
{
  double ang;
  double norm1 = theD1_1.Magnitude();
  double norm2 = theD1_2.Magnitude();

  if ((norm1 > myepsnul) && (norm2 > myepsnul))
  {
    if (norm1 >= norm2)
    {
      myLambda1 = norm2 / norm1;
    }
    else
    {
      myLambda1 = norm1 / norm2;
    }
    ang = theD1_1.Angle(theD1_2);
    if (ang > M_PI / 2)
      myContC1 = M_PI - ang;
    else
      myContC1 = ang;
  }
  else
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_NullFirstDerivative;
  }
}

/*********************************************************************************/

void LocalAnalysis_CurveContinuity::CurvC2(const gp_Vec& theD1_1,
                                            const gp_Vec& theD1_2,
                                            const gp_Vec& theD2_1,
                                            const gp_Vec& theD2_2)
{
  double norm1  = theD1_1.Magnitude();
  double norm2  = theD1_2.Magnitude();
  double norm12 = theD2_1.Magnitude();
  double norm22 = theD2_2.Magnitude();

  if ((norm1 > myepsnul) && (norm2 > myepsnul))
  {
    if ((norm12 > myepsnul) && (norm22 > myepsnul))
    {
      if (norm1 >= norm2)
      {
        myLambda1 = norm2 / norm1;
        myLambda2 = norm22 / norm12;
      }

      else
      {
        myLambda1 = norm1 / norm2;
        myLambda2 = norm12 / norm22;
      }
      double ang = theD2_1.Angle(theD2_2);
      if (ang > M_PI / 2)
        myContC2 = M_PI - ang;
      else
        myContC2 = ang;
    }

    else
    {
      myIsDone      = false;
      myErrorStatus = LocalAnalysis_NullSecondDerivative;
    }
  }

  else
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_NullFirstDerivative;
  }
}

/*********************************************************************************/

void LocalAnalysis_CurveContinuity::CurvG1(const gp_Dir& theTang1,
                                            bool          theTangDef1,
                                            const gp_Dir& theTang2,
                                            bool          theTangDef2)
{
  if (theTangDef1 && theTangDef2)
  {
    double ang = theTang1.Angle(theTang2);
    if (ang > M_PI / 2)
      myContG1 = M_PI - ang;
    else
      myContG1 = ang;
  }
  else
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_TangentNotDefined;
  }
}

/*********************************************************************************/

void LocalAnalysis_CurveContinuity::CurvG2(const gp_Dir& theTang1,
                                            bool          theTangDef1,
                                            double        theCurv1,
                                            bool          theCurvDef1,
                                            const gp_Dir& theNorm1,
                                            bool          theNormDef1,
                                            const gp_Vec& theD1_1,
                                            const gp_Dir& theTang2,
                                            bool          theTangDef2,
                                            double        theCurv2,
                                            bool          theCurvDef2,
                                            const gp_Dir& theNorm2,
                                            bool          theNormDef2,
                                            const gp_Vec& theD1_2)
{
  (void)theD1_1;
  (void)theD1_2;
  double epscrb = 8 * myepsC0 / (myMaxLon * myMaxLon);

  if (theTangDef1 && theTangDef2)
  {
    myCourbC1 = theCurvDef1 ? theCurv1 : 0.0;
    myCourbC2 = theCurvDef2 ? theCurv2 : 0.0;
    if ((std::abs(myCourbC1) > epscrb) && (std::abs(myCourbC2) > epscrb))
    {
      if (theNormDef1 && theNormDef2)
      {
        double ang = theNorm1.Angle(theNorm2);
        if (ang > M_PI / 2)
          myContG2 = M_PI - ang;
        else
          myContG2 = ang;
        myG2Variation = std::abs(myCourbC1 - myCourbC2) / sqrt(myCourbC1 * myCourbC2);
      }
      else
      {
        myIsDone      = false;
        myErrorStatus = LocalAnalysis_NormalNotDefined;
      }
    }
    else
    {
      myIsDone      = false;
      myErrorStatus = LocalAnalysis_NormalNotDefined;
    }
  }
  else
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_TangentNotDefined;
  }
}

/*********************************************************************************/

LocalAnalysis_CurveContinuity::LocalAnalysis_CurveContinuity(const occ::handle<Geom_Curve>& Curv1,
                                                             const double                   u1,
                                                             const occ::handle<Geom_Curve>& Curv2,
                                                             const double                   u2,
                                                             const GeomAbs_Shape            Order,
                                                             const double                   Epsnul,
                                                             const double                   EpsC0,
                                                             const double                   EpsC1,
                                                             const double                   EpsC2,
                                                             const double                   EpsG1,
                                                             const double                   EpsG2,
                                                             const double                   Percent,
                                                             const double                   Maxlen)
    : myContC0(0.0),
      myContC1(0.0),
      myContC2(0.0),
      myContG1(0.0),
      myContG2(0.0),
      myCourbC1(0.0),
      myCourbC2(0.0),
      myG2Variation(0.0),
      myLambda1(0.0),
      myLambda2(0.0)
{
  myTypeCont = Order;
  myepsnul   = Epsnul;
  myMaxLon   = Maxlen;
  myepsC0    = EpsC0;
  myepsC1    = EpsC1;
  myepsC2    = EpsC2;
  myepsG1    = EpsG1;
  myepsG2    = EpsG2;
  myperce    = Percent;

  myIsDone = true;

  GeomProp_Curve aProp1(Curv1);
  GeomProp_Curve aProp2(Curv2);

  switch (Order)
  {
    case GeomAbs_C0: {
      gp_Pnt P1, P2;
      Curv1->D0(u1, P1);
      Curv2->D0(u2, P2);
      CurvC0(P1, P2);
    }
    break;
    case GeomAbs_C1: {
      gp_Pnt P1, P2;
      gp_Vec D1_1, D1_2;
      Curv1->D1(u1, P1, D1_1);
      Curv2->D1(u2, P2, D1_2);
      CurvC0(P1, P2);
      CurvC1(D1_1, D1_2);
    }
    break;
    case GeomAbs_C2: {
      gp_Pnt P1, P2;
      gp_Vec D1_1, D1_2, D2_1, D2_2;
      Curv1->D2(u1, P1, D1_1, D2_1);
      Curv2->D2(u2, P2, D1_2, D2_2);
      CurvC0(P1, P2);
      CurvC1(D1_1, D1_2);
      CurvC2(D1_1, D1_2, D2_1, D2_2);
    }
    break;
    case GeomAbs_G1: {
      gp_Pnt P1, P2;
      Curv1->D0(u1, P1);
      Curv2->D0(u2, P2);
      CurvC0(P1, P2);
      GeomProp::TangentResult aTang1 = aProp1.Tangent(u1, myepsnul);
      GeomProp::TangentResult aTang2 = aProp2.Tangent(u2, myepsnul);
      CurvG1(aTang1.Direction, aTang1.IsDefined, aTang2.Direction, aTang2.IsDefined);
    }
    break;
    case GeomAbs_G2: {
      gp_Pnt P1, P2;
      gp_Vec D1_1, D1_2;
      Curv1->D1(u1, P1, D1_1);
      Curv2->D1(u2, P2, D1_2);
      CurvC0(P1, P2);
      GeomProp::TangentResult   aTang1 = aProp1.Tangent(u1, myepsnul);
      GeomProp::TangentResult   aTang2 = aProp2.Tangent(u2, myepsnul);
      GeomProp::CurvatureResult aCurv1 = aProp1.Curvature(u1, myepsnul);
      GeomProp::CurvatureResult aCurv2 = aProp2.Curvature(u2, myepsnul);
      GeomProp::NormalResult    aNorm1 = aProp1.Normal(u1, myepsnul);
      GeomProp::NormalResult    aNorm2 = aProp2.Normal(u2, myepsnul);
      CurvG1(aTang1.Direction, aTang1.IsDefined, aTang2.Direction, aTang2.IsDefined);
      CurvG2(aTang1.Direction,
             aTang1.IsDefined,
             aCurv1.Value,
             aCurv1.IsDefined,
             aNorm1.Direction,
             aNorm1.IsDefined,
             D1_1,
             aTang2.Direction,
             aTang2.IsDefined,
             aCurv2.Value,
             aCurv2.IsDefined,
             aNorm2.Direction,
             aNorm2.IsDefined,
             D1_2);
    }
    break;
    default: {
    }
  }
}

/*********************************************************************************/

bool LocalAnalysis_CurveContinuity::IsC0() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return myContC0 <= myepsC0;
}

/*********************************************************************************/

bool LocalAnalysis_CurveContinuity::IsC1() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return IsC0() && ((myContC1 <= myepsC1) || (std::abs(myContC1 - M_PI) <= myepsC1));
}

/*********************************************************************************/

bool LocalAnalysis_CurveContinuity::IsC2() const
{
  double epsil1, epsil2;

  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  if (IsC1())
  {
    if ((myContC2 <= myepsC2) || (std::abs(myContC2 - M_PI) <= myepsC2))
    {
      epsil1 = 0.5 * myepsC1 * myepsC1 * myLambda1;
      epsil2 = 0.5 * myepsC2 * myepsC2 * myLambda2;
      if ((std::abs(myLambda1 * myLambda1 - myLambda2)) <= (epsil1 * epsil1 + epsil2))
        return true;
    }
    else
      return false;
  }
  return false;
}

/*********************************************************************************/

bool LocalAnalysis_CurveContinuity::IsG1() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return IsC0() && ((myContG1 <= myepsG1 || (std::abs(myContG1 - M_PI) <= myepsG1)));
}

/*********************************************************************************/

bool LocalAnalysis_CurveContinuity::IsG2() const
{
  double CRBINF, CRBNUL;
  int    IETA1, IETA2;
  // etat des coubures IETA. -> 0 Crbure nulle
  //			   -> 1 Crbure finie
  //			   -> 2 Crbure infinie

  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  if (IsG1())
  {
    CRBINF = 1 / myepsC0;
    CRBNUL = 8 * myepsC0 / (myMaxLon * myMaxLon);

    if (myCourbC1 > CRBINF)
      IETA1 = 2;
    else if (myCourbC1 < CRBNUL)
      IETA1 = 0;
    else
      IETA1 = 1;
    if (myCourbC2 > CRBINF)
      IETA2 = 2;
    else if (myCourbC2 < CRBNUL)
      IETA2 = 0;
    else
      IETA2 = 1;
    if (IETA1 == IETA2)
    {
      if (IETA1 == 1)
      {
        double eps = RealPart((myContG2 + myepsG2) / M_PI) * M_PI;
        if (std::abs(eps - myepsG2) < myepsG2)
        {
          return myG2Variation < myperce;
        }
        else
          return false;
      }
      else
        return true;
    }
    else
      return false;
  }
  else
    return false;
}

/*********************************************************************************/

double LocalAnalysis_CurveContinuity::C0Value() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myContC0);
}

/*********************************************************************************/

double LocalAnalysis_CurveContinuity::C1Angle() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myContC1);
}

/*********************************************************************************/

double LocalAnalysis_CurveContinuity::C2Angle() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myContC2);
}

/*********************************************************************************/

double LocalAnalysis_CurveContinuity::G1Angle() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myContG1);
}

/*********************************************************************************/

double LocalAnalysis_CurveContinuity::G2Angle() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myContG2);
}

/*********************************************************************************/

double LocalAnalysis_CurveContinuity::C1Ratio() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myLambda1);
}

/*********************************************************************************/

double LocalAnalysis_CurveContinuity::C2Ratio() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myLambda2);
}

/********************************************************************************/
double LocalAnalysis_CurveContinuity::G2CurvatureVariation() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myG2Variation);
}

/********************************************************************************/

bool LocalAnalysis_CurveContinuity::IsDone() const
{
  return (myIsDone);
}

/*********************************************************************************/

LocalAnalysis_StatusErrorType LocalAnalysis_CurveContinuity::StatusError() const
{
  return myErrorStatus;
}

/*************************************************************************/
GeomAbs_Shape LocalAnalysis_CurveContinuity::ContinuityStatus() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myTypeCont);
}

/*********************************************************************************/
