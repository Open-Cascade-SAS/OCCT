// Created on: 1992-09-25
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntSurf_PathPointTool_HeaderFile
#define _IntSurf_PathPointTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir2d.hxx>
#include <Standard_Integer.hxx>
class Standard_OutOfRange;
class StdFail_UndefinedDerivative;
class IntSurf_PathPoint;

class IntSurf_PathPointTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the 3d coordinates of the starting point.
  static gp_Pnt Value3d(const IntSurf_PathPoint& PStart);

  //! Returns the <U, V> parameters which are associated
  //! with <P>
  //! it's the parameters which start the marching algorithm
  static void Value2d(const IntSurf_PathPoint& PStart, Standard_Real& U, Standard_Real& V);

  //! Returns True if the point is a point on a non-oriented
  //! arc, which means that the intersection line does not
  //! stop at such a point but just go through such a point.
  //! IsPassingPnt is True when IsOnArc is True
  static Standard_Boolean IsPassingPnt(const IntSurf_PathPoint& PStart);

  //! Returns True if the surfaces are tangent at this point.
  //! IsTangent can be True when IsOnArc is True
  //! if IsPassingPnt is True and IsTangent is True,this point
  //! is a stopped point.
  static Standard_Boolean IsTangent(const IntSurf_PathPoint& PStart);

  //! returns the tangent at the intersection in 3d space
  //! associated to <P>
  //! an exception is raised if IsTangent is true.
  static gp_Vec Direction3d(const IntSurf_PathPoint& PStart);

  //! returns the tangent at the intersection in the
  //! parametric space of the parametrized surface.This tangent
  //! is associated to the value2d
  //! la tangente a un sens signifiant (indique le sens de chemin
  //! ement)
  //! an exception is raised if IsTangent is true.
  static gp_Dir2d Direction2d(const IntSurf_PathPoint& PStart);

  //! Returns the multiplicity of the point i-e
  //! the number of auxillar parameters associated to the
  //! point which the principal parameters are given by Value2d
  static Standard_Integer Multiplicity(const IntSurf_PathPoint& PStart);

  //! Parametric coordinates associated to the multiplicity.
  //! An exception is raised if Mult<=0 or Mult>multiplicity.
  static void Parameters(const IntSurf_PathPoint& PStart,
                         const Standard_Integer   Mult,
                         Standard_Real&           U,
                         Standard_Real&           V);

protected:
private:
};

#include <IntSurf_PathPointTool.lxx>

#endif // _IntSurf_PathPointTool_HeaderFile
