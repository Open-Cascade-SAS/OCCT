// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <IGESData_DirPart.hxx>
#include <IGESData_IGESType.hxx>
#include <Standard_PCharacter.hxx>

IGESData_DirPart::IGESData_DirPart() {}

void IGESData_DirPart::Init(const int   i1,
                            const int   i2,
                            const int   i3,
                            const int   i4,
                            const int   i5,
                            const int   i6,
                            const int   i7,
                            const int   i8,
                            const int   i9,
                            const int   i10,
                            const int   i11,
                            const int   i12,
                            const int   i13,
                            const int   i14,
                            const int   i15,
                            const int   i16,
                            const int   i17,
                            const char* res1,
                            const char* res2,
                            const char* label,
                            const char* subscript)
{
  thevals[0]  = i1;
  thevals[1]  = i2;
  thevals[2]  = i3;
  thevals[3]  = i4;
  thevals[4]  = i5;
  thevals[5]  = i6;
  thevals[6]  = i7;
  thevals[7]  = i8;
  thevals[8]  = i9;
  thevals[9]  = i10;
  thevals[10] = i11;
  thevals[11] = i12;
  thevals[12] = i13;
  thevals[13] = i14;
  thevals[14] = i15;
  thevals[15] = i16;
  thevals[16] = i17;
  for (int i = 0; i < 8; i++)
  {
    theres1[i] = res1[i];
    theres2[i] = res2[i];
    thelabl[i] = label[i];
    thesubs[i] = subscript[i];
  }
  theres1[8] = '\0';
  theres2[8] = '\0';
  thelabl[8] = '\0';
  thesubs[8] = '\0';
}

//   CString : we modify their CONTENT, not the POINTER ITSELF

//=================================================================================================

void IGESData_DirPart::Values(int&        i1,
                              int&        i2,
                              int&        i3,
                              int&        i4,
                              int&        i5,
                              int&        i6,
                              int&        i7,
                              int&        i8,
                              int&        i9,
                              int&        i10,
                              int&        i11,
                              int&        i12,
                              int&        i13,
                              int&        i14,
                              int&        i15,
                              int&        i16,
                              int&        i17,
                              const char* res1,
                              const char* res2,
                              const char* label,
                              const char* subscript) const
{
  Standard_PCharacter pres1, pres2, plabel, psubscript;
  int                 i;
  //
  pres1      = (Standard_PCharacter)res1;
  pres2      = (Standard_PCharacter)res2;
  plabel     = (Standard_PCharacter)label;
  psubscript = (Standard_PCharacter)subscript;
  //
  i1  = thevals[0];
  i2  = thevals[1];
  i3  = thevals[2];
  i4  = thevals[3];
  i5  = thevals[4];
  i6  = thevals[5];
  i7  = thevals[6];
  i8  = thevals[7];
  i9  = thevals[8];
  i10 = thevals[9];
  i11 = thevals[10];
  i12 = thevals[11];
  i13 = thevals[12];
  i14 = thevals[13];
  i15 = thevals[14];
  i16 = thevals[15];
  i17 = thevals[16];
  for (i = 0; i < 8; ++i)
  {
    pres1[i]      = theres1[i];
    pres2[i]      = theres2[i];
    plabel[i]     = thelabl[i];
    psubscript[i] = thesubs[i];
  }
  pres1[8]      = '\0';
  pres2[8]      = '\0';
  plabel[8]     = '\0';
  psubscript[8] = '\0';
}

IGESData_IGESType IGESData_DirPart::Type() const
{
  return IGESData_IGESType(thevals[0], thevals[16]);
} // type & forme
