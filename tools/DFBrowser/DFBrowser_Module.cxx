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

#include <inspector/DFBrowser_Module.hxx>

#include <inspector/DFBrowser_Item.hxx>
#include <inspector/DFBrowser_ItemApplication.hxx>
#include <inspector/DFBrowser_ItemBase.hxx>
#include <inspector/DFBrowser_ItemRole.hxx>
#include <inspector/DFBrowser_Tools.hxx>
#include <inspector/DFBrowser_TreeModel.hxx>

#include <inspector/DFBrowserPane_AttributePane.hxx>
#include <inspector/DFBrowserPane_AttributePaneCreator.hxx>
#include <inspector/DFBrowserPane_ItemRole.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <inspector/DFBrowserPaneXDE_AttributePaneCreator.hxx>
#include <inspector/DFBrowserPaneXDE_Tools.hxx>

#include <XCAFApp_Application.hxx>

#include <QItemSelectionModel>
#include <QMessageBox>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_Module::DFBrowser_Module()
: myOCAFViewModel (0)
{
  RegisterPaneCreator (new DFBrowserPane_AttributePaneCreator());
}

// =======================================================================
// function : CreateViewModel
// purpose :
// =======================================================================
void DFBrowser_Module::CreateViewModel (void* theParent)
{
  myOCAFViewModel = new DFBrowser_TreeModel ((QWidget*)theParent, this);
}

// =======================================================================
// function : SetApplication
// purpose :
// =======================================================================
void DFBrowser_Module::SetApplication (const Handle(TDocStd_Application)& theApplication)
{
  myOCAFViewModel->Init (theApplication);

  myPaneCreators.clear();
  RegisterPaneCreator (new DFBrowserPane_AttributePaneCreator());
  if (!theApplication.IsNull() && DFBrowserPaneXDE_Tools::IsXDEApplication (theApplication))
    RegisterPaneCreator (new DFBrowserPaneXDE_AttributePaneCreator (myPaneCreators[0]));
}

// =======================================================================
// function : SetExternalContext
// purpose :
// =======================================================================
void DFBrowser_Module::SetExternalContext (const Handle(Standard_Transient)& theContext)
{
  myExternalContext = Handle(AIS_InteractiveContext)::DownCast (theContext);
}

// =======================================================================
// function : GetTDocStdApplication
// purpose :
// =======================================================================
Handle(TDocStd_Application) DFBrowser_Module::GetTDocStdApplication() const
{
  return myOCAFViewModel->GetTDocStdApplication();
}

// =======================================================================
// function : UpdateTreeModel
// purpose :
// =======================================================================
void DFBrowser_Module::UpdateTreeModel()
{
  QAbstractItemModel* aModel = GetOCAFViewModel();
  QItemSelectionModel* aSelectionModel = GetOCAFViewSelectionModel();
  if (!aModel || !aSelectionModel)
    return;
  aSelectionModel->clearSelection();

  emit beforeUpdateTreeModel();
  myOCAFViewModel->Reset();
  myOCAFViewModel->EmitLayoutChanged();

  SetInitialTreeViewSelection();
}

// =======================================================================
// function : SetInitialTreeViewSelection
// purpose :
// =======================================================================
void DFBrowser_Module::SetInitialTreeViewSelection()
{
  QAbstractItemModel* aModel = GetOCAFViewModel();
  QItemSelectionModel* aSelectionModel = GetOCAFViewSelectionModel();
  if (!aModel || !aSelectionModel)
    return;

  // select a parent(aplication) item
  aSelectionModel->select (aModel->index (0, 0), QItemSelectionModel::ClearAndSelect);
}

// =======================================================================
// function : FindAttribute
// purpose :
// =======================================================================
Handle(TDF_Attribute) DFBrowser_Module::FindAttribute (const QModelIndex& theIndex)
{
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (theIndex);
  if (!anItemBase)
    return Handle(TDF_Attribute)();

  DFBrowser_ItemPtr anItem = itemDynamicCast<DFBrowser_Item> (anItemBase);
  return (anItem && anItem->HasAttribute()) ? anItem->GetAttribute() : Handle(TDF_Attribute)();
}

