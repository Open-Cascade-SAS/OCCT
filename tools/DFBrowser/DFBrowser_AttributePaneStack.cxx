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

#include <inspector/DFBrowser_AttributePaneStack.hxx>

#include <inspector/DFBrowserPane_AttributePaneAPI.hxx>
#include <inspector/DFBrowserPane_AttributePaneSelector.hxx>
#include <inspector/DFBrowser_Item.hxx>
#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_Tools.hxx>
#include <inspector/DFBrowser_TreeLevelView.hxx>
#include <inspector/DFBrowser_TreeModel.hxx>
#include <inspector/DFBrowser_SearchView.hxx>
#include <inspector/DFBrowser_Window.hxx>

#include <TDF_Attribute.hxx>

#include <QItemSelectionModel>
#include <QMap>
#include <QStackedWidget>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_AttributePaneStack::DFBrowser_AttributePaneStack (QObject* theParent)
: QObject (theParent), myCurrentPane (0), myAttributesStack (0), myModule (0), myTreeLevelView (0),
  mySearchView (0), myEmptyWidget (0), myPaneMode (DFBrowser_AttributePaneType_ItemView)
{
  myPaneSelector = new DFBrowserPane_AttributePaneSelector (theParent);
}

// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
void DFBrowser_AttributePaneStack::CreateWidget (QWidget* theParent)
{
  myAttributesStack = new QStackedWidget (theParent);
  DFBrowser_Window::SetWhiteBackground (myAttributesStack);
  myEmptyWidget = new QWidget (theParent);
  DFBrowser_Window::SetWhiteBackground (myEmptyWidget);

  myAttributesStack->addWidget (myEmptyWidget);

  myTreeLevelView = new DFBrowser_TreeLevelView (theParent);
  myAttributesStack->addWidget (myTreeLevelView->GetControl());

  mySearchView = new DFBrowser_SearchView (theParent);
  myAttributesStack->addWidget (mySearchView->GetControl());

  myAttributesStack->setCurrentWidget (myEmptyWidget);
}

// =======================================================================
// function : SetPaneMode
// purpose :
// =======================================================================
void DFBrowser_AttributePaneStack::SetPaneMode (const DFBrowser_AttributePaneType& theMode)
{
  if (myPaneMode == theMode)
    return;

  myPaneMode = theMode;
  if (myPaneMode == DFBrowser_AttributePaneType_SearchView)
  {
    //! clear highlight in tree model
    DFBrowser_TreeModel* aModel = dynamic_cast<DFBrowser_TreeModel*> (myModule->GetOCAFViewModel());
    if (aModel && aModel->HasHighlighted())
      aModel->SetHighlighted (QModelIndexList());
    myAttributesStack->setCurrentWidget (mySearchView->GetControl());
  }
  else
  {
    QItemSelectionModel* aSelectionModel = myModule->GetOCAFViewSelectionModel();
    QModelIndex anIndex = DFBrowser_Window::SingleSelected (aSelectionModel->selectedIndexes(), 0);
    SetCurrentItem (anIndex);
  }
}

// =======================================================================
// function : SetCurrentItem
// purpose :
// =======================================================================
void DFBrowser_AttributePaneStack::SetCurrentItem (const QModelIndex& theIndex)
{
  if (myPaneMode != DFBrowser_AttributePaneType_ItemView)
    return;

  //! clear highlight in tree model
  DFBrowser_TreeModel* aModel = dynamic_cast<DFBrowser_TreeModel*> (myModule->GetOCAFViewModel());
  if (aModel && aModel->HasHighlighted())
    aModel->SetHighlighted (QModelIndexList());

  myCurrentPane = 0;
  QWidget* aWidget = 0;
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (theIndex);
  if (!anItemBase)
    return;

  if (DFBrowser_TreeLevelView::ProcessItem (theIndex))
    aWidget = myTreeLevelView->GetControl();
  else
  {
    DFBrowser_ItemPtr anItem = itemDynamicCast<DFBrowser_Item> (anItemBase);
    if (!anItem)
      return;

    if (myAttributesStack->currentWidget() == myTreeLevelView->GetControl())
      myTreeLevelView->ClearSelection();
    Handle(TDF_Attribute) anAttribute = anItem->GetAttribute();
    myCurrentPane = myModule->GetAttributePane (anAttribute);
    
    if (myCurrentPane)
    {
      aWidget = myCurrentPane->GetWidget (myAttributesStack, true);
      if (aWidget)
      {
        int aWidgetIndex = myAttributesStack->indexOf (aWidget);
        if (aWidgetIndex < 0)
          myAttributesStack->addWidget (aWidget);
      }
      myCurrentPane->Init (anAttribute);

      std::list<QItemSelectionModel*> aSelectionModels = myCurrentPane->GetSelectionModels();
      myPaneSelector->SetCurrentSelectionModels (aSelectionModels);
    }
  }
  myAttributesStack->setCurrentWidget (aWidget != NULL ? aWidget : myEmptyWidget);
}
