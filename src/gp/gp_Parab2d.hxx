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

#ifndef _gp_Parab2d_HeaderFile
#define _gp_Parab2d_HeaderFile

#include <gp_Ax22d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_ConstructionError.hxx>

//! Describes a parabola in the plane (2D space).
//! A parabola is defined by its focal length (that is, the
//! distance between its focus and apex) and positioned in
//! the plane with a coordinate system (a gp_Ax22d object) where:
//! -   the origin of the coordinate system is on the apex of
//! the parabola, and
//! -   the "X Axis" of the coordinate system is the axis of
//! symmetry; the parabola is on the positive side of this axis.
//! This coordinate system is the "local coordinate system"
//! of the parabola. Its orientation (direct or indirect sense)
//! gives an implicit orientation to the parabola.
//! In this coordinate system, the equation for the parabola is:
//! @code
//! Y**2 = (2*P) * X.
//! @endcode
//! where P, referred to as the parameter of the parabola, is
//! the distance between the focus and the directrix (P is
//! twice the focal length).
//! See Also
//! GCE2d_MakeParab2d which provides functions for
//! more complex parabola constructions
//! Geom2d_Parabola which provides additional functions
//! for constructing parabolas and works, in particular, with
//! the parametric equations of parabolas
class gp_Parab2d 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates an indefinite parabola.
  gp_Parab2d()
  : focalLength (RealLast())
  {}

  //! Creates a parabola with its vertex point, its axis of symmetry
  //! ("XAxis") and its focal length.
  //! The sense of parametrization is given by theSense. If theSense == TRUE
  //! (by default) then right-handed coordinate system is used,
  //! otherwise - left-handed.
  //! Warnings : It is possible to have FocalLength = 0. In this case,
  //! the parabola looks like a line, which is parallel to the symmetry-axis.
  //! Raises ConstructionError if FocalLength < 0.0
  gp_Parab2d (const gp_Ax2d& theMirrorAxis,
              const Standard_Real theFocalLength,
              const Standard_Boolean theSense = Standard_True)
  : focalLength (theFocalLength)
  {
    pos = gp_Ax22d (theMirrorAxis, theSense);
    Standard_ConstructionError_Raise_if (theFocalLength < 0.0, "gp_Parab2d() - focal length should be >= 0");
  }

  //! Creates a parabola with its vertex point, its axis of symmetry
  //! ("XAxis"), correspond Y-axis and its focal length.
  //! Warnings : It is possible to have FocalLength = 0. In this case,
  //! the parabola looks like a line, which is parallel to the symmetry-axis.
  //! Raises ConstructionError if Focal < 0.0
  gp_Parab2d (const gp_Ax22d& theAxes, const Standard_Real theFocalLength)
  : pos (theAxes),
    focalLength (theFocalLength)
  {
    Standard_ConstructionError_Raise_if (theFocalLength < 0.0, "gp_Parab2d() - focal length should be >= 0");
  }

  //! Creates a parabola with the directrix and the focus point.
  //! Y-axis of the parabola (in User Coordinate System - UCS) is
  //! the direction of theDirectrix. X-axis always directs from theDirectrix
  //! to theFocus point and always comes through theFocus.
  //! Apex of the parabola is a middle point between the theFocus and the
  //! intersection point of theDirectrix and the X-axis.
  //! Warnings : It is possible to have FocalLength = 0 (when theFocus lies
  //! in theDirectrix). In this case, X-direction of the parabola is defined 
  //! by theSense parameter. If theSense == TRUE (by default) then right-handed
  //! coordinate system is used, otherwise - left-handed. Result parabola will look
  //! like a line, which is perpendicular to the directrix.
  Standard_EXPORT gp_Parab2d (const gp_Ax2d& theDirectrix,
                              const gp_Pnt2d& theFocus,
                              const Standard_Boolean theSense = Standard_True);

  //! Changes the focal distance of the parabola
  //! Warnings : It is possible to have theFocal = 0.
  //! Raises ConstructionError if theFocal < 0.0
  void SetFocal (const Standard_Real theFocal)
  {
    Standard_ConstructionError_Raise_if (theFocal < 0.0, "gp_Parab2d::SetFocal() - focal length should be >= 0");
    focalLength = theFocal;
  }

  //! Changes the "Location" point of the parabola. It is the
  //! vertex of the parabola.
  void SetLocation (const gp_Pnt2d& theP) { pos.SetLocation (theP); }

  //! Modifies this parabola, by redefining its local coordinate system so that
  //! its origin and "X Direction" become those of the axis
  //! MA. The "Y Direction" of the local coordinate system is
  //! then recomputed. The orientation of the local
  //! coordinate system is not modified.
  void SetMirrorAxis (const gp_Ax2d& theA) { pos.SetXAxis (theA); }

  //! Changes the local coordinate system of the parabola.
  //! The "Location" point of A becomes the vertex of the parabola.
  void SetAxis (const gp_Ax22d& theA) { pos.SetAxis (theA); }

  //! Computes the coefficients of the implicit equation of the parabola
  //! (in WCS - World Coordinate System).
  //! @code
  //! theA * (X**2) + theB * (Y**2) + 2*theC*(X*Y) + 2*theD*X + 2*theE*Y + theF = 0.
  //! @endcode
  Standard_EXPORT void Coefficients (Standard_Real& theA, Standard_Real& theB,
                                     Standard_Real& theC, Standard_Real& theD,
                                     Standard_Real& theE, Standard_Real& theF) const;

  //! Computes the directrix of the parabola.
  //! The directrix is:
  //! -   a line parallel to the "Y Direction" of the local
  //! coordinate system of this parabola, and
  //! -   located on the negative side of the axis of symmetry,
  //! at a distance from the apex which is equal to the focal  length of this parabola.
  //! The directrix is returned as an axis (a gp_Ax2d object),
  //! the origin of which is situated on the "X Axis" of this parabola.
  gp_Ax2d Directrix() const;

  //! Returns the distance between the vertex and the focus
  //! of the parabola.
  Standard_Real Focal() const { return focalLength; }

  //! Returns the focus of the parabola.
  gp_Pnt2d Focus() const
  {
    return gp_Pnt2d (pos.Location().X() + focalLength * pos.XDirection().X(),
                     pos.Location().Y() + focalLength * pos.XDirection().Y());
  }

  //! Returns the vertex of the parabola.
  gp_Pnt2d Location() const { return pos.Location(); }

  //! Returns the symmetry axis of the parabola.
  //! The "Location" point of this axis is the vertex of the parabola.
  gp_Ax2d MirrorAxis() const { return pos.XAxis(); }

  //! Returns the local coordinate system of the parabola.
  //! The "Location" point of this axis is the vertex of the parabola.
  gp_Ax22d Axis() const { return pos; }

  //! Returns the distance between the focus and the
  //! directrix of the parabola.
  Standard_Real Parameter() const { return 2.0 * focalLength; }

  void Reverse()
  {
    gp_Dir2d aTemp = pos.YDirection();
    aTemp.Reverse();
    pos.SetAxis (gp_Ax22d (pos.Location(), pos.XDirection(), aTemp));
  }

  //! Reverses the orientation of the local coordinate system
  //! of this parabola (the "Y Direction" is reversed).
  //! Therefore, the implicit orientation of this parabola is reversed.
  //! Note:
  //! -   Reverse assigns the result to this parabola, while
  //! -   Reversed creates a new one.
  Standard_NODISCARD gp_Parab2d Reversed() const;

  //! Returns true if the local coordinate system is direct
  //! and false in the other case.
  Standard_Boolean IsDirect() const
  {
    return (pos.XDirection().Crossed (pos.YDirection())) >= 0.0;
  }

  Standard_EXPORT void Mirror (const gp_Pnt2d& theP);

  //! Performs the symmetrical transformation of a parabola with respect
  //! to the point theP which is the center of the symmetry
  Standard_NODISCARD Standard_EXPORT gp_Parab2d Mirrored (const gp_Pnt2d& theP) const;

  Standard_EXPORT void Mirror (const gp_Ax2d& theA);

  //! Performs the symmetrical transformation of a parabola with respect
  //! to an axis placement which is the axis of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Parab2d Mirrored (const gp_Ax2d& theA) const;

  void Rotate (const gp_Pnt2d& theP, const Standard_Real theAng) { pos.Rotate (theP, theAng); }

  //! Rotates a parabola. theP is the center of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Parab2d Rotated (const gp_Pnt2d& theP, const Standard_Real theAng) const
  {
    gp_Parab2d aPrb = *this;
    aPrb.pos.Rotate (theP, theAng);
    return aPrb;
  }

  void Scale (const gp_Pnt2d& theP, const Standard_Real theS);

  //! Scales a parabola. theS is the scaling value.
  //! If theS is negative the direction of the symmetry axis
  //! "XAxis" is reversed and the direction of the "YAxis" too.
  Standard_NODISCARD gp_Parab2d Scaled (const gp_Pnt2d& theP, const Standard_Real theS) const;

  void Transform (const gp_Trsf2d& theT);

  //! Transforms an parabola with the transformation theT from class Trsf2d.
  Standard_NODISCARD gp_Parab2d Transformed (const gp_Trsf2d& theT) const;

  void Translate (const gp_Vec2d& theV) { pos.Translate (theV); }

  //! Translates a parabola in the direction of the vectorthe theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Parab2d Translated (const gp_Vec2d& theV) const
  {
    gp_Parab2d aPrb = *this;
    aPrb.pos.Translate (theV);
    return aPrb;
  }

  void Translate (const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) { pos.Translate (theP1, theP2); }

  //! Translates a parabola from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Parab2d Translated (const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) const
  {
    gp_Parab2d aPrb = *this;
    aPrb.pos.Translate (theP1, theP2);
    return aPrb;
  }

