// Created on: 1993-02-03
// Created by: Laurent BOURESCHE
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

#ifndef _BRepSweep_Translation_HeaderFile
#define _BRepSweep_Translation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Vec.hxx>
#include <Standard_Boolean.hxx>
#include <BRepSweep_Trsf.hxx>
#include <TopAbs_Orientation.hxx>
class TopoDS_Shape;
class Sweep_NumShape;
class TopLoc_Location;

//! Provides   an  algorithm   to   build  object   by
//! translation sweep.
class BRepSweep_Translation : public BRepSweep_Trsf
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates  a  topology by  translating <S>  with the
  //! vector  <V>. If  C  is   true S Sucomponents   are
  //! copied. If Canonize is true then generated surfaces
  //! are attempted to be canonized in simple types
  Standard_EXPORT BRepSweep_Translation(const TopoDS_Shape&    S,
                                        const Sweep_NumShape&  N,
                                        const TopLoc_Location& L,
                                        const gp_Vec&          V,
                                        const Standard_Boolean C,
                                        const Standard_Boolean Canonize = Standard_True);

  //! Builds the vertex addressed by [aGenV,aDirV], with its
  //! geometric part, but without subcomponents.
  Standard_EXPORT TopoDS_Shape MakeEmptyVertex(const TopoDS_Shape&   aGenV,
                                               const Sweep_NumShape& aDirV);

  //! Builds the edge addressed by [aGenV,aDirE], with its
  //! geometric part, but without subcomponents.
  Standard_EXPORT TopoDS_Shape MakeEmptyDirectingEdge(const TopoDS_Shape&   aGenV,
                                                      const Sweep_NumShape& aDirE);

  //! Builds the edge addressed by [aGenE,aDirV], with its
  //! geometric part, but without subcomponents.
  Standard_EXPORT TopoDS_Shape MakeEmptyGeneratingEdge(const TopoDS_Shape&   aGenE,
                                                       const Sweep_NumShape& aDirV);

  //! Sets the  parameters of the new  vertex  on the new
  //! face. The new face and  new vertex where generated
  //! from aGenF, aGenV and aDirV .
  Standard_EXPORT void SetParameters(const TopoDS_Shape&   aNewFace,
                                     TopoDS_Shape&         aNewVertex,
                                     const TopoDS_Shape&   aGenF,
                                     const TopoDS_Shape&   aGenV,
                                     const Sweep_NumShape& aDirV);

  //! Sets the  parameter of the new  vertex  on the new
  //! edge. The new edge and  new vertex where generated
  //! from aGenV aDirE, and aDirV.
  Standard_EXPORT void SetDirectingParameter(const TopoDS_Shape&   aNewEdge,
                                             TopoDS_Shape&         aNewVertex,
                                             const TopoDS_Shape&   aGenV,
                                             const Sweep_NumShape& aDirE,
                                             const Sweep_NumShape& aDirV);

  //! Sets the  parameter of the new  vertex  on the new
  //! edge. The new edge and  new vertex where generated
  //! from aGenE, aGenV and aDirV .
  Standard_EXPORT void SetGeneratingParameter(const TopoDS_Shape&   aNewEdge,
                                              TopoDS_Shape&         aNewVertex,
                                              const TopoDS_Shape&   aGenE,
                                              const TopoDS_Shape&   aGenV,
                                              const Sweep_NumShape& aDirV);

  //! Builds the  face addressed  by [aGenS,aDirS], with
  //! its geometric part, but without subcomponents. The
  //! couple aGenS, aDirS can  be a "generating face and
  //! a  directing  vertex" or  "a generating edge and a
  //! directing  edge".
  Standard_EXPORT TopoDS_Shape MakeEmptyFace(const TopoDS_Shape&   aGenS,
                                             const Sweep_NumShape& aDirS);

  //! Sets the PCurve for a new edge on a new face. The
  //! new edge and  the  new face were generated  using
  //! aGenF, aGenE and aDirV.
  Standard_EXPORT void SetPCurve(const TopoDS_Shape&      aNewFace,
                                 TopoDS_Shape&            aNewEdge,
                                 const TopoDS_Shape&      aGenF,
                                 const TopoDS_Shape&      aGenE,
                                 const Sweep_NumShape&    aDirV,
                                 const TopAbs_Orientation orien);

  //! Sets the PCurve for a new edge on a new face. The
  //! new edge and  the  new face were generated  using
  //! aGenE, aDirE and aDirV.
  Standard_EXPORT void SetGeneratingPCurve(const TopoDS_Shape&      aNewFace,
                                           TopoDS_Shape&            aNewEdge,
                                           const TopoDS_Shape&      aGenE,
                                           const Sweep_NumShape&    aDirE,
                                           const Sweep_NumShape&    aDirV,
                                           const TopAbs_Orientation orien);

  //! Sets the PCurve for a new edge on a new face. The
  //! new edge and  the  new face were generated  using
  //! aGenE, aDirE and aGenV.
  Standard_EXPORT void SetDirectingPCurve(const TopoDS_Shape&      aNewFace,
                                          TopoDS_Shape&            aNewEdge,
                                          const TopoDS_Shape&      aGenE,
                                          const TopoDS_Shape&      aGenV,
                                          const Sweep_NumShape&    aDirE,
                                          const TopAbs_Orientation orien);

  //! Returns the Orientation of the  shell in the solid
  //! generated by the face aGenS  with  the edge aDirS.
  //! It is  REVERSED  if the surface is  swept  in  the
  //! direction of the normal.
  Standard_EXPORT TopAbs_Orientation DirectSolid(const TopoDS_Shape&   aGenS,
                                                 const Sweep_NumShape& aDirS);

  //! Returns   true   if  aNewSubShape    (addressed by
  //! aSubGenS  and aDirS)  must  be added  in aNewShape
  //! (addressed by aGenS and aDirS).
  Standard_EXPORT Standard_Boolean GGDShapeIsToAdd(const TopoDS_Shape&   aNewShape,
                                                   const TopoDS_Shape&   aNewSubShape,
                                                   const TopoDS_Shape&   aGenS,
                                                   const TopoDS_Shape&   aSubGenS,
                                                   const Sweep_NumShape& aDirS) const;

  //! Returns   true   if  aNewSubShape    (addressed by
  //! aGenS  and aSubDirS)  must  be added  in aNewShape
  //! (addressed by aGenS and aDirS).
  Standard_EXPORT Standard_Boolean GDDShapeIsToAdd(const TopoDS_Shape&   aNewShape,
                                                   const TopoDS_Shape&   aNewSubShape,
                                                   const TopoDS_Shape&   aGenS,
                                                   const Sweep_NumShape& aDirS,
                                                   const Sweep_NumShape& aSubDirS) const;

  //! In  some  particular  cases  the   topology  of  a
  //! generated  face must be  composed  of  independent
  //! closed wires,  in this case  this function returns
  //! true.
  //! Here it always returns false.
  Standard_EXPORT Standard_Boolean SeparatedWires(const TopoDS_Shape&   aNewShape,
                                                  const TopoDS_Shape&   aNewSubShape,
                                                  const TopoDS_Shape&   aGenS,
                                                  const TopoDS_Shape&   aSubGenS,
                                                  const Sweep_NumShape& aDirS) const;

  //! Returns true   if aDirS   and aGenS  addresses   a
  //! resulting Shape. In some  specific cases the shape
  //! can  be    geometrically   inexsistant,  then this
  //! function returns false.
  Standard_EXPORT Standard_Boolean HasShape(const TopoDS_Shape&   aGenS,
                                            const Sweep_NumShape& aDirS) const;

  //! Returns  always     false   because    here    the
  //! transformation is a translation.
  Standard_EXPORT Standard_Boolean IsInvariant(const TopoDS_Shape& aGenS) const;

  //! Returns the Vector of the Prism,  if it is an infinite
  //! prism the Vec is unitar.
  Standard_EXPORT gp_Vec Vec() const;

protected:
private:
  gp_Vec           myVec;
  Standard_Boolean myCanonize;
};

#endif // _BRepSweep_Translation_HeaderFile
