// Created on: 1991-09-06
// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_Group_HeaderFile
#define _Graphic3d_Group_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_BndBox4f.hxx>
#include <Standard_Boolean.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
#include <Standard_CString.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_TextPath.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Graphic3d_TypeOfPrimitiveArray.hxx>
#include <Graphic3d_IndexBuffer.hxx>
#include <Graphic3d_Buffer.hxx>
#include <Graphic3d_BoundBuffer.hxx>
#include <Standard_Address.hxx>
#include <Graphic3d_GroupAspect.hxx>
#include <gp_Ax2.hxx>
#include <TCollection_ExtendedString.hxx>

class Graphic3d_Structure;
class Graphic3d_ArrayOfPrimitives;

//! This class allows the definition of groups
//! of primitives inside of graphic objects (presentations).
//! A group contains the primitives and attributes
//! for which the range is limited to this group.
//! The primitives of a group can be globally suppressed.
//!
//! There are two main group usage models:
//!
//! 1) Non-modifiable, or unbounded, group ('black box').
//! Developers can repeat a sequence of
//! SetPrimitivesAspect() with AddPrimitiveArray() methods arbitrary number of times
//! to define arbitrary number of primitive "blocks" each having individual apect values.
//! Any modification of such a group is forbidden, as aspects and primitives are mixed
//! in memory without any high-level logical structure, and any modification is very likely to result
//! in corruption of the group internal data.
//! It is necessary to recreate such a group as a whole when some attribute should be changed.
//! (for example, in terms of AIS it is necessary to re-Compute() the whole presentation each time).
//! 2) Bounded group. Developers should specify the necessary group aspects with help of
//! SetGroupPrimitivesAspect() and then add primitives to the group.
//! Such a group have simplified organization in memory (a single block of attributes
//! followed by a block of primitives) and therefore it can be modified, if it is necessary to
//! change parameters of some aspect that has already been set, using methods:
//! IsGroupPrimitivesAspectSet() to detect which aspect was set for primitives;
//! GroupPrimitivesAspect() to read current aspect values
//! and SetGroupPrimitivesAspect() to set new values.
//!
//! Developers are strongly recommended to take all the above into account when filling Graphic3d_Group
//! with aspects and primitives and choose the group usage model beforehand out of application needs.
//! Note that some Graphic3d_Group class virtual methods contain only base implementation
//! that is extended by the descendant class in OpenGl package.
class Graphic3d_Group : public Standard_Transient
{
  friend class Graphic3d_Structure;
  DEFINE_STANDARD_RTTIEXT(Graphic3d_Group,Standard_Transient)

public:

  //! Supress all primitives and attributes of <me>.
  //! To clear group without update in Graphic3d_StructureManager
  //! pass Standard_False as <theUpdateStructureMgr>. This
  //! used on context and viewer destruction, when the pointer
  //! to structure manager in Graphic3d_Structure could be
  //! already released (pointers are used here to avoid handle
  //! cross-reference);
  Standard_EXPORT virtual void Clear (const Standard_Boolean theUpdateStructureMgr = Standard_True);

  //! Supress the group <me> in the structure.
  Standard_EXPORT virtual ~Graphic3d_Group();

  //! Supress the group <me> in the structure.
  //! Warning: No more graphic operations in <me> after this call.
  //! Modifies the current modelling transform persistence (pan, zoom or rotate)
  //! Get the current modelling transform persistence (pan, zoom or rotate)
  Standard_EXPORT void Remove();

public:

  //! Return line aspect.
  virtual Handle(Graphic3d_AspectLine3d) LineAspect() const = 0;

