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
  return Standard_True;
}
