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

#include <inspector/DFBrowser_ThreadItemUsedShapesMap.hxx>

#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_Tools.hxx>
#include <inspector/DFBrowser_TreeModel.hxx>

#include <inspector/DFBrowserPane_TNamingUsedShapes.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TNaming_DataMapIteratorOfDataMapOfShapePtrRefShape.hxx>
#include <TNaming_PtrRefShape.hxx>
#include <TNaming_RefShape.hxx>
#include <TNaming_UsedShapes.hxx>

#include <Standard_Type.hxx>
#include <TDocStd_Document.hxx>

// =======================================================================
// function : Run
// purpose :
// =======================================================================
void DFBrowser_ThreadItemUsedShapesMap::Run()
{
  if (!myModule)
    return;
  DFBrowser_TreeModel* aDFBrowserModel = dynamic_cast<DFBrowser_TreeModel*> (myModule->GetOCAFViewModel());
  if (!aDFBrowserModel)
    return;

  Handle(TDocStd_Application) anApplication = aDFBrowserModel->GetTDocStdApplication();
  if (anApplication.IsNull())
    return;

  for (Standard_Integer aDocId = 1, aNbDocuments = anApplication->NbDocuments(); aDocId <= aNbDocuments; aDocId++)
  {
    Handle(TDocStd_Document) aDocument;
    anApplication->GetDocument (aDocId, aDocument);
    if (aDocument.IsNull())
      continue;

    TDF_Label aLabel = aDocument->Main().Root();

    Handle(TNaming_UsedShapes) anAttribute;
    if (!aLabel.FindAttribute (TNaming_UsedShapes::GetID(), anAttribute))
      continue;

    std::list<TCollection_AsciiString> aReferences;
    findReferences (anAttribute, aReferences);
    if (!aReferences.empty())
      myAttributeRefs.Bind (anAttribute, aReferences);
  }
}

// =======================================================================
// function : ApplyValues
// purpose :
// =======================================================================
void DFBrowser_ThreadItemUsedShapesMap::ApplyValues()
{
 if (myAttributeRefs.IsEmpty())
    return;

  DFBrowserPane_AttributePaneAPI* aPane = myModule->GetAttributePane (STANDARD_TYPE (TNaming_UsedShapes)->Name());
  if (aPane)
  {
    DFBrowserPane_TNamingUsedShapes* aUsedShapesPane = dynamic_cast<DFBrowserPane_TNamingUsedShapes*> (aPane);
    aUsedShapesPane->SetSortedReferences (myAttributeRefs);
  }
  // update
  DFBrowser_TreeModel* aDFBrowserModel = dynamic_cast<DFBrowser_TreeModel*> (myModule->GetOCAFViewModel());
  for (NCollection_DataMap<Handle(TDF_Attribute), std::list<TCollection_AsciiString> >::Iterator aRefIt (myAttributeRefs);
       aRefIt.More(); aRefIt.Next())
  {
    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex
                                                   (aDFBrowserModel->FindIndexByAttribute (aRefIt.Key()));
    if (anItemBase)
    {
      anItemBase->Reset();
      anItemBase->Init();
    }
  }
  // clear cache
  myAttributeRefs.Clear();
}

// =======================================================================
// function : ClearSortedReferences
// purpose :
// =======================================================================
void DFBrowser_ThreadItemUsedShapesMap::ClearSortedReferences (DFBrowser_Module* theModule)
{
  DFBrowserPane_AttributePaneAPI* aPane = theModule->GetAttributePane (STANDARD_TYPE (TNaming_UsedShapes)->Name());
  if (!aPane)
    return;

  DFBrowserPane_TNamingUsedShapes* aUsedShapesPane = dynamic_cast<DFBrowserPane_TNamingUsedShapes*> (aPane);
  aUsedShapesPane->ClearSortedReferences();

  // update tree item state
  DFBrowser_TreeModel* aDFBrowserModel = dynamic_cast<DFBrowser_TreeModel*> (theModule->GetOCAFViewModel());
  if (!aDFBrowserModel)
    return;
  Handle(TDocStd_Application) anApplication = aDFBrowserModel->GetTDocStdApplication();
  if (anApplication.IsNull())
    return;

  for (Standard_Integer aDocId = 1, aNbDocuments = anApplication->NbDocuments(); aDocId <= aNbDocuments; aDocId++)
  {
    Handle(TDocStd_Document) aDocument;
    anApplication->GetDocument (aDocId, aDocument);
    if (aDocument.IsNull())
      continue;
    TDF_Label aLabel = aDocument->Main().Root();
    Handle(TNaming_UsedShapes) anAttribute;
    if (aLabel.FindAttribute (TNaming_UsedShapes::GetID(), anAttribute))
    {
      TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex(
                                                              aDFBrowserModel->FindIndexByAttribute (anAttribute));
      if (anItemBase)
        anItemBase->Init();
    }
  }
}

