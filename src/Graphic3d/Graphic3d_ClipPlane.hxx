// Created on: 2013-07-12
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

#ifndef _Graphic3d_ClipPlane_HeaderFile
#define _Graphic3d_ClipPlane_HeaderFile

#include <Aspect_HatchStyle.hxx>
#include <gp_Pln.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_CappingFlags.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <NCollection_Vec4.hxx>
#include <Standard_Macro.hxx>
#include <Standard_TypeDef.hxx>
#include <Standard_Transient.hxx>

//! Container for properties describing graphic driver clipping planes.
//! It is up to application to create instances of this class and specify its
//! properties. The instances are passed into graphic driver or other facilities
//! that implement clipping features (e.g. selection).
//! Depending on usage context the class can be used to specify:
//! - Global clipping applied over the whole scene.
//! - Object-level clipping applied for each particular object.
//! The plane equation is specified in "world" coordinate system.
//! Please note that the set of planes can define convex clipping volume.
//! Be aware that number of clip planes supported by OpenGl is implementation
//! dependent: at least 6 planes are available. Thus, take into account
//! number of clipping planes passed for rendering: the object planes plus
//! the view defined ones.
class Graphic3d_ClipPlane : public Standard_Transient
{
public:

  typedef NCollection_Vec4<Standard_Real> Equation;

  //! Default constructor.
  //! Initializes clip plane container with the following properties:
  //! - Equation (0.0, 0.0, 1.0, 0)
  //! - IsOn (True),
  //! - IsCapping (False),
  //! - Material (Graphic3d_NOM_DEFAULT),
  //! - Texture (NULL),
  //! - HatchStyle (Aspect_HS_HORIZONTAL),
  //! - IsHatchOn (False)
  Standard_EXPORT Graphic3d_ClipPlane();

  //! Copy constructor.
  //! @param theOther [in] the copied plane.
  Standard_EXPORT Graphic3d_ClipPlane(const Graphic3d_ClipPlane& theOther);

  //! Construct clip plane for the passed equation.
  //! By default the plane is on, capping is turned off.
  //! @param theEquation [in] the plane equation.
  Standard_EXPORT Graphic3d_ClipPlane (const Equation& theEquation);

  //! Construct clip plane from the passed geometrical definition.
  //! By default the plane is on, capping is turned off.
  //! @param thePlane [in] the plane.
  Standard_EXPORT Graphic3d_ClipPlane (const gp_Pln& thePlane);

  //! Set plane equation by its geometrical definition.
  //! The equation is specified in "world" coordinate system.
  //! @param thePlane [in] the plane.
  Standard_EXPORT void SetEquation (const gp_Pln& thePlane);

  //! Set 4-component equation vector for clipping plane.
  //! The equation is specified in "world" coordinate system.
  //! @param theEquation [in] the XYZW (or "ABCD") equation vector.
  Standard_EXPORT void SetEquation (const Equation& theEquation);

  //! Get 4-component equation vector for clipping plane.
  //! @return clipping plane equation vector.
  const Equation& GetEquation() const
  {
    return myEquation;
  }

  //! Check that the clipping plane is turned on.
  //! @return boolean flag indicating whether the plane is in on or off state.
  Standard_Boolean IsOn() const
  {
    return myIsOn;
  }

  //! Change state of the clipping plane.
  //! @param theIsOn [in] the flag specifying whether the graphic driver
  //! clipping by this plane should be turned on or off.
  Standard_EXPORT void SetOn(const Standard_Boolean theIsOn);

  //! Change state of capping surface rendering.
  //! @param theIsOn [in] the flag specifying whether the graphic driver should
  //! perform rendering of capping surface produced by this plane. The graphic
  //! driver produces this surface for convex graphics by means of stencil-test
  //! and multi-pass rendering.
  Standard_EXPORT void SetCapping(const Standard_Boolean theIsOn);

  //! Check state of capping surface rendering.
  //! @return true (turned on) or false depending on the state.
  Standard_Boolean IsCapping() const
  {
    return myIsCapping;
  }

  //! Get geometrical definition.
  //! @return geometrical definition of clipping plane
  const gp_Pln& ToPlane() const { return myPlane; }

  //! Clone plane. Virtual method to simplify copying procedure if plane
  //! class is redefined at application level to add specific fields to it
  //! e.g. id, name, etc.
  //! @return new instance of clipping plane with same properties and attributes.
  Standard_EXPORT virtual Handle(Graphic3d_ClipPlane) Clone() const;

public: // @name user-defined graphical attributes

  //! Set material for rendering capping surface.
  //! @param theMat [in] the material.
  Standard_EXPORT void SetCappingMaterial (const Graphic3d_MaterialAspect& theMat);

  //! @return capping material.
  const Graphic3d_MaterialAspect& CappingMaterial() const { return myAspect->FrontMaterial(); }

  //! Set texture to be applied on capping surface.
  //! @param theTexture [in] the texture.
  Standard_EXPORT void SetCappingTexture (const Handle(Graphic3d_TextureMap)& theTexture);

