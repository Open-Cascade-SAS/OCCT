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

#ifndef _gp_Ax2_HeaderFile
#define _gp_Ax2_HeaderFile

#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <Precision.hxx>

class gp_Trsf;
class gp_Vec;

//! Describes a right-handed coordinate system in 3D space.
//! A coordinate system is defined by:
//! -   its origin (also referred to as its "Location point"), and
//! -   three orthogonal unit vectors, termed respectively the
//! "X Direction", the "Y Direction" and the "Direction" (also
//! referred to as the "main Direction").
//! The "Direction" of the coordinate system is called its
//! "main Direction" because whenever this unit vector is
//! modified, the "X Direction" and the "Y Direction" are
//! recomputed. However, when we modify either the "X
//! Direction" or the "Y Direction", "Direction" is not modified.
//! The "main Direction" is also the "Z Direction".
//! Since an Ax2 coordinate system is right-handed, its
//! "main Direction" is always equal to the cross product of
//! its "X Direction" and "Y Direction". (To define a
//! left-handed coordinate system, use gp_Ax3.)
//! A coordinate system is used:
//! -   to describe geometric entities, in particular to position
//! them. The local coordinate system of a geometric
//! entity serves the same purpose as the STEP function
//! "axis placement two axes", or
//! -   to define geometric transformations.
//! Note: we refer to the "X Axis", "Y Axis" and "Z Axis",
//! respectively, as to axes having:
//! - the origin of the coordinate system as their origin, and
//! -   the unit vectors "X Direction", "Y Direction" and "main
//! Direction", respectively, as their unit vectors.
//! The "Z Axis" is also the "main Axis".
class gp_Ax2
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an object corresponding to the reference
  //! coordinate system (OXYZ).
  gp_Ax2()
      : vydir(0., 1., 0.)
  // vxdir(1.,0.,0.) use default ctor of gp_Dir, as it creates the same dir(1,0,0)
  {
  }

  //! Creates an axis placement with an origin P such that:
  //! -   N is the Direction, and
  //! -   the "X Direction" is normal to N, in the plane
  //! defined by the vectors (N, Vx): "X
  //! Direction" = (N ^ Vx) ^ N,
  //! Exception: raises ConstructionError if N and Vx are parallel (same or opposite orientation).
  gp_Ax2(const gp_Pnt& P, const gp_Dir& N, const gp_Dir& Vx)
      : axis(P, N),
        vydir(N),
        vxdir(N)
  {
    vxdir.CrossCross(Vx, N);
    vydir.Cross(vxdir);
  }

  //! Creates -   a coordinate system with an origin P, where V
  //! gives the "main Direction" (here, "X Direction" and "Y
  //! Direction" are defined automatically).
  Standard_EXPORT gp_Ax2(const gp_Pnt& P, const gp_Dir& V);

  //! Assigns the origin and "main Direction" of the axis A1 to
  //! this coordinate system, then recomputes its "X Direction" and "Y Direction".
  //! Note: The new "X Direction" is computed as follows:
  //! new "X Direction" = V1 ^(previous "X Direction" ^ V)
  //! where V is the "Direction" of A1.
  //! Exceptions
  //! Standard_ConstructionError if A1 is parallel to the "X
  //! Direction" of this coordinate system.
  void SetAxis(const gp_Ax1& A1);

  //! Changes the "main Direction" of this coordinate system,
  //! then recomputes its "X Direction" and "Y Direction".
  //! Note: the new "X Direction" is computed as follows:
  //! new "X Direction" = V ^ (previous "X Direction" ^ V)
  //! Exceptions
  //! Standard_ConstructionError if V is parallel to the "X
  //! Direction" of this coordinate system.
  void SetDirection(const gp_Dir& V);

  //! Changes the "Location" point (origin) of <me>.
  void SetLocation(const gp_Pnt& theP) { axis.SetLocation(theP); }

  //! Changes the "Xdirection" of <me>. The main direction
  //! "Direction" is not modified, the "Ydirection" is modified.
  //! If <Vx> is not normal to the main direction then <XDirection>
  //! is computed as follows XDirection = Direction ^ (Vx ^ Direction).
  //! Exceptions
  //! Standard_ConstructionError if Vx or Vy is parallel to
  //! the "main Direction" of this coordinate system.
  void SetXDirection(const gp_Dir& theVx)
  {
    vxdir = axis.Direction().CrossCrossed(theVx, axis.Direction());
    vydir = axis.Direction().Crossed(vxdir);
  }

  //! Changes the "Ydirection" of <me>. The main direction is not
  //! modified but the "Xdirection" is changed.
  //! If <Vy> is not normal to the main direction then "YDirection"
  //! is computed as  follows
  //! YDirection = Direction ^ (<Vy> ^ Direction).
  //! Exceptions
  //! Standard_ConstructionError if Vx or Vy is parallel to
  //! the "main Direction" of this coordinate system.
  void SetYDirection(const gp_Dir& theVy)
  {
    vxdir = theVy.Crossed(axis.Direction());
    vydir = (axis.Direction()).Crossed(vxdir);
  }

  //! Computes the angular value, in radians, between the main direction of
  //! <me> and the main direction of <theOther>. Returns the angle
  //! between 0 and PI in radians.
  Standard_Real Angle(const gp_Ax2& theOther) const { return axis.Angle(theOther.axis); }

  //! Returns the main axis of <me>. It is the "Location" point
  //! and the main "Direction".
  const gp_Ax1& Axis() const { return axis; }

  //! Returns the main direction of <me>.
  const gp_Dir& Direction() const { return axis.Direction(); }

  //! Returns the "Location" point (origin) of <me>.
  const gp_Pnt& Location() const { return axis.Location(); }

  //! Returns the "XDirection" of <me>.
  const gp_Dir& XDirection() const { return vxdir; }

  //! Returns the "YDirection" of <me>.
  const gp_Dir& YDirection() const { return vydir; }

  Standard_Boolean IsCoplanar(const gp_Ax2&       Other,
                              const Standard_Real LinearTolerance,
                              const Standard_Real AngularTolerance) const;

  //! Returns True if
  //! . the distance between <me> and the "Location" point of A1
  //! is lower of equal to LinearTolerance and
  //! . the main direction of <me> and the direction of A1 are normal.
  //! Note: the tolerance criterion for angular equality is given by AngularTolerance.
  Standard_Boolean IsCoplanar(const gp_Ax1&       A1,
                              const Standard_Real LinearTolerance,
                              const Standard_Real AngularTolerance) const;

  //! Performs a symmetrical transformation of this coordinate
  //! system with respect to:
  //! -   the point P, and assigns the result to this coordinate system.
  //! Warning
  //! This transformation is always performed on the origin.
  //! In case of a reflection with respect to a point:
  //! - the main direction of the coordinate system is not changed, and
  //! - the "X Direction" and the "Y Direction" are simply reversed
  //! In case of a reflection with respect to an axis or a plane:
  //! -   the transformation is applied to the "X Direction"
  //! and the "Y Direction", then
  //! -   the "main Direction" is recomputed as the cross
  //! product "X Direction" ^ "Y   Direction".
  //! This maintains the right-handed property of the
  //! coordinate system.
  Standard_EXPORT void Mirror(const gp_Pnt& P);

  //! Performs a symmetrical transformation of this coordinate
  //! system with respect to:
  //! -   the point P, and creates a new one.
  //! Warning
  //! This transformation is always performed on the origin.
  //! In case of a reflection with respect to a point:
  //! - the main direction of the coordinate system is not changed, and
  //! - the "X Direction" and the "Y Direction" are simply reversed
  //! In case of a reflection with respect to an axis or a plane:
  //! -   the transformation is applied to the "X Direction"
  //! and the "Y Direction", then
  //! -   the "main Direction" is recomputed as the cross
  //! product "X Direction" ^ "Y   Direction".
  //! This maintains the right-handed property of the
  //! coordinate system.
  Standard_NODISCARD Standard_EXPORT gp_Ax2 Mirrored(const gp_Pnt& P) const;

  //! Performs a symmetrical transformation of this coordinate
  //! system with respect to:
  //! -   the axis A1, and assigns the result to this coordinate systeme.
  //! Warning
  //! This transformation is always performed on the origin.
  //! In case of a reflection with respect to a point:
  //! - the main direction of the coordinate system is not changed, and
  //! - the "X Direction" and the "Y Direction" are simply reversed
  //! In case of a reflection with respect to an axis or a plane:
  //! -   the transformation is applied to the "X Direction"
  //! and the "Y Direction", then
  //! -   the "main Direction" is recomputed as the cross
  //! product "X Direction" ^ "Y   Direction".
  //! This maintains the right-handed property of the
  //! coordinate system.
  Standard_EXPORT void Mirror(const gp_Ax1& A1);

  //! Performs a symmetrical transformation of this coordinate
  //! system with respect to:
  //! -   the axis A1, and  creates a new one.
  //! Warning
  //! This transformation is always performed on the origin.
  //! In case of a reflection with respect to a point:
  //! - the main direction of the coordinate system is not changed, and
  //! - the "X Direction" and the "Y Direction" are simply reversed
  //! In case of a reflection with respect to an axis or a plane:
  //! -   the transformation is applied to the "X Direction"
  //! and the "Y Direction", then
  //! -   the "main Direction" is recomputed as the cross
  //! product "X Direction" ^ "Y   Direction".
  //! This maintains the right-handed property of the
  //! coordinate system.
  Standard_NODISCARD Standard_EXPORT gp_Ax2 Mirrored(const gp_Ax1& A1) const;

  //! Performs a symmetrical transformation of this coordinate
  //! system with respect to:
  //! -   the plane defined by the origin, "X Direction" and "Y
  //! Direction" of coordinate system A2 and  assigns the result to this coordinate systeme.
  //! Warning
  //! This transformation is always performed on the origin.
  //! In case of a reflection with respect to a point:
  //! - the main direction of the coordinate system is not changed, and
  //! - the "X Direction" and the "Y Direction" are simply reversed
  //! In case of a reflection with respect to an axis or a plane:
  //! -   the transformation is applied to the "X Direction"
  //! and the "Y Direction", then
  //! -   the "main Direction" is recomputed as the cross
  //! product "X Direction" ^ "Y   Direction".
  //! This maintains the right-handed property of the
  //! coordinate system.
  Standard_EXPORT void Mirror(const gp_Ax2& A2);

  //! Performs a symmetrical transformation of this coordinate
  //! system with respect to:
  //! -   the plane defined by the origin, "X Direction" and "Y
  //! Direction" of coordinate system A2 and creates a new one.
  //! Warning
  //! This transformation is always performed on the origin.
  //! In case of a reflection with respect to a point:
  //! - the main direction of the coordinate system is not changed, and
  //! - the "X Direction" and the "Y Direction" are simply reversed
  //! In case of a reflection with respect to an axis or a plane:
  //! -   the transformation is applied to the "X Direction"
  //! and the "Y Direction", then
  //! -   the "main Direction" is recomputed as the cross
  //! product "X Direction" ^ "Y   Direction".
  //! This maintains the right-handed property of the
  //! coordinate system.
  Standard_NODISCARD Standard_EXPORT gp_Ax2 Mirrored(const gp_Ax2& A2) const;

  void Rotate(const gp_Ax1& theA1, const Standard_Real theAng)
  {
    gp_Pnt aTemp = axis.Location();
    aTemp.Rotate(theA1, theAng);
    axis.SetLocation(aTemp);
    vxdir.Rotate(theA1, theAng);
    vydir.Rotate(theA1, theAng);
    axis.SetDirection(vxdir.Crossed(vydir));
  }

  //! Rotates an axis placement. <theA1> is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Ax2 Rotated(const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Ax2 aTemp = *this;
    aTemp.Rotate(theA1, theAng);
    return aTemp;
  }

  void Scale(const gp_Pnt& theP, const Standard_Real theS)
  {
    gp_Pnt aTemp = axis.Location();
    aTemp.Scale(theP, theS);
    axis.SetLocation(aTemp);
    if (theS < 0.0)
    {
      vxdir.Reverse();
      vydir.Reverse();
    }
  }

  //! Applies a scaling transformation on the axis placement.
  //! The "Location" point of the axisplacement is modified.
  //! Warnings :
  //! If the scale <S> is negative :
  //! . the main direction of the axis placement is not changed.
  //! . The "XDirection" and the "YDirection" are reversed.
  //! So the axis placement stay right handed.
  Standard_NODISCARD gp_Ax2 Scaled(const gp_Pnt& theP, const Standard_Real theS) const
  {
    gp_Ax2 aTemp = *this;
    aTemp.Scale(theP, theS);
    return aTemp;
  }

  void Transform(const gp_Trsf& theT)
  {
    gp_Pnt aTemp = axis.Location();
    aTemp.Transform(theT);
    axis.SetLocation(aTemp);
    vxdir.Transform(theT);
    vydir.Transform(theT);
    axis.SetDirection(vxdir.Crossed(vydir));
  }

  //! Transforms an axis placement with a Trsf.
  //! The "Location" point, the "XDirection" and the "YDirection" are transformed with theT.
  //! The resulting main "Direction" of <me> is the cross product between
  //! the "XDirection" and the "YDirection" after transformation.
  Standard_NODISCARD gp_Ax2 Transformed(const gp_Trsf& theT) const
  {
    gp_Ax2 aTemp = *this;
    aTemp.Transform(theT);
    return aTemp;
  }

  void Translate(const gp_Vec& theV) { axis.Translate(theV); }

  //! Translates an axis plaxement in the direction of the vector <theV>.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Ax2 Translated(const gp_Vec& theV) const
  {
    gp_Ax2 aTemp = *this;
    aTemp.Translate(theV);
    return aTemp;
  }

  void Translate(const gp_Pnt& theP1, const gp_Pnt& theP2) { axis.Translate(theP1, theP2); }

  //! Translates an axis placement from the point <theP1> to the point <theP2>.
  Standard_NODISCARD gp_Ax2 Translated(const gp_Pnt& theP1, const gp_Pnt& theP2) const
  {
    gp_Ax2 aTemp = *this;
    aTemp.Translate(theP1, theP2);
    return aTemp;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  //! Inits the content of me from the stream
  Standard_EXPORT Standard_Boolean InitFromJson(const Standard_SStream& theSStream,
                                                Standard_Integer&       theStreamPos);

private:
  gp_Ax1 axis;
  gp_Dir vydir;
  gp_Dir vxdir;
};

