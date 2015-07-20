// Created on: 2015-06-30
// Created by: Anton POLETAEV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _Graphic3d_WorldViewProjState_HeaderFile
#define _Graphic3d_WorldViewProjState_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_TypeDef.hxx>

//! Helper class for keeping reference on world-view-projection state.
//! Helpful for synchronizing state of WVP dependent data structures.
class Graphic3d_WorldViewProjState
{
public:

  //! Default constructor.
  Graphic3d_WorldViewProjState()
  {
    Reset();
  }

  //! Constructor for custom projector type.
  //! @param theProjectionState [in] the projection state.
  //! @param theWorldViewState [in] the world view state.
  //! @param theCamera [in] the pointer to the class supplying projection and
  //!                       world view matrices (camera).
  Graphic3d_WorldViewProjState (const Standard_Size theProjectionState,
                                const Standard_Size theWorldViewState,
                                const Standard_Transient* theCamera = NULL)
  {
    Initialize (theProjectionState, theWorldViewState, theCamera);
  }

public:

  //! Check state validity.
  //! @return true if state is set.
  Standard_Boolean IsValid()
  {
    return myIsValid;
  }

  //! Invalidate world view projection state.
  void Reset()
  {
    myIsValid         = Standard_False;
    myCamera          = NULL;
    myProjectionState = 0;
    myWorldViewState  = 0;
  }

  //! Initialize world view projection state.
  void Initialize (const Standard_Size theProjectionState,
                   const Standard_Size theWorldViewState,
                   const Standard_Transient* theCamera = NULL)
  {
    myIsValid         = Standard_True;
    myCamera          = const_cast<Standard_Transient*> (theCamera);
    myProjectionState = theProjectionState;
    myWorldViewState  = theWorldViewState;
  }

  //! Initialize world view projection state.
  void Initialize (const Standard_Transient* theCamera = NULL)
  {
    myIsValid         = Standard_True;
    myCamera          = const_cast<Standard_Transient*> (theCamera);
    myProjectionState = 0;
    myWorldViewState  = 0;
  }

public:

  //! @return projection state counter.
  Standard_Size& ProjectionState()
  {
    return myProjectionState;
  }

  //! @return world view state counter.
  Standard_Size& WorldViewState()
  {
    return myWorldViewState;
  }

public:

  //! Compare projection with other state.
  //! @return true when the projection of the given camera state differs from this one.
  Standard_Boolean IsProjectionChanged (const Graphic3d_WorldViewProjState& theState)
  {
    return myIsValid         != theState.myIsValid
        || myCamera          != theState.myCamera
        || myProjectionState != theState.myProjectionState;
  }

  //! Compare world view transformation with other state.
  //! @return true when the orientation of the given camera state differs from this one.
  Standard_Boolean IsWorldViewChanged (const Graphic3d_WorldViewProjState& theState)
  {
    return myIsValid        != theState.myIsValid
        || myCamera         != theState.myCamera
        || myWorldViewState != theState.myWorldViewState;
  }

  //! Compare with other world view projection state.
  //! @return true when the projection of the given camera state differs from this one.
  Standard_Boolean IsChanged (const Graphic3d_WorldViewProjState& theState)
  {
    return *this != theState;
  }

public:

  //! Compare with other world view projection state.
  //! @return true if the other projection state is different to this one.
  bool operator != (const Graphic3d_WorldViewProjState& theOther) const
  {
    return !(*this == theOther);
  }

  //! Compare with other world view projection state.
  //! @return true if the other projection state is equal to this one.
  bool operator == (const Graphic3d_WorldViewProjState& theOther) const
  {
    return myIsValid         == theOther.myIsValid
        && myCamera          == theOther.myCamera
        && myProjectionState == theOther.myProjectionState
        && myWorldViewState  == theOther.myWorldViewState;
  }

  //! Copy world view projection state.
  void operator = (const Graphic3d_WorldViewProjState& theOther)
  {
    myIsValid         = theOther.myIsValid;
    myCamera          = theOther.myCamera;
    myProjectionState = theOther.myProjectionState;
    myWorldViewState  = theOther.myWorldViewState;
  }

private:

  Standard_Boolean    myIsValid;
  Standard_Transient* myCamera;
  Standard_Size       myProjectionState;
  Standard_Size       myWorldViewState;
};

#endif
