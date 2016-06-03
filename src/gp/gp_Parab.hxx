// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _gp_Parab_HeaderFile
#define _gp_Parab_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax2.hxx>
#include <Standard_Real.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
class Standard_ConstructionError;
class gp_Ax2;
class gp_Ax1;
class gp_Pnt;
class gp_Trsf;
class gp_Vec;



//! Describes a parabola in 3D space.
//! A parabola is defined by its focal length (that is, the
//! distance between its focus and apex) and positioned in
//! space with a coordinate system (a gp_Ax2 object)
//! where:
//! -   the origin of the coordinate system is on the apex of
//! the parabola,
//! -   the "X Axis" of the coordinate system is the axis of
//! symmetry; the parabola is on the positive side of this axis, and
//! -   the origin, "X Direction" and "Y Direction" of the
//! coordinate system define the plane of the parabola.
//! The equation of the parabola in this coordinate system,
//! which is the "local coordinate system" of the parabola, is:
//! Y**2 = (2*P) * X.
//! where P, referred to as the parameter of the parabola, is
//! the distance between the focus and the directrix (P is
//! twice the focal length).
//! The "main Direction" of the local coordinate system gives
//! the normal vector to the plane of the parabola.
//! See Also
//! gce_MakeParab which provides functions for more
//! complex parabola constructions
//! Geom_Parabola which provides additional functions for
//! constructing parabolas and works, in particular, with the
//! parametric equations of parabolas
class gp_Parab 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an indefinite Parabola.
    gp_Parab();
  

  //! Creates a parabola with its local coordinate system "A2"
  //! and it's focal length "Focal".
  //! The XDirection of A2 defines the axis of symmetry of the
  //! parabola. The YDirection of A2 is parallel to the directrix
  //! of the parabola. The Location point of A2 is the vertex of
  //! the parabola
  //! Raises ConstructionError if Focal < 0.0
  //! Raised if Focal < 0.0
    gp_Parab(const gp_Ax2& A2, const Standard_Real Focal);
  

  //! D is the directrix of the parabola and F the focus point.
  //! The symmetry axis (XAxis) of the parabola is normal to the
  //! directrix and pass through the focus point F, but its
  //! location point is the vertex of the parabola.
  //! The YAxis of the parabola is parallel to D and its location
  //! point is the vertex of the parabola. The normal to the plane
  //! of the parabola is the cross product between the XAxis and the
  //! YAxis.
    gp_Parab(const gp_Ax1& D, const gp_Pnt& F);
  
  //! Modifies this parabola by redefining its local coordinate system so that
  //! -   its origin and "main Direction" become those of the
  //! axis A1 (the "X Direction" and "Y Direction" are then
  //! recomputed in the same way as for any gp_Ax2)
  //! Raises ConstructionError if the direction of A1 is parallel to the previous
  //! XAxis of the parabola.
    void SetAxis (const gp_Ax1& A1);
  
  //! Changes the focal distance of the parabola.
  //! Raises ConstructionError if Focal < 0.0
    void SetFocal (const Standard_Real Focal);
  

  //! Changes the location of the parabola. It is the vertex of
  //! the parabola.
    void SetLocation (const gp_Pnt& P);
  
  //! Changes the local coordinate system of the parabola.
  void SetPosition (const gp_Ax2& A2);
  

  //! Returns the main axis of the parabola.
  //! It is the axis normal to the plane of the parabola passing
  //! through the vertex of the parabola.
    const gp_Ax1& Axis() const;
  
  //! Computes the directrix of this parabola.
  //! The directrix is:
  //! -   a line parallel to the "Y Direction" of the local
  //! coordinate system of this parabola, and
  //! -   located on the negative side of the axis of symmetry,
  //! at a distance from the apex which is equal to the focal
  //! length of this parabola.
  //! The directrix is returned as an axis (a gp_Ax1 object),
  //! the origin of which is situated on the "X Axis" of this parabola.
    gp_Ax1 Directrix() const;
  

  //! Returns the distance between the vertex and the focus
  //! of the parabola.
    Standard_Real Focal() const;
  
  //! -   Computes the focus of the parabola.
    gp_Pnt Focus() const;
  

  //! Returns the vertex of the parabola. It is the "Location"
  //! point of the coordinate system of the parabola.
    const gp_Pnt& Location() const;
  

  //! Computes the parameter of the parabola.
  //! It is the distance between the focus and the directrix of
  //! the parabola. This distance is twice the focal length.
    Standard_Real Parameter() const;
  

  //! Returns the local coordinate system of the parabola.
    const gp_Ax2& Position() const;
  

  //! Returns the symmetry axis of the parabola. The location point
  //! of the axis is the vertex of the parabola.
    gp_Ax1 XAxis() const;
  

  //! It is an axis parallel to the directrix of the parabola.
  //! The location point of this axis is the vertex of the parabola.
    gp_Ax1 YAxis() const;
  
  Standard_EXPORT void Mirror (const gp_Pnt& P);
  

  //! Performs the symmetrical transformation of a parabola
  //! with respect to the point P which is the center of the
  //! symmetry.
  Standard_EXPORT gp_Parab Mirrored (const gp_Pnt& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of a parabola
  //! with respect to an axis placement which is the axis of
  //! the symmetry.
  Standard_EXPORT gp_Parab Mirrored (const gp_Ax1& A1) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Performs the symmetrical transformation of a parabola
  //! with respect to a plane. The axis placement A2 locates
  //! the plane of the symmetry (Location, XDirection, YDirection).
  Standard_EXPORT gp_Parab Mirrored (const gp_Ax2& A2) const;
  
    void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Rotates a parabola. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
    gp_Parab Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  
    void Scale (const gp_Pnt& P, const Standard_Real S);
  

  //! Scales a parabola. S is the scaling value.
  //! If S is negative the direction of the symmetry axis
  //! XAxis is reversed and the direction of the YAxis too.
    gp_Parab Scaled (const gp_Pnt& P, const Standard_Real S) const;
  
    void Transform (const gp_Trsf& T);
  

  //! Transforms a parabola with the transformation T from class Trsf.
    gp_Parab Transformed (const gp_Trsf& T) const;
  
    void Translate (const gp_Vec& V);
  

  //! Translates a parabola in the direction of the vector V.
  //! The magnitude of the translation is the vector's magnitude.
    gp_Parab Translated (const gp_Vec& V) const;
  
    void Translate (const gp_Pnt& P1, const gp_Pnt& P2);
  

  //! Translates a parabola from the point P1 to the point P2.
    gp_Parab Translated (const gp_Pnt& P1, const gp_Pnt& P2) const;




protected:





private:



  gp_Ax2 pos;
  Standard_Real focalLength;


};


#include <gp_Parab.lxx>





#endif // _gp_Parab_HeaderFile
