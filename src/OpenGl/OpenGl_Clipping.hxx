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

#ifndef OpenGl_Clipping_HeaderFile
#define OpenGl_Clipping_HeaderFile

#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_TypeDef.hxx>

class OpenGl_ClippingIterator;

//! This class contains logics related to tracking and modification of clipping plane
//! state for particular OpenGl context. It contains information about enabled
//! clipping planes and provides method to change clippings in context. The methods
//! should be executed within OpenGl context associated with instance of this
//! class.
class OpenGl_Clipping
{
  friend class OpenGl_ClippingIterator;
public: //! @name general methods

  //! Default constructor.
  Standard_EXPORT OpenGl_Clipping();

  //! Initialize.
  Standard_EXPORT void Init();

  //! Setup list of global (for entire view) clipping planes
  //! and clears local plane list if it was not released before.
  Standard_EXPORT void Reset (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes);

  //! Setup list of local (for current object) clipping planes.
  Standard_EXPORT void SetLocalPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes);

  //! @return true if there are enabled capping planes
  Standard_Boolean IsCappingOn() const { return myNbCapping > 0; }

  //! @return true if there are enabled clipping or capping planes
  Standard_Boolean IsClippingOrCappingOn() const { return NbClippingOrCappingOn() > 0; }

  //! @return number of enabled clipping + capping planes
  Standard_Integer NbClippingOrCappingOn() const
  {
    if (IsCappingDisableAllExcept())
    {
      return 1; // all Chains are disabled - only single (sub) plane is active
    }
    return myNbClipping + myNbCapping
        + (IsCappingEnableAllExcept() ? -1 : 0); // exclude 1 plane with Capping filter turned ON
  }

  //! Return TRUE if there are clipping chains in the list (defining more than 1 sub-plane)
  Standard_Boolean HasClippingChains() const
  {
    if (IsCappingDisableAllExcept()                 // all chains are disabled - only single (sub) plane is active;
     || myNbChains == (myNbClipping + myNbCapping)) // no sub-planes
    {
      return Standard_False;
    }
    return !IsCappingEnableAllExcept()
         || myCappedChain->NbChainNextPlanes() == 1
         || myNbChains > 1; // if capping filter ON - chains counter should be decremented
  }

public: //! @name advanced method for disabling defined planes

  //! Return true if some clipping planes have been temporarily disabled.
  Standard_Boolean HasDisabled() const { return myNbDisabled > 0; }

  //! Disable plane temporarily.
  Standard_EXPORT Standard_Boolean SetEnabled (const OpenGl_ClippingIterator& thePlane,
                                               const Standard_Boolean         theIsEnabled);

  //! Temporarily disable all planes from the global (view) list, keep only local (object) list.
  Standard_EXPORT void DisableGlobal();

  //! Restore all temporarily disabled planes.
  //! Does NOT affect constantly disabled planes Graphic3d_ClipPlane::IsOn().
  Standard_EXPORT void RestoreDisabled();

//! @name capping algorithm filter
public:

  //! Chain which is either temporary disabled or the only one enabled for Capping algorithm.
  const Handle(Graphic3d_ClipPlane)& CappedChain() const { return myCappedChain; }

  //! Sub-plane index within filtered Chain; positive number for DisableAllExcept and negative for EnableAllExcept.
  Standard_Integer CappedSubPlane() const { return myCappedSubPlane; }

  //! Return TRUE if capping algorithm is in state, when all clipping planes are temporarily disabled except currently processed one.
  bool IsCappingFilterOn() const { return !myCappedChain.IsNull(); }

  //! Return TRUE if capping algorithm is in state, when all clipping planes are temporarily disabled except currently processed one.
  bool IsCappingDisableAllExcept() const { return myCappedSubPlane > 0; }

  //! Return TRUE if capping algorithm is in state, when all clipping planes are enabled except currently rendered one.
  bool IsCappingEnableAllExcept() const { return myCappedSubPlane < 0; }

  //! Temporarily disable all planes except specified one for Capping algorithm.
  //! Does not affect already disabled planes.
  Standard_EXPORT void DisableAllExcept (const Handle(Graphic3d_ClipPlane)& theChain,
                                         const Standard_Integer theSubPlaneIndex);

  //! Enable back planes disabled by ::DisableAllExcept() for Capping algorithm.
  //! Keeps only specified plane enabled.
  Standard_EXPORT void EnableAllExcept (const Handle(Graphic3d_ClipPlane)& theChain,
                                        const Standard_Integer theSubPlaneIndex);

  //! Resets chain filter for Capping algorithm.
  Standard_EXPORT void ResetCappingFilter();

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
  //! @param thePlanes [in/out] the list of planes to be added
  //! The list then provides information on which planes were really added to clipping state.
  //! This list then can be used to fall back to previous state.
  Standard_EXPORT void add (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes,
                            const Standard_Integer theStartIndex);

  //! Remove the passed set of clipping planes from the context state.
  //! @param thePlanes [in] the planes to remove from list.
  Standard_EXPORT void remove (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes,
                               const Standard_Integer theStartIndex);

private:

  Handle(Graphic3d_SequenceOfHClipPlane)   myPlanesGlobal;   //!< global clipping planes
  Handle(Graphic3d_SequenceOfHClipPlane)   myPlanesLocal;    //!< object clipping planes
  NCollection_Vector<Standard_Boolean>     myDisabledPlanes; //!< ids of disabled planes

  Handle(Graphic3d_ClipPlane)              myCappedChain;    //!< chain which is either temporary disabled or the only one enabled for Capping algorithm
  Standard_Integer                         myCappedSubPlane; //!< sub-plane index within filtered chain; positive number for DisableAllExcept and negative for EnableAllExcept

  Standard_Integer                         myNbClipping;     //!< number of enabled clipping-only planes (NOT capping)
  Standard_Integer                         myNbCapping;      //!< number of enabled capping  planes
  Standard_Integer                         myNbChains;       //!< number of enabled chains
  Standard_Integer                         myNbDisabled;     //!< number of defined but disabled planes

private:
  //! Copying allowed only within Handles
  OpenGl_Clipping            (const OpenGl_Clipping& );
  OpenGl_Clipping& operator= (const OpenGl_Clipping& );
};

#endif
