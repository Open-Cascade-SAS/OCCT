// Created on: 2003-10-10
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _MeshVS_Mesh_HeaderFile
#define _MeshVS_Mesh_HeaderFile

#include <MeshVS_PrsBuilder.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <NCollection_DataMap.hxx>
#include <MeshVS_MeshSelectionMethod.hxx>
#include <AIS_InteractiveObject.hxx>

class MeshVS_PrsBuilder;
class TColStd_HPackedMapOfInteger;
class MeshVS_DataSource;
class MeshVS_Drawer;
class SelectMgr_EntityOwner;

//! the main class provides interface to create mesh presentation as a whole
class MeshVS_Mesh : public AIS_InteractiveObject
{

public:
  //! Constructor.
  //! theIsAllowOverlapped is true, if it is allowed to draw edges overlapped with beams
  //! Its value is stored in drawer
  Standard_EXPORT MeshVS_Mesh(const bool theIsAllowOverlapped = false);

  //! Returns true for supported display modes basing on a list of defined builders.
  Standard_EXPORT bool AcceptDisplayMode(const int theMode) const override;

  //! Computes presentation using builders added to sequence. Each builder computes
  //! own part of mesh presentation according to its type.
  Standard_EXPORT void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                               const occ::handle<Prs3d_Presentation>&         thePrs,
                               const int                                      theDispMode) override;

  //! Computes selection according to SelectMode
  Standard_EXPORT void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSel,
                                        const int theSelMode) override;

  //! Draw selected owners presentation
  Standard_EXPORT void HilightSelected(
    const occ::handle<PrsMgr_PresentationManager>&                  thePrsMgr,
    const NCollection_Sequence<occ::handle<SelectMgr_EntityOwner>>& theOwners) override;

  //! Draw hilighted owner presentation
  Standard_EXPORT void HilightOwnerWithColor(
    const occ::handle<PrsMgr_PresentationManager>& thePM,
    const occ::handle<Prs3d_Drawer>&               theColor,
    const occ::handle<SelectMgr_EntityOwner>&      theOwner) override;

  //! Clears internal selection presentation
  Standard_EXPORT void ClearSelected() override;

  //! How many builders there are in sequence
  Standard_EXPORT int GetBuildersCount() const;

  //! Returns builder by its index in sequence
  Standard_EXPORT occ::handle<MeshVS_PrsBuilder> GetBuilder(const int Index) const;

  //! Returns builder by its ID
  Standard_EXPORT occ::handle<MeshVS_PrsBuilder> GetBuilderById(const int Id) const;

  //! Returns the smallest positive ID, not occupied by any builder.
  //! This method using when builder is created with ID = -1
  Standard_EXPORT int GetFreeId() const;

  //! Adds builder to tale of sequence.
  //! PrsBuilder is builder to be added
  //! If TreatAsHilighter is true, MeshVS_Mesh will use this builder to create
  //! presentation of hilighted and selected owners.
  //! Only one builder can be hilighter, so that if you call this method with
  //! TreatAsHilighter = true some times, only last builder will be hilighter
  //! WARNING: As minimum one builder must be added as hilighter, otherwise selection cannot be
  //! computed
  Standard_EXPORT void AddBuilder(const occ::handle<MeshVS_PrsBuilder>& Builder,
                                  const bool                            TreatAsHilighter = false);

  //! Changes hilighter ( see above )
  Standard_EXPORT void SetHilighter(const occ::handle<MeshVS_PrsBuilder>& Builder);

  //! Sets builder with sequence index "Index" as hilighter
  Standard_EXPORT bool SetHilighter(const int Index);

  //! Sets builder with identificator "Id" as hilighter
  Standard_EXPORT bool SetHilighterById(const int Id);

  //! Returns hilighter
  Standard_EXPORT occ::handle<MeshVS_PrsBuilder> GetHilighter() const;

  //! Removes builder from sequence. If it is hilighter, hilighter will be NULL
  //! ( Don't remember to set it to other after!!! )
  Standard_EXPORT void RemoveBuilder(const int Index);

  //! Removes builder with identificator Id
  Standard_EXPORT void RemoveBuilderById(const int Id);

  //! Finds builder by its type the string represents
  Standard_DEPRECATED("This method will be removed right after 7.9 release. \
Use FindBuilder(const occ::handle<Standard_Type>&) instead \
or directly iterate under sequence of builders.")
  Standard_EXPORT occ::handle<MeshVS_PrsBuilder> FindBuilder(const char* const TypeString) const;

  //! Finds builder by its type the type represents
  Standard_EXPORT occ::handle<MeshVS_PrsBuilder> FindBuilder(
    const occ::handle<Standard_Type>& TypeString) const;

  //! Returns map of owners.
  Standard_EXPORT const NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>>& GetOwnerMaps(
    const bool IsElement);

  //! Returns default builders' data source
  Standard_EXPORT occ::handle<MeshVS_DataSource> GetDataSource() const;

  //! Sets default builders' data source
  Standard_EXPORT void SetDataSource(const occ::handle<MeshVS_DataSource>& aDataSource);

  //! Returns default builders' drawer
  Standard_EXPORT occ::handle<MeshVS_Drawer> GetDrawer() const;

  //! Sets default builders' drawer
  Standard_EXPORT void SetDrawer(const occ::handle<MeshVS_Drawer>& aDrawer);

  //! Returns True if specified element is hidden
  //! By default no elements are hidden
  Standard_EXPORT bool IsHiddenElem(const int ID) const;

  //! Returns True if specified node is hidden.
  //! By default all nodes are hidden
  Standard_EXPORT bool IsHiddenNode(const int ID) const;

  //! Returns True if specified element is not hidden
  Standard_EXPORT bool IsSelectableElem(const int ID) const;

  //! Returns True if specified node is specified as selectable.
  Standard_EXPORT bool IsSelectableNode(const int ID) const;

  //! Returns map of hidden nodes (may be null handle)
  Standard_EXPORT const occ::handle<TColStd_HPackedMapOfInteger>& GetHiddenNodes() const;

  //! Sets map of hidden nodes, which shall not be displayed individually.
  //! If nodes shared by some elements shall not be drawn,
  //! they should be included into that map
  Standard_EXPORT void SetHiddenNodes(const occ::handle<TColStd_HPackedMapOfInteger>& Ids);

  //! Returns map of hidden elements (may be null handle)
  Standard_EXPORT const occ::handle<TColStd_HPackedMapOfInteger>& GetHiddenElems() const;

  //! Sets map of hidden elements
  Standard_EXPORT void SetHiddenElems(const occ::handle<TColStd_HPackedMapOfInteger>& Ids);

  //! Returns map of selectable elements (may be null handle)
  Standard_EXPORT const occ::handle<TColStd_HPackedMapOfInteger>& GetSelectableNodes() const;

  //! Sets map of selectable nodes.
  Standard_EXPORT void SetSelectableNodes(const occ::handle<TColStd_HPackedMapOfInteger>& Ids);

  //! Automatically computes selectable nodes; the node is considered
  //! as being selectable if it is either not hidden, or is hidden
  //! but referred by at least one non-hidden element.
  //! Thus all nodes that are visible (either individually, or as ends or
  //! corners of elements) are selectable by default.
  Standard_EXPORT void UpdateSelectableNodes();

  //! Returns set mesh selection method (see MeshVS.cdl)
  Standard_EXPORT MeshVS_MeshSelectionMethod GetMeshSelMethod() const;

  //! Sets mesh selection method (see MeshVS.cdl)
  Standard_EXPORT void SetMeshSelMethod(const MeshVS_MeshSelectionMethod M);

  //! Returns True if the given owner represents a whole mesh.
  Standard_EXPORT virtual bool IsWholeMeshOwner(
    const occ::handle<SelectMgr_EntityOwner>& theOwner) const;

  friend class MeshVS_PrsBuilder;

  DEFINE_STANDARD_RTTIEXT(MeshVS_Mesh, AIS_InteractiveObject)

