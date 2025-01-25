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

#ifndef ViewControl_TreeView_H
#define ViewControl_TreeView_H

#include <Standard.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QSize>
#include <QTreeView>
#include <Standard_WarningsRestore.hxx>

class QWidget;

//! \class ViewControl_TreeView
//! Extended tree view control with possibility to set predefined size.
class ViewControl_TreeView : public QTreeView
{
public:
  //! Constructor
  ViewControl_TreeView(QWidget* theParent)
      : QTreeView(theParent)
  {
  }

  //! Destructor
  virtual ~ViewControl_TreeView() {}

  //! Sets default size of control, that is used by the first control show
  //! \param theDefaultWidth the width value
  //! \param theDefaultHeight the height value
  void SetPredefinedSize(const QSize& theSize) { myDefaultSize = theSize; }

  //! Returns predefined size if both values are positive, otherwise parent size hint
  virtual QSize sizeHint() const Standard_OVERRIDE
  {
    return myDefaultSize.isValid() ? myDefaultSize : QTreeView::sizeHint();
  }

private:
  QSize myDefaultSize; //!< default size, empty size if it should not be used
};

#endif
