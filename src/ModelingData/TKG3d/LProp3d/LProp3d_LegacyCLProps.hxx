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

#ifndef _LProp3d_LegacyCLProps_HeaderFile
#define _LProp3d_LegacyCLProps_HeaderFile

#include <LProp_Status.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

class Adaptor3d_Curve;

class LProp3d_LegacyCLProps
{
public:
  DEFINE_STANDARD_ALLOC

  LProp3d_LegacyCLProps(const occ::handle<Adaptor3d_Curve>& theCurve,
                        double                              theParam,
                        int                                 theDerOrder,
                        double                              theResolution);
  LProp3d_LegacyCLProps(const occ::handle<Adaptor3d_Curve>& theCurve,
                        int                                 theDerOrder,
                        double                              theResolution);
  LProp3d_LegacyCLProps(int theDerOrder, double theResolution);

  void SetParameter(double theParam);
  void SetCurve(const occ::handle<Adaptor3d_Curve>& theCurve);

  const gp_Pnt& Value() const;
  const gp_Vec& D1();
  const gp_Vec& D2();
  const gp_Vec& D3();
  bool          IsTangentDefined();
  void          Tangent(gp_Dir& theDir);
  double        Curvature();
  void          Normal(gp_Dir& theDir);
  void          CentreOfCurvature(gp_Pnt& thePoint);

  LProp_Status TangentStatus() const { return myTangentStatus; }

private:
  occ::handle<Adaptor3d_Curve> myCurve;
  double                       myU;
  int                          myDerOrder;
  int                          myCN;
  double                       myLinTol;
  gp_Pnt                       myPnt;
  gp_Vec                       myDerivArr[3];
  double                       myCurvature;
  LProp_Status                 myTangentStatus;
  int                          mySignificantFirstDerivativeOrder;
};

#endif
