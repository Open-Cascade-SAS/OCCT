// Created on: 1997-04-21
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawDim_Dimension.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawDim_Dimension,Draw_Drawable3D)

//=======================================================================
//function : DrawDim_Dimension
//purpose  : 
//=======================================================================
DrawDim_Dimension::DrawDim_Dimension()
     : is_valued(Standard_False),
       myTextColor(Draw_blanc)
{
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void DrawDim_Dimension::SetValue (const Standard_Real avalue)
{
  is_valued = Standard_True;
  myValue = avalue;
}

//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================

Standard_Real DrawDim_Dimension::GetValue() const 
{
  if (!is_valued) throw Standard_DomainError();
  return myValue;
}

//=======================================================================
//function : IsValued
//purpose  : 
//=======================================================================

Standard_Boolean DrawDim_Dimension::IsValued() const 
{
  return is_valued;
}

//=======================================================================
//function : TextColor
//purpose  : 
//=======================================================================

Draw_Color DrawDim_Dimension::TextColor() const
{
  return myTextColor;
}

//=======================================================================
//function : TextColor
//purpose  : 
//=======================================================================

void DrawDim_Dimension::TextColor(const Draw_Color& C)
{
   myTextColor = C;
}

//=======================================================================
//function : DrawText
//purpose  : 
//=======================================================================

void DrawDim_Dimension::DrawText(const gp_Pnt& P, Draw_Display& D) const
{
  TCollection_AsciiString t = Name();
  if (is_valued) {
    t+="=";
    Standard_Integer l = t.Length();
    t+= myValue;
    for (Standard_Integer i = l; i <= t.Length(); i++) {
      if (t.Value(i) == '.') { t.Trunc(i+2); break; }
    }
  }

  D.SetColor(myTextColor);
  D.DrawString(P,t.ToCString());
}
