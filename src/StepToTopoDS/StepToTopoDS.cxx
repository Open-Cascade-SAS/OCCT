// Created on: 1995-01-03
// Created by: Frederic MAUPAS
// Copyright (c) 1995-1999 Matra Datavision
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


#include <StepToTopoDS.hxx>
#include <TCollection_HAsciiString.hxx>

Handle(TCollection_HAsciiString) StepToTopoDS::DecodeBuilderError(const StepToTopoDS_BuilderError Error)
{
  Handle(TCollection_HAsciiString) mess;
  switch (Error)
  {
    case StepToTopoDS_BuilderDone:
    {
      mess = new TCollection_HAsciiString("Builder Done");
      break;
    }
    case StepToTopoDS_BuilderOther:
    {
      mess = new TCollection_HAsciiString("Builder Other");
      break;
    }
  }
  return mess;
}

Handle(TCollection_HAsciiString) StepToTopoDS::DecodeShellError(const StepToTopoDS_TranslateShellError Error)
{
  Handle(TCollection_HAsciiString) mess;
  switch (Error)
  {
    case StepToTopoDS_TranslateShellDone:
    {
      mess = new TCollection_HAsciiString("Translate Shell Done");
      break;
    }
    case StepToTopoDS_TranslateShellOther:
    {
      mess = new TCollection_HAsciiString("Translate Shell Other");
      break;
    }
  }
  return mess;
}

Handle(TCollection_HAsciiString) StepToTopoDS::DecodeFaceError(const StepToTopoDS_TranslateFaceError Error)
{
  Handle(TCollection_HAsciiString) mess;
  switch (Error)
  {
    case StepToTopoDS_TranslateFaceDone:
    {
      mess = new TCollection_HAsciiString("Translate Face Done");
      break;
    }
    case StepToTopoDS_TranslateFaceOther:
    {
      mess = new TCollection_HAsciiString("Translate Face Other");
      break;
    }
  }
  return mess;
}

Handle(TCollection_HAsciiString) StepToTopoDS::DecodeEdgeError(const StepToTopoDS_TranslateEdgeError Error)
{
  Handle(TCollection_HAsciiString) mess;
  switch (Error)
  {
    case StepToTopoDS_TranslateEdgeDone:
    {
      mess = new TCollection_HAsciiString("Translate Edge Done");
      break;
    }
    case StepToTopoDS_TranslateEdgeOther:
    {
      mess = new TCollection_HAsciiString("Translate Edge Other");
      break;
    }
  }
  return mess;
}

Handle(TCollection_HAsciiString) StepToTopoDS::DecodeVertexError(const StepToTopoDS_TranslateVertexError Error)
{
  Handle(TCollection_HAsciiString) mess;
  switch (Error)
  {
    case StepToTopoDS_TranslateVertexDone:
    {
      mess = new TCollection_HAsciiString("Translate Vertex Done");
      break;
    }
    case StepToTopoDS_TranslateVertexOther:
    {
      mess = new TCollection_HAsciiString("Translate Vertex Other");
      break;
    }
  }
  return mess;
}

Handle(TCollection_HAsciiString) StepToTopoDS::DecodeVertexLoopError(const StepToTopoDS_TranslateVertexLoopError Error)
{
  Handle(TCollection_HAsciiString) mess;
  switch (Error)
  {
    case StepToTopoDS_TranslateVertexLoopDone:
    {
      mess = new TCollection_HAsciiString("Translate VertexLoop Done");
      break;
    }
    case StepToTopoDS_TranslateVertexLoopOther:
    {
      mess = new TCollection_HAsciiString("Translate VertexLoop Other");
      break;
    }
  }
  return mess;
}

Handle(TCollection_HAsciiString) StepToTopoDS::DecodePolyLoopError(const StepToTopoDS_TranslatePolyLoopError Error)
{
  Handle(TCollection_HAsciiString) mess;
  switch (Error)
  {
    case StepToTopoDS_TranslatePolyLoopDone:
    {
      mess = new TCollection_HAsciiString("Translate PolyLoop Done");
      break;
    }
    case StepToTopoDS_TranslatePolyLoopOther:
    {
      mess = new TCollection_HAsciiString("Translate PolyLoop Other");
      break;
    }
  }
  return mess;
}


Standard_CString StepToTopoDS::DecodeGeometricToolError(const StepToTopoDS_GeometricToolError Error)
{
  Standard_CString  mess="";
  switch (Error)
  {
    case StepToTopoDS_GeometricToolDone:
    {
      mess = Standard_CString(" Geometric Tool is done");
      break;
    }
    case StepToTopoDS_GeometricToolIsDegenerated:
    {
      mess = Standard_CString(" an Edge is degenerated");
      break;
    }
    case StepToTopoDS_GeometricToolHasNoPCurve:
    {
      mess = Standard_CString(" SurfaceCurve does not contain a PCurve lying on the BasisSurface");
      break;
    }
    case StepToTopoDS_GeometricToolWrong3dParameters:
    {
      mess = Standard_CString(" the update of 3D-Parameters failed");
      break;
    }
    case StepToTopoDS_GeometricToolNoProjectiOnCurve:
    {
      mess = Standard_CString(" the projection of a VertexPoint on the curve3d failed");
      break;
    }
    case StepToTopoDS_GeometricToolOther:
    {
      mess = Standard_CString(" GeometricTool failed");
      break;
    }
  }
  return mess;
}


