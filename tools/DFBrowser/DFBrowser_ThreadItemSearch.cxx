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

#include <inspector/DFBrowser_ThreadItemSearch.hxx>

#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_Tools.hxx>
#include <inspector/DFBrowser_TreeModel.hxx>
#include <inspector/DFBrowser_SearchLine.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TDataStd_Comment.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>

#include <QDir>

// =======================================================================
// function : Run
// purpose :
// =======================================================================
void DFBrowser_ThreadItemSearch::Run()
{
  DFBrowser_TreeModel* aModel = dynamic_cast<DFBrowser_TreeModel*> (mySearchLine->GetModule()->GetOCAFViewModel());
  Handle(TDocStd_Application) anApplication = aModel->GetTDocStdApplication();
  if (anApplication.IsNull())
    return;

  myDocumentValues.clear();
  myDocumentInfoValues.clear();

  QMap<QString, DFBrowser_SearchItemInfo> anAdditionalValues;
  QStringList anInfoValues;
  QStringList aCurrentPath;
  for (Standard_Integer aDocId = 1, aNbDoc = anApplication->NbDocuments(); aDocId <= aNbDoc; aDocId++)
  {
    Handle(TDocStd_Document) aDocument;
    anApplication->GetDocument (aDocId, aDocument);
    if (aDocument.IsNull())
      continue;

    anAdditionalValues.clear();
    anInfoValues.clear();
    aCurrentPath.clear();
    getLabelLines (aDocument->Main().Root(), aCurrentPath, anAdditionalValues, anInfoValues);

    myDocumentValues[aDocId] = anAdditionalValues;
    myDocumentInfoValues[aDocId] = anInfoValues;
  }
}

// =======================================================================
// function : ApplyValues
// purpose :
// =======================================================================
void DFBrowser_ThreadItemSearch::ApplyValues()
{
  mySearchLine->SetValues (myDocumentValues, myDocumentInfoValues);
}

// =======================================================================
// function : ClearValues
// purpose :
// =======================================================================
void DFBrowser_ThreadItemSearch::ClearValues (DFBrowser_SearchLine* theSearchLine)
{
  theSearchLine->ClearValues();
}

// =======================================================================
// function : getLabelLines
// purpose :
// =======================================================================
void DFBrowser_ThreadItemSearch::getLabelLines (const TDF_Label& theLabel, QStringList& theCurrentPath,
                                                QMap<QString, DFBrowser_SearchItemInfo >& theValues,
                                                QStringList& theInfoValues)
{
  addLabel (theLabel, theCurrentPath, theValues, theInfoValues);
  theCurrentPath.append (DFBrowserPane_Tools::GetEntry (theLabel).ToCString());

  int anId = 0;
  for (TDF_AttributeIterator anAttrIt (theLabel); anAttrIt.More(); anAttrIt.Next(), anId++)
    addAttribute(anAttrIt.Value(), theCurrentPath, theValues, theInfoValues);

  for (TDF_ChildIterator aChildIt (theLabel); aChildIt.More(); aChildIt.Next())
    getLabelLines(aChildIt.Value(), theCurrentPath, theValues, theInfoValues);

  theCurrentPath.removeLast();
}

// =======================================================================
// function : addLabel
// purpose :
// =======================================================================
void DFBrowser_ThreadItemSearch::addLabel (const TDF_Label& theLabel, const QStringList& theCurrentPath,
                                           QMap<QString, DFBrowser_SearchItemInfo>& theValues,
                                           QStringList& theInfoValues)
{
  QString anEntry = DFBrowserPane_Tools::GetEntry (theLabel).ToCString();
  if (!theValues.contains (anEntry))
  {
    theInfoValues.append (anEntry);
    theValues[anEntry] = DFBrowser_SearchItemInfo ("40x40_label_icon", anEntry, theCurrentPath, QDir::separator());
  }
}

// =======================================================================
// function : addAttribute
// purpose :
// =======================================================================
void DFBrowser_ThreadItemSearch::addAttribute (const Handle(TDF_Attribute)& theAttribute,
                                               const QStringList& theCurrentPath,
                                               QMap<QString, DFBrowser_SearchItemInfo >& theValues,
                                               QStringList& theInfoValues)
{
  Standard_CString anAttributeKind = theAttribute->DynamicType()->Name();
  // add element of attribute kind
  QString anAttributeName = QString ("%1%2%3").arg (anAttributeKind)
                            .arg (DFBrowser_SearchLineModel::SplitSeparator())
                            .arg (DFBrowserPane_Tools::GetEntry (theAttribute->Label()).ToCString());

  if (!theInfoValues.contains (anAttributeName))
  {
    theInfoValues.append (anAttributeName);
    theValues[anAttributeName] = DFBrowser_SearchItemInfo (QVariant(), anAttributeName, theCurrentPath,
                                                           QDir::separator());
  }

  // add element of attribute value, e.g. Name or Comment string
  QString anAttributeValue;
  if (anAttributeKind == STANDARD_TYPE (TDataStd_Name)->Name())
  {
    Handle(TDataStd_Name) anAttribute = Handle(TDataStd_Name)::DownCast (theAttribute);
    anAttributeValue = DFBrowserPane_Tools::ToString (anAttribute->Get());
  }
  else if (anAttributeKind == STANDARD_TYPE (TDataStd_Comment)->Name())
  {
    Handle(TDataStd_Comment) anAttribute = Handle(TDataStd_Comment)::DownCast (theAttribute);
    anAttributeValue = DFBrowserPane_Tools::ToString (anAttribute->Get());
  }
  else
    return;

  if (anAttributeValue == "")
    return;

  // using attribute value in the name
  QString anAttributeValueExt = QString ("%1%2%3").arg (anAttributeValue)
                                          .arg (DFBrowser_SearchLineModel::SplitSeparator())
                                          .arg (DFBrowserPane_Tools::GetEntry (theAttribute->Label()).ToCString());
  if (!theInfoValues.contains (anAttributeValueExt))
  {
    theInfoValues.append (anAttributeValueExt);
    theValues[anAttributeValueExt] = DFBrowser_SearchItemInfo (QVariant(), anAttributeValueExt,
                                                               theCurrentPath, QDir::separator());
  }
}
