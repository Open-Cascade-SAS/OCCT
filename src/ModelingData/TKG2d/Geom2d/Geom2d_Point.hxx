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

#ifndef _Geom2d_Point_HeaderFile
#define _Geom2d_Point_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2d_Geometry.hxx>
#include <Standard_Real.hxx>
class gp_Pnt2d;

class Geom2d_Point;
DEFINE_STANDARD_HANDLE(Geom2d_Point, Geom2d_Geometry)

//! The abstract class Point describes the common
//! behavior of geometric points in 2D space.
//! The Geom2d package also provides the concrete
//! class Geom2d_CartesianPoint.
class Geom2d_Point : public Geom2d_Geometry
{

public:
  //! returns the Coordinates of <me>.
  Standard_EXPORT virtual void Coord(Standard_Real& X, Standard_Real& Y) const = 0;

  //! returns a non persistent copy of <me>
  Standard_EXPORT virtual gp_Pnt2d Pnt2d() const = 0;

  //! returns the X coordinate of <me>.
  Standard_EXPORT virtual Standard_Real X() const = 0;

  //! returns  the Y coordinate of <me>.
  Standard_EXPORT virtual Standard_Real Y() const = 0;

  //! computes the distance between <me> and <Other>.
  Standard_EXPORT Standard_Real Distance(const Handle(Geom2d_Point)& Other) const;

  //! computes the square distance between <me> and <Other>.
  Standard_EXPORT Standard_Real SquareDistance(const Handle(Geom2d_Point)& Other) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Geom2d_Point, Geom2d_Geometry)

protected:
private:
};

#endif // _Geom2d_Point_HeaderFile
