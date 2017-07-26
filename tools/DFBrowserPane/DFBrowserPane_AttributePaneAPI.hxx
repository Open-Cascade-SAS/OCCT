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

#ifndef DFBrowserPane_AttributePaneAPI_H
#define DFBrowserPane_AttributePaneAPI_H

#include <TDF_Attribute.hxx>
#include <NCollection_List.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelList.hxx>
#include <Standard.hxx>
#include <Standard_Transient.hxx>

#include <list>

class QWidget;
class QItemSelectionModel;

//! \class DFBrowserPane_AttributePane
//! \brief This is a base class for attribute pane. It defines methods that should be redefined in chindren.
//! The kinds of information provided by the methods:
//! - widget, a control that should be shown for this kind of attribute
//! - Init method to fill control by the attribute context
//! - selection models that are active, the application can connect to the models to react to selection change
//! - presentation/references/attribute references of attribute custom information about this attribute
class DFBrowserPane_AttributePaneAPI
{
public:

  //! Constructor
  DFBrowserPane_AttributePaneAPI() {}

  //! Destructor
  virtual ~DFBrowserPane_AttributePaneAPI() {}

  //! Creates widget to visualize values of this kind of attribute
  //! \param theParent a parent for the widget
  //! \param isToCreate flag whether the widget should be created
  virtual QWidget* GetWidget (QWidget* theParent, const bool isToCreate)
  { (void)theParent; (void)isToCreate; return 0; }

  //! Initializes the content of the pane by the parameter attribute
  //! \param theAttribute an OCAF attribute
  virtual void Init (const Handle(TDF_Attribute)& theAttribute) { (void)theAttribute; };

  //! Returns selections models of the pane, if the selection is possible in the pane controls
  virtual std::list<QItemSelectionModel*> GetSelectionModels() { return std::list<QItemSelectionModel*>(); }

  //! Returns selection kind for the model, it may be General selection or Additional selection for example
  //! \param theModel one of selection models provided by this pane
  //! \return selection kind
  virtual int GetSelectionKind (QItemSelectionModel* theModel) { (void)theModel; return -1; }

  //! Returns selection parameters, that may be useful for communicate between tools
  //! \param theModel one of selection models provided by this pane
  //! \theParameters a container of parameters, might be extended depending on the pane state(e.g. selection)
  virtual void GetSelectionParameters (QItemSelectionModel* theModel,
                                       NCollection_List<Handle(Standard_Transient)>& theParameters)
  { (void)theModel; (void)theParameters; }

  //! Returns presentation of the attribute to be visualized in the view
  //! \param theAttribute a current attribute
  //! \return handle of presentation if the attribute has, to be visualized
  virtual Handle(Standard_Transient) GetPresentation (const Handle(TDF_Attribute)& theAttribute)
  { (void)theAttribute; return Handle(Standard_Transient)(); }

  //! Returns container of Label references to the attribute
  //! \param theAttribute a current attribute
  //! \param theRefLabels a container of label references, to be selected in tree view
  //! \param theRefPresentation handle of presentation for the references, to be visualized
  virtual void GetReferences (const Handle(TDF_Attribute)& theAttribute,
                              NCollection_List<TDF_Label>& theRefLabels,
                              Handle(Standard_Transient)& theRefPresentation)
  { (void)theAttribute; (void)theRefLabels; (void)theRefPresentation; }

  //! Returns container of Attribute references to the attribute
  //! \param theAttribute a current attribute
  //! \param theRefAttributes a container of attribute references, to be selected in tree view
  //! \param theRefPresentation handle of presentation for the references, to be visualized
  virtual void GetAttributeReferences (const Handle(TDF_Attribute)& theAttribute,
                                       NCollection_List<Handle(TDF_Attribute)>& theRefAttributes,
                                       Handle(Standard_Transient)& theRefPresentation)
  { (void)theAttribute; (void)theRefAttributes; (void)theRefPresentation; }
};

#endif
