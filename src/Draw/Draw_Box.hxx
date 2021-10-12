// Created on: 1995-03-10
// Created by: Remi LEQUETTE
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

#ifndef _Draw_Box_HeaderFile
#define _Draw_Box_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Bnd_OBB.hxx>
#include <Draw_Color.hxx>
#include <Draw_Drawable3D.hxx>
class Draw_Display;


class Draw_Box;
DEFINE_STANDARD_HANDLE(Draw_Box, Draw_Drawable3D)

//! a 3d box
class Draw_Box : public Draw_Drawable3D
{
public:
  
  //! Constructor
  Standard_EXPORT Draw_Box(const Bnd_OBB& theOBB,
                           const Draw_Color& theColor);

  //! Draws myOBB
  Standard_EXPORT void DrawOn (Draw_Display& theDis) const Standard_OVERRIDE;
  
  DEFINE_STANDARD_RTTIEXT(Draw_Box,Draw_Drawable3D)

protected:

  //! Converts the point (theX, theY, theZ) in local coordinate system to WCS.
  void ToWCS(const Standard_Real theX, 
             const Standard_Real theY,
             const Standard_Real theZ,
             gp_Pnt& theP) const;

  //! Moves the point thePt along X-direction of myOBB on the distance theShift.
  void MoveX(const Standard_Real theShift, gp_Pnt& thePt) const;

  //! Moves the point thePt along Y-direction of myOBB on the distance theShift.
  void MoveY(const Standard_Real theShift, gp_Pnt& thePt) const;

  //! Moves the point thePt along Z-direction of myOBB on the distance theShift.
  void MoveZ(const Standard_Real theShift, gp_Pnt& thePt) const;

private:

  //! Oriented bounding box
  Bnd_OBB myOBB;

  //! Color value
  Draw_Color myColor;
};

#endif // _Draw_Box_HeaderFile
