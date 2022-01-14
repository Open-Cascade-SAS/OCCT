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

  
  Standard_EXPORT HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter(const Standard_Address& C1, const Standard_Address& C2, const Standard_Real Tol);
  
  Standard_EXPORT void Perform (const HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter& Poly1, const HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter& Poly2, Standard_Integer& NumSegOn1, Standard_Integer& NumSegOn2, Standard_Real& ParamOnSeg1, Standard_Real& ParamOnSeg2);
  
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
  HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter FctDist;
  math_Vector ToleranceVector;
  math_Vector BInfVector;
  math_Vector BSupVector;
  math_Vector StartingPoint;
  math_Vector Root;
  Standard_Boolean anErrorOccurred;


};







#endif // _HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter_HeaderFile
