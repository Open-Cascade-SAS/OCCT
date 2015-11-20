// Created on: 1993-12-02
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BlendFunc_CSConstRad_HeaderFile
#define _BlendFunc_CSConstRad_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_Integer.hxx>
#include <BlendFunc_SectionShape.hxx>
#include <Convert_ParameterisationType.hxx>
#include <Blend_CSFunction.hxx>
#include <math_Vector.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfInteger.hxx>
class Adaptor3d_HSurface;
class Adaptor3d_HCurve;
class math_Matrix;
class gp_Pnt;
class gp_Pnt2d;
class gp_Vec;
class gp_Vec2d;
class gp_Circ;
class Blend_Point;



class BlendFunc_CSConstRad  : public Blend_CSFunction
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BlendFunc_CSConstRad(const Handle(Adaptor3d_HSurface)& S, const Handle(Adaptor3d_HCurve)& C, const Handle(Adaptor3d_HCurve)& CGuide);
  
  //! returns the number of equations of the function (3).
  Standard_EXPORT Standard_Integer NbEquations() const Standard_OVERRIDE;
  
  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value (const math_Vector& X, math_Vector& F) Standard_OVERRIDE;
  
  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D) Standard_OVERRIDE;
  
  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Values (const math_Vector& X, math_Vector& F, math_Matrix& D) Standard_OVERRIDE;
  
  Standard_EXPORT void Set (const Standard_Real Param) Standard_OVERRIDE;
  
  Standard_EXPORT void Set (const Standard_Real First, const Standard_Real Last) Standard_OVERRIDE;
  
  Standard_EXPORT void GetTolerance (math_Vector& Tolerance, const Standard_Real Tol) const Standard_OVERRIDE;
  
  Standard_EXPORT void GetBounds (math_Vector& InfBound, math_Vector& SupBound) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsSolution (const math_Vector& Sol, const Standard_Real Tol) Standard_OVERRIDE;
  
  Standard_EXPORT const gp_Pnt& PointOnS() const Standard_OVERRIDE;
  
  Standard_EXPORT const gp_Pnt& PointOnC() const Standard_OVERRIDE;
  
  //! Returns U,V coordinates of the point on the surface.
  Standard_EXPORT const gp_Pnt2d& Pnt2d() const Standard_OVERRIDE;
  
  //! Returns parameter of the point on the curve.
  Standard_EXPORT Standard_Real ParameterOnC() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsTangencyPoint() const Standard_OVERRIDE;
  
  Standard_EXPORT const gp_Vec& TangentOnS() const Standard_OVERRIDE;
  
  Standard_EXPORT const gp_Vec2d& Tangent2d() const Standard_OVERRIDE;
  
  Standard_EXPORT const gp_Vec& TangentOnC() const Standard_OVERRIDE;
  
  //! Returns the tangent vector at the section,
  //! at the beginning and the end of the section, and
  //! returns the normal (of the surface) at
  //! these points.
  Standard_EXPORT void Tangent (const Standard_Real U, const Standard_Real V, gp_Vec& TgS, gp_Vec& NormS) const Standard_OVERRIDE;
  
  Standard_EXPORT void Set (const Standard_Real Radius, const Standard_Integer Choix);
  
  //! Sets  the  type  of   section generation   for the
  //! approximations.
  Standard_EXPORT void Set (const BlendFunc_SectionShape TypeSection);
  
  Standard_EXPORT void Section (const Standard_Real Param, const Standard_Real U, const Standard_Real V, const Standard_Real W, Standard_Real& Pdeb, Standard_Real& Pfin, gp_Circ& C);
  
  //! Used for the first and last section
  //! The method returns Standard_True if the derivatives
  //! are computed, otherwise it returns Standard_False.
  Standard_EXPORT virtual Standard_Boolean Section (const Blend_Point& P, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColgp_Array1OfVec& D2Poles, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d, TColgp_Array1OfVec2d& D2Poles2d, TColStd_Array1OfReal& Weigths, TColStd_Array1OfReal& DWeigths, TColStd_Array1OfReal& D2Weigths) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean GetSection (const Standard_Real Param, const Standard_Real U, const Standard_Real V, const Standard_Real W, TColgp_Array1OfPnt& tabP, TColgp_Array1OfVec& tabV);
  
  //! Returns  if the section is rationnal
  Standard_EXPORT Standard_Boolean IsRational() const Standard_OVERRIDE;
  
  //! Returns the length of the maximum section
  Standard_EXPORT Standard_Real GetSectionSize() const Standard_OVERRIDE;
  
  //! Compute the minimal value of weight for each poles
  //! of all sections.
  Standard_EXPORT void GetMinimalWeight (TColStd_Array1OfReal& Weigths) const Standard_OVERRIDE;
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //! The array must provide  enough room to  accomodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  //! raises
  //! OutOfRange from Standard
  Standard_EXPORT void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  Standard_EXPORT void GetShape (Standard_Integer& NbPoles, Standard_Integer& NbKnots, Standard_Integer& Degree, Standard_Integer& NbPoles2d) Standard_OVERRIDE;
  
  //! Returns the tolerance to reach in approximation
  //! to respecte
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary
  //! SurfTol error inside the surface.
  Standard_EXPORT void GetTolerance (const Standard_Real BoundTol, const Standard_Real SurfTol, const Standard_Real AngleTol, math_Vector& Tol3d, math_Vector& Tol1D) const Standard_OVERRIDE;
  
  Standard_EXPORT void Knots (TColStd_Array1OfReal& TKnots) Standard_OVERRIDE;
  
  Standard_EXPORT void Mults (TColStd_Array1OfInteger& TMults) Standard_OVERRIDE;
  
  //! Used for the first and last section
  Standard_EXPORT Standard_Boolean Section (const Blend_Point& P, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d, TColStd_Array1OfReal& Weigths, TColStd_Array1OfReal& DWeigths) Standard_OVERRIDE;
  
  Standard_EXPORT void Section (const Blend_Point& P, TColgp_Array1OfPnt& Poles, TColgp_Array1OfPnt2d& Poles2d, TColStd_Array1OfReal& Weigths) Standard_OVERRIDE;
  
  Standard_EXPORT void Resolution (const Standard_Integer IC2d, const Standard_Real Tol, Standard_Real& TolU, Standard_Real& TolV) const Standard_OVERRIDE;




protected:





private:



  Handle(Adaptor3d_HSurface) surf;
  Handle(Adaptor3d_HCurve) curv;
  Handle(Adaptor3d_HCurve) guide;
  gp_Pnt pts;
  gp_Pnt ptc;
  gp_Pnt2d pt2d;
  Standard_Real prmc;
  Standard_Boolean istangent;
  gp_Vec tgs;
  gp_Vec2d tg2d;
  gp_Vec tgc;
  Standard_Real ray;
  Standard_Integer choix;
  gp_Pnt ptgui;
  gp_Vec d1gui;
  gp_Vec d2gui;
  gp_Vec nplan;
  Standard_Real normtg;
  Standard_Real theD;
  Standard_Real maxang;
  Standard_Real minang;
  BlendFunc_SectionShape mySShape;
  Convert_ParameterisationType myTConv;


};







#endif // _BlendFunc_CSConstRad_HeaderFile
