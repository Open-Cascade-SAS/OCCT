// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2018 OPEN CASCADE SAS
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


#include <BRepAlgoAPI_Defeaturing.hxx>

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Defeaturing::Build(const Message_ProgressRange& theRange)
{
  // Set not done state for the operation
  NotDone();

  // Clear the tools performing the operation
  Clear();

  // Set the inputs to BOPAlgo_RemoveFeatures algorithm
  myFeatureRemovalTool.SetShape(myInputShape);
  myFeatureRemovalTool.AddFacesToRemove(myFacesToRemove);
  myFeatureRemovalTool.SetToFillHistory(myFillHistory);
  myFeatureRemovalTool.SetRunParallel(myRunParallel);

  // Perform the features removal
  myFeatureRemovalTool.Perform(theRange);

  // Merge the Errors/Warnings from the features removal tool
  GetReport()->Merge(myFeatureRemovalTool.GetReport());

  if (HasErrors())
    return;

  // Set done state
  Done();

  // Get the result shape
  myShape = myFeatureRemovalTool.Shape();
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_Defeaturing::Modified(const TopoDS_Shape& theS)
{
  return myFeatureRemovalTool.Modified(theS);
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_Defeaturing::Generated(const TopoDS_Shape& theS)
{
  return myFeatureRemovalTool.Generated(theS);
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_Defeaturing::IsDeleted(const TopoDS_Shape& theS)
{
  return myFeatureRemovalTool.IsDeleted(theS);
}

//=======================================================================
//function : HasModified
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_Defeaturing::HasModified() const
{
  return myFeatureRemovalTool.HasModified();
}

//=======================================================================
//function : HasGenerated
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_Defeaturing::HasGenerated() const
{
  return myFeatureRemovalTool.HasGenerated();
}

//=======================================================================
//function : HasDeleted
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_Defeaturing::HasDeleted() const
{
  return myFeatureRemovalTool.HasDeleted();
}
