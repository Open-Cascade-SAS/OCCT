// Created on: 2007-01-24
// Created by: Sergey  Kochetkov
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _MeshVS_MeshOwner_HeaderFile
#define _MeshVS_MeshOwner_HeaderFile

#include <SelectMgr_EntityOwner.hxx>
#include <PrsMgr_PresentationManager.hxx>

class MeshVS_DataSource;
class TColStd_HPackedMapOfInteger;
class PrsMgr_PresentationManager;

//! The custom mesh owner used for advanced mesh selection. This class provides methods to store
//! information: 1) IDs of hilighted mesh nodes and elements 2) IDs of mesh nodes and elements
//! selected on the mesh
class MeshVS_MeshOwner : public SelectMgr_EntityOwner
{

public:
  Standard_EXPORT MeshVS_MeshOwner(const SelectMgr_SelectableObject*     theSelObj,
                                   const occ::handle<MeshVS_DataSource>& theDS,
                                   const int                             thePriority = 0);

  Standard_EXPORT const occ::handle<MeshVS_DataSource>& GetDataSource() const;

  //! Returns ids of selected mesh nodes
  Standard_EXPORT const occ::handle<TColStd_HPackedMapOfInteger>& GetSelectedNodes() const;

  //! Returns ids of selected mesh elements
  Standard_EXPORT const occ::handle<TColStd_HPackedMapOfInteger>& GetSelectedElements() const;

  //! Saves ids of selected mesh entities
  Standard_EXPORT virtual void AddSelectedEntities(
    const occ::handle<TColStd_HPackedMapOfInteger>& Nodes,
    const occ::handle<TColStd_HPackedMapOfInteger>& Elems);

  //! Clears ids of selected mesh entities
  Standard_EXPORT virtual void ClearSelectedEntities();

  //! Returns ids of hilighted mesh nodes
  Standard_EXPORT const occ::handle<TColStd_HPackedMapOfInteger>& GetDetectedNodes() const;

  //! Returns ids of hilighted mesh elements
  Standard_EXPORT const occ::handle<TColStd_HPackedMapOfInteger>& GetDetectedElements() const;

  //! Saves ids of hilighted mesh entities
  Standard_EXPORT void SetDetectedEntities(const occ::handle<TColStd_HPackedMapOfInteger>& Nodes,
                                           const occ::handle<TColStd_HPackedMapOfInteger>& Elems);

  Standard_EXPORT virtual void HilightWithColor(
    const occ::handle<PrsMgr_PresentationManager>& thePM,
    const occ::handle<Prs3d_Drawer>&               theColor,
    const int                                      theMode) override;

  Standard_EXPORT virtual void Unhilight(const occ::handle<PrsMgr_PresentationManager>& PM,
                                         const int Mode = 0) override;

  Standard_EXPORT virtual bool IsForcedHilight() const override;

  DEFINE_STANDARD_RTTIEXT(MeshVS_MeshOwner, SelectMgr_EntityOwner)

protected:
  occ::handle<TColStd_HPackedMapOfInteger> mySelectedNodes;
  occ::handle<TColStd_HPackedMapOfInteger> mySelectedElems;

private:
  occ::handle<MeshVS_DataSource>           myDataSource;
  occ::handle<TColStd_HPackedMapOfInteger> myDetectedNodes;
  occ::handle<TColStd_HPackedMapOfInteger> myDetectedElems;
  int                                      myLastID;
};

#endif // _MeshVS_MeshOwner_HeaderFile
