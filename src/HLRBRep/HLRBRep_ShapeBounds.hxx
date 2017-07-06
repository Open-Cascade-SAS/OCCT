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
class Standard_Transient;


//! Contains  a Shape and the  bounds of its vertices,
//! edges and faces in the DataStructure.
class HLRBRep_ShapeBounds 
{
public:

  DEFINE_STANDARD_ALLOC

  
    HLRBRep_ShapeBounds();
  
  Standard_EXPORT HLRBRep_ShapeBounds(const Handle(HLRTopoBRep_OutLiner)& S, const Handle(Standard_Transient)& SData, const Standard_Integer nbIso, const Standard_Integer V1, const Standard_Integer V2, const Standard_Integer E1, const Standard_Integer E2, const Standard_Integer F1, const Standard_Integer F2);
  
  Standard_EXPORT HLRBRep_ShapeBounds(const Handle(HLRTopoBRep_OutLiner)& S, const Standard_Integer nbIso, const Standard_Integer V1, const Standard_Integer V2, const Standard_Integer E1, const Standard_Integer E2, const Standard_Integer F1, const Standard_Integer F2);
  
  Standard_EXPORT void Translate (const Standard_Integer NV, const Standard_Integer NE, const Standard_Integer NF);
  
    void Shape (const Handle(HLRTopoBRep_OutLiner)& S);
  
    const Handle(HLRTopoBRep_OutLiner)& Shape() const;
  
    void ShapeData (const Handle(Standard_Transient)& SD);
  
    const Handle(Standard_Transient)& ShapeData() const;
  
    void NbOfIso (const Standard_Integer nbIso);
  
    Standard_Integer NbOfIso() const;
  
  Standard_EXPORT void Sizes (Standard_Integer& NV, Standard_Integer& NE, Standard_Integer& NF) const;
  
  Standard_EXPORT void Bounds (Standard_Integer& V1, Standard_Integer& V2, Standard_Integer& E1, Standard_Integer& E2, Standard_Integer& F1, Standard_Integer& F2) const;
  
  void UpdateMinMax (const HLRAlgo_EdgesBlock::MinMaxIndices& theTotMinMax)
  {
    myMinMax = theTotMinMax;
  }
  
  HLRAlgo_EdgesBlock::MinMaxIndices& MinMax()
  {
    return myMinMax;
  }




protected:





private:



  Handle(HLRTopoBRep_OutLiner) myShape;
  Handle(Standard_Transient) myShapeData;
  Standard_Integer myNbIso;
  Standard_Integer myVertStart;
  Standard_Integer myVertEnd;
  Standard_Integer myEdgeStart;
  Standard_Integer myEdgeEnd;
  Standard_Integer myFaceStart;
  Standard_Integer myFaceEnd;
  HLRAlgo_EdgesBlock::MinMaxIndices myMinMax;


};


#include <HLRBRep_ShapeBounds.lxx>





#endif // _HLRBRep_ShapeBounds_HeaderFile
