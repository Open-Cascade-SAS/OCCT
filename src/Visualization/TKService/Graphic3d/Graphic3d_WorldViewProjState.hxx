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
  Graphic3d_WorldViewProjState() { Reset(); }

  //! Constructor for custom projector type.
  //! @param[in] theProjectionState  the projection state.
  //! @param[in] theWorldViewState  the world view state.
  //! @param[in] theCamera  the pointer to the class supplying projection and
  //!                       world view matrices (camera).
  Graphic3d_WorldViewProjState(const size_t              theProjectionState,
                               const size_t              theWorldViewState,
                               const Standard_Transient* theCamera = nullptr)
  {
    Initialize(theProjectionState, theWorldViewState, theCamera);
  }

public:
  //! Check state validity.
  //! @return true if state is set.
  bool IsValid() { return myIsValid; }

  //! Invalidate world view projection state.
  void Reset()
  {
    myIsValid         = false;
    myCamera          = nullptr;
    myProjectionState = 0;
    myWorldViewState  = 0;
  }

  //! Initialize world view projection state.
  void Initialize(const size_t              theProjectionState,
                  const size_t              theWorldViewState,
                  const Standard_Transient* theCamera = nullptr)
  {
    myIsValid         = true;
    myCamera          = const_cast<Standard_Transient*>(theCamera);
    myProjectionState = theProjectionState;
    myWorldViewState  = theWorldViewState;
  }

  //! Initialize world view projection state.
  void Initialize(const Standard_Transient* theCamera = nullptr)
  {
    myIsValid         = true;
    myCamera          = const_cast<Standard_Transient*>(theCamera);
    myProjectionState = 0;
    myWorldViewState  = 0;
  }

public:
  //! @return projection state counter.
  size_t& ProjectionState() { return myProjectionState; }

  //! @return world view state counter.
  size_t& WorldViewState() { return myWorldViewState; }

public:
  //! Compare projection with other state.
  //! @return true when the projection of the given camera state differs from this one.
  bool IsProjectionChanged(const Graphic3d_WorldViewProjState& theState)
  {
    return myIsValid != theState.myIsValid || myCamera != theState.myCamera
           || myProjectionState != theState.myProjectionState;
  }

  //! Compare world view transformation with other state.
  //! @return true when the orientation of the given camera state differs from this one.
  bool IsWorldViewChanged(const Graphic3d_WorldViewProjState& theState)
  {
    return myIsValid != theState.myIsValid || myCamera != theState.myCamera
           || myWorldViewState != theState.myWorldViewState;
  }

  //! Compare with other world view projection state.
  //! @return true when the projection of the given camera state differs from this one.
  bool IsChanged(const Graphic3d_WorldViewProjState& theState) { return *this != theState; }

public:
  //! Compare with other world view projection state.
  //! @return true if the other projection state is different to this one.
  bool operator!=(const Graphic3d_WorldViewProjState& theOther) const
  {
    return !(*this == theOther);
  }

  //! Compare with other world view projection state.
  //! @return true if the other projection state is equal to this one.
  bool operator==(const Graphic3d_WorldViewProjState& theOther) const
  {
    return myIsValid == theOther.myIsValid && myCamera == theOther.myCamera
           && myProjectionState == theOther.myProjectionState
           && myWorldViewState == theOther.myWorldViewState;
  }

  //! Dumps the content of me into the stream
  void DumpJson(Standard_OStream& theOStream, int) const
  {
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsValid)
    OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myCamera)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myProjectionState)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myWorldViewState)
  }

private:
  bool                myIsValid;
  Standard_Transient* myCamera;
  size_t              myProjectionState;
  size_t              myWorldViewState;
};

#endif
