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

#ifndef _Aspect_XRActionSet_HeaderFile
#define _Aspect_XRActionSet_HeaderFile

#include <Aspect_XRAction.hxx>

//! XR action set.
class Aspect_XRActionSet : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Aspect_XRActionSet, Standard_Transient)
public:
  //! Return action id.
  const TCollection_AsciiString& Id() const { return myId; }

  //! Return action handle.
  uint64_t RawHandle() const { return myRawHandle; }

  //! Set action handle.
  void SetRawHandle(uint64_t theHande) { myRawHandle = theHande; }

  //! Add action.
  void AddAction(const Handle(Aspect_XRAction)& theAction)
  {
    myActions.Add(theAction->Id(), theAction);
  }

  //! Return map of actions.
  const Aspect_XRActionMap& Actions() const { return myActions; }

  //! Main constructor.
  Aspect_XRActionSet(const TCollection_AsciiString& theId)
      : myId(theId),
        myRawHandle(0)
  {
  }

protected:
  TCollection_AsciiString myId;        //!< action set id
  uint64_t                myRawHandle; //!< action set handle
  Aspect_XRActionMap      myActions;   //!< map of actions
};

typedef NCollection_IndexedDataMap<TCollection_AsciiString, Handle(Aspect_XRActionSet)>
  Aspect_XRActionSetMap;

#endif // _Aspect_XRActionSet_HeaderFile
