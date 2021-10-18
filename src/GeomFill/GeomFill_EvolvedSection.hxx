// Created on: 1998-08-17
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _GeomFill_EvolvedSection_HeaderFile
#define _GeomFill_EvolvedSection_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <GeomAbs_Shape.hxx>
class Geom_Curve;
class Law_Function;
class Geom_BSplineCurve;
class Geom_BSplineSurface;
class gp_Pnt;


class GeomFill_EvolvedSection;
DEFINE_STANDARD_HANDLE(GeomFill_EvolvedSection, GeomFill_SectionLaw)

//! Define an Constant Section Law
class GeomFill_EvolvedSection : public GeomFill_SectionLaw
{

public:

  
  //! Make an SectionLaw with a Curve and a real  Law.
  Standard_EXPORT GeomFill_EvolvedSection(const Handle(Geom_Curve)& C, const Handle(Law_Function)& L);
  
  //! compute the section for v = param
  Standard_EXPORT virtual Standard_Boolean D0 (const Standard_Real Param, TColgp_Array1OfPnt& Poles, TColStd_Array1OfReal& Weigths) Standard_OVERRIDE;
  
  //! compute the first  derivative in v direction  of the
  //! section for v =  param
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual Standard_Boolean D1 (const Standard_Real Param, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColStd_Array1OfReal& Weigths, TColStd_Array1OfReal& DWeigths) Standard_OVERRIDE;
  
  //! compute the second derivative  in v direction of the
  //! section  for v = param
  //! Warning : It used only for C2 approximation
  Standard_EXPORT virtual Standard_Boolean D2 (const Standard_Real Param, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColgp_Array1OfVec& D2Poles, TColStd_Array1OfReal& Weigths, TColStd_Array1OfReal& DWeigths, TColStd_Array1OfReal& D2Weigths) Standard_OVERRIDE;
  
  //! give if possible an bspline Surface, like iso-v are the
  //! section.  If it is  not possible  this methode have  to
  //! get an Null Surface.  Is it the default implementation.
  Standard_EXPORT virtual Handle(Geom_BSplineSurface) BSplineSurface() const Standard_OVERRIDE;
  
  //! get the format of an  section
  Standard_EXPORT virtual void SectionShape (Standard_Integer& NbPoles, Standard_Integer& NbKnots, Standard_Integer& Degree) const Standard_OVERRIDE;
  
  //! get the Knots of the section
  Standard_EXPORT virtual void Knots (TColStd_Array1OfReal& TKnots) const Standard_OVERRIDE;
  
  //! get the Multplicities of the section
  Standard_EXPORT virtual void Mults (TColStd_Array1OfInteger& TMults) const Standard_OVERRIDE;
  
  //! Returns if the sections are rationnal or not
  Standard_EXPORT virtual Standard_Boolean IsRational() const Standard_OVERRIDE;
  
  //! Returns if the sections are periodic or not
  Standard_EXPORT virtual Standard_Boolean IsUPeriodic() const Standard_OVERRIDE;
  
  //! Returns if the law  isperiodic or not
  Standard_EXPORT virtual Standard_Boolean IsVPeriodic() const Standard_OVERRIDE;
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval (const Standard_Real First, const Standard_Real Last) Standard_OVERRIDE;
  
  //! Gets the bounds of the parametric interval on
  //! the function
  Standard_EXPORT virtual void GetInterval (Standard_Real& First, Standard_Real& Last) const Standard_OVERRIDE;
  
  //! Gets the bounds of the function parametric domain.
  //! Warning: This domain it is  not modified by the
  //! SetValue method
  Standard_EXPORT virtual void GetDomain (Standard_Real& First, Standard_Real& Last) const Standard_OVERRIDE;
  
  //! Returns the tolerances associated at each poles to
  //! reach  in approximation, to satisfy: BoundTol error
  //! at the   Boundary  AngleTol tangent error  at  the
  //! Boundary  (in radian)  SurfTol   error inside the
  //! surface.
  Standard_EXPORT virtual void GetTolerance (const Standard_Real BoundTol, const Standard_Real SurfTol, const Standard_Real AngleTol, TColStd_Array1OfReal& Tol3d) const Standard_OVERRIDE;
  
  //! Get the barycentre of Surface.
  //! An   very  poor estimation is sufficient.
  //! This information is useful to perform well
  //! conditioned rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT virtual gp_Pnt BarycentreOfSurf() const Standard_OVERRIDE;
  
  //! Returns the   length of the greater section. This
  //! information is useful to G1's control.
  //! Warning: With an little value, approximation can be slower.
  Standard_EXPORT virtual Standard_Real MaximalSection() const Standard_OVERRIDE;
  
  //! Compute the minimal value of weight for each poles
  //! in all  sections.
  //! This information is  useful to control error
  //! in rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT virtual void GetMinimalWeight (TColStd_Array1OfReal& Weigths) const Standard_OVERRIDE;
  
  //! return True If the Law isConstant
  Standard_EXPORT virtual Standard_Boolean IsConstant (Standard_Real& Error) const Standard_OVERRIDE;
  
  //! Return the constant Section if <me>  IsConstant.
  Standard_EXPORT virtual Handle(Geom_Curve) ConstantSection() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GeomFill_EvolvedSection,GeomFill_SectionLaw)

protected:




private:


  Standard_Real First;
  Standard_Real Last;
  Handle(Geom_Curve) mySection;
  Handle(Law_Function) myLaw;
  Handle(Law_Function) TLaw;
  Handle(Geom_BSplineCurve) myCurve;


};







#endif // _GeomFill_EvolvedSection_HeaderFile
