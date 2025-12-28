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

occ::handle<TCollection_HAsciiString> StepToTopoDS::DecodeBuilderError(
  const StepToTopoDS_BuilderError Error)
{
  occ::handle<TCollection_HAsciiString> mess;
  switch (Error)
  {
    case StepToTopoDS_BuilderDone: {
      mess = new TCollection_HAsciiString("Builder Done");
      break;
    }
    case StepToTopoDS_BuilderOther: {
      mess = new TCollection_HAsciiString("Builder Other");
      break;
    }
  }
  return mess;
}

occ::handle<TCollection_HAsciiString> StepToTopoDS::DecodeShellError(
  const StepToTopoDS_TranslateShellError Error)
{
  occ::handle<TCollection_HAsciiString> mess;
  switch (Error)
  {
    case StepToTopoDS_TranslateShellDone: {
      mess = new TCollection_HAsciiString("Translate Shell Done");
      break;
    }
    case StepToTopoDS_TranslateShellOther: {
      mess = new TCollection_HAsciiString("Translate Shell Other");
      break;
    }
  }
  return mess;
}

occ::handle<TCollection_HAsciiString> StepToTopoDS::DecodeFaceError(
  const StepToTopoDS_TranslateFaceError Error)
{
  occ::handle<TCollection_HAsciiString> mess;
  switch (Error)
  {
    case StepToTopoDS_TranslateFaceDone: {
      mess = new TCollection_HAsciiString("Translate Face Done");
      break;
    }
    case StepToTopoDS_TranslateFaceOther: {
      mess = new TCollection_HAsciiString("Translate Face Other");
      break;
    }
  }
  return mess;
}

occ::handle<TCollection_HAsciiString> StepToTopoDS::DecodeEdgeError(
  const StepToTopoDS_TranslateEdgeError Error)
{
  occ::handle<TCollection_HAsciiString> mess;
  switch (Error)
  {
    case StepToTopoDS_TranslateEdgeDone: {
      mess = new TCollection_HAsciiString("Translate Edge Done");
      break;
    }
    case StepToTopoDS_TranslateEdgeOther: {
      mess = new TCollection_HAsciiString("Translate Edge Other");
      break;
    }
  }
  return mess;
}

occ::handle<TCollection_HAsciiString> StepToTopoDS::DecodeVertexError(
  const StepToTopoDS_TranslateVertexError Error)
{
  occ::handle<TCollection_HAsciiString> mess;
  switch (Error)
  {
    case StepToTopoDS_TranslateVertexDone: {
      mess = new TCollection_HAsciiString("Translate Vertex Done");
      break;
    }
    case StepToTopoDS_TranslateVertexOther: {
      mess = new TCollection_HAsciiString("Translate Vertex Other");
      break;
    }
  }
  return mess;
}

occ::handle<TCollection_HAsciiString> StepToTopoDS::DecodeVertexLoopError(
  const StepToTopoDS_TranslateVertexLoopError Error)
{
  occ::handle<TCollection_HAsciiString> mess;
  switch (Error)
  {
    case StepToTopoDS_TranslateVertexLoopDone: {
      mess = new TCollection_HAsciiString("Translate VertexLoop Done");
      break;
    }
    case StepToTopoDS_TranslateVertexLoopOther: {
      mess = new TCollection_HAsciiString("Translate VertexLoop Other");
      break;
    }
  }
  return mess;
}

occ::handle<TCollection_HAsciiString> StepToTopoDS::DecodePolyLoopError(
  const StepToTopoDS_TranslatePolyLoopError Error)
{
  occ::handle<TCollection_HAsciiString> mess;
  switch (Error)
  {
    case StepToTopoDS_TranslatePolyLoopDone: {
      mess = new TCollection_HAsciiString("Translate PolyLoop Done");
      break;
    }
    case StepToTopoDS_TranslatePolyLoopOther: {
      mess = new TCollection_HAsciiString("Translate PolyLoop Other");
      break;
    }
  }
  return mess;
}

const char* StepToTopoDS::DecodeGeometricToolError(const StepToTopoDS_GeometricToolError Error)
{
  const char* mess = "";
  switch (Error)
  {
    case StepToTopoDS_GeometricToolDone: {
      mess = static_cast<const char*>(" Geometric Tool is done");
      break;
    }
    case StepToTopoDS_GeometricToolIsDegenerated: {
      mess = static_cast<const char*>(" an Edge is degenerated");
      break;
    }
    case StepToTopoDS_GeometricToolHasNoPCurve: {
      mess = static_cast<const char*>(" SurfaceCurve does not contain a PCurve lying on the BasisSurface");
      break;
    }
    case StepToTopoDS_GeometricToolWrong3dParameters: {
      mess = static_cast<const char*>(" the update of 3D-Parameters failed");
      break;
    }
    case StepToTopoDS_GeometricToolNoProjectiOnCurve: {
      mess = static_cast<const char*>(" the projection of a VertexPoint on the curve3d failed");
      break;
    }
    case StepToTopoDS_GeometricToolOther: {
      mess = static_cast<const char*>(" GeometricTool failed");
      break;
    }
  }
  return mess;
}
