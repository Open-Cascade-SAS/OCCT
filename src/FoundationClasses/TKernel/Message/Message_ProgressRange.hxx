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

#ifndef _Message_ProgressRange_HeaderFile
#define _Message_ProgressRange_HeaderFile

#include <Standard_TypeDef.hxx>

class Message_ProgressScope;

//! Auxiliary class representing a part of the global progress scale allocated by
//! a step of the progress scope, see Message_ProgressScope::Next().
//!
//! A range object takes responsibility of advancing the progress by the size of
//! allocated step, which is then performed depending on how it is used:
//!
//! - If Message_ProgressScope object is created using this range as argument, then
//!   this respondibility is taken over by that scope.
//!
//! - Otherwise, a range advances progress directly upon destruction.
//!
//! A range object can be copied, the responsibility for progress advancement is
//! then taken by the copy.
//! The same range object may be used (either copied or used to create scope) only once.
//! Any consequent attempts to use range will give no result on the progress;
//! in debug mode, an assert message will be generated.
//!
//! @sa Message_ProgressScope for more details
class Message_ProgressRange
{
public:
  //! Constructor of the empty range
  Message_ProgressRange()
      : myParentScope(0),
        myStart(0.),
        myDelta(0.),
        myWasUsed(false)
  {
  }

  //! Copy constructor disarms the source
  Message_ProgressRange(const Message_ProgressRange& theOther)
      : myParentScope(theOther.myParentScope),
        myStart(theOther.myStart),
        myDelta(theOther.myDelta),
        myWasUsed(theOther.myWasUsed)
  {
    // discharge theOther
    theOther.myWasUsed = true;
  }

  //! Copy assignment disarms the source
  Message_ProgressRange& operator=(const Message_ProgressRange& theOther)
  {
    myParentScope      = theOther.myParentScope;
    myStart            = theOther.myStart;
    myDelta            = theOther.myDelta;
    myWasUsed          = theOther.myWasUsed;
    theOther.myWasUsed = true;
    return *this;
  }

  //! Returns true if ProgressIndicator signals UserBreak
  Standard_Boolean UserBreak() const;

  //! Returns false if ProgressIndicator signals UserBreak
  Standard_Boolean More() const { return !UserBreak(); }

  //! Returns true if this progress range is attached to some indicator.
  Standard_Boolean IsActive() const;

  //! Closes the current range and advances indicator
  void Close();

  //! Destructor
  ~Message_ProgressRange() { Close(); }

private:
  //! Constructor is private
  Message_ProgressRange(const Message_ProgressScope& theParent,
                        Standard_Real                theStart,
                        Standard_Real                theDelta)
      : myParentScope(&theParent),
        myStart(theStart),
        myDelta(theDelta),
        myWasUsed(false)
  {
  }

private:
  const Message_ProgressScope* myParentScope; //!< Pointer to parent scope
  Standard_Real                myStart;       //!< Start point on the global scale
  Standard_Real                myDelta;       //!< Step of incrementation on the global scale

  mutable Standard_Boolean myWasUsed; //!< Flag indicating that this range
                                      //!  was used to create a new scope

  friend class Message_ProgressScope;
};

#include <Message_ProgressIndicator.hxx>

//=======================================================================
// function : IsActive
// purpose  :
//=======================================================================
inline Standard_Boolean Message_ProgressRange::IsActive() const
{
  return !myWasUsed && myParentScope && myParentScope->myProgress;
}

//=======================================================================
// function : UserBreak
// purpose  :
//=======================================================================
inline Standard_Boolean Message_ProgressRange::UserBreak() const
{
  return myParentScope && myParentScope->myProgress && myParentScope->myProgress->UserBreak();
}

//=======================================================================
// function : Close
// purpose  :
//=======================================================================
inline void Message_ProgressRange::Close()
{
  if (!IsActive())
    return;

  myParentScope->myProgress->Increment(myDelta, *myParentScope);
  myParentScope = 0;
  myWasUsed     = true;
}

#endif // _Message_ProgressRange_HeaderFile
