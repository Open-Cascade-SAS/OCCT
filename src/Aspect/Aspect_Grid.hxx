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

#ifndef _Aspect_Grid_HeaderFile
#define _Aspect_Grid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Quantity_PlaneAngle.hxx>
#include <Quantity_Length.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Boolean.hxx>
#include <Aspect_GridDrawMode.hxx>
#include <MMgt_TShared.hxx>
class Quantity_Color;


class Aspect_Grid;
DEFINE_STANDARD_HANDLE(Aspect_Grid, MMgt_TShared)


class Aspect_Grid : public MMgt_TShared
{

public:

  
  //! defines the x Origin of the grid.
  Standard_EXPORT void SetXOrigin (const Quantity_Length anOrigin);
  
  //! defines the y Origin of the grid.
  Standard_EXPORT void SetYOrigin (const Quantity_Length anOrigin);
  
  //! defines the orientation of the the grid.
  Standard_EXPORT void SetRotationAngle (const Quantity_PlaneAngle anAngle);
  
  //! Rotate the grid from a relative angle.
  Standard_EXPORT void Rotate (const Quantity_PlaneAngle anAngle);
  
  //! Translate the grid from a relative distance.
  Standard_EXPORT void Translate (const Quantity_Length aDx, const Quantity_Length aDy);
  
  //! Change the colors of the grid
  Standard_EXPORT virtual void SetColors (const Quantity_Color& aColor, const Quantity_Color& aTenthColor);
  
  //! returns the point of the grid the closest to the point X,Y
  //! if the grid is active. If the grid is not active returns
  //! X,Y.
  Standard_EXPORT void Hit (const Quantity_Length X, const Quantity_Length Y, Quantity_Length& gridX, Quantity_Length& gridY) const;
  
  //! returns the point of the grid the closest to the point X,Y
  Standard_EXPORT virtual void Compute (const Quantity_Length X, const Quantity_Length Y, Quantity_Length& gridX, Quantity_Length& gridY) const = 0;
  
  //! activates the grid. The Hit method will return
  //! gridx and gridx computed according to the steps
  //! of the grid.
  Standard_EXPORT void Activate();
  
  //! deactivates the grid. The hit method will return
  //! gridx and gridx as the enter value X & Y.
  Standard_EXPORT void Deactivate();
  
  //! returns the x Origin of the grid.
  Standard_EXPORT Quantity_Length XOrigin() const;
  
  //! returns the x Origin of the grid.
  Standard_EXPORT Quantity_Length YOrigin() const;
  
  //! returns the x Angle of the grid.
  Standard_EXPORT Quantity_PlaneAngle RotationAngle() const;
  
  //! Returns TRUE when the grid is active.
  Standard_EXPORT Standard_Boolean IsActive() const;
  
  //! Returns the colors of the grid.
  Standard_EXPORT void Colors (Quantity_Color& aColor, Quantity_Color& aTenthColor) const;
  
  //! Change the grid aspect.
  Standard_EXPORT void SetDrawMode (const Aspect_GridDrawMode aDrawMode);
  
  //! Returns the grid aspect.
  Standard_EXPORT Aspect_GridDrawMode DrawMode() const;
  
  //! Display the grid at screen.
  Standard_EXPORT virtual void Display();
  
  //! Erase the grid from screen.
  Standard_EXPORT virtual void Erase() const;
  
  //! Returns TRUE when the grid is displayed at screen.
  Standard_EXPORT virtual Standard_Boolean IsDisplayed() const;
  
  Standard_EXPORT virtual void Init() = 0;




  DEFINE_STANDARD_RTTI(Aspect_Grid,MMgt_TShared)

protected:

  
  //! creates a new grid. By default this grid is not
  //! active.
  Standard_EXPORT Aspect_Grid(const Quantity_Length anXOrigin = 0.0, const Quantity_Length anYOrigin = 0.0, const Quantity_PlaneAngle aRotationAngle = 0, const Quantity_Color& aColor = Quantity_NOC_GRAY50, const Quantity_Color& aTenthColor = Quantity_NOC_GRAY70);
  
  //! Updates the grid parameters.
  Standard_EXPORT virtual void UpdateDisplay();

  Quantity_PlaneAngle myRotationAngle;
  Quantity_Length myXOrigin;
  Quantity_Length myYOrigin;
  Quantity_Color myColor;
  Quantity_Color myTenthColor;


private:


  Standard_Boolean myIsActive;
  Aspect_GridDrawMode myDrawMode;


};







#endif // _Aspect_Grid_HeaderFile
