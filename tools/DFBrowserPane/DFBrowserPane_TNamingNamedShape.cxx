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

#include <DFBrowserPane_TNamingNamedShape.hxx>

#include <DFBrowserPane_AttributePaneModel.hxx>
#include <DFBrowserPane_ItemDelegateButton.hxx>
#include <DFBrowserPane_ItemRole.hxx>
#include <DFBrowserPane_HelperExport.hxx>
#include <DFBrowserPane_SelectionKind.hxx>
#include <DFBrowserPane_TableView.hxx>
#include <DFBrowserPane_Tools.hxx>

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

//#define REQUIRE_OCAF_REVIEW:13
// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_TNamingNamedShape::DFBrowserPane_TNamingNamedShape()
: DFBrowserPane_AttributePane(), myEvolutionTableView (0), myHelperExport (0)
{
  QList<QVariant> aHeaderValues;
  aHeaderValues << "Value" << "Type" << "BREP" << "SV";
  getPaneModel()->SetHeaderValues (aHeaderValues, Qt::Horizontal);
  getPaneModel()->SetColumnCount (aHeaderValues.count());

  aHeaderValues.clear();
  aHeaderValues << "Version" << "Evolution" << "Shape" << "Current Shape" << "Original Shape";
  getPaneModel()->SetHeaderValues (aHeaderValues, Qt::Vertical);

  aHeaderValues.clear();
  aHeaderValues << "New Shape" << "Type" << "" << "Old Shape" << "Type" << "Label" << "" << "Evolution" << "isModified";
  myEvolutionPaneModel = new DFBrowserPane_AttributePaneModel();
  myEvolutionPaneModel->SetHeaderValues (aHeaderValues, Qt::Horizontal);
  myEvolutionPaneModel->SetColumnCount (aHeaderValues.count());
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
  myTableView->SetModel (myPaneModel);
  QTableView* aTableView = myTableView->GetTableView();
  aTableView->setSelectionBehavior (QAbstractItemView::SelectItems);
  std::list<QItemSelectionModel*>::const_iterator aSelectionModelsIt = mySelectionModels.begin();
  aTableView->setSelectionModel (*aSelectionModelsIt);
  aSelectionModelsIt++;

  aTableView->horizontalHeader()->setStretchLastSection (false);
  aTableView->setColumnWidth (0, COLUMN_POINTER_WIDTH);
  aTableView->setColumnWidth (1, COLUMN_TYPE_WIDTH);
  aTableView->setColumnWidth (2, COLUMN_EXPORT_WIDTH);
  aTableView->setColumnWidth (3, COLUMN_EXPORT_WIDTH);
  aTableView->verticalHeader()->setVisible (true);
  DFBrowserPane_ItemDelegateButton* anItemDelegate = new DFBrowserPane_ItemDelegateButton (aTableView,
                                                                                          ":/icons/export_shape.png");
  QList<int> aRows;
  aRows << 0 << 1;
  anItemDelegate->SetFreeRows (aRows);
  QObject::connect (anItemDelegate, SIGNAL (buttonPressed (const QModelIndex&)),
                    &myHelperExport, SLOT (OnButtonPressed (const QModelIndex&)));
  aTableView->setItemDelegateForColumn (2, anItemDelegate);

  DFBrowserPane_ItemDelegateButton* anItemDelegate2 = new DFBrowserPane_ItemDelegateButton (aTableView,
                                                                                            ":/icons/folder_export.png");
  anItemDelegate2->SetFreeRows (aRows);
  aTableView->setItemDelegateForColumn (3, anItemDelegate2);

  myEvolutionTableView = new DFBrowserPane_TableView (aMainWidget);
  myEvolutionTableView->SetModel (myEvolutionPaneModel);
  aTableView = myEvolutionTableView->GetTableView();

  aTableView->setSelectionModel (*aSelectionModelsIt);

  QList<int> aColumnWidths;
  aColumnWidths << COLUMN_POINTER_WIDTH << COLUMN_TYPE_WIDTH << COLUMN_EXPORT_WIDTH << COLUMN_POINTER_WIDTH
                << COLUMN_TYPE_WIDTH << COLUMN_REFERENCE_WIDTH << COLUMN_EXPORT_WIDTH << COLUMN_EVOLUTION_WIDTH;
  for (int aColumnId = 0, aCount = aColumnWidths.size(); aColumnId < aCount; aColumnId++)
    aTableView->setColumnWidth (aColumnId, aColumnWidths[aColumnId]);

  anItemDelegate = new DFBrowserPane_ItemDelegateButton (myEvolutionTableView->GetTableView(), ":/icons/export_shape.png");
  QObject::connect (anItemDelegate, SIGNAL (buttonPressed (const QModelIndex&)),
                    &myHelperExport, SLOT (OnButtonPressed (const QModelIndex&)));
  myEvolutionTableView->GetTableView()->setItemDelegateForColumn (2, anItemDelegate);

  anItemDelegate = new DFBrowserPane_ItemDelegateButton (myEvolutionTableView->GetTableView(), ":/icons/export_shape.png");
  QObject::connect (anItemDelegate, SIGNAL (buttonPressed (const QModelIndex&)),
                    &myHelperExport, SLOT (OnButtonPressed (const QModelIndex&)));
  myEvolutionTableView->GetTableView()->setItemDelegateForColumn (6, anItemDelegate);

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

  // table view filling
  QList<QVariant> aValues;
  aValues << QString::number (aShapeAttr->Version()) << "" << "" << "";
  aValues << DFBrowserPane_Tools::ToName (DB_NS_TYPE, aShapeAttr->Evolution()).ToCString() << "" << "" << "";

  NCollection_List<TopoDS_Shape> aShapes;
  {
    TopoDS_Shape aShape = aShapeAttr->Get();
    TCollection_AsciiString aShapeInfo = !aShape.IsNull() ? DFBrowserPane_Tools::GetPointerInfo (aShape.TShape()) : "";
    aValues << aShapeInfo.ToCString() << DFBrowserPane_Tools::ShapeTypeInfo (aShape) << "" << "";
    aShapes.Append (aShape);

    TopoDS_Shape aCurrentShape = TNaming_Tool::CurrentShape (aShapeAttr);
    TCollection_AsciiString aCurrentShapeInfo = !aCurrentShape.IsNull() ?
                                    DFBrowserPane_Tools::GetPointerInfo (aCurrentShape.TShape()) : "";
    aValues << aCurrentShapeInfo.ToCString() << DFBrowserPane_Tools::ShapeTypeInfo (aCurrentShape) << "" << "";
    aShapes.Append (aCurrentShape);

    TopoDS_Shape anOriginalShape = TNaming_Tool::OriginalShape (aShapeAttr);
    TCollection_AsciiString anOriginalShapeInfo = !anOriginalShape.IsNull() ?
              DFBrowserPane_Tools::GetPointerInfo (aShape.TShape()) : "";
    aValues << anOriginalShapeInfo.ToCString() << DFBrowserPane_Tools::ShapeTypeInfo (anOriginalShape) << "" << "";
    aShapes.Append (anOriginalShape);
  }

  DFBrowserPane_AttributePaneModel* aModel = getPaneModel();
  aModel->Init (aValues);

  QModelIndexList anIndices;
  int aRowId = 2;
  for (NCollection_List<TopoDS_Shape>::Iterator aShapeIt (aShapes); aShapeIt.More(); aShapeIt.Next(), aRowId++)
  {
    const TopoDS_Shape& aShape = aShapeIt.Value();
    if (aShape.IsNull())
      continue;

    anIndices.clear();
    anIndices << aModel->index (aRowId, 2) << aModel->index (aRowId, 3);
    myHelperExport.AddShape (aShape, anIndices);
  }

  // evolution table view filling
  aValues.clear();
  aRowId = 0;
  for (TNaming_Iterator aShapeAttrIt (aShapeAttr); aShapeAttrIt.More(); aShapeAttrIt.Next(), aRowId++)
  {
    const TopoDS_Shape& anOldShape = aShapeAttrIt.OldShape();
    const TopoDS_Shape& aNewShape = aShapeAttrIt.NewShape();

    Handle(TNaming_NamedShape) anOldAttr = TNaming_Tool::NamedShape (anOldShape, aShapeAttr->Label());
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
      aValues << "" << "" << "";
    if (!anOldShape.IsNull())
      aValues << DFBrowserPane_Tools::GetPointerInfo (anOldShape.TShape()->This()).ToCString()
              << DFBrowserPane_Tools::ShapeTypeInfo (anOldShape)
              << aLabelInfo
              << "";
    else
      aValues << "" << "" << "" << "";
    aValues << DFBrowserPane_Tools::ToName (DB_NS_TYPE, aShapeAttrIt.Evolution()).ToCString()
            << (aShapeAttrIt.IsModification() ? "true" : "false");
  }

  if (myEvolutionTableView)
  {
    myEvolutionTableView->setVisible (aValues.size() > 0);
    myEvolutionPaneModel->Init (aValues);

    aRowId = 0;
    for (TNaming_Iterator aShapeAttrIt (aShapeAttr); aShapeAttrIt.More(); aShapeAttrIt.Next(), aRowId++)
    {
      const TopoDS_Shape& anOldShape = aShapeAttrIt.OldShape();
      const TopoDS_Shape& aNewShape = aShapeAttrIt.NewShape();

      if (!aNewShape.IsNull())
      {
        anIndices.clear();
        anIndices << myEvolutionPaneModel->index (aRowId, 0) << myEvolutionPaneModel->index (aRowId, 1)
                  << myEvolutionPaneModel->index (aRowId, 2);
        myHelperExport.AddShape (aNewShape, anIndices);
      }
      if (!anOldShape.IsNull())
      {
        anIndices.clear();
        anIndices << myEvolutionPaneModel->index (aRowId, 3) << myEvolutionPaneModel->index (aRowId, 4)
                  << myEvolutionPaneModel->index (aRowId, 5) << myEvolutionPaneModel->index (aRowId, 6);
        myHelperExport.AddShape (anOldShape, anIndices);
      }
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

  if (aSelectedIndex.column() == 3)
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
  if (aSelectedIndex.column() != 3)
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
                                                          myEvolutionTableView->GetTableView(), 5);

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
