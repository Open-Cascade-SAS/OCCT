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

#ifndef DFBrowser_Module_H
#define DFBrowser_Module_H

#include <inspector/DFBrowser_TreeModel.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

#include <AIS_InteractiveContext.hxx>
#include <NCollection_Map.hxx>
#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDocStd_Application.hxx>
#include <TopoDS_Shape.hxx>

#include <QObject>

class DFBrowserPane_AttributePaneAPI;
class DFBrowserPane_AttributePaneCreatorAPI;

class QAbstractItemModel;
class QItemSelectionModel;

//! \class DFBrowser_Module
//! The class is a container of current components of DFBrowser:
//! - OCAF view model
//! - OCAF selection model
//! - container of attribute panes into attribute name
//! - acceptable attribute pane creators
//! - external AIS interactive context
//! It has general attribute pane creator, if the application is XDE, it uses XDE attribute pane creator.
//! It fills container of created attribute pane.
class DFBrowser_Module : public QObject
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowser_Module();

  //! Destructor
  virtual ~DFBrowser_Module() Standard_OVERRIDE {};

  //! Creates tree model for OCAF application
  Standard_EXPORT void CreateViewModel (void* theParent);

  //! Fills tree model by the application and register pane creator
  //! \param theApplication a current application
  Standard_EXPORT void SetApplication (const Handle(TDocStd_Application)& theApplication);

  //! Fills viewer by the context
  //! \param theContext a current context where presentations should be visualized
  Standard_EXPORT void SetExternalContext (const Handle(Standard_Transient)& theContext);

  //! Returns external context or NULL
  const Handle(AIS_InteractiveContext)& GetExternalContext() const { return myExternalContext; }

  //! Returns a view model with the OCAF structure content
  QAbstractItemModel* GetOCAFViewModel() { return myOCAFViewModel; }

  //! Sets selection model of tree view
  void SetOCAFViewSelectionModel(QItemSelectionModel* theSelectionModel)
  { myOCAFViewSelectionModel = theSelectionModel; }

  //! Returns selection model of tree view
  QItemSelectionModel* GetOCAFViewSelectionModel() const { return myOCAFViewSelectionModel; }

  //! Returns an OCAF application or NULL
  //! \return an application instance
  Standard_EXPORT Handle(TDocStd_Application) GetTDocStdApplication() const;

  //! Rebuild an OCAF tree view model
  Standard_EXPORT void UpdateTreeModel();

  //! Sets initial selection in OCAF tree view, it is an application(root) item
  Standard_EXPORT void SetInitialTreeViewSelection();

  //! Returns attibute placed on the parameter index in the OCAF tree view or NULL
  //! \param theIndex an index in OCAF tree view.
  //! \return an attribute
  Standard_EXPORT Handle(TDF_Attribute) FindAttribute (const QModelIndex& theIndex);

  //! Append creator of a pane by attribute type
  //! \param thePaneCreator
  void RegisterPaneCreator (DFBrowserPane_AttributePaneCreatorAPI* thePaneCreator)
  { myPaneCreators.append (thePaneCreator); }

  //! Returns an attribute pane for the attribute: create a new if it is not exist in
  //! internal map and the module processes this kind of attribute
  //! \param theAttributeGUID an attribute key
  //! \return attribute pane
  Standard_EXPORT DFBrowserPane_AttributePaneAPI* GetAttributePane (Handle(TDF_Attribute) theAttribute);

  //! Returns an attribute pane for the attribute GUID: create a new if it is not exist in
  //! internal map and the module processes this kind of attribute
  //! \param theAttributeGUID an attribute key
  //! \return attribute pane
  Standard_EXPORT DFBrowserPane_AttributePaneAPI* GetAttributePane (const Standard_CString& theAttributeName);

  //! Finds the attribute pane according to the give attribute and returns its information
  //! \param theAttribute a source attribute
  //! \param theModule to provide a map of attribute id to attribute pane
  //! \param theRole an attribute role in the tree view, includes: text, icon, color roles
  //! \param theColumnId a column index
  Standard_EXPORT static QVariant GetAttributeInfo (Handle(TDF_Attribute) theAttribute, DFBrowser_Module* theModule,
                                                    int theRole, int theColumnId);

  //! Returns information for the given attribute type name
  //! \param theAttributeName a current attribute type name
  //! \param theModule a current module
  //! \param theRole a role of information, used by tree model (e.g. DisplayRole, icon, background and so on)
  //! \param theColumnId a tree model column
  //! \return value, interpreted by tree model depending on the role
  Standard_EXPORT static QVariant GetAttributeInfo (const Standard_CString& theAttributeName, DFBrowser_Module* theModule,
                                                    int theRole, int theColumnId);
signals:

  //! Emits signal about updating tree model
  void beforeUpdateTreeModel();

protected:

  //! Tries to create attribute pane for the attribute name using registered attribute pane creators
  //! \param theAttributeName a source attribute
  //! \return attribute pane or NULL
  DFBrowserPane_AttributePaneAPI* CreateAttributePane (const Standard_CString& theAttributeName);

private:

  DFBrowser_TreeModel* myOCAFViewModel; //!< the tree view abstract model
  QItemSelectionModel* myOCAFViewSelectionModel; //!< selection model over OCAF tree view
  QMap<TCollection_AsciiString, DFBrowserPane_AttributePaneAPI*> myAttributeTypes; //!< container of created panes
  QList<DFBrowserPane_AttributePaneCreatorAPI*> myPaneCreators; //!< pane creators
  Handle(AIS_InteractiveContext) myExternalContext; //!< context that comes in initialize parameters
};

#endif
