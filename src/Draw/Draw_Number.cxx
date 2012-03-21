// Created on: 1993-08-16
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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



#include <Draw_Number.ixx>

//=======================================================================
//function : Draw_Number
//purpose  : 
//=======================================================================

Draw_Number::Draw_Number(const Standard_Real V) :
       myValue(V)
{
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real  Draw_Number::Value()const 
{
  return myValue;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void  Draw_Number::Value(const Standard_Real V)
{
  myValue = V;
}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void  Draw_Number::DrawOn(Draw_Display&)const 
{
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  Draw_Number::Copy()const 
{
  Handle(Draw_Number) D = new Draw_Number(myValue);
  return D;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  Draw_Number::Dump(Standard_OStream& S)const 
{
  S << myValue;
}


//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void  Draw_Number::Whatis(Draw_Interpretor& S)const 
{
  S << "numeric";
}
