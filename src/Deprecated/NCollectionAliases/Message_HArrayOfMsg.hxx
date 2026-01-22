// Copyright (c) 2025 OPEN CASCADE SAS
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

//! @file Message_HArrayOfMsg.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _Message_HArrayOfMsg_hxx
#define _Message_HArrayOfMsg_hxx

#include <Standard_Macro.hxx>
#include <Message_Msg.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Handle.hxx>

Standard_HEADER_DEPRECATED("Message_HArrayOfMsg.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("Message_ArrayOfMsg is deprecated, use NCollection_Array1<NCollection_Handle<Message_Msg>> directly")
typedef NCollection_Array1<NCollection_Handle<Message_Msg>> Message_ArrayOfMsg;
Standard_DEPRECATED("Message_HArrayOfMsg is deprecated, use NCollection_Handle<Message_ArrayOfMsg> directly")
typedef NCollection_Handle<Message_ArrayOfMsg> Message_HArrayOfMsg;

#endif // _Message_HArrayOfMsg_hxx
