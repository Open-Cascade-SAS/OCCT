// Created on: 2018/03/21
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <BRepTest_DrawableHistory.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepTest_DrawableHistory, Draw_Drawable3D)

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================
void BRepTest_DrawableHistory::DrawOn(Draw_Display&) const
{
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void BRepTest_DrawableHistory:: Dump(Standard_OStream& theS) const
{
  myHistory->Dump(theS);
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================
void BRepTest_DrawableHistory::Whatis(Draw_Interpretor& theDI) const
{
  theDI << "history";
}
