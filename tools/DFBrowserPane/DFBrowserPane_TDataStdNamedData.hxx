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

#ifndef DFBrowserPane_TDataStdNamedData_H
#define DFBrowserPane_TDataStdNamedData_H

#include <inspector/DFBrowserPane_AttributePane.hxx>

#include <Standard.hxx>

class DFBrowserPane_AttributePaneModel;
class DFBrowserPane_TableView;

//! \class DFBrowserPane_TDataStdNamedData
//! \brief The class to manipulate of TDataStd_NamedData attribute
//! This pane contains six pair of table views, where values of this data container presented
//! The first table in pair presents size of a separate data container, the second presents values
//! Each control for data container is grouped in a checkable group box to switch off/on using of a container.
class DFBrowserPane_TDataStdNamedData : public DFBrowserPane_AttributePane
{
public:

  //! Constructor
  Standard_EXPORT DFBrowserPane_TDataStdNamedData();

  //! Destructor
  virtual ~DFBrowserPane_TDataStdNamedData() {}

  //! Creates a new widget that contains containers for attribute values
  //! \param theParent a parent widget
  //! \return pane widget
  Standard_EXPORT virtual QWidget* CreateWidget (QWidget* theParent) Standard_OVERRIDE;

  //! Gets values of attribute using GetValues(), after fill tables if the pane with the values and Init the view model
  //! \param theAttribute a current attribute
  Standard_EXPORT virtual void Init (const Handle(TDF_Attribute)& theAttribute) Standard_OVERRIDE;

  //! Returns values to fill the table view model. The values are separated by an auxiliary key for getPartOfValues
  //! \param theAttribute a current attribute
  //! \param theValues a container of values
  Standard_EXPORT virtual void GetValues (const Handle(TDF_Attribute)& theAttribute,
                                          QList<QVariant>& theValues) Standard_OVERRIDE;

private:

  //! Obtains sub container of a general values container for given keys to fill tables for this kind
  //! \param theKey1 a key where values are started
  //! \param theKey2 a key before that the values are got
  //! \param theValues a full container of values for the current attribute
  //! \returns sub container
  QList<QVariant> getPartOfValues (const QString& theKey1, const QString& theKey2,
                                   const QList<QVariant>& theValues) const;

  //! Creates a model with two columns: "Name" to "Value". The orientation is horizontal
  DFBrowserPane_AttributePaneModel* createPaneModel();

private:
  //! myPaneMode and myTableView are used for int values

  DFBrowserPane_AttributePaneModel* myRealValuesModel; //!< real values model
  DFBrowserPane_TableView* myRealValues; //!< values table view

  DFBrowserPane_AttributePaneModel* myStringValuesModel; //!< string values model
  DFBrowserPane_TableView* myStringValues; //!< values table view

  DFBrowserPane_AttributePaneModel* myByteValuesModel; //!< byte values model
  DFBrowserPane_TableView* myByteValues; //!< values table view

  DFBrowserPane_AttributePaneModel* myIntArrayValuesModel; //!< int array values model
  DFBrowserPane_TableView* myIntArrayValues; //!< values table view

  DFBrowserPane_AttributePaneModel* myRealArrayModel; //!< real array values model
  DFBrowserPane_TableView* myRealArrayValues; //!< values table view
};

#endif
