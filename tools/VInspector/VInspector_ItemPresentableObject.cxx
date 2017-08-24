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

#include <AIS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemEntityOwner.hxx>
#include <inspector/VInspector_ItemSelection.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/VInspector_ViewModel.hxx>

#include <NCollection_List.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <Standard_Version.hxx>

#include <QColor>
#include <QItemSelectionModel>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemPresentableObject::initValue (int theItemRole) const
{
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
      case 1:
        return rowCount();
      case 2:
      {
        if (!aNullIO)
          return VInspector_Tools::GetPointerInfo (anIO, true).ToCString();
        break;
      }
      case 3:
      {
        Handle(AIS_Shape) aShapeIO = Handle(AIS_Shape)::DownCast (anIO);
        if (!aShapeIO.IsNull())
        {
          const TopoDS_Shape& aShape = aShapeIO->Shape();
          if (!aShape.IsNull())
            return VInspector_Tools::GetShapeTypeInfo (aShape.ShapeType()).ToCString();
        }
        break;
      }
      case 4:
      {
        if (theItemRole == Qt::ToolTipRole)
          return "SelectedOwners";
        else
        {
          Handle(AIS_InteractiveContext) aContext = GetContext();
          int aCount = VInspector_Tools::SelectedOwners (aContext, anIO, false);
          if (aCount > 0)
            return aCount;
        }
        break;
      }
      case 8:
      {
        if (theItemRole == Qt::ToolTipRole)
          return QString ("%1 / %2 / %3").arg ("OwnDeviationCoefficient")
                                         .arg ("ShapeDeflection")
                                         .arg ("IsAutoTriangulation");
        else
        {
          double aDeviationCoefficient = 0;
          Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast (anIO);
          if (!anAISShape.IsNull())
          {
            Standard_Real aPreviousCoefficient;
            anAISShape->OwnDeviationCoefficient(aDeviationCoefficient, aPreviousCoefficient);
          }
          double aShapeDeflection = 0;
          Handle(AIS_Shape) aShapeIO = Handle(AIS_Shape)::DownCast (anIO);
          if (!aShapeIO.IsNull())
          {
            const TopoDS_Shape& aShape = aShapeIO->Shape();
            if (!aShape.IsNull())
              aShapeDeflection = Prs3d::GetDeflection(aShape, anIO->Attributes());
          }
          bool anIsAutoTriangulation = anIO->Attributes()->IsAutoTriangulation();
          return QString ("%1 / %2 / %3").arg (aDeviationCoefficient)
                                         .arg (aShapeDeflection)
                                         .arg (anIsAutoTriangulation);
        }
        break;
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
  int aRows = 0;
  Handle(AIS_InteractiveObject) anIO = GetInteractiveObject();
  if (anIO.IsNull())
    return aRows;
  // iteration through sensitive privitives
  for (anIO->Init(); anIO->More(); anIO->Next())
    aRows++;

  return aRows;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemPresentableObject::createChild (int theRow, int theColumn)
{
  return VInspector_ItemSelection::CreateItem(currentItem(), theRow, theColumn);
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
    int aCurrentIndex = 0;
    for (AIS_ListIteratorOfListOfInteractive anIOIt (aListOfIO); anIOIt.More(); anIOIt.Next(), aCurrentIndex++)
    {
      if (aCurrentIndex != aRowId)
        continue;
      anIO = anIOIt.Value();
      break;
    }
  }

  setInteractiveObject (anIO);
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
// function : GetInteractiveObject
// purpose :
// =======================================================================
Handle(AIS_InteractiveObject) VInspector_ItemPresentableObject::GetInteractiveObject() const
{
  initItem();
  return myIO;
}

// =======================================================================
// function : PointerInfo
// purpose :
// =======================================================================
QString VInspector_ItemPresentableObject::PointerInfo() const
{
  return VInspector_Tools::GetPointerInfo (GetInteractiveObject(), true).ToCString();
}

// =======================================================================
// function : GetSelectedPresentations
// purpose :
// =======================================================================
NCollection_List<Handle(AIS_InteractiveObject)> VInspector_ItemPresentableObject::GetSelectedPresentations
                                                                  (QItemSelectionModel* theSelectionModel)
{
  NCollection_List<Handle(AIS_InteractiveObject)> aResultList;
  if (!theSelectionModel)
    return aResultList;
  
  QList<TreeModel_ItemBasePtr> anItems;
  
  QModelIndexList anIndices = theSelectionModel->selectedIndexes();
  for (QModelIndexList::const_iterator anIndicesIt = anIndices.begin(); anIndicesIt != anIndices.end(); anIndicesIt++)
  {
    TreeModel_ItemBasePtr anItem = TreeModel_ModelBase::GetItemByIndex (*anIndicesIt);
    if (!anItem || anItems.contains (anItem))
      continue;
    anItems.append (anItem);
  }

  QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
  for (QList<TreeModel_ItemBasePtr>::const_iterator anItemIt = anItems.begin(); anItemIt != anItems.end(); anItemIt++)
  {
    TreeModel_ItemBasePtr anItem = *anItemIt;
    VInspector_ItemPresentableObjectPtr aPrsItem = itemDynamicCast<VInspector_ItemPresentableObject>(anItem);
    if (!aPrsItem)
      continue;
    Handle(AIS_InteractiveObject) aPresentation = aPrsItem->GetInteractiveObject();
    if (aSelectedIds.contains ((size_t)aPresentation.operator->()))
      continue;
    aSelectedIds.append ((size_t)aPresentation.operator->());
    if (!aPresentation.IsNull())
      aResultList.Append (aPresentation);
  }
  return aResultList;
}
