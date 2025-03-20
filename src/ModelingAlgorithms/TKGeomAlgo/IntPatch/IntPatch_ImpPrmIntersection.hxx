// Created on: 1992-05-07
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

#ifndef _IntPatch_ImpPrmIntersection_HeaderFile
#define _IntPatch_ImpPrmIntersection_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <IntPatch_SequenceOfPoint.hxx>
#include <IntPatch_SequenceOfLine.hxx>
#include <IntPatch_TheSOnBounds.hxx>
#include <IntPatch_TheSearchInside.hxx>

class Adaptor3d_TopolTool;

//! Implementation of the intersection between a natural
//! quadric patch : Plane, Cone, Cylinder or Sphere and
//! a bi-parametrised surface.
class IntPatch_ImpPrmIntersection
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntPatch_ImpPrmIntersection();

  Standard_EXPORT IntPatch_ImpPrmIntersection(const Handle(Adaptor3d_Surface)&   Surf1,
                                              const Handle(Adaptor3d_TopolTool)& D1,
                                              const Handle(Adaptor3d_Surface)&   Surf2,
                                              const Handle(Adaptor3d_TopolTool)& D2,
                                              const Standard_Real                TolArc,
                                              const Standard_Real                TolTang,
                                              const Standard_Real                Fleche,
                                              const Standard_Real                Pas);

  //! to search for solution from the given point
  Standard_EXPORT void SetStartPoint(const Standard_Real U, const Standard_Real V);

  Standard_EXPORT void Perform(const Handle(Adaptor3d_Surface)&   Surf1,
                               const Handle(Adaptor3d_TopolTool)& D1,
                               const Handle(Adaptor3d_Surface)&   Surf2,
                               const Handle(Adaptor3d_TopolTool)& D2,
                               const Standard_Real                TolArc,
                               const Standard_Real                TolTang,
                               const Standard_Real                Fleche,
                               const Standard_Real                Pas);

  //! Returns true if the calculus was successful.
  Standard_Boolean IsDone() const;

  //! Returns true if the is no intersection.
  Standard_Boolean IsEmpty() const;

  //! Returns the number of "single" points.
  Standard_Integer NbPnts() const;

  //! Returns the point of range Index.
  //! An exception is raised if Index<=0 or Index>NbPnt.
  const IntPatch_Point& Point(const Standard_Integer Index) const;

  //! Returns the number of intersection lines.
  Standard_Integer NbLines() const;

  //! Returns the line of range Index.
  //! An exception is raised if Index<=0 or Index>NbLine.
  const Handle(IntPatch_Line)& Line(const Standard_Integer Index) const;

protected:
private:
  Standard_Boolean         done;
  Standard_Boolean         empt;
  IntPatch_SequenceOfPoint spnt;
  IntPatch_SequenceOfLine  slin;
  IntPatch_TheSOnBounds    solrst;
  IntPatch_TheSearchInside solins;
  Standard_Boolean         myIsStartPnt;
  Standard_Real            myUStart;
  Standard_Real            myVStart;
};

#include <IntPatch_ImpPrmIntersection.lxx>

#endif // _IntPatch_ImpPrmIntersection_HeaderFile
