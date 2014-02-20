// Created on: 2003-08-11
// Created by: Sergey ZARITCHNY
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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
