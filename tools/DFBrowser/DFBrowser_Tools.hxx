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

#ifndef DFBrowser_Tools_H
#define DFBrowser_Tools_H

#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard.hxx>
#include <Standard_GUID.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <TDF_Attribute.hxx>
#include <TopoDS_Shape.hxx>

#include <QAbstractItemModel>
#include <QColor>
#include <QMap>
#include <QModelIndex>
#include <QPixmap>
#include <QString>
#include <QIcon>

class DFBrowserPane_AttributePaneAPI;

//! \namespace DFBrowser_Tools
//! The namespace that gives auxiliary methods for TDF elements manipulation
namespace DFBrowser_Tools
{

  //! Returns true if the label is not root, has attribute or at least one sub-label with attribute
  //! \param theLabel a label
  //! \return boolean result
  Standard_EXPORT bool IsEmptyLabel (const TDF_Label& theLabel);

  //! Returns the label entry
  //! \param isUseShortInfo boolean value if value of name attribute should be included to result
  //! \return string value
  Standard_EXPORT QString GetLabelInfo (const TDF_Label& theLabel, const bool isUseShortInfo = true);

  //! Returns the label icon
  Standard_EXPORT QIcon GetLabelIcon (const TDF_Label& theLabel, bool isStandard16x16 = true);
}

#endif
