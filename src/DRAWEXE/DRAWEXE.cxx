// File:	DRAWEXE.cxx
// Created:	Mon Aug 11 14:39:50 2003
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CASCADE S.A. 2003

#include <Draw.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>

//=======================================================================
//function : Draw_InitAppli
//purpose  : 
//=======================================================================

void Draw_InitAppli (Draw_Interpretor& di)
{
  Draw::Commands (di);
  DBRep::BasicCommands (di);
  DrawTrSurf::BasicCommands (di);
}

#include <Draw_Main.hxx>
DRAW_MAIN
