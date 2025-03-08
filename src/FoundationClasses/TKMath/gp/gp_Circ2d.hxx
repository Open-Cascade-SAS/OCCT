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

#ifndef _gp_Circ2d_HeaderFile
#define _gp_Circ2d_HeaderFile

#include <gp_Ax22d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_ConstructionError.hxx>

//! Describes a circle in the plane (2D space).
//! A circle is defined by its radius and positioned in the
//! plane with a coordinate system (a gp_Ax22d object) as follows:
//! -   the origin of the coordinate system is the center of the circle, and
//! -   the orientation (direct or indirect) of the coordinate
//! system gives an implicit orientation to the circle (and
//! defines its trigonometric sense).
//! This positioning coordinate system is the "local
//! coordinate system" of the circle.
//! Note: when a gp_Circ2d circle is converted into a
//! Geom2d_Circle circle, some implicit properties of the
//! circle are used explicitly:
//! -   the implicit orientation corresponds to the direction in
//! which parameter values increase,
//! -   the starting point for parameterization is that of the "X
//! Axis" of the local coordinate system (i.e. the "X Axis" of the circle).
//! See Also
//! GccAna and Geom2dGcc packages which provide
//! functions for constructing circles defined by geometric constraints
//! gce_MakeCirc2d which provides functions for more
//! complex circle constructions
//! Geom2d_Circle which provides additional functions for
//! constructing circles and works, with the parametric
//! equations of circles in particular  gp_Ax22d
class gp_Circ2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! creates an indefinite circle.
  gp_Circ2d()
      : radius(RealLast())
  {
  }

  //! The location point of theXAxis is the center of the circle.
  //! Warnings :
  //! It is not forbidden to create a circle with theRadius = 0.0   Raises ConstructionError if
  //! theRadius < 0.0. Raised if theRadius < 0.0.
  gp_Circ2d(const gp_Ax2d&         theXAxis,
            const Standard_Real    theRadius,
            const Standard_Boolean theIsSense = Standard_True)
      : radius(theRadius)
  {
    Standard_ConstructionError_Raise_if(theRadius < 0.0,
                                        "gp_Circ2d() - radius should be positive number");
    pos = gp_Ax22d(theXAxis, theIsSense);
  }

  //! theAxis defines the Xaxis and Yaxis of the circle which defines
  //! the origin and the sense of parametrization.
  //! The location point of theAxis is the center of the circle.
  //! Warnings :
  //! It is not forbidden to create a circle with theRadius = 0.0 Raises ConstructionError if
  //! theRadius < 0.0. Raised if theRadius < 0.0.
  gp_Circ2d(const gp_Ax22d& theAxis, const Standard_Real theRadius)
      : pos(theAxis),
        radius(theRadius)
  {
    Standard_ConstructionError_Raise_if(theRadius < 0.0,
                                        "gp_Circ2d() - radius should be positive number");
  }

  //! Changes the location point (center) of the circle.
  void SetLocation(const gp_Pnt2d& theP) { pos.SetLocation(theP); }

  //! Changes the X axis of the circle.
  void SetXAxis(const gp_Ax2d& theA) { pos.SetXAxis(theA); }

  //! Changes the X axis of the circle.
  void SetAxis(const gp_Ax22d& theA) { pos.SetAxis(theA); }

  //! Changes the Y axis of the circle.
  void SetYAxis(const gp_Ax2d& theA) { pos.SetYAxis(theA); }

  //! Modifies the radius of this circle.
  //! This class does not prevent the creation of a circle where
  //! theRadius is null.
  //! Exceptions
  //! Standard_ConstructionError if theRadius is negative.
  void SetRadius(const Standard_Real theRadius)
  {
    Standard_ConstructionError_Raise_if(
      theRadius < 0.0,
      "gp_Circ2d::SetRadius() - radius should be positive number");
    radius = theRadius;
  }

  //! Computes the area of the circle.
  Standard_Real Area() const { return M_PI * radius * radius; }

  //! Returns the normalized coefficients from the implicit equation
  //! of the circle :
  //! theA * (X**2) + theB * (Y**2) + 2*theC*(X*Y) + 2*theD*X + 2*theE*Y + theF = 0.0
  void Coefficients(Standard_Real& theA,
                    Standard_Real& theB,
                    Standard_Real& theC,
                    Standard_Real& theD,
                    Standard_Real& theE,
                    Standard_Real& theF) const;

  //! Does <me> contain theP ?
  //! Returns True if the distance between theP and any point on
  //! the circumference of the circle is lower of equal to
  //! <theLinearTolerance>.
  Standard_Boolean Contains(const gp_Pnt2d& theP, const Standard_Real theLinearTolerance) const
  {
    return Distance(theP) <= theLinearTolerance;
  }

  //! Computes the minimum of distance between the point theP and any
  //! point on the circumference of the circle.
  Standard_Real Distance(const gp_Pnt2d& theP) const;

  //! Computes the square distance between <me> and the point theP.
  Standard_Real SquareDistance(const gp_Pnt2d& theP) const;

  //! computes the circumference of the circle.
  Standard_Real Length() const { return 2. * M_PI * radius; }

  //! Returns the location point (center) of the circle.
  const gp_Pnt2d& Location() const { return pos.Location(); }

  //! Returns the radius value of the circle.
  Standard_Real Radius() const { return radius; }

  //! returns the position of the circle.
  const gp_Ax22d& Axis() const { return pos; }

  //! returns the position of the circle. Idem Axis(me).
  const gp_Ax22d& Position() const { return pos; }

  //! returns the X axis of the circle.
  gp_Ax2d XAxis() const { return gp_Ax2d(pos.XAxis()); }

  //! Returns the Y axis of the circle.
  //! Reverses the direction of the circle.
  gp_Ax2d YAxis() const { return gp_Ax2d(pos.YAxis()); }

  //! Reverses the orientation of the local coordinate system
  //! of this circle (the "Y Direction" is reversed) and therefore
  //! changes the implicit orientation of this circle.
  //! Reverse assigns the result to this circle,
  void Reverse()
  {
    gp_Dir2d aTemp = pos.YDirection();
    aTemp.Reverse();
    pos.SetAxis(gp_Ax22d(pos.Location(), pos.XDirection(), aTemp));
  }

  //! Reverses the orientation of the local coordinate system
  //! of this circle (the "Y Direction" is reversed) and therefore
  //! changes the implicit orientation of this circle.
  //! Reversed creates a new circle.
  Standard_NODISCARD gp_Circ2d Reversed() const;

  //! Returns true if the local coordinate system is direct
  //! and false in the other case.
  Standard_Boolean IsDirect() const { return (pos.XDirection().Crossed(pos.YDirection())) >= 0.0; }

  Standard_EXPORT void Mirror(const gp_Pnt2d& theP);

  //! Performs the symmetrical transformation of a circle with respect
  //! to the point theP which is the center of the symmetry
  Standard_NODISCARD Standard_EXPORT gp_Circ2d Mirrored(const gp_Pnt2d& theP) const;

  Standard_EXPORT void Mirror(const gp_Ax2d& theA);

  //! Performs the symmetrical transformation of a circle with respect
  //! to an axis placement which is the axis of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Circ2d Mirrored(const gp_Ax2d& theA) const;

  void Rotate(const gp_Pnt2d& theP, const Standard_Real theAng) { pos.Rotate(theP, theAng); }

  //! Rotates a circle. theP is the center of the rotation.
  //! Ang is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Circ2d Rotated(const gp_Pnt2d& theP, const Standard_Real theAng) const
  {
    gp_Circ2d aCirc = *this;
    aCirc.pos.Rotate(theP, theAng);
    return aCirc;
  }

  void Scale(const gp_Pnt2d& theP, const Standard_Real theS);

  //! Scales a circle. theS is the scaling value.
  //! Warnings :
  //! If theS is negative the radius stay positive but
  //! the "XAxis" and the "YAxis" are  reversed as for
  //! an ellipse.
  Standard_NODISCARD gp_Circ2d Scaled(const gp_Pnt2d& theP, const Standard_Real theS) const;

  void Transform(const gp_Trsf2d& theT);

  //! Transforms a circle with the transformation theT from class Trsf2d.
  Standard_NODISCARD gp_Circ2d Transformed(const gp_Trsf2d& theT) const;

  void Translate(const gp_Vec2d& theV) { pos.Translate(theV); }

  //! Translates a circle in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Circ2d Translated(const gp_Vec2d& theV) const
  {
    gp_Circ2d aCirc = *this;
    aCirc.pos.Translate(theV);
    return aCirc;
  }

  void Translate(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) { pos.Translate(theP1, theP2); }

  //! Translates a circle from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Circ2d Translated(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) const
  {
    gp_Circ2d aCirc = *this;
    aCirc.pos.Translate(theP1, theP2);
    return aCirc;
  }

