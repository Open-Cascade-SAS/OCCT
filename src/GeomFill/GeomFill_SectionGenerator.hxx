// Created on: 1994-02-18
// Created by: Bruno DUMORTIER
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

#ifndef _GeomFill_SectionGenerator_HeaderFile
#define _GeomFill_SectionGenerator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <GeomFill_Profiler.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>


//! gives  the  functions  needed  for  instantiation from
//! AppSurf in AppBlend.   Allow  to  evaluate  a  surface
//! passing by all the curves if the Profiler.
class GeomFill_SectionGenerator  : public GeomFill_Profiler
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomFill_SectionGenerator();
  
  Standard_EXPORT void SetParam (const Handle(TColStd_HArray1OfReal)& Params);
  
  Standard_EXPORT void GetShape (Standard_Integer& NbPoles, Standard_Integer& NbKnots, Standard_Integer& Degree, Standard_Integer& NbPoles2d) const;
  
  Standard_EXPORT void Knots (TColStd_Array1OfReal& TKnots) const;
  
  Standard_EXPORT void Mults (TColStd_Array1OfInteger& TMults) const;
  
  //! Used for the first and last section
  //! The method returns Standard_True if the derivatives
  //! are computed, otherwise it returns Standard_False.
  Standard_EXPORT Standard_Boolean Section (const Standard_Integer P, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d, TColStd_Array1OfReal& Weigths, TColStd_Array1OfReal& DWeigths) const;
  
  Standard_EXPORT void Section (const Standard_Integer P, TColgp_Array1OfPnt& Poles, TColgp_Array1OfPnt2d& Poles2d, TColStd_Array1OfReal& Weigths) const;
  
  //! Returns  the parameter of   Section<P>, to impose  it for the
  //! approximation.
  Standard_EXPORT Standard_Real Parameter (const Standard_Integer P) const;




protected:



  Handle(TColStd_HArray1OfReal) myParams;


private:





};







#endif // _GeomFill_SectionGenerator_HeaderFile
