// Created on: 1992-04-06
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

#ifndef _HLRAlgo_EdgesBlock_HeaderFile
#define _HLRAlgo_EdgesBlock_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Transient.hxx>
#include <TopAbs_Orientation.hxx>

//! An EdgesBlock is a set of Edges. It is used by the
//! DataStructure to structure the Edges.
//!
//! An EdgesBlock contains:
//!
//! * An Array of index of Edges.
//!
//! * An Array of flagsf
//! (Orientation
//!  OutLine
//!  Internal
//!  Double
//!  IsoLine)
class HLRAlgo_EdgesBlock : public Standard_Transient
{

public:
  struct MinMaxIndices
  {
    int Min[8], Max[8];

    MinMaxIndices& Minimize(const MinMaxIndices& theMinMaxIndices)
    {
      for (int aI = 0; aI < 8; ++aI)
      {
        if (Min[aI] > theMinMaxIndices.Min[aI])
        {
          Min[aI] = theMinMaxIndices.Min[aI];
        }
        if (Max[aI] > theMinMaxIndices.Max[aI])
        {
          Max[aI] = theMinMaxIndices.Max[aI];
        }
      }
      return *this;
    }

    MinMaxIndices& Maximize(const MinMaxIndices& theMinMaxIndices)
    {
      for (int aI = 0; aI < 8; ++aI)
      {
        if (Min[aI] < theMinMaxIndices.Min[aI])
        {
          Min[aI] = theMinMaxIndices.Min[aI];
        }
        if (Max[aI] < theMinMaxIndices.Max[aI])
        {
          Max[aI] = theMinMaxIndices.Max[aI];
        }
      }
      return *this;
    }
  };

  //! Create a Block of Edges for a wire.
  Standard_EXPORT HLRAlgo_EdgesBlock(const int NbEdges);

  int NbEdges() const { return myEdges.Upper(); }

  void Edge(const int I, const int EI) { myEdges(I) = EI; }

  int Edge(const int I) const { return myEdges(I); }

  void Orientation(const int I, const TopAbs_Orientation Or)
  {
    myFlags(I) &= ~EMaskOrient;
    myFlags(I) |= ((int)Or & (int)EMaskOrient);
  }

  TopAbs_Orientation Orientation(const int I) const
  {
    return ((TopAbs_Orientation)(myFlags(I) & EMaskOrient));
  }

  bool OutLine(const int I) const { return (myFlags(I) & EMaskOutLine) != 0; }

  void OutLine(const int I, const bool B)
  {
    if (B)
      myFlags(I) |= EMaskOutLine;
    else
      myFlags(I) &= ~EMaskOutLine;
  }

  bool Internal(const int I) const { return (myFlags(I) & EMaskInternal) != 0; }

  void Internal(const int I, const bool B)
  {
    if (B)
      myFlags(I) |= EMaskInternal;
    else
      myFlags(I) &= ~EMaskInternal;
  }

  bool Double(const int I) const { return (myFlags(I) & EMaskDouble) != 0; }

  void Double(const int I, const bool B)
  {
    if (B)
      myFlags(I) |= EMaskDouble;
    else
      myFlags(I) &= ~EMaskDouble;
  }

  bool IsoLine(const int I) const { return (myFlags(I) & EMaskIsoLine) != 0; }

  void IsoLine(const int I, const bool B)
  {
    if (B)
      myFlags(I) |= EMaskIsoLine;
    else
      myFlags(I) &= ~EMaskIsoLine;
  }

  void UpdateMinMax(const MinMaxIndices& TotMinMax) { myMinMax = TotMinMax; }

  MinMaxIndices& MinMax() { return myMinMax; }

  DEFINE_STANDARD_RTTIEXT(HLRAlgo_EdgesBlock, Standard_Transient)

protected:
  enum EMskFlags
  {
    EMaskOrient   = 15,
    EMaskOutLine  = 16,
    EMaskInternal = 32,
    EMaskDouble   = 64,
    EMaskIsoLine  = 128
  };

private:
  NCollection_Array1<int> myEdges;
  NCollection_Array1<int> myFlags;
  MinMaxIndices           myMinMax;
};

#endif // _HLRAlgo_EdgesBlock_HeaderFile
