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

class MeshVS_MeshOwner;
DEFINE_STANDARD_HANDLE(MeshVS_MeshOwner, SelectMgr_EntityOwner)

//! The custom mesh owner used for advanced mesh selection. This class provides methods to store
//! information: 1) IDs of hilighted mesh nodes and elements 2) IDs of mesh nodes and elements
//! selected on the mesh
class MeshVS_MeshOwner : public SelectMgr_EntityOwner
{

public:
  Standard_EXPORT MeshVS_MeshOwner(const SelectMgr_SelectableObject* theSelObj,
                                   const Handle(MeshVS_DataSource)&  theDS,
                                   const Standard_Integer            thePriority = 0);

  Standard_EXPORT const Handle(MeshVS_DataSource)& GetDataSource() const;

  //! Returns ids of selected mesh nodes
  Standard_EXPORT const Handle(TColStd_HPackedMapOfInteger)& GetSelectedNodes() const;

  //! Returns ids of selected mesh elements
  Standard_EXPORT const Handle(TColStd_HPackedMapOfInteger)& GetSelectedElements() const;

  //! Saves ids of selected mesh entities
  Standard_EXPORT virtual void AddSelectedEntities(
    const Handle(TColStd_HPackedMapOfInteger)& Nodes,
    const Handle(TColStd_HPackedMapOfInteger)& Elems);

  //! Clears ids of selected mesh entities
  Standard_EXPORT virtual void ClearSelectedEntities();

  //! Returns ids of hilighted mesh nodes
  Standard_EXPORT const Handle(TColStd_HPackedMapOfInteger)& GetDetectedNodes() const;

  //! Returns ids of hilighted mesh elements
  Standard_EXPORT const Handle(TColStd_HPackedMapOfInteger)& GetDetectedElements() const;

  //! Saves ids of hilighted mesh entities
  Standard_EXPORT void SetDetectedEntities(const Handle(TColStd_HPackedMapOfInteger)& Nodes,
                                           const Handle(TColStd_HPackedMapOfInteger)& Elems);

  Standard_EXPORT virtual void HilightWithColor(const Handle(PrsMgr_PresentationManager)& thePM,
                                                const Handle(Prs3d_Drawer)&               theColor,
                                                const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void Unhilight(const Handle(PrsMgr_PresentationManager)& PM,
                                         const Standard_Integer Mode = 0) Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean IsForcedHilight() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(MeshVS_MeshOwner, SelectMgr_EntityOwner)

protected:
  Handle(TColStd_HPackedMapOfInteger) mySelectedNodes;
  Handle(TColStd_HPackedMapOfInteger) mySelectedElems;

private:
  Handle(MeshVS_DataSource)           myDataSource;
  Handle(TColStd_HPackedMapOfInteger) myDetectedNodes;
  Handle(TColStd_HPackedMapOfInteger) myDetectedElems;
  Standard_Integer                    myLastID;
};

#endif // _MeshVS_MeshOwner_HeaderFile
