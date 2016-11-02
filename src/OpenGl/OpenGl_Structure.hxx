// Created on: 2011-08-01
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef OpenGl_Structure_Header
#define OpenGl_Structure_Header

#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>

#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_Workspace.hxx>

#include <NCollection_List.hxx>

class OpenGl_Structure;
class OpenGl_GraphicDriver;

typedef NCollection_List<const OpenGl_Structure* > OpenGl_ListOfStructure;

//! Implementation of low-level graphic structure.
class OpenGl_Structure : public Graphic3d_CStructure
{
  friend class OpenGl_Group;

public:

  //! Auxiliary wrapper to iterate OpenGl_Group sequence.
  class GroupIterator
  {

  public:
    GroupIterator (const Graphic3d_SequenceOfGroup& theGroups) : myIter (theGroups) {}
    Standard_Boolean More() const     { return myIter.More(); }
    void Next()                       { myIter.Next(); }
    const OpenGl_Group* Value() const { return (const OpenGl_Group* )(myIter.Value().operator->()); }
    OpenGl_Group*       ChangeValue() { return (OpenGl_Group* )(myIter.ChangeValue().operator->()); }

  private:
    Graphic3d_SequenceOfGroup::Iterator myIter;

  };

public:

  //! Create empty structure
  Standard_EXPORT OpenGl_Structure (const Handle(Graphic3d_StructureManager)& theManager);

  //! Setup structure graphic state
  Standard_EXPORT virtual void OnVisibilityChanged() Standard_OVERRIDE;

  //! Clear graphic data
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Connect other structure to this one
  Standard_EXPORT virtual void Connect    (Graphic3d_CStructure& theStructure) Standard_OVERRIDE;

  //! Disconnect other structure to this one
  Standard_EXPORT virtual void Disconnect (Graphic3d_CStructure& theStructure) Standard_OVERRIDE;

  //! Synchronize structure transformation
  Standard_EXPORT virtual void SetTransformation (const Handle(Geom_Transformation)& theTrsf) Standard_OVERRIDE;

  //! Set transformation persistence.
  Standard_EXPORT virtual void SetTransformPersistence (const Handle(Graphic3d_TransformPers)& theTrsfPers) Standard_OVERRIDE;

  //! Set z layer ID to display the structure in specified layer
  Standard_EXPORT virtual void SetZLayer(const Graphic3d_ZLayerId theLayerIndex) Standard_OVERRIDE;

  //! Highlights structure according to the given style and updates corresponding class fields
  //! (highlight status and style)
  Standard_EXPORT virtual void GraphicHighlight (const Handle(Graphic3d_PresentationAttributes)& theStyle,
                                                 const Handle(Graphic3d_Structure)&  theStruct) Standard_OVERRIDE;

  //! Unighlights structure and updates corresponding class fields (highlight status and style)
  Standard_EXPORT virtual void GraphicUnhighlight() Standard_OVERRIDE;

  //! Create shadow link to this structure
  Standard_EXPORT virtual Handle(Graphic3d_CStructure) ShadowLink (const Handle(Graphic3d_StructureManager)& theManager) const Standard_OVERRIDE;

  //! Create new group within this structure
  Standard_EXPORT virtual Handle(Graphic3d_Group) NewGroup (const Handle(Graphic3d_Structure)& theStruct) Standard_OVERRIDE;

  //! Remove group from this structure
  Standard_EXPORT virtual void RemoveGroup (const Handle(Graphic3d_Group)& theGroup) Standard_OVERRIDE;

public:

  //! Access graphic driver
  OpenGl_GraphicDriver* GlDriver() const
  {
    return (OpenGl_GraphicDriver* )myGraphicDriver.operator->();
  }

  Standard_EXPORT void Clear (const Handle(OpenGl_Context)& theGlCtx);

  //! Renders the structure.
  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;

  //! Releases structure resources.
  virtual void Release (const Handle(OpenGl_Context)& theGlCtx);

  //! Marks structure as not overlapping view volume (as it is by default).
  void ResetCullingStatus() const
  {
    if (!IsAlwaysRendered())
    {
      myIsCulled = Standard_True;
    }
  }

  //! Marks structure as overlapping the current view volume one.
  //! The method is called during traverse of BVH tree.
  void MarkAsNotCulled() const { myIsCulled = Standard_False; }

  //! Returns Standard_False if the structure hits the current view volume, otherwise
  //! returns Standard_True. The default value for all structures before each traverse
  //! of BVH tree is Standard_True.
  Standard_Boolean IsCulled() const { return myIsCulled; }

  //! Checks if the structure should be included into BVH tree or not.
  Standard_Boolean IsAlwaysRendered() const
  {
    return IsInfinite
        || IsForHighlight
        || IsMutable
        || Is2dText
        || (!myTrsfPers.IsNull() && myTrsfPers->IsTrihedronOr2d());
  }

  //! This method releases GL resources without actual elements destruction.
  //! As result structure could be correctly destroyed layer without GL context
  //! (after last window was closed for example).
  //!
  //! Notice however that reusage of this structure after calling this method is incorrect
  //! and will lead to broken visualization due to loosed data.
  Standard_EXPORT void ReleaseGlResources (const Handle(OpenGl_Context)& theGlCtx);

  //! Returns instanced OpenGL structure.
  const OpenGl_Structure* InstancedStructure() const { return myInstancedStructure; }

  //! Returns structure modification state (for ray-tracing).
  Standard_Size ModificationState() const { return myModificationState; }

  //! Resets structure modification state (for ray-tracing).
  void ResetModificationState() const { myModificationState = 0; }

  //! Is the structure ray-tracable (contains ray-tracable elements)?
  Standard_Boolean IsRaytracable() const;

  //! Update render transformation matrix.
  Standard_EXPORT void updateLayerTransformation();

protected:

  Standard_EXPORT virtual ~OpenGl_Structure();

  //! Updates ray-tracable status for structure and its parents.
  void UpdateStateIfRaytracable (const Standard_Boolean toCheck = Standard_True) const;

  //! Renders groups of structure without applying any attributes (i.e. transform, material etc).
  //! @param theWorkspace current workspace
  //! @param theHasClosed flag will be set to TRUE if structure contains at least one group of closed primitives
  Standard_EXPORT void renderGeometry (const Handle(OpenGl_Workspace)& theWorkspace,
                                       bool&                           theHasClosed) const;

  //! Highlight structure using boundary box
  Standard_EXPORT void highlightWithBndBox (const Handle(Graphic3d_Structure)& theStruct);

  //! Invalidates highlight box and releases graphic resources it uses
  Standard_EXPORT void clearHighlightBox (const Handle(OpenGl_Context)& theGlCtx);

protected:

  Handle(OpenGl_Group)       myHighlightBox;

  OpenGl_Structure*          myInstancedStructure;
  Graphic3d_Mat4             myRenderTrsf; //!< transformation, actually used for rendering (includes Local Origin shift)

  mutable Standard_Boolean   myIsRaytracable;
  mutable Standard_Size      myModificationState;

  mutable Standard_Boolean   myIsCulled; //!< A status specifying is structure needs to be rendered after BVH tree traverse.

  Standard_Boolean           myIsMirrored; //!< Used to tell OpenGl to interpret polygons in clockwise order.

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_Structure,Graphic3d_CStructure) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_Structure, Graphic3d_CStructure)

#endif // OpenGl_Structure_Header
