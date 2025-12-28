// Created on: 1992-10-14
// Created by: Christophe MARION
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

#ifndef _HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter_HeaderFile
#define _HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <HLRBRep_TypeDef.hxx>
#include <Standard_Boolean.hxx>
#include <HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter.hxx>
#include <math_Vector.hxx>
class HLRBRep_CurveTool;
class HLRBRep_TheProjPCurOfCInter;
class HLRBRep_TheIntPCurvePCurveOfCInter;
class HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter;
class HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter;

class HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter(
    const HLRBRep_CurvePtr& C1,
    const HLRBRep_CurvePtr& C2,
    const double            Tol);

  Standard_EXPORT void Perform(const HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter& Poly1,
                               const HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter& Poly2,
                               int&                                                    NumSegOn1,
                               int&                                                    NumSegOn2,
                               double&                                                 ParamOnSeg1,
                               double&                                                 ParamOnSeg2);

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

  bool                                                      done;
  int                                                       nbroots;
  double                                                    myTol;
  HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter FctDist;
  math_Vector                                               ToleranceVector;
  math_Vector                                               BInfVector;
  math_Vector                                               BSupVector;
  math_Vector                                               StartingPoint;
  math_Vector                                               Root;
  bool                                                      anErrorOccurred;
};

#endif // _HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter_HeaderFile
