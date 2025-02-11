// Created on: 1993-01-27
// Created by: Philippe DAUTRY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepSweep_NumLinearRegularSweep_HeaderFile
#define _BRepSweep_NumLinearRegularSweep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <BRepSweep_Builder.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepSweep_Tool.hxx>
#include <Sweep_NumShapeTool.hxx>
#include <TopTools_Array2OfShape.hxx>
#include <TColStd_Array2OfBoolean.hxx>
#include <TopAbs_Orientation.hxx>

//! This  a generic  class  is  used   to build Sweept
//! primitives   with    a  generating  "shape"  and a
//! directing "line".
//!
//! The indexation and type analysis services required
//! for the generatrix are given by <Tool from BRepSweep>.
//!
//! The indexation and type analysis services required
//! for the directrix are given by <NumShapeTool from Sweep>.
//!
//! The iteration services required for the generatrix
//! are given by <Iterator from BRepSweep>.
//!
//! The iteration services required  for the directrix
//! are given by <NumShapeIterator from Sweep>.
//!
//! The topology is like a grid of shapes.  Each shape
//! of the grid  must be addressable without confusion
//! by one  or  two objects   from  the generating  or
//! directing   shapes.  Here are  examples of correct
//! associations to address:
//!
//! - a vertex : GenVertex - DirVertex
//! - an edge  : GenVertex - DirEdge
//! -          : GenEdge   - DirVertex
//! - a face   : GenEdge   - DirEdge
//! GenFace   - DirVertex
//! ...
//!
//! "GenObject" is used to identify an object from the
//! Generating Shape, and "DirObject" from the
//! Directing Shape. So may they be from different
//! types.
//!
//! The method Has... is given because in some special
//! cases, a vertex, an  edge or a face may be
//! geometricaly nonexistent or not useful.
class BRepSweep_NumLinearRegularSweep
{
public:
  DEFINE_STANDARD_ALLOC

  //! Builds the vertex addressed by [aGenV,aDirV], with its
  //! geometric part, but without subcomponents.
  Standard_EXPORT virtual TopoDS_Shape MakeEmptyVertex(const TopoDS_Shape&   aGenV,
                                                       const Sweep_NumShape& aDirV) = 0;
  Standard_EXPORT virtual ~BRepSweep_NumLinearRegularSweep();

  //! Builds the edge addressed by [aGenV,aDirE], with its
  //! geometric part, but without subcomponents.
  Standard_EXPORT virtual TopoDS_Shape MakeEmptyDirectingEdge(const TopoDS_Shape&   aGenV,
                                                              const Sweep_NumShape& aDirE) = 0;

  //! Builds the edge addressed by [aGenE,aDirV], with its
  //! geometric part, but without subcomponents.
  Standard_EXPORT virtual TopoDS_Shape MakeEmptyGeneratingEdge(const TopoDS_Shape&   aGenE,
                                                               const Sweep_NumShape& aDirV) = 0;

  //! Sets the  parameters of the new  vertex  on the new
  //! face. The new face and  new vertex where generated
  //! from aGenF, aGenV and aDirV .
  Standard_EXPORT virtual void SetParameters(const TopoDS_Shape&   aNewFace,
                                             TopoDS_Shape&         aNewVertex,
                                             const TopoDS_Shape&   aGenF,
                                             const TopoDS_Shape&   aGenV,
                                             const Sweep_NumShape& aDirV) = 0;

  //! Sets the  parameter of the new  vertex  on the new
  //! edge. The new edge and  new vertex where generated
  //! from aGenV aDirE, and aDirV.
  Standard_EXPORT virtual void SetDirectingParameter(const TopoDS_Shape&   aNewEdge,
                                                     TopoDS_Shape&         aNewVertex,
                                                     const TopoDS_Shape&   aGenV,
                                                     const Sweep_NumShape& aDirE,
                                                     const Sweep_NumShape& aDirV) = 0;

