// Created on: 1992-01-24
// Created by: Remi LEQUETTE
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

#ifndef _TopAbs_ShapeEnum_HeaderFile
#define _TopAbs_ShapeEnum_HeaderFile

//! Identifies various topological shapes. This
//! enumeration allows you to use dynamic typing of shapes.
//! The values are listed in order of complexity, from the
//! most complex to the most simple i.e.
//! COMPOUND > COMPSOLID > SOLID > .... > VERTEX > SHAPE.
//! Any shape can contain simpler shapes in its definition.
//! Abstract topological data structure describes a basic
//! entity, the shape (present in this enumeration as the
//! SHAPE value), which can be divided into the following
//! component topologies:
//! - COMPOUND: A group of any of the shapes below.
//! - COMPSOLID: A set of solids connected by their
//! faces. This expands the notions of WIRE and SHELL to solids.
//! - SOLID: A part of 3D space bounded by shells.
//! - SHELL: A set of faces connected by some of the
//! edges of their wire boundaries. A shell can be open or closed.
//! - FACE: Part of a plane (in 2D geometry) or a surface
//! (in 3D geometry) bounded by a closed wire. Its
//! geometry is constrained (trimmed) by contours.
//! - WIRE: A sequence of edges connected by their
//! vertices. It can be open or closed depending on
//! whether the edges are linked or not.
//! - EDGE: A single dimensional shape corresponding
//! to a curve, and bound by a vertex at each extremity.
//! - VERTEX: A zero-dimensional shape corresponding to a point in geometry.
enum TopAbs_ShapeEnum
{
TopAbs_COMPOUND,
TopAbs_COMPSOLID,
TopAbs_SOLID,
TopAbs_SHELL,
TopAbs_FACE,
TopAbs_WIRE,
TopAbs_EDGE,
TopAbs_VERTEX,
TopAbs_SHAPE
};

#endif // _TopAbs_ShapeEnum_HeaderFile
