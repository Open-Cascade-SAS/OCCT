// Copyright (c) 2017-2021 OPEN CASCADE SAS
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

#ifndef _Message_LazyProgressScope_HeaderFiler
#define _Message_LazyProgressScope_HeaderFiler

#include <Message_ProgressScope.hxx>

//! Progress scope with lazy updates and abort fetches.
//!
//! Although Message_ProgressIndicator implementation is encouraged to spare GUI updates,
//! even optimized implementation might show a noticeable overhead on a very small update step (e.g.
//! per triangle).
//!
//! The class splits initial (displayed) number of overall steps into larger chunks specified in
//! constructor, so that displayed progress is updated at larger steps.
class Message_LazyProgressScope : protected Message_ProgressScope
{
public:
  //! Main constructor.
  //! @param[in] theRange  progress range to scope
  //! @param[in] theName   name of this scope
  //! @param[in] theMax    number of steps within this scope
  //! @param[in] thePatchStep  number of steps to update progress
  //! @param[in] theIsInf  infinite flag
  Message_LazyProgressScope(const Message_ProgressRange& theRange,
                            const char*                  theName,
                            const Standard_Real          theMax,
                            const Standard_Real          thePatchStep,
                            const Standard_Boolean       theIsInf = Standard_False)
      : Message_ProgressScope(theRange, theName, theMax, theIsInf),
        myPatchStep(thePatchStep),
        myPatchProgress(0.0),
        myIsLazyAborted(Standard_False)
  {
  }

  //! Increment progress with 1.
  void Next()
  {
    if (++myPatchProgress < myPatchStep)
    {
      return;
    }

    myPatchProgress = 0.0;
    Message_ProgressScope::Next(myPatchStep);
    IsAborted();
  }

  //! Return TRUE if progress has been aborted - return the cached state lazily updated.
  Standard_Boolean More() const { return !myIsLazyAborted; }

  //! Return TRUE if progress has been aborted - fetches actual value from the Progress.
  Standard_Boolean IsAborted()
  {
    myIsLazyAborted = myIsLazyAborted || !Message_ProgressScope::More();
    return myIsLazyAborted;
  }

protected:
  Standard_Real    myPatchStep;
  Standard_Real    myPatchProgress;
  Standard_Boolean myIsLazyAborted;
};

#endif // _Message_LazyProgressScope_HeaderFiler
