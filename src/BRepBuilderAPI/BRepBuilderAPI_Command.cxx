// File:	BRepBuilderAPI_Command.cxx
// Created:	Fri Jul 23 15:51:38 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepBuilderAPI_Command.ixx>

//=======================================================================
//function : BRepBuilderAPI_Command
//purpose  : 
//=======================================================================

BRepBuilderAPI_Command::BRepBuilderAPI_Command() :
       myDone(Standard_False)
{
}

void BRepBuilderAPI_Command::Delete()
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
    StdFail_NotDone::Raise("BRep_API: command not done");
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



