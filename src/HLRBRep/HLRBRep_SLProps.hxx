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

#ifndef _HLRBRep_SLProps_HeaderFile
#define _HLRBRep_SLProps_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
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
class HLRBRep_SLPropsATool;
class gp_Pnt;
class gp_Vec;
class gp_Dir;



class HLRBRep_SLProps 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT HLRBRep_SLProps(const Standard_Address& S, const Standard_Real U, const Standard_Real V, const Standard_Integer N, const Standard_Real Resolution);
  
  Standard_EXPORT HLRBRep_SLProps(const Standard_Address& S, const Standard_Integer N, const Standard_Real Resolution);
  
  Standard_EXPORT HLRBRep_SLProps(const Standard_Integer N, const Standard_Real Resolution);
  
  Standard_EXPORT void SetSurface (const Standard_Address& S);
  
  Standard_EXPORT void SetParameters (const Standard_Real U, const Standard_Real V);
  
  Standard_EXPORT const gp_Pnt& Value() const;
  
  Standard_EXPORT const gp_Vec& D1U();
  
  Standard_EXPORT const gp_Vec& D1V();
  
  Standard_EXPORT const gp_Vec& D2U();
  
  Standard_EXPORT const gp_Vec& D2V();
  
  Standard_EXPORT const gp_Vec& DUV();
  
  Standard_EXPORT Standard_Boolean IsTangentUDefined();
  
  Standard_EXPORT void TangentU (gp_Dir& D);
  
  Standard_EXPORT Standard_Boolean IsTangentVDefined();
  
  Standard_EXPORT void TangentV (gp_Dir& D);
  
  Standard_EXPORT Standard_Boolean IsNormalDefined();
  
  Standard_EXPORT const gp_Dir& Normal();
  
  Standard_EXPORT Standard_Boolean IsCurvatureDefined();
  
  Standard_EXPORT Standard_Boolean IsUmbilic();
  
  Standard_EXPORT Standard_Real MaxCurvature();
  
  Standard_EXPORT Standard_Real MinCurvature();
  
  Standard_EXPORT void CurvatureDirections (gp_Dir& MaxD, gp_Dir& MinD);
  
  Standard_EXPORT Standard_Real MeanCurvature();
  
  Standard_EXPORT Standard_Real GaussianCurvature();




protected:





private:



  Standard_Address mySurf;
  Standard_Real myU;
  Standard_Real myV;
  Standard_Integer myDerOrder;
  Standard_Integer myCN;
  Standard_Real myLinTol;
  gp_Pnt myPnt;
  gp_Vec myD1u;
  gp_Vec myD1v;
  gp_Vec myD2u;
  gp_Vec myD2v;
  gp_Vec myDuv;
  gp_Dir myNormal;
  Standard_Real myMinCurv;
  Standard_Real myMaxCurv;
  gp_Dir myDirMinCurv;
  gp_Dir myDirMaxCurv;
  Standard_Real myMeanCurv;
  Standard_Real myGausCurv;
  Standard_Integer mySignificantFirstDerivativeOrderU;
  Standard_Integer mySignificantFirstDerivativeOrderV;
  LProp_Status myUTangentStatus;
  LProp_Status myVTangentStatus;
  LProp_Status myNormalStatus;
  LProp_Status myCurvatureStatus;


};







#endif // _HLRBRep_SLProps_HeaderFile
