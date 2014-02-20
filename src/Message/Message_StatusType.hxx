// Created on: 2007-07-06
// Created by: Pavel TELKOV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

// Original implementation copyright (c) RINA S.p.A.

#ifndef Message_StatusType_HeaderFile
#define Message_StatusType_HeaderFile

//! Definition of types of execution status supported by
//! the class Message_ExecStatus

enum Message_StatusType 
{
  Message_DONE         = 0x00000100,
  Message_WARN         = 0x00000200,
  Message_ALARM        = 0x00000400,
  Message_FAIL         = 0x00000800
};

#endif
