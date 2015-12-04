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
#include <Standard_Real.hxx>
class Prs3d_LineAspect;


class Prs3d_DatumAspect;
DEFINE_STANDARD_HANDLE(Prs3d_DatumAspect, Prs3d_BasicAspect)

//! A framework to define the display of datums.
class Prs3d_DatumAspect : public Prs3d_BasicAspect
{

public:

  //! An empty framework to define the display of datums.
  Standard_EXPORT Prs3d_DatumAspect();
  
  //! Returns the attributes for display of the first axis.
  Standard_EXPORT Handle(Prs3d_LineAspect) FirstAxisAspect() const;
  
  //! Returns the attributes for display of the second axis.
  Standard_EXPORT Handle(Prs3d_LineAspect) SecondAxisAspect() const;
  
  //! Returns the attributes for display of the third axis.
  Standard_EXPORT Handle(Prs3d_LineAspect) ThirdAxisAspect() const;
  
  //! Sets the DrawFirstAndSecondAxis attributes to active.
  Standard_EXPORT void SetDrawFirstAndSecondAxis (const Standard_Boolean draw);
  
  //! Returns true if the first and second axes can be drawn.
  Standard_EXPORT Standard_Boolean DrawFirstAndSecondAxis() const;
  
  //! Sets the DrawThirdAxis attributes to active.
  Standard_EXPORT void SetDrawThirdAxis (const Standard_Boolean draw);
  
  //! Returns true if the third axis can be drawn.
  Standard_EXPORT Standard_Boolean DrawThirdAxis() const;
  
  //! Sets the lengths L1, L2 and L3 of the three axes.
  Standard_EXPORT void SetAxisLength (const Standard_Real L1, const Standard_Real L2, const Standard_Real L3);
  
  //! Returns the length of the displayed first axis.
  Standard_EXPORT Quantity_Length FirstAxisLength() const;
  
  //! Returns the length of the displayed second axis.
  Standard_EXPORT Quantity_Length SecondAxisLength() const;
  
  //! Returns the length of the displayed third axis.
  Standard_EXPORT Quantity_Length ThirdAxisLength() const;

  //! Sets option to draw or not to draw text labels for axes
  Standard_EXPORT void SetToDrawLabels (const Standard_Boolean theToDraw);

  //! @return true if axes labels are drawn
  Standard_EXPORT Standard_Boolean ToDrawLabels() const;

  DEFINE_STANDARD_RTTIEXT(Prs3d_DatumAspect,Prs3d_BasicAspect)

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







#endif // _Prs3d_DatumAspect_HeaderFile
