// Created on: 2013-02-05
// Created by: Julia GERASIMOVA
// Copyright (c) 2001-2013 OPEN CASCADE SAS
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

#ifndef _GeomFill_DiscreteTrihedron_HeaderFile
#define _GeomFill_DiscreteTrihedron_HeaderFile

#include <Standard.hxx>

#include <GeomFill_HSequenceOfAx2.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
class GeomFill_Frenet;
class gp_Vec;

class GeomFill_DiscreteTrihedron;
DEFINE_STANDARD_HANDLE(GeomFill_DiscreteTrihedron, GeomFill_TrihedronLaw)

//! Defined Discrete Trihedron Law.
//! The requirement for path curve is only G1.
//! The result is C0-continuous surface
//! that can be later approximated to C1.
class GeomFill_DiscreteTrihedron : public GeomFill_TrihedronLaw
{

public:
  Standard_EXPORT GeomFill_DiscreteTrihedron();

  Standard_EXPORT virtual Handle(GeomFill_TrihedronLaw) Copy() const Standard_OVERRIDE;

  Standard_EXPORT void Init();

  //! initialize curve of trihedron law
  //! @return Standard_True in case if execution end correctly
  Standard_EXPORT virtual Standard_Boolean SetCurve(const Handle(Adaptor3d_Curve)& C)
    Standard_OVERRIDE;

  //! compute Trihedron on curve at parameter <Param>
  Standard_EXPORT virtual Standard_Boolean D0(const Standard_Real Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             BiNormal) Standard_OVERRIDE;

  //! compute Trihedron and  derivative Trihedron  on curve
  //! at parameter <Param>
  //! Warning : It used only for C1 or C2 approximation
  //! For the moment it returns null values for DTangent, DNormal
  //! and DBiNormal.
  Standard_EXPORT virtual Standard_Boolean D1(const Standard_Real Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             DTangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             DNormal,
                                              gp_Vec&             BiNormal,
                                              gp_Vec&             DBiNormal) Standard_OVERRIDE;

  //! compute  Trihedron on curve
  //! first and seconde  derivatives.
  //! Warning : It used only for C2 approximation
  //! For the moment it returns null values for DTangent, DNormal
  //! DBiNormal, D2Tangent, D2Normal, D2BiNormal.
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

  //! Get average value of Tangent(t) and Normal(t) it is usful to
  //! make fast approximation of rational  surfaces.
  Standard_EXPORT virtual void GetAverageLaw(gp_Vec& ATangent,
                                             gp_Vec& ANormal,
                                             gp_Vec& ABiNormal) Standard_OVERRIDE;

  //! Say if the law is Constant.
  Standard_EXPORT virtual Standard_Boolean IsConstant() const Standard_OVERRIDE;

  //! Return True.
  Standard_EXPORT virtual Standard_Boolean IsOnlyBy3dCurve() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(GeomFill_DiscreteTrihedron, GeomFill_TrihedronLaw)

protected:
private:
  gp_Pnt                          myPoint;
  Handle(GeomFill_HSequenceOfAx2) myTrihedrons;
  Handle(TColStd_HSequenceOfReal) myKnots;
  Handle(GeomFill_Frenet)         myFrenet;
  Standard_Boolean                myUseFrenet;
};

#endif // _GeomFill_DiscreteTrihedron_HeaderFile
