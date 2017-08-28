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

#include <inspector/DFBrowserPane_TDataStdReferenceList.hxx>

#include <inspector/DFBrowserPane_TableView.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TDataStd_ReferenceList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>

#include <QVariant>
#include <QWidget>

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdReferenceList::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TDataStd_ReferenceList) anAttribute = Handle(TDataStd_ReferenceList)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  for (TDF_ListIteratorOfLabelList aLabelIt(anAttribute->List()); aLabelIt.More(); aLabelIt.Next())
  {
    theValues.append ("Value");
    theValues.append (DFBrowserPane_Tools::GetEntry (aLabelIt.Value()).ToCString());
  }
}

// =======================================================================
// function : GetReferences
// purpose :
// =======================================================================
void DFBrowserPane_TDataStdReferenceList::GetReferences (const Handle(TDF_Attribute)& theAttribute,
                                                         NCollection_List<TDF_Label>& theRefLabels,
                                                         Handle(Standard_Transient)& /*theRefPresentation*/)
{
  QStringList aSelectedEntries = DFBrowserPane_TableView::GetSelectedColumnValues (getTableView()->GetTableView(), 1);
  Handle(TDataStd_ReferenceList) anAttribute = Handle(TDataStd_ReferenceList)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;
  for (TDF_ListIteratorOfLabelList aLabelIt(anAttribute->List()); aLabelIt.More(); aLabelIt.Next())
  {
    const TDF_Label& aLabel = aLabelIt.Value();
    if (aSelectedEntries.contains (DFBrowserPane_Tools::GetEntry (aLabel).ToCString()))
      theRefLabels.Append (aLabel);
  }
}
