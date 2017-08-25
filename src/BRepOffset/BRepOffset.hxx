// Created on: 1995-10-12
// Created by: Bruno DUMORTIER
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

#ifndef _BRepOffset_HeaderFile
#define _BRepOffset_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepOffset_Status.hxx>

class Geom_Surface;
class TopoDS_Face;

//! Auxiliary tools for offset algorithms

class BRepOffset 
{
public:

  //! returns   the  Offset  surface  computed from  the
  //! surface <Surface> at an OffsetDistance <Offset>.
  //!
  //! If possible, this method returns  the real type of
  //! the surface ( e.g. An Offset of a plane is a plane).
  //!
  //! If  no particular  case  is detected, the returned
  //! surface will have the Type Geom_OffsetSurface.
  //! Parameter allowC0 is then passed as last argument to 
  //! constructor of Geom_OffsetSurface.
  Standard_EXPORT static Handle(Geom_Surface) Surface (const Handle(Geom_Surface)& Surface, 
                                                       const Standard_Real Offset, 
                                                       BRepOffset_Status& theStatus,
                                                       Standard_Boolean allowC0 = Standard_False);

  //! Preprocess surface to be offset (bspline, bezier, or revolution based on
  //! bspline or bezier curve), by collapsing each singular side to single point.
  //!
  //! This is to avoid possible flipping of normal at the singularity 
  //! of the surface due to non-zero distance between the poles that
  //! logically should be in one point (singularity).
  //! 
  //! The (parametric) side of the surface is considered to be singularity if face 
  //! has degenerated edge whose vertex encompasses (by its tolerance) all points on that side,
  //! or if all poles defining that side fit into sphere with radius thePrecision.
  //!
  //! Returns either original surface or its modified copy (if some poles have been moved).
  Standard_EXPORT static Handle(Geom_Surface) CollapseSingularities (const Handle(Geom_Surface)& theSurface, 
                                                                     const TopoDS_Face& theFace,
                                                                     Standard_Real thePrecision);

};

#endif // _BRepOffset_HeaderFile
