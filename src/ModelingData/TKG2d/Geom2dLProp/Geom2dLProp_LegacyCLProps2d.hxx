// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _Geom2dLProp_LegacyCLProps2d_HeaderFile
#define _Geom2dLProp_LegacyCLProps2d_HeaderFile

#include <LProp_Status.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

class Geom2d_Curve;

class Geom2dLProp_LegacyCLProps2d
{
public:
  DEFINE_STANDARD_ALLOC

  Geom2dLProp_LegacyCLProps2d(const occ::handle<Geom2d_Curve>& theCurve,
                              double                           theParam,
                              int                              theDerOrder,
                              double                           theResolution);
  Geom2dLProp_LegacyCLProps2d(const occ::handle<Geom2d_Curve>& theCurve,
                              int                              theDerOrder,
                              double                           theResolution);
  Geom2dLProp_LegacyCLProps2d(int theDerOrder, double theResolution);

  void SetParameter(double theParam);
  void SetCurve(const occ::handle<Geom2d_Curve>& theCurve);

  const gp_Pnt2d& Value() const;
  const gp_Vec2d& D1();
  const gp_Vec2d& D2();
  const gp_Vec2d& D3();
  bool            IsTangentDefined();
  void            Tangent(gp_Dir2d& theDir);
  double          Curvature();
  void            Normal(gp_Dir2d& theDir);
  void            CentreOfCurvature(gp_Pnt2d& thePoint);

  LProp_Status TangentStatus() const { return myTangentStatus; }

private:
  occ::handle<Geom2d_Curve> myCurve;
  double                    myU;
  int                       myDerOrder;
  int                       myCN;
  double                    myLinTol;
  gp_Pnt2d                  myPnt;
  gp_Vec2d                  myDerivArr[3];
  double                    myCurvature;
  LProp_Status              myTangentStatus;
  int                       mySignificantFirstDerivativeOrder;
};

#endif
