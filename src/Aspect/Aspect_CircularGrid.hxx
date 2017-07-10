// Created on: 1995-03-17
// Created by: Mister rmi
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

#ifndef _Aspect_CircularGrid_HeaderFile
#define _Aspect_CircularGrid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Aspect_Grid.hxx>

class Standard_NegativeValue;
class Standard_NullValue;
class Standard_NumericError;

class Aspect_CircularGrid : public Aspect_Grid
{
  DEFINE_STANDARD_RTTIEXT(Aspect_CircularGrid, Aspect_Grid)
public:

  //! creates a new grid. By default this grid is not
  //! active.
  Standard_EXPORT Aspect_CircularGrid(const Standard_Real aRadiusStep, const Standard_Integer aDivisionNumber, const Standard_Real XOrigin = 0, const Standard_Real anYOrigin = 0, const Standard_Real aRotationAngle = 0);
  
  //! defines the x step of the grid.
  Standard_EXPORT void SetRadiusStep (const Standard_Real aStep);
  
  //! defines the step of the grid.
  Standard_EXPORT void SetDivisionNumber (const Standard_Integer aNumber);
  
  Standard_EXPORT void SetGridValues (const Standard_Real XOrigin, const Standard_Real YOrigin, const Standard_Real RadiusStep, const Standard_Integer DivisionNumber, const Standard_Real RotationAngle);
  
  //! returns the point of the grid the closest to the point X,Y
  Standard_EXPORT virtual void Compute (const Standard_Real X, const Standard_Real Y, Standard_Real& gridX, Standard_Real& gridY) const Standard_OVERRIDE;
  
  //! returns the x step of the grid.
  Standard_EXPORT Standard_Real RadiusStep() const;
  
  //! returns the x step of the grid.
  Standard_EXPORT Standard_Integer DivisionNumber() const;
  
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

private:

  Standard_Real myRadiusStep;
  Standard_Integer myDivisionNumber;
  Standard_Real myAlpha;
  Standard_Real myA1;
  Standard_Real myB1;

};

DEFINE_STANDARD_HANDLE(Aspect_CircularGrid, Aspect_Grid)

#endif // _Aspect_CircularGrid_HeaderFile
