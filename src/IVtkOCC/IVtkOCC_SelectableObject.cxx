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

#include <IVtkOCC_SelectableObject.hxx>

#include <AIS_Shape.hxx>
#include <BRepBndLib.hxx>
#include <Message.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_ErrorHandler.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <TopoDS_Iterator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IVtkOCC_SelectableObject,SelectMgr_SelectableObject)

//============================================================================
// Method:  Constructor
// Purpose:
//============================================================================
IVtkOCC_SelectableObject::IVtkOCC_SelectableObject (const IVtkOCC_Shape::Handle& theShape)
: SelectMgr_SelectableObject (PrsMgr_TOP_AllView),
  myShape (theShape)
{
  if (!myShape.IsNull())
  {
    myShape->SetSelectableObject (this);
  }
}

//============================================================================
// Method:  Constructor
// Purpose:
//============================================================================
IVtkOCC_SelectableObject::IVtkOCC_SelectableObject()
: SelectMgr_SelectableObject (PrsMgr_TOP_AllView)
{
  //
}

//============================================================================
// Method:  Destructor
// Purpose:
//============================================================================
IVtkOCC_SelectableObject::~IVtkOCC_SelectableObject()
{
  //
}

//============================================================================
// Method:  SetShape
// Purpose:
//============================================================================
void IVtkOCC_SelectableObject::SetShape (const IVtkOCC_Shape::Handle& theShape)
{
  myShape = theShape;
  if (!myShape.IsNull())
  {
    myShape->SetSelectableObject (this);
  }

  // Shape has changed -> Clear all internal data
  myBndBox.SetVoid();
  myselections.Clear();
}

//============================================================================
// Method:  ComputeSelection
// Purpose:
//============================================================================
void IVtkOCC_SelectableObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                 const Standard_Integer theMode)
{
  if (myShape.IsNull())
  {
    return;
  }

  const TopoDS_Shape& anOcctShape = myShape->GetShape();
  if (anOcctShape.ShapeType() == TopAbs_COMPOUND
   && anOcctShape.NbChildren() == 0)
  {
    // Shape empty -> go away
    return;
  }

  const TopAbs_ShapeEnum aTypeOfSel = AIS_Shape::SelectionType (theMode);
  const Handle(Prs3d_Drawer)& aDrawer = myShape->Attributes();
  const Standard_Real aDeflection = StdPrs_ToolTriangulatedShape::GetDeflection (anOcctShape, aDrawer);
  try
  {
    OCC_CATCH_SIGNALS
    StdSelect_BRepSelectionTool::Load (theSelection,
                                       this,
                                       anOcctShape,
                                       aTypeOfSel,
                                       aDeflection,
                                       aDrawer->DeviationAngle(),
                                       aDrawer->IsAutoTriangulation());
  }
  catch (const Standard_Failure& anException)
  {
    Message::SendFail (TCollection_AsciiString("Error: IVtkOCC_SelectableObject::ComputeSelection(") + theMode + ") has failed ("
                     + anException.GetMessageString() + ")");
    if (theMode == 0)
    {
      Bnd_Box aBndBox = BoundingBox();
      Handle(StdSelect_BRepOwner) aOwner = new StdSelect_BRepOwner (anOcctShape, this);
      Handle(Select3D_SensitiveBox) aSensitiveBox = new Select3D_SensitiveBox (aOwner, aBndBox);
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

  const TopoDS_Shape& anOcctShape = myShape->GetShape();
  if (anOcctShape.ShapeType() == TopAbs_COMPOUND && anOcctShape.NbChildren() == 0)
  {
    // Shape empty -> nothing to do
    myBndBox.SetVoid ();
    return myBndBox;
  }

  if (myBndBox.IsVoid())
  {
    BRepBndLib::Add (anOcctShape, myBndBox, true);
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
