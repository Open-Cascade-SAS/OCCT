// Created on: 1993-03-24
// Created by: JCV
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

#ifndef _Geom2d_BoundedCurve_HeaderFile
#define _Geom2d_BoundedCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2d_Curve.hxx>
class gp_Pnt2d;


class Geom2d_BoundedCurve;
DEFINE_STANDARD_HANDLE(Geom2d_BoundedCurve, Geom2d_Curve)

//! The abstract class BoundedCurve describes the
//! common behavior of bounded curves in 2D space. A
//! bounded curve is limited by two finite values of the
//! parameter, termed respectively "first parameter" and
//! "last parameter". The "first parameter" gives the "start
//! point" of the bounded curve, and the "last parameter"
//! gives the "end point" of the bounded curve.
//! The length of a bounded curve is finite.
//! The Geom2d package provides three concrete
//! classes of bounded curves:
//! - two frequently used mathematical formulations of complex curves:
//! - Geom2d_BezierCurve,
//! - Geom2d_BSplineCurve, and
//! - Geom2d_TrimmedCurve to trim a curve, i.e. to
//! only take part of the curve limited by two values of
//! the parameter of the basis curve.
class Geom2d_BoundedCurve : public Geom2d_Curve
{

public:

  

  //! Returns the end point of the curve.
  //! The end point is the value of the curve for the
  //! "LastParameter" of the curve.
  Standard_EXPORT virtual gp_Pnt2d EndPoint() const = 0;
  

  //! Returns the start point of the curve.
  //! The start point is the value of the curve for the
  //! "FirstParameter" of the curve.
  Standard_EXPORT virtual gp_Pnt2d StartPoint() const = 0;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom2d_BoundedCurve,Geom2d_Curve)

protected:




private:




};







#endif // _Geom2d_BoundedCurve_HeaderFile
