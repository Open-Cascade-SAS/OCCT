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

#include <inspector/DFBrowserPane_TDFReference.hxx>

#include <inspector/DFBrowserPane_TableView.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TDF_Reference.hxx>

#include <QVariant>
#include <QTableView>
// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TDFReference::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TDF_Reference) anAttribute = Handle(TDF_Reference)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  TDF_Label aLabel = anAttribute->Get();
  theValues.append ("Get");
  theValues.append (DFBrowserPane_Tools::GetEntry (aLabel).ToCString());
}

// =======================================================================
// function : GetReferences
// purpose :
// =======================================================================
void DFBrowserPane_TDFReference::GetReferences (const Handle(TDF_Attribute)& theAttribute,
                                                NCollection_List<TDF_Label>& theRefLabels,
                                                Handle(Standard_Transient)& /*theRefPresentation*/)
{
  Handle(TDF_Reference) anAttribute = Handle(TDF_Reference)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  QTableView* aTableView = getTableView()->GetTableView();
  QItemSelectionModel* aSelectionModel = aTableView->selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();
  if (aSelectedIndices.size() > 0)
    theRefLabels.Append (anAttribute->Get());
}
