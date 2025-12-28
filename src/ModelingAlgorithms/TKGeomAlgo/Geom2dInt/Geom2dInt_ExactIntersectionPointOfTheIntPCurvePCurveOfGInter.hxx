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

#ifndef _Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter_HeaderFile
#define _Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter.hxx>
#include <math_Vector.hxx>
class Adaptor2d_Curve2d;
class Geom2dInt_Geom2dCurveTool;
class Geom2dInt_TheProjPCurOfGInter;
class Geom2dInt_TheIntPCurvePCurveOfGInter;
class Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter;
class Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter;

class Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter(
    const Adaptor2d_Curve2d& C1,
    const Adaptor2d_Curve2d& C2,
    const double             Tol);

  Standard_EXPORT void Perform(const Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter& Poly1,
                               const Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter& Poly2,
                               int&                                                      NumSegOn1,
                               int&                                                      NumSegOn2,
                               double& ParamOnSeg1,
                               double& ParamOnSeg2);

  Standard_EXPORT void Perform(const double Uo,
                               const double Vo,
                               const double UInf,
                               const double VInf,
                               const double USup,
                               const double VSup);

  Standard_EXPORT int NbRoots() const;

  Standard_EXPORT void Roots(double& U, double& V);

  Standard_EXPORT bool AnErrorOccurred() const;

private:
  Standard_EXPORT void MathPerform();

  bool                                                        done;
  int                                                         nbroots;
  double                                                      myTol;
  Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter FctDist;
  math_Vector                                                 ToleranceVector;
  math_Vector                                                 BInfVector;
  math_Vector                                                 BSupVector;
  math_Vector                                                 StartingPoint;
  math_Vector                                                 Root;
  bool                                                        anErrorOccurred;
};

#endif // _Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter_HeaderFile
