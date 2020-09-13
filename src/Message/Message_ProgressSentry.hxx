// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef Message_ProgressSentry_HeaderFile
#define Message_ProgressSentry_HeaderFile

#include <Message_ProgressScope.hxx>

//! Functionality of this class (Message_ProgressSentry) has been superseded by Message_ProgressScope.
//! This class is kept just to simplify transition of an old code and will be removed in future.
class Standard_DEPRECATED("Deprecated class, Message_ProgressScope should be used instead")
Message_ProgressSentry : public Message_ProgressScope
{
public:
  //! Deprecated constructor, Message_ProgressScope should be created instead.
  Message_ProgressSentry (const Message_ProgressRange& theRange,
                          const Standard_CString theName,
                          const Standard_Real theMin,
                          const Standard_Real theMax,
                          const Standard_Real theStep,
                          const Standard_Boolean theIsInf = Standard_False,
                          const Standard_Real theNewScopeSpan = 0.0)
  : Message_ProgressScope (theRange, theName, theMax, theIsInf)
  {
    if (theMin != 0.0 || theStep != 1.0 || theNewScopeSpan != 0.0)
    {
      throw Standard_ProgramError ("Message_ProgressSentry, invalid parameters");
    }
  }

  //! Method Relieve() was replaced by Close() in Message_ProgressScope
  void Relieve () { Close(); }

private:
  //! Message_ProgressRange should be passed to constructor instead of Message_ProgressIndicator.
  Message_ProgressSentry (const Handle(Message_ProgressIndicator)& theProgress,
                          const Standard_CString theName,
                          const Standard_Real theMin,
                          const Standard_Real theMax,
                          const Standard_Real theStep,
                          const Standard_Boolean theIsInf = Standard_False,
                          const Standard_Real theNewScopeSpan = 0.0);
};

#endif // Message_ProgressSentry_HeaderFile
