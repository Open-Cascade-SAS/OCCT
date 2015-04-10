// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Graphic3d_CStructure_HeaderFile
#define _Graphic3d_CStructure_HeaderFile

#include <Graphic3d_BndBox4f.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_SequenceOfGroup.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Graphic3d_TypeOfComposition.hxx>
#include <Graphic3d_ViewAffinity.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Vec3.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Standard_Transient.hxx>
#include <Handle_Graphic3d_GraphicDriver.hxx>

//! Forward declaration.
class Handle(Graphic3d_StructureManager);
class Handle(Graphic3d_CStructure);

//! Low-level graphic structure interface
class Graphic3d_CStructure : public Standard_Transient
{

public:

  //! @return graphic driver created this structure
  const Handle(Graphic3d_GraphicDriver)& GraphicDriver() const
  {
    return myGraphicDriver;
  }

  //! @return graphic groups
  const Graphic3d_SequenceOfGroup& Groups() const
  {
    return myGroups;
  }

  //! @return associated clip planes
  const Graphic3d_SequenceOfHClipPlane& ClipPlanes() const
  {
    return myClipPlanes;
  }

  //! Pass clip planes to the associated graphic driver structure
  void SetClipPlanes (const Graphic3d_SequenceOfHClipPlane& thePlanes) { myClipPlanes = thePlanes; }

  //! @return bounding box of this presentation
  const Graphic3d_BndBox4f& BoundingBox() const
  {
    return myBndBox;
  }

  //! @return bounding box of this presentation
  //! without transformation matrix applied
  Graphic3d_BndBox4f& ChangeBoundingBox()
  {
    return myBndBox;
  }

  //! Return structure visibility flag
  bool IsVisible() const { return visible != 0; }

  //! Set z layer ID to display the structure in specified layer
  void SetZLayer (const Graphic3d_ZLayerId theLayerIndex) { myZLayer = theLayerIndex; }

  //! Get z layer ID
  Graphic3d_ZLayerId ZLayer() const { return myZLayer; }

public:

  //! Update structure visibility state
  virtual void OnVisibilityChanged() = 0;

  //! Clear graphic data
  virtual void Clear() = 0;

  //! Connect other structure to this one
  virtual void Connect    (Graphic3d_CStructure& theStructure) = 0;

  //! Disconnect other structure to this one
  virtual void Disconnect (Graphic3d_CStructure& theStructure) = 0;

  //! Synchronize structure aspects
  virtual void UpdateAspects() = 0;

  //! Synchronize structure transformation
  virtual void UpdateTransformation() = 0;

  //! Highlight entire structure with color
  virtual void HighlightWithColor  (const Graphic3d_Vec3&  theColor,
                                    const Standard_Boolean theToCreate) = 0;

  //! Highlight structure using boundary box
  virtual void HighlightWithBndBox (const Handle(Graphic3d_Structure)& theStruct,
                                    const Standard_Boolean             theToCreate) = 0;

  //! Create shadow link to this structure
  virtual Handle(Graphic3d_CStructure) ShadowLink (const Handle(Graphic3d_StructureManager)& theManager) const = 0;

  //! Create new group within this structure
  virtual Handle(Graphic3d_Group) NewGroup (const Handle(Graphic3d_Structure)& theStruct) = 0;

  //! Remove group from this structure
  virtual void RemoveGroup (const Handle(Graphic3d_Group)& theGroup) = 0;

public:

  int                      Id;
  Graphic3d_ZLayerId       myZLayer;
  int                      Priority;
  int                      PreviousPriority;

  CALL_DEF_CONTEXTLINE     ContextLine;
  CALL_DEF_CONTEXTFILLAREA ContextFillArea;
  CALL_DEF_CONTEXTMARKER   ContextMarker;
  CALL_DEF_CONTEXTTEXT     ContextText;

  CALL_DEF_COLOR HighlightColor;

  float Transformation[4][4];
  Graphic3d_TypeOfComposition Composition;

  int   ContainsFacet;

  Handle(Graphic3d_ViewAffinity) ViewAffinity; //!< view affinity mask

  unsigned IsInfinite     : 1;
  unsigned stick          : 1; //!< displaying state - should be set when structure has been added to scene graph (but can be in hidden state)
  unsigned highlight      : 1;
  unsigned visible        : 1; //!< visibility flag - can be used to suppress structure while leaving it in the scene graph
  unsigned HLRValidation  : 1;
  unsigned IsForHighlight : 1;
  unsigned IsMutable      : 1;
  unsigned Is2dText       : 1;

  CALL_DEF_TRANSFORM_PERSISTENCE TransformPersistence;

protected:

  //! Create empty structure.
  Standard_EXPORT Graphic3d_CStructure (const Handle(Graphic3d_StructureManager)& theManager);

protected:

  Handle(Graphic3d_GraphicDriver) myGraphicDriver;
  Graphic3d_SequenceOfGroup       myGroups;
  Graphic3d_BndBox4f              myBndBox;
  Graphic3d_SequenceOfHClipPlane  myClipPlanes;

public:

  DEFINE_STANDARD_RTTI(Graphic3d_CStructure) // Type definition

};

DEFINE_STANDARD_HANDLE (Graphic3d_CStructure, Standard_Transient)

#endif // _Graphic3d_CStructure_HeaderFile