  //! Sets the  parameter of the new  vertex  on the new
  //! edge. The new edge and  new vertex where generated
  //! from aGenE, aGenV and aDirV .
  Standard_EXPORT virtual void SetGeneratingParameter(const TopoDS_Shape&   aNewEdge,
                                                      TopoDS_Shape&         aNewVertex,
                                                      const TopoDS_Shape&   aGenE,
                                                      const TopoDS_Shape&   aGenV,
                                                      const Sweep_NumShape& aDirV) = 0;

  //! Builds the face  addressed by  [aGenS,aDirS], with
  //! its geometric part, but without subcomponents. The
  //! couple aGenS, aDirS  can be a "generating face and
  //! a directing vertex"   or "a generating  edge and a
  //! directing  edge".
  Standard_EXPORT virtual TopoDS_Shape MakeEmptyFace(const TopoDS_Shape&   aGenS,
                                                     const Sweep_NumShape& aDirS) = 0;

  //! Sets the PCurve for a new edge on a new face. The
  //! new edge and  the  new face were generated  using
  //! aGenF, aGenE and aDirV.
  Standard_EXPORT virtual void SetPCurve(const TopoDS_Shape&      aNewFace,
                                         TopoDS_Shape&            aNewEdge,
                                         const TopoDS_Shape&      aGenF,
                                         const TopoDS_Shape&      aGenE,
                                         const Sweep_NumShape&    aDirV,
                                         const TopAbs_Orientation orien) = 0;

  //! Sets the PCurve for a new edge on a new face. The
  //! new edge and  the  new face were generated  using
  //! aGenE, aDirE and aDirV.
  Standard_EXPORT virtual void SetGeneratingPCurve(const TopoDS_Shape&      aNewFace,
                                                   TopoDS_Shape&            aNewEdge,
                                                   const TopoDS_Shape&      aGenE,
                                                   const Sweep_NumShape&    aDirE,
                                                   const Sweep_NumShape&    aDirV,
                                                   const TopAbs_Orientation orien) = 0;

  //! Sets the PCurve for a new edge on a new face. The
  //! new edge and  the  new face were generated  using
  //! aGenE, aDirE and aGenV.
  Standard_EXPORT virtual void SetDirectingPCurve(const TopoDS_Shape&      aNewFace,
                                                  TopoDS_Shape&            aNewEdge,
                                                  const TopoDS_Shape&      aGenE,
                                                  const TopoDS_Shape&      aGenV,
                                                  const Sweep_NumShape&    aDirE,
                                                  const TopAbs_Orientation orien) = 0;

  //! Returns the Orientation of the  shell in the solid
  //! generated by the face aGenS  with  the edge aDirS.
  //! It is  REVERSED  if the surface is  swept  in  the
  //! direction of the normal.
  Standard_EXPORT virtual TopAbs_Orientation DirectSolid(const TopoDS_Shape&   aGenS,
                                                         const Sweep_NumShape& aDirS) = 0;

  //! Returns   true   if  aNewSubShape    (addressed by
  //! aSubGenS  and aDirS)  must  be added  in aNewShape
  //! (addressed by aGenS and aDirS).
  Standard_EXPORT virtual Standard_Boolean GGDShapeIsToAdd(const TopoDS_Shape&   aNewShape,
                                                           const TopoDS_Shape&   aNewSubShape,
                                                           const TopoDS_Shape&   aGenS,
                                                           const TopoDS_Shape&   aSubGenS,
                                                           const Sweep_NumShape& aDirS) const = 0;

  //! Returns   true   if  aNewSubShape    (addressed by
  //! aGenS  and aSubDirS)  must  be added  in aNewShape
  //! (addressed by aGenS and aDirS).
  Standard_EXPORT virtual Standard_Boolean GDDShapeIsToAdd(
    const TopoDS_Shape&   aNewShape,
    const TopoDS_Shape&   aNewSubShape,
    const TopoDS_Shape&   aGenS,
    const Sweep_NumShape& aDirS,
    const Sweep_NumShape& aSubDirS) const = 0;

