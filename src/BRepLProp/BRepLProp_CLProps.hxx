// Created on: 1994-02-24
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepLProp_CLProps_HeaderFile
#define _BRepLProp_CLProps_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <LProp_Status.hxx>
#include <Standard_Boolean.hxx>
class LProp_BadContinuity;
class Standard_DomainError;
class Standard_OutOfRange;
class LProp_NotDefined;
class BRepAdaptor_Curve;
class gp_Vec;
class gp_Pnt;
class gp_Dir;
class BRepLProp_CurveTool;



class BRepLProp_CLProps 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepLProp_CLProps(const BRepAdaptor_Curve& C, const Standard_Integer N, const Standard_Real Resolution);
  
  Standard_EXPORT BRepLProp_CLProps(const BRepAdaptor_Curve& C, const Standard_Real U, const Standard_Integer N, const Standard_Real Resolution);
  
  Standard_EXPORT BRepLProp_CLProps(const Standard_Integer N, const Standard_Real Resolution);
  
  Standard_EXPORT void SetParameter (const Standard_Real U);
  
  Standard_EXPORT void SetCurve (const BRepAdaptor_Curve& C);
  
  Standard_EXPORT const gp_Pnt& Value() const;
  
  Standard_EXPORT const gp_Vec& D1();
  
  Standard_EXPORT const gp_Vec& D2();
  
  Standard_EXPORT const gp_Vec& D3();
  
  Standard_EXPORT Standard_Boolean IsTangentDefined();
  
  Standard_EXPORT void Tangent (gp_Dir& D);
  
  Standard_EXPORT Standard_Real Curvature();
  
  Standard_EXPORT void Normal (gp_Dir& N);
  
  Standard_EXPORT void CentreOfCurvature (gp_Pnt& P);




protected:





private:



  BRepAdaptor_Curve myCurve;
  Standard_Real myU;
  Standard_Integer myDerOrder;
  Standard_Real myCN;
  Standard_Real myLinTol;
  gp_Pnt myPnt;
  gp_Vec myDerivArr[3];
  gp_Dir myTangent;
  Standard_Real myCurvature;
  LProp_Status myTangentStatus;
  Standard_Integer mySignificantFirstDerivativeOrder;


};







#endif // _BRepLProp_CLProps_HeaderFile
