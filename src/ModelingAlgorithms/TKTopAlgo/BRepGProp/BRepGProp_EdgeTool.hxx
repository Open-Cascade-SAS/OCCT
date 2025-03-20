// Created on: 1993-12-07
// Created by: Modelistation
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

#ifndef _BRepGProp_EdgeTool_HeaderFile
#define _BRepGProp_EdgeTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
class BRepAdaptor_Curve;
class gp_Pnt;
class gp_Vec;

//! Provides  the required  methods    to instantiate
//! CGProps from GProp with a Curve from BRepAdaptor.
class BRepGProp_EdgeTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the parametric value of the start point of
  //! the curve.  The curve is oriented from the start point
  //! to the end point.
  Standard_EXPORT static Standard_Real FirstParameter(const BRepAdaptor_Curve& C);

  //! Returns the parametric value of the end point of
  //! the curve.  The curve is oriented from the start point
  //! to the end point.
  Standard_EXPORT static Standard_Real LastParameter(const BRepAdaptor_Curve& C);

  //! Returns the number of Gauss points required to do
  //! the integration with a good accuracy using the
  //! Gauss method.  For a polynomial curve of degree n
  //! the maxima of accuracy is obtained with an order
  //! of integration equal to 2*n-1.
  Standard_EXPORT static Standard_Integer IntegrationOrder(const BRepAdaptor_Curve& C);

  //! Returns the point of parameter U on the loaded curve.
  Standard_EXPORT static gp_Pnt Value(const BRepAdaptor_Curve& C, const Standard_Real U);

  //! Returns the point of parameter U and the first derivative
  //! at this point.
  Standard_EXPORT static void D1(const BRepAdaptor_Curve& C,
                                 const Standard_Real      U,
                                 gp_Pnt&                  P,
                                 gp_Vec&                  V1);

  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT static Standard_Integer NbIntervals(const BRepAdaptor_Curve& C,
                                                      const GeomAbs_Shape      S);

  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT static void Intervals(const BRepAdaptor_Curve& C,
                                        TColStd_Array1OfReal&    T,
                                        const GeomAbs_Shape      S);

protected:
private:
};

#endif // _BRepGProp_EdgeTool_HeaderFile
