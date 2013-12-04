// Created on: 2008-08-15
// Created by: Sergey ZARITCHNY
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PNaming_Naming_1.ixx>

//=======================================================================
//function : PNaming_Naming_1
//purpose  : 
//=======================================================================

PNaming_Naming_1::PNaming_Naming_1() {}


//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void PNaming_Naming_1::SetName(const Handle(PNaming_Name_1)& Name) 
{myName = Name ;}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(PNaming_Name_1) PNaming_Naming_1::GetName() const
{
  return myName;
}
