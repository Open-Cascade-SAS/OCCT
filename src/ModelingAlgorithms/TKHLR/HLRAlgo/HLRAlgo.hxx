// Created on: 1992-02-18
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRAlgo_HeaderFile
#define _HLRAlgo_HeaderFile

#include <HLRAlgo_WiresBlock.hxx>

//! In order to have the precision required in
//! industrial design, drawings need to offer the
//! possibility of removing lines, which are hidden
//! in a given projection. To do this, the Hidden
//! Line Removal component provides two
//! algorithms: HLRBRep_Algo and HLRBRep_PolyAlgo.
//! These algorithms remove or indicate lines
//! hidden by surfaces. For a given projection, they
//! calculate a set of lines characteristic of the
//! object being represented. They are also used
//! in conjunction with extraction utilities, which
//! reconstruct a new, simplified shape from a
//! selection of calculation results. This new shape
//! is made up of edges, which represent the lines
//! of the visualized shape in a plane. This plane is the projection plane.
//! HLRBRep_Algo takes into account the shape
//! itself. HLRBRep_PolyAlgo works with a
//! polyhedral simplification of the shape. When
//! you use HLRBRep_Algo, you obtain an exact
//! result, whereas, when you use
//! HLRBRep_PolyAlgo, you reduce computation
//! time but obtain polygonal segments.
class HLRAlgo
{
public:
  DEFINE_STANDARD_ALLOC

  //! Iterator  on the  visible or  hidden  parts of  an
  //! EdgeStatus.
  Standard_EXPORT static void UpdateMinMax(const Standard_Real x,
                                           const Standard_Real y,
                                           const Standard_Real z,
                                           Standard_Real       Min[16],
                                           Standard_Real       Max[16]);

  Standard_EXPORT static void EnlargeMinMax(const Standard_Real tol,
                                            Standard_Real       Min[16],
                                            Standard_Real       Max[16]);

  Standard_EXPORT static void InitMinMax(const Standard_Real Big,
                                         Standard_Real       Min[16],
                                         Standard_Real       Max[16]);

  Standard_EXPORT static void EncodeMinMax(HLRAlgo_EdgesBlock::MinMaxIndices& Min,
                                           HLRAlgo_EdgesBlock::MinMaxIndices& Max,
                                           HLRAlgo_EdgesBlock::MinMaxIndices& MinMax);

  Standard_EXPORT static Standard_Real SizeBox(HLRAlgo_EdgesBlock::MinMaxIndices& Min,
                                               HLRAlgo_EdgesBlock::MinMaxIndices& Max);

  Standard_EXPORT static void DecodeMinMax(const HLRAlgo_EdgesBlock::MinMaxIndices& MinMax,
                                           HLRAlgo_EdgesBlock::MinMaxIndices&       Min,
                                           HLRAlgo_EdgesBlock::MinMaxIndices&       Max);

  static void CopyMinMax(HLRAlgo_EdgesBlock::MinMaxIndices& IMin,
                         HLRAlgo_EdgesBlock::MinMaxIndices& IMax,
                         HLRAlgo_EdgesBlock::MinMaxIndices& OMin,
                         HLRAlgo_EdgesBlock::MinMaxIndices& OMax)
  {
    OMin = IMin;
    OMax = IMax;
  }

  Standard_EXPORT static void AddMinMax(HLRAlgo_EdgesBlock::MinMaxIndices& IMin,
                                        HLRAlgo_EdgesBlock::MinMaxIndices& IMax,
                                        HLRAlgo_EdgesBlock::MinMaxIndices& OMin,
                                        HLRAlgo_EdgesBlock::MinMaxIndices& OMax);
};

#endif // _HLRAlgo_HeaderFile
