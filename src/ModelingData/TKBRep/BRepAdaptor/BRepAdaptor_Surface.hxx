// Created on: 1993-02-22
// Created by: Remi LEQUETTE
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

#ifndef _BRepAdaptor_Surface_HeaderFile
#define _BRepAdaptor_Surface_HeaderFile

#include <GeomAdaptor_TransformedSurface.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopoDS_Face.hxx>

class gp_Pln;
class gp_Cylinder;
class gp_Cone;
class gp_Sphere;
class gp_Torus;
class Geom_BezierSurface;
class Geom_BSplineSurface;
class gp_Ax1;
class gp_Dir;

DEFINE_STANDARD_HANDLE(BRepAdaptor_Surface, GeomAdaptor_TransformedSurface)

//! The Surface from BRepAdaptor allows to use a Face
//! of the BRep topology look like a 3D surface.
//!
//! It has the methods of the class Surface from
//! Adaptor3d.
//!
//! It is created or initialized with a Face. It takes
//! into account the local coordinates system.
//!
//! The u,v parameter range is the minmax value for
//! the restriction, unless the flag restriction is
//! set to false.
class BRepAdaptor_Surface : public GeomAdaptor_TransformedSurface
{
  DEFINE_STANDARD_RTTIEXT(BRepAdaptor_Surface, GeomAdaptor_TransformedSurface)
public:
  //! Creates an undefined surface with no face loaded.
  Standard_EXPORT BRepAdaptor_Surface();

  //! Creates a surface to access the geometry of <F>.
  //! If <Restriction> is true the parameter range is
  //! the parameter range in the UV space of the
  //! restriction.
  Standard_EXPORT BRepAdaptor_Surface(const TopoDS_Face&     F,
                                      const Standard_Boolean R = Standard_True);

  //! Shallow copy of adaptor.
  Standard_EXPORT virtual Handle(Adaptor3d_Surface) ShallowCopy() const Standard_OVERRIDE;

  //! Sets the surface to the geometry of <F>.
  Standard_EXPORT void Initialize(const TopoDS_Face&     F,
                                  const Standard_Boolean Restriction = Standard_True);

  //! Returns the face.
  Standard_EXPORT const TopoDS_Face& Face() const;

  //! Returns the face tolerance.
  Standard_EXPORT Standard_Real Tolerance() const;

  // Note: Most methods are inherited from GeomAdaptor_TransformedSurface.
  // The following methods provide access to the underlying surface/transformation:
  // - Surface() - returns const GeomAdaptor_Surface&
  // - ChangeSurface() - returns GeomAdaptor_Surface&
  // - Trsf() - returns const gp_Trsf&
  //
  // Value, D0, D1, D2, D3, DN methods are inherited and marked as final.
  // They apply the transformation automatically.

private:
  TopoDS_Face myFace;
};

#endif // _BRepAdaptor_Surface_HeaderFile
