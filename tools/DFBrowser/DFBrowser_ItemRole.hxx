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

#ifndef DFBrowser_ItemRole_H
#define DFBrowser_ItemRole_H

#include <inspector/DFBrowserPane_ItemRole.hxx>

#include <Qt>

//! Additional data item role for DFBrowser tree item
enum DFBrowser_ItemRole
{
  DFBrowser_ItemRole_AdditionalInfo = DFBrowserPane_ItemRole_LastTreeRole + 1 //!< an attribute additional information 
};

#endif
