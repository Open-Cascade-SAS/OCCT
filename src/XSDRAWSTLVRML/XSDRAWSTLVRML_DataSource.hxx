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

#ifndef _XSDRAWSTLVRML_DataSource_HeaderFile
#define _XSDRAWSTLVRML_DataSource_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <MeshVS_DataSource.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <MeshVS_EntityType.hxx>
#include <Standard_Address.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_Real.hxx>
#include <Poly_Triangulation.hxx>


class XSDRAWSTLVRML_DataSource;
DEFINE_STANDARD_HANDLE(XSDRAWSTLVRML_DataSource, MeshVS_DataSource)

//! The sample DataSource for working with STLMesh_Mesh
class XSDRAWSTLVRML_DataSource : public MeshVS_DataSource
{

public:

  
  //! Constructor
  Standard_EXPORT XSDRAWSTLVRML_DataSource(const Handle(Poly_Triangulation)& aMesh);
  
  //! Returns geometry information about node (if IsElement is False) or element (IsElement is True) by coordinates.
  //! For element this method must return all its nodes coordinates in the strict order: X, Y, Z and
  //! with nodes order is the same as in wire bounding the face or link. NbNodes is number of nodes of element.
  //! It is recommended to return 1 for node. Type is an element type.
  Standard_EXPORT Standard_Boolean GetGeom (const Standard_Integer ID, const Standard_Boolean IsElement, TColStd_Array1OfReal& Coords, Standard_Integer& NbNodes, MeshVS_EntityType& Type) const Standard_OVERRIDE;
  
  //! This method is similar to GetGeom, but returns only element or node type. This method is provided for
  //! a fine performance.
  Standard_EXPORT Standard_Boolean GetGeomType (const Standard_Integer ID, const Standard_Boolean IsElement, MeshVS_EntityType& Type) const Standard_OVERRIDE;
  
  //! This method returns by number an address of any entity which represents element or node data structure.
  Standard_EXPORT Standard_Address GetAddr (const Standard_Integer ID, const Standard_Boolean IsElement) const Standard_OVERRIDE;
  
  //! This method returns information about what node this element consist of.
  Standard_EXPORT virtual Standard_Boolean GetNodesByElement (const Standard_Integer ID, TColStd_Array1OfInteger& NodeIDs, Standard_Integer& NbNodes) const Standard_OVERRIDE;
  
  //! This method returns map of all nodes the object consist of.
  Standard_EXPORT const TColStd_PackedMapOfInteger& GetAllNodes() const Standard_OVERRIDE;
  
  //! This method returns map of all elements the object consist of.
  Standard_EXPORT const TColStd_PackedMapOfInteger& GetAllElements() const Standard_OVERRIDE;
  
  //! This method calculates normal of face, which is using for correct reflection presentation.
  //! There is default method, for advance reflection this method can be redefined.
  Standard_EXPORT virtual Standard_Boolean GetNormal (const Standard_Integer Id, const Standard_Integer Max, Standard_Real& nx, Standard_Real& ny, Standard_Real& nz) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XSDRAWSTLVRML_DataSource,MeshVS_DataSource)

protected:




private:


  Handle(Poly_Triangulation) myMesh;
  TColStd_PackedMapOfInteger myNodes;
  TColStd_PackedMapOfInteger myElements;
  Handle(TColStd_HArray2OfInteger) myElemNodes;
  Handle(TColStd_HArray2OfReal) myNodeCoords;
  Handle(TColStd_HArray2OfReal) myElemNormals;


};







#endif // _XSDRAWSTLVRML_DataSource_HeaderFile
