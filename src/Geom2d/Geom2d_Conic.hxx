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

#ifndef _Geom2d_Conic_HeaderFile
#define _Geom2d_Conic_HeaderFile

#include <gp_Ax22d.hxx>
#include <Geom2d_Curve.hxx>

class Geom2d_Conic;
DEFINE_STANDARD_HANDLE(Geom2d_Conic, Geom2d_Curve)

//! The abstract class Conic describes the common
//! behavior of conic curves in 2D space and, in
//! particular, their general characteristics. The Geom2d
//! package provides four specific classes of conics:
//! Geom2d_Circle, Geom2d_Ellipse,
//! Geom2d_Hyperbola and Geom2d_Parabola.
//! A conic is positioned in the plane with a coordinate
//! system (gp_Ax22d object), where the origin is the
//! center of the conic (or the apex in case of a parabola).
//! This coordinate system is the local coordinate
//! system of the conic. It gives the conic an explicit
//! orientation, determining the direction in which the
//! parameter increases along the conic. The "X Axis" of
//! the local coordinate system also defines the origin of
//! the parameter of the conic.
class Geom2d_Conic : public Geom2d_Curve
{
public:

  //! Modifies this conic, redefining its local coordinate system
  //! partially, by assigning theA as its axis
  void SetAxis (const gp_Ax22d& theA) { pos.SetAxis(theA); }
  
  //! Assigns the origin and unit vector of axis theA to the
  //! origin of the local coordinate system of this conic and X Direction.
  //! The other unit vector of the local coordinate system
  //! of this conic is recomputed normal to theA, without
  //! changing the orientation of the local coordinate
  //! system (right-handed or left-handed).
  void SetXAxis (const gp_Ax2d& theAX) { pos.SetXAxis(theAX); }
  
  //! Assigns the origin and unit vector of axis theA to the
  //! origin of the local coordinate system of this conic and Y Direction.
  //! The other unit vector of the local coordinate system
  //! of this conic is recomputed normal to theA, without
  //! changing the orientation of the local coordinate
  //! system (right-handed or left-handed).
  void SetYAxis (const gp_Ax2d& theAY) { pos.SetYAxis(theAY); }
  
  //! Modifies this conic, redefining its local coordinate
  //! system partially, by assigning theP as its origin.
  void SetLocation (const gp_Pnt2d& theP) { pos.SetLocation(theP); }

  //! Returns the "XAxis" of the conic.
  //! This axis defines the origin of parametrization of the conic.
  //! This axis and the "Yaxis" define the local coordinate system
  //! of the conic.
  //! -C++: return const&
  Standard_EXPORT gp_Ax2d XAxis() const;
  

  //! Returns the "YAxis" of the conic.
  //! The "YAxis" is perpendicular to the "Xaxis".
  Standard_EXPORT gp_Ax2d YAxis() const;
  

  //! returns the eccentricity value of the conic e.
  //! e = 0 for a circle
  //! 0 < e < 1 for an ellipse  (e = 0 if MajorRadius = MinorRadius)
  //! e > 1 for a hyperbola
  //! e = 1 for a parabola
  Standard_EXPORT virtual Standard_Real Eccentricity() const = 0;
  

  //! Returns the location point of the conic.
  //! For the circle, the ellipse and the hyperbola it is the center of
  //! the conic. For the parabola it is the vertex of the parabola.
  const gp_Pnt2d& Location() const { return pos.Location(); }
  
  //! Returns the local coordinates system of the conic.
  const gp_Ax22d& Position() const { return pos; }

  //! Reverses the direction of parameterization of <me>.
  //! The local coordinate system of the conic is modified.
  Standard_EXPORT void Reverse() Standard_OVERRIDE;
  
  //! Returns the  parameter on the  reversed  curve for
  //! the point of parameter U on <me>.
  Standard_EXPORT virtual Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE = 0;
  
  //! Returns GeomAbs_CN which is the global continuity of any conic.
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  //! Returns True, the order of continuity of a conic is infinite.
  Standard_EXPORT Standard_Boolean IsCN (const Standard_Integer N) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Geom2d_Conic,Geom2d_Curve)

protected:
  gp_Ax22d pos;
};

#endif // _Geom2d_Conic_HeaderFile
