// Created on: 1992-09-28
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _GCE2d_MakeParabola_HeaderFile
#define _GCE2d_MakeParabola_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GCE2d_Root.hxx>
#include <Geom2d_Parabola.hxx>

class gp_Parab2d;
class gp_Ax22d;
class gp_Ax2d;
class gp_Pnt2d;

//! This class implements the following algorithms used to
//! create Parabola from Geom2d.
//! * Create an Parabola from two apex  and the center.
//! Defines the parabola in the parameterization range  :
//! ]-infinite,+infinite[
//! The vertex of the parabola is the "Location" point of the
//! local coordinate system "XAxis" of the parabola.
//! The "XAxis" of the parabola is its axis of symmetry.
//! The "Xaxis" is oriented from the vertex of the parabola to the
//! Focus of the parabola.
//! The equation of the parabola in the local coordinate system is
//! Y**2 = (2*P) * X
//! P is the distance between the focus and the directrix of the
//! parabola called Parameter).
//! The focal length F = P/2 is the distance between the vertex
//! and the focus of the parabola.
class GCE2d_MakeParabola : public GCE2d_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a parabola from a non persistent one.
  Standard_EXPORT GCE2d_MakeParabola(const gp_Parab2d& Prb);

  //! Creates a parabola with its local coordinate system and it's focal
  //! length "Focal".
  //! The "Location" point of "Axis" is the vertex of the parabola
  //! Status is "NegativeFocusLength" if Focal < 0.0
  Standard_EXPORT GCE2d_MakeParabola(const gp_Ax22d& Axis, const Standard_Real Focal);

  //! Creates a parabola with its "MirrorAxis" and it's focal length "Focal".
  //! MirrorAxis is the axis of symmetry of the curve, it is the
  //! "XAxis". The "YAxis" is parallel to the directrix of the
  //! parabola. The "Location" point of "MirrorAxis" is the vertex of the parabola
  //! Status is "NegativeFocusLength" if Focal < 0.0
  Standard_EXPORT GCE2d_MakeParabola(const gp_Ax2d&         MirrorAxis,
                                     const Standard_Real    Focal,
                                     const Standard_Boolean Sense);

  //! D is the directrix of the parabola and F the focus point.
  //! The symmetry axis "XAxis" of the parabola is normal to the
  //! directrix and pass through the focus point F, but its
  //! "Location" point is the vertex of the parabola.
  //! The "YAxis" of the parabola is parallel to D and its "Location"
  //! point is the vertex of the parabola.
  Standard_EXPORT GCE2d_MakeParabola(const gp_Ax2d&         D,
                                     const gp_Pnt2d&        F,
                                     const Standard_Boolean Sense = Standard_True);

  //! Make a parabola with focal point S1 and
  //! center O
  //! The branch of the parabola returned will have <S1> as
  //! focal point
  //! The implicit orientation of the parabola is:
  //! -   the same one as the parabola Prb,
  //! -   the sense defined by the coordinate system Axis or the directrix D,
  //! -   the trigonometric sense if Sense is not given or is true, or
  //! -   the opposite sense if Sense is false.
  //! Warning
  //! The MakeParabola class does not prevent the
  //! construction of a parabola with a null focal distance.
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_NullFocusLength if Focal is less than 0.0, or
  //! -   gce_NullAxis if points S1 and O are coincident.
  Standard_EXPORT GCE2d_MakeParabola(const gp_Pnt2d& S1, const gp_Pnt2d& O);

  //! Returns the constructed parabola.
  //! Exceptions StdFail_NotDone if no parabola is constructed.
  Standard_EXPORT const Handle(Geom2d_Parabola)& Value() const;

  operator const Handle(Geom2d_Parabola) & () const { return Value(); }

private:
  Handle(Geom2d_Parabola) TheParabola;
};

#endif // _GCE2d_MakeParabola_HeaderFile
