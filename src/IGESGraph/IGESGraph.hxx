// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen (TCD)
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

#ifndef _IGESGraph_HeaderFile
#define _IGESGraph_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESGraph_Protocol;


//! This package contains the group of classes necessary
//! to define Graphic data among Structure Entities.
//! (e.g., Fonts, Colors, Screen management ...)
class IGESGraph 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Prepares dynamic data (Protocol, Modules) for this package
  Standard_EXPORT static void Init();
  
  //! Returns the Protocol for this Package
  Standard_EXPORT static Handle(IGESGraph_Protocol) Protocol();




protected:





private:




friend class IGESGraph_LineFontDefTemplate;
friend class IGESGraph_LineFontDefPattern;
friend class IGESGraph_TextFontDef;
friend class IGESGraph_TextDisplayTemplate;
friend class IGESGraph_Color;
friend class IGESGraph_DefinitionLevel;
friend class IGESGraph_NominalSize;
friend class IGESGraph_DrawingSize;
friend class IGESGraph_DrawingUnits;
friend class IGESGraph_IntercharacterSpacing;
friend class IGESGraph_LineFontPredefined;
friend class IGESGraph_HighLight;
friend class IGESGraph_Pick;
friend class IGESGraph_UniformRectGrid;
friend class IGESGraph_ToolLineFontDefTemplate;
friend class IGESGraph_ToolLineFontDefPattern;
friend class IGESGraph_ToolTextFontDef;
friend class IGESGraph_ToolTextDisplayTemplate;
friend class IGESGraph_ToolColor;
friend class IGESGraph_ToolDefinitionLevel;
friend class IGESGraph_ToolNominalSize;
friend class IGESGraph_ToolDrawingSize;
friend class IGESGraph_ToolDrawingUnits;
friend class IGESGraph_ToolIntercharacterSpacing;
friend class IGESGraph_ToolLineFontPredefined;
friend class IGESGraph_ToolHighLight;
friend class IGESGraph_ToolPick;
friend class IGESGraph_ToolUniformRectGrid;
friend class IGESGraph_Protocol;
friend class IGESGraph_ReadWriteModule;
friend class IGESGraph_GeneralModule;
friend class IGESGraph_SpecificModule;

};







#endif // _IGESGraph_HeaderFile
