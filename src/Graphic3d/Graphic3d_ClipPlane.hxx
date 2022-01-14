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

#include <gp_Pln.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_BndBox3d.hxx>
#include <Graphic3d_CappingFlags.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <NCollection_Vec4.hxx>
#include <Standard_Macro.hxx>
#include <Standard_TypeDef.hxx>
#include <Standard_Transient.hxx>

//! Clipping state.
enum Graphic3d_ClipState
{
  Graphic3d_ClipState_Out, //!< fully outside (clipped) - should be discarded
  Graphic3d_ClipState_In,  //!< fully inside  (NOT clipped) - should NOT be discarded
  Graphic3d_ClipState_On,  //!< on (not clipped / partially clipped) - should NOT be discarded
};

//! Container for properties describing either a Clipping halfspace (single Clipping Plane),
//! or a chain of Clipping Planes defining logical AND (conjunction) operation.
//! The plane equation is specified in "world" coordinate system.
class Graphic3d_ClipPlane : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_ClipPlane,Standard_Transient)
public:

  //! Type defining XYZW (ABCD) plane equation - left for compatibility with old code using Graphic3d_ClipPlane::Equation type.
  typedef Graphic3d_Vec4d Equation;

public:

  //! Default constructor.
  //! Initializes clip plane container with the following properties:
  //! - Equation (0.0, 0.0, 1.0, 0)
  //! - IsOn (True),
  //! - IsCapping (False),
  //! - Material (Graphic3d_NameOfMaterial_DEFAULT),
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
  Standard_EXPORT Graphic3d_ClipPlane (const Graphic3d_Vec4d& theEquation);

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
  Standard_EXPORT void SetEquation (const Graphic3d_Vec4d& theEquation);

  //! Get 4-component equation vector for clipping plane.
  //! @return clipping plane equation vector.
  const Graphic3d_Vec4d& GetEquation() const { return myEquation; }

  //! Get 4-component equation vector for clipping plane.
  //! @return clipping plane equation vector.
  const Graphic3d_Vec4d& ReversedEquation() const { return myEquationRev; }

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

public:

  //! Return TRUE if this item defines a conjunction (logical AND) between a set of Planes.
  //! Graphic3d_ClipPlane item defines either a Clipping halfspace (single Clipping Plane)
  //! or a Clipping volume defined by a logical AND (conjunction) operation between a set of Planes defined as a Chain
  //! (so that the volume cuts a space only in case if check fails for ALL Planes in the Chain).
  //!
  //! Note that Graphic3d_ClipPlane item cannot:
  //! - Define a Chain with logical OR (disjunction) operation;
  //!   this should be done through Graphic3d_SequenceOfHClipPlane.
  //! - Define nested Chains.
  //! - Disable Chain items; only entire Chain can be disabled (by disabled a head of Chain).
  //!
  //! The head of a Chain defines all visual properties of the Chain,
  //! so that Graphic3d_ClipPlane of next items in a Chain merely defines only geometrical definition of the plane.
  Standard_Boolean IsChain() const { return !myNextInChain.IsNull(); }

  //! Return the previous plane in a Chain of Planes defining logical AND operation,
  //! or NULL if there is no Chain or it is a first element in Chain.
  //! When clipping is defined by a Chain of Planes,
  //! it cuts a space only in case if check fails for all Planes in Chain.
  Handle(Graphic3d_ClipPlane) ChainPreviousPlane() const { return myPrevInChain; }

  //! Return the next plane in a Chain of Planes defining logical AND operation,
  //! or NULL if there is no chain or it is a last element in chain.

  const Handle(Graphic3d_ClipPlane)& ChainNextPlane() const { return myNextInChain; }

  //! Return the number of chains in forward direction (including this item, so it is always >= 1).
  //! For a head of Chain - returns the length of entire Chain.
  Standard_Integer NbChainNextPlanes() const { return myChainLenFwd; }

  //! Set the next plane in a Chain of Planes.
  //! This operation also updates relationship between chains (Previous/Next items),
  //! so that the previously set Next plane is cut off.
  Standard_EXPORT void SetChainNextPlane (const Handle(Graphic3d_ClipPlane)& thePlane);

