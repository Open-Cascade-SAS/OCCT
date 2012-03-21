// Created on: 2000-09-29
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


#include <STEPConstruct_Tool.ixx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_TransferWriter.hxx>

//=======================================================================
//function : STEPConstruct_Tool
//purpose  : 
//=======================================================================

STEPConstruct_Tool::STEPConstruct_Tool () 
{
}

//=======================================================================
//function : STEPConstruct_Tool
//purpose  : 
//=======================================================================

STEPConstruct_Tool::STEPConstruct_Tool (const Handle(XSControl_WorkSession) &WS) 
{
  SetWS ( WS );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_Tool::SetWS (const Handle(XSControl_WorkSession) &WS)
{
  myWS.Nullify();
  myTransientProcess.Nullify();
  myFinderProcess.Nullify();
  
  if ( WS.IsNull() ) return Standard_False;
  myWS = WS;
  myHGraph = myWS->HGraph();
  
  // collect data on reading process
  Handle(XSControl_TransferReader) TR = WS->TransferReader();
  if ( ! TR.IsNull() ) myTransientProcess = TR->TransientProcess();

  // collect data on writing process
  Handle(XSControl_TransferWriter) TW = myWS->TransferWriter();
  if ( ! TW.IsNull() ) myFinderProcess = TW->FinderProcess();

  return ! myTransientProcess.IsNull() && ! myFinderProcess.IsNull();
}

