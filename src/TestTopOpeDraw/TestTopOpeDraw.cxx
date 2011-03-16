// File:	TestTopOpeDraw.cxx
// Created:	Mon Jan 20 16:51:04 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#include <TestTopOpeDraw.ixx>
#include <DBRep.hxx>

//=======================================================================
//function : AllCommands
//purpose  : 
//=======================================================================

void  TestTopOpeDraw::AllCommands(Draw_Interpretor& theCommands)
{
  TestTopOpeDraw::OtherCommands(theCommands);
}