  //! @return capping texture map.
  Handle(Graphic3d_TextureMap) CappingTexture() const { return !myAspect->TextureSet().IsNull() && !myAspect->TextureSet()->IsEmpty()
                                                              ? myAspect->TextureSet()->First()
                                                              : Handle(Graphic3d_TextureMap)(); }

  //! Set hatch style (stipple) and turn hatching on.
  //! @param theStyle [in] the hatch style.
  Standard_EXPORT void SetCappingHatch (const Aspect_HatchStyle theStyle);

  //! @return hatching style.
  Aspect_HatchStyle CappingHatch() const { return (Aspect_HatchStyle)myAspect->HatchStyle()->HatchType(); }

  //! Set custom hatch style (stipple) and turn hatching on.
  //! @param theStyle [in] the hatch pattern.
  Standard_EXPORT void SetCappingCustomHatch (const Handle(Graphic3d_HatchStyle)& theStyle);

  //! @return hatching style.
  const Handle(Graphic3d_HatchStyle)& CappingCustomHatch() const { return myAspect->HatchStyle(); }

  //! Turn on hatching.
  Standard_EXPORT void SetCappingHatchOn();

  //! Turn off hatching.
  Standard_EXPORT void SetCappingHatchOff();

  //! @return True if hatching mask is turned on.
  Standard_Boolean IsHatchOn() const { return myAspect->InteriorStyle() == Aspect_IS_HATCH; }

  //! This ID is used for managing associated resources in graphical driver.
  //! The clip plane can be assigned within a range of IO which can be
  //! displayed in separate OpenGl contexts. For each of the context an associated
  //! OpenGl resource for graphical aspects should be created and kept.
  //! The resources are stored in graphical driver for each of individual groups
  //! of shared context under the clip plane identifier.
  //! @return clip plane resource identifier string.
  const TCollection_AsciiString& GetId() const
  {
    return myId;
  }

public:

  //! Return capping aspect.
  //! @return capping surface rendering aspect.
  const Handle(Graphic3d_AspectFillArea3d)& CappingAspect() const { return myAspect; }

  //! Assign capping aspect.
  Standard_EXPORT void SetCappingAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspect);

  //! Flag indicating whether material for capping plane should be taken from object.
  //! Default value: FALSE (use dedicated capping plane material).
  bool ToUseObjectMaterial() const { return (myFlags & Graphic3d_CappingFlags_ObjectMaterial) != 0; }

  //! Set flag for controlling the source of capping plane material.
  void SetUseObjectMaterial (bool theToUse) { setCappingFlag (theToUse, Graphic3d_CappingFlags_ObjectMaterial); }

  //! Flag indicating whether texture for capping plane should be taken from object.
  //! Default value: FALSE.
  bool ToUseObjectTexture() const { return (myFlags & Graphic3d_CappingFlags_ObjectTexture) != 0; }

  //! Set flag for controlling the source of capping plane texture.
  void SetUseObjectTexture (bool theToUse) { setCappingFlag (theToUse, Graphic3d_CappingFlags_ObjectTexture); }

  //! Flag indicating whether shader program for capping plane should be taken from object.
  //! Default value: FALSE.
  bool ToUseObjectShader() const { return (myFlags & Graphic3d_CappingFlags_ObjectShader) != 0; }

  //! Set flag for controlling the source of capping plane shader program.
  void SetUseObjectShader(bool theToUse) { setCappingFlag (theToUse, Graphic3d_CappingFlags_ObjectShader); }

  //! Return true if some fill area aspect properties should be taken from object.
  bool ToUseObjectProperties() const { return myFlags != Graphic3d_CappingFlags_None; }

public: // @name modification counters

  //! @return modification counter for equation.
  unsigned int MCountEquation() const
  {
    return myEquationMod;
  }

  //! @return modification counter for aspect.
  unsigned int MCountAspect() const
  {
    return myAspectMod;
  }

private:

  //! Generate unique object id for OpenGL graphic resource manager.
  void makeId();

  //! Set capping flag.
  Standard_EXPORT void setCappingFlag (bool theToUse, int theFlag);

private:

  Handle(Graphic3d_AspectFillArea3d) myAspect;    //!< fill area aspect
  TCollection_AsciiString myId;                   //!< resource id
  gp_Pln                  myPlane;                //!< plane definition
  Equation                myEquation;             //!< plane equation vector
  unsigned int            myFlags;                //!< capping flags
  unsigned int            myEquationMod;          //!< modification counter for equation
  unsigned int            myAspectMod;            //!< modification counter of aspect
  Standard_Boolean        myIsOn;                 //!< state of the clipping plane
  Standard_Boolean        myIsCapping;            //!< state of graphic driver capping

public:

  DEFINE_STANDARD_RTTIEXT(Graphic3d_ClipPlane,Standard_Transient)
};

DEFINE_STANDARD_HANDLE (Graphic3d_ClipPlane, Standard_Transient)

#endif
