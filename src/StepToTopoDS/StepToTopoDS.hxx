// Created on: 1993-07-28
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _StepToTopoDS_HeaderFile
#define _StepToTopoDS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepToTopoDS_BuilderError.hxx>
#include <StepToTopoDS_TranslateShellError.hxx>
#include <StepToTopoDS_TranslateFaceError.hxx>
#include <StepToTopoDS_TranslateEdgeError.hxx>
#include <StepToTopoDS_TranslateVertexError.hxx>
#include <StepToTopoDS_TranslateVertexLoopError.hxx>
#include <StepToTopoDS_TranslatePolyLoopError.hxx>
#include <Standard_CString.hxx>
#include <StepToTopoDS_GeometricToolError.hxx>
class TCollection_HAsciiString;


//! This package implements the mapping between AP214
//! Shape representation and  CAS.CAD Shape Representation.
//! The source schema is Part42 (which is included in AP214)
class StepToTopoDS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodeBuilderError (const StepToTopoDS_BuilderError Error);
  
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodeShellError (const StepToTopoDS_TranslateShellError Error);
  
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodeFaceError (const StepToTopoDS_TranslateFaceError Error);
  
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodeEdgeError (const StepToTopoDS_TranslateEdgeError Error);
  
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodeVertexError (const StepToTopoDS_TranslateVertexError Error);
  
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodeVertexLoopError (const StepToTopoDS_TranslateVertexLoopError Error);
  
  Standard_EXPORT static Handle(TCollection_HAsciiString) DecodePolyLoopError (const StepToTopoDS_TranslatePolyLoopError Error);
  
  Standard_EXPORT static Standard_CString DecodeGeometricToolError (const StepToTopoDS_GeometricToolError Error);

};

#endif // _StepToTopoDS_HeaderFile