// =======================================================================
// function : SetAxis
// purpose  :
// =======================================================================
inline void gp_Ax2::SetAxis(const gp_Ax1& theA1)
{
  Standard_Real a = theA1.Direction() * vxdir;
  if (Abs(Abs(a) - 1.) <= Precision::Angular())
  {
    if (a > 0.)
    {
      vxdir = vydir;
      vydir = axis.Direction();
      axis  = theA1;
    }
    else
    {
      vxdir = axis.Direction();
      axis  = theA1;
    }
  }
  else
  {
    axis  = theA1;
    vxdir = axis.Direction().CrossCrossed(vxdir, axis.Direction());
    vydir = axis.Direction().Crossed(vxdir);
  }
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
inline void gp_Ax2::SetDirection(const gp_Dir& theV)
{
  Standard_Real a = theV * vxdir;
  if (Abs(Abs(a) - 1.) <= Precision::Angular())
  {
    if (a > 0.)
    {
      vxdir = vydir;
      vydir = axis.Direction();
      axis.SetDirection(theV);
    }
    else
    {
      vxdir = axis.Direction();
      axis.SetDirection(theV);
    }
  }
  else
  {
    axis.SetDirection(theV);
    vxdir = theV.CrossCrossed(vxdir, theV);
    vydir = theV.Crossed(vxdir);
  }
}

// =======================================================================
// function : IsCoplanar
// purpose  :
// =======================================================================
inline Standard_Boolean gp_Ax2::IsCoplanar(const gp_Ax2&       theOther,
                                           const Standard_Real theLinearTolerance,
                                           const Standard_Real theAngularTolerance) const
{
  const gp_Dir& DD = axis.Direction();
  const gp_Pnt& PP = axis.Location();
  const gp_Pnt& OP = theOther.axis.Location();
  Standard_Real D1 =
    (DD.X() * (OP.X() - PP.X()) + DD.Y() * (OP.Y() - PP.Y()) + DD.Z() * (OP.Z() - PP.Z()));
  if (D1 < 0)
  {
    D1 = -D1;
  }
  return D1 <= theLinearTolerance && axis.IsParallel(theOther.axis, theAngularTolerance);
}

// =======================================================================
// function : IsCoplanar
// purpose  :
// =======================================================================
inline Standard_Boolean gp_Ax2::IsCoplanar(const gp_Ax1&       theA,
                                           const Standard_Real theLinearTolerance,
                                           const Standard_Real theAngularTolerance) const
{
  const gp_Dir& DD = axis.Direction();
  const gp_Pnt& PP = axis.Location();
  const gp_Pnt& AP = theA.Location();
  Standard_Real D1 =
    (DD.X() * (AP.X() - PP.X()) + DD.Y() * (AP.Y() - PP.Y()) + DD.Z() * (AP.Z() - PP.Z()));
  if (D1 < 0)
  {
    D1 = -D1;
  }
  return D1 <= theLinearTolerance && axis.IsNormal(theA, theAngularTolerance);
}

#endif // _gp_Ax2_HeaderFile
