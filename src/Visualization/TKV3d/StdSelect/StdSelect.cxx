// Created on: 1995-03-27
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
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

#include <StdSelect.hxx>

#include <SelectMgr_Selection.hxx>
#include <StdSelect_BRepOwner.hxx>

//=================================================================================================

void StdSelect::SetDrawerForBRepOwner(const occ::handle<SelectMgr_Selection>& /*Sel*/,
                                      const occ::handle<Prs3d_Drawer>& /*Drwr*/)
{
  //  occ::handle<StdSelect_BRepOwner> BROWN;

  //   for(Sel->Init();Sel->More();Sel->Next()){
  //     BROWN =
  //     occ::down_cast<StdSelect_BRepOwner>(Sel->Sensitive()->BaseSensitive()->OwnerId());
  //     if(!BROWN.IsNull())
  //       BROWN->SetDrawer(Drwr);
  //   }
}
