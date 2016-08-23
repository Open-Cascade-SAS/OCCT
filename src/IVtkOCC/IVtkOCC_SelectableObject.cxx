// Created on: 2011-10-20 
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS 
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

#include <AIS_Shape.hxx>
#include <BRepBndLib.hxx>
#include <IVtkOCC_SelectableObject.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_ErrorHandler.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <TopoDS_Iterator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IVtkOCC_SelectableObject,SelectMgr_SelectableObject)

// Handle implementation


//============================================================================
// Method:  Constructor
// Purpose: Constructs a selectable object initalized by the given shape
//============================================================================
IVtkOCC_SelectableObject::IVtkOCC_SelectableObject (const IVtkOCC_Shape::Handle& theShape)
: SelectMgr_SelectableObject (PrsMgr_TOP_AllView),
  myShape (theShape)
{
  if (!myShape.IsNull())
  {
    myShape->SetSelectableObject (this);
  }

  // Minor stuff - but it facilitates usage of OCCT selection 
  // classes dealing with deflection, see ComputeSelection() below
  myOCCTDrawer = new Prs3d_Drawer();
}

//============================================================================
// Method:  Constructor
// Purpose: Constructs uninitialized selectable object.
//          setShape() should be called later.
//============================================================================
IVtkOCC_SelectableObject::IVtkOCC_SelectableObject()
: SelectMgr_SelectableObject (PrsMgr_TOP_AllView),
  myShape (0)
{ }

//============================================================================
// Method:  Destructor
// Purpose: 
//============================================================================
IVtkOCC_SelectableObject::~IVtkOCC_SelectableObject()
{ }

//============================================================================
// Method:  SetShape
// Purpose: Sets the selectable shape
//============================================================================
void IVtkOCC_SelectableObject::SetShape (const IVtkOCC_Shape::Handle& theShape)
{
  myShape = theShape;
  if (! myShape.IsNull())
  {
    myShape->SetSelectableObject (this);
  }

  // Shape has changed -> Clear all internal data
  myBndBox.SetVoid();
  myselections.Clear();
}

//============================================================================
// Method:  ComputeSelection
// Purpose: Internal method, computes selection data for viewer selector
//============================================================================
void IVtkOCC_SelectableObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                 const Standard_Integer theMode)
{
  if (myShape.IsNull())
  {
    return;
  }

  TopoDS_Shape anOcctShape = myShape->GetShape();

  if (anOcctShape.ShapeType() == TopAbs_COMPOUND)
  {
    TopoDS_Iterator anExplor (anOcctShape);
    if (!anExplor.More()) // Shape empty -> go away
    {
      return;
    }
  }

  TopAbs_ShapeEnum aTypeOfSel = AIS_Shape::SelectionType (theMode);

  Standard_Real aDeflection = myOCCTDrawer->MaximalChordialDeviation();
  if (myOCCTDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE)
  {
    Bnd_Box aBndBox;
    BRepBndLib::Add (anOcctShape, aBndBox);
    if (!aBndBox.IsVoid())
    {
      Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
      aBndBox.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
      aDeflection = Max (aXmax - aXmin, Max (aYmax - aYmin, aZmax - aZmin)) * 
        myOCCTDrawer->DeviationCoefficient();
    }
  }

  // Assume the shape has been displayed already -> triangulation should exist
  Standard_Boolean isAutoTriangulation = Standard_False;

  try
  {
    OCC_CATCH_SIGNALS
    StdSelect_BRepSelectionTool::Load (theSelection,
                                       this,
                                       anOcctShape,
                                       aTypeOfSel,
                                       aDeflection,
                                       myOCCTDrawer->DeviationAngle(),
                                       isAutoTriangulation);
  }
  catch (Standard_Failure)
  {
    if (theMode == 0)
    {
      Bnd_Box aBndBox = BoundingBox();
      Handle(StdSelect_BRepOwner) aOwner = new StdSelect_BRepOwner (anOcctShape, this);
      Handle(Select3D_SensitiveBox) aSensitiveBox = 
        new Select3D_SensitiveBox (aOwner, aBndBox);
      theSelection->Add (aSensitiveBox);
    }
  }
}

//============================================================================
// Method:  BoundingBox
// Purpose:
//============================================================================
const Bnd_Box& IVtkOCC_SelectableObject::BoundingBox()
{
  if (myShape.IsNull())
  {
    myBndBox.SetVoid();
    return myBndBox;
  }

  TopoDS_Shape anOcctShape = myShape->GetShape();

  if (anOcctShape.ShapeType() == TopAbs_COMPOUND)
  {
    TopoDS_Iterator anExplor (anOcctShape);
    if (!anExplor.More())
    { // Shape empty -> nothing to do
      myBndBox.SetVoid();
      return myBndBox;
    }
  }

  if (myBndBox.IsVoid())
  {
    // Add only edges and vertices, in case of troubles this should work anyway
    BRepBndLib::AddClose (anOcctShape, myBndBox);
  }

  return myBndBox;
}

//============================================================================
// Method:  BoundingBox
// Purpose:
//============================================================================
void IVtkOCC_SelectableObject::BoundingBox (Bnd_Box& theBndBox)
{
  BoundingBox();
  theBndBox = myBndBox;
}
