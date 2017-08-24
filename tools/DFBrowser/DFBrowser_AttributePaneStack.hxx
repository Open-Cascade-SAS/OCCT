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

#ifndef DFBrowser_AttributePaneStack_H
#define DFBrowser_AttributePaneStack_H

#include <inspector/DFBrowser_AttributePaneType.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard.hxx>

#include <QMap>
#include <QItemSelection>
#include <QStackedWidget>
#include <QWidget>

class DFBrowserPane_AttributePaneAPI;
class DFBrowserPane_AttributePaneSelector;
class DFBrowser_Module;
class DFBrowser_SearchView;
class DFBrowser_TreeLevelView;

class QStackedWidget;
class QWidget;

//! \class DFBrowser_AttributePaneStack
//! \brief Provides work to stacked widget where the current attribute pane is shown.
class DFBrowser_AttributePaneStack : public QObject
{
public:

  //! Constructor
  Standard_EXPORT DFBrowser_AttributePaneStack (QObject* theParent);

  //! Destructor
  virtual ~DFBrowser_AttributePaneStack() {}

  //! Creates a stacked widget
  //! \param theParent a parent widget
  Standard_EXPORT virtual void CreateWidget (QWidget* theParent);

  //! \return the stacked widget
  QWidget* GetWidget() const { return myAttributesStack; }

  //! Fills stackk with the information
  //! \param theAttributeTypes a map of attribute type name to information pane
  void SetModule (DFBrowser_Module* theModule) { myModule = theModule; }

  //! Sets the current widget in the stacked control either attribute pane or search view
  //! \param theMode an active mode
  Standard_EXPORT void SetPaneMode (const DFBrowser_AttributePaneType& theMode);

  //! \return an attribute pane, which corresponds the current widget of the stacked widget control
  DFBrowserPane_AttributePaneAPI* GetCurrentPane() const { return myCurrentPane; }

  //! \return tree level view
  DFBrowser_TreeLevelView* GetTreeLevelView() const { return myTreeLevelView; }

  //! \return the search view
  DFBrowser_SearchView* GetSearchView() const { return mySearchView; }

  //! \return the pane selector
  DFBrowserPane_AttributePaneSelector* GetPaneSelector() const { return myPaneSelector; }

  //! Sets an active widget of attribute pane if the pane mode is item view mode.
  //! Gets selection models of this pane and set the models into pane selector
  Standard_EXPORT void SetCurrentItem (const QModelIndex& theIndex);

protected:

  DFBrowserPane_AttributePaneAPI* myCurrentPane; //!< active pane if mode is item view mode
  DFBrowserPane_AttributePaneSelector* myPaneSelector; //!< pane selector filled by the active pane
  QStackedWidget* myAttributesStack; //!< container of already created panes
  DFBrowser_Module* myModule; //!< the current module
  DFBrowser_TreeLevelView* myTreeLevelView; //!< view of objects from one level of tree view
  DFBrowser_SearchView* mySearchView; //!< view of search control
  QWidget* myEmptyWidget; //!< an empty widget when nothing is selected in tree view
  DFBrowser_AttributePaneType myPaneMode; //!< the current pane mode, either item view or search view
};

#endif
