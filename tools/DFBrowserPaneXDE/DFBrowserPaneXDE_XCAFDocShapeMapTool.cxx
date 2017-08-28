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

#include <inspector/DFBrowserPaneXDE_XCAFDocShapeMapTool.hxx>

#include <inspector/DFBrowserPane_AttributePaneModel.hxx>
#include <inspector/DFBrowserPane_HelperExport.hxx>
#include <inspector/DFBrowserPane_ItemDelegateButton.hxx>
#include <inspector/DFBrowserPane_TableView.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <AIS_Shape.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>

#include <QEvent>
#include <QFileDialog>
#include <QGridLayout>
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

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPaneXDE_XCAFDocShapeMapTool::DFBrowserPaneXDE_XCAFDocShapeMapTool()
: DFBrowserPane_AttributePane(), myHelperExport (0)
{
  QList<QVariant> aHeaderValues;
  aHeaderValues << "TShape"<< "Type" << "";

  getPaneModel()->SetHeaderValues (aHeaderValues, Qt::Horizontal);
  getPaneModel()->SetColumnCount (aHeaderValues.count());
  getPaneModel()->SetItalicColumns (QList<int>());
}

// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
QWidget* DFBrowserPaneXDE_XCAFDocShapeMapTool::CreateWidget (QWidget* theParent)
{
  QWidget* aMainWidget = new QWidget (theParent);
  aMainWidget->setVisible (false);

  myTableView = new DFBrowserPane_TableView (aMainWidget);
  myTableView->SetModel (myPaneModel);
  QTableView* aTableView = myTableView->GetTableView();
  aTableView->setSelectionModel (mySelectionModels.front());
  aTableView->horizontalHeader()->setStretchLastSection (false);

  aTableView->setColumnWidth (0, COLUMN_POINTER_WIDTH);
  aTableView->setColumnWidth (1, COLUMN_TYPE_WIDTH);
  aTableView->setColumnWidth (2, COLUMN_EXPORT_WIDTH);
  aTableView->verticalHeader()->setVisible (true);
  DFBrowserPane_ItemDelegateButton* anItemDelegate = new DFBrowserPane_ItemDelegateButton (aTableView,
                                                                                           ":/icons/export_shape.png");
  QObject::connect (anItemDelegate, SIGNAL (buttonPressed (const QModelIndex&)),
                    &myHelperExport, SLOT (OnButtonPressed (const QModelIndex&)));
  aTableView->setItemDelegateForColumn (2, anItemDelegate);

  QGridLayout* aLay = new QGridLayout (aMainWidget);
  aLay->setContentsMargins (0, 0, 0, 0);
  aLay->addWidget (myTableView, 0, 0);

  return aMainWidget;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPaneXDE_XCAFDocShapeMapTool::Init (const Handle(TDF_Attribute)& theAttribute)
{
  DFBrowserPane_AttributePaneAPI::Init (theAttribute);
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPaneXDE_XCAFDocShapeMapTool::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(XCAFDoc_ShapeMapTool) anAttr = Handle(XCAFDoc_ShapeMapTool)::DownCast (theAttribute);

  const TopTools_IndexedMapOfShape& aShapeMap = anAttr->GetMap();
  for (int aShapeValueId = 1, aNbShapes = aShapeMap.Extent(); aShapeValueId <= aNbShapes; aShapeValueId++)
  {
    const TopoDS_Shape& aShape = aShapeMap(aShapeValueId);

    if (!aShape.IsNull())
      theValues << DFBrowserPane_Tools::GetPointerInfo (aShape.TShape()->This()).ToCString()
                << DFBrowserPane_Tools::ShapeTypeInfo (aShape)
                << "";
    else
      theValues << "" << "" << "";
  }
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPaneXDE_XCAFDocShapeMapTool::GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                                  QList<QVariant>& theValues)
{
  Handle(XCAFDoc_ShapeMapTool) anAttr = Handle(XCAFDoc_ShapeMapTool)::DownCast (theAttribute);
  theValues.append (QString ("[%1]").arg (anAttr->GetMap().Extent()));
}

// =======================================================================
// function : GetPresentation
// purpose :
// =======================================================================
Handle(Standard_Transient) DFBrowserPaneXDE_XCAFDocShapeMapTool::GetPresentation (const Handle(TDF_Attribute)& theAttribute)
{
  Handle(AIS_InteractiveObject) aPresentation;

  Handle(XCAFDoc_ShapeMapTool) anAttr = Handle(XCAFDoc_ShapeMapTool)::DownCast (theAttribute);
  if (anAttr.IsNull())
    return aPresentation;

  BRep_Builder aBuilder;
  TopoDS_Compound aComp;
  aBuilder.MakeCompound (aComp);

  const TopTools_IndexedMapOfShape& aShapeMap = anAttr->GetMap();
  for (int aShapeValueId = 1, aNbShapes = aShapeMap.Extent(); aShapeValueId <= aNbShapes; aShapeValueId++)
  {
    const TopoDS_Shape& aShape = aShapeMap(aShapeValueId);
    if (!aShape.IsNull())
      aBuilder.Add (aComp, aShape);
  }
  aPresentation = new AIS_Shape (aComp);
  return aPresentation;
}
