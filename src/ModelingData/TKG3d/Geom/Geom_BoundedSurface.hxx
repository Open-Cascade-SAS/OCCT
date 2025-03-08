// Created on: 1993-03-10
// Created by: Philippe DAUTRY
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

#ifndef _Geom_BoundedSurface_HeaderFile
#define _Geom_BoundedSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_Surface.hxx>

class Geom_BoundedSurface;
DEFINE_STANDARD_HANDLE(Geom_BoundedSurface, Geom_Surface)

//! The root class for bounded surfaces in 3D space. A
//! bounded surface is defined by a rectangle in its 2D parametric space, i.e.
//! - its u parameter, which ranges between two finite
//! values u0 and u1, referred to as "First u
//! parameter" and "Last u parameter" respectively, and
//! - its v parameter, which ranges between two finite
//! values v0 and v1, referred to as "First v
//! parameter" and the "Last v parameter" respectively.
//! The surface is limited by four curves which are the
//! boundaries of the surface:
//! - its u0 and u1 isoparametric curves in the u parametric direction, and
//! - its v0 and v1 isoparametric curves in the v parametric direction.
//! A bounded surface is finite.
//! The common behavior of all bounded surfaces is
//! described by the Geom_Surface class.
//! The Geom package provides three concrete
//! implementations of bounded surfaces:
//! - Geom_BezierSurface,
//! - Geom_BSplineSurface, and
//! - Geom_RectangularTrimmedSurface.
//! The first two of these implement well known
//! mathematical definitions of complex surfaces, the third
//! trims a surface using four isoparametric curves, i.e. it
//! limits the variation of its parameters to a rectangle in
//! 2D parametric space.
class Geom_BoundedSurface : public Geom_Surface
{

public:
  DEFINE_STANDARD_RTTIEXT(Geom_BoundedSurface, Geom_Surface)

protected:
private:
};

#endif // _Geom_BoundedSurface_HeaderFile
