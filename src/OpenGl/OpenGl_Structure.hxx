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

#include <OpenGl_Group.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_Workspace.hxx>

#include <NCollection_List.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

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
  Standard_EXPORT virtual void Clear();

  //! Connect other structure to this one
  Standard_EXPORT virtual void Connect    (Graphic3d_CStructure& theStructure);

  //! Disconnect other structure to this one
  Standard_EXPORT virtual void Disconnect (Graphic3d_CStructure& theStructure);

  //! Synchronize structure aspects
  Standard_EXPORT virtual void UpdateAspects();

  //! Synchronize structure transformation
  Standard_EXPORT virtual void UpdateTransformation();

  //! Highlight entire structure with color
  Standard_EXPORT virtual void HighlightWithColor (const Graphic3d_Vec3&  theColor,
                                                   const Standard_Boolean theToCreate);

  //! Highlight structure using boundary box
  Standard_EXPORT virtual void HighlightWithBndBox (const Handle(Graphic3d_Structure)& theStruct,
                                                    const Standard_Boolean             theToCreate);

  //! Create shadow link to this structure
  Standard_EXPORT virtual Handle(Graphic3d_CStructure) ShadowLink (const Handle(Graphic3d_StructureManager)& theManager) const;

  //! Create new group within this structure
  Standard_EXPORT virtual Handle(Graphic3d_Group) NewGroup (const Handle(Graphic3d_Structure)& theStruct);

  //! Remove group from this structure
  Standard_EXPORT virtual void RemoveGroup (const Handle(Graphic3d_Group)& theGroup);

public:

  //! @return graphic groups
  virtual const Graphic3d_SequenceOfGroup& DrawGroups() const
  {
    return myGroups;
  }

  //! Access graphic driver
  OpenGl_GraphicDriver* GlDriver() const
  {
    return (OpenGl_GraphicDriver* )myGraphicDriver.operator->();
  }

  void SetTransformPersistence (const CALL_DEF_TRANSFORM_PERSISTENCE &ATransPers);

  void SetAspectLine   (const CALL_DEF_CONTEXTLINE &theAspect);
  void SetAspectFace   (const CALL_DEF_CONTEXTFILLAREA& theAspect);
  void SetAspectMarker (const CALL_DEF_CONTEXTMARKER& theAspect);
  void SetAspectText   (const CALL_DEF_CONTEXTTEXT &theAspect);

  void clearHighlightBox (const Handle(OpenGl_Context)& theGlCtx);

  void setHighlightColor (const Handle(OpenGl_Context)& theGlCtx,
                          const Graphic3d_Vec3&         theColor);

  void clearHighlightColor (const Handle(OpenGl_Context)& theGlCtx);

  Standard_EXPORT void Clear (const Handle(OpenGl_Context)& theGlCtx);

  //! Renders groups of structure without applying any attributes (i.e. transform, material etc).
  virtual void RenderGeometry  (const Handle(OpenGl_Workspace)& theWorkspace) const;

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
  const Standard_Boolean IsAlwaysRendered() const
  {
    return IsInfinite
        || IsForHighlight
        || IsMutable
        || Is2dText
        || TransformPersistence.Flag != 0;
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

  //! Returns OpenGL face aspect.
  const OpenGl_AspectFace* AspectFace() const { return myAspectFace; }

  //! Returns OpenGL transformation matrix.
  const OpenGl_Matrix* Transformation() const { return myTransformation; }
  
  //! Returns OpenGL persistent translation.
  const TEL_TRANSFORM_PERSISTENCE* PersistentTranslation() const { return myTransPers; }

  //! Returns structure modification state (for ray-tracing).
  Standard_Size ModificationState() const { return myModificationState; }

  //! Resets structure modification state (for ray-tracing)
  void ResetModificationState() const { myModificationState = 0; }

  //! Is the structure ray-tracable (contains ray-tracable elements)?
  Standard_Boolean IsRaytracable() const;

protected:

  Standard_EXPORT virtual ~OpenGl_Structure();

  //! Updates ray-tracable status for structure and its parents.
  void UpdateStateIfRaytracable (const Standard_Boolean toCheck = Standard_True) const;

protected:

  OpenGl_Matrix*             myTransformation;
  TEL_TRANSFORM_PERSISTENCE* myTransPers;
  OpenGl_AspectLine*         myAspectLine;
  OpenGl_AspectFace*         myAspectFace;
  OpenGl_AspectMarker*       myAspectMarker;
  OpenGl_AspectText*         myAspectText;

  Handle(OpenGl_Group)       myHighlightBox;
  TEL_COLOUR*                myHighlightColor;

  OpenGl_Structure*          myInstancedStructure;

  mutable Standard_Boolean   myIsRaytracable;
  mutable Standard_Size      myModificationState;

  mutable Standard_Boolean   myIsCulled; //!< A status specifying is structure needs to be rendered after BVH tree traverse.

  Standard_Boolean           myIsMirrored; //!< Used to tell OpenGl to interpret polygons in clockwise order.

public:

  DEFINE_STANDARD_RTTI(OpenGl_Structure) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_Structure, Graphic3d_CStructure)

#endif // OpenGl_Structure_Header
