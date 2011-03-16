// File:	Draw_Commands.cxx
// Created:	Wed Mar  1 13:34:09 1995
// Author:	Remi LEQUETTE
//		<rle@bravox>

#include <Draw.ixx>

void Draw::Commands(Draw_Interpretor& theCommands)
{
  Draw::BasicCommands(theCommands);
  Draw::VariableCommands(theCommands);
  Draw::GraphicCommands(theCommands);
  Draw::PloadCommands(theCommands);
  Draw::UnitCommands(theCommands);
}