protected:
  //! Stores all vertices that belong to one of the faces to the given map
  //! @param[in] theAllElements  the map of all mesh elements
  //! @param[in] theNbMaxFaceNodes  the maximum amount of nodes per face, retrieved from drawer
  //! @param[out] theSharedNodes  the result map of all vertices that belong to one face at least
  Standard_EXPORT void scanFacesForSharedNodes(const TColStd_PackedMapOfInteger& theAllElements,
                                               const int                         theNbMaxFaceNodes,
                                               TColStd_PackedMapOfInteger& theSharedNodes) const;

protected:
  NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>> myNodeOwners;
  NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>> myElementOwners;
  NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>> my0DOwners;
  NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>> myLinkOwners;
  NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>> myFaceOwners;
  NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>> myVolumeOwners;
  NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>> myGroupOwners;
  NCollection_DataMap<int, occ::handle<SelectMgr_EntityOwner>> myMeshOwners;
  occ::handle<MeshVS_Drawer>                                   myCurrentDrawer;
  occ::handle<MeshVS_Drawer>                                   mySelectionDrawer;
  occ::handle<MeshVS_Drawer>                                   myHilightDrawer;
  occ::handle<SelectMgr_EntityOwner>                           myWholeMeshOwner;

private:
  NCollection_Sequence<occ::handle<MeshVS_PrsBuilder>> myBuilders;
  occ::handle<MeshVS_PrsBuilder>                       myHilighter;
  occ::handle<TColStd_HPackedMapOfInteger>             myHiddenElements;
  occ::handle<TColStd_HPackedMapOfInteger>             myHiddenNodes;
  occ::handle<TColStd_HPackedMapOfInteger>             mySelectableNodes;
  occ::handle<MeshVS_DataSource>                       myDataSource;
  MeshVS_MeshSelectionMethod                           mySelectionMethod;
};

#endif // _MeshVS_Mesh_HeaderFile
