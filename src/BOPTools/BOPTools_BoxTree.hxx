// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef BOPTools_BoxTree_HeaderFile
#define BOPTools_BoxTree_HeaderFile

#include <BOPTools_BoxSelector.hxx>
#include <BOPTools_PairSelector.hxx>
#include <Standard_Integer.hxx>
#include <BVH_LinearBuilder.hxx>

//! Redefines BoxSet to use the Linear builder by default

template <class NumType, int Dimension, class DataType>
class BOPTools_BoxSet : public BVH_BoxSet <NumType, Dimension, DataType>
{
public: //! @name Constructors
  //! Empty constructor for use the default BVH_Builder
  BOPTools_BoxSet (const opencascade::handle <BVH_Builder <NumType, Dimension> >& theBuilder = NULL)
    : BVH_BoxSet <NumType, Dimension, DataType> (theBuilder.IsNull() ? new BVH_LinearBuilder<NumType, Dimension>() : theBuilder)
  {}
};

//! 2D definitions
typedef BOPTools_BoxSet <Standard_Real, 2, Standard_Integer> BOPTools_Box2dTree;
typedef BOPTools_BoxSelector<2> BOPTools_Box2dTreeSelector;
typedef BOPTools_PairSelector<2> BOPTools_Box2dPairSelector;

//! 3D definitions
typedef BOPTools_BoxSet <Standard_Real, 3, Standard_Integer> BOPTools_BoxTree;
typedef BOPTools_BoxSelector<3> BOPTools_BoxTreeSelector;
typedef BOPTools_PairSelector<3> BOPTools_BoxPairSelector;

#endif
