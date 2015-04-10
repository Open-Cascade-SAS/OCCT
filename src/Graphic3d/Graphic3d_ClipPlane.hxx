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

#include <Standard_Macro.hxx>
#include <Standard_TypeDef.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Vec4.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <Aspect_HatchStyle.hxx>

class gp_Pln;
class Graphic3d_AspectFillArea3d;
class Handle(Graphic3d_AspectFillArea3d);
class Handle(Graphic3d_ClipPlane);

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

  //! Get geometrical definition. The plane is built up
  //! from the equation clipping plane equation vector.
  //! @return geometrical definition of clipping plane.
  Standard_EXPORT gp_Pln ToPlane() const;

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
  const Graphic3d_MaterialAspect& CappingMaterial() const
  {
    return myMaterial;
  }

  //! Set texture to be applied on capping surface.
  //! @param theTexture [in] the texture.
  Standard_EXPORT void SetCappingTexture (const Handle(Graphic3d_TextureMap)& theTexture);

  //! @return capping texture map.
  const Handle(Graphic3d_TextureMap)& CappingTexture() const
  {
    return myTexture;
  }

  //! Set hatch style (stipple) and turn hatching on.
  //! @param theStyle [in] the hatch style.
  Standard_EXPORT void SetCappingHatch (const Aspect_HatchStyle theStyle);

  //! @return hatching style.
  Aspect_HatchStyle CappingHatch() const
  {
    return myHatch;
  }

  //! Turn on hatching.
  Standard_EXPORT void SetCappingHatchOn();

  //! Turn off hatching.
  Standard_EXPORT void SetCappingHatchOff();

  //! @return True if hatching mask is turned on.
  Standard_Boolean IsHatchOn() const
  {
    return myHatchOn;
  }

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

  //! Compute and return capping aspect from the graphical attributes.
  //! @return capping surface rendering aspect.
  Standard_EXPORT Handle(Graphic3d_AspectFillArea3d) CappingAspect() const;

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

  void MakeId();

private:

  Equation                     myEquation;    //!< Plane equation vector.
  Standard_Boolean             myIsOn;        //!< State of the clipping plane.
  Standard_Boolean             myIsCapping;   //!< State of graphic driver capping.
  Graphic3d_MaterialAspect     myMaterial;    //!< Capping surface material.
  Handle(Graphic3d_TextureMap) myTexture;     //!< Capping surface texture.
  Aspect_HatchStyle            myHatch;       //!< Capping surface hatch mask.
  Standard_Boolean             myHatchOn;     //!< Capping surface hatching flag.
  TCollection_AsciiString      myId;          //!< Resource id.
  unsigned int                 myEquationMod; //!< Modification counter for equation.
  unsigned int                 myAspectMod;   //!< Modification counter of aspect.

public:

  DEFINE_STANDARD_RTTI(Graphic3d_ClipPlane);
};

DEFINE_STANDARD_HANDLE (Graphic3d_ClipPlane, Standard_Transient)

#endif
