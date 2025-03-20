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

  Standard_EXPORT IntPolyh_StartPoint(const Standard_Real    xx,
                                      const Standard_Real    yy,
                                      const Standard_Real    zz,
                                      const Standard_Real    uu1,
                                      const Standard_Real    vv1,
                                      const Standard_Real    uu2,
                                      const Standard_Real    vv2,
                                      const Standard_Integer T1,
                                      const Standard_Integer E1,
                                      const Standard_Real    LAM1,
                                      const Standard_Integer T2,
                                      const Standard_Integer E2,
                                      const Standard_Real    LAM2,
                                      const Standard_Integer List);

  Standard_EXPORT Standard_Real X() const;

  Standard_EXPORT Standard_Real Y() const;

  Standard_EXPORT Standard_Real Z() const;

  Standard_EXPORT Standard_Real U1() const;

  Standard_EXPORT Standard_Real V1() const;

  Standard_EXPORT Standard_Real U2() const;

  Standard_EXPORT Standard_Real V2() const;

  Standard_EXPORT Standard_Integer T1() const;

  Standard_EXPORT Standard_Integer E1() const;

  Standard_EXPORT Standard_Real Lambda1() const;

  Standard_EXPORT Standard_Integer T2() const;

  Standard_EXPORT Standard_Integer E2() const;

  Standard_EXPORT Standard_Real Lambda2() const;

  Standard_EXPORT Standard_Real GetAngle() const;

  Standard_EXPORT Standard_Integer ChainList() const;

  Standard_EXPORT Standard_Integer GetEdgePoints(const IntPolyh_Triangle& Triangle,
                                                 Standard_Integer&        FirstEdgePoint,
                                                 Standard_Integer&        SecondEdgePoint,
                                                 Standard_Integer&        LastPoint) const;

  Standard_EXPORT void SetXYZ(const Standard_Real XX,
                              const Standard_Real YY,
                              const Standard_Real ZZ);

  Standard_EXPORT void SetUV1(const Standard_Real UU1, const Standard_Real VV1);

  Standard_EXPORT void SetUV2(const Standard_Real UU2, const Standard_Real VV2);

  Standard_EXPORT void SetEdge1(const Standard_Integer IE1);

  Standard_EXPORT void SetLambda1(const Standard_Real LAM1);

  Standard_EXPORT void SetEdge2(const Standard_Integer IE2);

  Standard_EXPORT void SetLambda2(const Standard_Real LAM2);

  Standard_EXPORT void SetCoupleValue(const Standard_Integer IT1, const Standard_Integer IT2);

  Standard_EXPORT void SetAngle(const Standard_Real ang);

  Standard_EXPORT void SetChainList(const Standard_Integer ChList);

  Standard_EXPORT Standard_Integer CheckSameSP(const IntPolyh_StartPoint& SP) const;

  Standard_EXPORT void Dump() const;

  Standard_EXPORT void Dump(const Standard_Integer i) const;

private:
  Standard_Real    x;
  Standard_Real    y;
  Standard_Real    z;
  Standard_Real    u1;
  Standard_Real    v1;
  Standard_Real    u2;
  Standard_Real    v2;
  Standard_Real    lambda1;
  Standard_Real    lambda2;
  Standard_Real    angle;
  Standard_Integer t1;
  Standard_Integer e1;
  Standard_Integer t2;
  Standard_Integer e2;
  Standard_Integer chainlist;
};

#endif // _IntPolyh_StartPoint_HeaderFile
