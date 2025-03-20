// Created on: 1996-04-10
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AdvApp2Var_Framework_HeaderFile
#define _AdvApp2Var_Framework_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AdvApp2Var_SequenceOfNode.hxx>
#include <AdvApp2Var_SequenceOfStrip.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_IsoType.hxx>
#include <Standard_Real.hxx>
#include <TColStd_HArray1OfReal.hxx>
class AdvApp2Var_Iso;
class AdvApp2Var_Node;

class AdvApp2Var_Framework
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT AdvApp2Var_Framework();

  Standard_EXPORT AdvApp2Var_Framework(const AdvApp2Var_SequenceOfNode&  Frame,
                                       const AdvApp2Var_SequenceOfStrip& UFrontier,
                                       const AdvApp2Var_SequenceOfStrip& VFrontier);

  //! search the Index of the first Iso not approximated,
  //! if all Isos are approximated NULL is returned.
  Standard_EXPORT Handle(AdvApp2Var_Iso) FirstNotApprox(Standard_Integer& IndexIso,
                                                        Standard_Integer& IndexStrip) const;

  Standard_EXPORT Standard_Integer FirstNode(const GeomAbs_IsoType  Type,
                                             const Standard_Integer IndexIso,
                                             const Standard_Integer IndexStrip) const;

  Standard_EXPORT Standard_Integer LastNode(const GeomAbs_IsoType  Type,
                                            const Standard_Integer IndexIso,
                                            const Standard_Integer IndexStrip) const;

  Standard_EXPORT void ChangeIso(const Standard_Integer        IndexIso,
                                 const Standard_Integer        IndexStrip,
                                 const Handle(AdvApp2Var_Iso)& anIso);

  const Handle(AdvApp2Var_Node)& Node(const Standard_Integer IndexNode) const
  {
    return myNodeConstraints.Value(IndexNode);
  }

  Standard_EXPORT const Handle(AdvApp2Var_Node)& Node(const Standard_Real U,
                                                      const Standard_Real V) const;

  Standard_EXPORT const AdvApp2Var_Iso& IsoU(const Standard_Real U,
                                             const Standard_Real V0,
                                             const Standard_Real V1) const;

  Standard_EXPORT const AdvApp2Var_Iso& IsoV(const Standard_Real U0,
                                             const Standard_Real U1,
                                             const Standard_Real V) const;

  Standard_EXPORT void UpdateInU(const Standard_Real CuttingValue);

  Standard_EXPORT void UpdateInV(const Standard_Real CuttingValue);

  Standard_EXPORT const Handle(TColStd_HArray1OfReal)& UEquation(
    const Standard_Integer IndexIso,
    const Standard_Integer IndexStrip) const;

  Standard_EXPORT const Handle(TColStd_HArray1OfReal)& VEquation(
    const Standard_Integer IndexIso,
    const Standard_Integer IndexStrip) const;

private:
  AdvApp2Var_SequenceOfNode  myNodeConstraints;
  AdvApp2Var_SequenceOfStrip myUConstraints;
  AdvApp2Var_SequenceOfStrip myVConstraints;
};

#endif // _AdvApp2Var_Framework_HeaderFile
