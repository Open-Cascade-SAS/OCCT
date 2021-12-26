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

#include <Quantity_Color.hxx>
#include <Aspect_GridDrawMode.hxx>
#include <Standard_Transient.hxx>

class Aspect_Grid : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Aspect_Grid, Standard_Transient)
public:

  //! defines the x Origin of the grid.
  Standard_EXPORT void SetXOrigin (const Standard_Real anOrigin);
  
  //! defines the y Origin of the grid.
  Standard_EXPORT void SetYOrigin (const Standard_Real anOrigin);
  
  //! defines the orientation of the grid.
  Standard_EXPORT void SetRotationAngle (const Standard_Real anAngle);
  
  //! Rotate the grid from a relative angle.
  Standard_EXPORT void Rotate (const Standard_Real anAngle);
  
  //! Translate the grid from a relative distance.
  Standard_EXPORT void Translate (const Standard_Real aDx, const Standard_Real aDy);
  
  //! Change the colors of the grid
  Standard_EXPORT virtual void SetColors (const Quantity_Color& aColor, const Quantity_Color& aTenthColor);
  
  //! returns the point of the grid the closest to the point X,Y
  //! if the grid is active. If the grid is not active returns
  //! X,Y.
  Standard_EXPORT void Hit (const Standard_Real X, const Standard_Real Y, Standard_Real& gridX, Standard_Real& gridY) const;
  
  //! returns the point of the grid the closest to the point X,Y
  Standard_EXPORT virtual void Compute (const Standard_Real X, const Standard_Real Y, Standard_Real& gridX, Standard_Real& gridY) const = 0;
  
  //! activates the grid. The Hit method will return
  //! gridx and gridx computed according to the steps
  //! of the grid.
  void Activate() { myIsActive = Standard_True; }
  
  //! deactivates the grid. The hit method will return
  //! gridx and gridx as the enter value X & Y.
  void Deactivate() { myIsActive = Standard_False; }
  
  //! returns the x Origin of the grid.
  Standard_Real XOrigin() const { return myXOrigin; }
  
  //! returns the x Origin of the grid.
  Standard_Real YOrigin() const { return myYOrigin; }
  
  //! returns the x Angle of the grid.
  Standard_Real RotationAngle() const { return myRotationAngle; }
  
  //! Returns TRUE when the grid is active.
  Standard_Boolean IsActive() const { return myIsActive; }
  
  //! Returns the colors of the grid.
  Standard_EXPORT void Colors (Quantity_Color& aColor, Quantity_Color& aTenthColor) const;
  
  //! Change the grid aspect.
  Standard_EXPORT void SetDrawMode (const Aspect_GridDrawMode aDrawMode);
  
  //! Returns the grid aspect.
  Aspect_GridDrawMode DrawMode() const { return myDrawMode; }

  //! Display the grid at screen.
  Standard_EXPORT virtual void Display() = 0;
  
  //! Erase the grid from screen.
  Standard_EXPORT virtual void Erase() const = 0;
  
  //! Returns TRUE when the grid is displayed at screen.
  Standard_EXPORT virtual Standard_Boolean IsDisplayed() const = 0;
  
  Standard_EXPORT virtual void Init() = 0;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

protected:

  //! Creates a new grid. By default this grid is not active.
  Standard_EXPORT Aspect_Grid (const Standard_Real theXOrigin = 0.0,
                               const Standard_Real theYOrigin = 0.0,
                               const Standard_Real theRotationAngle = 0,
                               const Quantity_Color& theColor = Quantity_NOC_GRAY50,
                               const Quantity_Color& theTenthColor = Quantity_NOC_GRAY70);
  
  //! Updates the grid parameters.
  Standard_EXPORT virtual void UpdateDisplay() = 0;

protected:

  Standard_Real myRotationAngle;
  Standard_Real myXOrigin;
  Standard_Real myYOrigin;
  Quantity_Color myColor;
  Quantity_Color myTenthColor;
  Standard_Boolean myIsActive;
  Aspect_GridDrawMode myDrawMode;

};

DEFINE_STANDARD_HANDLE(Aspect_Grid, Standard_Transient)

#endif // _Aspect_Grid_HeaderFile
