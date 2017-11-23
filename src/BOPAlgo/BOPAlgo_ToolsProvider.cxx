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


#include <BOPAlgo_ToolsProvider.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Alerts.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BOPAlgo_ToolsProvider::BOPAlgo_ToolsProvider()
:
  BOPAlgo_Builder(),
  myTools(myAllocator),
  myMapTools(100, myAllocator)
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BOPAlgo_ToolsProvider::BOPAlgo_ToolsProvider
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Builder(theAllocator),
  myTools(myAllocator),
  myMapTools(100, myAllocator)
{
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BOPAlgo_ToolsProvider::Clear()
{
  BOPAlgo_Builder::Clear();
  myTools.Clear();
  myMapTools.Clear();
}

//=======================================================================
//function : AddTool
//purpose  : 
//=======================================================================
void BOPAlgo_ToolsProvider::AddTool(const TopoDS_Shape& theShape)
{
  if (myMapTools.Add(theShape))
    myTools.Append(theShape);
}

//=======================================================================
//function : SetTools
//purpose  : 
//=======================================================================
void BOPAlgo_ToolsProvider::SetTools(const TopTools_ListOfShape& theShapes)
{
  myTools.Clear();
  TopTools_ListIteratorOfListOfShape aIt(theShapes);
  for (; aIt.More(); aIt.Next())
    AddTool(aIt.Value());
}
