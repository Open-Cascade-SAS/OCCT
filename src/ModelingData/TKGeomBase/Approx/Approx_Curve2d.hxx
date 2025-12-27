// Created on: 1997-10-28
// Created by: Roman BORISOV
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

#ifndef _Approx_Curve2d_HeaderFile
#define _Approx_Curve2d_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAbs_Shape.hxx>

//! Makes an approximation for HCurve2d from Adaptor3d
class Approx_Curve2d
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Approx_Curve2d(const occ::handle<Adaptor2d_Curve2d>& C2D,
                                 const double              First,
                                 const double              Last,
                                 const double              TolU,
                                 const double              TolV,
                                 const GeomAbs_Shape              Continuity,
                                 const int           MaxDegree,
                                 const int           MaxSegments);

  Standard_EXPORT bool IsDone() const;

  Standard_EXPORT bool HasResult() const;

  Standard_EXPORT occ::handle<Geom2d_BSplineCurve> Curve() const;

  Standard_EXPORT double MaxError2dU() const;

  Standard_EXPORT double MaxError2dV() const;

private:
  occ::handle<Geom2d_BSplineCurve> myCurve;
  bool            myIsDone;
  bool            myHasResult;
  double               myMaxError2dU;
  double               myMaxError2dV;
};

#endif // _Approx_Curve2d_HeaderFile
