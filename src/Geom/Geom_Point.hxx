// Created on: 1993-03-10
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

#ifndef _Geom_Point_HeaderFile
#define _Geom_Point_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_Geometry.hxx>
#include <Standard_Real.hxx>
class gp_Pnt;


class Geom_Point;
DEFINE_STANDARD_HANDLE(Geom_Point, Geom_Geometry)

//! The abstract class Point describes the common
//! behavior of geometric points in 3D space.
//! The Geom package also provides the concrete class
//! Geom_CartesianPoint.
class Geom_Point : public Geom_Geometry
{

public:

  
  //! returns the Coordinates of <me>.
  Standard_EXPORT virtual void Coord (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const = 0;
  
  //! returns a non transient copy of <me>
  Standard_EXPORT virtual gp_Pnt Pnt() const = 0;
  
  //! returns the X coordinate of <me>.
  Standard_EXPORT virtual Standard_Real X() const = 0;
  
  //! returns  the Y coordinate of <me>.
  Standard_EXPORT virtual Standard_Real Y() const = 0;
  
  //! returns the Z coordinate of <me>.
  Standard_EXPORT virtual Standard_Real Z() const = 0;
  
  //! Computes the distance between <me> and <Other>.
  Standard_EXPORT Standard_Real Distance (const Handle(Geom_Point)& Other) const;
  
  //! Computes the square distance between <me> and <Other>.
  Standard_EXPORT Standard_Real SquareDistance (const Handle(Geom_Point)& Other) const;




  DEFINE_STANDARD_RTTIEXT(Geom_Point,Geom_Geometry)

protected:




private:




};







#endif // _Geom_Point_HeaderFile
