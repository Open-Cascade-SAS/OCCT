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

#include <Quantity_Length.hxx>
#include <Quantity_PlaneAngle.hxx>
#include <Standard_Real.hxx>
#include <Aspect_Grid.hxx>
#include <Standard_Boolean.hxx>
class Standard_NegativeValue;
class Standard_NullValue;
class Standard_NumericError;


class Aspect_RectangularGrid;
DEFINE_STANDARD_HANDLE(Aspect_RectangularGrid, Aspect_Grid)


class Aspect_RectangularGrid : public Aspect_Grid
{

public:

  
  //! creates a new grid. By default this grid is not
  //! active.
  //! The first angle is given relatively to the horizontal.
  //! The second angle is given relatively to the vertical.
  Standard_EXPORT Aspect_RectangularGrid(const Quantity_Length aXStep, const Quantity_Length aYStep, const Quantity_Length anXOrigin = 0, const Quantity_Length anYOrigin = 0, const Quantity_PlaneAngle aFirstAngle = 0, const Quantity_PlaneAngle aSecondAngle = 0, const Quantity_PlaneAngle aRotationAngle = 0);
  
  //! defines the x step of the grid.
  Standard_EXPORT void SetXStep (const Quantity_Length aStep);
  
  //! defines the y step of the grid.
  Standard_EXPORT void SetYStep (const Quantity_Length aStep);
  
  //! defines the angle of the second network
  //! the fist angle is given relatively to the horizontal.
  //! the second angle is given relatively to the vertical.
  Standard_EXPORT void SetAngle (const Quantity_PlaneAngle anAngle1, const Quantity_PlaneAngle anAngle2);
  
  Standard_EXPORT void SetGridValues (const Quantity_Length XOrigin, const Quantity_Length YOrigin, const Quantity_Length XStep, const Quantity_Length YStep, const Quantity_PlaneAngle RotationAngle);
  
  //! returns the point of the grid the closest to the point X,Y
  Standard_EXPORT void Compute (const Quantity_Length X, const Quantity_Length Y, Quantity_Length& gridX, Quantity_Length& gridY) const;
  
  //! returns the x step of the grid.
  Standard_EXPORT Quantity_Length XStep() const;
  
  //! returns the x step of the grid.
  Standard_EXPORT Quantity_Length YStep() const;
  
  //! returns the x Angle of the grid, relatively to the horizontal.
  Standard_EXPORT Quantity_PlaneAngle FirstAngle() const;
  
  //! returns the y Angle of the grid, relatively to the vertical.
  Standard_EXPORT Quantity_PlaneAngle SecondAngle() const;
  
  Standard_EXPORT void Init() Standard_OVERRIDE;




  DEFINE_STANDARD_RTTI(Aspect_RectangularGrid,Aspect_Grid)

protected:




private:

  
  Standard_EXPORT Standard_Boolean CheckAngle (const Quantity_PlaneAngle alpha, const Quantity_PlaneAngle beta) const;

  Quantity_Length myXStep;
  Quantity_Length myYStep;
  Quantity_PlaneAngle myFirstAngle;
  Quantity_PlaneAngle mySecondAngle;
  Standard_Real a1;
  Standard_Real b1;
  Standard_Real c1;
  Standard_Real a2;
  Standard_Real b2;
  Standard_Real c2;


};







#endif // _Aspect_RectangularGrid_HeaderFile
