// Created on: 1995-06-22
// Created by: Christophe MARION
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _HLRAlgo_BiPoint_HeaderFile
#define _HLRAlgo_BiPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Address.hxx>



class HLRAlgo_BiPoint 
{
public:

  DEFINE_STANDARD_ALLOC

  HLRAlgo_BiPoint() {}

  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Boolean reg1, const Standard_Boolean regn, const Standard_Boolean outl, const Standard_Boolean intl);
  
  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Integer flag);
  
  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Integer i1, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Boolean reg1, const Standard_Boolean regn, const Standard_Boolean outl, const Standard_Boolean intl);
  
  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Integer i1, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Integer flag);
  
  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Integer i1, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Integer i2, const Standard_Integer i2p1, const Standard_Integer i2p2, const Standard_Boolean reg1, const Standard_Boolean regn, const Standard_Boolean outl, const Standard_Boolean intl);
  
  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Integer i1, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Integer i2, const Standard_Integer i2p1, const Standard_Integer i2p2, const Standard_Integer flag);

  Standard_Boolean Rg1Line() const { return (myIndices[9] & EMskRg1Line) != 0; }

  void Rg1Line (const Standard_Boolean B)
  {
    if (B) myIndices[9] |=  EMskRg1Line;
    else   myIndices[9] &= ~EMskRg1Line;
  }

  Standard_Boolean RgNLine() const { return (myIndices[9] & EMskRgNLine) != 0; }

  void RgNLine (const Standard_Boolean B)
  {
    if (B) myIndices[9] |=  EMskRgNLine;
    else   myIndices[9] &= ~EMskRgNLine;
  }

  Standard_Boolean OutLine() const { return (myIndices[9] & EMskOutLine) != 0; }

  void OutLine (const Standard_Boolean B)
  {
    if (B) myIndices[9] |=  EMskOutLine;
    else   myIndices[9] &= ~EMskOutLine;
  }

  Standard_Boolean IntLine() const { return (myIndices[9] & EMskIntLine) != 0; }

  void IntLine (const Standard_Boolean B)
  {
    if (B) myIndices[9] |=  EMskIntLine;
    else   myIndices[9] &= ~EMskIntLine;
  }

  Standard_Boolean Hidden() const { return (myIndices[9] & EMskHidden) != 0; }

  void Hidden (const Standard_Boolean B)
  {
    if (B) myIndices[9] |=  EMskHidden;
    else   myIndices[9] &= ~EMskHidden;
  }

  Standard_Address Indices()     const { return const_cast<Standard_Integer* >(myIndices); }

  Standard_Address Coordinates() const { return const_cast<Standard_Real* >(myCoordinates); }

protected:

  enum EMskFlags
  {
    EMskRg1Line = 1,
    EMskRgNLine = 2,
    EMskOutLine = 4,
    EMskIntLine = 8,
    EMskHidden  = 16
  };

private:

  Standard_Integer myIndices[10];
  Standard_Real myCoordinates[12];

};

#endif // _HLRAlgo_BiPoint_HeaderFile
