// Created on: 1993-07-23
// Created by: Remi LEQUETTE
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



#include <BRepLib_Command.ixx>

//=======================================================================
//function : BRepLib_Command
//purpose  : 
//=======================================================================

BRepLib_Command::BRepLib_Command() :
       myDone(Standard_False)
{
}

void BRepLib_Command::Delete()
{}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean  BRepLib_Command::IsDone()const 
{
  return myDone;
}


//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

void  BRepLib_Command::Check()const 
{
  if (!myDone)
    StdFail_NotDone::Raise("BRep_API: command not done");
}


//=======================================================================
//function : Done
//purpose  : 
//=======================================================================

void  BRepLib_Command::Done()
{
  myDone = Standard_True;
}



//=======================================================================
//function : NotDone
//purpose  : 
//=======================================================================

void  BRepLib_Command::NotDone()
{
  myDone = Standard_False;
}



