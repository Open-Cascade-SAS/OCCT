// Created on: 1999-04-06
// Created by: Fabrice SERVANT
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

#include <IntPolyh_StartPoint.hxx>
#include <IntPolyh_Triangle.hxx>

#include <stdio.h>
// #include <Precision.hxx>
#define MyConfusionPrecision 10.0e-12

IntPolyh_StartPoint::IntPolyh_StartPoint()
    : x(0),
      y(0),
      z(0),
      u1(0),
      v1(0),
      u2(0),
      v2(0),
      lambda1(-1.0),
      lambda2(-1.0),
      angle(-2.0),
      t1(-1),
      e1(-2),
      t2(-1),
      e2(-2),
      chainlist(-1)
{
}

IntPolyh_StartPoint::IntPolyh_StartPoint(const double _x,
                                         const double _y,
                                         const double _z,
                                         const double _u1,
                                         const double _v1,
                                         const double _u2,
                                         const double _v2,
                                         const int    _t1,
                                         const int    _e1,
                                         const double _lambda1,
                                         const int    _t2,
                                         const int    _e2,
                                         const double _lambda2,
                                         const int    _chainlist)
    : angle(-2.0)
{
  x         = _x;
  y         = _y;
  z         = _z;
  u1        = _u1;
  v1        = _v1;
  u2        = _u2;
  v2        = _v2;
  t1        = _t1;
  e1        = _e1;
  lambda1   = _lambda1;
  t2        = _t2;
  e2        = _e2;
  lambda2   = _lambda2;
  chainlist = _chainlist;
}

double IntPolyh_StartPoint::X() const
{
  return (x);
}

double IntPolyh_StartPoint::Y() const
{
  return (y);
}

double IntPolyh_StartPoint::Z() const
{
  return (z);
}

double IntPolyh_StartPoint::U1() const
{
  return (u1);
}

double IntPolyh_StartPoint::V1() const
{
  return (v1);
}

double IntPolyh_StartPoint::U2() const
{
  return (u2);
}

double IntPolyh_StartPoint::V2() const
{
  return (v2);
}

int IntPolyh_StartPoint::T1() const
{
  return (t1);
}

int IntPolyh_StartPoint::E1() const
{
  return (e1);
}

double IntPolyh_StartPoint::Lambda1() const
{
  return (lambda1);
}

int IntPolyh_StartPoint::T2() const
{
  return (t2);
}

int IntPolyh_StartPoint::E2() const
{
  return (e2);
}

double IntPolyh_StartPoint::Lambda2() const
{
  return (lambda2);
}

int IntPolyh_StartPoint::ChainList() const
{
  return (chainlist);
}

double IntPolyh_StartPoint::GetAngle() const
{
  return (angle);
}

int IntPolyh_StartPoint::GetEdgePoints(const IntPolyh_Triangle& Triangle,
                                       int&                     FirstEdgePoint,
                                       int&                     SecondEdgePoint,
                                       int&                     LastPoint) const
{
  int SurfID;
  if (e1 != -1)
  {
    if (e1 == 1)
    {
      FirstEdgePoint  = Triangle.FirstPoint();
      SecondEdgePoint = Triangle.SecondPoint();
      LastPoint       = Triangle.ThirdPoint();
    }
    else if (e1 == 2)
    {
      FirstEdgePoint  = Triangle.SecondPoint();
      SecondEdgePoint = Triangle.ThirdPoint();
      LastPoint       = Triangle.FirstPoint();
    }
    else if (e1 == 3)
    {
      FirstEdgePoint  = Triangle.ThirdPoint();
      SecondEdgePoint = Triangle.FirstPoint();
      LastPoint       = Triangle.SecondPoint();
    }
    SurfID = 1;
  }
  else if (e2 != -1)
  {
    if (e2 == 1)
    {
      FirstEdgePoint  = Triangle.FirstPoint();
      SecondEdgePoint = Triangle.SecondPoint();
      LastPoint       = Triangle.ThirdPoint();
    }
    else if (e2 == 2)
    {
      FirstEdgePoint  = Triangle.SecondPoint();
      SecondEdgePoint = Triangle.ThirdPoint();
      LastPoint       = Triangle.FirstPoint();
    }
    else if (e2 == 3)
    {
      FirstEdgePoint  = Triangle.ThirdPoint();
      SecondEdgePoint = Triangle.FirstPoint();
      LastPoint       = Triangle.SecondPoint();
    }
    SurfID = 2;
  }
  else
    SurfID = 0;
  return (SurfID);
}

