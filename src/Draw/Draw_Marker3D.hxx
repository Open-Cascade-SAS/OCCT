// Created on: 1992-04-23
// Created by: Modelistation
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Draw_Marker3D_HeaderFile
#define _Draw_Marker3D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <Draw_Color.hxx>
#include <Draw_MarkerShape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Draw_Drawable3D.hxx>
class gp_Pnt;
class Draw_Color;
class Draw_Display;


class Draw_Marker3D;
DEFINE_STANDARD_HANDLE(Draw_Marker3D, Draw_Drawable3D)


class Draw_Marker3D : public Draw_Drawable3D
{

public:

  
  Standard_EXPORT Draw_Marker3D(const gp_Pnt& P, const Draw_MarkerShape T, const Draw_Color& C, const Standard_Integer ISize = 5);
  
  Standard_EXPORT Draw_Marker3D(const gp_Pnt& P, const Draw_MarkerShape T, const Draw_Color& C, const Standard_Real RSize);
  
  //! myPos field
  Standard_EXPORT gp_Pnt& ChangePos();
  
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  //! Returs always false
  Standard_EXPORT virtual Standard_Boolean PickReject (const Standard_Real X, const Standard_Real Y, const Standard_Real Prec) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Draw_Marker3D,Draw_Drawable3D)

protected:




private:


  gp_Pnt myPos;
  Draw_Color myCol;
  Draw_MarkerShape myTyp;
  Standard_Integer mySiz;
  Standard_Real myRSiz;
  Standard_Boolean myIsRSiz;


};







#endif // _Draw_Marker3D_HeaderFile