private:
  gp_Ax22d      pos;
  Standard_Real radius;
};

// =======================================================================
// function : Coefficients
// purpose  :
// =======================================================================
inline void gp_Circ2d::Coefficients(Standard_Real& theA,
                                    Standard_Real& theB,
                                    Standard_Real& theC,
                                    Standard_Real& theD,
                                    Standard_Real& theE,
                                    Standard_Real& theF) const
{
  Standard_Real aXc  = pos.Location().X();
  Standard_Real anYc = pos.Location().Y();
  theA               = 1.0;
  theB               = 1.0;
  theC               = 0.0;
  theD               = -aXc;
  theE               = -anYc;
  theF               = aXc * aXc + anYc * anYc - radius * radius;
}

// =======================================================================
// function : Distance
// purpose  :
// =======================================================================
inline Standard_Real gp_Circ2d::Distance(const gp_Pnt2d& theP) const
{
  gp_XY aCoord = theP.XY();
  aCoord.Subtract(pos.Location().XY());
  Standard_Real aD = radius - aCoord.Modulus();
  if (aD < 0)
  {
    aD = -aD;
  }
  return aD;
}

// =======================================================================
// function : Reversed
// purpose  :
// =======================================================================
inline gp_Circ2d gp_Circ2d::Reversed() const
{
  gp_Circ2d aCirc = *this;
  gp_Dir2d  aTemp = pos.YDirection();
  aTemp.Reverse();
  aCirc.pos.SetAxis(gp_Ax22d(pos.Location(), pos.XDirection(), aTemp));
  return aCirc;
}