private:

  gp_Ax22d pos;
  Standard_Real focalLength;

};

//=======================================================================
//function : Directrix
// purpose :
//=======================================================================
inline gp_Ax2d gp_Parab2d::Directrix() const
{
  gp_Pnt2d aP (pos.Location().X() - focalLength * pos.XDirection().X(),
               pos.Location().Y() - focalLength * pos.XDirection().Y());
  gp_Dir2d aV (pos.YDirection());
  return gp_Ax2d (aP, aV);
}

//=======================================================================
//function : Reversed
// purpose :
//=======================================================================
inline gp_Parab2d gp_Parab2d::Reversed() const
{
  gp_Parab2d aP = *this;
  gp_Dir2d aTemp = pos.YDirection();
  aTemp.Reverse();
  aP.pos.SetAxis (gp_Ax22d (pos.Location(), pos.XDirection(), aTemp));
  return aP;
}

//=======================================================================
//function : Scale
// purpose :
//=======================================================================
inline void gp_Parab2d::Scale (const gp_Pnt2d& theP, const Standard_Real theS)
{
  focalLength *= theS;
  if (focalLength < 0)
  {
    focalLength = -focalLength;
  }
  pos.Scale (theP, theS);
}

//=======================================================================
//function : Scaled
// purpose :
//=======================================================================
inline gp_Parab2d gp_Parab2d::Scaled (const gp_Pnt2d& theP, const Standard_Real theS) const
{
  gp_Parab2d aPrb = *this;
  aPrb.focalLength *= theS;
  if (aPrb.focalLength < 0)
  {
    aPrb.focalLength = -aPrb.focalLength;
  }
  aPrb.pos.Scale (theP, theS);
  return aPrb;
}

//=======================================================================
//function : Transform
// purpose :
//=======================================================================
inline void gp_Parab2d::Transform (const gp_Trsf2d& theT)
{
  focalLength *= theT.ScaleFactor();
  if (focalLength < 0)
  {
    focalLength = -focalLength;
  }
  pos.Transform (theT);
}

//=======================================================================
//function : Transformed
// purpose :
//=======================================================================
inline gp_Parab2d gp_Parab2d::Transformed (const gp_Trsf2d& theT) const
{
  gp_Parab2d aPrb = *this;
  aPrb.focalLength *= theT.ScaleFactor();
  if (aPrb.focalLength < 0)
  {
    aPrb.focalLength = -aPrb.focalLength;
  }
  aPrb.pos.Transform (theT);
  return aPrb;
}

#endif // _gp_Parab2d_HeaderFile
