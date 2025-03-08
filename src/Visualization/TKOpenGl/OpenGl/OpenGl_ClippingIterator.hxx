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

#ifndef OpenGl_ClippingIterator_Header
#define OpenGl_ClippingIterator_Header

#include <OpenGl_Clipping.hxx>

//! The iterator through clipping planes.
class OpenGl_ClippingIterator
{
public:
  //! Main constructor.
  OpenGl_ClippingIterator(const OpenGl_Clipping& theClipping)
      : myDisabled(&theClipping.myDisabledPlanes),
        myCurrIndex(1)
  {
    myIter1.Init(theClipping.myPlanesGlobal);
    myIter2.Init(theClipping.myPlanesLocal);
  }

  //! Return true if iterator points to the valid clipping plane.
  bool More() const { return myIter1.More() || myIter2.More(); }

  //! Go to the next clipping plane.
  void Next()
  {
    ++myCurrIndex;
    if (myIter1.More())
    {
      myIter1.Next();
    }
    else
    {
      myIter2.Next();
    }
  }

  //! Return true if plane has been temporarily disabled either by Graphic3d_ClipPlane->IsOn()
  //! property or by temporary filter. Beware that this method does NOT handle a Chain filter for
  //! Capping algorithm OpenGl_Clipping::CappedChain()!
  bool IsDisabled() const { return myDisabled->Value(myCurrIndex) || !Value()->IsOn(); }

  //! Return the plane at current iterator position.
  const Handle(Graphic3d_ClipPlane)& Value() const
  {
    return myIter1.More() ? myIter1.Value() : myIter2.Value();
  }

  //! Return true if plane from the global (view) list.
  bool IsGlobal() const { return myIter1.More(); }

  //! Return the plane index.
  Standard_Integer PlaneIndex() const { return myCurrIndex; }

private:
  Graphic3d_SequenceOfHClipPlane::Iterator    myIter1;
  Graphic3d_SequenceOfHClipPlane::Iterator    myIter2;
  const NCollection_Vector<Standard_Boolean>* myDisabled;
  Standard_Integer                            myCurrIndex;
};

#endif // OpenGl_ClippingIterator_Header
