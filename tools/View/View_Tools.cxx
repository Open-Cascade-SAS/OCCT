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

#include <inspector/View_Tools.hxx>
#include <inspector/View_Viewer.hxx>
#include <inspector/View_Widget.hxx>
#include <inspector/View_Window.hxx>

#include <V3d_View.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QObject>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : CreateAction
// purpose :
// =======================================================================
QAction* View_Tools::CreateAction (const QString& theText, const char* theSlot, QObject* theParent, QObject* theContext)
{
  QAction* anAction = new QAction (theText, theParent);
  QObject::connect (anAction, SIGNAL (triggered (bool)), theContext, theSlot);
  return anAction;
}

// =======================================================================
// function : SaveState
// purpose :
// =======================================================================
void View_Tools::SaveState (View_Window* theView, QMap<QString, QString>& theItems,
                            const QString& thePrefix)
{
  QStringList aCameraDirection;
  Standard_Real aVX, aVY, aVZ;
  theView->GetView()->GetViewer()->GetView()->Proj (aVX, aVY, aVZ);
  aCameraDirection << QString::number (aVX) << QString::number (aVY) << QString::number (aVZ);

  theItems[thePrefix + "view_camera_direction"] = aCameraDirection.join (",");
}

// =======================================================================
// function : RestoreState
// purpose :
// =======================================================================
bool View_Tools::RestoreState (View_Window* theView, const QString& theKey, const QString& theValue,
                               const QString& thePrefix)
{
  if (theKey == thePrefix + "view_camera_direction")
  {
    QStringList aValues = theValue.split (",");
    if (aValues.size() == 3)
    {
      Standard_Real aVX = aValues.at (0).toDouble();
      Standard_Real aVY = aValues.at (1).toDouble();
      Standard_Real aVZ = aValues.at (2).toDouble();

      theView->GetView()->SetInitProj (aVX, aVY, aVZ);
    }
  }
  else
    return false;
  return true;
}
