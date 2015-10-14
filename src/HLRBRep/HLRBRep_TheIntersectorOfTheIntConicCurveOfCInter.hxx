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

#ifndef _HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter_HeaderFile
#define _HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntRes2d_Intersection.hxx>
#include <Standard_Address.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
class Standard_ConstructionError;
class IntCurve_IConicTool;
class HLRBRep_CurveTool;
class HLRBRep_TheProjPCurOfCInter;
class HLRBRep_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfCInter;
class IntRes2d_Domain;
class gp_Pnt2d;



class HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter  : public IntRes2d_Intersection
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter();
  
  //! Intersection between an implicit curve and
  //! a parametrised curve.
  //! The exception ConstructionError is raised if the domain
  //! of the parametrised curve does not verify HasFirstPoint
  //! and HasLastPoint return True.
  Standard_EXPORT HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter(const IntCurve_IConicTool& ITool, const IntRes2d_Domain& Dom1, const Standard_Address& PCurve, const IntRes2d_Domain& Dom2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between an implicit curve and
  //! a parametrised curve.
  //! The exception ConstructionError is raised if the domain
  //! of the parametrised curve does not verify HasFirstPoint
  //! and HasLastPoint return True.
  Standard_EXPORT void Perform (const IntCurve_IConicTool& ITool, const IntRes2d_Domain& Dom1, const Standard_Address& PCurve, const IntRes2d_Domain& Dom2, const Standard_Real TolConf, const Standard_Real Tol);
  
  Standard_EXPORT Standard_Real FindU (const Standard_Real parameter, gp_Pnt2d& point, const Standard_Address& TheParCurev, const IntCurve_IConicTool& TheImpTool) const;
  
  Standard_EXPORT Standard_Real FindV (const Standard_Real parameter, gp_Pnt2d& point, const IntCurve_IConicTool& TheImpTool, const Standard_Address& ParCurve, const IntRes2d_Domain& TheParCurveDomain, const Standard_Real V0, const Standard_Real V1, const Standard_Real Tolerance) const;
  
  Standard_EXPORT void And_Domaine_Objet1_Intersections (const IntCurve_IConicTool& TheImpTool, const Standard_Address& TheParCurve, const IntRes2d_Domain& TheImpCurveDomain, const IntRes2d_Domain& TheParCurveDomain, Standard_Integer& NbResultats, TColStd_Array1OfReal& Inter2_And_Domain2, TColStd_Array1OfReal& Inter1, TColStd_Array1OfReal& Resultat1, TColStd_Array1OfReal& Resultat2, const Standard_Real EpsNul) const;




protected:





private:





};







#endif // _HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter_HeaderFile
