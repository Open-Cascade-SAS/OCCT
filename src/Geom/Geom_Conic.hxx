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

#ifndef _Geom_Conic_HeaderFile
#define _Geom_Conic_HeaderFile

#include <gp_Ax2.hxx>
#include <Geom_Curve.hxx>

class Geom_Conic;
DEFINE_STANDARD_HANDLE(Geom_Conic, Geom_Curve)

//! The abstract class Conic describes the common
//! behavior of conic curves in 3D space and, in
//! particular, their general characteristics. The Geom
//! package provides four concrete classes of conics:
//! Geom_Circle, Geom_Ellipse, Geom_Hyperbola and Geom_Parabola.
//! A conic is positioned in space with a right-handed
//! coordinate system (gp_Ax2 object), where:
//! - the origin is the center of the conic (or the apex in
//! the case of a parabola),
//! - the origin, "X Direction" and "Y Direction" define the
//! plane of the conic.
//! This coordinate system is the local coordinate
//! system of the conic.
//! The "main Direction" of this coordinate system is the
//! vector normal to the plane of the conic. The axis, of
//! which the origin and unit vector are respectively the
//! origin and "main Direction" of the local coordinate
//! system, is termed the "Axis" or "main Axis" of the conic.
//! The "main Direction" of the local coordinate system
//! gives an explicit orientation to the conic, determining
//! the direction in which the parameter increases along
//! the conic. The "X Axis" of the local coordinate system
//! also defines the origin of the parameter of the conic.
class Geom_Conic : public Geom_Curve
{
public:
  
  //! Changes the orientation of the conic's plane. The normal
  //! axis to the plane is A1. The XAxis and the YAxis are recomputed.
  //!
  //! raised if the A1 is parallel to the XAxis of the conic.
  void SetAxis (const gp_Ax1& theA1) { pos.SetAxis(theA1); }
  
  //! changes the location point of the conic.
  void SetLocation (const gp_Pnt& theP) { pos.SetLocation(theP); }
  
  //! changes the local coordinate system of the conic.
  void SetPosition (const gp_Ax2& theA2) { pos = theA2; }
  
  //! Returns the "main Axis" of this conic. This axis is
  //! normal to the plane of the conic.
  const gp_Ax1& Axis() const { return pos.Axis(); }  

  //! Returns the location point of the conic.
  //! For the circle, the ellipse and the hyperbola it is the center of
  //! the conic. For the parabola it is the Apex of the parabola.
  const gp_Pnt& Location() const { return pos.Location(); }
  
  //! Returns the local coordinates system of the conic.
  //! The main direction of the Axis2Placement is normal to the
  //! plane of the conic. The X direction of the Axis2placement
  //! is in the plane of the conic and corresponds to the origin
  //! for the conic's parametric value u.
  const gp_Ax2& Position() const { return pos; }
  
  //! Returns the eccentricity value of the conic e.
  //! e = 0 for a circle
  //! 0 < e < 1 for an ellipse  (e = 0 if MajorRadius = MinorRadius)
  //! e > 1 for a hyperbola
  //! e = 1 for a parabola
  //! Exceptions
  //! Standard_DomainError in the case of a hyperbola if
  //! its major radius is null.
  virtual Standard_Real Eccentricity() const = 0;

  //! Returns the XAxis of the conic.
  //! This axis defines the origin of parametrization of the conic.
  //! This axis is perpendicular to the Axis of the conic.
  //! This axis and the Yaxis define the plane of the conic.
  Standard_EXPORT gp_Ax1 XAxis() const;
  

  //! Returns the YAxis of the conic.
  //! The YAxis is perpendicular to the Xaxis.
  //! This axis and the Xaxis define the plane of the conic.
  Standard_EXPORT gp_Ax1 YAxis() const;
  

  //! Reverses the direction of parameterization of <me>.
  //! The local coordinate system of the conic is modified.
  Standard_EXPORT void Reverse() Standard_OVERRIDE;
  
  //! Returns the  parameter on the  reversed  curve for
  //! the point of parameter U on <me>.
  Standard_EXPORT virtual Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE = 0;
  
  //! The continuity of the conic is Cn.
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  //! Returns True.
  //! Raised if N < 0.
  Standard_EXPORT Standard_Boolean IsCN (const Standard_Integer N) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Geom_Conic,Geom_Curve)

protected:
  gp_Ax2 pos;
};

#endif // _Geom_Conic_HeaderFile
