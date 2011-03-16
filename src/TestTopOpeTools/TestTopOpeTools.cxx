// File:	TestTopOpeTools.cxx
// Created:	Mon Oct 24 13:33:04 1994
// Author:	Jean Yves LEBEY
//		<jyl@bravox>

#include <TestTopOpeTools.ixx>

//=======================================================================
//function : AllCommands
//purpose  : 
//=======================================================================

void  TestTopOpeTools::AllCommands(Draw_Interpretor& theCommands)
{
  TestTopOpeTools::TraceCommands(theCommands);
  TestTopOpeTools::OtherCommands(theCommands);
}
