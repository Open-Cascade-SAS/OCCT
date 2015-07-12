// Created on: 1998-07-16
// Created by: CAL
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _V3d_RectangularGrid_HeaderFile
#define _V3d_RectangularGrid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Ax3.hxx>
#include <V3d_ViewerPointer.hxx>
#include <Standard_Boolean.hxx>
#include <Aspect_GridDrawMode.hxx>
#include <Standard_Real.hxx>
#include <Aspect_RectangularGrid.hxx>
class Graphic3d_Structure;
class Graphic3d_Group;
class Quantity_Color;


class V3d_RectangularGrid;
DEFINE_STANDARD_HANDLE(V3d_RectangularGrid, Aspect_RectangularGrid)


class V3d_RectangularGrid : public Aspect_RectangularGrid
{

public:

  
  Standard_EXPORT V3d_RectangularGrid(const V3d_ViewerPointer& aViewer, const Quantity_Color& aColor, const Quantity_Color& aTenthColor);
  
  Standard_EXPORT void SetColors (const Quantity_Color& aColor, const Quantity_Color& aTenthColor) Standard_OVERRIDE;
  
  Standard_EXPORT void Display() Standard_OVERRIDE;
  
  Standard_EXPORT void Erase() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsDisplayed() const Standard_OVERRIDE;
  
  Standard_EXPORT void GraphicValues (Standard_Real& XSize, Standard_Real& YSize, Standard_Real& OffSet) const;
  
  Standard_EXPORT void SetGraphicValues (const Standard_Real XSize, const Standard_Real YSize, const Standard_Real OffSet);




  DEFINE_STANDARD_RTTI(V3d_RectangularGrid,Aspect_RectangularGrid)

protected:

  
  Standard_EXPORT void UpdateDisplay() Standard_OVERRIDE;



private:

  
  Standard_EXPORT void DefineLines();
  
  Standard_EXPORT void DefinePoints();

  Handle(Graphic3d_Structure) myStructure;
  Handle(Graphic3d_Group) myGroup;
  gp_Ax3 myCurViewPlane;
  V3d_ViewerPointer myViewer;
  Standard_Boolean myCurAreDefined;
  Aspect_GridDrawMode myCurDrawMode;
  Standard_Real myCurXo;
  Standard_Real myCurYo;
  Standard_Real myCurAngle;
  Standard_Real myCurXStep;
  Standard_Real myCurYStep;
  Standard_Real myXSize;
  Standard_Real myYSize;
  Standard_Real myOffSet;


};







#endif // _V3d_RectangularGrid_HeaderFile
