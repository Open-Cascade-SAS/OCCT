// Created on: 1999-05-06
// Created by: data exchange team
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

#ifndef _ShapeUpgrade_SplitSurfaceAngle_HeaderFile
#define _ShapeUpgrade_SplitSurfaceAngle_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <ShapeUpgrade_SplitSurface.hxx>

class ShapeUpgrade_SplitSurfaceAngle;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_SplitSurfaceAngle, ShapeUpgrade_SplitSurface)

//! Splits a surfaces of revolution, cylindrical, toroidal,
//! conical, spherical so that each resulting segment covers
//! not more than defined number of degrees.
class ShapeUpgrade_SplitSurfaceAngle : public ShapeUpgrade_SplitSurface
{

public:
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_SplitSurfaceAngle(const Standard_Real MaxAngle);

  //! Set maximal angle
  Standard_EXPORT void SetMaxAngle(const Standard_Real MaxAngle);

  //! Returns maximal angle
  Standard_EXPORT Standard_Real MaxAngle() const;

  //! Performs splitting of the supporting surface(s).
  //! First defines splitting values, then calls inherited method.
  Standard_EXPORT virtual void Compute(const Standard_Boolean Segment) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_SplitSurfaceAngle, ShapeUpgrade_SplitSurface)

protected:
private:
  Standard_Real myMaxAngle;
};

#endif // _ShapeUpgrade_SplitSurfaceAngle_HeaderFile
