// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
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

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Precision.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>

//=================================================================================================

TopOpeBRepDS_Curve::TopOpeBRepDS_Curve()
    : myFirst(0.0),
      myLast(0.0),
      myRangeDefined(false),
      myTolerance(Precision::Confusion()),
      myIsWalk(false),
      myKeep(true),
      myMother(0),
      myDSIndex(0)
{
}

//=================================================================================================

TopOpeBRepDS_Curve::TopOpeBRepDS_Curve(const occ::handle<Geom_Curve>& C,
                                       const double                   T,
                                       const bool                     IsWalk)
    : myFirst(0.0),
      myLast(0.0),
      myRangeDefined(false),
      myKeep(true),
      myMother(0),
      myDSIndex(0)
{
  DefineCurve(C, T, IsWalk);
}

//=================================================================================================

void TopOpeBRepDS_Curve::DefineCurve(const occ::handle<Geom_Curve>& C,
                                     const double                   T,
                                     const bool                     IsWalk)
{
  myCurve     = C;
  myTolerance = T;
  myIsWalk    = IsWalk;
}

//=================================================================================================

void TopOpeBRepDS_Curve::Tolerance(const double T)
{
  myTolerance = T;
}

//=================================================================================================

void TopOpeBRepDS_Curve::SetSCI(const occ::handle<TopOpeBRepDS_Interference>& SCI1,
                                const occ::handle<TopOpeBRepDS_Interference>& SCI2)
{
  mySCI1 = SCI1;
  mySCI2 = SCI2;
}

//=================================================================================================

void TopOpeBRepDS_Curve::GetSCI(occ::handle<TopOpeBRepDS_Interference>& SCI1,
                                occ::handle<TopOpeBRepDS_Interference>& SCI2) const
{
  SCI1 = mySCI1;
  SCI2 = mySCI2;
}

//=================================================================================================

const occ::handle<TopOpeBRepDS_Interference>& TopOpeBRepDS_Curve::GetSCI1() const
{
  return mySCI1;
}

//=================================================================================================

const occ::handle<TopOpeBRepDS_Interference>& TopOpeBRepDS_Curve::GetSCI2() const
{
  return mySCI2;
}

//=================================================================================================

void TopOpeBRepDS_Curve::SetShapes(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  myS1 = S1;
  myS2 = S2;
}

//=================================================================================================

void TopOpeBRepDS_Curve::GetShapes(TopoDS_Shape& S1, TopoDS_Shape& S2) const
{
  S1 = myS1;
  S2 = myS2;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepDS_Curve::Shape1() const
{
  return myS1;
}

//=================================================================================================

TopoDS_Shape& TopOpeBRepDS_Curve::ChangeShape1()
{
  return myS1;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepDS_Curve::Shape2() const
{
  return myS2;
}

//=================================================================================================

TopoDS_Shape& TopOpeBRepDS_Curve::ChangeShape2()
{
  return myS2;
}

//=================================================================================================

occ::handle<Geom_Curve>& TopOpeBRepDS_Curve::ChangeCurve()
{
  return myCurve;
}

//=================================================================================================

const occ::handle<Geom_Curve>& TopOpeBRepDS_Curve::Curve() const
{
  return myCurve;
}

//=================================================================================================

void TopOpeBRepDS_Curve::SetRange(const double First, const double Last)
{
  myFirst        = First;
  myLast         = Last;
  myRangeDefined = true;
}

//=================================================================================================

bool TopOpeBRepDS_Curve::Range(double& First, double& Last) const
{
  if (myRangeDefined)
  {
    First = myFirst;
    Last  = myLast;
  }
  return myRangeDefined;
}

//=================================================================================================

double TopOpeBRepDS_Curve::Tolerance() const
{
  return myTolerance;
}

//=================================================================================================

void TopOpeBRepDS_Curve::Curve(const occ::handle<Geom_Curve>& C3D, const double Tol)
{
  myCurve     = C3D;
  myTolerance = Tol;
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& TopOpeBRepDS_Curve::Curve1() const
{
  if (!mySCI1.IsNull())
  {
    return occ::down_cast<TopOpeBRepDS_SurfaceCurveInterference>(mySCI1)->PCurve();
  }
  else
  {
    static occ::handle<Geom2d_Curve> STALOC_Geom2dCurveNull1;
    return STALOC_Geom2dCurveNull1;
  }
}

//=================================================================================================

void TopOpeBRepDS_Curve::Curve1(const occ::handle<Geom2d_Curve>& PC1)
{
  if (!mySCI1.IsNull())
  {
    occ::down_cast<TopOpeBRepDS_SurfaceCurveInterference>(mySCI1)->PCurve(PC1);
  }
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& TopOpeBRepDS_Curve::Curve2() const
{
  if (!mySCI2.IsNull())
  {
    return occ::down_cast<TopOpeBRepDS_SurfaceCurveInterference>(mySCI2)->PCurve();
  }
  else
  {
    static occ::handle<Geom2d_Curve> STALOC_Geom2dCurveNull2;
    return STALOC_Geom2dCurveNull2;
  }
}

//=================================================================================================

void TopOpeBRepDS_Curve::Curve2(const occ::handle<Geom2d_Curve>& PC2)
{
  if (!mySCI2.IsNull())
  {
    occ::down_cast<TopOpeBRepDS_SurfaceCurveInterference>(mySCI2)->PCurve(PC2);
  }
}

//=================================================================================================

bool TopOpeBRepDS_Curve::IsWalk() const
{
  return myIsWalk;
}

//=================================================================================================

void TopOpeBRepDS_Curve::ChangeIsWalk(const bool B)
{
  myIsWalk = B;
}

//=================================================================================================

bool TopOpeBRepDS_Curve::Keep() const
{
  return myKeep;
}

//=================================================================================================

void TopOpeBRepDS_Curve::ChangeKeep(const bool b)
{
  myKeep = b;
}

//=================================================================================================

int TopOpeBRepDS_Curve::Mother() const
{
  return myMother;
}

//=================================================================================================

void TopOpeBRepDS_Curve::ChangeMother(const int b)
{
  myMother = b;
}

//=================================================================================================

int TopOpeBRepDS_Curve::DSIndex() const
{
  return myDSIndex;
}

//=================================================================================================

void TopOpeBRepDS_Curve::ChangeDSIndex(const int b)
{
  myDSIndex = b;
}
