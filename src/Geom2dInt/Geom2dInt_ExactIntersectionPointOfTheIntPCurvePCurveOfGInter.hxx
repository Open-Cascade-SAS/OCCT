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
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
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

  
  Standard_EXPORT Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter(const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2, const Standard_Real Tol);
  
  Standard_EXPORT void Perform (const Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter& Poly1, const Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter& Poly2, Standard_Integer& NumSegOn1, Standard_Integer& NumSegOn2, Standard_Real& ParamOnSeg1, Standard_Real& ParamOnSeg2);
  
  Standard_EXPORT void Perform (const Standard_Real Uo, const Standard_Real Vo, const Standard_Real UInf, const Standard_Real VInf, const Standard_Real USup, const Standard_Real VSup);
  
  Standard_EXPORT Standard_Integer NbRoots() const;
  
  Standard_EXPORT void Roots (Standard_Real& U, Standard_Real& V);
  
  Standard_EXPORT Standard_Boolean AnErrorOccurred() const;




protected:





private:

  
  Standard_EXPORT void MathPerform();


  Standard_Boolean done;
  Standard_Integer nbroots;
  Standard_Real myTol;
  Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter FctDist;
  math_Vector ToleranceVector;
  math_Vector BInfVector;
  math_Vector BSupVector;
  math_Vector StartingPoint;
  math_Vector Root;
  Standard_Boolean anErrorOccurred;


};







#endif // _Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter_HeaderFile
