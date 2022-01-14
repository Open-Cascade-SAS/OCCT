// Created on: 1997-01-10
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

#ifndef _HLRAlgo_PolyInternalNode_HeaderFile
#define _HLRAlgo_PolyInternalNode_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
#include <gp_XYZ.hxx>
#include <gp_XY.hxx>


class HLRAlgo_PolyInternalNode;
DEFINE_STANDARD_HANDLE(HLRAlgo_PolyInternalNode, Standard_Transient)

//! to Update OutLines.
class HLRAlgo_PolyInternalNode : public Standard_Transient
{
public:
  struct NodeIndices
  {
    Standard_Integer NdSg, Flag, Edg1, Edg2;
  };

  struct NodeData
  {
    NodeData()
    : PCu1(0.0),
      PCu2(0.0),
      Scal(0.0)
    {
    }
    gp_XYZ Point, Normal;
    gp_XY UV;
    Standard_Real PCu1, PCu2, Scal;
  };

  HLRAlgo_PolyInternalNode()
  {
    myIndices.NdSg = 0;
    myIndices.Flag = 0;
    myIndices.Edg1 = 0;
    myIndices.Edg2 = 0;
  }

  NodeIndices& Indices()
  {
    return myIndices;
  }

  NodeData& Data()
  {
    return myData;
  }

  DEFINE_STANDARD_RTTIEXT(HLRAlgo_PolyInternalNode,Standard_Transient)

private:
  NodeIndices myIndices;
  NodeData myData;
};

#endif // _HLRAlgo_PolyInternalNode_HeaderFile
