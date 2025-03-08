// Created on: 1995-01-27
// Created by: Marie Jose MARTZ
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepToIGES_BRWire_HeaderFile
#define _BRepToIGES_BRWire_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepToIGES_BREntity.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

class IGESData_IGESEntity;
class TopoDS_Shape;
class TopoDS_Vertex;
class TopoDS_Edge;
class TopoDS_Face;
class Geom_Surface;
class TopLoc_Location;
class gp_Pnt2d;
class TopoDS_Wire;

//! This class implements the transfer of Shape Entities
//! from Geom To IGES. These can be :
//! . Vertex
//! . Edge
//! . Wire
class BRepToIGES_BRWire : public BRepToIGES_BREntity
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepToIGES_BRWire();

  Standard_EXPORT BRepToIGES_BRWire(const BRepToIGES_BREntity& BR);

  //! Transfert a Shape entity from TopoDS to IGES
  //! this entity must be a Vertex or an Edge or a Wire.
  //! If this Entity could not be converted,
  //! this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferWire(const TopoDS_Shape& start);

  //! Transfert a Vertex entity from TopoDS to IGES
  //! If this Entity could not be converted,
  //! this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferVertex(const TopoDS_Vertex& myvertex);

  //! Transfert a Vertex entity on an Edge from TopoDS to IGES
  //! Returns the parameter of myvertex on myedge.
  //! If this Entity could not be converted,
  //! this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferVertex(const TopoDS_Vertex& myvertex,
                                                             const TopoDS_Edge&   myedge,
                                                             Standard_Real&       parameter);

  //! Transfert a Vertex entity of an edge on a Face
  //! from TopoDS to IGES
  //! Returns the parameter of myvertex on the pcurve
  //! of myedge on myface
  //! If this Entity could not be converted,
  //! this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferVertex(const TopoDS_Vertex& myvertex,
                                                             const TopoDS_Edge&   myedge,
                                                             const TopoDS_Face&   myface,
                                                             Standard_Real&       parameter);

  //! Transfert a Vertex entity of an edge on a Surface
  //! from TopoDS to IGES
  //! Returns the parameter of myvertex on the pcurve
  //! of myedge on mysurface
  //! If this Entity could not be converted,
  //! this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferVertex(const TopoDS_Vertex&        myvertex,
                                                             const TopoDS_Edge&          myedge,
                                                             const Handle(Geom_Surface)& mysurface,
                                                             const TopLoc_Location&      myloc,
                                                             Standard_Real&              parameter);

  //! Transfert a Vertex entity on a Face from TopoDS to IGES
  //! Returns the parameters of myvertex on myface
  //! If this Entity could not be converted,
  //! this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferVertex(const TopoDS_Vertex& myvertex,
                                                             const TopoDS_Face&   myface,
                                                             gp_Pnt2d&            mypoint);

  //! Transfert an Edge 3d entity from TopoDS to IGES
  //! If edge is REVERSED and isBRepMode is False 3D edge curve is reversed
  //! @param[in] theEdge input edge to transfer
  //! @param[in] theOriginMap shapemap contains the original shapes. Should be empty if face is not
  //! reversed
  //! @param[in] theIsBRepMode indicates if write mode is BRep
  //! @return Iges entity or null if could not be converted
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferEdge(
    const TopoDS_Edge&                  theEdge,
    const TopTools_DataMapOfShapeShape& theOriginMap,
    const Standard_Boolean              theIsBRepMode);

  //! Transfert an Edge 2d entity on a Face from TopoDS to IGES
  //! @param[in] theEdge input edge to transfer
  //! @param[in] theFace input face to get the surface and UV coordinates from it
  //! @param[in] theOriginMap shapemap contains the original shapes. Should be empty if face is not
  //! reversed
  //! @param[in] theLength input surface length
  //! @param[in] theIsBRepMode indicates if write mode is BRep
  //! @return Iges entity or null if could not be converted
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferEdge(
    const TopoDS_Edge&                  theEdge,
    const TopoDS_Face&                  theFace,
    const TopTools_DataMapOfShapeShape& theOriginMap,
    const Standard_Real                 theLength,
    const Standard_Boolean              theIsBRepMode);

  //! Transfert a Wire entity from TopoDS to IGES
  //! If this Entity could not be converted,
  //! this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferWire(const TopoDS_Wire& mywire);

  //! Transfert a Wire entity from TopoDS to IGES.
  //! @param[in] theWire input wire
  //! @param[in] theFace input face
  //! @param[in] theOriginMap shapemap contains the original shapes. Should be empty if face is not
  //! reversed
  //! @param[in] theCurve2d input curve 2d
  //! @param[in] theLength input surface length
  //! @return Iges entity (the curve associated to mywire in the parametric space of myface)
  //! or null if could not be converted
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferWire(
    const TopoDS_Wire&                  theWire,
    const TopoDS_Face&                  theFace,
    const TopTools_DataMapOfShapeShape& theOriginMap,
    Handle(IGESData_IGESEntity)&        theCurve2d,
    const Standard_Real                 theLength);

protected:
private:
};

#endif // _BRepToIGES_BRWire_HeaderFile
