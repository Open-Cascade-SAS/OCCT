// Created on: 1992-03-26
// Created by: Herve LEGRAND
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

#ifndef _Geom2dLProp_CLProps2d_HeaderFile
#define _Geom2dLProp_CLProps2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>
#include <LProp_Status.hxx>
#include <Standard_Boolean.hxx>
class Geom2d_Curve;
class LProp_BadContinuity;
class Standard_DomainError;
class Standard_OutOfRange;
class LProp_NotDefined;
class gp_Vec2d;
class gp_Pnt2d;
class gp_Dir2d;
class Geom2dLProp_Curve2dTool;



class Geom2dLProp_CLProps2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dLProp_CLProps2d(const Handle(Geom2d_Curve)& C, const Standard_Integer N, const Standard_Real Resolution);
  
  Standard_EXPORT Geom2dLProp_CLProps2d(const Handle(Geom2d_Curve)& C, const Standard_Real U, const Standard_Integer N, const Standard_Real Resolution);
  
  Standard_EXPORT Geom2dLProp_CLProps2d(const Standard_Integer N, const Standard_Real Resolution);
  
  Standard_EXPORT void SetParameter (const Standard_Real U);
  
  Standard_EXPORT void SetCurve (const Handle(Geom2d_Curve)& C);
  
  Standard_EXPORT const gp_Pnt2d& Value() const;
  
  Standard_EXPORT const gp_Vec2d& D1();
  
  Standard_EXPORT const gp_Vec2d& D2();
  
  Standard_EXPORT const gp_Vec2d& D3();
  
  Standard_EXPORT Standard_Boolean IsTangentDefined();
  
  Standard_EXPORT void Tangent (gp_Dir2d& D);
  
  Standard_EXPORT Standard_Real Curvature();
  
  Standard_EXPORT void Normal (gp_Dir2d& N);
  
  Standard_EXPORT void CentreOfCurvature (gp_Pnt2d& P);




protected:





private:



  Handle(Geom2d_Curve) myCurve;
  Standard_Real myU;
  Standard_Integer myDerOrder;
  Standard_Real myCN;
  Standard_Real myLinTol;
  gp_Pnt2d myPnt;
  gp_Vec2d myDerivArr[3];
  gp_Dir2d myTangent;
  Standard_Real myCurvature;
  LProp_Status myTangentStatus;
  Standard_Integer mySignificantFirstDerivativeOrder;


};







#endif // _Geom2dLProp_CLProps2d_HeaderFile
