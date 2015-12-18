// Created on: 2000-01-19
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


#include <Interface_Static.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeProcess_OperLibrary.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>

static Handle(XSAlgo_AlgoContainer) theContainer;

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

 void XSAlgo::Init() 
{
  static Standard_Boolean init = Standard_False;
  if (init) return;
  init = Standard_True;
  ShapeAlgo::Init();
  theContainer = new XSAlgo_AlgoContainer;

  // init parameters
  Interface_Static::Standards();
  
  //#74 rln 10.03.99 S4135: adding new parameter for handling use of BRepLib::SameParameter
  Interface_Static::Init("XSTEP"  ,"read.stdsameparameter.mode", 'e',"");
  Interface_Static::Init("XSTEP"  ,"read.stdsameparameter.mode", '&',"ematch 0");
  Interface_Static::Init("XSTEP"  ,"read.stdsameparameter.mode", '&',"eval Off");
  Interface_Static::Init("XSTEP"  ,"read.stdsameparameter.mode", '&',"eval On");
  Interface_Static::SetIVal ("read.stdsameparameter.mode",0);
   
  // unit: supposed to be cascade unit (target unit for reading)
  Interface_Static::Init("XSTEP","xstep.cascade.unit", 'e',"");
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"enum 1");
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"eval INCH");  // 1
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"eval MM");    // 2
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"eval ??");    // 3
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"eval FT");    // 4
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"eval MI");    // 5
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"eval M");     // 6
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"eval KM");    // 7
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"eval MIL");   // 8
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"eval UM");    // 9
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"eval CM");    //10
  Interface_Static::Init ("XSTEP","xstep.cascade.unit",'&',"eval UIN");   //11
  Interface_Static::SetCVal ("xstep.cascade.unit","MM");
  
  // init Standard Shape Processing operators
  ShapeProcess_OperLibrary::Init();
}

//=======================================================================
//function : SetAlgoContainer
//purpose  : 
//=======================================================================

 void XSAlgo::SetAlgoContainer(const Handle(XSAlgo_AlgoContainer)& aContainer) 
{
  theContainer = aContainer;
}

//=======================================================================
//function : AlgoContainer
//purpose  : 
//=======================================================================

 Handle(XSAlgo_AlgoContainer) XSAlgo::AlgoContainer() 
{
  return theContainer;
}
