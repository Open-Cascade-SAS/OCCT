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

#ifndef TreeModel_Tools_H
#define TreeModel_Tools_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QByteArray>
#include <QMap>
#include <QString>
#include <QStyle>
#include <Standard_WarningsRestore.hxx>

class QAction;
class QObject;
class QTreeView;

//! \class TreeModel_Tools
//! \brief The tool that gives auxiliary methods for qt elements manipulation
class TreeModel_Tools
{
public:

  //! Converts a Qt string to byte array, string has mask: @ByteArray[...]
  //! \param theValue a converted string
  //! \return the extended filled array
  Standard_EXPORT static QString ToString (const QByteArray& theValue);

  //! Converts a Qt byte array to Qt string. It has mask: @ByteArray[...]
  //! \param theValue a converted string
  //! \return the extended filled array
  Standard_EXPORT static QByteArray ToByteArray (const QString& theValue);

  //! Returns header margin, defined in style settings of application
  //! \return integer value
  Standard_EXPORT static int HeaderSectionMargin() { return qApp->style()->pixelMetric (QStyle::PM_HeaderMargin); }

  //! Save state of three view in a container in form: key, value. It saves:
  //! - visibiblity of columns,
  //! - columns width
  //! \param theTreeView a view instance
  //! \param theItems [out] properties
  //! \param thePrefix peference item prefix
  Standard_EXPORT static void SaveState (QTreeView* theTreeView, QMap<QString, QString>& theItems,
                                         const QString& thePrefix = QString());
  //! Restore state of three view by a container
  //! \param theTreeView a view instance
  //! \param theKey property key
  //! \param theValue property value
  //! \param thePrefix peference item prefix
  //! \return boolean value whether the property is applyed to the tree view
  Standard_EXPORT static bool RestoreState (QTreeView* theTreeView, const QString& theKey, const QString& theValue,
                                            const QString& thePrefix = QString());

  //! Fills tree view by default sections parameters obtained in view's tree model
  //! \param theTreeView tree view instance
  Standard_EXPORT static void SetDefaultHeaderSections (QTreeView* theTreeView);

  //! Sets using visibility column in the tree view:
  //! - sets the state in the TreeModel
  //! - set section width, not resizable
  //! \param theTreeView a view instance
  //! \param theActive boolean value if the column should be connected/visible and other
  Standard_EXPORT static void UseVisibilityColumn (QTreeView* theTreeView, const bool theActive = true);

  //! Returns the text width
  //! \param theText source text
  //! \param theParent parent widget with its own style
  //! \return calculated width value
  Standard_EXPORT static int GetTextWidth (const QString& theText, QObject* theParent);

  //! Returns string cut by width and '\n'
  //! \param theText processing string
  //! \param theWidth width value, if -1, default value is used
  //! \param theTail symbols added to the end of the cut string
  Standard_EXPORT static QString CutString (const QString& theText, const int theWidth = -1, const QString& theTail = "...");
};

#endif