public: // @name user-defined graphical attributes

  //! Return color for rendering capping surface.
  Quantity_Color CappingColor() const { return myAspect->FrontMaterial().MaterialType() == Graphic3d_MATERIAL_ASPECT ? myAspect->FrontMaterial().Color() : myAspect->InteriorColor(); }

  //! Set color for rendering capping surface.
  Standard_EXPORT void SetCappingColor (const Quantity_Color& theColor);

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

public:

  //! Check if the given point is outside / inside / on section.
  Graphic3d_ClipState ProbePoint (const Graphic3d_Vec4d& thePoint) const
  {
    Graphic3d_ClipState aState = Graphic3d_ClipState_Out;
    for (const Graphic3d_ClipPlane* aPlaneIter = this; aPlaneIter != NULL; aPlaneIter = aPlaneIter->myNextInChain.get())
    {
      Graphic3d_ClipState aPlnState = aPlaneIter->ProbePointHalfspace (thePoint);
      if (aPlnState == Graphic3d_ClipState_In)
      {
        return Graphic3d_ClipState_In;
      }
      else if (aPlnState != Graphic3d_ClipState_Out)
      {
        aState = Graphic3d_ClipState_On;
      }
    }
    return aState;
  }

  //! Check if the given bounding box is fully outside / fully inside.
  Graphic3d_ClipState ProbeBox (const Graphic3d_BndBox3d& theBox) const
  {
    Graphic3d_ClipState aState = Graphic3d_ClipState_Out;
    for (const Graphic3d_ClipPlane* aPlaneIter = this; aPlaneIter != NULL; aPlaneIter = aPlaneIter->myNextInChain.get())
    {
      if (aPlaneIter->IsBoxFullInHalfspace (theBox))
      {
        // within union operation, if box is entirely inside at least one half-space, others can be ignored
        return Graphic3d_ClipState_In;
      }
      else if (!aPlaneIter->IsBoxFullOutHalfspace (theBox))
      {
        // if at least one full out test fail, clipping state is inconclusive (partially clipped)
        aState = Graphic3d_ClipState_On;
      }
    }
    return aState;
  }

  //! Check if the given bounding box is In and touch the clipping planes
  Standard_Boolean ProbeBoxTouch (const Graphic3d_BndBox3d& theBox) const
  {
    for (const Graphic3d_ClipPlane* aPlaneIter = this; aPlaneIter != NULL; aPlaneIter = aPlaneIter->myNextInChain.get())
    {
      if (aPlaneIter->IsBoxFullInHalfspace (theBox))
      {
        // within union operation, if box is entirely inside at least one half-space, others can be ignored
        return Standard_False;
      }
      else if (!aPlaneIter->IsBoxFullOutHalfspace (theBox))
      {
        // the box is not fully out, and not fully in, check is it on (but not intersect)
        if (ProbeBoxMaxPointHalfspace (theBox) != Graphic3d_ClipState_Out)
        {
          return Standard_True;
        }
      }
    }
    return Standard_False;
  }

