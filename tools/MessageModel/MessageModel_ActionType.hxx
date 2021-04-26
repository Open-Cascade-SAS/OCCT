// Created on: 2021-04-27
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef MessageModel_ActionType_H
#define MessageModel_ActionType_H

//! Kind of action type for tree view context menu item
enum MessageModel_ActionType
{
  MessageModel_ActionType_Activate, //!< set Message_Report active
  MessageModel_ActionType_Deactivate, //!< set Message_Report not active
  MessageModel_ActionType_Clear, //!< clear Message_Report alerts
  MessageModel_ActionType_ExportToShapeView, //!< export TopoDS_Shape of selected item into TKShapeView plugin
  MessageModel_ActionType_TestMetric, //!< test alerts
  MessageModel_ActionType_TestMessenger, //!< test message view on messenger printer to report
  MessageModel_ActionType_TestReportTree //!< test message view on hierarchical report
};

#endif
