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

#include <DFBrowserPane_TNamingNaming.hxx>

#include <AIS_Shape.hxx>
#include <BRep_Builder.hxx>

#include <DFBrowserPane_AttributePaneModel.hxx>
#include <DFBrowserPane_TableView.hxx>
#include <DFBrowserPane_Tools.hxx>

#include <TDF_Label.hxx>
#include <TNaming_ListIteratorOfListOfNamedShape.hxx>
#include <TNaming_Name.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamedShape.hxx>

#include <TopoDS_Compound.hxx>

#include <QGridLayout>
#include <QHeaderView>
#include <QTableView>
#include <QVariant>
#include <QWidget>

//#define REQUIRE_OCAF_REVIEW:14
// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_TNamingNaming::DFBrowserPane_TNamingNaming()
: DFBrowserPane_AttributePane(), myNamingView (0)
{
  myNamingModel = new DFBrowserPane_AttributePaneModel();

  QList<QVariant> aHeaderValues;
  aHeaderValues << "Type" << "Shape Type" << "Stop Named Shape" << "Index";
  aHeaderValues << "Key_TShape" << "Context Label" << "Orientation";
  myNamingModel->SetHeaderValues (aHeaderValues, Qt::Vertical);

  aHeaderValues.clear();
  aHeaderValues << "Arguments";
  getPaneModel()->SetHeaderValues (aHeaderValues, Qt::Horizontal);
}

// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
QWidget* DFBrowserPane_TNamingNaming::CreateWidget (QWidget* theParent)
{
  QWidget* aMainWidget = new QWidget (theParent);

  myNamingView = new DFBrowserPane_TableView (aMainWidget);
  myNamingView->GetTableView()->verticalHeader()->setVisible (true);
  myNamingView->GetTableView()->horizontalHeader()->setVisible (false);
  myNamingView->SetModel (myNamingModel);

  myTableView = new DFBrowserPane_TableView (aMainWidget);
  myTableView->SetModel (getPaneModel());
  myTableView->GetTableView()->setSelectionModel (mySelectionModels.front());

  QGridLayout* aLay = new QGridLayout (aMainWidget);
  aLay->addWidget (myNamingView);
  aLay->addWidget (myTableView);
  aLay->setRowStretch (1, 1);

  return aMainWidget;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPane_TNamingNaming::Init (const Handle(TDF_Attribute)& theAttribute)
{
  QList<QVariant> aValues;
  GetValues (theAttribute, aValues);

  QList<QVariant> aNamingValues;
  for (int anValueId = 0; anValueId < 7; anValueId++)
    aNamingValues.append (aValues[anValueId]);
  myNamingModel->Init (aNamingValues);

  QList<QVariant> aNamedShapesValues;
  for (int anValueId = 7, aCount = aValues.size(); anValueId < aCount; anValueId++)
    aNamedShapesValues.append (aValues[anValueId]);

  getPaneModel()->Init (aNamedShapesValues);
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TNamingNaming::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TNaming_Naming) anAttribute = Handle(TNaming_Naming)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  TNaming_Name aNamingName = anAttribute->GetName();

  // values from 0-6
  theValues.append (DFBrowserPane_Tools::ToName (DB_NAMING_TYPE, aNamingName.Type()).ToCString());
  theValues.append (DFBrowserPane_Tools::ToName (DB_SHAPE_TYPE, aNamingName.ShapeType()).ToCString());
  Handle(TNaming_NamedShape) aStopShape = aNamingName.StopNamedShape();
  theValues.append (!aStopShape.IsNull() ? DFBrowserPane_Tools::GetEntry (aStopShape->Label()).ToCString() : "");
  theValues.append (QString::number (aNamingName.Index()));
  TopoDS_Shape aShape = aNamingName.Shape();
  theValues.append (!aShape.IsNull() ? DFBrowserPane_Tools::GetPointerInfo (aShape.TShape()->This()).ToCString() : "");
  TDF_Label aContextLabel = aNamingName.ContextLabel();
  theValues.append (!aContextLabel.IsNull() ? DFBrowserPane_Tools::GetEntry (aContextLabel).ToCString() : "");
  theValues.append (DFBrowserPane_Tools::ToName (DB_ORIENTATION_TYPE, aNamingName.Orientation()).ToCString());

  // values from 7 till count of arguments
  for (TNaming_ListIteratorOfListOfNamedShape anArgIt(aNamingName.Arguments()); anArgIt.More(); anArgIt.Next())
    theValues.append (DFBrowserPane_Tools::GetEntry (anArgIt.Value()->Label()).ToCString());
}

// =======================================================================
// function : GetPresentation
// purpose :
// =======================================================================
Handle(Standard_Transient) DFBrowserPane_TNamingNaming::GetPresentation (const Handle (TDF_Attribute)& theAttribute)
{
  Handle(Standard_Transient) aPresentation;
  Handle(TNaming_Naming) anAttribute = Handle(TNaming_Naming)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return aPresentation;

  DFBrowserPane_TableView* aTableView = getTableView();
  if (!aTableView) // the pane is not visualized yet
    return aPresentation;

  QStringList aSelectedEntries = DFBrowserPane_TableView::GetSelectedColumnValues (aTableView->GetTableView(), 0);
  TNaming_Name aNamingName = anAttribute->GetName();

  BRep_Builder aBuilder;
  TopoDS_Compound aComp;
  aBuilder.MakeCompound (aComp);
  bool aHasShapes = false;
  for (TNaming_ListIteratorOfListOfNamedShape aNamingIt(aNamingName.Arguments()); aNamingIt.More(); aNamingIt.Next())
  {
    Handle(TNaming_NamedShape) aShapeAttr = aNamingIt.Value();
    if (aShapeAttr.IsNull())
      continue;
    TDF_Label aLabel = aShapeAttr->Label();
    if (!aSelectedEntries.contains (DFBrowserPane_Tools::GetEntry (aLabel).ToCString()))
      continue;
    aBuilder.Add (aComp, aShapeAttr->Get());
    aHasShapes = true;
  }
  TopoDS_Shape aShape = aComp;
  if (!aShape.IsNull() && aHasShapes)
    aPresentation = new AIS_Shape (aShape);

  return aPresentation;
}

// =======================================================================
// function : GetReferences
// purpose :
// =======================================================================
void DFBrowserPane_TNamingNaming::GetReferences (const Handle(TDF_Attribute)& theAttribute,
                                                 NCollection_List<TDF_Label>& theRefLabels,
                                                 Handle(Standard_Transient)&)
{
  Handle(TNaming_Naming) anAttribute = Handle(TNaming_Naming)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  QStringList aSelectedEntries = DFBrowserPane_TableView::GetSelectedColumnValues (getTableView()->GetTableView(), 0);
  for (TNaming_ListIteratorOfListOfNamedShape aNamingIt(anAttribute->GetName().Arguments()); aNamingIt.More(); aNamingIt.Next())
  {
    Handle(TNaming_NamedShape) aShapeAttr = aNamingIt.Value();
    if (aShapeAttr.IsNull())
      continue;
    TDF_Label aLabel = aShapeAttr->Label();
    if (aSelectedEntries.contains (DFBrowserPane_Tools::GetEntry (aLabel).ToCString()))
      theRefLabels.Append (aLabel);
  }
}
