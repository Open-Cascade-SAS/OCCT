// Created on: 2022-05-11
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

#include <Standard_NullObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_RangeError.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_AssemblyIterator.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

// =======================================================================
// function : XCAFDoc_AssemblyIterator constructor
// purpose  : Starts from free shapes
// =======================================================================

XCAFDoc_AssemblyIterator::XCAFDoc_AssemblyIterator(const Handle(TDocStd_Document)& theDoc,
                                                   const Standard_Integer          theLevel)
    : myMaxLevel(theLevel),
      mySeedLevel(1)
{
  Standard_NullObject_Raise_if(theDoc.IsNull(), "Null document!");

  myShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  Standard_NoSuchObject_Raise_if(myShapeTool.IsNull(), "No XCAFDoc_ShapeTool attribute!");

  Standard_RangeError_Raise_if(myMaxLevel < 0, "Null document!");

  TDF_LabelSequence aRoots;
  myShapeTool->GetFreeShapes(aRoots);

  AuxAssemblyItem           anAuxItem;
  TColStd_ListOfAsciiString aParentPath;
  for (TDF_LabelSequence::Iterator anIt(aRoots); anIt.More(); anIt.Next())
  {
    createItem(anIt.Value(), aParentPath, anAuxItem);
    myFringe.Append(anAuxItem);
  }
}

// =======================================================================
// function : XCAFDoc_AssemblyIterator constructor
// purpose  : Starts from the specified root
// =======================================================================

XCAFDoc_AssemblyIterator::XCAFDoc_AssemblyIterator(const Handle(TDocStd_Document)& theDoc,
                                                   const XCAFDoc_AssemblyItemId&   theRoot,
                                                   const Standard_Integer          theLevel)
    : myMaxLevel(theLevel),
      mySeedLevel(theRoot.GetPath().Size())
{
  Standard_NullObject_Raise_if(theDoc.IsNull(), "Null document!");

  myShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  Standard_NoSuchObject_Raise_if(myShapeTool.IsNull(), "No XCAFDoc_ShapeTool attribute!");

  Standard_NullObject_Raise_if(theRoot.IsNull(), "Null assembly item!");

  Standard_RangeError_Raise_if(myMaxLevel < 0, "Null document!");

  AuxAssemblyItem aSeed;
  aSeed.myItem = theRoot;
  TDF_Tool::Label(theDoc->GetData(), theRoot.GetPath().Last(), aSeed.myLabel);

  if (aSeed.myLabel.IsNull())
    return;

  TDF_Label anOriginal;
  if (myShapeTool->GetReferredShape(aSeed.myLabel, anOriginal))
  {
    if (!myShapeTool->IsAssembly(aSeed.myLabel))
    {
      aSeed.myLabel = anOriginal;
    }
    else
    {
      TCollection_AsciiString aPathStr = theRoot.ToString();
      Standard_Integer        anIndex  = aPathStr.SearchFromEnd("/");
      if (anIndex != -1)
      {
        aPathStr.Remove(anIndex, aPathStr.Length() - anIndex + 1);
      }
      aSeed.myItem.Init(aPathStr);
    }
  }

  myFringe.Append(aSeed);
}

// =======================================================================
// function : More
// purpose  : Checks possibility to continue iteration
// =======================================================================

Standard_Boolean XCAFDoc_AssemblyIterator::More() const
{
  return !myFringe.IsEmpty();
}

// =======================================================================
// function : Next
// purpose  : Moves to the next position
// =======================================================================

void XCAFDoc_AssemblyIterator::Next()
{
  if (!More())
    return; // No next item.

  // Pop item
  AuxAssemblyItem aCurrent = myFringe.Last();
  myFringe.Remove(myFringe.Size());

  // Check current depth of iteration (root level is 0-level by convention)
  const int aCurrentDepth = aCurrent.myItem.GetPath().Size() - mySeedLevel;

  if (aCurrentDepth < myMaxLevel)
  {
    // If current item is an assembly, then the next items to iterate in
    // depth-first order are the components of this assembly
    TDF_LabelSequence aComponents;
    if (myShapeTool->IsAssembly(aCurrent.myLabel))
    {
      myShapeTool->GetComponents(aCurrent.myLabel, aComponents);
    }
    else if (myShapeTool->IsComponent(aCurrent.myLabel))
    {
      aComponents.Append(aCurrent.myLabel);
    }

    // Put all labels pending for iteration to the fringe
    AuxAssemblyItem anAuxItem;
    for (Standard_Integer l = aComponents.Length(); l >= 1; --l)
    {
      TDF_Label aLabel = aComponents(l); // Insertion-level label
      createItem(aLabel, aCurrent.myItem.GetPath(), anAuxItem);

      // Set item to iterate
      myFringe.Append(anAuxItem);
    }
  }
}

// =======================================================================
// function : Current
// purpose  : Returns current assembly item
// =======================================================================

XCAFDoc_AssemblyItemId XCAFDoc_AssemblyIterator::Current() const
{
  return myFringe.Last().myItem;
}

// =======================================================================
// function : createItem
// purpose  : Makes an assembly item id from the specified label
// =======================================================================

void XCAFDoc_AssemblyIterator::createItem(const TDF_Label&                 theLabel,
                                          const TColStd_ListOfAsciiString& theParentPath,
                                          AuxAssemblyItem&                 theAuxItem) const
{
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theLabel, anEntry);

  TDF_Label anOriginal;
  if (myShapeTool->GetReferredShape(theLabel, anOriginal))
  {
    theAuxItem.myLabel = anOriginal;
  }
  else
  {
    theAuxItem.myLabel = theLabel;
  }

  TColStd_ListOfAsciiString aPath = theParentPath;
  aPath.Append(anEntry);
  theAuxItem.myItem.Init(aPath);
}
