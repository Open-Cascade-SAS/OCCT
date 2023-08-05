// Created on: 2021-04-27
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <inspector/MessageModel_ItemRoot.hxx>
#include <inspector/MessageModel_ItemReport.hxx>

// =======================================================================
// function : SetReport
// purpose :
// =======================================================================
void MessageModel_ItemRoot::SetReport (const int theRowId, const Handle(Message_Report)& theReport,
                                       const TCollection_AsciiString& theReportDescription)
{
  NCollection_List<MessageModel_ReportInformation>::Iterator aReportsIt (myReports);
  for (int aRowId = 0; aReportsIt.More(); aReportsIt.Next(), aRowId++)
  {
    if (aRowId == theRowId)
      break;
  }
  aReportsIt.ChangeValue().myReport = theReport;
  aReportsIt.ChangeValue().myDescription = theReportDescription;
}

// =======================================================================
// function : GetReport
// purpose :
// =======================================================================
const Handle(Message_Report)& MessageModel_ItemRoot::GetReport (const int theRowId,
  TCollection_AsciiString& theReportDescription)
{
  NCollection_List<MessageModel_ReportInformation>::Iterator aReportsIt (myReports);
  for (int aRowId = 0; aReportsIt.More(); aReportsIt.Next(), aRowId++)
  {
    if (aRowId == theRowId)
      break;
  }
  theReportDescription = aReportsIt.Value().myDescription;
  return aReportsIt.Value().myReport;
}

// =======================================================================
// function : HasReport
// purpose :
// =======================================================================
Standard_Boolean MessageModel_ItemRoot::HasReport (const Handle(Message_Report)& theReport)
{
  NCollection_List<MessageModel_ReportInformation>::Iterator aReportsIt (myReports);
  for (int aRowId = 0; aReportsIt.More(); aReportsIt.Next(), aRowId++)
  {
    if (aReportsIt.Value().myReport == theReport)
      return Standard_True;
  }
  return Standard_False;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant MessageModel_ItemRoot::initValue (const int theRole) const
{
  QVariant aParentValue = MessageModel_ItemBase::initValue (theRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (Column() != 0)
    return QVariant();

  if (theRole == Qt::DisplayRole)
    return myName.IsEmpty() ? "Message_Reports" : myName.ToCString();

  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr MessageModel_ItemRoot::createChild (int theRow, int theColumn)
{
  return MessageModel_ItemReport::CreateItem (currentItem(), theRow, theColumn);
}

