// Created on: 1999-03-05
// Created by: Fabrice SERVANT
// Copyright (c) 1999 Matra Datavision
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

#ifndef _IntPolyh_Point_HeaderFile
#define _IntPolyh_Point_HeaderFile

#include <Adaptor3d_Surface.hxx>

//! The class represents the point on the surface with
//! both 3D and 2D points.
class IntPolyh_Point
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor
  IntPolyh_Point()
      : myX(0.),
        myY(0.),
        myZ(0.),
        myU(0.),
        myV(0.),
        myPOC(1),
        myDegenerated(Standard_False)
  {
  }

  //! Constructor
  IntPolyh_Point(const Standard_Real x,
                 const Standard_Real y,
                 const Standard_Real z,
                 const Standard_Real u,
                 const Standard_Real v)
      : myX(x),
        myY(y),
        myZ(z),
        myU(u),
        myV(v),
        myPOC(1),
        myDegenerated(Standard_False)
  {
  }

  //! Returns X coordinate of the 3D point
  Standard_Real X() const { return myX; }

  //! Returns Y coordinate of the 3D point
  Standard_Real Y() const { return myY; }

  //! Returns the Z coordinate of the 3D point
  Standard_Real Z() const { return myZ; }

  //! Returns the U coordinate of the 2D point
  Standard_Real U() const { return myU; }

  //! Returns the V coordinate of the 2D point
  Standard_Real V() const { return myV; }

  //! Returns 0 if the point is not common with the other surface
  Standard_Integer PartOfCommon() const { return myPOC; }

  //! Sets the point
  void Set(const Standard_Real    x,
           const Standard_Real    y,
           const Standard_Real    z,
           const Standard_Real    u,
           const Standard_Real    v,
           const Standard_Integer II = 1)
  {
    myX   = x;
    myY   = y;
    myZ   = z;
    myU   = u;
    myV   = v;
    myPOC = II;
  }

  //! Sets the X coordinate for the 3D point
  void SetX(const Standard_Real x) { myX = x; }

  //! Sets the Y coordinate for the 3D point
  void SetY(const Standard_Real y) { myY = y; }

  //! Sets the Z coordinate for the 3D point
  void SetZ(const Standard_Real z) { myZ = z; }

  //! Sets the U coordinate for the 2D point
  void SetU(const Standard_Real u) { myU = u; }

  //! Sets the V coordinate for the 2D point
  void SetV(const Standard_Real v) { myV = v; }

  //! Sets the part of common
  void SetPartOfCommon(const Standard_Integer ii) { myPOC = ii; }

  //! Creates middle point from P1 and P2 and stores it to this
  Standard_EXPORT void Middle(const Handle(Adaptor3d_Surface)& MySurface,
                              const IntPolyh_Point&            P1,
                              const IntPolyh_Point&            P2);
  //! Addition
  Standard_EXPORT IntPolyh_Point Add(const IntPolyh_Point& P1) const;

  IntPolyh_Point operator+(const IntPolyh_Point& P1) const { return Add(P1); }

  //! Subtraction
  Standard_EXPORT IntPolyh_Point Sub(const IntPolyh_Point& P1) const;

  IntPolyh_Point operator-(const IntPolyh_Point& P1) const { return Sub(P1); }

  //! Division
  Standard_EXPORT IntPolyh_Point Divide(const Standard_Real rr) const;

  IntPolyh_Point operator/(const Standard_Real rr) const { return Divide(rr); }

  //! Multiplication
  Standard_EXPORT IntPolyh_Point Multiplication(const Standard_Real rr) const;

  IntPolyh_Point operator*(const Standard_Real rr) const { return Multiplication(rr); }

  //! Square modulus
  Standard_EXPORT Standard_Real SquareModulus() const;
  //! Square distance to the other point
  Standard_EXPORT Standard_Real SquareDistance(const IntPolyh_Point& P2) const;
  //! Dot
  Standard_EXPORT Standard_Real Dot(const IntPolyh_Point& P2) const;
  //! Cross
  Standard_EXPORT void Cross(const IntPolyh_Point& P1, const IntPolyh_Point& P2);
  //! Dump
  Standard_EXPORT void Dump() const;
  //! Dump
  Standard_EXPORT void Dump(const Standard_Integer i) const;

  //! Sets the degenerated flag
  void SetDegenerated(const Standard_Boolean theFlag) { myDegenerated = theFlag; }

  //! Returns the degenerated flag
  Standard_Boolean Degenerated() const { return myDegenerated; }

protected:
private:
  Standard_Real    myX;
  Standard_Real    myY;
  Standard_Real    myZ;
  Standard_Real    myU;
  Standard_Real    myV;
  Standard_Integer myPOC;
  Standard_Boolean myDegenerated;
};

#endif // _IntPolyh_Point_HeaderFile
