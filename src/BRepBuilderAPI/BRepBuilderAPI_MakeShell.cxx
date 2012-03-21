// Created on: 1994-02-18
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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



#include <BRepBuilderAPI_MakeShell.ixx>


//=======================================================================
//function : BRepBuilderAPI_MakeShell
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeShell::BRepBuilderAPI_MakeShell()
{
}


//=======================================================================
//function : BRepBuilderAPI_MakeShell
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeShell::BRepBuilderAPI_MakeShell(const Handle(Geom_Surface)& S,
				     const Standard_Boolean Segment)
: myMakeShell(S,Segment)
{
  if ( myMakeShell.IsDone()) {
    Done();
    myShape = myMakeShell.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeShell
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeShell::BRepBuilderAPI_MakeShell(const Handle(Geom_Surface)& S, 
				     const Standard_Real UMin,
				     const Standard_Real UMax, 
				     const Standard_Real VMin, 
				     const Standard_Real VMax,
				     const Standard_Boolean Segment)
: myMakeShell(S,UMin,UMax,VMin,VMax,Segment)
{
  if ( myMakeShell.IsDone()) {
    Done();
    myShape = myMakeShell.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepBuilderAPI_MakeShell::Init(const Handle(Geom_Surface)& S, 
			     const Standard_Real UMin, 
			     const Standard_Real UMax, 
			     const Standard_Real VMin, 
			     const Standard_Real VMax,
			     const Standard_Boolean Segment)
{
  myMakeShell.Init(S,UMin,UMax,VMin,VMax,Segment);
  if ( myMakeShell.IsDone()) {
    Done();
    myShape = myMakeShell.Shape();
  }
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeShell::IsDone() const
{
  return myMakeShell.IsDone();
}



//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

BRepBuilderAPI_ShellError BRepBuilderAPI_MakeShell::Error() const 
{
  switch ( myMakeShell.Error()) {

  case BRepLib_ShellDone:
    return BRepBuilderAPI_ShellDone;

  case BRepLib_EmptyShell:
    return BRepBuilderAPI_EmptyShell;

  case BRepLib_DisconnectedShell:
    return BRepBuilderAPI_DisconnectedShell;

  case BRepLib_ShellParametersOutOfRange:
    return BRepBuilderAPI_ShellParametersOutOfRange;

  }

  // portage WNT
  return BRepBuilderAPI_ShellDone;
}


//=======================================================================
//function : TopoDS_Shell&
//purpose  : 
//=======================================================================

const TopoDS_Shell& BRepBuilderAPI_MakeShell::Shell() const 
{
  return myMakeShell.Shell();
}



//=======================================================================
//function : TopoDS_Shell
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeShell::operator TopoDS_Shell() const
{
  return Shell();
}


