// Created on: 1993-07-30
// Created by: Jean-Louis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Prs3d_DatumAspect_HeaderFile
#define _Prs3d_DatumAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_Boolean.hxx>
#include <Quantity_Length.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Standard_Real.hxx>

//! A framework to define the display of datums.
class Prs3d_DatumAspect : public Prs3d_BasicAspect
{
  DEFINE_STANDARD_RTTIEXT(Prs3d_DatumAspect, Prs3d_BasicAspect)
public:

  //! An empty framework to define the display of datums.
  Standard_EXPORT Prs3d_DatumAspect();
  
  //! Returns the attributes for display of the first axis.
  const Handle(Prs3d_LineAspect)& FirstAxisAspect() const { return myFirstAxisAspect; }
  
  //! Returns the attributes for display of the second axis.
  const Handle(Prs3d_LineAspect)& SecondAxisAspect() const { return mySecondAxisAspect; }
  
  //! Returns the attributes for display of the third axis.
  const Handle(Prs3d_LineAspect)& ThirdAxisAspect() const { return myThirdAxisAspect; }
  
  //! Sets the DrawFirstAndSecondAxis attributes to active.
  void SetDrawFirstAndSecondAxis (const Standard_Boolean theToDraw) { myDrawFirstAndSecondAxis = theToDraw; }
  
  //! Returns true if the first and second axes can be drawn.
  Standard_Boolean DrawFirstAndSecondAxis() const { return myDrawFirstAndSecondAxis; }

  //! Sets the DrawThirdAxis attributes to active.
  void SetDrawThirdAxis (const Standard_Boolean theToDraw) { myDrawThirdAxis = theToDraw; }
  
  //! Returns true if the third axis can be drawn.
  Standard_Boolean DrawThirdAxis() const { return myDrawThirdAxis; }

  //! Sets the lengths of the three axes.
  void SetAxisLength (const Standard_Real theL1, const Standard_Real theL2, const Standard_Real theL3)
  {
    myFirstAxisLength  = theL1;
    mySecondAxisLength = theL2;
    myThirdAxisLength  = theL3;
  }

  //! Returns the length of the displayed first axis.
  Quantity_Length FirstAxisLength() const { return myFirstAxisLength; }

  //! Returns the length of the displayed second axis.
  Quantity_Length SecondAxisLength() const { return mySecondAxisLength; }
  
  //! Returns the length of the displayed third axis.
  Quantity_Length ThirdAxisLength() const { return myThirdAxisLength; }

  //! Sets option to draw or not to draw text labels for axes
  void SetToDrawLabels (const Standard_Boolean theToDraw) { myToDrawLabels = theToDraw; }

  //! @return true if axes labels are drawn
  Standard_Boolean ToDrawLabels() const { return myToDrawLabels; }

private:

  Handle(Prs3d_LineAspect) myFirstAxisAspect;
  Handle(Prs3d_LineAspect) mySecondAxisAspect;
  Handle(Prs3d_LineAspect) myThirdAxisAspect;
  Standard_Boolean myDrawFirstAndSecondAxis;
  Standard_Boolean myDrawThirdAxis;
  Standard_Boolean myToDrawLabels;
  Quantity_Length myFirstAxisLength;
  Quantity_Length mySecondAxisLength;
  Quantity_Length myThirdAxisLength;

};

DEFINE_STANDARD_HANDLE(Prs3d_DatumAspect, Prs3d_BasicAspect)

#endif // _Prs3d_DatumAspect_HeaderFile
