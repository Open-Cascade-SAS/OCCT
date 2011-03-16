// File:	BRepBuilderAPI_MakeShell.cxx
// Created:	Fri Feb 18 15:13:16 1994
// Author:	Remi LEQUETTE
//		<rle@nonox>


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


