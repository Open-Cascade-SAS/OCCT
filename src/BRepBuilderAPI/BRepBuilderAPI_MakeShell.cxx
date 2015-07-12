// Created on: 1994-02-18
// Created by: Remi LEQUETTE
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


#include <BRepBuilderAPI_MakeShell.hxx>
#include <Geom_Surface.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS_Shell.hxx>

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


