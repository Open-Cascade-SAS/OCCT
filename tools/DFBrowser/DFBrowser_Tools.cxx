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

#include <inspector/DFBrowser_Tools.hxx>

#include <inspector/DFBrowser_ItemBase.hxx>

#include <inspector/DFBrowserPane_AttributePaneCreator.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <TDF_ChildIterator.hxx>
#include <Standard_GUID.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>

#include <QColor>
#include <QDir>
#include <QPalette>
#include <QWidget>

namespace DFBrowser_Tools
{
  // =======================================================================
  // function : IsEmptyLabel
  // purpose :
  // =======================================================================
  bool IsEmptyLabel (const TDF_Label& theLabel)
  {
    if (theLabel.IsNull())
      return true;

    if (theLabel.IsRoot())
      return false;

    if (theLabel.NbAttributes() > 0)
      return false;

    for (TDF_ChildIterator aChildIt (theLabel); aChildIt.More(); aChildIt.Next())
    {
      if (!IsEmptyLabel (aChildIt.Value()))
        return false;
    }
    return true;
  }

  // =======================================================================
  // function : GetLabelInfo
  // purpose :
  // =======================================================================
  QString GetLabelInfo (const TDF_Label& theLabel, const bool isUseShortInfo)
  {
    QString aValue = DFBrowserPane_Tools::GetEntry (theLabel).ToCString();
    if (!isUseShortInfo)
      return aValue;

    Handle(TDataStd_Name) aName;
    if (!theLabel.FindAttribute (TDataStd_Name::GetID(), aName))
      return aValue;

    aValue += " ";
    aValue += DFBrowserPane_Tools::ToString (aName->Get());

    return aValue;
  }


  enum DFBrowser_IconType
  {
    DFBrowser_IconType_16x16,
    DFBrowser_IconType_40x40
  };
  static QMap<DFBrowser_IconType, QIcon> MyLabelIcons;

  // =======================================================================
  // function : GetLabelIcon
  // purpose :
  // =======================================================================
  QIcon GetLabelIcon (const TDF_Label& theLabel, bool isStandard16x16)
  {
    (void)theLabel;
    if (MyLabelIcons.empty())
    {
      MyLabelIcons[DFBrowser_IconType_16x16] = QIcon (":/icons/label_folder_16x16.png");
      MyLabelIcons[DFBrowser_IconType_40x40] = QIcon (":/icons/label_folder_40x40.png");
    }
    return MyLabelIcons[isStandard16x16 ? DFBrowser_IconType_16x16 : DFBrowser_IconType_40x40];
  }
}
