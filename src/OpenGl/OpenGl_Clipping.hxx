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

#include <Aspect_GenId.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Handle.hxx>
#include <Standard_TypeDef.hxx>
#include <OpenGl_Matrix.hxx>

class OpenGl_Workspace;

//! This class contains logics related to tracking and modification of clipping plane
//! state for particular OpenGl context. It contains information about enabled
//! clipping planes and provides method to change clippings in context. The methods
//! should be executed within OpenGl context associated with instance of this
//! class.
class OpenGl_Clipping
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
  Standard_EXPORT OpenGl_Clipping();

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
  inline const Graphic3d_SequenceOfHClipPlane& Planes() const
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

  //! @return true if there are enabled clipping planes (NOT capping)
  inline Standard_Boolean IsClippingOn() const
  {
    return myNbClipping > 0;
  }

  //! @return true if there are enabled capping planes
  inline Standard_Boolean IsCappingOn() const
  {
    return myNbCapping > 0;
  }

  //! @return true if there are enabled clipping or capping planes
  inline Standard_Boolean IsClippingOrCappingOn() const
  {
    return (myNbClipping + myNbCapping) > 0;
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
  Standard_EXPORT void Add (Graphic3d_SequenceOfHClipPlane& thePlanes,
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
  Standard_EXPORT void Add (Graphic3d_SequenceOfHClipPlane& thePlanes,
                            const EquationCoords& theCoordSpace);

  //! Remove the passed set of clipping planes from the context state.
  //! @param thePlanes [in] the planes to remove from list.
  Standard_EXPORT void Remove (const Graphic3d_SequenceOfHClipPlane& thePlanes);

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
  inline void AddView (Graphic3d_SequenceOfHClipPlane& thePlanes, const Handle(OpenGl_Workspace)& theWS)
  {
    Add (thePlanes, EquationCoords_View, theWS);
  }

  //! Add planes to the context clipping at the view system of coordinates.
  //! If the number of the passed planes exceeds capabilities of OpenGl, the last planes
  //! are simply ignored.
  //! @param thePlanes [in/out] the list of planes to be added.
  //! The list then provides information on which planes were really added to clipping state.
  //! This list then can be used to fall back to previous state.
  inline void AddView (Graphic3d_SequenceOfHClipPlane& thePlanes)
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
  inline void AddWorld (Graphic3d_SequenceOfHClipPlane& thePlanes, const Handle(OpenGl_Workspace)& theWS)
  {
    Add (thePlanes, EquationCoords_World, theWS);
  }

  //! Add planes to the context clipping at the world system of coordinates.
  //! If the number of the passed planes exceeds capabilities of OpenGl, the last planes
  //! are simply ignored.
  //! @param thePlanes [in/out] the list of planes to be added.
  //! The list then provides information on which planes were really added to clipping state.
  //! This list then can be used to fall back to previous state.
  inline void AddWorld (Graphic3d_SequenceOfHClipPlane& thePlanes)
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

  Graphic3d_SequenceOfHClipPlane myPlanes;        //!< defined clipping planes
  OpenGl_MapOfPlaneStates        myPlaneStates;   //!< map of clip planes bound for the props
  OpenGl_EmptyPlaneIds           myEmptyPlaneIds; //!< generator of empty ids
  Standard_Boolean               myNbClipping;    //!< number of enabled clipping-only planes (NOT capping)
  Standard_Boolean               myNbCapping;     //!< number of enabled capping  planes

private:

  //! Copying allowed only within Handles
  OpenGl_Clipping            (const OpenGl_Clipping& );
  OpenGl_Clipping& operator= (const OpenGl_Clipping& );
};

#endif
