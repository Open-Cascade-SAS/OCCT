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

#include <Aspect_Grid.hxx>

class Aspect_RectangularGrid : public Aspect_Grid
{
  DEFINE_STANDARD_RTTIEXT(Aspect_RectangularGrid, Aspect_Grid)
public:
  //! creates a new grid. By default this grid is not
  //! active.
  //! The first angle is given relatively to the horizontal.
  //! The second angle is given relatively to the vertical.
  Standard_EXPORT Aspect_RectangularGrid(const double aXStep,
                                         const double aYStep,
                                         const double anXOrigin      = 0,
                                         const double anYOrigin      = 0,
                                         const double aFirstAngle    = 0,
                                         const double aSecondAngle   = 0,
                                         const double aRotationAngle = 0);

  //! defines the x step of the grid.
  Standard_EXPORT void SetXStep(const double aStep);

  //! defines the y step of the grid.
  Standard_EXPORT void SetYStep(const double aStep);

  //! defines the angle of the second network
  //! the fist angle is given relatively to the horizontal.
  //! the second angle is given relatively to the vertical.
  Standard_EXPORT void SetAngle(const double anAngle1, const double anAngle2);

  Standard_EXPORT void SetGridValues(const double XOrigin,
                                     const double YOrigin,
                                     const double XStep,
                                     const double YStep,
                                     const double RotationAngle);

  //! returns the point of the grid the closest to the point X,Y
  Standard_EXPORT void Compute(const double X,
                                       const double Y,
                                       double&      gridX,
                                       double&      gridY) const override;

  //! returns the x step of the grid.
  Standard_EXPORT double XStep() const;

  //! returns the x step of the grid.
  Standard_EXPORT double YStep() const;

  //! returns the x Angle of the grid, relatively to the horizontal.
  Standard_EXPORT double FirstAngle() const;

  //! returns the y Angle of the grid, relatively to the vertical.
  Standard_EXPORT double SecondAngle() const;

  Standard_EXPORT void Init() override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

private:
  Standard_EXPORT bool CheckAngle(const double alpha, const double beta) const;

private:
  double myXStep;
  double myYStep;
  double myFirstAngle;
  double mySecondAngle;
  double a1;
  double b1;
  double c1;
  double a2;
  double b2;
  double c2;
};

#endif // _Aspect_RectangularGrid_HeaderFile
