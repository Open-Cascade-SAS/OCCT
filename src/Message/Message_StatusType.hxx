// File:      Message_StatusType.hxx
// Created:   06.07.07 11:43:48
// Author:    Pavel TELKOV
// Copyright: Open CASCADE S.A. 2007
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
