// Created on: 1999-04-06
// Created by: Fabrice SERVANT
// Copyright (c) 1999 Matra Datavision
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

#ifndef _IntPolyh_StartPoint_HeaderFile
#define _IntPolyh_StartPoint_HeaderFile

#include <Standard.hxx>

class IntPolyh_Triangle;

class IntPolyh_StartPoint
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntPolyh_StartPoint();

  Standard_EXPORT IntPolyh_StartPoint(const double    xx,
                                      const double    yy,
                                      const double    zz,
                                      const double    uu1,
                                      const double    vv1,
                                      const double    uu2,
                                      const double    vv2,
                                      const int T1,
                                      const int E1,
                                      const double    LAM1,
                                      const int T2,
                                      const int E2,
                                      const double    LAM2,
                                      const int List);

  Standard_EXPORT double X() const;

  Standard_EXPORT double Y() const;

  Standard_EXPORT double Z() const;

  Standard_EXPORT double U1() const;

  Standard_EXPORT double V1() const;

  Standard_EXPORT double U2() const;

  Standard_EXPORT double V2() const;

  Standard_EXPORT int T1() const;

  Standard_EXPORT int E1() const;

  Standard_EXPORT double Lambda1() const;

  Standard_EXPORT int T2() const;

  Standard_EXPORT int E2() const;

  Standard_EXPORT double Lambda2() const;

  Standard_EXPORT double GetAngle() const;

  Standard_EXPORT int ChainList() const;

  Standard_EXPORT int GetEdgePoints(const IntPolyh_Triangle& Triangle,
                                                 int&        FirstEdgePoint,
                                                 int&        SecondEdgePoint,
                                                 int&        LastPoint) const;

  Standard_EXPORT void SetXYZ(const double XX,
                              const double YY,
                              const double ZZ);

  Standard_EXPORT void SetUV1(const double UU1, const double VV1);

  Standard_EXPORT void SetUV2(const double UU2, const double VV2);

  Standard_EXPORT void SetEdge1(const int IE1);

  Standard_EXPORT void SetLambda1(const double LAM1);

  Standard_EXPORT void SetEdge2(const int IE2);

  Standard_EXPORT void SetLambda2(const double LAM2);

  Standard_EXPORT void SetCoupleValue(const int IT1, const int IT2);

  Standard_EXPORT void SetAngle(const double ang);

  Standard_EXPORT void SetChainList(const int ChList);

  Standard_EXPORT int CheckSameSP(const IntPolyh_StartPoint& SP) const;

  Standard_EXPORT void Dump() const;

  Standard_EXPORT void Dump(const int i) const;

private:
  double    x;
  double    y;
  double    z;
  double    u1;
  double    v1;
  double    u2;
  double    v2;
  double    lambda1;
  double    lambda2;
  double    angle;
  int t1;
  int e1;
  int t2;
  int e2;
  int chainlist;
};

#endif // _IntPolyh_StartPoint_HeaderFile
