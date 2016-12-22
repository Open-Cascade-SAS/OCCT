// Created on: 2013-09-05
// Created by: Anton POLETAEV
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

#ifndef _OpenGl_Clipping_H__
#define _OpenGl_Clipping_H__

#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_TypeDef.hxx>

class OpenGl_Context;
class OpenGl_ClippingIterator;

//! This class contains logics related to tracking and modification of clipping plane
//! state for particular OpenGl context. It contains information about enabled
//! clipping planes and provides method to change clippings in context. The methods
//! should be executed within OpenGl context associated with instance of this
//! class.
class OpenGl_Clipping
{
public: //! @name general methods

  //! Default constructor.
  Standard_EXPORT OpenGl_Clipping();

  //! Initialize.
  //! @param theMaxPlanes [in] number of clipping planes supported by OpenGl context.
  Standard_EXPORT void Init (const Standard_Integer theMaxPlanes);

  //! Setup list of global (for entire view) clipping planes
  //! and clears local plane list if it was not released before.
  Standard_EXPORT void Reset (const Handle(OpenGl_Context)& theGlCtx,
                              const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes);

  //! Setup list of local (for current object) clipping planes.
  Standard_EXPORT void SetLocalPlanes (const Handle(OpenGl_Context)& theGlCtx,
                                       const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes);

  //! @return true if there are enabled clipping planes (NOT capping)
  Standard_Boolean IsClippingOn() const { return myNbClipping > 0; }

  //! @return true if there are enabled capping planes
  Standard_Boolean IsCappingOn() const { return myNbCapping > 0; }

  //! @return true if there are enabled clipping or capping planes
  Standard_Boolean IsClippingOrCappingOn() const { return (myNbClipping + myNbCapping) > 0; }

  //! @return number of enabled clipping + capping planes
  Standard_Integer NbClippingOrCappingOn() const { return myNbClipping + myNbCapping; }

public: //! @name advanced method for disabling defined planes

  //! Return true if some clipping planes have been temporarily disabled.
  Standard_Boolean HasDisabled() const { return myNbDisabled > 0; }

  //! Disable plane temporarily.
  Standard_EXPORT Standard_Boolean SetEnabled (const Handle(OpenGl_Context)&  theGlCtx,
                                               const OpenGl_ClippingIterator& thePlane,
                                               const Standard_Boolean         theIsEnabled);

  //! Temporarily disable all planes from the global (view) list, keep only local (object) list.
  Standard_EXPORT void DisableGlobal (const Handle(OpenGl_Context)& theGlCtx);

  //! Restore all temporarily disabled planes.
  //! Does NOT affect constantly disabled planes Graphic3d_ClipPlane::IsOn().
  Standard_EXPORT void RestoreDisabled (const Handle(OpenGl_Context)& theGlCtx);

  //! Temporarily disable all planes except specified one.
  //! Does not affect already disabled planes.
  Standard_EXPORT void DisableAllExcept (const Handle(OpenGl_Context)&  theGlCtx,
                                         const OpenGl_ClippingIterator& thePlane);

  //! Enable back planes disabled by ::DisableAllExcept().
  //! Keeps only specified plane enabled.
  Standard_EXPORT void EnableAllExcept (const Handle(OpenGl_Context)&  theGlCtx,
                                        const OpenGl_ClippingIterator& thePlane);

protected: //! @name clipping state modification commands

  //! Add planes to the context clipping at the specified system of coordinates.
  //! This methods loads appropriate transformation matrix from workspace to
  //! to transform equation coordinates. The planes become enabled in the context.
  //! If the number of the passed planes exceeds capabilities of OpenGl, the last planes
  //! are simply ignored.
  //!
  //! Within FFP, method also temporarily resets ModelView matrix before calling glClipPlane().
  //! Otherwise the method just redirects to addLazy().
  //!
  //! @param theGlCtx [in] context to access the matrices
  //! @param thePlanes [in/out] the list of planes to be added
  //! The list then provides information on which planes were really added to clipping state.
  //! This list then can be used to fall back to previous state.
  Standard_EXPORT void add (const Handle(OpenGl_Context)& theGlCtx,
                            const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes,
                            const Standard_Integer theStartIndex);

  //! Remove the passed set of clipping planes from the context state.
  //! @param thePlanes [in] the planes to remove from list.
  Standard_EXPORT void remove (const Handle(OpenGl_Context)& theGlCtx,
                               const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes,
                               const Standard_Integer theStartIndex);

private:

  Handle(Graphic3d_SequenceOfHClipPlane)   myPlanesGlobal;   //!< global clipping planes
  Handle(Graphic3d_SequenceOfHClipPlane)   myPlanesLocal;    //!< object clipping planes
  NCollection_Vector<Standard_Boolean>     myDisabledPlanes; //!< ids of disabled planes
  NCollection_Vector<Standard_Boolean>     mySkipFilter;     //!< ids of planes that were disabled before calling ::DisableAllExcept()
  Standard_Integer                         myNbClipping;     //!< number of enabled clipping-only planes (NOT capping)
  Standard_Integer                         myNbCapping;      //!< number of enabled capping  planes
  Standard_Integer                         myNbDisabled;     //!< number of defined but disabled planes

private:

  //! Copying allowed only within Handles
  OpenGl_Clipping            (const OpenGl_Clipping& );
  OpenGl_Clipping& operator= (const OpenGl_Clipping& );

  friend class OpenGl_ClippingIterator;
};

//! The iterator through clipping planes.
class OpenGl_ClippingIterator
{
public:

  //! Main constructor.
  Standard_EXPORT OpenGl_ClippingIterator(const OpenGl_Clipping& theClipping);

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

  //! Return true if plane has been temporarily disabled
  //! either by Graphic3d_ClipPlane->IsOn() property or by temporary filter.
  bool IsDisabled() const { return myDisabled->Value (myCurrIndex) || !Value()->IsOn(); }

  //! Return the plane at current iterator position.
  const Handle(Graphic3d_ClipPlane)& Value() const
  {
    return myIter1.More()
         ? myIter1.Value()
         : myIter2.Value();
  }

  //! Return true if plane from the global (view) list.
  bool IsGlobal() const { return myIter1.More(); }

  //! Return the plane index.
  Standard_Integer PlaneIndex() const { return myCurrIndex; }

private:

  Graphic3d_SequenceOfHClipPlane::Iterator myIter1;
  Graphic3d_SequenceOfHClipPlane::Iterator myIter2;
  const NCollection_Vector<Standard_Boolean>* myDisabled;
  Standard_Integer myCurrIndex;

};

#endif
