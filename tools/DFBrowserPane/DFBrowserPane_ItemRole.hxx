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

#ifndef DFBrowserPane_ItemRole_H
#define DFBrowserPane_ItemRole_H

#include <inspector/TreeModel_ItemRole.hxx>

//! Custom item role for attribute pane
enum DFBrowserPane_ItemRole
{
  DFBrowserPane_ItemRole_ShortInfo = TreeModel_ItemRole_LastTreeRole + 1, //!< not full information of an attribute
  DFBrowserPane_ItemRole_Decoration_40x40, //!< icon with greater size
  DFBrowserPane_ItemRole_DisplayExtended, // extended item information (with additional information) for Qt::DisplayRole
  DFBrowserPane_ItemRole_ToolTipExtended, // exteneded tool tip information for Qt::ToolTipRole
  DFBrowserPane_ItemRole_LastTreeRole //! last enumeration value to use outside incremented
};

#endif
