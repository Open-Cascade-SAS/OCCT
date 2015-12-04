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

#ifndef _Geom_SweptSurface_HeaderFile
#define _Geom_SweptSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Dir.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom_Surface.hxx>
class Geom_Curve;
class gp_Dir;


class Geom_SweptSurface;
DEFINE_STANDARD_HANDLE(Geom_SweptSurface, Geom_Surface)

//! Describes the common behavior for surfaces
//! constructed by sweeping a curve with another curve.
//! The Geom package provides two concrete derived
//! surfaces: surface of revolution (a revolved surface),
//! and surface of linear extrusion (an extruded surface).
class Geom_SweptSurface : public Geom_Surface
{

public:

  

  //! returns the continuity of the surface :
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the surface,
  //! C2 : continuity of the second derivative all along the surface,
  //! C3 : continuity of the third derivative all along the surface,
  //! G1 : tangency continuity all along the surface,
  //! G2 : curvature continuity all along the surface,
  //! CN : the order of continuity is infinite.
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  

  //! Returns the reference direction of the swept surface.
  //! For a surface of revolution it is the direction of the
  //! revolution axis, for a surface of linear extrusion it is
  //! the direction of extrusion.
  Standard_EXPORT const gp_Dir& Direction() const;
  

  //! Returns the referenced curve of the surface.
  //! For a surface of revolution it is the revolution curve,
  //! for a surface of linear extrusion it is the extruded curve.
  Standard_EXPORT Handle(Geom_Curve) BasisCurve() const;




  DEFINE_STANDARD_RTTIEXT(Geom_SweptSurface,Geom_Surface)

protected:


  Handle(Geom_Curve) basisCurve;
  gp_Dir direction;
  GeomAbs_Shape smooth;


private:




};







#endif // _Geom_SweptSurface_HeaderFile
