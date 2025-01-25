// Created on: 1992-06-04
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter_HeaderFile
#define _Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class Adaptor2d_Curve2d;
class Geom2dInt_Geom2dCurveTool;
class Extrema_POnCurv2d;
class gp_Pnt2d;

class Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter
{
public:
  DEFINE_STANDARD_ALLOC

  //! Among a set of points {C(ui),i=1,NbU}, locate the point
  //! P=C(uj) such that:
  //! distance(P,C) = Min{distance(P,C(ui))}
  Standard_EXPORT static void Locate(const gp_Pnt2d&          P,
                                     const Adaptor2d_Curve2d& C,
                                     const Standard_Integer   NbU,
                                     Extrema_POnCurv2d&       Papp);

  //! Among a set of points {C(ui),i=1,NbU}, locate the point
  //! P=C(uj) such that:
  //! distance(P,C) = Min{distance(P,C(ui))}
  //! The research is done between umin and usup.
  Standard_EXPORT static void Locate(const gp_Pnt2d&          P,
                                     const Adaptor2d_Curve2d& C,
                                     const Standard_Integer   NbU,
                                     const Standard_Real      Umin,
                                     const Standard_Real      Usup,
                                     Extrema_POnCurv2d&       Papp);

  //! Among two sets of points {C1(ui),i=1,NbU} and
  //! {C2(vj),j=1,NbV}, locate the two points P1=C1(uk) and
  //! P2=C2(vl) such that:
  //! distance(P1,P2) = Min {distance(C1(ui),C2(vj))}.
  Standard_EXPORT static void Locate(const Adaptor2d_Curve2d& C1,
                                     const Adaptor2d_Curve2d& C2,
                                     const Standard_Integer   NbU,
                                     const Standard_Integer   NbV,
                                     Extrema_POnCurv2d&       Papp1,
                                     Extrema_POnCurv2d&       Papp2);

protected:
private:
};

#endif // _Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter_HeaderFile
