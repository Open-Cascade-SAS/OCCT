// Created on: 1994-02-28
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

#ifndef _GeomFill_SweepSectionGenerator_HeaderFile
#define _GeomFill_SweepSectionGenerator_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <gp_Ax1.hxx>
#include <GeomFill_SequenceOfTrsf.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>

class Geom_BSplineCurve;
class Geom_Curve;
class gp_Trsf;


//! class for instantiation of AppBlend.
//! evaluate the sections of a sweep surface.
class GeomFill_SweepSectionGenerator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomFill_SweepSectionGenerator();
  
  //! Create a sweept surface with a constant radius.
  Standard_EXPORT GeomFill_SweepSectionGenerator(const Handle(Geom_Curve)& Path, const Standard_Real Radius);
  
  //! Create a sweept surface with a constant section
  Standard_EXPORT GeomFill_SweepSectionGenerator(const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& FirstSect);
  
  //! Create a sweept surface with an evolving section
  //! The section evaluate from First to Last Section
  Standard_EXPORT GeomFill_SweepSectionGenerator(const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& FirstSect, const Handle(Geom_Curve)& LastSect);
  
  //! Create  a pipe  with  a constant  radius with  2
  //! guide-line.
  Standard_EXPORT GeomFill_SweepSectionGenerator(const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& Curve1, const Handle(Geom_Curve)& Curve2, const Standard_Real Radius);
  
  //! Create  a pipe  with  a constant  radius with  2
  //! guide-line.
  Standard_EXPORT GeomFill_SweepSectionGenerator(const Handle(Adaptor3d_Curve)& Path, const Handle(Adaptor3d_Curve)& Curve1, const Handle(Adaptor3d_Curve)& Curve2, const Standard_Real Radius);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Path, const Standard_Real Radius);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& FirstSect);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& FirstSect, const Handle(Geom_Curve)& LastSect);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& Path, const Handle(Geom_Curve)& Curve1, const Handle(Geom_Curve)& Curve2, const Standard_Real Radius);
  
  Standard_EXPORT void Init (const Handle(Adaptor3d_Curve)& Path, const Handle(Adaptor3d_Curve)& Curve1, const Handle(Adaptor3d_Curve)& Curve2, const Standard_Real Radius);
  
  Standard_EXPORT void Perform (const Standard_Boolean Polynomial = Standard_False);
  
  Standard_EXPORT void GetShape (Standard_Integer& NbPoles, Standard_Integer& NbKnots, Standard_Integer& Degree, Standard_Integer& NbPoles2d) const;
  
  Standard_EXPORT void Knots (TColStd_Array1OfReal& TKnots) const;
  
  Standard_EXPORT void Mults (TColStd_Array1OfInteger& TMults) const;
  
    Standard_Integer NbSections() const;
  
  //! Used for the first and last section
  //! The method returns Standard_True if the derivatives
  //! are computed, otherwise it returns Standard_False.
  Standard_EXPORT Standard_Boolean Section (const Standard_Integer P, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d, TColStd_Array1OfReal& Weigths, TColStd_Array1OfReal& DWeigths) const;
  
  Standard_EXPORT void Section (const Standard_Integer P, TColgp_Array1OfPnt& Poles, TColgp_Array1OfPnt2d& Poles2d, TColStd_Array1OfReal& Weigths) const;
  
  //! raised if <Index> not in the range [1,NbSections()]
  Standard_EXPORT const gp_Trsf& Transformation (const Standard_Integer Index) const;
  
  //! Returns  the parameter of   <P>, to impose  it for the
  //! approximation.
  Standard_EXPORT Standard_Real Parameter (const Standard_Integer P) const;




protected:





private:



  Handle(Geom_BSplineCurve) myPath;
  Handle(Geom_BSplineCurve) myFirstSect;
  Handle(Geom_BSplineCurve) myLastSect;
  Handle(Adaptor3d_Curve) myAdpPath;
  Handle(Adaptor3d_Curve) myAdpFirstSect;
  Handle(Adaptor3d_Curve) myAdpLastSect;
  gp_Ax1 myCircPathAxis;
  Standard_Real myRadius;
  Standard_Boolean myIsDone;
  Standard_Integer myNbSections;
  GeomFill_SequenceOfTrsf myTrsfs;
  Standard_Integer myType;
  Standard_Boolean myPolynomial;


};


#include <GeomFill_SweepSectionGenerator.lxx>





#endif // _GeomFill_SweepSectionGenerator_HeaderFile
