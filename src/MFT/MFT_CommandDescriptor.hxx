// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#ifndef _MFT_CommandDescriptor_HeaderFile
#define _MFT_CommandDescriptor_HeaderFile

typedef unsigned int MFT_CommandDescriptor;

#define COMMAND_VALUETYPE(d,n) ((d >> (32 - 2*n)) & 0x3)
#define SET_COMMAND_VALUETYPE(d,n,t) (d |= t << (32 - 2*n))
#define COMMAND_LENGTH(d) ((d >> 8) & 0xFF)
#define SET_COMMAND_LENGTH(d,l) (d = (d & 0xFFFF00FF)  | (l << 8))
#define COMMAND_TYPE(d) MFT_TypeOfCommand(d & 0xFF)
#define SET_COMMAND_TYPE(d,t) (d = (d & 0xFFFFFF00) | t)

#ifndef _Standard_Type_HeaderFile
#include <Standard_Type.hxx>
#endif
const Handle(Standard_Type)& STANDARD_TYPE(MFT_CommandDescriptor);

#endif
