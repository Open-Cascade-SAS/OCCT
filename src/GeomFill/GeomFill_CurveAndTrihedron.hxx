// Created on: 1997-12-02
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _GeomFill_CurveAndTrihedron_HeaderFile
#define _GeomFill_CurveAndTrihedron_HeaderFile

#include <Standard.hxx>

#include <gp_Mat.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <Standard_Real.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
class GeomFill_TrihedronLaw;


class GeomFill_CurveAndTrihedron;
DEFINE_STANDARD_HANDLE(GeomFill_CurveAndTrihedron, GeomFill_LocationLaw)

//! Define location law with an TrihedronLaw and an
//! curve
//! Definition Location is :
//! transformed  section   coordinates  in  (Curve(v)),
//! (Normal(v),   BiNormal(v), Tangente(v))) systeme are
//! the  same like section  shape coordinates in
//! (O,(OX, OY, OZ)) systeme.
class GeomFill_CurveAndTrihedron : public GeomFill_LocationLaw
{

public:

  
  Standard_EXPORT GeomFill_CurveAndTrihedron(const Handle(GeomFill_TrihedronLaw)& Trihedron);
  
  //! initialize curve of trihedron law
  //! @return Standard_True in case if execution end correctly
  Standard_EXPORT virtual Standard_Boolean SetCurve (const Handle(Adaptor3d_Curve)& C) Standard_OVERRIDE;
  
  Standard_EXPORT virtual const Handle(Adaptor3d_Curve)& GetCurve() const Standard_OVERRIDE;
  
  //! Set a transformation Matrix like   the law M(t) become
  //! Mat * M(t)
  Standard_EXPORT virtual void SetTrsf (const gp_Mat& Transfo) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(GeomFill_LocationLaw) Copy() const Standard_OVERRIDE;
  
  //! compute Location and 2d points
  Standard_EXPORT virtual Standard_Boolean D0 (const Standard_Real Param, gp_Mat& M, gp_Vec& V) Standard_OVERRIDE;
  
  //! compute Location and 2d points
  Standard_EXPORT virtual Standard_Boolean D0 (const Standard_Real Param, gp_Mat& M, gp_Vec& V, TColgp_Array1OfPnt2d& Poles2d) Standard_OVERRIDE;
  
  //! compute location 2d  points and  associated
  //! first derivatives.
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual Standard_Boolean D1 (const Standard_Real Param, gp_Mat& M, gp_Vec& V, gp_Mat& DM, gp_Vec& DV, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d) Standard_OVERRIDE;
  
  //! compute location 2d  points and associated
  //! first and seconde  derivatives.
  //! Warning : It used only for C2 approximation
  Standard_EXPORT virtual Standard_Boolean D2 (const Standard_Real Param, gp_Mat& M, gp_Vec& V, gp_Mat& DM, gp_Vec& DV, gp_Mat& D2M, gp_Vec& D2V, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d, TColgp_Array1OfVec2d& D2Poles2d) Standard_OVERRIDE;
  
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
  
  //! Get the maximum Norm  of the matrix-location part.  It
  //! is usful to find an good Tolerance to approx M(t).
  Standard_EXPORT virtual Standard_Real GetMaximalNorm() Standard_OVERRIDE;
  
  //! Get average value of M(t) and V(t) it is usfull to
  //! make fast approximation of rational  surfaces.
  Standard_EXPORT virtual void GetAverageLaw (gp_Mat& AM, gp_Vec& AV) Standard_OVERRIDE;
  
  //! Say if the Location  Law, is an translation of  Location
  //! The default implementation is " returns False ".
  Standard_EXPORT virtual Standard_Boolean IsTranslation (Standard_Real& Error) const Standard_OVERRIDE;
  
  //! Say if the Location  Law, is a rotation of Location
  //! The default implementation is " returns False ".
  Standard_EXPORT virtual Standard_Boolean IsRotation (Standard_Real& Error) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Rotation (gp_Pnt& Center) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GeomFill_CurveAndTrihedron,GeomFill_LocationLaw)

protected:




private:


  Standard_Boolean WithTrans;
  Handle(GeomFill_TrihedronLaw) myLaw;
  Handle(Adaptor3d_Curve) myCurve;
  Handle(Adaptor3d_Curve) myTrimmed;
  gp_Pnt Point;
  gp_Vec V1;
  gp_Vec V2;
  gp_Vec V3;
  gp_Mat Trans;


};







#endif // _GeomFill_CurveAndTrihedron_HeaderFile
