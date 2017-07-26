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

#include <DFBrowserPane_TDocStdOwner.hxx>

#include <DFBrowserPane_AttributePaneModel.hxx>
#include <DFBrowserPane_TableView.hxx>
#include <DFBrowserPane_Tools.hxx>

#include <TDocStd_Document.hxx>
#include <TDocStd_Owner.hxx>
#include <TDF_Delta.hxx>
#include <TDF_ListIteratorOfDeltaList.hxx>

#include <QVariant>
#include <QWidget>

#include <QGridLayout>
#include <QGroupBox>
#include <QTableView>

//#define REQUIRE_OCAF_REVIEW:12
// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_TDocStdOwner::DFBrowserPane_TDocStdOwner()
 : DFBrowserPane_AttributePane()
{
  getPaneModel()->SetColumnCount (2);

  QList<QVariant> theValues;
  theValues << "Method" << "Value";
  getPaneModel()->SetHeaderValues (theValues, Qt::Horizontal);
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPane_TDocStdOwner::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TDocStd_Owner) anAttribute = Handle(TDocStd_Owner)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  Handle(TDocStd_Document) aDocument = anAttribute->GetDocument();
  if (aDocument.IsNull())
    return;

  theValues << STANDARD_TYPE (TDocStd_Document)->Name() << ""
            << "IsSaved" << DFBrowserPane_Tools::BoolToStr (aDocument->IsSaved())
            << "IsChanged" << DFBrowserPane_Tools::BoolToStr (aDocument->IsChanged())
            << "GetSavedTime" << DFBrowserPane_Tools::BoolToStr (aDocument->GetSavedTime())
            << "GetName" << (aDocument->IsSaved() ? DFBrowserPane_Tools::ToString (aDocument->GetName()) : "")
            << "GetPath" << (aDocument->IsSaved() ? DFBrowserPane_Tools::ToString (aDocument->GetPath()) : "")
            << "Main" << DFBrowserPane_Tools::GetEntry (aDocument->Main()).ToCString()
            << "IsEmpty" << DFBrowserPane_Tools::BoolToStr (aDocument->IsEmpty())
            << "IsValid" << DFBrowserPane_Tools::BoolToStr (aDocument->IsValid())
            << "HasOpenCommand" << DFBrowserPane_Tools::BoolToStr (aDocument->HasOpenCommand())
            << "GetUndoLimit" << QString::number (aDocument->GetUndoLimit())
            << "GetAvailableUndos" << QString::number (aDocument->GetAvailableUndos())
            << "GetUndos" << convertToString (aDocument->GetUndos())
            << "GetAvailableRedos" << QString::number (aDocument->GetAvailableRedos())
            << "GetRedos" << convertToString (aDocument->GetRedos())
            << "StorageFormat" << DFBrowserPane_Tools::ToString (aDocument->StorageFormat())
            << "IsNestedTransactionMode" << DFBrowserPane_Tools::BoolToStr (aDocument->IsNestedTransactionMode())
            << "ModificationMode" << DFBrowserPane_Tools::BoolToStr (aDocument->ModificationMode())
            << STANDARD_TYPE (CDM_Document)->Name() << "";

  TColStd_SequenceOfExtendedString anExtensions;
  aDocument->Extensions(anExtensions);
  theValues << "Extensions" << convertToString (anExtensions)
            << "FromReferencesNumber" << QString::number (aDocument->FromReferencesNumber())
            << "ToReferencesNumber" << QString::number (aDocument->ToReferencesNumber())
            << "IsReadOnly" << DFBrowserPane_Tools::BoolToStr (aDocument->IsReadOnly())
            << "Modifications" << QString::number (aDocument->Modifications());
  TColStd_SequenceOfExtendedString aComments;
  aDocument->Extensions(aComments);

  if (!aDocument->IsOpened())
    return; // application is empty, an exception is sent in Application(),
            // an exception is sent in FindDescription if the application is not defined

  theValues << "Comments" << convertToString (aComments)
            << "Presentation" << ""
            << "IsStored" << DFBrowserPane_Tools::BoolToStr (aDocument->IsStored())
            << "StorageVersion" << QString::number (aDocument->StorageVersion())
            << "Folder" << (aDocument->IsStored() ? DFBrowserPane_Tools::ToString (aDocument->Folder()) : "")
            << "HasRequestedFolder" << DFBrowserPane_Tools::BoolToStr (aDocument->HasRequestedFolder())
            << "RequestedFolder" << (aDocument->HasRequestedFolder() ? DFBrowserPane_Tools::ToString (aDocument->RequestedFolder()) : "")
            << "RequestedName" << DFBrowserPane_Tools::ToString (aDocument->RequestedName())
            << "HasRequestedPreviousVersion" << DFBrowserPane_Tools::BoolToStr (aDocument->HasRequestedPreviousVersion())
            << "RequestedPreviousVersion" << (aDocument->HasRequestedPreviousVersion() ?
                                              DFBrowserPane_Tools::ToString (aDocument->RequestedPreviousVersion()) : "")
            << "RequestedComment" << DFBrowserPane_Tools::ToString (aDocument->RequestedComment())
            << "FindFileExtension" << DFBrowserPane_Tools::BoolToStr (aDocument->FindFileExtension())
            << "FileExtension" << DFBrowserPane_Tools::ToString (aDocument->FileExtension())
            << "FindDataType" << "" // DFBrowserPane_Tools::BoolToStr (aDocument->FindDataType())
            << "DataType" << ""// DFBrowserPane_Tools::ToString (aDocument->DataType())
            << "FindVersionDataType" << ""// DFBrowserPane_Tools::BoolToStr (aDocument->FindVersionDataType())

            << "VersionDataType" << ""// DFBrowserPane_Tools::ToString (aDocument->VersionDataType())
            << "FindDescription" << DFBrowserPane_Tools::BoolToStr (aDocument->FindDescription())
            << "Description" << DFBrowserPane_Tools::ToString (aDocument->Description())
            << "FindDomain" << ""// DFBrowserPane_Tools::BoolToStr (aDocument->FindDomain())
            << "Domain" << ""// DFBrowserPane_Tools::ToString (aDocument->Domain())
            << "FindStoragePlugin" << ""//  DFBrowserPane_Tools::BoolToStr (aDocument->FindStoragePlugin())
            << "StoragePlugin" << ""//  DFBrowserPane_Tools::ToString (aDocument->StoragePlugin())
            << "IsModified" << DFBrowserPane_Tools::BoolToStr (aDocument->IsModified())
            << "IsOpened" << DFBrowserPane_Tools::BoolToStr (aDocument->IsOpened())
            << "CanClose" << DFBrowserPane_Tools::BoolToStr (aDocument->CanClose())
            << "ReferenceCounter" << QString::number (aDocument->ReferenceCounter());

  // TODO IsInSession, IsStored, Name, IsReadOnly, IsUpToDate, CanCloseReference, IsOpened
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPane_TDocStdOwner::GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                        QList<QVariant>& theValues)
{
  Handle(TDocStd_Owner) anAttribute = Handle(TDocStd_Owner)::DownCast (theAttribute);
  if (anAttribute.IsNull())
    return;

  Handle(TDocStd_Document) aDocument = anAttribute->GetDocument();
  if (aDocument.IsNull())
    return;

  theValues.append (DFBrowserPane_Tools::ToString (aDocument->StorageFormat()));
}

// =======================================================================
// function : convertToString
// purpose :
// =======================================================================
QString DFBrowserPane_TDocStdOwner::convertToString (const TDF_DeltaList& theDeltaList)
{
  QStringList aNames;

  for (TDF_ListIteratorOfDeltaList aDeltaIt (theDeltaList); aDeltaIt.More(); aDeltaIt.Next())
  {
    QString aName = DFBrowserPane_Tools::ToString (aDeltaIt.Value()->Name());
    aNames.append (aName.isEmpty() ? "-" : aName);
  }
  return QString ("[%1]").arg (aNames.join (";"));
}

// =======================================================================
// function : convertToString
// purpose :
// =======================================================================
QString DFBrowserPane_TDocStdOwner::convertToString (const TColStd_SequenceOfExtendedString& theExtensions)
{
  QStringList aNames;
  for (Standard_Integer anExtensionId = 1, aNumber = theExtensions.Length(); anExtensionId <= aNumber; anExtensionId++)
    aNames.append (DFBrowserPane_Tools::ToString (theExtensions(anExtensionId)));

  return QString ("[%1]").arg (aNames.join (";"));
}
