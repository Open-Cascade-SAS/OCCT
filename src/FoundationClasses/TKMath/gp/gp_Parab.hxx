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

#include <gp_Ax1.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Standard_ConstructionError.hxx>

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
//! @code
//! Y**2 = (2*P) * X.
//! @endcode
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
  gp_Parab()
      : focalLength(RealLast())
  {
  }

  //! Creates a parabola with its local coordinate system "theA2"
  //! and it's focal length "Focal".
  //! The XDirection of theA2 defines the axis of symmetry of the
  //! parabola. The YDirection of theA2 is parallel to the directrix
  //! of the parabola. The Location point of theA2 is the vertex of
  //! the parabola
  //! Raises ConstructionError if theFocal < 0.0
  //! Raised if theFocal < 0.0
  gp_Parab(const gp_Ax2& theA2, const Standard_Real theFocal)
      : pos(theA2),
        focalLength(theFocal)
  {
    Standard_ConstructionError_Raise_if(theFocal < 0.0, "gp_Parab() - focal length should be >= 0");
  }

  //! theD is the directrix of the parabola and theF the focus point.
  //! The symmetry axis (XAxis) of the parabola is normal to the
  //! directrix and pass through the focus point theF, but its
  //! location point is the vertex of the parabola.
  //! The YAxis of the parabola is parallel to theD and its location
  //! point is the vertex of the parabola. The normal to the plane
  //! of the parabola is the cross product between the XAxis and the
  //! YAxis.
  gp_Parab(const gp_Ax1& theD, const gp_Pnt& theF);

  //! Modifies this parabola by redefining its local coordinate system so that
  //! -   its origin and "main Direction" become those of the
  //! axis theA1 (the "X Direction" and "Y Direction" are then
  //! recomputed in the same way as for any gp_Ax2)
  //! Raises ConstructionError if the direction of theA1 is parallel to the previous
  //! XAxis of the parabola.
  void SetAxis(const gp_Ax1& theA1) { pos.SetAxis(theA1); }

  //! Changes the focal distance of the parabola.
  //! Raises ConstructionError if theFocal < 0.0
  void SetFocal(const Standard_Real theFocal)
  {
    Standard_ConstructionError_Raise_if(theFocal < 0.0,
                                        "gp_Parab::SetFocal() - focal length should be >= 0");
    focalLength = theFocal;
  }

  //! Changes the location of the parabola. It is the vertex of
  //! the parabola.
  void SetLocation(const gp_Pnt& theP) { pos.SetLocation(theP); }

  //! Changes the local coordinate system of the parabola.
  void SetPosition(const gp_Ax2& theA2) { pos = theA2; }

  //! Returns the main axis of the parabola.
  //! It is the axis normal to the plane of the parabola passing
  //! through the vertex of the parabola.
  const gp_Ax1& Axis() const { return pos.Axis(); }

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
  Standard_Real Focal() const { return focalLength; }

  //! -   Computes the focus of the parabola.
  gp_Pnt Focus() const;

  //! Returns the vertex of the parabola. It is the "Location"
  //! point of the coordinate system of the parabola.
  const gp_Pnt& Location() const { return pos.Location(); }

  //! Computes the parameter of the parabola.
  //! It is the distance between the focus and the directrix of
  //! the parabola. This distance is twice the focal length.
  Standard_Real Parameter() const { return 2.0 * focalLength; }

  //! Returns the local coordinate system of the parabola.
  const gp_Ax2& Position() const { return pos; }

  //! Returns the symmetry axis of the parabola. The location point
  //! of the axis is the vertex of the parabola.
  gp_Ax1 XAxis() const { return gp_Ax1(pos.Location(), pos.XDirection()); }

  //! It is an axis parallel to the directrix of the parabola.
  //! The location point of this axis is the vertex of the parabola.
  gp_Ax1 YAxis() const { return gp_Ax1(pos.Location(), pos.YDirection()); }

  Standard_EXPORT void Mirror(const gp_Pnt& theP);

  //! Performs the symmetrical transformation of a parabola
  //! with respect to the point theP which is the center of the
  //! symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Parab Mirrored(const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror(const gp_Ax1& theA1);

  //! Performs the symmetrical transformation of a parabola
  //! with respect to an axis placement which is the axis of
  //! the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Parab Mirrored(const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror(const gp_Ax2& theA2);

  //! Performs the symmetrical transformation of a parabola
  //! with respect to a plane. The axis placement theA2 locates
  //! the plane of the symmetry (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Parab Mirrored(const gp_Ax2& theA2) const;

  void Rotate(const gp_Ax1& theA1, const Standard_Real theAng) { pos.Rotate(theA1, theAng); }

  //! Rotates a parabola. theA1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Parab Rotated(const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Parab aPrb = *this;
    aPrb.pos.Rotate(theA1, theAng);
    return aPrb;
  }

  void Scale(const gp_Pnt& theP, const Standard_Real theS);

  //! Scales a parabola. theS is the scaling value.
  //! If theS is negative the direction of the symmetry axis
  //! XAxis is reversed and the direction of the YAxis too.
  Standard_NODISCARD gp_Parab Scaled(const gp_Pnt& theP, const Standard_Real theS) const;

  void Transform(const gp_Trsf& theT);

  //! Transforms a parabola with the transformation theT from class Trsf.
  Standard_NODISCARD gp_Parab Transformed(const gp_Trsf& theT) const;

  void Translate(const gp_Vec& theV) { pos.Translate(theV); }

  //! Translates a parabola in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Parab Translated(const gp_Vec& theV) const
  {
    gp_Parab aPrb = *this;
    aPrb.pos.Translate(theV);
    return aPrb;
  }

  void Translate(const gp_Pnt& theP1, const gp_Pnt& theP2) { pos.Translate(theP1, theP2); }

  //! Translates a parabola from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Parab Translated(const gp_Pnt& theP1, const gp_Pnt& theP2) const
  {
    gp_Parab aPrb = *this;
    aPrb.pos.Translate(theP1, theP2);
    return aPrb;
  }

