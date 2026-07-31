// Created on: 2003-12-11
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

#ifndef _MeshVS_DeformedDataSource_HeaderFile
#define _MeshVS_DeformedDataSource_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <gp_Vec.hxx>
#include <NCollection_DataMap.hxx>
#include <MeshVS_DataSource.hxx>
#include <NCollection_Array1.hxx>
#include <MeshVS_EntityType.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HArray1.hxx>
class gp_Vec;

//! The class provides default class which helps to represent node displacements by deformed mesh
//! This class has an internal handle to canonical non-deformed mesh data source and
//! map of displacement vectors. The displacement can be magnified to useful size.
//! All methods is implemented with calling the corresponding methods of non-deformed data source.
class MeshVS_DeformedDataSource : public MeshVS_DataSource
{

public:
  //! Constructor
  //! theNonDeformDS is canonical non-deformed data source, by which we are able to calculate
  //! deformed mesh geometry
  //! theMagnify is coefficient of displacement magnify
  Standard_EXPORT MeshVS_DeformedDataSource(const occ::handle<MeshVS_DataSource>& theNonDeformDS,
                                            const double                          theMagnify);

  Standard_EXPORT bool GetGeom(const int                   ID,
                               const bool                  IsElement,
                               NCollection_Array1<double>& Coords,
                               int&                        NbNodes,
                               MeshVS_EntityType&          Type) const override;

  Standard_EXPORT bool GetGeomType(const int          ID,
                                   const bool         IsElement,
                                   MeshVS_EntityType& Type) const override;

  Standard_EXPORT bool Get3DGeom(
    const int                                                    ID,
    int&                                                         NbNodes,
    occ::handle<NCollection_HArray1<NCollection_Sequence<int>>>& Data) const override;

  Standard_EXPORT void* GetAddr(const int ID, const bool IsElement) const override;

  Standard_EXPORT bool GetNodesByElement(const int                ID,
                                         NCollection_Array1<int>& NodeIDs,
                                         int&                     NbNodes) const override;

  Standard_EXPORT const TColStd_PackedMapOfInteger& GetAllNodes() const override;

  Standard_EXPORT const TColStd_PackedMapOfInteger& GetAllElements() const override;

  //! This method returns map of nodal displacement vectors
  Standard_EXPORT const NCollection_DataMap<int, gp_Vec>& GetVectors() const;

  //! This method sets map of nodal displacement vectors (Map).
  Standard_EXPORT void SetVectors(const NCollection_DataMap<int, gp_Vec>& Map);

  //! This method returns vector ( Vect ) assigned to node number ID.
  Standard_EXPORT bool GetVector(const int ID, gp_Vec& Vect) const;

  //! This method sets vector ( Vect ) assigned to node number ID.
  Standard_EXPORT void SetVector(const int ID, const gp_Vec& Vect);

  Standard_EXPORT void SetNonDeformedDataSource(const occ::handle<MeshVS_DataSource>& theDS);

  //! With this methods you can read and change internal canonical data source
  Standard_EXPORT occ::handle<MeshVS_DataSource> GetNonDeformedDataSource() const;

  Standard_EXPORT void SetMagnify(const double theMagnify);

  //! With this methods you can read and change magnify coefficient of nodal displacements
  Standard_EXPORT double GetMagnify() const;

  DEFINE_STANDARD_RTTIEXT(MeshVS_DeformedDataSource, MeshVS_DataSource)

private:
  occ::handle<MeshVS_DataSource>   myNonDeformedDataSource;
  TColStd_PackedMapOfInteger       myEmptyMap;
  NCollection_DataMap<int, gp_Vec> myVectors;
  double                           myMagnify;
};

#endif // _MeshVS_DeformedDataSource_HeaderFile
