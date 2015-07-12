// Created on: 1994-12-13
// Created by: Frederic MAUPAS
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


#include <BRep_Tool.hxx>
#include <BRepTools_Modifier.hxx>
#include <Geom_Surface.hxx>
#include <MoniTool_DataMapIteratorOfDataMapOfShapeTransient.hxx>
#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//#include <TopoDSToStep_DirectModification.hxx>
//#include <TopoDSToStep_ConicalSurfModif.hxx>
Handle(TCollection_HAsciiString) 
TopoDSToStep::DecodeBuilderError(const TopoDSToStep_BuilderError E)
{
  Handle(TCollection_HAsciiString) mess;
  switch(E)
    {
    case TopoDSToStep_BuilderDone:
      {
	mess = new TCollection_HAsciiString("Builder Done");
	break;
      }
    case TopoDSToStep_NoFaceMapped:
      {
	mess = new TCollection_HAsciiString("None of the Shell Faces has been mapped");
	break;
      }
    case TopoDSToStep_BuilderOther:
      {
	mess = new TCollection_HAsciiString("Other Error in Builder");
	break;
      }
    }
  return mess;
}

Handle(TCollection_HAsciiString) 
TopoDSToStep::DecodeFaceError(const TopoDSToStep_MakeFaceError E)
{
  Handle(TCollection_HAsciiString) mess;
  switch(E)
    {
    case TopoDSToStep_FaceDone:
      {
	mess = new TCollection_HAsciiString("Face Done");
	break;
      }
    case TopoDSToStep_FaceOther:
      {
	mess = new TCollection_HAsciiString("Other Error in Make STEP face");
	break;
      }
    case TopoDSToStep_InfiniteFace:
      {
	mess = new TCollection_HAsciiString("The Face has no Outer Wire");
	break;
      }
    case TopoDSToStep_NonManifoldFace:
      {
	mess = new TCollection_HAsciiString("The Face is Internal or External");
	break;
      }
    case TopoDSToStep_NoWireMapped:
      {
	mess = new TCollection_HAsciiString("None of the Face Wires has been mapped");
	break;
      }
    }
  return mess;
}

Handle(TCollection_HAsciiString) 
TopoDSToStep::DecodeWireError(const TopoDSToStep_MakeWireError E)
{
  Handle(TCollection_HAsciiString) mess;
  switch(E)
    {
    case TopoDSToStep_WireDone:
      {
	mess = new TCollection_HAsciiString("Wire Done");
	break;
      }
    case TopoDSToStep_WireOther:
      {
	mess = new TCollection_HAsciiString("Other Error in Make STEP wire");
	break;
      }
    case TopoDSToStep_NonManifoldWire:
      {
	mess = new TCollection_HAsciiString("The Wire is Internal or External");
	break;
	}
    }
  return mess;
}

Handle(TCollection_HAsciiString) 
TopoDSToStep::DecodeEdgeError(const TopoDSToStep_MakeEdgeError E)
{
  Handle(TCollection_HAsciiString) mess;
  switch(E)
    {
    case TopoDSToStep_EdgeDone:
      {
	mess = new TCollection_HAsciiString("Edge Done");
	break;
      }
    case TopoDSToStep_EdgeOther:
      {
	mess = new TCollection_HAsciiString("Other Error in Make STEP Edge");
	break;
      }
    case TopoDSToStep_NonManifoldEdge:
      {
	mess = new TCollection_HAsciiString("The Edge is Internal or External");
	break;
      }
    }
  return mess;
}

Handle(TCollection_HAsciiString) 
TopoDSToStep::DecodeVertexError(const TopoDSToStep_MakeVertexError E)
{
  Handle(TCollection_HAsciiString) mess;
  switch(E)
    {
    case TopoDSToStep_VertexDone:
      {
	mess = new TCollection_HAsciiString("Vertex Done");
	break;
      }
    case TopoDSToStep_VertexOther:
      {
	mess = new TCollection_HAsciiString("Other Error in Make STEP Vertex");
	break;
      }
    }
  return mess;
}

//=======================================================================
//function : AddResult
//purpose  : 
//=======================================================================

void TopoDSToStep::AddResult (const Handle(Transfer_FinderProcess)& FP,
			      const TopoDS_Shape &Shape,
			      const Handle(Standard_Transient) &ent)
{
  Handle(Transfer_SimpleBinderOfTransient) result = new Transfer_SimpleBinderOfTransient;
  result->SetResult (ent);

  Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper (FP,Shape);
  Handle(Transfer_Binder) binder = FP->Find ( mapper );

  if ( binder.IsNull() ) FP->Bind ( mapper, result );
  else binder->AddResult ( result );
}

//=======================================================================
//function : AddResult
//purpose  : 
//=======================================================================

void TopoDSToStep::AddResult (const Handle(Transfer_FinderProcess)& FP,
			      const TopoDSToStep_Tool &Tool)
{
  const MoniTool_DataMapOfShapeTransient &Map = Tool.Map();
  MoniTool_DataMapIteratorOfDataMapOfShapeTransient it ( Map );
  for ( ; it.More(); it.Next() ) 
    TopoDSToStep::AddResult ( FP, it.Key(), it.Value() );
}
