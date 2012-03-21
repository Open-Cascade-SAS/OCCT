// Created on: 2007-07-06
// Created by: Pavel TELKOV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
