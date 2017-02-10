// Created on: 2017-08-10
// Created by: Sergey NIKONOV
// Copyright (c) 2005-2017 OPEN CASCADE SAS
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

#ifndef _BinMXCAFDoc_NoteBalloonDriver_HeaderFile
#define _BinMXCAFDoc_NoteBalloonDriver_HeaderFile

#include <BinMXCAFDoc_NoteCommentDriver.hxx>

class BinMXCAFDoc_NoteBalloonDriver;
DEFINE_STANDARD_HANDLE(BinMXCAFDoc_NoteBalloonDriver, BinMXCAFDoc_NoteCommentDriver)

class BinMXCAFDoc_NoteBalloonDriver : public BinMXCAFDoc_NoteCommentDriver
{
public:
  
  Standard_EXPORT BinMXCAFDoc_NoteBalloonDriver(const Handle(CDM_MessageDriver)& theMsgDriver);
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  DEFINE_STANDARD_RTTIEXT(BinMXCAFDoc_NoteBalloonDriver, BinMXCAFDoc_NoteCommentDriver)


protected:

  BinMXCAFDoc_NoteBalloonDriver(const Handle(CDM_MessageDriver)& theMsgDriver,
                                Standard_CString                 theName);

};

#endif // _BinMXCAFDoc_NoteCommentDriver_HeaderFile
