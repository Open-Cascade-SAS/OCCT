// Created on: 1994-02-03
// Created by: Jean Marc LACHAUME
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Draw_Grid_HeaderFile
#define _Draw_Grid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Draw_Drawable3D.hxx>
class Draw_Display;


class Draw_Grid;
DEFINE_STANDARD_HANDLE(Draw_Grid, Draw_Drawable3D)


class Draw_Grid : public Draw_Drawable3D
{

public:

  
  //! Creates a grid.
  Standard_EXPORT Draw_Grid();
  
  //! Sets the steps along the X, Y & Z axis.
  Standard_EXPORT void Steps (const Standard_Real StepX, const Standard_Real StepY, const Standard_Real StepZ);
  
  //! Returns the step along the X axis.
    Standard_Real StepX() const;
  
  //! Returns the step along the Y axis.
    Standard_Real StepY() const;
  
  //! Returns the step along the Z axis.
    Standard_Real StepZ() const;
  
  //! Returns if the grid is active or not.
    Standard_Boolean IsActive() const;
  
  //! Displays the grid.
  Standard_EXPORT void DrawOn (Draw_Display& Out) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Draw_Grid,Draw_Drawable3D)

protected:




private:


  Standard_Real myStepX;
  Standard_Real myStepY;
  Standard_Real myStepZ;
  Standard_Boolean myIsActive;


};


#include <Draw_Grid.lxx>





#endif // _Draw_Grid_HeaderFile
