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


#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntSurf_LineOn2S,Standard_Transient)

IntSurf_LineOn2S::IntSurf_LineOn2S (const IntSurf_Allocator& theAllocator) :
    mySeq (theAllocator)
{}


Handle(IntSurf_LineOn2S) IntSurf_LineOn2S::Split (const Standard_Integer Index)
{
  IntSurf_SequenceOfPntOn2S SS;
  mySeq.Split(Index,SS);
  Handle(IntSurf_LineOn2S) NS = new IntSurf_LineOn2S ();
  Standard_Integer i;
  Standard_Integer leng = SS.Length();
  for (i=1; i<=leng; i++) {
    NS->Add(SS(i));
  }
  return NS;
}


void IntSurf_LineOn2S::InsertBefore(const Standard_Integer index, const IntSurf_PntOn2S& P) { 
  if(index>mySeq.Length()) { 
    mySeq.Append(P);
  }
  else { 
    mySeq.InsertBefore(index,P);
  }
}

void IntSurf_LineOn2S::RemovePoint(const Standard_Integer index) { 
  mySeq.Remove(index);
}
