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

#include <Message_ProgressSentry.ixx>

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
