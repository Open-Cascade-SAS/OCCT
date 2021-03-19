// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
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

#include <inspector/VInspector_ItemPresentableObject.hxx>

#include <AIS.hxx>
#include <AIS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/VInspector_ViewModel.hxx>

#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <NCollection_List.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColor>
#include <QItemSelectionModel>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemPresentableObject::initValue (int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole == Qt::DisplayRole || theItemRole == Qt::ToolTipRole)
  {
    Handle(AIS_InteractiveObject) anIO = GetInteractiveObject();
    bool aNullIO = anIO.IsNull();
    switch (Column())
    {
      case 0:
      {
        if (aNullIO)
          return theItemRole == Qt::ToolTipRole ? QVariant ("Owners where Selectable is empty")
                                                : QVariant ("Free Owners");
        else
          return theItemRole == Qt::ToolTipRole ? QVariant ("")
                                                : QVariant (anIO->DynamicType()->Name());
      }
      case 4:
      {
        int aNbSelected = VInspector_Tools::SelectedOwners (GetContext(), anIO, false);
        return aNbSelected > 0 ? QString::number (aNbSelected) : "";
      }
      case 6:
      {
        double aDeviationCoefficient = 0;
        Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast (anIO);
        if (!anAISShape.IsNull())
        {
          Standard_Real aPreviousCoefficient;
          anAISShape->OwnDeviationCoefficient(aDeviationCoefficient, aPreviousCoefficient);
        }
        return QString::number(aDeviationCoefficient);
      }
      case 8:
      {
        double aDeviationCoefficient = 0;
        Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast (anIO);
        if (!anAISShape.IsNull())
        {
          Standard_Real aPreviousCoefficient;
          anAISShape->OwnDeviationCoefficient(aDeviationCoefficient, aPreviousCoefficient);
        }
        Handle(AIS_Shape) aShapeIO = Handle(AIS_Shape)::DownCast (anIO);
        bool anIsAutoTriangulation = aNullIO ? false : anIO->Attributes()->IsAutoTriangulation();
        return anIsAutoTriangulation ? QString ("true") : QString ("false");
      }
      default: break;
    }
  }
  if (theItemRole == Qt::BackgroundRole || theItemRole == Qt::ForegroundRole)
  {
    Handle(AIS_InteractiveContext) aContext = GetContext();
    if (Column() == 2 && VInspector_Tools::SelectedOwners(aContext, GetInteractiveObject(), false) > 0)
    {
      return (theItemRole == Qt::BackgroundRole) ? QColor(Qt::darkBlue) : QColor(Qt::white);
    }
    else if (theItemRole == Qt::ForegroundRole)
    {
      Handle(AIS_InteractiveObject) anIO = GetInteractiveObject();
      if (anIO.IsNull())
        return QVariant();

      AIS_ListOfInteractive aListOfIO;
      GetContext()->ErasedObjects(aListOfIO);
      for (AIS_ListIteratorOfListOfInteractive anIOIt(aListOfIO); anIOIt.More(); anIOIt.Next())
      {
        if (anIO == anIOIt.Value())
          return QColor(Qt::darkGray);
      }
      return QColor(Qt::black);
    }
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemPresentableObject::initRowCount() const
{
  return 0;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemPresentableObject::Init()
{
  VInspector_ItemContextPtr aParentItem = itemDynamicCast<VInspector_ItemContext>(Parent());
  Handle(AIS_InteractiveContext) aContext = aParentItem->GetContext();
  SetContext (aContext);

  Handle(AIS_InteractiveObject) anIO;
  if (!GetContext().IsNull())
  {
    int aRowId = Row();
    AIS_ListOfInteractive aListOfIO;
    GetContext()->DisplayedObjects (aListOfIO); // the presentation is in displayed objects of Context
    GetContext()->ErasedObjects (aListOfIO); // the presentation is in erased objects of Context

    std::vector<Handle(AIS_InteractiveObject)> aListOfIOSorted;
    aListOfIOSorted.reserve (aListOfIO.Size());
    for (AIS_ListIteratorOfListOfInteractive anIOIt (aListOfIO); anIOIt.More(); anIOIt.Next())
    {
      aListOfIOSorted.push_back (anIOIt.Value());
    }
    std::sort (aListOfIOSorted.begin(), aListOfIOSorted.end());

    int aCurrentIndex = 1; /* Properties item of context*/
    for (std::vector<Handle(AIS_InteractiveObject)>::const_iterator anIOIt = aListOfIOSorted.begin(); anIOIt != aListOfIOSorted.end(); anIOIt++, aCurrentIndex++)
    {
      if (aCurrentIndex != aRowId)
        continue;
      anIO = *anIOIt;
      break;
    }
  }

  setInteractiveObject (anIO);
  myTransformPersistence = !anIO.IsNull() ? anIO->TransformPersistence() : NULL;
  UpdatePresentationShape();
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemPresentableObject::Reset()
{
  VInspector_ItemBase::Reset();

  SetContext (NULL);
  setInteractiveObject (NULL);
  myTransformPersistence = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemPresentableObject::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemPresentableObject*>(this)->Init();
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemPresentableObject::buildPresentationShape()
{
  Handle(AIS_InteractiveObject) aPrs = myIO;
  if (aPrs.IsNull())
    return TopoDS_Shape();

  Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast (aPrs);
  if (!aShapePrs.IsNull())
    return aShapePrs->Shape();

  return TopoDS_Shape();
}

// =======================================================================
// function : PointerInfo
// purpose :
// =======================================================================
QString VInspector_ItemPresentableObject::PointerInfo() const
{
  return Standard_Dump::GetPointerInfo (GetInteractiveObject(), true).ToCString();
}

// =======================================================================
// function : Presentations
// purpose :
// =======================================================================
void VInspector_ItemPresentableObject::Presentations (NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  TreeModel_ItemBase::Presentations (thePresentations); 

  if (Column() != 0)
    return;
  thePresentations.Append (GetInteractiveObject());
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void VInspector_ItemPresentableObject::initStream (Standard_OStream& theOStream) const
{
  Handle(AIS_InteractiveObject) anIO = GetInteractiveObject();
  if (anIO.IsNull())
    return;

  anIO->DumpJson (theOStream);
}

