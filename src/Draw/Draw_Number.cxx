// Created on: 1993-08-16
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Draw_Display.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Number.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Number,Draw_Drawable3D)

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
