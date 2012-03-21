// Created on: 2000-01-19
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <XSAlgo.ixx>
#include <ShapeAlgo.hxx>
#include <ShapeProcess_OperLibrary.hxx>
#include <Interface_Static.hxx>

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
  
  //  unit : pour depannage / test de non regression  0 MM  1 M(ancien)
  Interface_Static::Init("XSTEP"    ,"read.scale.unit", 'e',"");
  Interface_Static::Init("XSTEP","read.scale.unit",'&',"ematch 0");
  Interface_Static::Init("XSTEP","read.scale.unit",'&',"eval MM");
  Interface_Static::Init("XSTEP","read.scale.unit",'&',"eval M");
  Interface_Static::SetIVal ("read.scale.unit",0);

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
