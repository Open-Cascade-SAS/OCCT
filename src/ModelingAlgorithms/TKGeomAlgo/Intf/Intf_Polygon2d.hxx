// Created on: 2012-02-10
// Created by: Serey ZERCHANINOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _Intf_Polygon2d_HeaderFile
#define _Intf_Polygon2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_Box2d.hxx>
#include <Standard_Boolean.hxx>
class gp_Pnt2d;

//! Describes the necessary polygon information to compute
//! the interferences.
class Intf_Polygon2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the bounding box of the polygon.
  const Bnd_Box2d& Bounding() const;

  //! Returns True if the polyline is closed.
  Standard_EXPORT virtual Standard_Boolean Closed() const;

  virtual ~Intf_Polygon2d() {}

  //! Returns the tolerance of the polygon.
  Standard_EXPORT virtual Standard_Real DeflectionOverEstimation() const = 0;

  //! Returns the number of Segments in the polyline.
  Standard_EXPORT virtual Standard_Integer NbSegments() const = 0;

  //! Returns the points of the segment <Index> in the Polygon.
  Standard_EXPORT virtual void Segment(const Standard_Integer theIndex,
                                       gp_Pnt2d&              theBegin,
                                       gp_Pnt2d&              theEnd) const = 0;

protected:
  Bnd_Box2d myBox;

private:
};

#include <Intf_Polygon2d.lxx>

#endif // _Intf_Polygon2d_HeaderFile
