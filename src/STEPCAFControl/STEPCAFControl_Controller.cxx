// Created on: 2000-10-05
// Created by: Andrey BETENEV
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


#include <STEPCAFControl_Controller.ixx>
#include <STEPCAFControl_ActorWrite.hxx>
#include <XSAlgo.hxx>
#include <Interface_Static.hxx>

//=======================================================================
//function : STEPCAFControl_Controller
//purpose  : 
//=======================================================================

STEPCAFControl_Controller::STEPCAFControl_Controller ()
{
  Handle(STEPCAFControl_ActorWrite) ActWrite = new STEPCAFControl_ActorWrite;
  theAdaptorWrite = ActWrite;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_Controller::Init ()
{
  static Standard_Boolean inic = Standard_False;
  if (inic) return Standard_True;
  inic = Standard_True;
  // self-registering
  Handle(STEPCAFControl_Controller) STEPCTL = new STEPCAFControl_Controller;
  // do XSAlgo::Init, cause it does not called before.
  XSAlgo::Init();
  // do something to avoid warnings...
  STEPCTL->AutoRecord();

  //-----------------------------------------------------------
  // Few variables for advanced control of translation process
  //-----------------------------------------------------------

  // Indicates whether to write sub-shape names to 'Name' attributes of
  // STEP Representation Items
  Interface_Static::Init   ("stepcaf", "write.stepcaf.subshapes.name", 'e', "");
  Interface_Static::Init   ("stepcaf", "write.stepcaf.subshapes.name", '&', "enum 0");
  Interface_Static::Init   ("stepcaf", "write.stepcaf.subshapes.name", '&', "eval Off"); // 0
  Interface_Static::Init   ("stepcaf", "write.stepcaf.subshapes.name", '&', "eval On");  // 1
  Interface_Static::SetIVal("write.stepcaf.subshapes.name", 0); // Disabled by default

  // Indicates whether to read sub-shape names from 'Name' attributes of
  // STEP Representation Items
  Interface_Static::Init   ("stepcaf", "read.stepcaf.subshapes.name", 'e', "");
  Interface_Static::Init   ("stepcaf", "read.stepcaf.subshapes.name", '&', "enum 0");
  Interface_Static::Init   ("stepcaf", "read.stepcaf.subshapes.name", '&', "eval Off"); // 0
  Interface_Static::Init   ("stepcaf", "read.stepcaf.subshapes.name", '&', "eval On");  // 1
  Interface_Static::SetIVal("read.stepcaf.subshapes.name", 0); // Disabled by default

  return Standard_True;
}
