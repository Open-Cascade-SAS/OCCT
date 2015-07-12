// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
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

#ifndef _IntPatch_TheSegmentOfTheSOnBounds_HeaderFile
#define _IntPatch_TheSegmentOfTheSOnBounds_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <IntPatch_ThePathPointOfTheSOnBounds.hxx>
class Adaptor2d_HCurve2d;
class Standard_DomainError;
class Adaptor3d_HVertex;
class IntPatch_ThePathPointOfTheSOnBounds;



class IntPatch_TheSegmentOfTheSOnBounds 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntPatch_TheSegmentOfTheSOnBounds();
  
    void SetValue (const Handle(Adaptor2d_HCurve2d)& A);
  
  Standard_EXPORT void SetLimitPoint (const IntPatch_ThePathPointOfTheSOnBounds& V, const Standard_Boolean First);
  
    const Handle(Adaptor2d_HCurve2d)& Curve() const;
  
    Standard_Boolean HasFirstPoint() const;
  
    const IntPatch_ThePathPointOfTheSOnBounds& FirstPoint() const;
  
    Standard_Boolean HasLastPoint() const;
  
    const IntPatch_ThePathPointOfTheSOnBounds& LastPoint() const;




protected:





private:



  Handle(Adaptor2d_HCurve2d) arc;
  Standard_Boolean hasfp;
  IntPatch_ThePathPointOfTheSOnBounds thefp;
  Standard_Boolean haslp;
  IntPatch_ThePathPointOfTheSOnBounds thelp;


};

#define TheVertex Handle(Adaptor3d_HVertex)
#define TheVertex_hxx <Adaptor3d_HVertex.hxx>
#define TheArc Handle(Adaptor2d_HCurve2d)
#define TheArc_hxx <Adaptor2d_HCurve2d.hxx>
#define ThePathPoint IntPatch_ThePathPointOfTheSOnBounds
#define ThePathPoint_hxx <IntPatch_ThePathPointOfTheSOnBounds.hxx>
#define IntStart_Segment IntPatch_TheSegmentOfTheSOnBounds
#define IntStart_Segment_hxx <IntPatch_TheSegmentOfTheSOnBounds.hxx>

#include <IntStart_Segment.lxx>

#undef TheVertex
#undef TheVertex_hxx
#undef TheArc
#undef TheArc_hxx
#undef ThePathPoint
#undef ThePathPoint_hxx
#undef IntStart_Segment
#undef IntStart_Segment_hxx




#endif // _IntPatch_TheSegmentOfTheSOnBounds_HeaderFile
