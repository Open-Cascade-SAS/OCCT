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

#include <inspector/DFBrowserPane_TDataStdReferenceArray.hxx>

#include <inspector/DFBrowserPane_AttributePaneModel.hxx>
#include <inspector/DFBrowserPane_HelperArray.hxx>
#include <inspector/DFBrowserPane_TableView.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TDataStd_ReferenceArray.hxx>

#include <QGridLayout>
#include <QItemSelectionModel>
#include <QTableView>
#include <QVariant>
#include <QWidget>

// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
QWidget* DFBrowserPane_TDataStdReferenceArray::CreateWidget (QWidget* theParent)
{
  QWidget* aMainWidget = new QWidget (theParent);
  myTableView = new DFBrowserPane_TableView (aMainWidget);
  myTableView->SetModel (getPaneModel());
  myTableView->GetTableView()->setSelectionModel (mySelectionModels.front());

  myArrayTableHelper.CreateWidget (aMainWidget, myTableView);

  return aMainWidget;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdReferenceArray::Init (const Handle(TDF_Attribute)& theAttribute)
{
  QList<QVariant> aValues;
  GetValues (theAttribute, aValues);
  myArrayTableHelper.Init (aValues);
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdReferenceArray::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TDataStd_ReferenceArray) anAttribute = Handle(TDataStd_ReferenceArray)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  theValues.append (anAttribute->Lower());
  theValues.append (anAttribute->Upper());
  for (int aValueId = anAttribute->Lower(); aValueId <= anAttribute->Upper(); aValueId++)
    theValues.append (DFBrowserPane_Tools::GetEntry (anAttribute->Value(aValueId)).ToCString());
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdReferenceArray::GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                                  QList<QVariant>& theValues)
{
  QList<QVariant> aValues;
  GetValues (theAttribute, aValues);
  myArrayTableHelper.Init (aValues);
  return myArrayTableHelper.GetShortAttributeInfo (theAttribute, theValues);
}

// =======================================================================
// function : GetReferences
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdReferenceArray::GetReferences (const Handle(TDF_Attribute)& theAttribute,
                                                          NCollection_List<TDF_Label>& theRefLabels,
                                                          Handle(Standard_Transient)& /*theRefPresentation*/)
{
  if (!getTableView())
    return;
  QStringList aSelectedEntries = DFBrowserPane_TableView::GetSelectedColumnValues (getTableView()->GetTableView(), 1);

  Handle(TDataStd_ReferenceArray) anAttribute = Handle(TDataStd_ReferenceArray)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  for (int aValueId = anAttribute->Lower(); aValueId <= anAttribute->Upper(); aValueId++)
  {
    TDF_Label aLabel = anAttribute->Value (aValueId);
    if (aSelectedEntries.contains (DFBrowserPane_Tools::GetEntry (aLabel).ToCString()))
      theRefLabels.Append (aLabel);
  }
}
