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




#include <Draw_Chronometer.ixx>

//=======================================================================
//function : Draw_Chronometer
//purpose  : 
//=======================================================================

Draw_Chronometer::Draw_Chronometer() 
{
}


//=======================================================================
//function : Timer
//purpose  : 
//=======================================================================

OSD_Timer&  Draw_Chronometer::Timer()
{
  return myTimer;
}



//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void  Draw_Chronometer::DrawOn(Draw_Display&)const 
{
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D)  Draw_Chronometer::Copy()const 
{
  Handle(Draw_Chronometer) C = new Draw_Chronometer();
  return C;
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  Draw_Chronometer::Dump(Standard_OStream& S)const 
{
  S << "Chronometer : ";
}


//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void  Draw_Chronometer::Whatis(Draw_Interpretor& i)const 
{
  i << "chronometer";
}


