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


#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressSentry.hxx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : Message_ProgressSentry
//purpose  : 
//=======================================================================
Message_ProgressSentry::Message_ProgressSentry (const Handle(Message_ProgressIndicator) &progress,
                                                  const Standard_CString name,
                                                  const Standard_Real min, 
                                                  const Standard_Real max, 
                                                  const Standard_Real step,
                                                  const Standard_Boolean isInf,
                                                  const Standard_Real newScopeSpan) :
       myProgress(progress), myActive(!progress.IsNull())
{
  if ( ! myActive ) return;
  progress->SetName ( name );
  progress->SetScale ( min, max, step, isInf );
  progress->NewScope ( newScopeSpan >0 ? newScopeSpan : step );
}

//=======================================================================
//function : Message_ProgressSentry
//purpose  : 
//=======================================================================

Message_ProgressSentry::Message_ProgressSentry (const Handle(Message_ProgressIndicator) &progress,
                                                  const Handle(TCollection_HAsciiString) &name,
                                                  const Standard_Real min, 
                                                  const Standard_Real max, 
                                                  const Standard_Real step,
                                                  const Standard_Boolean isInf,
                                                  const Standard_Real newScopeSpan) :
       myProgress(progress), myActive(!progress.IsNull())
{
  if ( ! myActive ) return;
  progress->SetName ( name );
  progress->SetScale ( min, max, step, isInf );
  progress->NewScope ( newScopeSpan >0 ? newScopeSpan : step );
}