// =======================================================================
// function : GetAttributePane
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneAPI* DFBrowser_Module::GetAttributePane (Handle(TDF_Attribute) theAttribute)
{
  DFBrowserPane_AttributePaneAPI* aPane = 0;
  if (theAttribute.IsNull())
    return aPane;
  return GetAttributePane (theAttribute->DynamicType()->Name());
}

// =======================================================================
// function : GetAttributePane
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneAPI* DFBrowser_Module::GetAttributePane (const Standard_CString& theAttributeName)
{
  DFBrowserPane_AttributePaneAPI* aPane = 0;

  if (!myAttributeTypes.contains (theAttributeName))
  {
    aPane = CreateAttributePane (theAttributeName);
    if (aPane)
      myAttributeTypes[theAttributeName] = aPane;
  }
  else
    aPane = myAttributeTypes[theAttributeName];

  return aPane;
}

// =======================================================================
// function : GetAttributeInfo
// purpose :
// =======================================================================
QVariant DFBrowser_Module::GetAttributeInfo (Handle(TDF_Attribute) theAttribute, DFBrowser_Module* theModule,
                                             int theRole, int theColumnId)
{
  DFBrowserPane_AttributePane* anAttributePane = 0;
  if (!theAttribute.IsNull())
  {
    DFBrowserPane_AttributePaneAPI* anAPIPane = theModule->GetAttributePane (theAttribute);
    if (anAPIPane)
      anAttributePane = dynamic_cast<DFBrowserPane_AttributePane*> (anAPIPane);
  }

  QVariant aValue;
  if (anAttributePane)
    aValue = anAttributePane->GetAttributeInfo (theAttribute,
               theRole == DFBrowser_ItemRole_AdditionalInfo ? DFBrowserPane_ItemRole_ShortInfo : theRole,
               theColumnId);
  else
    aValue = DFBrowserPane_AttributePane::GetAttributeInfoByType (theAttribute->DynamicType()->Name(), theRole, theColumnId);
  return aValue;
}

// =======================================================================
// function : GetAttributeInfo
// purpose :
// =======================================================================
QVariant DFBrowser_Module::GetAttributeInfo (const Standard_CString& theAttributeName, DFBrowser_Module* theModule,
                                             int theRole, int theColumnId)
{
  DFBrowserPane_AttributePane* anAttributePane = 0;
  DFBrowserPane_AttributePaneAPI* anAPIPane = theModule->GetAttributePane (theAttributeName);
  if (anAPIPane)
    anAttributePane = dynamic_cast<DFBrowserPane_AttributePane*> (anAPIPane);

  QVariant aValue;
  if (anAttributePane)
  {
    Handle(TDF_Attribute) anAttribute;
    aValue = anAttributePane->GetAttributeInfo (anAttribute,
               theRole == DFBrowser_ItemRole_AdditionalInfo ? DFBrowserPane_ItemRole_ShortInfo : theRole, theColumnId);
  }
  else
    aValue = DFBrowserPane_AttributePane::GetAttributeInfoByType (theAttributeName, theRole, theColumnId);
  return aValue;

}

// =======================================================================
// function : CreateAttributePane
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneAPI* DFBrowser_Module::CreateAttributePane (const Standard_CString& theAttributeName)
{
  DFBrowserPane_AttributePaneAPI* aPane = 0;
  // iteration should be performed from the tail of the list, as latest added creator has
  // larger priority
  for (int aPaneCreatorId = myPaneCreators.size()-1; aPaneCreatorId >= 0 && !aPane; aPaneCreatorId--)
    aPane = myPaneCreators[aPaneCreatorId]->CreateAttributePane (theAttributeName);
  return aPane;
}
