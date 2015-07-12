// Created on: 1997-01-17
// Created by: Didier PIFFAULT
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

#ifndef _BRepAlgo_HeaderFile
#define _BRepAlgo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_Shape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Wire;
class TopoDS_Edge;
class TopoDS_Shape;
class BRepAlgo_BooleanOperation;
class BRepAlgo_Fuse;
class BRepAlgo_Cut;
class BRepAlgo_Common;
class BRepAlgo_Section;
class BRepAlgo_Loop;
class BRepAlgo_Tool;
class BRepAlgo_Image;
class BRepAlgo_AsDes;
class BRepAlgo_FaceRestrictor;
class BRepAlgo_BooleanOperations;
class BRepAlgo_DSAccess;
class BRepAlgo_EdgeConnector;
class BRepAlgo_NormalProjection;



//! The BRepAlgo package provides a full range of
//! services to perform Old Boolean Operations in Open CASCADE.
//! Attention:
//! The New Boolean Operation has replaced the Old
//! Boolean Operations algorithm in the BrepAlgoAPI
//! package in Open CASCADE.
class BRepAlgo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! this method makes a wire whose edges are C1 from
  //! a Wire whose edges could be G1. It removes a vertex
  //! between G1 edges.
  //! Option can be G1 or C1.
  Standard_EXPORT static TopoDS_Wire ConcatenateWire (const TopoDS_Wire& Wire, const GeomAbs_Shape Option, const Standard_Real AngularTolerance = 1.0e-4);
  
  //! this method makes an edge from a wire.
  //! Junction points between edges of wire may be sharp,
  //! resulting curve of the resulting edge may be C0.
  Standard_EXPORT static TopoDS_Edge ConcatenateWireC0 (const TopoDS_Wire& Wire);
  
  //! Checks if the  shape is "correct". If not, returns
  //! <Standard_False>, else returns <Standard_True>.
  Standard_EXPORT static Standard_Boolean IsValid (const TopoDS_Shape& S);
  
  //! Checks if  the  Generated and Modified Faces  from
  //! the shapes <arguments> in  the shape <result>  are
  //! "correct". The args   may be empty, then all faces
  //! will be checked.
  //! If <Closed> is True,  only  closed shape are valid.
  //! If <GeomCtrl>  is    False the geometry  of   new
  //! vertices and edges   are   not verified and  the
  //! auto-intersection of new wires are not searched.
  Standard_EXPORT static Standard_Boolean IsValid (const TopTools_ListOfShape& theArgs, const TopoDS_Shape& theResult, const Standard_Boolean closedSolid = Standard_False, const Standard_Boolean GeomCtrl = Standard_True);
  
  //! Checks if the shape is  "correct". If not, returns
  //! <Standard_False>,  else   returns <Standard_True>.
  //! This method differs from  the previous one in  the
  //! fact that no   geometric contols (intersection  of
  //! wires, pcurve validity) are performed.
  Standard_EXPORT static Standard_Boolean IsTopologicallyValid (const TopoDS_Shape& S);




protected:





private:




friend class BRepAlgo_BooleanOperation;
friend class BRepAlgo_Fuse;
friend class BRepAlgo_Cut;
friend class BRepAlgo_Common;
friend class BRepAlgo_Section;
friend class BRepAlgo_Loop;
friend class BRepAlgo_Tool;
friend class BRepAlgo_Image;
friend class BRepAlgo_AsDes;
friend class BRepAlgo_FaceRestrictor;
friend class BRepAlgo_BooleanOperations;
friend class BRepAlgo_DSAccess;
friend class BRepAlgo_EdgeConnector;
friend class BRepAlgo_NormalProjection;

};







#endif // _BRepAlgo_HeaderFile
