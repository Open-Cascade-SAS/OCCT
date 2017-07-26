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

#ifndef DFBrowser_SearchItemInfo_H
#define DFBrowser_SearchItemInfo_H

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QVariant>
#include <QStringList>
#include <QString>

//! \class DFBrowser_SearchItemInfo
//! Information of item in search model
class DFBrowser_SearchItemInfo
{
public:

  //! Constructor
  DFBrowser_SearchItemInfo() {}

  //! Constructor
  DFBrowser_SearchItemInfo (const QVariant& theIcon, const QString& theName,
                            const QStringList& thePath, const QString& theSeparator)
  : myIcon (theIcon), myPath (thePath)
  { myPathUnited = QString ("%1 \n%2").arg (theName).arg (myPath.join (theSeparator)); }

  //! Destructor
  virtual ~DFBrowser_SearchItemInfo() {}

  //! Returns the item icon
  const QVariant& Icon() const { return myIcon; }

  //! Returns path to the item.
  const QStringList& Path() const { return myPath; }

  //! Returns united path to the item.
  const QString& PathUnited() const { return myPathUnited; }

private:

  QVariant myIcon; //!< item icon
  QStringList myPath; //!< item path
  QString myPathUnited; //!< item name and item path
};

#endif