// =======================================================================
// function : isLessThan
// purpose :
// =======================================================================
bool DFBrowser_ThreadItemUsedShapesMap::isLessThan (const QStringList& theLeft, const QStringList& theRight)
{
  int aState = 1; //! where 0 - less, 1 - equal, 2 - larger
  int aLeftSize = theLeft.size();
  int aRightSize = theRight.size();

  for (int anItemId = 0; anItemId < aRightSize && anItemId < aLeftSize && aState == 1; anItemId++)
  {
    int aRightId = theRight[anItemId].toInt();
    int aLeftId = theLeft[anItemId].toInt();
    if (aLeftId == aRightId)
      continue;
    else if (aLeftId < aRightId)
      aState = 0; // less
    else if (aLeftId > aRightId)
      aState = 2; // less
  }
  if (aState == 1)
  { // equal in similar parts
    if (aLeftSize < aRightSize)
      aState = 0;
    else if (aLeftSize > aRightSize)
      aState = 2;
  }
  return aState == 0;
}

// =======================================================================
// function : addValue
// purpose :
// =======================================================================
void DFBrowser_ThreadItemUsedShapesMap::addValue (const TCollection_AsciiString& theEntry,
                                                  QList<QPair<TCollection_AsciiString, QStringList > >& theEntries)
{
  QStringList aSplit = QString (theEntry.ToCString()).split (":");

  int aLessIndex = -1;
  bool isInserted = false;
  // looking for nearest smaller value from the end of the list
  for (int anEntryId = theEntries.size() - 1; anEntryId >= 0 && !isInserted; anEntryId--)
  {
    if (isLessThan(aSplit, theEntries[anEntryId].second))
      aLessIndex = anEntryId;
    else
    {
      // if less than was found and now, the entry is greater than current entry
      if (aLessIndex != -1)
      {
        theEntries.insert (aLessIndex, qMakePair (theEntry, aSplit));
        isInserted = true;
      }
      break;
    }
  }
  if (!isInserted)
  {
    if (aLessIndex != -1) // less than all, insert at this position
      theEntries.insert (aLessIndex, qMakePair (theEntry, aSplit));
    else
      theEntries.append (qMakePair (theEntry, aSplit));
  }
}

// =======================================================================
// function : findReferences
// purpose :
// =======================================================================
void DFBrowser_ThreadItemUsedShapesMap::findReferences (Handle(TDF_Attribute) theAttribute,
                                                        std::list<TCollection_AsciiString>& theReferences)
{
  Handle(TNaming_UsedShapes) anAttribute = Handle(TNaming_UsedShapes)::DownCast (theAttribute);

  QList<QPair<TCollection_AsciiString, QStringList> > anEntries;
  for (TNaming_DataMapIteratorOfDataMapOfShapePtrRefShape aDataIt(anAttribute->Map()); aDataIt.More(); aDataIt.Next())
    addValue(DFBrowserPane_Tools::GetEntry (aDataIt.Value()->Label()), anEntries);

  for (QList<QPair<TCollection_AsciiString, QStringList> >::const_iterator anEntryIt = anEntries.begin();
       anEntryIt != anEntries.end(); anEntryIt++)
    theReferences.push_back (anEntryIt->first);
}
