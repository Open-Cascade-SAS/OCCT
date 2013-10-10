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
#include <Handle_OpenGl_Workspace.hxx>

//! This class contains logics related to tracking and modification of clipping plane
//! state for particular OpenGl context. It contains information about enabled
//! clipping planes and provides method to change clippings in context. The methods
//! should be executed within OpenGl context associated with instance of this
//! class.
class OpenGl_ClippingState
{
public:

  //! Enumerates supported equation coordinate spaces.
  enum EquationCoords
  {
    EquationCoords_View,
    EquationCoords_World
  };

public: //! @name general methods

  //! Default constructor.
  Standard_EXPORT OpenGl_ClippingState();

  //! Initialize.
  //! @param theMaxPlanes [in] number of clipping planes supported by OpenGl context.
  Standard_EXPORT void Init (const Standard_Integer theMaxPlanes);

public: //! @name non-modifying getters

  //! Check whether the clipping plane has been added to the current context state.
  //! @param thePlane [in] the plane to check.
  //! @return True if plane is set.
  inline Standard_Boolean Contains (const Handle(Graphic3d_ClipPlane)& thePlane) const
  {
    return myPlaneStates.IsBound (thePlane);
  }

  //! Get clip planes defined for context.
  //! @return sequence of set clipping planes.
  inline const Graphic3d_SetOfHClipPlane& Planes() const
  {
    return myPlanes;
  }

  //! @return kind of equation coordinate space used for the clip plane.
  inline EquationCoords GetEquationSpace (const Handle(Graphic3d_ClipPlane)& thePlane) const
  {
    return myPlaneStates.Find (thePlane).CoordSpace;
  }

  //! Check whether the clipping plane has been set and enabled for the current context state.
  //! @param thePlane [in] the plane to check.
  //! @return True if plane is enabled.
  inline Standard_Boolean IsEnabled (const Handle(Graphic3d_ClipPlane)& thePlane) const
  {
    return myPlaneStates.Find (thePlane).IsEnabled;
  }

public: //! @name clipping state modification commands

  //! Add planes to the context clipping at the specified system of coordinates.
  //! This methods loads appropriate transformation matrix from workspace to
  //! to transform equation coordinates. The planes become enabled in the context.
  //! If the number of the passed planes exceeds capabilities of OpenGl, the last planes
  //! are simply ignored.
  //! @param thePlanes [in/out] the list of planes to be added.
  //! The list then provides information on which planes were really added to clipping state.
  //! This list then can be used to fall back to previous state.
  //! @param theCoordSpace [in] the equation definition space.
  //! @param theWS [in] the workspace to access the matrices.
  Standard_EXPORT void Add (Graphic3d_SetOfHClipPlane& thePlanes,
                            const EquationCoords& theCoordSpace,
                            const Handle(OpenGl_Workspace)& theWS);

  //! Add planes to the context clipping at the specified system of coordinates.
  //! This method assumes that appropriate matrix is already set in context state.
  //! If the number of the passed planes exceeds capabilities of OpenGl, the last planes
  //! are simply ignored.
  //! @param thePlanes [in/out] the list of planes to be added.
  //! The list then provides information on which planes were really added to clipping state.
  //! This list then can be used to fall back to previous state.
  //! @param theCoordSpace [in] the equation definition space.
  Standard_EXPORT void Add (Graphic3d_SetOfHClipPlane& thePlanes,
                            const EquationCoords& theCoordSpace);

  //! Remove the passed set of clipping planes from the context state.
  //! @param thePlanes [in] the planes to remove from list.
  Standard_EXPORT void Remove (const Graphic3d_SetOfHClipPlane& thePlanes);

  //! Enable or disable clipping plane in the OpenGl context.
  //! @param thePlane [in] the plane to affect.
  //! @param theIsEnabled [in] the state of the plane.
  Standard_EXPORT void SetEnabled (const Handle(Graphic3d_ClipPlane)& thePlane,
                                   const Standard_Boolean theIsEnabled);

public: //! @name Short-cuts

  //! Add planes to the context clipping at the view system of coordinates.
  //! If the number of the passed planes exceeds capabilities of OpenGl, the last planes
  //! are simply ignored.
  //! @param thePlanes [in/out] the list of planes to be added.
  //! The list then provides information on which planes were really added to clipping state.
  //! This list then can be used to fall back to previous state.
  //! @param theWS [in] the workspace to access the matrices.
  inline void AddView (Graphic3d_SetOfHClipPlane& thePlanes, const Handle(OpenGl_Workspace)& theWS)
  {
    Add (thePlanes, EquationCoords_View, theWS);
  }

  //! Add planes to the context clipping at the view system of coordinates.
  //! If the number of the passed planes exceeds capabilities of OpenGl, the last planes
  //! are simply ignored.
  //! @param thePlanes [in/out] the list of planes to be added.
  //! The list then provides information on which planes were really added to clipping state.
  //! This list then can be used to fall back to previous state.
  inline void AddView (Graphic3d_SetOfHClipPlane& thePlanes)
  {
    Add (thePlanes, EquationCoords_View);
  }

  //! Add planes to the context clipping at the world system of coordinates.
  //! If the number of the passed planes exceeds capabilities of OpenGl, the last planes
  //! are simply ignored.
  //! @param thePlanes [in/out] the list of planes to be added.
  //! The list then provides information on which planes were really added to clipping state.
  //! This list then can be used to fall back to previous state.
  //! @param theWS [in] the workspace to access the matrices.
  inline void AddWorld (Graphic3d_SetOfHClipPlane& thePlanes, const Handle(OpenGl_Workspace)& theWS)
  {
    Add (thePlanes, EquationCoords_World, theWS);
  }

  //! Add planes to the context clipping at the world system of coordinates.
  //! If the number of the passed planes exceeds capabilities of OpenGl, the last planes
  //! are simply ignored.
  //! @param thePlanes [in/out] the list of planes to be added.
  //! The list then provides information on which planes were really added to clipping state.
  //! This list then can be used to fall back to previous state.
  inline void AddWorld (Graphic3d_SetOfHClipPlane& thePlanes)
  {
    Add (thePlanes, EquationCoords_World);
  }

  //! Remove all of the planes from context state.
  inline void RemoveAll()
  {
    Remove (Planes());
  }

private:

  struct PlaneProps
  {
    // declare default constructor
    // to allow compilation of template collections
    PlaneProps() {}
    PlaneProps (const EquationCoords theCoords,
                const Standard_Integer theID,
                const Standard_Boolean theIsEnabled)
    {
      CoordSpace = theCoords;
      ContextID  = theID;
      IsEnabled  = theIsEnabled;
    }

    EquationCoords   CoordSpace;
    Standard_Integer ContextID;
    Standard_Boolean IsEnabled;
  };

private:

  typedef NCollection_DataMap<Handle(Graphic3d_ClipPlane), PlaneProps> OpenGl_MapOfPlaneStates;
  typedef NCollection_Handle<Aspect_GenId> OpenGl_EmptyPlaneIds;

  Graphic3d_SetOfHClipPlane myPlanes;          //!< defined clipping planes.
  OpenGl_MapOfPlaneStates   myPlaneStates;     //!< map of clip planes bound for the props.
  OpenGl_EmptyPlaneIds      myEmptyPlaneIds;   //!< generator of empty ids.

private:

  //! Copying allowed only within Handles
  OpenGl_ClippingState            (const OpenGl_ClippingState& );
  OpenGl_ClippingState& operator= (const OpenGl_ClippingState& );
};

#endif
