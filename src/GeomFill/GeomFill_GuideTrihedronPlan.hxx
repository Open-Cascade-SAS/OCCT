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

#ifndef _GeomFill_GuideTrihedronPlan_HeaderFile
#define _GeomFill_GuideTrihedronPlan_HeaderFile

#include <Standard.hxx>

#include <TColgp_HArray2OfPnt2d.hxx>
#include <math_Vector.hxx>
#include <Standard_Integer.hxx>
#include <GeomFill_TrihedronWithGuide.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>

class GeomFill_Frenet;
class GeomFill_TrihedronLaw;
class gp_Vec;

class GeomFill_GuideTrihedronPlan;
DEFINE_STANDARD_HANDLE(GeomFill_GuideTrihedronPlan, GeomFill_TrihedronWithGuide)

//! Trihedron in  the case of sweeping along a guide curve defined
//! by the orthogonal  plan on  the trajectory
class GeomFill_GuideTrihedronPlan : public GeomFill_TrihedronWithGuide
{

public:
  Standard_EXPORT GeomFill_GuideTrihedronPlan(const Handle(Adaptor3d_Curve)& theGuide);

  //! initialize curve of trihedron law
  //! @return Standard_True in case if execution end correctly
  Standard_EXPORT virtual Standard_Boolean SetCurve(const Handle(Adaptor3d_Curve)& thePath)
    Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(GeomFill_TrihedronLaw) Copy() const Standard_OVERRIDE;

  //! Give a status to the Law
  //! Returns PipeOk (default implementation)
  Standard_EXPORT virtual GeomFill_PipeError ErrorStatus() const Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Adaptor3d_Curve) Guide() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean D0(const Standard_Real Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             BiNormal) Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean D1(const Standard_Real Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             DTangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             DNormal,
                                              gp_Vec&             BiNormal,
                                              gp_Vec&             DBiNormal) Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean D2(const Standard_Real Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             DTangent,
                                              gp_Vec&             D2Tangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             DNormal,
                                              gp_Vec&             D2Normal,
                                              gp_Vec&             BiNormal,
                                              gp_Vec&             DBiNormal,
                                              gp_Vec&             D2BiNormal) Standard_OVERRIDE;

  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval(const Standard_Real First,
                                           const Standard_Real Last) Standard_OVERRIDE;

  //! Returns  the number  of  intervals for  continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbIntervals(const GeomAbs_Shape S) const
    Standard_OVERRIDE;

  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(TColStd_Array1OfReal& T,
                                         const GeomAbs_Shape   S) const Standard_OVERRIDE;

  //! Get average value of M(t) and V(t) it is usfull to
  //! make fast approximation of rational  surfaces.
  Standard_EXPORT virtual void GetAverageLaw(gp_Vec& ATangent,
                                             gp_Vec& ANormal,
                                             gp_Vec& ABiNormal) Standard_OVERRIDE;

  //! Say if the law is Constant
  Standard_EXPORT virtual Standard_Boolean IsConstant() const Standard_OVERRIDE;

  //! Say if the law is defined, only by the 3d Geometry of
  //! the set Curve
  //! Return False by Default.
  Standard_EXPORT virtual Standard_Boolean IsOnlyBy3dCurve() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Origine(const Standard_Real OrACR1,
                                       const Standard_Real OrACR2) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(GeomFill_GuideTrihedronPlan, GeomFill_TrihedronWithGuide)

protected:
private:
  Standard_EXPORT void Init();

  Standard_EXPORT void InitX(const Standard_Real Param);

  Handle(Adaptor3d_Curve)       myTrimmed;
  Handle(Adaptor3d_Curve)       myCurve;
  Handle(TColgp_HArray2OfPnt2d) Pole;
  math_Vector                   X;
  math_Vector                   XTol;
  math_Vector                   Inf;
  math_Vector                   Sup;
  Handle(GeomFill_Frenet)       frenet;
  Standard_Integer              myNbPts;
  GeomFill_PipeError            myStatus;
};

#endif // _GeomFill_GuideTrihedronPlan_HeaderFile
