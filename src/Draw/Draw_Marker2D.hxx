// Created on: 1994-04-18
// Created by: Modelistation
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

#ifndef _Draw_Marker2D_HeaderFile
#define _Draw_Marker2D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt2d.hxx>
#include <Draw_Color.hxx>
#include <Draw_MarkerShape.hxx>
#include <Standard_Integer.hxx>
#include <Draw_Drawable2D.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Draw_Display;


class Draw_Marker2D;
DEFINE_STANDARD_HANDLE(Draw_Marker2D, Draw_Drawable2D)


class Draw_Marker2D : public Draw_Drawable2D
{

public:

  
  Standard_EXPORT Draw_Marker2D(const gp_Pnt2d& P, const Draw_MarkerShape T, const Draw_Color& C, const Standard_Integer Size = 5);
  
  Standard_EXPORT Draw_Marker2D(const gp_Pnt2d& P, const Draw_MarkerShape T, const Draw_Color& C, const Standard_Real RSize);
  
  //! myPos field
  Standard_EXPORT gp_Pnt2d& ChangePos();
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  //! Returns always false
  Standard_EXPORT virtual Standard_Boolean PickReject (const Standard_Real X, const Standard_Real Y, const Standard_Real Prec) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Draw_Marker2D,Draw_Drawable2D)

protected:




private:


  gp_Pnt2d myPos;
  Draw_Color myCol;
  Draw_MarkerShape myTyp;
  Standard_Integer mySiz;


};







#endif // _Draw_Marker2D_HeaderFile
