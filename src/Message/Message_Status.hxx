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

#ifndef Message_Status_HeaderFile
#define Message_Status_HeaderFile

#include <Message_StatusType.hxx>

//! Enumeration covering all execution statuses supported by the class
//! Message_ExecStatus: 32 statuses per each of 4 types (DONE, WARN, ALARM, FAIL)

enum Message_Status
{
  //! Empty status
  Message_None  = 0,          

  //! Something done, 32 variants
  Message_Done1 = Message_DONE, 
  Message_Done2,  Message_Done3,  Message_Done4,  Message_Done5,
  Message_Done6,  Message_Done7,  Message_Done8,  Message_Done9,
  Message_Done10, Message_Done11, Message_Done12, Message_Done13,
  Message_Done14, Message_Done15, Message_Done16, Message_Done17,
  Message_Done18, Message_Done19, Message_Done20, Message_Done21,
  Message_Done22, Message_Done23, Message_Done24, Message_Done25,
  Message_Done26, Message_Done27, Message_Done28, Message_Done29,
  Message_Done30, Message_Done31, Message_Done32,

  //! Warning for possible problem encountered, 32 variants
  Message_Warn1 = Message_WARN, 
  Message_Warn2,  Message_Warn3,  Message_Warn4,  Message_Warn5,
  Message_Warn6,  Message_Warn7,  Message_Warn8,  Message_Warn9,
  Message_Warn10, Message_Warn11, Message_Warn12, Message_Warn13,
  Message_Warn14, Message_Warn15, Message_Warn16, Message_Warn17,
  Message_Warn18, Message_Warn19, Message_Warn20, Message_Warn21,
  Message_Warn22, Message_Warn23, Message_Warn24, Message_Warn25,
  Message_Warn26, Message_Warn27, Message_Warn28, Message_Warn29,
  Message_Warn30, Message_Warn31, Message_Warn32,

  //! Alarm (severe warning) for problem encountered, 32 variants
  Message_Alarm1 = Message_ALARM, 
  Message_Alarm2,  Message_Alarm3,  Message_Alarm4,  Message_Alarm5,
  Message_Alarm6,  Message_Alarm7,  Message_Alarm8,  Message_Alarm9,
  Message_Alarm10, Message_Alarm11, Message_Alarm12, Message_Alarm13,
  Message_Alarm14, Message_Alarm15, Message_Alarm16, Message_Alarm17,
  Message_Alarm18, Message_Alarm19, Message_Alarm20, Message_Alarm21,
  Message_Alarm22, Message_Alarm23, Message_Alarm24, Message_Alarm25,
  Message_Alarm26, Message_Alarm27, Message_Alarm28, Message_Alarm29,
  Message_Alarm30, Message_Alarm31, Message_Alarm32,

  //! Execution failed, 32 variants
  Message_Fail1 = Message_FAIL, 
  Message_Fail2,  Message_Fail3,  Message_Fail4,  Message_Fail5,
  Message_Fail6,  Message_Fail7,  Message_Fail8,  Message_Fail9,
  Message_Fail10, Message_Fail11, Message_Fail12, Message_Fail13,
  Message_Fail14, Message_Fail15, Message_Fail16, Message_Fail17,
  Message_Fail18, Message_Fail19, Message_Fail20, Message_Fail21,
  Message_Fail22, Message_Fail23, Message_Fail24, Message_Fail25,
  Message_Fail26, Message_Fail27, Message_Fail28, Message_Fail29,
  Message_Fail30, Message_Fail31, Message_Fail32
};

#endif
