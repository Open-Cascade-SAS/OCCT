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

IMPLEMENT_STANDARD_RTTIEXT(Message_ProgressIndicator, Standard_Transient)

//=================================================================================================

Message_ProgressIndicator::Message_ProgressIndicator()
    : myPosition(0.),
      myRootScope(NULL)
{
  myRootScope = new Message_ProgressScope(this);
}

//=================================================================================================

Message_ProgressIndicator::~Message_ProgressIndicator()
{
  // Avoid calling Increment() from myRootScope.Close()
  myRootScope->myProgress = 0;
  myRootScope->myIsActive = false;
  delete myRootScope;
}

//=================================================================================================

Message_ProgressRange Message_ProgressIndicator::Start()
{
  myPosition           = 0.;
  myRootScope->myValue = 0.;
  Reset();
  Show(*myRootScope, Standard_False);
  return myRootScope->Next();
}

//=================================================================================================

Message_ProgressRange Message_ProgressIndicator::Start(
  const Handle(Message_ProgressIndicator)& theProgress)
{
  return theProgress.IsNull() ? Message_ProgressRange() : theProgress->Start();
}
