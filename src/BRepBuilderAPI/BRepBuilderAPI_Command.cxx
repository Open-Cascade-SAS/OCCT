// Created on: 1993-07-23
// Created by: Remi LEQUETTE
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


#include <BRepBuilderAPI_Command.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : BRepBuilderAPI_Command
//purpose  : 
//=======================================================================
BRepBuilderAPI_Command::BRepBuilderAPI_Command() :
       myDone(Standard_False)
{
}

//=======================================================================
//function : ~BRepBuilderAPI_Command
//purpose  : 
//=======================================================================
BRepBuilderAPI_Command::~BRepBuilderAPI_Command()
{}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean  BRepBuilderAPI_Command::IsDone()const 
{
  return myDone;
}


//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_Command::Check()const 
{
  if (!myDone)
    throw StdFail_NotDone("BRep_API: command not done");
}


//=======================================================================
//function : Done
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_Command::Done()
{
  myDone = Standard_True;
}



//=======================================================================
//function : NotDone
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_Command::NotDone()
{
  myDone = Standard_False;
}



