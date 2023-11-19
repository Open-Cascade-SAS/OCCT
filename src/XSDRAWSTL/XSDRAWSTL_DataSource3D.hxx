// Created on: 2014-08-04
// Created by: Artem NOVIKOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _XSDRAWSTL_DataSource3D_HeaderFile
#define _XSDRAWSTL_DataSource3D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <MeshVS_DataSource.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <MeshVS_EntityType.hxx>
#include <MeshVS_HArray1OfSequenceOfInteger.hxx>
#include <TColStd_Array1OfInteger.hxx>


class XSDRAWSTL_DataSource3D;
DEFINE_STANDARD_HANDLE(XSDRAWSTL_DataSource3D, MeshVS_DataSource)

//! The sample DataSource3D for working with STLMesh_Mesh
class XSDRAWSTL_DataSource3D : public MeshVS_DataSource
{

public:

  
  //! Constructor
  Standard_EXPORT XSDRAWSTL_DataSource3D();
  
  //! Returns geometry information about node (if IsElement is False) or element (IsElement is True) by coordinates.
  //! For element this method must return all its nodes coordinates in the strict order: X, Y, Z and
  //! with nodes order is the same as in wire bounding the face or link. NbNodes is number of nodes of element.
  //! It is recommended to return 1 for node. Type is an element type.
  Standard_EXPORT Standard_Boolean GetGeom (const Standard_Integer theID, const Standard_Boolean theIsElement, TColStd_Array1OfReal& theCoords, Standard_Integer& theNbNodes, MeshVS_EntityType& theType) const Standard_OVERRIDE;
  
  //! This method returns topology information about 3D-element
  //! Returns false if element with ID isn't 3D or because other troubles
  Standard_EXPORT virtual Standard_Boolean Get3DGeom (const Standard_Integer theID, Standard_Integer& theNbNodes, Handle(MeshVS_HArray1OfSequenceOfInteger)& theData) const Standard_OVERRIDE;
  
  //! This method is similar to GetGeom, but returns only element or node type. This method is provided for
  //! a fine performance.
  Standard_EXPORT Standard_Boolean GetGeomType (const Standard_Integer theID, const Standard_Boolean theIsElement, MeshVS_EntityType& theType) const Standard_OVERRIDE;
  
  //! This method returns by number an address of any entity which represents element or node data structure.
  Standard_EXPORT Standard_Address GetAddr (const Standard_Integer theID, const Standard_Boolean theIsElement) const Standard_OVERRIDE;
  
  //! This method returns information about what node this element consist of.
  Standard_EXPORT virtual Standard_Boolean GetNodesByElement (const Standard_Integer theID, TColStd_Array1OfInteger& theNodeIDs, Standard_Integer& theNbNodes) const Standard_OVERRIDE;
  
  //! This method returns map of all nodes the object consist of.
  Standard_EXPORT const TColStd_PackedMapOfInteger& GetAllNodes() const Standard_OVERRIDE;
  
  //! This method returns map of all elements the object consist of.
  Standard_EXPORT const TColStd_PackedMapOfInteger& GetAllElements() const Standard_OVERRIDE;
  
  //! This method calculates normal of face, which is using for correct reflection presentation.
  //! There is default method, for advance reflection this method can be redefined.
  Standard_EXPORT virtual Standard_Boolean GetNormal (const Standard_Integer theID, const Standard_Integer theMax, Standard_Real& theNx, Standard_Real& theNy, Standard_Real& theNz) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XSDRAWSTL_DataSource3D,MeshVS_DataSource)

protected:




private:


  TColStd_PackedMapOfInteger myNodes;
  TColStd_PackedMapOfInteger myElements;
  Handle(TColStd_HArray1OfInteger) myElemNbNodes;
  Handle(TColStd_HArray2OfReal) myNodeCoords;
  Handle(TColStd_HArray2OfInteger) myElemNodes;


};







#endif // _XSDRAWSTL_DataSource3D_HeaderFile
