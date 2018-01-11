// Created on: 1993-08-02
// Created by: Laurent BOURESCHE
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

#ifndef _gp_Ax3_HeaderFile
#define _gp_Ax3_HeaderFile

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

class gp_Trsf;

//! Describes a coordinate system in 3D space. Unlike a
//! gp_Ax2 coordinate system, a gp_Ax3 can be
//! right-handed ("direct sense") or left-handed ("indirect sense").
//! A coordinate system is defined by:
//! -   its origin (also referred to as its "Location point"), and
//! -   three orthogonal unit vectors, termed the "X
//! Direction", the "Y Direction" and the "Direction" (also
//! referred to as the "main Direction").
//! The "Direction" of the coordinate system is called its
//! "main Direction" because whenever this unit vector is
//! modified, the "X Direction" and the "Y Direction" are
//! recomputed. However, when we modify either the "X
//! Direction" or the "Y Direction", "Direction" is not modified.
//! "Direction" is also the "Z Direction".
//! The "main Direction" is always parallel to the cross
//! product of its "X Direction" and "Y Direction".
//! If the coordinate system is right-handed, it satisfies the equation:
//! "main Direction" = "X Direction" ^ "Y Direction"
//! and if it is left-handed, it satisfies the equation:
//! "main Direction" = -"X Direction" ^ "Y Direction"
//! A coordinate system is used:
//! -   to describe geometric entities, in particular to position
//! them. The local coordinate system of a geometric
//! entity serves the same purpose as the STEP function
//! "axis placement three axes", or
//! -   to define geometric transformations.
//! Note:
//! -   We refer to the "X Axis", "Y Axis" and "Z Axis",
//! respectively, as the axes having:
//! -   the origin of the coordinate system as their origin, and
//! -   the unit vectors "X Direction", "Y Direction" and
//! "main Direction", respectively, as their unit vectors.
//! -   The "Z Axis" is also the "main Axis".
//! -   gp_Ax2 is used to define a coordinate system that must be always right-handed.
class gp_Ax3 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates an object corresponding to the reference
  //! coordinate system (OXYZ).
  gp_Ax3() : vydir (0., 1., 0.)
  // vxdir(1.,0.,0.) use default ctor of gp_Dir, as it creates the same dir(1,0,0)
  {}

  //! Creates  a  coordinate  system from a right-handed
  //! coordinate system.
  gp_Ax3 (const gp_Ax2& theA);

  //! Creates a  right handed axis placement with the
  //! "Location" point theP and  two directions, theN gives the
  //! "Direction" and theVx gives the "XDirection".
  //! Raises ConstructionError if theN and theVx are parallel (same or opposite orientation).
  gp_Ax3 (const gp_Pnt& theP, const gp_Dir& theN, const gp_Dir& theVx)
  : axis (theP, theN),
    vydir(theN),
    vxdir(theN)
  {
    vxdir.CrossCross (theVx, theN);
    vydir.Cross (vxdir);
  }

  //! Creates an axis placement with the "Location" point <theP>
  //! and the normal direction <theV>.
  Standard_EXPORT gp_Ax3 (const gp_Pnt& theP, const gp_Dir& theV);

  //! Reverses the X direction of <me>.
  void XReverse() { vxdir.Reverse(); }

  //! Reverses the Y direction of <me>.
  void YReverse() { vydir.Reverse(); }

  //! Reverses the Z direction of <me>.
  void ZReverse() { axis.Reverse(); }

  //! Assigns the origin and "main Direction" of the axis theA1 to
  //! this coordinate system, then recomputes its "X Direction" and "Y Direction".
  //! Note:
  //! -   The new "X Direction" is computed as follows:
  //! new "X Direction" = V1 ^(previous "X Direction" ^ V)
  //! where V is the "Direction" of theA1.
  //! -   The orientation of this coordinate system
  //! (right-handed or left-handed) is not modified.
  //! Raises ConstructionError  if the "Direction" of <theA1> and the "XDirection" of <me>
  //! are parallel (same or opposite orientation) because it is
  //! impossible to calculate the new "XDirection" and the new
  //! "YDirection".
  void SetAxis (const gp_Ax1& theA1);

  //! Changes the main direction of this coordinate system,
  //! then recomputes its "X Direction" and "Y Direction".
  //! Note:
  //! -   The new "X Direction" is computed as follows:
  //! new "X Direction" = theV ^ (previous "X Direction" ^ theV).
  //! -   The orientation of this coordinate system (left- or right-handed) is not modified.
  //! Raises ConstructionError if <theV> and the previous "XDirection" are parallel
  //! because it is impossible to calculate the new "XDirection"
  //! and the new "YDirection".
  void SetDirection (const gp_Dir& theV);

  //! Changes the "Location" point (origin) of <me>.
  void SetLocation (const gp_Pnt& theP) { axis.SetLocation (theP); }

  //! Changes the "Xdirection" of <me>. The main direction
  //! "Direction" is not modified, the "Ydirection" is modified.
  //! If <theVx> is not normal to the main direction then <XDirection>
  //! is computed as follows XDirection = Direction ^ (theVx ^ Direction).
  //! Raises ConstructionError if <theVx> is parallel (same or opposite
  //! orientation) to the main direction of <me>
  void SetXDirection (const gp_Dir& theVx);

  //! Changes the "Ydirection" of <me>. The main direction is not
  //! modified but the "Xdirection" is changed.
  //! If <theVy> is not normal to the main direction then "YDirection"
  //! is computed as  follows
  //! YDirection = Direction ^ (<theVy> ^ Direction).
  //! Raises ConstructionError if <theVy> is parallel to the main direction of <me>
  void SetYDirection (const gp_Dir& theVy);

  //! Computes the angular value between the main direction of
  //! <me> and the main direction of <theOther>. Returns the angle
  //! between 0 and PI in radians.
  Standard_Real Angle (const gp_Ax3& theOther) const { return axis.Angle (theOther.axis); }

  //! Returns the main axis of <me>. It is the "Location" point
  //! and the main "Direction".
  const gp_Ax1& Axis() const { return axis; }

  //! Computes a right-handed coordinate system with the
  //! same "X Direction" and "Y Direction" as those of this
  //! coordinate system, then recomputes the "main Direction".
  //! If this coordinate system is right-handed, the result
  //! returned is the same coordinate system. If this
  //! coordinate system is left-handed, the result is reversed.
  gp_Ax2 Ax2() const;

  //! Returns the main direction of <me>.
  const gp_Dir& Direction() const { return axis.Direction(); }

  //! Returns the "Location" point (origin) of <me>.
  const gp_Pnt& Location() const { return axis.Location(); }

  //! Returns the "XDirection" of <me>.
  const gp_Dir& XDirection() const { return vxdir; }

  //! Returns the "YDirection" of <me>.
  const gp_Dir& YDirection() const { return vydir; }

  //! Returns  True if  the  coordinate  system is right-handed. i.e.
  //! XDirection().Crossed(YDirection()).Dot(Direction()) > 0
  Standard_Boolean Direct() const { return (vxdir.Crossed (vydir).Dot (axis.Direction()) > 0.); }

  //! Returns True if
  //! . the distance between the "Location" point of <me> and
  //! <theOther> is lower or equal to theLinearTolerance and
  //! . the distance between the "Location" point of <theOther> and
  //! <me> is lower or equal to theLinearTolerance and
  //! . the main direction of <me> and the main direction of
  //! <theOther> are parallel (same or opposite orientation).
  Standard_Boolean IsCoplanar (const gp_Ax3& theOther, const Standard_Real theLinearTolerance, const Standard_Real theAngularTolerance) const;

  //! Returns True if
  //! . the distance between <me> and the "Location" point of theA1
  //! is lower of equal to theLinearTolerance and
  //! . the distance between theA1 and the "Location" point of <me>
  //! is lower or equal to theLinearTolerance and
  //! . the main direction of <me> and the direction of theA1 are normal.
  Standard_Boolean IsCoplanar (const gp_Ax1& theA1, const Standard_Real theLinearTolerance, const Standard_Real theAngularTolerance) const;

  Standard_EXPORT void Mirror (const gp_Pnt& theP);

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to the point theP which is the
  //! center of the symmetry.
  //! Warnings :
  //! The main direction of the axis placement is not changed.
  //! The "XDirection" and the "YDirection" are reversed.
  //! So the axis placement stay right handed.
  Standard_NODISCARD Standard_EXPORT gp_Ax3 Mirrored (const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror (const gp_Ax1& theA1);

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to an axis placement which
  //! is the axis of the symmetry.
  //! The transformation is performed on the "Location"
  //! point, on the "XDirection" and "YDirection".
  //! The resulting main "Direction" is the cross product between
  //! the "XDirection" and the "YDirection" after transformation.
  Standard_NODISCARD Standard_EXPORT gp_Ax3 Mirrored (const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror (const gp_Ax2& theA2);

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to a plane.
  //! The axis placement  <theA2> locates the plane of the symmetry :
  //! (Location, XDirection, YDirection).
  //! The transformation is performed on the "Location"
  //! point, on the "XDirection" and "YDirection".
  //! The resulting main "Direction" is the cross product between
  //! the "XDirection" and the "YDirection" after transformation.
  Standard_NODISCARD Standard_EXPORT gp_Ax3 Mirrored (const gp_Ax2& theA2) const;

  void Rotate (const gp_Ax1& theA1, const Standard_Real theAng)
  {
    axis.Rotate (theA1, theAng);
    vxdir.Rotate (theA1, theAng);
    vydir.Rotate (theA1, theAng);
  }

  //! Rotates an axis placement. <theA1> is the axis of the
  //! rotation . theAng is the angular value of the rotation
  //! in radians.
  Standard_NODISCARD gp_Ax3 Rotated (const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Ax3 aTemp = *this;
    aTemp.Rotate (theA1, theAng);
    return aTemp;
  }

  void Scale (const gp_Pnt& theP, const Standard_Real theS)
  {
    axis.Scale (theP, theS);
    if (theS < 0.)
    {
      vxdir.Reverse();
      vydir.Reverse();
    }
  }

  //! Applies a scaling transformation on the axis placement.
  //! The "Location" point of the axisplacement is modified.
  //! Warnings :
  //! If the scale <theS> is negative :
  //! . the main direction of the axis placement is not changed.
  //! . The "XDirection" and the "YDirection" are reversed.
  //! So the axis placement stay right handed.
  Standard_NODISCARD gp_Ax3 Scaled (const gp_Pnt& theP, const Standard_Real theS) const
  {
    gp_Ax3 aTemp = *this;
    aTemp.Scale (theP, theS);
    return aTemp;
  }

  void Transform (const gp_Trsf& theT)
  {
    axis.Transform (theT);
    vxdir.Transform (theT);
    vydir.Transform (theT);
  }

  //! Transforms an axis placement with a Trsf.
  //! The "Location" point, the "XDirection" and the
  //! "YDirection" are transformed with theT.  The resulting
  //! main "Direction" of <me> is the cross product between
  //! the "XDirection" and the "YDirection" after transformation.
  Standard_NODISCARD gp_Ax3 Transformed (const gp_Trsf& theT) const
  {
    gp_Ax3 aTemp = *this;
    aTemp.Transform (theT);
    return aTemp;
  }

  void Translate (const gp_Vec& theV) { axis.Translate (theV); }

  //! Translates an axis plaxement in the direction of the vector
  //! <theV>. The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Ax3 Translated (const gp_Vec& theV) const
  {
    gp_Ax3 aTemp = *this;
    aTemp.Translate (theV);
    return aTemp;
  }

  void Translate (const gp_Pnt& theP1, const gp_Pnt& theP2) { Translate (gp_Vec (theP1, theP2)); }

  //! Translates an axis placement from the point <theP1> to the
  //! point <theP2>.
  Standard_NODISCARD gp_Ax3 Translated (const gp_Pnt& theP1, const gp_Pnt& theP2) const { return Translated (gp_Vec (theP1, theP2)); }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  //! Inits the content of me from the stream
  Standard_EXPORT Standard_Boolean InitFromJson (const Standard_SStream& theSStream, Standard_Integer& theStreamPos);

private:

  gp_Ax1 axis;
  gp_Dir vydir;
  gp_Dir vxdir;

};

// =======================================================================
// function : gp_Ax3
// purpose  :
// =======================================================================
inline gp_Ax3::gp_Ax3 (const gp_Ax2& theA)
: axis (theA.Axis()),
  vydir (theA.YDirection()),
  vxdir (theA.XDirection())
{}

// =======================================================================
// function : Ax2
// purpose  :
// =======================================================================
inline gp_Ax2  gp_Ax3::Ax2()const
{
  gp_Dir aZz = axis.Direction();
  if (!Direct())
  {
    aZz.Reverse();
  }
  return gp_Ax2 (axis.Location(), aZz, vxdir);
}

// =======================================================================
// function : SetAxis
// purpose  :
// =======================================================================
inline void  gp_Ax3::SetAxis(const gp_Ax1& theA1)
{
  axis.SetLocation(theA1.Location());
  SetDirection(theA1.Direction());
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
inline void  gp_Ax3::SetDirection(const gp_Dir& theV)
{
  Standard_Real aDot = theV.Dot(vxdir);
  if(1. - Abs(aDot) <= Precision::Angular())
  {
    if(aDot > 0)
    {
      vxdir = vydir;
      vydir = axis.Direction();
    }
    else
    {
      vxdir = axis.Direction();
    }
    axis.SetDirection(theV);
  }
  else
  { 
    Standard_Boolean direct = Direct();
    axis.SetDirection (theV);
    vxdir = theV.CrossCrossed (vxdir, theV);
    if (direct)
    { 
      vydir = theV.Crossed (vxdir); 
    }
    else        
    { 
      vydir = vxdir.Crossed (theV); 
    }
  }
}

// =======================================================================
// function : SetXDirection
// purpose  :
// =======================================================================
inline void  gp_Ax3::SetXDirection (const gp_Dir& theVx)
{
  Standard_Real aDot = theVx.Dot(axis.Direction());
  if (1. - Abs(aDot) <= Precision::Angular())
  {
    if (aDot > 0)
    {
      axis.SetDirection(vxdir);
      vydir = -vydir;
    }
    else
    {
      axis.SetDirection(vxdir);
    }
    vxdir = theVx;
  }
  else
  {
    Standard_Boolean direct = Direct();
    vxdir = axis.Direction().CrossCrossed(theVx, axis.Direction());
    if (direct)
    {
      vydir = axis.Direction().Crossed(vxdir);
    }
    else
    {
      vydir = vxdir.Crossed(axis.Direction());
    }
  }
}

// =======================================================================
// function : SetYDirection
// purpose  :
// =======================================================================
inline void  gp_Ax3::SetYDirection (const gp_Dir& theVy)
{
  Standard_Real aDot = theVy.Dot(axis.Direction());
  if (1. - Abs(aDot) <= Precision::Angular())
  {
    if (aDot > 0)
    {
      axis.SetDirection(vydir);
      vxdir = -vxdir;
    }
    else
    {
      axis.SetDirection(vydir);
    }
    vydir = theVy;
  }
  else
  {
    Standard_Boolean isDirect = Direct();
    vxdir = theVy.Crossed(axis.Direction());
    vydir = (axis.Direction()).Crossed(vxdir);
    if (!isDirect)
    {
      vxdir.Reverse();
    }
  }
}

// =======================================================================
// function : IsCoplanar
// purpose  :
// =======================================================================
inline Standard_Boolean gp_Ax3::IsCoplanar (const gp_Ax3& theOther, 
                                            const Standard_Real theLinearTolerance,
                                            const Standard_Real theAngularTolerance)const
{
  gp_Vec aVec (axis.Location(), theOther.axis.Location());
  Standard_Real aD1 = gp_Vec (axis.Direction()).Dot(aVec);
  if (aD1 < 0)
  {
    aD1 = -aD1;
  }
  Standard_Real aD2 = gp_Vec (theOther.axis.Direction()).Dot(aVec);
  if (aD2 < 0)
  {
    aD2 = -aD2;
  }
  return (aD1 <= theLinearTolerance && aD2 <= theLinearTolerance &&
          axis.IsParallel (theOther.axis, theAngularTolerance));
}

// =======================================================================
// function : IsCoplanar
// purpose  :
// =======================================================================
inline Standard_Boolean gp_Ax3::IsCoplanar (const gp_Ax1& theA1,
                                            const Standard_Real theLinearTolerance,
                                            const Standard_Real theAngularTolerance)const
{
  gp_Vec aVec (axis.Location(), theA1.Location());
  Standard_Real aD1 = gp_Vec (axis.Direction()).Dot (aVec);
  if (aD1 < 0)
  {
    aD1 = -aD1;
  }
  Standard_Real aD2 = (gp_Vec (theA1.Direction()).Crossed (aVec)).Magnitude();
  if (aD2 < 0)
  {
    aD2 = -aD2;
  }
  return (aD1 <= theLinearTolerance && aD2 <= theLinearTolerance &&
          axis.IsNormal (theA1, theAngularTolerance));
}

#endif // _gp_Ax3_HeaderFile
