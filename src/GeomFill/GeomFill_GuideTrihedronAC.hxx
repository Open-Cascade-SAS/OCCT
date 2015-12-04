// Created on: 1998-06-23
// Created by: Stephanie HUMEAU
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

#ifndef _GeomFill_GuideTrihedronAC_HeaderFile
#define _GeomFill_GuideTrihedronAC_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <GeomFill_TrihedronWithGuide.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
class Approx_CurvlinFunc;
class Adaptor3d_HCurve;
class Standard_OutOfRange;
class Standard_ConstructionError;
class GeomFill_TrihedronLaw;
class gp_Vec;


class GeomFill_GuideTrihedronAC;
DEFINE_STANDARD_HANDLE(GeomFill_GuideTrihedronAC, GeomFill_TrihedronWithGuide)

//! Trihedron in  the  case of a sweeping along a guide  curve.
//! defined by curviline  absciss
class GeomFill_GuideTrihedronAC : public GeomFill_TrihedronWithGuide
{

public:

  
  Standard_EXPORT GeomFill_GuideTrihedronAC(const Handle(Adaptor3d_HCurve)& guide);
  
  Standard_EXPORT virtual void SetCurve (const Handle(Adaptor3d_HCurve)& C) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(GeomFill_TrihedronLaw) Copy() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Adaptor3d_HCurve) Guide() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean D0 (const Standard_Real Param, gp_Vec& Tangent, gp_Vec& Normal, gp_Vec& BiNormal) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean D1 (const Standard_Real Param, gp_Vec& Tangent, gp_Vec& DTangent, gp_Vec& Normal, gp_Vec& DNormal, gp_Vec& BiNormal, gp_Vec& DBiNormal) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean D2 (const Standard_Real Param, gp_Vec& Tangent, gp_Vec& DTangent, gp_Vec& D2Tangent, gp_Vec& Normal, gp_Vec& DNormal, gp_Vec& D2Normal, gp_Vec& BiNormal, gp_Vec& DBiNormal, gp_Vec& D2BiNormal) Standard_OVERRIDE;
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accomodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval (const Standard_Real First, const Standard_Real Last) Standard_OVERRIDE;
  
  //! Get average value of M(t) and V(t) it is usfull to
  //! make fast approximation of rational  surfaces.
  Standard_EXPORT virtual void GetAverageLaw (gp_Vec& ATangent, gp_Vec& ANormal, gp_Vec& ABiNormal) Standard_OVERRIDE;
  
  //! Say if the law is Constant
  Standard_EXPORT virtual Standard_Boolean IsConstant() const Standard_OVERRIDE;
  
  //! Say if the law is defined, only by the 3d Geometry of
  //! the setted Curve
  //! Return False by Default.
  Standard_EXPORT virtual Standard_Boolean IsOnlyBy3dCurve() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Origine (const Standard_Real OrACR1, const Standard_Real OrACR2) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GeomFill_GuideTrihedronAC,GeomFill_TrihedronWithGuide)

protected:




private:


  Handle(Approx_CurvlinFunc) myGuideAC;
  Standard_Real Lguide;
  Handle(Approx_CurvlinFunc) myCurveAC;
  Standard_Real L;
  Handle(Adaptor3d_HCurve) myCurve;
  Standard_Real UTol;
  Standard_Real STol;
  Standard_Real Orig1;
  Standard_Real Orig2;


};







#endif // _GeomFill_GuideTrihedronAC_HeaderFile
