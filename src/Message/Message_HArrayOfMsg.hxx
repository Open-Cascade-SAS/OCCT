// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _Message_HArrayOfMsg_HeaderFile
#define _Message_HArrayOfMsg_HeaderFile

#include <Message_Msg.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Handle.hxx>

typedef NCollection_Handle<Message_Msg>         Handle(Message_Msg);
typedef NCollection_Array1<Handle(Message_Msg)> Message_ArrayOfMsg;
typedef NCollection_Handle<Message_ArrayOfMsg>  Handle(Message_ArrayOfMsg);
typedef Handle(Message_ArrayOfMsg)              Message_HArrayOfMsg;

#endif // _Message_HArrayOfMsg_HeaderFile
