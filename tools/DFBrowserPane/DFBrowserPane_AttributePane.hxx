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

#ifndef DFBrowserPane_AttributePane_H
#define DFBrowserPane_AttributePane_H

#include <inspector/DFBrowserPane_AttributePaneAPI.hxx>

#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <Standard.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QList>
#include <QVariant>

class DFBrowserPane_AttributePaneModel;
class DFBrowserPane_TableView;

class QAbstractTableModel;
class QItemSelectionModel;

//! \class DFBrowserPane_AttributePane
//! \brief This is an extension of base attribute pane:
//! - GetWidget() creates table view, view model and selection model. Table is vertical with one column.
//! - Init() obtains GetValues and give it to the table view model
//! If standard pane with such a table is used, only GetValues() should be redefined in children
class DFBrowserPane_AttributePane : public DFBrowserPane_AttributePaneAPI
{
public:

  //! Constructor
  Standard_EXPORT DFBrowserPane_AttributePane();

  //! Destructor
  virtual ~DFBrowserPane_AttributePane() {}

  //! Creates a new widget
  //! \param theParent a parent widget
  //! \return pane widget
  Standard_EXPORT virtual QWidget* CreateWidget(QWidget* theParent);

  //! Creates widget if it was not created and isToCreate is true
  //! \param theParent a parent widget
  //! \param isToCreate flag if the widget should be created if it is NULL
  //! \return pane widget
  Standard_EXPORT virtual QWidget* GetWidget(QWidget* theParent, const bool isToCreate) Standard_OVERRIDE;

  //! Gets values of attribute using GetValues() and Init the view model
  //! \param theAttribute a current attribute
  Standard_EXPORT virtual void Init(const Handle(TDF_Attribute)& theAttribute) Standard_OVERRIDE;

  //! Returns values to fill the table view model
  //! \param theAttribute a current attribute
  //! \param theValues a container of values
  virtual void GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
  { (void)theAttribute; (void)theValues; };

  //! Returns common information for the given attribute kind
  //! \param theAttributeName a kind of attribute
  //! \param theRole a role of information, used by tree model (e.g. DisplayRole, icon, background and so on)
  //! \param theColumnId a tree model column
  //! \return value, interpreted by tree model depending on the role
  Standard_EXPORT static QVariant GetAttributeInfoByType(const Standard_CString& theAttributeName, int theRole, int theColumnId);

  //! Returns information for the given attribute
  //! \param theAttribute a current attribute
  //! \param theRole a role of information, used by tree model (e.g. DisplayRole, icon, background and so on)
  //! \param theColumnId a tree model column
  //! \return value, interpreted by tree model depending on the role
  Standard_EXPORT virtual QVariant GetAttributeInfo(const Handle(TDF_Attribute)& theAttribute, int theRole, int theColumnId);

  //! Returns brief attribute information. In general case, it returns even values of GetValues() result.
  //! \param theAttribute a current attribute
  //! \param theValues a result list of values
  Standard_EXPORT virtual void GetShortAttributeInfo (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues);

  //! Returns list of selection models. In default implementation it contains a selection model for the table view
  //! \returns container of models
  virtual std::list<QItemSelectionModel*> GetSelectionModels() Standard_OVERRIDE { return mySelectionModels; }

protected:

  //! Returns converted table view model
  Standard_EXPORT DFBrowserPane_AttributePaneModel* getPaneModel() const;

  //! Returns converted table view
  DFBrowserPane_TableView* getTableView() const { return myTableView; }

  //! Returns header text values for 0...n table cells in parameter orientation
  //! \param theOrientation defines horizontal or vertical values
  //! \param theValues output container of values
  virtual QList<QVariant> getHeaderValues (const Qt::Orientation theOrientation)
    { (void)theOrientation; return QList<QVariant>(); }

  //! Retuns number of columns in internal table. By default it returns 2 : method name for method value.
  //! \return integer value
  virtual int getColumnCount() const { return 2; }

  //! Defines widths of table columns
  //! \return container of widths
  Standard_EXPORT virtual QMap<int, int> getTableColumnWidths() const;

protected:

  QWidget* myMainWidget; //!< widget created in this pane
  DFBrowserPane_TableView* myTableView; //!< table for visualization of attribute parameters
  QAbstractTableModel* myPaneModel; //!< table view model. It is created before the table view, so we need to cache it
  std::list<QItemSelectionModel*> mySelectionModels; //! selection models
};

#endif