  //! In  some  particular  cases  the   topology  of  a
  //! generated  face must be  composed  of  independent
  //! closed wires,  in this case  this function returns
  //! true.
  Standard_EXPORT virtual Standard_Boolean SeparatedWires(const TopoDS_Shape&   aNewShape,
                                                          const TopoDS_Shape&   aNewSubShape,
                                                          const TopoDS_Shape&   aGenS,
                                                          const TopoDS_Shape&   aSubGenS,
                                                          const Sweep_NumShape& aDirS) const = 0;

  //! In  some  particular  cases  the   topology  of  a
  //! generated  Shell must be  composed  of  independent
  //! closed Shells,  in this case  this function returns
  //! a Compound of independent Shells.
  Standard_EXPORT virtual TopoDS_Shape SplitShell(const TopoDS_Shape& aNewShape) const;

  //! Called to propagate the continuity of  every vertex
  //! between two edges of the  generating wire  aGenS on
  //! the generated edge and faces.
  Standard_EXPORT virtual void SetContinuity(const TopoDS_Shape&   aGenS,
                                             const Sweep_NumShape& aDirS) = 0;

  //! Returns true   if aDirS   and aGenS  addresses   a
  //! resulting Shape. In some  specific cases the shape
  //! can  be    geometrically   inexsistant,  then this
  //! function returns false.
  Standard_EXPORT virtual Standard_Boolean HasShape(const TopoDS_Shape&   aGenS,
                                                    const Sweep_NumShape& aDirS) const = 0;

  //! Returns true if aGenS cannot be transformed.
  Standard_EXPORT virtual Standard_Boolean IsInvariant(const TopoDS_Shape& aGenS) const = 0;

  //! Returns the resulting  Shape indexed by aDirS  and
  //! aGenS.
  Standard_EXPORT TopoDS_Shape Shape(const TopoDS_Shape& aGenS, const Sweep_NumShape& aDirS);

  //! Returns  the resulting Shape  indexed by myDirWire
  //! and aGenS.
  Standard_EXPORT TopoDS_Shape Shape(const TopoDS_Shape& aGenS);

  //! Returns true if the initial shape aGenS
  //! is used in result shape
  Standard_EXPORT Standard_Boolean IsUsed(const TopoDS_Shape& aGenS) const;

  //! Returns true if the shape, generated from theS
  //! is used in result shape
  Standard_EXPORT Standard_Boolean GenIsUsed(const TopoDS_Shape& theS) const;

  //! Returns the resulting  Shape indexed by  myDirWire
  //! and myGenShape.
  Standard_EXPORT TopoDS_Shape Shape();

  //! Returns the resulting Shape  indexed by the  first
  //! Vertex  of myDirWire and myGenShape.
  Standard_EXPORT TopoDS_Shape FirstShape();

  //! Returns the  resulting Shape  indexed by the  last
  //! Vertex of myDirWire and myGenShape.
  Standard_EXPORT TopoDS_Shape LastShape();

  //! Returns the resulting Shape  indexed by the  first
  //! Vertex  of myDirWire and aGenS.
  Standard_EXPORT TopoDS_Shape FirstShape(const TopoDS_Shape& aGenS);

  //! Returns the  resulting Shape  indexed by the  last
  //! Vertex of myDirWire and aGenS.
  Standard_EXPORT TopoDS_Shape LastShape(const TopoDS_Shape& aGenS);

  Standard_EXPORT Standard_Boolean Closed() const;

protected:
  //! Creates a NumLinearRegularSweep.    <aBuilder>  gives
  //! basic topological services.
  Standard_EXPORT BRepSweep_NumLinearRegularSweep(const BRepSweep_Builder& aBuilder,
                                                  const TopoDS_Shape&      aGenShape,
                                                  const Sweep_NumShape&    aDirWire);

  BRepSweep_Builder       myBuilder;
  TopoDS_Shape            myGenShape;
  Sweep_NumShape          myDirWire;
  BRepSweep_Tool          myGenShapeTool;
  Sweep_NumShapeTool      myDirShapeTool;
  TopTools_Array2OfShape  myShapes;
  TColStd_Array2OfBoolean myBuiltShapes;
  TColStd_Array2OfBoolean myUsedShapes;

private:
};

#endif // _BRepSweep_NumLinearRegularSweep_HeaderFile
