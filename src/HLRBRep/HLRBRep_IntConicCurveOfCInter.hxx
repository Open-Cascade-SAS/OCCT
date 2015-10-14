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

#ifndef _HLRBRep_IntConicCurveOfCInter_HeaderFile
#define _HLRBRep_IntConicCurveOfCInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <IntCurve_IntConicConic.hxx>
#include <HLRBRep_TheIntConicCurveOfCInter.hxx>
#include <IntRes2d_Intersection.hxx>
#include <Standard_Address.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class IntCurve_IConicTool;
class HLRBRep_CurveTool;
class HLRBRep_TheIntConicCurveOfCInter;
class gp_Lin2d;
class IntRes2d_Domain;
class gp_Circ2d;
class gp_Elips2d;
class gp_Parab2d;
class gp_Hypr2d;



class HLRBRep_IntConicCurveOfCInter  : public IntRes2d_Intersection
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT HLRBRep_IntConicCurveOfCInter();
  
  //! Intersection between a line and a parametric curve.
  Standard_EXPORT HLRBRep_IntConicCurveOfCInter(const gp_Lin2d& L, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between a line and a parametric curve.
  Standard_EXPORT HLRBRep_IntConicCurveOfCInter(const gp_Circ2d& C, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between an ellipse and a parametric curve.
  Standard_EXPORT HLRBRep_IntConicCurveOfCInter(const gp_Elips2d& E, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between a parabola and a parametric curve.
  Standard_EXPORT HLRBRep_IntConicCurveOfCInter(const gp_Parab2d& Prb, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between the main branch of an hyperbola
  //! and a parametric curve.
  Standard_EXPORT HLRBRep_IntConicCurveOfCInter(const gp_Hypr2d& H, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between a line and a parametric curve.
  Standard_EXPORT void Perform (const gp_Lin2d& L, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between a line and a parametric curve.
  Standard_EXPORT void Perform (const gp_Circ2d& C, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between an ellipse and a parametric curve.
  Standard_EXPORT void Perform (const gp_Elips2d& E, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between a parabola and a parametric curve.
  Standard_EXPORT void Perform (const gp_Parab2d& Prb, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between the main branch of an hyperbola
  //! and a parametric curve.
  Standard_EXPORT void Perform (const gp_Hypr2d& H, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);




protected:





private:

  
  Standard_EXPORT void InternalPerform (const gp_Lin2d& Lin1, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean Composite);
  
  Standard_EXPORT void InternalPerform (const gp_Circ2d& Circ1, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean Composite);
  
  Standard_EXPORT void InternalPerform (const gp_Elips2d& Eli1, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean Composite);
  
  Standard_EXPORT void InternalPerform (const gp_Parab2d& Prb1, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean Composite);
  
  Standard_EXPORT void InternalPerform (const gp_Hypr2d& Hpr1, const IntRes2d_Domain& D1, const Standard_Address& PCurve, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean Composite);


  Standard_Real param1inf;
  Standard_Real param1sup;
  Standard_Real param2inf;
  Standard_Real param2sup;
  IntCurve_IntConicConic intconiconi;
  HLRBRep_TheIntConicCurveOfCInter intconicurv;


};







#endif // _HLRBRep_IntConicCurveOfCInter_HeaderFile
