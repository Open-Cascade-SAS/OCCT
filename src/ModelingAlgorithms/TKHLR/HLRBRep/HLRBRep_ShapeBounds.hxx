// Created on: 1997-04-17
// Created by: Christophe MARION
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

#ifndef _HLRBRep_ShapeBounds_HeaderFile
#define _HLRBRep_ShapeBounds_HeaderFile

#include <HLRAlgo_EdgesBlock.hxx>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class HLRTopoBRep_OutLiner;

//! Contains a Shape and the bounds of its vertices,
//! edges and faces in the DataStructure.
class HLRBRep_ShapeBounds
{
public:
  DEFINE_STANDARD_ALLOC

  HLRBRep_ShapeBounds();

  Standard_EXPORT HLRBRep_ShapeBounds(const occ::handle<HLRTopoBRep_OutLiner>& S,
                                      const occ::handle<Standard_Transient>&   SData,
                                      const int                                nbIso,
                                      const int                                V1,
                                      const int                                V2,
                                      const int                                E1,
                                      const int                                E2,
                                      const int                                F1,
                                      const int                                F2);

  Standard_EXPORT HLRBRep_ShapeBounds(const occ::handle<HLRTopoBRep_OutLiner>& S,
                                      const int                                nbIso,
                                      const int                                V1,
                                      const int                                V2,
                                      const int                                E1,
                                      const int                                E2,
                                      const int                                F1,
                                      const int                                F2);

  Standard_EXPORT void Translate(const int NV, const int NE, const int NF);

  void Shape(const occ::handle<HLRTopoBRep_OutLiner>& S);

  const occ::handle<HLRTopoBRep_OutLiner>& Shape() const;

  void ShapeData(const occ::handle<Standard_Transient>& SD);

  const occ::handle<Standard_Transient>& ShapeData() const;

  void NbOfIso(const int nbIso);

  int NbOfIso() const;

  Standard_EXPORT void Sizes(int& NV, int& NE, int& NF) const;

  Standard_EXPORT void Bounds(int& V1, int& V2, int& E1, int& E2, int& F1, int& F2) const;

  void UpdateMinMax(const HLRAlgo_EdgesBlock::MinMaxIndices& theTotMinMax)
  {
    myMinMax = theTotMinMax;
  }

  HLRAlgo_EdgesBlock::MinMaxIndices& MinMax() { return myMinMax; }

private:
  occ::handle<HLRTopoBRep_OutLiner> myShape;
  occ::handle<Standard_Transient>   myShapeData;
  int                               myNbIso;
  int                               myVertStart;
  int                               myVertEnd;
  int                               myEdgeStart;
  int                               myEdgeEnd;
  int                               myFaceStart;
  int                               myFaceEnd;
  HLRAlgo_EdgesBlock::MinMaxIndices myMinMax;
};

#include <HLRBRep_ShapeBounds.lxx>

#endif // _HLRBRep_ShapeBounds_HeaderFile
