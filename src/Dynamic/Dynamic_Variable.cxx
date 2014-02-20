// Created on: 1994-08-24
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Dynamic_Variable.ixx>

//=======================================================================
//function : Dynamic_Variable
//purpose  : 
//=======================================================================

Dynamic_Variable::Dynamic_Variable()
{
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_Variable::Parameter(const Handle(Dynamic_Parameter)& aparameter)
{
  theparameter = aparameter;
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Handle(Dynamic_Parameter) Dynamic_Variable::Parameter() const
{
  return theparameter;
}

//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================

void Dynamic_Variable::Mode(const Dynamic_ModeEnum amode)
{
  themode = amode;
}

//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================

Dynamic_ModeEnum Dynamic_Variable::Mode() const
{
  return themode;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_Variable::Dump(Standard_OStream& astream) const
{
  theparameter->Dump(astream);
}
