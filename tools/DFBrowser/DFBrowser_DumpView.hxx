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

#ifndef DFBrowser_DumpView_H
#define DFBrowser_DumpView_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QItemSelection>
#include <QPlainTextEdit>
#include <Standard_WarningsRestore.hxx>

class QWidget;

//! \class DFBrowser_DumpView
//! \brief View to display result of Dump() method of a TDF_Attribute
class DFBrowser_DumpView : public QObject
{
  Q_OBJECT
public:
  //! Constructor
  DFBrowser_DumpView(QWidget* theParent)
      : QObject(theParent),
        myTextEdit(new QPlainTextEdit(theParent))
  {
  }

  //! Destructor
  virtual ~DFBrowser_DumpView() {}

  //! \return the text edit control
  QWidget* GetControl() const { return myTextEdit; }

public slots:

  //! Listens selection change and update the current control content by selection
  //! \param theSelected container of selected items
  //! \param theDeselected container of items that become deselected
  Standard_EXPORT void OnTreeViewSelectionChanged(const QItemSelection& theSelected,
                                                  const QItemSelection& theDeselected);

private:
  QPlainTextEdit* myTextEdit; //!< information view
};
#endif
