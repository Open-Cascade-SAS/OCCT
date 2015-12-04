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

#include <TColStd_PackedMapOfInteger.hxx>
#include <MeshVS_DataMapOfIntegerVector.hxx>
#include <Standard_Real.hxx>
#include <MeshVS_DataSource.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <MeshVS_EntityType.hxx>
#include <MeshVS_HArray1OfSequenceOfInteger.hxx>
#include <Standard_Address.hxx>
#include <TColStd_Array1OfInteger.hxx>
class MeshVS_DataSource;
class gp_Vec;


class MeshVS_DeformedDataSource;
DEFINE_STANDARD_HANDLE(MeshVS_DeformedDataSource, MeshVS_DataSource)

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
  Standard_EXPORT MeshVS_DeformedDataSource(const Handle(MeshVS_DataSource)& theNonDeformDS, const Standard_Real theMagnify);
  
  Standard_EXPORT virtual Standard_Boolean GetGeom (const Standard_Integer ID, const Standard_Boolean IsElement, TColStd_Array1OfReal& Coords, Standard_Integer& NbNodes, MeshVS_EntityType& Type) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean GetGeomType (const Standard_Integer ID, const Standard_Boolean IsElement, MeshVS_EntityType& Type) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean Get3DGeom (const Standard_Integer ID, Standard_Integer& NbNodes, Handle(MeshVS_HArray1OfSequenceOfInteger)& Data) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Address GetAddr (const Standard_Integer ID, const Standard_Boolean IsElement) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean GetNodesByElement (const Standard_Integer ID, TColStd_Array1OfInteger& NodeIDs, Standard_Integer& NbNodes) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual const TColStd_PackedMapOfInteger& GetAllNodes() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual const TColStd_PackedMapOfInteger& GetAllElements() const Standard_OVERRIDE;
  
  //! This method returns map of nodal displacement vectors
  Standard_EXPORT const MeshVS_DataMapOfIntegerVector& GetVectors() const;
  
  //! This method sets map of nodal displacement vectors (Map).
  Standard_EXPORT void SetVectors (const MeshVS_DataMapOfIntegerVector& Map);
  
  //! This method returns vector ( Vect ) assigned to node number ID.
  Standard_EXPORT Standard_Boolean GetVector (const Standard_Integer ID, gp_Vec& Vect) const;
  
  //! This method sets vector ( Vect ) assigned to node number ID.
  Standard_EXPORT void SetVector (const Standard_Integer ID, const gp_Vec& Vect);
  
  Standard_EXPORT void SetNonDeformedDataSource (const Handle(MeshVS_DataSource)& theDS);
  
  //! With this methods you can read and change internal canonical data source
  Standard_EXPORT Handle(MeshVS_DataSource) GetNonDeformedDataSource() const;
  
  Standard_EXPORT void SetMagnify (const Standard_Real theMagnify);
  
  //! With this methods you can read and change magnify coefficient of nodal displacements
  Standard_EXPORT Standard_Real GetMagnify() const;




  DEFINE_STANDARD_RTTIEXT(MeshVS_DeformedDataSource,MeshVS_DataSource)

protected:




private:


  Handle(MeshVS_DataSource) myNonDeformedDataSource;
  TColStd_PackedMapOfInteger myEmptyMap;
  MeshVS_DataMapOfIntegerVector myVectors;
  Standard_Real myMagnify;


};







#endif // _MeshVS_DeformedDataSource_HeaderFile