  //! Assign line aspect to the group.
  virtual void SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& theAspect) = 0;

  //! Return fill area aspect.
  virtual Handle(Graphic3d_AspectFillArea3d) FillAreaAspect() const = 0;

  //! Modifies the context for all the face primitives of the group.
  virtual void SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspect) = 0;

  //! Return text aspect.
  virtual Handle(Graphic3d_AspectText3d) TextAspect() const = 0;

  //! Modifies the context for all the text primitives of the group.
  virtual void SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectText3d)& theAspect) = 0;

  //! Return marker aspect.
  virtual Handle(Graphic3d_AspectMarker3d) MarkerAspect() const = 0;

  //! Modifies the context for all the marker primitives of the group.
  virtual void SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectMarker3d)& theAspect) = 0;

  //! Modifies the current context of the group to give
  //! another aspect for all the line primitives created
  //! after this call in the group.
  virtual void SetPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& theAspect) = 0;

  //! Modifies the current context of the group to give
  //! another aspect for all the face primitives created
  //! after this call in the group.
  virtual void SetPrimitivesAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspect) = 0;

  //! Modifies the current context of the group to give
  //! another aspect for all the text primitives created
  //! after this call in the group.
  virtual void SetPrimitivesAspect (const Handle(Graphic3d_AspectText3d)& theAspect) = 0;

  //! Modifies the current context of the group to give
  //! another aspect for all the marker primitives created
  //! after this call in the group.
  virtual void SetPrimitivesAspect (const Handle(Graphic3d_AspectMarker3d)& theAspect) = 0;

  //! Returns TRUE if aspect is set for the group.
  Standard_EXPORT Standard_Boolean IsGroupPrimitivesAspectSet (const Graphic3d_GroupAspect theAspect) const;

  //! Returns the context of all the primitives of the group.
  Standard_EXPORT void GroupPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)&     theAspLine,
                                              const Handle(Graphic3d_AspectText3d)&     theAspText,
                                              const Handle(Graphic3d_AspectMarker3d)&   theAspMarker,
                                              const Handle(Graphic3d_AspectFillArea3d)& theAspFill) const;

  //! Returns the last inserted context in the group for each kind of primitives.
  void PrimitivesAspect (const Handle(Graphic3d_AspectLine3d)&     theAspLine,
                         const Handle(Graphic3d_AspectText3d)&     theAspText,
                         const Handle(Graphic3d_AspectMarker3d)&   theAspMarker,
                         const Handle(Graphic3d_AspectFillArea3d)& theAspFill) const
  {
    GroupPrimitivesAspect (theAspLine, theAspText, theAspMarker, theAspFill);
  }

public:

  //! Creates the string <AText> at position <APoint>.
  //! The 3D point of attachment is projected. The text is
  //! written in the plane of projection.
  //! The attributes are given with respect to the plane of
  //! projection.
  //! AHeight : Height of text.
  //! (Relative to the Normalized Projection
  //! Coordinates (NPC) Space).
  //! AAngle  : Orientation of the text
  //! (with respect to the horizontal).
  Standard_EXPORT virtual void Text (const Standard_CString AText, const Graphic3d_Vertex& APoint, const Standard_Real AHeight, const Standard_Real AAngle, const Graphic3d_TextPath ATp, const Graphic3d_HorizontalTextAlignment AHta, const Graphic3d_VerticalTextAlignment AVta, const Standard_Boolean EvalMinMax = Standard_True);

  //! Creates the string <AText> at position <APoint>.
  //! The 3D point of attachment is projected. The text is
  //! written in the plane of projection.
  //! The attributes are given with respect to the plane of
  //! projection.
  //! AHeight : Height of text.
  //! (Relative to the Normalized Projection
  //! Coordinates (NPC) Space).
  //! The other attributes have the following default values:
  //! AAngle  : PI / 2.
  //! ATp     : TP_RIGHT
  //! AHta    : HTA_LEFT
  //! AVta    : VTA_BOTTOM
  Standard_EXPORT void Text (const Standard_CString AText, const Graphic3d_Vertex& APoint, const Standard_Real AHeight, const Standard_Boolean EvalMinMax = Standard_True);

  //! Creates the string <AText> at position <APoint>.
  //! The 3D point of attachment is projected. The text is
  //! written in the plane of projection.
  //! The attributes are given with respect to the plane of
  //! projection.
  //! AHeight : Height of text.
  //! (Relative to the Normalized Projection
  //! Coordinates (NPC) Space).
  //! AAngle  : Orientation of the text
  //! (with respect to the horizontal).
  Standard_EXPORT void Text (const TCollection_ExtendedString& AText, const Graphic3d_Vertex& APoint, const Standard_Real AHeight, const Standard_Real AAngle, const Graphic3d_TextPath ATp, const Graphic3d_HorizontalTextAlignment AHta, const Graphic3d_VerticalTextAlignment AVta, const Standard_Boolean EvalMinMax = Standard_True);

  //! Creates the string <AText> at position <APoint>.
  //! The 3D point of attachment is projected. The text is
  //! written in the plane of projection.
  //! The attributes are given with respect to the plane of
  //! projection.
  //! AHeight : Height of text.
  //! (Relative to the Normalized Projection
  //! Coordinates (NPC) Space).
  //! The other attributes have the following default values:
  //! AAngle  : PI / 2.
  //! ATp     : TP_RIGHT
  //! AHta    : HTA_LEFT
  //! AVta    : VTA_BOTTOM
  Standard_EXPORT void Text (const TCollection_ExtendedString& AText, const Graphic3d_Vertex& APoint, const Standard_Real AHeight, const Standard_Boolean EvalMinMax = Standard_True);

  //! Creates the string <theText> at orientation <theOrientation> in 3D space.
  Standard_EXPORT virtual void Text (const Standard_CString                  theTextUtf,
                                     const gp_Ax2&                           theOrientation,
                                     const Standard_Real                     theHeight,
                                     const Standard_Real                     theAngle,
                                     const Graphic3d_TextPath                theTp,
                                     const Graphic3d_HorizontalTextAlignment theHTA,
                                     const Graphic3d_VerticalTextAlignment   theVTA,
                                     const Standard_Boolean                  theToEvalMinMax = Standard_True,
                                     const Standard_Boolean                  theHasOwnAnchor = Standard_True);

  //! Creates the string <theText> at orientation <theOrientation> in 3D space.
  Standard_EXPORT virtual void Text (const TCollection_ExtendedString&       theText,
                                     const gp_Ax2&                           theOrientation,
                                     const Standard_Real                     theHeight,
                                     const Standard_Real                     theAngle,
                                     const Graphic3d_TextPath                theTp,
                                     const Graphic3d_HorizontalTextAlignment theHTA,
                                     const Graphic3d_VerticalTextAlignment   theVTA,
                                     const Standard_Boolean                  theToEvalMinMax = Standard_True,
                                     const Standard_Boolean                  theHasOwnAnchor = Standard_True);


  //! Adds an array of primitives for display
  Standard_EXPORT virtual void AddPrimitiveArray (const Graphic3d_TypeOfPrimitiveArray theType, const Handle(Graphic3d_IndexBuffer)& theIndices, const Handle(Graphic3d_Buffer)& theAttribs, const Handle(Graphic3d_BoundBuffer)& theBounds, const Standard_Boolean theToEvalMinMax = Standard_True);

  //! Adds an array of primitives for display
  Standard_EXPORT void AddPrimitiveArray (const Handle(Graphic3d_ArrayOfPrimitives)& thePrim, const Standard_Boolean theToEvalMinMax = Standard_True);

  //! Creates a primitive array with single marker using AddPrimitiveArray().
  Standard_EXPORT void Marker (const Graphic3d_Vertex& thePoint, const Standard_Boolean theToEvalMinMax = Standard_True);

