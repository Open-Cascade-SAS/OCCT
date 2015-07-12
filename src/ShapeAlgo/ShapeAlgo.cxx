// Created on: 2000-02-07
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeExtend.hxx>

static Handle(ShapeAlgo_AlgoContainer) theContainer;

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

 void ShapeAlgo::Init() 
{
  static Standard_Boolean init = Standard_False;
  if (init) return;
  init = Standard_True;
  theContainer = new ShapeAlgo_AlgoContainer;

  // initialization of Standard Shape Healing
  ShapeExtend::Init();
}

//=======================================================================
//function : SetAlgoContainer
//purpose  : 
//=======================================================================

 void ShapeAlgo::SetAlgoContainer(const Handle(ShapeAlgo_AlgoContainer)& aContainer) 
{
  theContainer = aContainer;
}

//=======================================================================
//function : AlgoContainer
//purpose  : 
//=======================================================================

 Handle(ShapeAlgo_AlgoContainer) ShapeAlgo::AlgoContainer() 
{
  return theContainer;
}