// =======================================================================
// function : SquareDistance
// purpose  :
// =======================================================================
inline Standard_Real gp_Circ2d::SquareDistance(const gp_Pnt2d& theP) const
{
  gp_XY aCoord = theP.XY();
  aCoord.Subtract(pos.Location().XY());
  Standard_Real aD = radius - aCoord.Modulus();
  return aD * aD;
}

// =======================================================================
// function : Scale
// purpose  :
// =======================================================================
inline void gp_Circ2d::Scale(const gp_Pnt2d& theP, const Standard_Real theS)
{
  radius *= theS;
  if (radius < 0)
  {
    radius = -radius;
  }
  pos.Scale(theP, theS);
}

// =======================================================================
// function : Scaled
// purpose  :
// =======================================================================
inline gp_Circ2d gp_Circ2d::Scaled(const gp_Pnt2d& theP, const Standard_Real theS) const
{
  gp_Circ2d aCirc = *this;
  aCirc.radius *= theS;
  if (aCirc.radius < 0)
  {
    aCirc.radius = -aCirc.radius;
  }
  aCirc.pos.Scale(theP, theS);
  return aCirc;
}

// =======================================================================
// function : Transform
// purpose  :
// =======================================================================
inline void gp_Circ2d::Transform(const gp_Trsf2d& theT)
{
  radius *= theT.ScaleFactor();
  if (radius < 0)
  {
    radius = -radius;
  }
  pos.Transform(theT);
}

// =======================================================================
// function : Transformed
// purpose  :
// =======================================================================
inline gp_Circ2d gp_Circ2d::Transformed(const gp_Trsf2d& theT) const
{
  gp_Circ2d aCirc = *this;
  aCirc.radius *= theT.ScaleFactor();
  if (aCirc.radius < 0)
  {
    aCirc.radius = -aCirc.radius;
  }
  aCirc.pos.Transform(theT);
  return aCirc;
}

#endif // _gp_Circ2d_HeaderFile
