// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomInt_HeaderFile
#define _GeomInt_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>


//! Provides intersections on between two surfaces of Geom.
//! The result are curves from Geom.
class GeomInt 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adjusts the parameter <thePar> to the range [theParMin,  theParMax]
  Standard_EXPORT static Standard_Boolean AdjustPeriodic (const Standard_Real thePar, const Standard_Real theParMin, const Standard_Real theParMax, const Standard_Real thePeriod, Standard_Real& theNewPar, Standard_Real& theOffset, const Standard_Real theEps = 0.0);




protected:





private:




friend class GeomInt_IntSS;
friend class GeomInt_LineConstructor;
friend class GeomInt_LineTool;
friend class GeomInt_WLApprox;
friend class GeomInt_ParameterAndOrientation;
friend class GeomInt_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfWLApprox;
friend class GeomInt_TheInt2SOfThePrmPrmSvSurfacesOfWLApprox;
friend class GeomInt_ThePrmPrmSvSurfacesOfWLApprox;
friend class GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox;
friend class GeomInt_TheImpPrmSvSurfacesOfWLApprox;
friend class GeomInt_TheMultiLineOfWLApprox;
friend class GeomInt_TheMultiLineToolOfWLApprox;
friend class GeomInt_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfWLApprox;
friend class GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox;
friend class GeomInt_BSpGradient_BFGSOfMyBSplGradientOfTheComputeLineOfWLApprox;
friend class GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox;
friend class GeomInt_ParLeastSquareOfMyGradientbisOfTheComputeLineOfWLApprox;
friend class GeomInt_ResConstraintOfMyGradientbisOfTheComputeLineOfWLApprox;
friend class GeomInt_ParFunctionOfMyGradientbisOfTheComputeLineOfWLApprox;
friend class GeomInt_Gradient_BFGSOfMyGradientbisOfTheComputeLineOfWLApprox;
friend class GeomInt_MyGradientbisOfTheComputeLineOfWLApprox;
friend class GeomInt_TheComputeLineOfWLApprox;
friend class GeomInt_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfWLApprox;
friend class GeomInt_ResConstraintOfMyGradientOfTheComputeLineBezierOfWLApprox;
friend class GeomInt_ParFunctionOfMyGradientOfTheComputeLineBezierOfWLApprox;
friend class GeomInt_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfWLApprox;
friend class GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox;
friend class GeomInt_TheComputeLineBezierOfWLApprox;

};







#endif // _GeomInt_HeaderFile
