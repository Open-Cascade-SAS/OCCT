// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BRepMesh_Circle_HeaderFile
#define _BRepMesh_Circle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <gp_XY.hxx>

//! Describes a 2d circle with a size of only 3 Standard_Real
//! numbers instead of gp who needs 7 Standard_Real numbers.
class BRepMesh_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  BRepMesh_Circle()
      : myRadius(0.0)
  {
  }

  //! Constructor.
  //! @param theLocation location of a circle.
  //! @param theRadius radius of a circle.
  BRepMesh_Circle(const gp_XY& theLocation, const Standard_Real theRadius)
      : myLocation(theLocation),
        myRadius(theRadius)
  {
  }

  //! Sets location of a circle.
  //! @param theLocation location of a circle.
  void SetLocation(const gp_XY& theLocation) { myLocation = theLocation; }

  //! Sets radius of a circle.
  //! @param theRadius radius of a circle.
  void SetRadius(const Standard_Real theRadius) { myRadius = theRadius; }

  //! Returns location of a circle.
  const gp_XY& Location() const { return myLocation; }

  //! Returns radius of a circle.
  const Standard_Real& Radius() const { return myRadius; }

private:
  gp_XY         myLocation;
  Standard_Real myRadius;
};

#endif
