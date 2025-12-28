// Created on: 2004-06-10
// Created by: Alexander SOLOVYOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _XSDRAWSTL_DataSource_HeaderFile
#define _XSDRAWSTL_DataSource_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <MeshVS_DataSource.hxx>
#include <NCollection_Array1.hxx>
#include <MeshVS_EntityType.hxx>
#include <Poly_Triangulation.hxx>

//! The sample DataSource for working with STLMesh_Mesh
class XSDRAWSTL_DataSource : public MeshVS_DataSource
{

public:
  //! Constructor
  Standard_EXPORT XSDRAWSTL_DataSource(const occ::handle<Poly_Triangulation>& aMesh);

  //! Returns geometry information about node (if IsElement is False) or element (IsElement is True)
  //! by coordinates. For element this method must return all its nodes coordinates in the strict
  //! order: X, Y, Z and with nodes order is the same as in wire bounding the face or link. NbNodes
  //! is number of nodes of element. It is recommended to return 1 for node. Type is an element
  //! type.
  Standard_EXPORT bool GetGeom(const int                   ID,
                               const bool                  IsElement,
                               NCollection_Array1<double>& Coords,
                               int&                        NbNodes,
                               MeshVS_EntityType&          Type) const override;

  //! This method is similar to GetGeom, but returns only element or node type. This method is
  //! provided for a fine performance.
  Standard_EXPORT bool GetGeomType(const int          ID,
                                   const bool         IsElement,
                                   MeshVS_EntityType& Type) const override;

  //! This method returns by number an address of any entity which represents element or node data
  //! structure.
  Standard_EXPORT void* GetAddr(const int ID, const bool IsElement) const override;

  //! This method returns information about what node this element consist of.
  Standard_EXPORT bool GetNodesByElement(const int                ID,
                                                 NCollection_Array1<int>& NodeIDs,
                                                 int&                     NbNodes) const override;

  //! This method returns map of all nodes the object consist of.
  Standard_EXPORT const TColStd_PackedMapOfInteger& GetAllNodes() const override;

  //! This method returns map of all elements the object consist of.
  Standard_EXPORT const TColStd_PackedMapOfInteger& GetAllElements() const override;

  //! This method calculates normal of face, which is using for correct reflection presentation.
  //! There is default method, for advance reflection this method can be redefined.
  Standard_EXPORT bool GetNormal(const int Id,
                                         const int Max,
                                         double&   nx,
                                         double&   ny,
                                         double&   nz) const override;

  DEFINE_STANDARD_RTTIEXT(XSDRAWSTL_DataSource, MeshVS_DataSource)

private:
  occ::handle<Poly_Triangulation>          myMesh;
  TColStd_PackedMapOfInteger               myNodes;
  TColStd_PackedMapOfInteger               myElements;
  occ::handle<NCollection_HArray2<int>>    myElemNodes;
  occ::handle<NCollection_HArray2<double>> myNodeCoords;
  occ::handle<NCollection_HArray2<double>> myElemNormals;
};

#endif // _XSDRAWSTL_DataSource_HeaderFile