private:
  gp_Ax2        pos;
  Standard_Real focalLength;
};

//=======================================================================
// function : gp_Parab
// purpose :
//=======================================================================
inline gp_Parab::gp_Parab(const gp_Ax1& theD, const gp_Pnt& theF)
{
  gp_Lin aDroite(theD);
  focalLength        = aDroite.Distance(theF) / 2.;
  gp_Ax1        anAx = aDroite.Normal(theF).Position();
  gp_Ax1        anAy = aDroite.Position();
  const gp_Dir& aDD  = anAx.Direction();
  pos                = gp_Ax2(gp_Pnt(theF.X() - focalLength * aDD.X(),
                      theF.Y() - focalLength * aDD.Y(),
                      theF.Z() - focalLength * aDD.Z()),
               anAx.Direction().Crossed(anAy.Direction()),
               anAx.Direction());
}

//=======================================================================
// function : Directrix
// purpose :
//=======================================================================
inline gp_Ax1 gp_Parab::Directrix() const
{
  const gp_Pnt& aPP = pos.Location();
  const gp_Dir& aDD = pos.XDirection();
  gp_Pnt        aP(aPP.X() - focalLength * aDD.X(),
            aPP.Y() - focalLength * aDD.Y(),
            aPP.Z() - focalLength * aDD.Z());
  return gp_Ax1(aP, pos.YDirection());
}

//=======================================================================
// function : Focus
// purpose :
//=======================================================================
inline gp_Pnt gp_Parab::Focus() const
{
  const gp_Pnt& aPP = pos.Location();
  const gp_Dir& aDD = pos.XDirection();
  return gp_Pnt(aPP.X() + focalLength * aDD.X(),
                aPP.Y() + focalLength * aDD.Y(),
                aPP.Z() + focalLength * aDD.Z());
}

//=======================================================================
// function : Scale
// purpose :
//=======================================================================
inline void gp_Parab::Scale(const gp_Pnt& theP, const Standard_Real theS)
{
  focalLength *= theS;
  if (focalLength < 0)
  {
    focalLength = -focalLength;
  }
  pos.Scale(theP, theS);
}

//=======================================================================
// function : Scaled
// purpose :
//=======================================================================
inline gp_Parab gp_Parab::Scaled(const gp_Pnt& theP, const Standard_Real theS) const
{
  gp_Parab aPrb = *this;
  aPrb.focalLength *= theS;
  if (aPrb.focalLength < 0)
  {
    aPrb.focalLength = -aPrb.focalLength;
  }
  aPrb.pos.Scale(theP, theS);
  return aPrb;
}

//=======================================================================
// function : Transform
// purpose :
//=======================================================================
inline void gp_Parab::Transform(const gp_Trsf& theT)
{
  focalLength *= theT.ScaleFactor();
  if (focalLength < 0)
  {
    focalLength = -focalLength;
  }
  pos.Transform(theT);
}

//=======================================================================
// function : Transformed
// purpose :
//=======================================================================
inline gp_Parab gp_Parab::Transformed(const gp_Trsf& theT) const
{
  gp_Parab aPrb = *this;
  aPrb.focalLength *= theT.ScaleFactor();
  if (aPrb.focalLength < 0)
  {
    aPrb.focalLength = -aPrb.focalLength;
  }
  aPrb.pos.Transform(theT);
  return aPrb;
}

#endif // _gp_Parab_HeaderFile
