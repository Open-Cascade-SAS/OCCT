// Created on: 1997-08-22
// Created by: Prestataire Mary FABIEN
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BRepAlgo_EdgeConnector_HeaderFile
#define _BRepAlgo_EdgeConnector_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopTools_ListOfShape.hxx>
#include <BRepAlgo_DataMapOfShapeBoolean.hxx>
#include <TopOpeBRepBuild_BlockBuilder.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
class TopoDS_Edge;
class TopoDS_Shape;


class BRepAlgo_EdgeConnector;
DEFINE_STANDARD_HANDLE(BRepAlgo_EdgeConnector, Standard_Transient)

//! Used by DSAccess to reconstruct an EdgeSet of connected edges. The result produced by
//! MakeBlock is a list of non-standard TopoDS_wire,
//! which  can present connexions of edge  of  order > 2
//! in certain  vertex. The method  IsWire
//! indicates standard/non-standard character of  all wire produced.
class BRepAlgo_EdgeConnector : public Standard_Transient
{

public:

  
  Standard_EXPORT BRepAlgo_EdgeConnector();
  
  Standard_EXPORT void Add (const TopoDS_Edge& e);
  
  Standard_EXPORT void Add (TopTools_ListOfShape& LOEdge);
  
  Standard_EXPORT void AddStart (const TopoDS_Shape& e);
  
  Standard_EXPORT void AddStart (TopTools_ListOfShape& LOEdge);
  
  Standard_EXPORT void ClearStartElement();
  
  //! returns a list of wire non standard
  Standard_EXPORT TopTools_ListOfShape& MakeBlock();
  
  Standard_EXPORT void Done();
  
  //! NYI
  //! returns true if proceeded  to MakeBlock()
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! NYI
  //! returns true if W is  a Wire standard.
  //! W must belong  to the list returned  by MakeBlock.
  Standard_EXPORT Standard_Boolean IsWire (const TopoDS_Shape& W);




  DEFINE_STANDARD_RTTIEXT(BRepAlgo_EdgeConnector,Standard_Transient)

protected:




private:


  TopTools_ListOfShape myListeOfEdge;
  TopTools_ListOfShape myListeOfStartEdge;
  BRepAlgo_DataMapOfShapeBoolean myResultMap;
  TopTools_ListOfShape myResultList;
  TopOpeBRepBuild_BlockBuilder myBlockB;
  Standard_Boolean myIsDone;


};







#endif // _BRepAlgo_EdgeConnector_HeaderFile