void IntPolyh_StartPoint::SetXYZ(const double XX, const double YY, const double ZZ)
{
  x = XX;
  y = YY;
  z = ZZ;
}

void IntPolyh_StartPoint::SetUV1(const double UU1, const double VV1)
{
  u1 = UU1;
  v1 = VV1;
}

void IntPolyh_StartPoint::SetUV2(const double UU2, const double VV2)
{
  u2 = UU2;
  v2 = VV2;
}

void IntPolyh_StartPoint::SetEdge1(const int IE1)
{
  e1 = IE1;
}

void IntPolyh_StartPoint::SetLambda1(const double LAM1)
{
  lambda1 = LAM1;
}

void IntPolyh_StartPoint::SetEdge2(const int IE2)
{
  e2 = IE2;
}

void IntPolyh_StartPoint::SetLambda2(const double LAM2)
{
  lambda2 = LAM2;
}

void IntPolyh_StartPoint::SetCoupleValue(const int IT1, const int IT2)
{
  t1 = IT1;
  t2 = IT2;
}

void IntPolyh_StartPoint::SetAngle(const double Ang)
{
  angle = Ang;
}

void IntPolyh_StartPoint::SetChainList(const int ChList)
{
  chainlist = ChList;
}

int IntPolyh_StartPoint::CheckSameSP(const IntPolyh_StartPoint& SP) const
{
  /// Renvoit 1 si monSP==SP
  int Test = 0;
  if (((e1 >= -1) && (e1 == SP.e1)) || ((e2 >= -1) && (e2 == SP.e2)))
  {
    /// Les edges sont definis

    if (((lambda1 > -MyConfusionPrecision)
         && (std::abs(lambda1 - SP.lambda1)
             < MyConfusionPrecision)) // lambda1!=-1 && lambda1==SP.lambda2
        || ((lambda2 > -MyConfusionPrecision)
            && (std::abs(lambda2 - SP.lambda2) < MyConfusionPrecision)))
      Test = 1;
    // if( (std::abs(u1-SP.u1)<MyConfusionPrecision)&&(std::abs(v1-SP.v1)<MyConfusionPrecision) )
    // Test=1;
  }
  if ((Test == 0) && ((e1 == -1) || (e2 == -1)))
  {
    /// monSP est un sommet
    if ((std::abs(SP.u1 - u1) < MyConfusionPrecision)
        && (std::abs(SP.v1 - v1) < MyConfusionPrecision))
      Test = 1;
  }
  else if ((e1 == -2) && (e2 == -2))
  {
    Dump(00200);
    SP.Dump(00201);
    printf("e1==-2 & e2==-2 Can't Check\n");
  }
  /*  if( (std::abs(u1-SP.u1)<MyConfusionPrecision)&&(std::abs(v1-SP.v1)<MyConfusionPrecision) )
      Test=1;*/
  return (Test);
}

void IntPolyh_StartPoint::Dump() const
{
  printf("\nPoint : x=%+8.3eg y=%+8.3eg z=%+8.3eg u1=%+8.3eg v1=%+8.3eg u2=%+8.3eg v2=%+8.3eg\n",
         x,
         y,
         z,
         u1,
         v1,
         u2,
         v2);
  printf("Triangle S1:%d Edge S1:%d Lambda1:%f Triangle S2:%d Edge S2:%d Lambda2:%f\n",
         t1,
         e1,
         lambda1,
         t2,
         e2,
         lambda2);
  printf("Angle: %f List Number: %d\n", angle, chainlist);
}

void IntPolyh_StartPoint::Dump(const int i) const
{
  printf(
    "\nPoint(%d) : x=%+8.3eg y=%+8.3eg z=%+8.3eg u1=%+8.3eg v1=%+8.3eg u2=%+8.3eg v2=%+8.3eg\n",
    i,
    x,
    y,
    z,
    u1,
    v1,
    u2,
    v2);
  printf("Triangle S1:%d Edge S1:%d Lambda1:%f Triangle S2:%d Edge S2:%d Lambda2:%f\n",
         t1,
         e1,
         lambda1,
         t2,
         e2,
         lambda2);
  printf("Angle: %f List Number: %d\n", angle, chainlist);
}