public:

  //! sets the stencil test to theIsEnabled state;
  Standard_EXPORT virtual void SetStencilTestOptions (const Standard_Boolean theIsEnabled) = 0;

  //! sets the flipping to theIsEnabled state.
  Standard_EXPORT virtual void SetFlippingOptions (const Standard_Boolean theIsEnabled, const gp_Ax2& theRefPlane) = 0;

  //! Returns true if the group contains Polygons, Triangles or Quadrangles.
  bool ContainsFacet() const { return myContainsFacet; }

  //! Returns Standard_True if the group <me> is deleted.
  //! <me> is deleted after the call Remove (me) or the
  //! associated structure is deleted.
  Standard_EXPORT Standard_Boolean IsDeleted() const;

  //! Returns Standard_True if the group <me> is empty.
  Standard_EXPORT Standard_Boolean IsEmpty() const;

  //! Returns the coordinates of the boundary box of the group.
  Standard_EXPORT void MinMaxValues (Standard_Real& theXMin, Standard_Real& theYMin, Standard_Real& theZMin,
                                     Standard_Real& theXMax, Standard_Real& theYMax, Standard_Real& theZMax) const;

  //! Sets the coordinates of the boundary box of the group.
  Standard_EXPORT void SetMinMaxValues (const Standard_Real theXMin, const Standard_Real theYMin, const Standard_Real theZMin,
                                        const Standard_Real theXMax, const Standard_Real theYMax, const Standard_Real theZMax);

  //! Returns boundary box of the group <me> without transformation applied,
  const Graphic3d_BndBox4f& BoundingBox() const { return myBounds; }

  //! Returns non-const boundary box of the group <me> without transformation applied,
  Graphic3d_BndBox4f& ChangeBoundingBox() { return myBounds; }

  //! Returns the structure containing the group <me>.
  Standard_EXPORT Handle(Graphic3d_Structure) Structure() const;

  //! Changes property shown that primitive arrays within this group form closed volume (do no contain open shells).
  void SetClosed (const bool theIsClosed) { myIsClosed = theIsClosed; }

  //! Return true if primitive arrays within this graphic group form closed volume (do no contain open shells).
  bool IsClosed() const { return myIsClosed; }

protected:

  //! Creates a group in the structure <AStructure>.
  Standard_EXPORT Graphic3d_Group(const Handle(Graphic3d_Structure)& theStructure);

  //! Calls the Update method of the StructureManager which
  //! contains the associated Structure of the Group <me>.
  Standard_EXPORT void Update() const;

protected:

  Graphic3d_Structure* myStructure;     //!< pointer to the parent structure
  Graphic3d_BndBox4f   myBounds;        //!< bounding box
  bool                 myIsClosed;      //!< flag indicating closed volume
  bool                 myContainsFacet; //!< flag indicating that this group contains face primitives

};

DEFINE_STANDARD_HANDLE(Graphic3d_Group, Standard_Transient)

#endif // _Graphic3d_Group_HeaderFile
