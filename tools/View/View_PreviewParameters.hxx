// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef View_PreviewParameters_H
#define View_PreviewParameters_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <Prs3d_Drawer.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QMap>
#include <QString>
#include <Standard_WarningsRestore.hxx>

//! \class View_PreviewParameters
//! Container of View tool bar actions
class View_PreviewParameters
{
public:
  //! Constructor
  Standard_EXPORT View_PreviewParameters(const Standard_Boolean theToTransparent = Standard_True);

  //! Destructor
  virtual ~View_PreviewParameters() {}

  //! Returns main control
  const Handle(Prs3d_Drawer)& GetDrawer() const { return myDrawer; }

  //! Saves state of preview parameters in a container in form: key, value. It saves:
  //! - visibility of columns,
  //! - columns width
  //! \param theTreeView a view instance
  //! \param[out] theItems  properties
  //! \param thePrefix preference item prefix
  static void SaveState(View_PreviewParameters* theParameters,
                        QMap<QString, QString>& theItems,
                        const QString&          thePrefix = QString())
  {
    (void)theParameters;
    (void)theItems;
    (void)thePrefix;
  }

  //! Restores state of preview parameters by a container
  //! \param theTreeView a view instance
  //! \param theKey property key
  //! \param theValue property value
  //! \param thePrefix preference item prefix
  //! \return boolean value whether the property is applied to the tree view
  static bool RestoreState(View_PreviewParameters* theParameters,
                           const QString&          theKey,
                           const QString&          theValue,
                           const QString&          thePrefix = QString())
  {
    (void)theParameters;
    (void)theKey;
    (void)theValue;
    (void)thePrefix;
    return false;
  }

private:
  Handle(Prs3d_Drawer) myDrawer; //!< attributes for preview presentation
};

#endif
