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

#include <inspector/DFBrowserPane_TNamingNamedShape.hxx>

#include <inspector/DFBrowserPane_AttributePaneModel.hxx>
#include <inspector/DFBrowserPane_ItemDelegateButton.hxx>
#include <inspector/DFBrowserPane_ItemRole.hxx>
#include <inspector/DFBrowserPane_HelperExport.hxx>
#include <inspector/DFBrowserPane_SelectionKind.hxx>
#include <inspector/DFBrowserPane_TableView.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>

#include <QFileDialog>
#include <QGridLayout>
#include <QEvent>
#include <QHeaderView>
#include <QIcon>
#include <QMap>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QVariant>
#include <QWidget>

const int COLUMN_EXPORT_WIDTH = 20;
const int COLUMN_TYPE_WIDTH = 70;
const int COLUMN_POINTER_WIDTH = 110;
const int COLUMN_REFERENCE_WIDTH = 90;
const int COLUMN_EVOLUTION_WIDTH = 90;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_TNamingNamedShape::DFBrowserPane_TNamingNamedShape()
: DFBrowserPane_AttributePane(), myEvolutionTableView (0), myHelperExport (0)
{
  getPaneModel()->SetColumnCount (5);

  myEvolutionPaneModel = new DFBrowserPane_AttributePaneModel();
  myEvolutionPaneModel->SetColumnCount (11);
  QList<int> anItalicColumns;
  anItalicColumns << 2 << 6;
  myEvolutionPaneModel->SetItalicColumns (anItalicColumns);
  QItemSelectionModel* aSelectionModel = new QItemSelectionModel (myEvolutionPaneModel);
  mySelectionModels.push_back (aSelectionModel);
}

// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
QWidget* DFBrowserPane_TNamingNamedShape::CreateWidget (QWidget* theParent)
{
  QWidget* aMainWidget = new QWidget (theParent);
  aMainWidget->setVisible (false);

  myTableView = new DFBrowserPane_TableView (aMainWidget);
  myTableView->GetTableView()->verticalHeader()->setVisible (false);
  myTableView->SetModel (myPaneModel);
  QTableView* aTableView = myTableView->GetTableView();
  aTableView->setSelectionBehavior (QAbstractItemView::SelectItems);
  std::list<QItemSelectionModel*>::const_iterator aSelectionModelsIt = mySelectionModels.begin();
  aTableView->setSelectionModel (*aSelectionModelsIt);
  aSelectionModelsIt++;

  aTableView->horizontalHeader()->setStretchLastSection (false);
  aTableView->setColumnWidth (3, COLUMN_EXPORT_WIDTH);
  aTableView->setColumnWidth (4, COLUMN_EXPORT_WIDTH);
  DFBrowserPane_ItemDelegateButton* anItemDelegate = new DFBrowserPane_ItemDelegateButton (aTableView,
                                                                                          ":/icons/export_shape.png");
  QList<int> aRows;
  aRows << 0 << 1;
  anItemDelegate->SetFreeRows (aRows);
  QObject::connect (anItemDelegate, SIGNAL (buttonPressed (const QModelIndex&)),
                    &myHelperExport, SLOT (OnButtonPressed (const QModelIndex&)));
  aTableView->setItemDelegateForColumn (3, anItemDelegate);

  DFBrowserPane_ItemDelegateButton* anItemDelegate2 = new DFBrowserPane_ItemDelegateButton (aTableView,
                                                                                            ":/icons/folder_export.png");
  anItemDelegate2->SetFreeRows (aRows);
  aTableView->setItemDelegateForColumn (4, anItemDelegate2);

  myEvolutionTableView = new DFBrowserPane_TableView (aMainWidget);
  myEvolutionTableView->SetModel (myEvolutionPaneModel);
  aTableView = myEvolutionTableView->GetTableView();

  aTableView->setSelectionModel (*aSelectionModelsIt);

  aTableView->horizontalHeader()->setStretchLastSection (false);
  aTableView->setColumnWidth (5, COLUMN_EXPORT_WIDTH);
  aTableView->setColumnWidth (10, COLUMN_EXPORT_WIDTH);

  anItemDelegate = new DFBrowserPane_ItemDelegateButton (myEvolutionTableView->GetTableView(), ":/icons/export_shape.png");
  QObject::connect (anItemDelegate, SIGNAL (buttonPressed (const QModelIndex&)),
                    &myHelperExport, SLOT (OnButtonPressed (const QModelIndex&)));
  myEvolutionTableView->GetTableView()->setItemDelegateForColumn (5, anItemDelegate);

  anItemDelegate = new DFBrowserPane_ItemDelegateButton (myEvolutionTableView->GetTableView(), ":/icons/export_shape.png");
  QObject::connect (anItemDelegate, SIGNAL (buttonPressed (const QModelIndex&)),
                    &myHelperExport, SLOT (OnButtonPressed (const QModelIndex&)));
  myEvolutionTableView->GetTableView()->setItemDelegateForColumn (10, anItemDelegate);

  QGridLayout* aLay = new QGridLayout (aMainWidget);
  aLay->setContentsMargins (0, 0, 0, 0);
  aLay->addWidget (myTableView, 0, 0);
  aLay->addWidget (myEvolutionTableView, 1, 0);

  return aMainWidget;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPane_TNamingNamedShape::Init (const Handle(TDF_Attribute)& theAttribute)
{
  Handle(TNaming_NamedShape) aShapeAttr = Handle(TNaming_NamedShape)::DownCast (theAttribute);
  myHelperExport.Clear();

  QList<QVariant> aValues;
  aValues << "Version" << QString::number (aShapeAttr->Version()) << "" << "" << "";
  aValues << "Evolution" << DFBrowserPane_Tools::ToName (DB_NS_TYPE, aShapeAttr->Evolution()).ToCString() << "" << "" << "";

  NCollection_List<TopoDS_Shape> aShapes;
  QList<int> aFreeRows;
  aFreeRows << 0 << 1;

  TopoDS_Shape aShape = aShapeAttr->Get();
  TCollection_AsciiString aShapeInfo = !aShape.IsNull() ? DFBrowserPane_Tools::GetPointerInfo (aShape.TShape()) : "";
  aValues << "Shape" << aShapeInfo.ToCString() << DFBrowserPane_Tools::ShapeTypeInfo (aShape) << "" << "";
  aShapes.Append (aShape);
  if (aShape.IsNull())
    aFreeRows << 2;

  TopoDS_Shape aCurrentShape = TNaming_Tool::CurrentShape (aShapeAttr);
  TCollection_AsciiString aCurrentShapeInfo = !aCurrentShape.IsNull() ?
                                  DFBrowserPane_Tools::GetPointerInfo (aCurrentShape.TShape()) : "";
  aValues << "CurrentShape" << aCurrentShapeInfo.ToCString()
          << DFBrowserPane_Tools::ShapeTypeInfo (aCurrentShape) << "" << "";
  aShapes.Append (aCurrentShape);
  if (aCurrentShape.IsNull())
    aFreeRows << 3;

  TopoDS_Shape anOriginalShape = TNaming_Tool::OriginalShape (aShapeAttr);
  TCollection_AsciiString anOriginalShapeInfo = !anOriginalShape.IsNull() ?
            DFBrowserPane_Tools::GetPointerInfo (anOriginalShape.TShape()) : "";
  aValues << "OriginalShape" << anOriginalShapeInfo.ToCString()
          << DFBrowserPane_Tools::ShapeTypeInfo (anOriginalShape) << "" << "";
  aShapes.Append (anOriginalShape);
  if (anOriginalShape.IsNull())
    aFreeRows << 4;


  DFBrowserPane_AttributePaneModel* aModel = getPaneModel();
  aModel->Init (aValues);
  if (myTableView)
  {
    QTableView* aTableView = myTableView->GetTableView();
    for (int i = 0; i < aModel->columnCount(); i++)
    {
      if (i == 3 || i == 4)
        dynamic_cast<DFBrowserPane_ItemDelegateButton*>(aTableView->itemDelegateForColumn(3))->SetFreeRows (aFreeRows);
      else
        aTableView->resizeColumnToContents (i);
    }
  }
  QModelIndexList anIndices;
  int aRowId = 2;
  for (NCollection_List<TopoDS_Shape>::Iterator aShapeIt (aShapes); aShapeIt.More(); aShapeIt.Next(), aRowId++)
  {
    if (aShapeIt.Value().IsNull())
      continue;
    anIndices.clear();
    anIndices << aModel->index (aRowId, 1) << aModel->index (aRowId, 2) <<
                 aModel->index (aRowId, 3) << aModel->index (aRowId, 4);
    myHelperExport.AddShape (aShapeIt.Value(), anIndices);
  }

  // evolution table view filling
  aValues.clear();
  aRowId = 0;
  bool aHasModified = false;
  for (TNaming_Iterator aShapeAttrIt (aShapeAttr); aShapeAttrIt.More(); aShapeAttrIt.Next(), aRowId++)
  {
    const TopoDS_Shape& anOldShape = aShapeAttrIt.OldShape();
    const TopoDS_Shape& aNewShape = aShapeAttrIt.NewShape();

    Handle(TNaming_NamedShape) anOldAttr = TNaming_Tool::NamedShape (anOldShape, aShapeAttr->Label());
    aValues << DFBrowserPane_Tools::ToName (DB_NS_TYPE, aShapeAttrIt.Evolution()).ToCString()
            << (aShapeAttrIt.IsModification() ? "modified" : "-");
    aHasModified = aHasModified | aShapeAttrIt.IsModification();

    aValues << "New:";

    QString aLabelInfo;
    if (!anOldAttr.IsNull())
    {
      TDF_Label anOldLabel = anOldAttr->Label();
      if (!anOldLabel.IsNull())
        aLabelInfo = QString (DFBrowserPane_Tools::GetEntry (anOldLabel).ToCString());
    }
    if (!aNewShape.IsNull())
      aValues << DFBrowserPane_Tools::GetPointerInfo (aNewShape.TShape()->This()).ToCString()
              << DFBrowserPane_Tools::ShapeTypeInfo (aNewShape)
              << "";
    else
      aValues << "-" << "-" << "";
    aValues << "Old:";
    if (!anOldShape.IsNull())
      aValues << DFBrowserPane_Tools::GetPointerInfo (anOldShape.TShape()->This()).ToCString()
              << DFBrowserPane_Tools::ShapeTypeInfo (anOldShape)
              << aLabelInfo
              << "";
    else
      aValues << "-" << "-" << "-" << "";
  }

  if (myEvolutionTableView)
  {
    myEvolutionTableView->setVisible (aValues.size() > 0);
    myEvolutionTableView->GetTableView()->setColumnHidden (1, !aHasModified);
    myEvolutionPaneModel->Init (aValues);

    aRowId = 0;
    for (TNaming_Iterator aShapeAttrIt (aShapeAttr); aShapeAttrIt.More(); aShapeAttrIt.Next(), aRowId++)
    {
      const TopoDS_Shape& anOldShape = aShapeAttrIt.OldShape();
      const TopoDS_Shape& aNewShape = aShapeAttrIt.NewShape();

      if (!aNewShape.IsNull())
      {
        anIndices.clear();
        anIndices << myEvolutionPaneModel->index (aRowId, 3) << myEvolutionPaneModel->index (aRowId, 4)
                  << myEvolutionPaneModel->index (aRowId, 5);
        myHelperExport.AddShape (aNewShape, anIndices);
      }
      if (!anOldShape.IsNull())
      {
        anIndices.clear();
        anIndices << myEvolutionPaneModel->index (aRowId, 7) << myEvolutionPaneModel->index (aRowId, 8)
                  << myEvolutionPaneModel->index (aRowId, 10);
        myHelperExport.AddShape (anOldShape, anIndices);
      }
    }
    for (int i = 0; i < myEvolutionPaneModel->columnCount(); i++)
    {
      if (i == 5 || i == 10) continue;
      myEvolutionTableView->GetTableView()->resizeColumnToContents (i);
    }
  }
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TNamingNamedShape::GetValues (const Handle(TDF_Attribute)&, QList<QVariant>&)
{
}

// =======================================================================
// function : GetAttributeInfo
// purpose :
// =======================================================================
QVariant DFBrowserPane_TNamingNamedShape::GetAttributeInfo (const Handle(TDF_Attribute)& theAttribute, int theRole,
                                                            int theColumnId)
{
  if (theColumnId != 0)
    return DFBrowserPane_AttributePane::GetAttributeInfo (theAttribute, theRole, theColumnId);

  switch (theRole)
  {
    case Qt::DecorationRole: return QIcon (":/icons/named_shape.png");
    case DFBrowserPane_ItemRole_Decoration_40x40: return QIcon (":/icons/named_shape_40x40.png");
    case Qt::ForegroundRole:
    {
      TopoDS_Shape aShape;
      Handle(TNaming_NamedShape) anAttribute = Handle(TNaming_NamedShape)::DownCast (theAttribute);
      if (!anAttribute.IsNull())
        aShape = anAttribute->Get();
      if (aShape.IsNull())
        return QColor (Qt::black);

      return QColor (aShape.Orientation() == TopAbs_FORWARD ? Qt::darkGray :
                     aShape.Orientation() == TopAbs_REVERSED ? QColor (Qt::gray) : Qt::black);
    }
    default:
    break;
  }
  return DFBrowserPane_AttributePane::GetAttributeInfo (theAttribute, theRole, theColumnId);
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPane_TNamingNamedShape::GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                             QList<QVariant>& theValues)
{
  Handle(TNaming_NamedShape) aShapeAttribute = Handle(TNaming_NamedShape)::DownCast (theAttribute);

  if (aShapeAttribute->Get().IsNull())
    theValues.append ("EMPTY SHAPE");
  else
  {
    theValues.append (QString ("%1 : %2").arg (DFBrowserPane_Tools::ToName (DB_SHAPE_TYPE, aShapeAttribute->Get().ShapeType()).ToCString())
                                         .arg (DFBrowserPane_Tools::ToName (DB_NS_TYPE, aShapeAttribute->Evolution()).ToCString()));
  }
}

// =======================================================================
// function : GetSelectionKind
// purpose :
// =======================================================================
int DFBrowserPane_TNamingNamedShape::GetSelectionKind (QItemSelectionModel* theModel)
{
  int aKind = DFBrowserPane_AttributePaneAPI::GetSelectionKind (theModel);

  QTableView* aTableView = myTableView->GetTableView();
  if (aTableView->selectionModel() != theModel)
    return aKind;

  QModelIndexList aSelectedIndices = theModel->selectedIndexes();
  if (aSelectedIndices.size() != 1)
    return aKind;

  QModelIndex aSelectedIndex = aSelectedIndices.first();
  int aRow = aSelectedIndex.row();
  if (aRow == 0 || aRow == 1)
    return aKind;

  if (aSelectedIndex.column() == 4)
    aKind = DFBrowserPane_SelectionKind_ExportToShapeViewer;

  return aKind;
}

// =======================================================================
// function : GetSelectionParameters
// purpose :
// =======================================================================
void DFBrowserPane_TNamingNamedShape::GetSelectionParameters (QItemSelectionModel* theModel,
                                                              NCollection_List<Handle(Standard_Transient)>& theParameters)
{
  QTableView* aTableView = myTableView->GetTableView();
  if (aTableView->selectionModel() != theModel)
    return;

  QModelIndexList aSelectedIndices = theModel->selectedIndexes();
  if (aSelectedIndices.size() != 1)
    return;

  QModelIndex aSelectedIndex = aSelectedIndices.first();
  if (aSelectedIndex.column() != 4)
    return;

  const TopoDS_Shape& aShape = myHelperExport.GetShape (aSelectedIndex);
  if (aShape.IsNull())
    return;
  theParameters.Append (aShape.TShape());
}

// =======================================================================
// function : GetReferences
// purpose :
// =======================================================================
void DFBrowserPane_TNamingNamedShape::GetReferences (const Handle(TDF_Attribute)& theAttribute,
                                                     NCollection_List<TDF_Label>& theRefLabels,
                                                     Handle(Standard_Transient)& theRefPresentation)
{
  if (!myEvolutionTableView)
    return;
  QStringList aSelectedEntries = DFBrowserPane_TableView::GetSelectedColumnValues (
                                                          myEvolutionTableView->GetTableView(), 9);

  Handle(TNaming_NamedShape) aShapeAttr = Handle(TNaming_NamedShape)::DownCast (theAttribute);
  for (TNaming_Iterator aShapeAttrIt (aShapeAttr); aShapeAttrIt.More(); aShapeAttrIt.Next())
  {
    const TopoDS_Shape& anOldShape = aShapeAttrIt.OldShape();

    Handle(TNaming_NamedShape) anOldAttr = TNaming_Tool::NamedShape (anOldShape, aShapeAttr->Label());
    QString aLabelInfo;
    if (!anOldAttr.IsNull())
    {
      TDF_Label anOldLabel = anOldAttr->Label();
      if (!anOldLabel.IsNull())
      {
        if (aSelectedEntries.contains (DFBrowserPane_Tools::GetEntry (anOldLabel).ToCString()))
        theRefLabels.Append (anOldLabel);
      }
    }
  }
  TopoDS_Shape aShape = getSelectedShapes();
  if (!aShape.IsNull())
    theRefPresentation = new AIS_Shape (aShape);
}

// =======================================================================
// function : GetPresentation
// purpose :
// =======================================================================
Handle(Standard_Transient) DFBrowserPane_TNamingNamedShape::GetPresentation (const Handle(TDF_Attribute)& theAttribute)
{
  Handle(Standard_Transient) aPresentation;
  Handle(TNaming_NamedShape) aShapeAttr = Handle(TNaming_NamedShape)::DownCast (theAttribute);
  if (aShapeAttr.IsNull())
    return aPresentation;

  TopoDS_Shape aShape = aShapeAttr->Get();
  if (aShape.IsNull())
    return aPresentation;

  aPresentation = new AIS_Shape (aShape);
  return aPresentation;
}

// =======================================================================
// function : getSelectedShapes
// purpose :
// =======================================================================
TopoDS_Shape DFBrowserPane_TNamingNamedShape::getSelectedShapes()
{
  TopoDS_Shape aShape;

  if (!myTableView && !myEvolutionTableView)
    return aShape;

  // table view selected shapes
  QItemSelectionModel* aTableViewSelModel = myTableView->GetTableView()->selectionModel();
  QModelIndexList anIndices = aTableViewSelModel->selectedIndexes();

  BRep_Builder aBuilder;
  TopoDS_Compound aComp;
  aBuilder.MakeCompound (aComp);
  bool aHasShapes = false;
  for (QModelIndexList::const_iterator anIt = anIndices.begin(), aLast = anIndices.end(); anIt != aLast; anIt++)
  {
    QModelIndex anIndex = *anIt;
    if (!myHelperExport.HasShape (anIndex))
      continue;
    aBuilder.Add (aComp, myHelperExport.GetShape (anIndex));
    aHasShapes = true;
  }

  // evolution table selected shapes
  aTableViewSelModel = myEvolutionTableView->GetTableView()->selectionModel();
  anIndices.clear();
  anIndices = aTableViewSelModel->selectedIndexes();
  for (QModelIndexList::const_iterator anIt = anIndices.begin(), aLast = anIndices.end(); anIt != aLast; anIt++)
  {
    QModelIndex anIndex = *anIt;
    if (!myHelperExport.HasShape (anIndex))
      continue;
    aBuilder.Add (aComp, myHelperExport.GetShape (anIndex));
    aHasShapes = true;
  }
  if (aHasShapes)
    aShape = aComp;
  return aShape;
}
