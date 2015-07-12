// Created on: 1994-10-24
// Created by: Jean Yves LEBEY
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

#ifndef _TestTopOpeDraw_DrawableSHA_HeaderFile
#define _TestTopOpeDraw_DrawableSHA_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Draw_Color.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <DBRep_DrawableShape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
class Draw_Text3D;
class Draw_Marker3D;
class TopoDS_Shape;
class Draw_Color;
class gp_Pnt;
class Draw_Display;


class TestTopOpeDraw_DrawableSHA;
DEFINE_STANDARD_HANDLE(TestTopOpeDraw_DrawableSHA, DBRep_DrawableShape)


class TestTopOpeDraw_DrawableSHA : public DBRep_DrawableShape
{

public:

  
  Standard_EXPORT TestTopOpeDraw_DrawableSHA(const TopoDS_Shape& S, const Draw_Color& FreeCol, const Draw_Color& ConnCol, const Draw_Color& EdgeCol, const Draw_Color& IsosCol, const Standard_Real size, const Standard_Integer nbisos, const Standard_Integer discret, const Standard_CString Text, const Draw_Color& TextColor, const Standard_Boolean DisplayGeometry = Standard_False);
  
  Standard_EXPORT void SetDisplayGeometry (const Standard_Boolean b);
  
  Standard_EXPORT void SetTol (const Standard_Real t);
  
  Standard_EXPORT void SetPar (const Standard_Real p);
  
  Standard_EXPORT void DisplayGeometry (Draw_Display& dis) const;
  
  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTI(TestTopOpeDraw_DrawableSHA,DBRep_DrawableShape)

protected:




private:

  
  Standard_EXPORT gp_Pnt Pnt() const;

  Handle(Draw_Text3D) myText;
  Draw_Color myTextColor;
  Standard_Boolean myDisplayGeometry;
  Handle(Draw_Marker3D) myDM3d;
  Standard_Real myTol;
  Standard_Real myPar;


};







#endif // _TestTopOpeDraw_DrawableSHA_HeaderFile
