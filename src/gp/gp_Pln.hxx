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

#ifndef _gp_Pln_HeaderFile
#define _gp_Pln_HeaderFile

#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>

//! Describes a plane.
//! A plane is positioned in space with a coordinate system
//! (a gp_Ax3 object), such that the plane is defined by the
//! origin, "X Direction" and "Y Direction" of this coordinate
//! system, which is the "local coordinate system" of the
//! plane. The "main Direction" of the coordinate system is a
//! vector normal to the plane. It gives the plane an implicit
//! orientation such that the plane is said to be "direct", if the
//! coordinate system is right-handed, or "indirect" in the other case.
//! Note: when a gp_Pln plane is converted into a
//! Geom_Plane plane, some implicit properties of its local
//! coordinate system are used explicitly:
//! -   its origin defines the origin of the two parameters of
//! the planar surface,
//! -   its implicit orientation is also that of the Geom_Plane.
//! See Also
//! gce_MakePln which provides functions for more complex
//! plane constructions
//! Geom_Plane which provides additional functions for
//! constructing planes and works, in particular, with the
//! parametric equations of planes
class gp_Pln 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a plane coincident with OXY plane of the
  //! reference coordinate system.
  gp_Pln() {}

  //! The coordinate system of the plane is defined with the axis
  //! placement theA3.
  //! The "Direction" of theA3 defines the normal to the plane.
  //! The "Location" of theA3 defines the location (origin) of the plane.
  //! The "XDirection" and "YDirection" of theA3 define the "XAxis" and
  //! the "YAxis" of the plane used to parametrize the plane.
  gp_Pln (const gp_Ax3& theA3)
  : pos (theA3)
  {}

  //! Creates a plane with the  "Location" point <theP>
  //! and the normal direction <theV>.
  Standard_EXPORT gp_Pln (const gp_Pnt& theP, const gp_Dir& theV);

  //! Creates a plane from its cartesian equation :
  //! @code
  //! theA * X + theB * Y + theC * Z + theD = 0.0
  //! @endcode
  //! Raises ConstructionError if Sqrt (theA*theA + theB*theB + theC*theC) <= Resolution from gp.
  Standard_EXPORT gp_Pln (const Standard_Real theA, const Standard_Real theB, const Standard_Real theC, const Standard_Real theD);

  //! Returns the coefficients of the plane's cartesian equation :
  //! @code
  //! theA * X + theB * Y + theC * Z + theD = 0.
  //! @endcode
  void Coefficients (Standard_Real& theA, Standard_Real& theB, Standard_Real& theC, Standard_Real& theD) const;

  //! Modifies this plane, by redefining its local coordinate system so that
  //! -   its origin and "main Direction" become those of the
  //! axis theA1 (the "X Direction" and "Y Direction" are then recomputed).
  //! Raises ConstructionError if the theA1 is parallel to the "XAxis" of the plane.
  void SetAxis (const gp_Ax1& theA1) { pos.SetAxis (theA1); }

  //! Changes the origin of the plane.
  void SetLocation (const gp_Pnt& theLoc) { pos.SetLocation (theLoc); }

  //! Changes the local coordinate system of the plane.
  void SetPosition (const gp_Ax3& theA3) { pos = theA3; }

  //! Reverses the   U   parametrization of   the  plane
  //! reversing the XAxis.
  void UReverse() { pos.XReverse(); }

  //! Reverses the   V   parametrization of   the  plane
  //! reversing the YAxis.
  void VReverse() { pos.YReverse(); }

  //! returns true if the Ax3 is right handed.
  Standard_Boolean Direct() const { return pos.Direct(); }

  //! Returns the plane's normal Axis.
  const gp_Ax1& Axis() const { return pos.Axis(); }

  //! Returns the plane's location (origin).
  const gp_Pnt& Location() const { return pos.Location(); }

  //! Returns the local coordinate system of the plane .
  const gp_Ax3& Position() const { return pos; }

  //! Computes the distance between <me> and the point <theP>.
  Standard_Real Distance (const gp_Pnt& theP) const;

  //! Computes the distance between <me> and the line <theL>.
  Standard_Real Distance (const gp_Lin& theL) const;

  //! Computes the distance between two planes.
  Standard_Real Distance (const gp_Pln& theOther) const;

  //! Computes the square distance between <me> and the point <theP>.
  Standard_Real SquareDistance (const gp_Pnt& theP) const
  {
    Standard_Real aD = Distance (theP);
    return aD * aD;
  }

  //! Computes the square distance between <me> and the line <theL>.
  Standard_Real SquareDistance (const gp_Lin& theL) const
  {
    Standard_Real aD = Distance (theL);
    return aD * aD;
  }

  //! Computes the square distance between two planes.
  Standard_Real SquareDistance (const gp_Pln& theOther) const
  {
    Standard_Real aD = Distance (theOther);
    return aD * aD;
  }

  //! Returns the X axis of the plane.
  gp_Ax1 XAxis() const { return gp_Ax1 (pos.Location(), pos.XDirection()); }

  //! Returns the Y axis  of the plane.
  gp_Ax1 YAxis() const { return gp_Ax1 (pos.Location(), pos.YDirection()); }

  //! Returns true if this plane contains the point theP. This means that
  //! -   the distance between point theP and this plane is less
  //! than or equal to theLinearTolerance, or
  //! -   line L is normal to the "main Axis" of the local
  //! coordinate system of this plane, within the tolerance
  //! AngularTolerance, and the distance between the origin
  //! of line L and this plane is less than or equal to
  //! theLinearTolerance.
  Standard_Boolean Contains (const gp_Pnt& theP, const Standard_Real theLinearTolerance) const
  {
    return Distance (theP) <= theLinearTolerance;
  }

  //! Returns true if this plane contains the line theL. This means that
  //! -   the distance between point P and this plane is less
  //! than or equal to LinearTolerance, or
  //! -   line theL is normal to the "main Axis" of the local
  //! coordinate system of this plane, within the tolerance
  //! theAngularTolerance, and the distance between the origin
  //! of line theL and this plane is less than or equal to
  //! theLinearTolerance.
  Standard_Boolean Contains (const gp_Lin& theL, const Standard_Real theLinearTolerance, const Standard_Real theAngularTolerance) const
  {
    return Contains (theL.Location(), theLinearTolerance) &&
                     pos.Direction().IsNormal (theL.Direction(), theAngularTolerance);
  }

  Standard_EXPORT void Mirror (const gp_Pnt& theP);

  //! Performs the symmetrical transformation of a plane with respect
  //! to the point <theP> which is the center of the symmetry
  //! Warnings :
  //! The normal direction to the plane is not changed.
  //! The "XAxis" and the "YAxis" are reversed.
  Standard_NODISCARD Standard_EXPORT gp_Pln Mirrored (const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror (const gp_Ax1& theA1);

  //! Performs   the symmetrical transformation  of a
  //! plane with respect to an axis placement  which is the axis
  //! of  the symmetry.  The  transformation is performed on the
  //! "Location" point, on  the "XAxis"  and the "YAxis".    The
  //! resulting normal  direction  is  the cross product between
  //! the "XDirection" and the "YDirection" after transformation
  //! if  the  initial plane was right  handed,  else  it is the
  //! opposite.
  Standard_NODISCARD Standard_EXPORT gp_Pln Mirrored (const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror (const gp_Ax2& theA2);

  //! Performs the  symmetrical transformation  of  a
  //! plane    with respect to    an axis  placement.   The axis
  //! placement  <A2> locates the plane  of  the symmetry.   The
  //! transformation is performed  on  the  "Location" point, on
  //! the  "XAxis" and  the    "YAxis".  The resulting    normal
  //! direction is the cross  product between   the "XDirection"
  //! and the "YDirection"  after  transformation if the initial
  //! plane was right handed, else it is the opposite.
  Standard_NODISCARD Standard_EXPORT gp_Pln Mirrored (const gp_Ax2& theA2) const;

  void Rotate (const gp_Ax1& theA1, const Standard_Real theAng) { pos.Rotate (theA1, theAng); }

  //! rotates a plane. theA1 is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Pln Rotated (const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Pln aPl = *this;
    aPl.pos.Rotate (theA1, theAng);
    return aPl;
  }

  void Scale (const gp_Pnt& theP, const Standard_Real theS) { pos.Scale (theP, theS); }

  //! Scales a plane. theS is the scaling value.
  Standard_NODISCARD gp_Pln Scaled (const gp_Pnt& theP, const Standard_Real theS) const
  {
    gp_Pln aPl = *this;
    aPl.pos.Scale (theP, theS);
    return aPl;
  }

  void Transform (const gp_Trsf& theT) { pos.Transform (theT); }

  //! Transforms a plane with the transformation theT from class Trsf.
  //! The transformation is performed on the "Location"
  //! point, on the "XAxis" and the "YAxis".
  //! The resulting normal direction is the cross product between
  //! the "XDirection" and the "YDirection" after transformation.
  Standard_NODISCARD gp_Pln Transformed (const gp_Trsf& theT) const
  {
    gp_Pln aPl = *this;
    aPl.pos.Transform (theT);
    return aPl;
  }

  void Translate (const gp_Vec& theV) { pos.Translate (theV); }

  //! Translates a plane in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Pln Translated (const gp_Vec& theV) const
  {
    gp_Pln aPl = *this;
    aPl.pos.Translate (theV);
    return aPl;
  }

   void Translate (const gp_Pnt& theP1, const gp_Pnt& theP2) { pos.Translate (theP1, theP2); }

  //! Translates a plane from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Pln Translated (const gp_Pnt& theP1, const gp_Pnt& theP2) const
  {
    gp_Pln aPl = *this;
    aPl.pos.Translate (theP1, theP2);
    return aPl;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:

  gp_Ax3 pos;

};

#include <gp_Lin.hxx>

//=======================================================================
//function : Coefficients
// purpose :
//=======================================================================
inline void gp_Pln::Coefficients (Standard_Real& theA,
                                  Standard_Real& theB,
                                  Standard_Real& theC,
                                  Standard_Real& theD) const
{
  const gp_Dir& aDir = pos.Direction();
  if (pos.Direct())
  {
    theA = aDir.X();
    theB = aDir.Y();
    theC = aDir.Z();
  }
  else
  {
    theA = -aDir.X();
    theB = -aDir.Y();
    theC = -aDir.Z();
  }
  const gp_Pnt& aP = pos.Location();
  theD = -(theA * aP.X() + theB * aP.Y() + theC * aP.Z());
}

//=======================================================================
//function : Distance
// purpose :
//=======================================================================
inline Standard_Real gp_Pln::Distance (const gp_Pnt& theP) const
{
  const gp_Pnt& aLoc = pos.Location ();
  const gp_Dir& aDir = pos.Direction();
  Standard_Real aD = (aDir.X() * (theP.X() - aLoc.X()) +
                      aDir.Y() * (theP.Y() - aLoc.Y()) +
                      aDir.Z() * (theP.Z() - aLoc.Z()));
  if (aD < 0)
  {
    aD = -aD;
  }
  return aD;
}

//=======================================================================
//function : Distance
// purpose :
//=======================================================================
inline Standard_Real gp_Pln::Distance (const gp_Lin& theL)  const
{
  Standard_Real aD = 0.0;
  if ((pos.Direction()).IsNormal (theL.Direction(), gp::Resolution()))
  {
    const gp_Pnt& aP = theL.Location();
    const gp_Pnt& aLoc = pos.Location();
    const gp_Dir& aDir = pos.Direction();
    aD = (aDir.X() * (aP.X() - aLoc.X()) +
          aDir.Y() * (aP.Y() - aLoc.Y()) +
          aDir.Z() * (aP.Z() - aLoc.Z()));
    if (aD < 0)
    {
      aD = -aD;
    }
  }
  return aD;
}

//=======================================================================
//function : Distance
// purpose :
//=======================================================================
inline Standard_Real gp_Pln::Distance (const gp_Pln& theOther) const
{
  Standard_Real aD = 0.0;
  if ((pos.Direction()).IsParallel (theOther.pos.Direction(), gp::Resolution()))
  {
    const gp_Pnt& aP = theOther.pos.Location();
    const gp_Pnt& aLoc = pos.Location ();
    const gp_Dir& aDir = pos.Direction();
    aD = (aDir.X() * (aP.X() - aLoc.X()) +
          aDir.Y() * (aP.Y() - aLoc.Y()) +
          aDir.Z() * (aP.Z() - aLoc.Z()));
    if (aD < 0)
    {
      aD = -aD;
    }
  }
  return aD;
}

#endif // _gp_Pln_HeaderFile
