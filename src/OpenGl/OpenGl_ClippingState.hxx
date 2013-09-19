// Created on: 2013-09-05
// Created by: Anton POLETAEV
// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _OpenGl_ClippingState_H__
#define _OpenGl_ClippingState_H__

#include <Aspect_GenId.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_SetOfHClipPlane.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Handle.hxx>
#include <Standard_TypeDef.hxx>
#include <OpenGl_Matrix.hxx>

//! This class contains logics related to tracking and modification of clipping plane
//! state for particular OpenGl context. It contains information about enabled
//! clipping planes and provides method to change clippings in context. The methods
//! should be executed within OpenGl context associated with instance of this
//! class.
class OpenGl_ClippingState
{
public:

  //! Default constructor.
  Standard_EXPORT OpenGl_ClippingState ();

  //! Initialize.
  //! @param theMaxPlanes [in] number of clipping planes supported by OpenGl context.
  Standard_EXPORT void Init (const Standard_Integer theMaxPlanes);

  //! @return sequence of set clipping planes.
  Standard_EXPORT Graphic3d_SetOfHClipPlane Planes() const;

  //! Check whether the clipping plane has been set for the current context state.
  //! @param thePlane [in] the plane to check.
  //! @return True if plane is set.
  Standard_EXPORT Standard_Boolean IsSet (const Handle(Graphic3d_ClipPlane)& thePlane) const;

  //! Set collection of clipping planes for available plane ids. Current model view matrix is used.
  //! @param thePlanes [in] collection of planes.
  //! @param theToEnable [in] the boolean flag notifying whether the planes should be enabled.
  Standard_EXPORT void Set (const Graphic3d_SetOfHClipPlane& thePlanes,
                            const Standard_Boolean theToEnable = Standard_True);

  //! Set collection of clipping planes for available plane ids. Identity matrix in case
  //! if passed matrix pointer is NULL.
  //! @param thePlanes [in] collection of planes.
  //! @param theViewMatrix [in] view matrix to be used to define plane equation.
  //! @param theToEnable [in] the boolean flag notifying whether the planes should be enabled.
  Standard_EXPORT void Set (const Graphic3d_SetOfHClipPlane& thePlanes,
                            const OpenGl_Matrix* theViewMatrix,
                            const Standard_Boolean theToEnable = Standard_True);

  //! Unset and disable collection of clipping planes.
  //! @param thePlanes [in] the plane to deactivate.
  Standard_EXPORT void Unset (const Graphic3d_SetOfHClipPlane& thePlanes);

  //! Check whether the clipping plane has been set and enabled for the current context state.
  //! @param thePlane [in] the plane to check.
  //! @return True if plane is enabled.
  Standard_EXPORT Standard_Boolean IsEnabled (const Handle(Graphic3d_ClipPlane)& thePlane) const;

  //! Change enabled / disabled state of the clipping plane.
  //! @param thePlane [in] the plane to change the state.
  //! @param theIsEnabled [in] the flag indicating whether the plane should be enabled or not.
  //! @return False if plane is not set for the context.
  Standard_EXPORT void SetEnabled (const Handle(Graphic3d_ClipPlane)& thePlane, const Standard_Boolean theIsEnabled);

private:

  typedef NCollection_DataMap<Handle(Graphic3d_ClipPlane), Standard_Integer> OpenGl_MapOfContextPlanes;
  typedef NCollection_DataMap<Handle(Graphic3d_ClipPlane), Standard_Boolean> OpenGl_MapOfPlaneStates;
  typedef NCollection_Handle<Aspect_GenId> OpenGl_EmptyPlaneIds;

  OpenGl_MapOfContextPlanes      myPlanes;          //!< map of clip planes bound for the ids
  OpenGl_MapOfPlaneStates        myPlaneStates;     //!< map of clip planes state (enabled/disabled).
  OpenGl_EmptyPlaneIds           myEmptyPlaneIds;   //!< generator of empty ids

private:

  //! Copying allowed only within Handles
  OpenGl_ClippingState            (const OpenGl_ClippingState& );
  OpenGl_ClippingState& operator= (const OpenGl_ClippingState& );

};

#endif
