// Created on: 1997-04-21
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <DrawDim_Dimension.ixx>
#include <Standard_DomainError.hxx>
#include <Draw_Interpretor.hxx>
#include <TCollection_AsciiString.hxx>


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
  if (!is_valued) Standard_DomainError::Raise();
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
