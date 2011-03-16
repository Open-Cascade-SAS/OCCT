// File:	BRepLib_Command.cxx
// Created:	Fri Jul 23 15:51:38 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


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



