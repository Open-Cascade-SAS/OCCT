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

//! @file Message_ListOfMsg.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _Message_ListOfMsg_hxx
#define _Message_ListOfMsg_hxx

#include <Standard_Macro.hxx>
#include <Message_Msg.hxx>
#include <NCollection_List.hxx>

Standard_HEADER_DEPRECATED(
  "Message_ListOfMsg.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED("Message_ListOfMsg is deprecated, use NCollection_List<Message_Msg> directly")
typedef NCollection_List<Message_Msg> Message_ListOfMsg;
Standard_DEPRECATED("Message_ListIteratorOfListOfMsg is deprecated, use "
                    "NCollection_List<Message_Msg>::Iterator directly")
typedef NCollection_List<Message_Msg>::Iterator Message_ListIteratorOfListOfMsg;

#endif // _Message_ListOfMsg_hxx
