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

#ifndef DFBrowserPaneXDE_AttributeCommonPane_H
#define DFBrowserPaneXDE_AttributeCommonPane_H

#include <inspector/DFBrowserPane_AttributePane.hxx>

#include <Standard.hxx>
#include <TopoDS_Shape.hxx>

#include <QVariant>

class QWidget;
class QItemSelectionModel;

//! \class DFBrowserPaneXDE_AttributeCommonPane
//! \brief It covers standard attribute panes. The purpose is to return XDE specific short information for
//! several kinds of attributes. The other functionality is the same as for standard panes, it just sends
//! it to the panes.
class DFBrowserPaneXDE_AttributeCommonPane : public DFBrowserPane_AttributePane
{
public:

  //! Constructor
  Standard_EXPORT DFBrowserPaneXDE_AttributeCommonPane (DFBrowserPane_AttributePaneAPI* theStandardPane);

  //! Destructor
  virtual ~DFBrowserPaneXDE_AttributeCommonPane() {}

  //! Returns true if this type can be processed by this pane. It contains standard attributes that
  //! have difference in presentation (TDataStd_TreeNode, TDF_Reference, TNaming_NamedShape and TDataStd_UAttribute).
  //! Also it contains XCAFDoc attributes (should be implemented in this package or pane will be empty)
  //! \param theAttributeType an attribute type
  Standard_EXPORT static bool ProcessAttribute (const Standard_CString& theAttributeType);

  //! Creates table view and call create widget of array table helper
  //! \param theParent a parent widget
  //! \return a new widget
  Standard_EXPORT virtual QWidget* CreateWidget (QWidget* theParent) Standard_OVERRIDE;

  //! Initializes the content of the pane by the parameter attribute
  //! \param theAttribute an OCAF attribute
  Standard_EXPORT void Init (const Handle(TDF_Attribute)& theAttribute);

  //! Returns list of selection models. In default implementation it contains a selection model for the table view
  //! \returns container of models
  Standard_EXPORT virtual std::list<QItemSelectionModel*> GetSelectionModels() Standard_OVERRIDE;

  //! Returns information for the given attribute
  //! \param theAttribute a current attribute
  //! \param theRole a role of information, used by tree model (e.g. DisplayRole, icon, background and so on)
  //! \param theColumnId a tree model column
  //! \return value, interpreted by tree model depending on the role
  Standard_EXPORT virtual QVariant GetAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                     int theRole, int theColumnId) Standard_OVERRIDE;

  //! Returns brief attribute information. In general case, it returns GetValues() result.
  //! \param theAttribute a current attribute
  //! \param theValues a result list of values
  Standard_EXPORT virtual void GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                      QList<QVariant>& theValues) Standard_OVERRIDE;

  //! Returns selection kind for the model, it may be General selection or Additional selection for example
  //! \param theModel one of selection models provided by this pane
  //! \return selection kind
  Standard_EXPORT virtual int GetSelectionKind (QItemSelectionModel* theModel) Standard_OVERRIDE;

  //! Returns selection parameters, that may be useful for communicate between tools
  //! \param theModel one of selection models provided by this pane
  //! \theParameters a container of parameters, might be extended depending on the pane state(e.g. selection)
  Standard_EXPORT virtual void GetSelectionParameters (QItemSelectionModel* theModel,
                                       NCollection_List<Handle(Standard_Transient)>& theParameters) Standard_OVERRIDE;

  //! Returns presentation of the attribute to be visualized in the view
  //! \param theAttribute a current attribute
  //! \return handle of presentation if the attribute has, to be visualized
  Standard_EXPORT virtual Handle(Standard_Transient) GetPresentation
    (const Handle (TDF_Attribute)& theAttribute) Standard_OVERRIDE;

  //! Returns container of Label references to the attribute
  //! \param theAttribute a current attribute
  //! \param theRefLabels a container of label references, to be selected in tree view
  //! \param theRefPresentation handle of presentation for the references, to be visualized
  Standard_EXPORT virtual void GetReferences (const Handle(TDF_Attribute)& theAttribute,
                                              NCollection_List<TDF_Label>& theRefLabels,
                                              Handle(Standard_Transient)& theRefPresentation) Standard_OVERRIDE;

  //! Returns container of Attribute references to the attribute
  //! \param theAttribute a current attribute
  //! \param theRefAttributes a container of attribute references, to be selected in tree view
  //! \param theRefPresentation handle of presentation for the references, to be visualized
  Standard_EXPORT virtual void GetAttributeReferences (const Handle(TDF_Attribute)& theAttribute,
                                       NCollection_List<Handle(TDF_Attribute)>& theRefAttributes,
                                       Handle(Standard_Transient)& theRefPresentation) Standard_OVERRIDE;

  //! Returns values to fill the table view model
  //! \param theAttribute a current attribute
  //! \param theValues a container of values
  Standard_EXPORT virtual void GetValues (const Handle(TDF_Attribute)& theAttribute,
                                          QList<QVariant>& theValues) Standard_OVERRIDE;

private:

  DFBrowserPane_AttributePane* myStandardPane; //!< pane, that corresponds to the current type of attribute
};

#endif
