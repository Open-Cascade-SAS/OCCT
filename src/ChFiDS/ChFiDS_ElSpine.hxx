// Created on: 1995-05-04
// Created by: Laurent BOURESCHE
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

#ifndef _ChFiDS_ElSpine_HeaderFile
#define _ChFiDS_ElSpine_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TColgp_SequenceOfAx1.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_CurveType.hxx>
class ChFiDS_SurfData;
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class Adaptor3d_HCurve;
class gp_Pnt;
class gp_Vec;
class gp_Ax1;
class Geom_Curve;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Hypr;
class gp_Parab;
class Geom_BezierCurve;
class Geom_BSplineCurve;


//! Elementary  Spine for cheminements and approximations.
class ChFiDS_ElSpine  : public Adaptor3d_Curve
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ChFiDS_ElSpine();
  
  Standard_EXPORT virtual Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Real LastParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real GetSavedFirstParameter() const;
  
  Standard_EXPORT Standard_Real GetSavedLastParameter() const;
  
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  Standard_EXPORT void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Returns    a  curve equivalent   of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  Standard_EXPORT virtual Handle(Adaptor3d_HCurve) Trim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Real Resolution (const Standard_Real R3d) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual GeomAbs_CurveType GetType() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  
  Standard_EXPORT void SetPeriodic (const Standard_Boolean I);
  
  Standard_EXPORT virtual Standard_Real Period() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual gp_Pnt Value (const Standard_Real AbsC) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void D0 (const Standard_Real AbsC, gp_Pnt& P) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void D1 (const Standard_Real AbsC, gp_Pnt& P, gp_Vec& V1) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void D2 (const Standard_Real AbsC, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void D3 (const Standard_Real AbsC, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const Standard_OVERRIDE;
  
  Standard_EXPORT void FirstParameter (const Standard_Real P);
  
  Standard_EXPORT void LastParameter (const Standard_Real P);
  
  Standard_EXPORT void SaveFirstParameter();
  
  Standard_EXPORT void SaveLastParameter();
  
  Standard_EXPORT void SetOrigin (const Standard_Real O);
  
  Standard_EXPORT void FirstPointAndTgt (gp_Pnt& P, gp_Vec& T) const;
  
  Standard_EXPORT void LastPointAndTgt (gp_Pnt& P, gp_Vec& T) const;
  
  Standard_EXPORT Standard_Integer NbVertices() const;
  
  Standard_EXPORT const gp_Ax1& VertexWithTangent (const Standard_Integer Index) const;
  
  Standard_EXPORT void SetFirstPointAndTgt (const gp_Pnt& P, const gp_Vec& T);
  
  Standard_EXPORT void SetLastPointAndTgt (const gp_Pnt& P, const gp_Vec& T);
  
  Standard_EXPORT void AddVertexWithTangent (const gp_Ax1& anAx1);
  
  Standard_EXPORT void SetCurve (const Handle(Geom_Curve)& C);
  
  Standard_EXPORT const Handle(ChFiDS_SurfData)& Previous() const;
  
  Standard_EXPORT Handle(ChFiDS_SurfData)& ChangePrevious();
  
  Standard_EXPORT const Handle(ChFiDS_SurfData)& Next() const;
  
  Standard_EXPORT Handle(ChFiDS_SurfData)& ChangeNext();
  
  Standard_EXPORT gp_Lin Line() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Circ Circle() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Elips Ellipse() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Hypr Hyperbola() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Parab Parabola() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom_BezierCurve) Bezier() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom_BSplineCurve) BSpline() const Standard_OVERRIDE;




protected:





private:



  GeomAdaptor_Curve curve;
  gp_Pnt ptfirst;
  gp_Pnt ptlast;
  gp_Vec tgfirst;
  gp_Vec tglast;
  TColgp_SequenceOfAx1 VerticesWithTangents;
  Handle(ChFiDS_SurfData) previous;
  Handle(ChFiDS_SurfData) next;
  Standard_Real pfirst;
  Standard_Real plast;
  Standard_Real period;
  Standard_Boolean periodic;
  Standard_Real pfirstsav;
  Standard_Real plastsav;


};







#endif // _ChFiDS_ElSpine_HeaderFile
