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

#include <inspector/DFBrowser_PropertyPanel.hxx>

#include <inspector/DFBrowser_AttributePaneStack.hxx>
#include <inspector/DFBrowser_Item.hxx>
#include <inspector/DFBrowser_SearchView.hxx>
#include <inspector/DFBrowser_Window.hxx>
#include <inspector/DFBrowser_TreeLevelView.hxx>

#include <QAbstractItemModel>
#include <QGridLayout>
#include <QItemSelectionModel>
#include <QWidget>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_PropertyPanel::DFBrowser_PropertyPanel (QWidget* theParent)
: QObject (theParent), myAttributesStack (0)
{
  myMainWindow = new QWidget (theParent);
  DFBrowser_Window::SetWhiteBackground (myMainWindow);

  QGridLayout* aLayout = new QGridLayout (myMainWindow);
  aLayout->setContentsMargins (0, 0, 0, 0);

  myAttributesStack = new DFBrowser_AttributePaneStack (this);
  myAttributesStack->CreateWidget (myMainWindow);
  aLayout->addWidget (myAttributesStack->GetWidget(), 0, 0);
}

// =======================================================================
// function : UpdateBySelectionChanged
// purpose :
// =======================================================================
void DFBrowser_PropertyPanel::UpdateBySelectionChanged (const QItemSelection& theSelected,
                                                        const QItemSelection&)
{
  GetAttributesStack()->GetSearchView()->Reset();

  QModelIndex anIndex = DFBrowser_Window::SingleSelected (theSelected.indexes(), 0);
  myAttributesStack->SetCurrentItem (anIndex);
}
