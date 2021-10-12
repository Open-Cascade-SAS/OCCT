// Created on: 1995-03-02
// Created by: Jean-Louis Frenkel
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Aspect_RectangularGrid_HeaderFile
#define _Aspect_RectangularGrid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Aspect_Grid.hxx>
#include <Standard_Boolean.hxx>

class Aspect_RectangularGrid : public Aspect_Grid
{
  DEFINE_STANDARD_RTTIEXT(Aspect_RectangularGrid, Aspect_Grid)
public:

  //! creates a new grid. By default this grid is not
  //! active.
  //! The first angle is given relatively to the horizontal.
  //! The second angle is given relatively to the vertical.
  Standard_EXPORT Aspect_RectangularGrid(const Standard_Real aXStep, const Standard_Real aYStep, const Standard_Real anXOrigin = 0, const Standard_Real anYOrigin = 0, const Standard_Real aFirstAngle = 0, const Standard_Real aSecondAngle = 0, const Standard_Real aRotationAngle = 0);
  
  //! defines the x step of the grid.
  Standard_EXPORT void SetXStep (const Standard_Real aStep);
  
  //! defines the y step of the grid.
  Standard_EXPORT void SetYStep (const Standard_Real aStep);
  
  //! defines the angle of the second network
  //! the fist angle is given relatively to the horizontal.
  //! the second angle is given relatively to the vertical.
  Standard_EXPORT void SetAngle (const Standard_Real anAngle1, const Standard_Real anAngle2);
  
  Standard_EXPORT void SetGridValues (const Standard_Real XOrigin, const Standard_Real YOrigin, const Standard_Real XStep, const Standard_Real YStep, const Standard_Real RotationAngle);
  
  //! returns the point of the grid the closest to the point X,Y
  Standard_EXPORT virtual void Compute (const Standard_Real X, const Standard_Real Y, Standard_Real& gridX, Standard_Real& gridY) const Standard_OVERRIDE;
  
  //! returns the x step of the grid.
  Standard_EXPORT Standard_Real XStep() const;
  
  //! returns the x step of the grid.
  Standard_EXPORT Standard_Real YStep() const;
  
  //! returns the x Angle of the grid, relatively to the horizontal.
  Standard_EXPORT Standard_Real FirstAngle() const;
  
  //! returns the y Angle of the grid, relatively to the vertical.
  Standard_EXPORT Standard_Real SecondAngle() const;
  
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

private:

  Standard_EXPORT Standard_Boolean CheckAngle (const Standard_Real alpha, const Standard_Real beta) const;

private:

  Standard_Real myXStep;
  Standard_Real myYStep;
  Standard_Real myFirstAngle;
  Standard_Real mySecondAngle;
  Standard_Real a1;
  Standard_Real b1;
  Standard_Real c1;
  Standard_Real a2;
  Standard_Real b2;
  Standard_Real c2;

};

DEFINE_STANDARD_HANDLE(Aspect_RectangularGrid, Aspect_Grid)

#endif // _Aspect_RectangularGrid_HeaderFile
