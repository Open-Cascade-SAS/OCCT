// Created on: 1993-03-24
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

#ifndef _Geom2d_CartesianPoint_HeaderFile
#define _Geom2d_CartesianPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt2d.hxx>
#include <Geom2d_Point.hxx>
#include <Standard_Real.hxx>
class gp_Pnt2d;
class gp_Trsf2d;
class Geom2d_Geometry;


class Geom2d_CartesianPoint;
DEFINE_STANDARD_HANDLE(Geom2d_CartesianPoint, Geom2d_Point)

//! Describes a point in 2D space. A
//! Geom2d_CartesianPoint is defined by a gp_Pnt2d
//! point, with its two Cartesian coordinates X and Y.
class Geom2d_CartesianPoint : public Geom2d_Point
{

public:

  
  //! Returns a persistent copy of P.
  Standard_EXPORT Geom2d_CartesianPoint(const gp_Pnt2d& P);
  
  Standard_EXPORT Geom2d_CartesianPoint(const Standard_Real X, const Standard_Real Y);
  
  //! Set <me> to X, Y coordinates.
  Standard_EXPORT void SetCoord (const Standard_Real X, const Standard_Real Y);
  
  //! Set <me> to P.X(), P.Y() coordinates.
  Standard_EXPORT void SetPnt2d (const gp_Pnt2d& P);
  
  //! Changes the X coordinate of me.
  Standard_EXPORT void SetX (const Standard_Real X);
  
  //! Changes the Y coordinate of me.
  Standard_EXPORT void SetY (const Standard_Real Y);
  
  //! Returns the coordinates of <me>.
  Standard_EXPORT void Coord (Standard_Real& X, Standard_Real& Y) const Standard_OVERRIDE;
  

  //! Returns a non persistent cartesian point with
  //! the same coordinates as <me>.
  //! -C++: return const&
  Standard_EXPORT gp_Pnt2d Pnt2d() const Standard_OVERRIDE;
  
  //! Returns the X coordinate of <me>.
  Standard_EXPORT Standard_Real X() const Standard_OVERRIDE;
  
  //! Returns the Y coordinate of <me>.
  Standard_EXPORT Standard_Real Y() const Standard_OVERRIDE;
  
  Standard_EXPORT void Transform (const gp_Trsf2d& T) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom2d_Geometry) Copy() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom2d_CartesianPoint,Geom2d_Point)

protected:




private:


  gp_Pnt2d gpPnt2d;


};







#endif // _Geom2d_CartesianPoint_HeaderFile
