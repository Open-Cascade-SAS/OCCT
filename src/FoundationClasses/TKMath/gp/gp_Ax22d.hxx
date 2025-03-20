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

#ifndef _gp_Ax22d_HeaderFile
#define _gp_Ax22d_HeaderFile

#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>

//! Describes a coordinate system in a plane (2D space).
//! A coordinate system is defined by:
//! -   its origin (also referred to as its "Location point"), and
//! -   two orthogonal unit vectors, respectively, called the "X
//! Direction" and the "Y Direction".
//! A gp_Ax22d may be right-handed ("direct sense") or
//! left-handed ("inverse" or "indirect sense").
//! You use a gp_Ax22d to:
//! - describe 2D geometric entities, in particular to position
//! them. The local coordinate system of a geometric
//! entity serves for the same purpose as the STEP
//! function "axis placement two axes", or
//! -   define geometric transformations.
//! Note: we refer to the "X Axis" and "Y Axis" as the axes having:
//! -   the origin of the coordinate system as their origin, and
//! -   the unit vectors "X Direction" and "Y Direction",
//! respectively, as their unit vectors.
class gp_Ax22d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an object representing the reference
  //! coordinate system (OXY).
  gp_Ax22d()
      : vydir(0., 1.)
  // vxdir(1.,0.) use default ctor of gp_Dir2d, as it creates the same dir(1, 0)
  {
  }

  //! Creates a coordinate system with origin theP and where:
  //! -   theVx is the "X Direction", and
  //! -   the "Y Direction" is orthogonal to theVx and
  //! oriented so that the cross products theVx^"Y
  //! Direction" and theVx^theVy have the same sign.
  //! Raises ConstructionError if theVx and theVy are parallel (same or opposite orientation).
  gp_Ax22d(const gp_Pnt2d& theP, const gp_Dir2d& theVx, const gp_Dir2d& theVy)
      : point(theP),
        vydir(theVy),
        vxdir(theVx)
  {
    Standard_Real aValue = theVx.Crossed(theVy);
    if (aValue >= 0.0)
    {
      vydir.SetCoord(-vxdir.Y(), vxdir.X());
    }
    else
    {
      vydir.SetCoord(vxdir.Y(), -vxdir.X());
    }
  }

  //! Creates -   a coordinate system with origin theP and "X Direction"
  //! theV, which is:
  //! -   right-handed if theIsSense is true (default value), or
  //! -   left-handed if theIsSense is false
  gp_Ax22d(const gp_Pnt2d&        theP,
           const gp_Dir2d&        theV,
           const Standard_Boolean theIsSense = Standard_True)
      : point(theP),
        vxdir(theV)
  {
    if (theIsSense)
    {
      vydir.SetCoord(-theV.Y(), theV.X());
    }
    else
    {
      vydir.SetCoord(theV.Y(), -theV.X());
    }
  }

  //! Creates -   a coordinate system where its origin is the origin of
  //! theA and its "X Direction" is the unit vector of theA, which   is:
  //! -   right-handed if theIsSense is true (default value), or
  //! -   left-handed if theIsSense is false.
  gp_Ax22d(const gp_Ax2d& theA, const Standard_Boolean theIsSense = Standard_True)
      : point(theA.Location()),
        vxdir(theA.Direction())
  {
    if (theIsSense)
    {
      vydir.SetCoord(-vxdir.Y(), vxdir.X());
    }
    else
    {
      vydir.SetCoord(vxdir.Y(), -vxdir.X());
    }
  }

  //! Assigns the origin and the two unit vectors of the
  //! coordinate system theA1 to this coordinate system.
  void SetAxis(const gp_Ax22d& theA1)
  {
    point = theA1.Location();
    vxdir = theA1.XDirection();
    vydir = theA1.YDirection();
  }

  //! Changes the XAxis and YAxis ("Location" point and "Direction")
  //! of <me>.
  //! The "YDirection" is recomputed in the same sense as before.
  void SetXAxis(const gp_Ax2d& theA1);

  //! Changes the XAxis and YAxis ("Location" point and "Direction") of <me>.
  //! The "XDirection" is recomputed in the same sense as before.
  void SetYAxis(const gp_Ax2d& theA1);

  //! Changes the "Location" point (origin) of <me>.
  void SetLocation(const gp_Pnt2d& theP) { point = theP; }

  //! Assigns theVx to the "X Direction"  of
  //! this coordinate system. The other unit vector of this
  //! coordinate system is recomputed, normal to theVx ,
  //! without modifying the orientation (right-handed or
  //! left-handed) of this coordinate system.
  void SetXDirection(const gp_Dir2d& theVx);

  //! Assignsr theVy to the  "Y Direction" of
  //! this coordinate system. The other unit vector of this
  //! coordinate system is recomputed, normal to theVy,
  //! without modifying the orientation (right-handed or
  //! left-handed) of this coordinate system.
  void SetYDirection(const gp_Dir2d& theVy);

  //! Returns an axis, for which
  //! -   the origin is that of this coordinate system, and
  //! -   the unit vector is either the "X Direction"  of this coordinate system.
  //! Note: the result is the "X Axis" of this coordinate system.
  gp_Ax2d XAxis() const { return gp_Ax2d(point, vxdir); }

  //! Returns an axis, for which
  //! -   the origin is that of this coordinate system, and
  //! - the unit vector is either the  "Y Direction" of this coordinate system.
  //! Note: the result is the "Y Axis" of this coordinate system.
  gp_Ax2d YAxis() const { return gp_Ax2d(point, vydir); }

  //! Returns the "Location" point (origin) of <me>.
  const gp_Pnt2d& Location() const { return point; }

  //! Returns the "XDirection" of <me>.
  const gp_Dir2d& XDirection() const { return vxdir; }

  //! Returns the "YDirection" of <me>.
  const gp_Dir2d& YDirection() const { return vydir; }

  Standard_EXPORT void Mirror(const gp_Pnt2d& theP);

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to the point theP which is the
  //! center of the symmetry.
  //! Warnings :
  //! The main direction of the axis placement is not changed.
  //! The "XDirection" and the "YDirection" are reversed.
  //! So the axis placement stay right handed.
  Standard_NODISCARD Standard_EXPORT gp_Ax22d Mirrored(const gp_Pnt2d& theP) const;

  Standard_EXPORT void Mirror(const gp_Ax2d& theA);

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to an axis placement which
  //! is the axis of the symmetry.
  //! The transformation is performed on the "Location"
  //! point, on the "XDirection" and "YDirection".
  //! The resulting main "Direction" is the cross product between
  //! the "XDirection" and the "YDirection" after transformation.
  Standard_NODISCARD Standard_EXPORT gp_Ax22d Mirrored(const gp_Ax2d& theA) const;

  void Rotate(const gp_Pnt2d& theP, const Standard_Real theAng);

  //! Rotates an axis placement. <theA1> is the axis of the
  //! rotation . theAng is the angular value of the rotation
  //! in radians.
  Standard_NODISCARD gp_Ax22d Rotated(const gp_Pnt2d& theP, const Standard_Real theAng) const
  {
    gp_Ax22d aTemp = *this;
    aTemp.Rotate(theP, theAng);
    return aTemp;
  }

  void Scale(const gp_Pnt2d& theP, const Standard_Real theS);

  //! Applies a scaling transformation on the axis placement.
  //! The "Location" point of the axisplacement is modified.
  //! Warnings :
  //! If the scale <theS> is negative :
  //! . the main direction of the axis placement is not changed.
  //! . The "XDirection" and the "YDirection" are reversed.
  //! So the axis placement stay right handed.
  Standard_NODISCARD gp_Ax22d Scaled(const gp_Pnt2d& theP, const Standard_Real theS) const
  {
    gp_Ax22d aTemp = *this;
    aTemp.Scale(theP, theS);
    return aTemp;
  }

  void Transform(const gp_Trsf2d& theT);

  //! Transforms an axis placement with a Trsf.
  //! The "Location" point, the "XDirection" and the
  //! "YDirection" are transformed with theT.  The resulting
  //! main "Direction" of <me> is the cross product between
  //! the "XDirection" and the "YDirection" after transformation.
  Standard_NODISCARD gp_Ax22d Transformed(const gp_Trsf2d& theT) const
  {
    gp_Ax22d aTemp = *this;
    aTemp.Transform(theT);
    return aTemp;
  }

  void Translate(const gp_Vec2d& theV) { point.Translate(theV); }

  //! Translates an axis plaxement in the direction of the vector
  //! <theV>. The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Ax22d Translated(const gp_Vec2d& theV) const
  {
    gp_Ax22d aTemp = *this;
    aTemp.Translate(theV);
    return aTemp;
  }

  void Translate(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) { point.Translate(theP1, theP2); }

  //! Translates an axis placement from the point <theP1> to the
  //! point <theP2>.
  Standard_NODISCARD gp_Ax22d Translated(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) const
  {
    gp_Ax22d aTemp = *this;
    aTemp.Translate(theP1, theP2);
    return aTemp;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:
  gp_Pnt2d point;
  gp_Dir2d vydir;
  gp_Dir2d vxdir;
};

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
inline void gp_Ax22d::SetXAxis(const gp_Ax2d& theA1)
{
  Standard_Boolean isSign = (vxdir.Crossed(vydir)) >= 0.0;
  point                   = theA1.Location();
  vxdir                   = theA1.Direction();
  if (isSign)
  {
    vydir.SetCoord(-vxdir.Y(), vxdir.X());
  }
  else
  {
    vydir.SetCoord(vxdir.Y(), -vxdir.X());
  }
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
inline void gp_Ax22d::SetYAxis(const gp_Ax2d& theA1)
{
  Standard_Boolean isSign = (vxdir.Crossed(vydir)) >= 0.0;
  point                   = theA1.Location();
  vydir                   = theA1.Direction();
  if (isSign)
  {
    vxdir.SetCoord(vydir.Y(), -vydir.X());
  }
  else
  {
    vxdir.SetCoord(-vydir.Y(), vydir.X());
  }
}

// =======================================================================
// function : SetXDirection
// purpose  :
// =======================================================================
inline void gp_Ax22d::SetXDirection(const gp_Dir2d& theVx)
{
  Standard_Boolean isSign = (vxdir.Crossed(vydir)) >= 0.0;
  vxdir                   = theVx;
  if (isSign)
  {
    vydir.SetCoord(-theVx.Y(), theVx.X());
  }
  else
  {
    vydir.SetCoord(theVx.Y(), -theVx.X());
  }
}

// =======================================================================
// function : SetYDirection
// purpose  :
// =======================================================================
inline void gp_Ax22d::SetYDirection(const gp_Dir2d& theVy)
{
  Standard_Boolean isSign = (vxdir.Crossed(vydir)) >= 0.0;
  vydir                   = theVy;
  if (isSign)
  {
    vxdir.SetCoord(theVy.Y(), -theVy.X());
  }
  else
  {
    vxdir.SetCoord(-theVy.Y(), theVy.X());
  }
}

// =======================================================================
// function : Rotate
// purpose  :
// =======================================================================
inline void gp_Ax22d::Rotate(const gp_Pnt2d& theP, const Standard_Real theAng)
{
  gp_Pnt2d aTemp = point;
  aTemp.Rotate(theP, theAng);
  point = aTemp;
  vxdir.Rotate(theAng);
  vydir.Rotate(theAng);
}

// =======================================================================
// function : Scale
// purpose  :
// =======================================================================
inline void gp_Ax22d::Scale(const gp_Pnt2d& theP, const Standard_Real theS)
{
  gp_Pnt2d aTemp = point;
  aTemp.Scale(theP, theS);
  point = aTemp;
  if (theS < 0.0)
  {
    vxdir.Reverse();
    vydir.Reverse();
  }
}

// =======================================================================
// function : Transform
// purpose  :
// =======================================================================
inline void gp_Ax22d::Transform(const gp_Trsf2d& theT)
{
  gp_Pnt2d aTemp = point;
  aTemp.Transform(theT);
  point = aTemp;
  vxdir.Transform(theT);
  vydir.Transform(theT);
}

#endif // _gp_Ax22d_HeaderFile
