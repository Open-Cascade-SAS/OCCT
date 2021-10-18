// Created on: 1993-03-09
// Created by: JVC
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

#ifndef _Geom_Geometry_HeaderFile
#define _Geom_Geometry_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
class gp_Pnt;
class gp_Ax1;
class gp_Ax2;
class gp_Vec;
class gp_Trsf;


class Geom_Geometry;
DEFINE_STANDARD_HANDLE(Geom_Geometry, Standard_Transient)

//! The abstract class Geometry for 3D space is the root
//! class of all geometric objects from the Geom
//! package. It describes the common behavior of these objects when:
//! - applying geometric transformations to objects, and
//! - constructing objects by geometric transformation (including copying).
//! Warning
//! Only transformations which do not modify the nature
//! of the geometry can be applied to Geom objects: this
//! is the case with translations, rotations, symmetries
//! and scales; this is also the case with gp_Trsf
//! composite transformations which are used to define
//! the geometric transformations applied using the
//! Transform or Transformed functions.
//! Note: Geometry defines the "prototype" of the
//! abstract method Transform which is defined for each
//! concrete type of derived object. All other
//! transformations are implemented using the Transform method.
class Geom_Geometry : public Standard_Transient
{

public:

  

  //! Performs the symmetrical transformation of a Geometry
  //! with respect to the point P which is the center of the
  //! symmetry.
  Standard_EXPORT void Mirror (const gp_Pnt& P);
  

  //! Performs the symmetrical transformation of a Geometry
  //! with respect to an axis placement which is the axis of the
  //! symmetry.
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of a Geometry
  //! with respect to a plane. The axis placement A2 locates
  //! the plane of the symmetry : (Location, XDirection, YDirection).
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Rotates a Geometry. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
  Standard_EXPORT void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Scales a Geometry. S is the scaling value.
  Standard_EXPORT void Scale (const gp_Pnt& P, const Standard_Real S);
  

  //! Translates a Geometry.  V is the vector of the translation.
  Standard_EXPORT void Translate (const gp_Vec& V);
  

  //! Translates a Geometry from the point P1 to the point P2.
  Standard_EXPORT void Translate (const gp_Pnt& P1, const gp_Pnt& P2);
  

  //! Transformation of a geometric object. This tansformation
  //! can be a translation, a rotation, a symmetry, a scaling
  //! or a complex transformation obtained by combination of
  //! the previous elementaries transformations.
  //! (see class Transformation of the package Geom).
  Standard_EXPORT virtual void Transform (const gp_Trsf& T) = 0;
  
  Standard_NODISCARD Standard_EXPORT Handle(Geom_Geometry) Mirrored (const gp_Pnt& P) const;
  
  Standard_NODISCARD Standard_EXPORT Handle(Geom_Geometry) Mirrored (const gp_Ax1& A1) const;
  
  Standard_NODISCARD Standard_EXPORT Handle(Geom_Geometry) Mirrored (const gp_Ax2& A2) const;
  
  Standard_NODISCARD Standard_EXPORT Handle(Geom_Geometry) Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  
  Standard_NODISCARD Standard_EXPORT Handle(Geom_Geometry) Scaled (const gp_Pnt& P, const Standard_Real S) const;
  
  Standard_NODISCARD Standard_EXPORT Handle(Geom_Geometry) Transformed (const gp_Trsf& T) const;
  
  Standard_NODISCARD Standard_EXPORT Handle(Geom_Geometry) Translated (const gp_Vec& V) const;
  
  Standard_NODISCARD Standard_EXPORT Handle(Geom_Geometry) Translated (const gp_Pnt& P1, const gp_Pnt& P2) const;
  
  //! Creates a new object which is a copy of this geometric object.
  Standard_EXPORT virtual Handle(Geom_Geometry) Copy() const = 0;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;




  DEFINE_STANDARD_RTTIEXT(Geom_Geometry,Standard_Transient)

protected:




private:




};







#endif // _Geom_Geometry_HeaderFile
