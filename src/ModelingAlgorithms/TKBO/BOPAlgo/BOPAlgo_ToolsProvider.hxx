// Created by: Oleg AGASHIN
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_ToolsProvider_HeaderFile
#define _BOPAlgo_ToolsProvider_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_Builder.hxx>

//! Auxiliary class providing API to operate tool arguments.
class BOPAlgo_ToolsProvider : public BOPAlgo_Builder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT BOPAlgo_ToolsProvider();

  Standard_EXPORT BOPAlgo_ToolsProvider(const Handle(NCollection_BaseAllocator)& theAllocator);

  //! Clears internal fields and arguments
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Adds Tool argument of the operation
  Standard_EXPORT virtual void AddTool(const TopoDS_Shape& theShape);

  //! Adds the Tool arguments of the operation
  Standard_EXPORT virtual void SetTools(const TopTools_ListOfShape& theShapes);

  //! Returns the Tool arguments of the operation
  const TopTools_ListOfShape& Tools() const { return myTools; }

protected:
  TopTools_ListOfShape myTools;
  TopTools_MapOfShape  myMapTools;
};

#endif // _BOPAlgo_ToolsProvider_HeaderFile
