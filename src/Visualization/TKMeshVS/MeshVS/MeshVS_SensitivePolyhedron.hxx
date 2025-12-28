// Created on: 2005-01-21
// Created by: Alexander SOLOVYOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _MeshVS_SensitivePolyhedron_HeaderFile
#define _MeshVS_SensitivePolyhedron_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_List.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Select3D_BndBox3d.hxx>

//! This class is used to detect selection of a polyhedron. The main
//! principle of detection algorithm is to search for overlap with
//! each polyhedron's face separately, treating them as planar convex
//! polygons.
class MeshVS_SensitivePolyhedron : public Select3D_SensitiveEntity
{
public:
  Standard_EXPORT MeshVS_SensitivePolyhedron(
    const occ::handle<SelectMgr_EntityOwner>&                          theOwner,
    const NCollection_Array1<gp_Pnt>&                                  theNodes,
    const occ::handle<NCollection_HArray1<NCollection_Sequence<int>>>& theTopo);

  Standard_EXPORT occ::handle<Select3D_SensitiveEntity> GetConnected() override;

  Standard_EXPORT bool Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                       SelectBasics_PickResult&             thePickResult) override;

  //! Returns the amount of nodes of polyhedron
  Standard_EXPORT int NbSubElements() const override;

  Standard_EXPORT Select3D_BndBox3d BoundingBox() override;

  Standard_EXPORT gp_Pnt CenterOfGeometry() const override;

  DEFINE_STANDARD_RTTIEXT(MeshVS_SensitivePolyhedron, Select3D_SensitiveEntity)

private:
  NCollection_List<occ::handle<NCollection_HArray1<gp_Pnt>>>  myTopology;
  gp_XYZ                                                      myCenter;
  Select3D_BndBox3d                                           myBndBox;
  occ::handle<NCollection_HArray1<gp_Pnt>>                    myNodes;
  occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> myTopo;
};

#endif // _MeshVS_SensitivePolyhedron_HeaderFile
