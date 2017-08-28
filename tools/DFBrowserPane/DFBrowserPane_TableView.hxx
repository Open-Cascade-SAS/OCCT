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

#ifndef DFBrowserPane_TableView_H
#define DFBrowserPane_TableView_H

#include <Standard.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QWidget>
#include <QMap>

class QTableView;
class QAbstractTableModel;

//! \class DFBrowserPane_TableView
//! \brief The widget that contains table view with some settings:
//! - table grid is hidden
//! - vertical header is hidden
//! - stretch last column by horizonal
//! - default width of columns.
class DFBrowserPane_TableView : public QWidget
{
public:

  //! Constructor
  Standard_EXPORT DFBrowserPane_TableView (QWidget* theParent,
                                           const QMap<int, int>& theDefaultColumnWidths = QMap<int, int>());
  //! Destructor
  virtual ~DFBrowserPane_TableView() {}

  //! Sets model into table view. After, set column widths for view
  Standard_EXPORT void SetModel (QAbstractTableModel* theModel);

  //! Returns the current table view
  QTableView* GetTableView() const { return myTableView; }

  //! Set horizontal header shown or hidden
  //! \param theVisible visibility flag
  Standard_EXPORT void SetVisibleHorizontalHeader (const bool& theVisible);

  //! Updates table view height to contain the given number of rows only
  //! \param theCount a row count
  //! \param theView a table view, which size will be changed
  //! \param theScroll flag whether add scroll height to size
  Standard_EXPORT static void SetFixedRowCount (const int theCount, QTableView* theView, const bool theScroll = false);

  //! Returns names of selected items in the view
  //! \param theView a table view
  //! \param theColumnId a column index
  //! \return container of strings
  Standard_EXPORT static QStringList GetSelectedColumnValues (QTableView* theTable, const int theColumnId);

private:

  QTableView* myTableView; //!< the current table view
  QMap<int, int> myDefaultColumnWidths; //!< container of default widths of columns
};

#endif
