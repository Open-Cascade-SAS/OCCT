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

#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <Standard_Integer.hxx>
#include <MMgt_TShared.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Address.hxx>


class HLRAlgo_EdgesBlock;
DEFINE_STANDARD_HANDLE(HLRAlgo_EdgesBlock, MMgt_TShared)

//! An EdgesBlock is a set of Edges. It is used by the
//! DataStructure to structure the Edges.
//!
//! An EdgesBlock contains :
//!
//! * An Array  of index of Edges.
//!
//! * An Array of flagsf ( Orientation
//! OutLine
//! Internal
//! Double
//! IsoLine)
class HLRAlgo_EdgesBlock : public MMgt_TShared
{

public:

  
  //! Create a Block of Edges for a wire.
  Standard_EXPORT HLRAlgo_EdgesBlock(const Standard_Integer NbEdges);
  
    Standard_Integer NbEdges() const;
  
    void Edge (const Standard_Integer I, const Standard_Integer EI);
  
    Standard_Integer Edge (const Standard_Integer I) const;
  
    void Orientation (const Standard_Integer I, const TopAbs_Orientation Or);
  
    TopAbs_Orientation Orientation (const Standard_Integer I) const;
  
    Standard_Boolean OutLine (const Standard_Integer I) const;
  
    void OutLine (const Standard_Integer I, const Standard_Boolean B);
  
    Standard_Boolean Internal (const Standard_Integer I) const;
  
    void Internal (const Standard_Integer I, const Standard_Boolean B);
  
    Standard_Boolean Double (const Standard_Integer I) const;
  
    void Double (const Standard_Integer I, const Standard_Boolean B);
  
    Standard_Boolean IsoLine (const Standard_Integer I) const;
  
    void IsoLine (const Standard_Integer I, const Standard_Boolean B);
  
  Standard_EXPORT void UpdateMinMax (const Standard_Address TotMinMax);
  
    Standard_Address MinMax() const;




  DEFINE_STANDARD_RTTI(HLRAlgo_EdgesBlock,MMgt_TShared)

protected:




private:


  TColStd_Array1OfInteger myEdges;
  TColStd_Array1OfBoolean myFlags;
  Standard_Integer myMinMax[16];


};


#include <HLRAlgo_EdgesBlock.lxx>





#endif // _HLRAlgo_EdgesBlock_HeaderFile
