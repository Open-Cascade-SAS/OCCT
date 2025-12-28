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

#ifndef OpenGl_Structure_HeaderFile
#define OpenGl_Structure_HeaderFile

#include <Graphic3d_CStructure.hxx>

#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_Workspace.hxx>

#include <NCollection_List.hxx>

//! Implementation of low-level graphic structure.
class OpenGl_Structure : public Graphic3d_CStructure
{
  friend class OpenGl_Group;
  DEFINE_STANDARD_RTTIEXT(OpenGl_Structure, Graphic3d_CStructure)
public:
  //! Auxiliary wrapper to iterate OpenGl_Structure sequence.
  typedef SubclassStructIterator<OpenGl_Structure> StructIterator;

  //! Auxiliary wrapper to iterate OpenGl_Group sequence.
  typedef SubclassGroupIterator<OpenGl_Group> GroupIterator;

public:
  //! Create empty structure
  Standard_EXPORT OpenGl_Structure(const occ::handle<Graphic3d_StructureManager>& theManager);

  //! Setup structure graphic state
  Standard_EXPORT virtual void OnVisibilityChanged() override;

  //! Clear graphic data
  Standard_EXPORT virtual void Clear() override;

  //! Connect other structure to this one
  Standard_EXPORT virtual void Connect(Graphic3d_CStructure& theStructure) override;

  //! Disconnect other structure to this one
  Standard_EXPORT virtual void Disconnect(Graphic3d_CStructure& theStructure) override;

  //! Synchronize structure transformation
  Standard_EXPORT virtual void SetTransformation(
    const occ::handle<TopLoc_Datum3D>& theTrsf) override;

  //! Set transformation persistence.
  Standard_EXPORT virtual void SetTransformPersistence(
    const occ::handle<Graphic3d_TransformPers>& theTrsfPers) override;

  //! Set z layer ID to display the structure in specified layer
  Standard_EXPORT virtual void SetZLayer(const Graphic3d_ZLayerId theLayerIndex) override;

  //! Highlights structure according to the given style and updates corresponding class fields
  //! (highlight status and style)
  Standard_EXPORT virtual void GraphicHighlight(
    const occ::handle<Graphic3d_PresentationAttributes>& theStyle) override;

  //! Unighlights structure and updates corresponding class fields (highlight status and style)
  Standard_EXPORT virtual void GraphicUnhighlight() override;

  //! Create shadow link to this structure
  Standard_EXPORT virtual occ::handle<Graphic3d_CStructure> ShadowLink(
    const occ::handle<Graphic3d_StructureManager>& theManager) const override;

  //! Create new group within this structure
  Standard_EXPORT virtual occ::handle<Graphic3d_Group> NewGroup(
    const occ::handle<Graphic3d_Structure>& theStruct) override;

  //! Remove group from this structure
  Standard_EXPORT virtual void RemoveGroup(const occ::handle<Graphic3d_Group>& theGroup) override;

public:
  //! Access graphic driver
  OpenGl_GraphicDriver* GlDriver() const
  {
    return (OpenGl_GraphicDriver*)myGraphicDriver.operator->();
  }

  Standard_EXPORT void Clear(const occ::handle<OpenGl_Context>& theGlCtx);

  //! Renders the structure.
  Standard_EXPORT virtual void Render(const occ::handle<OpenGl_Workspace>& theWorkspace) const;

  //! Releases structure resources.
  Standard_EXPORT virtual void Release(const occ::handle<OpenGl_Context>& theGlCtx);

  //! This method releases GL resources without actual elements destruction.
  //! As result structure could be correctly destroyed layer without GL context
  //! (after last window was closed for example).
  //!
  //! Notice however that reusage of this structure after calling this method is incorrect
  //! and will lead to broken visualization due to loosed data.
  Standard_EXPORT void ReleaseGlResources(const occ::handle<OpenGl_Context>& theGlCtx);

  //! Returns instanced OpenGL structure.
  const OpenGl_Structure* InstancedStructure() const { return myInstancedStructure; }

  //! Returns structure modification state (for ray-tracing).
  size_t ModificationState() const { return myModificationState; }

  //! Resets structure modification state (for ray-tracing).
  void ResetModificationState() const { myModificationState = 0; }

  //! Is the structure ray-tracable (contains ray-tracable elements)?
  bool IsRaytracable() const;

  //! Update render transformation matrix.
  Standard_EXPORT virtual void updateLayerTransformation() override;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

protected:
  Standard_EXPORT virtual ~OpenGl_Structure();

  //! Updates ray-tracable status for structure and its parents.
  void UpdateStateIfRaytracable(const bool toCheck = true) const;

  //! Renders groups of structure without applying any attributes (i.e. transform, material etc).
  //! @param theWorkspace current workspace
  //! @param theHasClosed flag will be set to TRUE if structure contains at least one group of
  //! closed primitives
  Standard_EXPORT void renderGeometry(const occ::handle<OpenGl_Workspace>& theWorkspace,
                                      bool&                                theHasClosed) const;

  //! Render the bounding box.
  Standard_EXPORT void renderBoundingBox(const occ::handle<OpenGl_Workspace>& theWorkspace) const;

  //! Apply transformation into context.
  //! @param theWorkspace current workspace
  //! @param theTrsf transformation
  //! @param toEnable flag to switch ON/OFF transformation
  Standard_EXPORT void applyTransformation(const occ::handle<OpenGl_Context>& theContext,
                                           const gp_Trsf&                     theTrsf,
                                           const bool                         toEnable) const;

  //! Apply transform persistence into context.
  //! It disables shadows on non-3d objects when toEnable is true and restores otherwise.
  //! @param[in]  theCtx current context
  //! @param[in]  theTrsfPers transform persistence
  //! @param[in]  theIsLocal specifies if transform persistence is defined locally or to entire
  //! presentation
  //! @param[out] theOldCastShadows state of the previous cast shadows state
  Standard_EXPORT void applyPersistence(
    const occ::handle<OpenGl_Context>&          theCtx,
    const occ::handle<Graphic3d_TransformPers>& theTrsfPersistence,
    const bool                                  theIsLocal,
    bool&                                       theOldCastShadows) const;

  //! Restore context from transform persistence changes.
  //! @param[in] theCtx current context
  //! @param[in] theTrsfPers transform persistence
  //! @param[in] theIsLocal specifies if transform persistence is defined locally or to entire
  //! presentation
  //! @param[in] theOldCastShadows state of the previous cast shadows state
  Standard_EXPORT void revertPersistence(
    const occ::handle<OpenGl_Context>&          theCtx,
    const occ::handle<Graphic3d_TransformPers>& theTrsfPersistence,
    const bool                                  theIsLocal,
    const bool                                  theOldCastShadows) const;

protected:
  OpenGl_Structure* myInstancedStructure;
  // clang-format off
  NCollection_Mat4<float>             myRenderTrsf; //!< transformation, actually used for rendering (includes Local Origin shift)

  mutable bool   myIsRaytracable;
  mutable size_t      myModificationState;

  bool           myIsMirrored; //!< Used to tell OpenGl to interpret polygons in clockwise order.
  // clang-format on
};

#endif // OpenGl_Structure_Header
