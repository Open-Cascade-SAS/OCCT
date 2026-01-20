// Created on: 1997-05-28
// Created by: Xavier BENVENISTE
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _GeomLib_Check2dBSplineCurve_HeaderFile
#define _GeomLib_Check2dBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Geom2d_BSplineCurve;

//! Checks for the end tangents : whether or not those
//! are reversed
class GeomLib_Check2dBSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomLib_Check2dBSplineCurve(const occ::handle<Geom2d_BSplineCurve>& Curve,
                                              const double                Tolerance,
                                              const double                AngularTolerance);

  bool IsDone() const;

  Standard_EXPORT void NeedTangentFix(bool& FirstFlag,
                                      bool& SecondFlag) const;

  Standard_EXPORT void FixTangent(const bool FirstFlag,
                                  const bool LastFlag);

  //! modifies the curve
  //! by fixing the first or the last tangencies
  //!
  //! if Index3D not in the Range [1,Nb3dSpaces]
  //! if the Approx is not Done
  Standard_EXPORT occ::handle<Geom2d_BSplineCurve> FixedTangent(const bool FirstFlag,
                                                           const bool LastFlag);

private:
  void FixTangentOnCurve(occ::handle<Geom2d_BSplineCurve>& theCurve,
                         const bool       FirstFlag,
                         const bool       LastFlag);

  occ::handle<Geom2d_BSplineCurve> myCurve;
  bool            myDone;
  bool            myFixFirstTangent;
  bool            myFixLastTangent;
  double               myAngularTolerance;
  double               myTolerance;

  int myIndSecondPole;
  int myIndPrelastPole;
};

#include <GeomLib_Check2dBSplineCurve.lxx>

#endif // _GeomLib_Check2dBSplineCurve_HeaderFile
