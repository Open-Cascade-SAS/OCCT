// File:	STEPCAFControl_Controller.cxx
// Created:	Thu Oct  5 19:05:26 2000
// Author:	Andrey BETENEV
//		<abv@doomox.nnov.matra-dtv.fr>

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
