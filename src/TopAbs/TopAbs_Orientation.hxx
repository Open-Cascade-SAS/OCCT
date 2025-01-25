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

#ifndef _TopAbs_Orientation_HeaderFile
#define _TopAbs_Orientation_HeaderFile

//! Identifies the orientation of a topological shape.
//! Orientation can represent a relation between two
//! entities, or it can apply to a shape in its own right.
//! When used to describe a relation between two
//! shapes, orientation allows you to use the underlying
//! entity in either direction. For example on a curve
//! which is oriented FORWARD (say from left to right)
//! you can have both a FORWARD and a REVERSED
//! edge. The FORWARD edge will be oriented from
//! left to right, and the REVERSED edge from right to
//! left. In this way, you share the underlying entity. In
//! other words, two faces of a cube can share an
//! edge, and can also be used to build compound shapes.
//! For each case in which an element is used as the
//! boundary of a geometric domain of a higher
//! dimension, this element defines two local regions of
//! which one is arbitrarily considered as the default
//! region. A change in orientation implies a switch of
//! default region. This allows you to apply changes of
//! orientation to the shape as a whole.
enum TopAbs_Orientation
{
  TopAbs_FORWARD,
  TopAbs_REVERSED,
  TopAbs_INTERNAL,
  TopAbs_EXTERNAL
};

#endif // _TopAbs_Orientation_HeaderFile