public:

  //! Check if the given point is outside of the half-space (e.g. should be discarded by clipping plane).
  Graphic3d_ClipState ProbePointHalfspace (const Graphic3d_Vec4d& thePoint) const
  {
    const Standard_Real aVal = myEquation.Dot (thePoint);
    return aVal < 0.0
         ? Graphic3d_ClipState_Out
         : (aVal == 0.0
          ? Graphic3d_ClipState_On
          : Graphic3d_ClipState_In);
  }

  //! Check if the given bounding box is fully outside / fully inside the half-space.
  Graphic3d_ClipState ProbeBoxHalfspace (const Graphic3d_BndBox3d& theBox) const
  {
    if (IsBoxFullOutHalfspace (theBox))
    {
      return Graphic3d_ClipState_Out;
    }
    return IsBoxFullInHalfspace (theBox)
         ? Graphic3d_ClipState_In
         : Graphic3d_ClipState_On;
  }

  //! Check if the given point is outside of the half-space (e.g. should be discarded by clipping plane).
  bool IsPointOutHalfspace (const Graphic3d_Vec4d& thePoint) const { return ProbePointHalfspace (thePoint) == Graphic3d_ClipState_Out; }

  //! Check if the given bounding box is fully outside of the half-space (e.g. should be discarded by clipping plane).
  bool IsBoxFullOutHalfspace (const Graphic3d_BndBox3d& theBox) const
  {
    const Graphic3d_Vec4d aMaxPnt (myEquation.x() > 0.0 ? theBox.CornerMax().x() : theBox.CornerMin().x(),
                                   myEquation.y() > 0.0 ? theBox.CornerMax().y() : theBox.CornerMin().y(),
                                   myEquation.z() > 0.0 ? theBox.CornerMax().z() : theBox.CornerMin().z(),
                                   1.0);
    return IsPointOutHalfspace (aMaxPnt);
  }

  //! Check if the given bounding box is fully outside of the half-space (e.g. should be discarded by clipping plane).
  Graphic3d_ClipState ProbeBoxMaxPointHalfspace (const Graphic3d_BndBox3d& theBox) const
  {
    const Graphic3d_Vec4d aMaxPnt (myEquation.x() > 0.0 ? theBox.CornerMax().x() : theBox.CornerMin().x(),
                                   myEquation.y() > 0.0 ? theBox.CornerMax().y() : theBox.CornerMin().y(),
                                   myEquation.z() > 0.0 ? theBox.CornerMax().z() : theBox.CornerMin().z(),
                                   1.0);
    return ProbePointHalfspace (aMaxPnt);
  }

  //! Check if the given bounding box is fully inside (or touches from inside) the half-space (e.g. NOT discarded by clipping plane).
  bool IsBoxFullInHalfspace (const Graphic3d_BndBox3d& theBox) const
  {
    const Graphic3d_Vec4d aMinPnt (myEquation.x() > 0.0 ? theBox.CornerMin().x() : theBox.CornerMax().x(),
                                   myEquation.y() > 0.0 ? theBox.CornerMin().y() : theBox.CornerMax().y(),
                                   myEquation.z() > 0.0 ? theBox.CornerMin().z() : theBox.CornerMax().z(),
                                   1.0);
    return !IsPointOutHalfspace (aMinPnt);
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

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

  //! Update chain length in backward direction.
  void updateChainLen();

  //! Update inversed plane definition from main plane.
  void updateInversedPlane()
  {
    gp_Pln aPlane = myPlane;
    aPlane.SetAxis (aPlane.Axis().Reversed());
    aPlane.Coefficients (myEquationRev[0], myEquationRev[1], myEquationRev[2], myEquationRev[3]);
  }

private:

  Handle(Graphic3d_AspectFillArea3d) myAspect;    //!< fill area aspect
  Handle(Graphic3d_ClipPlane)   myNextInChain;    //!< next     plane in a chain of planes defining logical AND operation
  Graphic3d_ClipPlane*          myPrevInChain;    //!< previous plane in a chain of planes defining logical AND operation
  TCollection_AsciiString myId;                   //!< resource id
  gp_Pln                  myPlane;                //!< plane definition
  Graphic3d_Vec4d         myEquation;             //!< plane equation vector
  Graphic3d_Vec4d         myEquationRev;          //!< reversed plane equation
  Standard_Integer        myChainLenFwd;          //!< chain length in forward direction (including this item)
  unsigned int            myFlags;                //!< capping flags
  unsigned int            myEquationMod;          //!< modification counter for equation
  unsigned int            myAspectMod;            //!< modification counter of aspect
  Standard_Boolean        myIsOn;                 //!< state of the clipping plane
  Standard_Boolean        myIsCapping;            //!< state of graphic driver capping

};

DEFINE_STANDARD_HANDLE (Graphic3d_ClipPlane, Standard_Transient)

#endif
