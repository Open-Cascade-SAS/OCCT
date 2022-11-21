// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _TopoDSToStep_MakeTessellatedItem_HeaderFile
#define _TopoDSToStep_MakeTessellatedItem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDSToStep_Root.hxx>
#include <Message_ProgressRange.hxx>

class StepVisual_TessellatedItem;
class TopoDS_Face;
class TopoDS_Shell;
class Transfer_FinderProcess;

//! This class implements the mapping between
//! Face, Shell fromTopoDS and TriangulatedFace from StepVisual. 
class TopoDSToStep_MakeTessellatedItem : public TopoDSToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC


  Standard_EXPORT TopoDSToStep_MakeTessellatedItem();

  Standard_EXPORT TopoDSToStep_MakeTessellatedItem(const TopoDS_Face& theFace,
                                                   TopoDSToStep_Tool& theTool,
                                                   const Handle(Transfer_FinderProcess)& theFP,
                                                   const Standard_Boolean theToPreferSurfaceSet,
                                                   const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  Standard_EXPORT TopoDSToStep_MakeTessellatedItem(const TopoDS_Shell& theShell,
                                                   TopoDSToStep_Tool& theTool,
                                                   const Handle(Transfer_FinderProcess)& theFP,
                                                   const Message_ProgressRange& theProgress = Message_ProgressRange());

  Standard_EXPORT void Init(const TopoDS_Face& theFace,
                            TopoDSToStep_Tool& theTool,
                            const Handle(Transfer_FinderProcess)& theFP,
                            const Standard_Boolean theToPreferSurfaceSet,
                            const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  Standard_EXPORT void Init(const TopoDS_Shell& theShell,
                            TopoDSToStep_Tool& theTool,
                            const Handle(Transfer_FinderProcess)& theFP,
                            const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  Standard_EXPORT const Handle(StepVisual_TessellatedItem)& Value() const;

private:

  Handle(StepVisual_TessellatedItem) theTessellatedItem;

};

#endif // _TopoDSToStep_MakeTessellatedItem_HeaderFile
