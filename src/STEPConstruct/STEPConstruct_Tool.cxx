// File:	STEPConstruct_Tool.cxx
// Created:	Fri Sep 29 16:18:16 2000
// Author:	Andrey BETENEV
//		<abv@doomox.nnov.matra-dtv.fr>

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

