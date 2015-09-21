// Created on: 1993-10-29
// Created by: Jean Marc LACHAUME
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _HatchGen_IntersectionPoint_HeaderFile
#define _HatchGen_IntersectionPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Boolean.hxx>



class HatchGen_IntersectionPoint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Sets the index of the supporting curve.
  Standard_EXPORT void SetIndex (const Standard_Integer Index);
  
  //! Returns the index of the supporting curve.
  Standard_EXPORT Standard_Integer Index() const;
  
  //! Sets the parameter on the curve.
  Standard_EXPORT void SetParameter (const Standard_Real Parameter);
  
  //! Returns the parameter on the curve.
  Standard_EXPORT Standard_Real Parameter() const;
  
  //! Sets the position of the point on the curve.
  Standard_EXPORT void SetPosition (const TopAbs_Orientation Position);
  
  //! Returns the position of the point on the curve.
  Standard_EXPORT TopAbs_Orientation Position() const;
  
  //! Sets the transition state before the intersection.
  Standard_EXPORT void SetStateBefore (const TopAbs_State State);
  
  //! Returns the transition state before the intersection.
  Standard_EXPORT TopAbs_State StateBefore() const;
  
  //! Sets the transition state after the intersection.
  Standard_EXPORT void SetStateAfter (const TopAbs_State State);
  
  //! Returns the transition state after of the intersection.
  Standard_EXPORT TopAbs_State StateAfter() const;
  
  //! Sets the flag that the point is the beginning of a segment.
  Standard_EXPORT void SetSegmentBeginning (const Standard_Boolean State = Standard_True);
  
  //! Returns the flag that the point is the beginning of a segment.
  Standard_EXPORT Standard_Boolean SegmentBeginning() const;
  
  //! Sets the flag that the point is the end of a segment.
  Standard_EXPORT void SetSegmentEnd (const Standard_Boolean State = Standard_True);
  
  //! Returns the flag that the point is the end of a segment.
  Standard_EXPORT Standard_Boolean SegmentEnd() const;
  
  //! Dump of the point on element.
  Standard_EXPORT virtual void Dump (const Standard_Integer Index = 0) const = 0;

protected:
  
  //! Creates an empty intersection point.
  Standard_EXPORT HatchGen_IntersectionPoint();

  //! Destructor is protected for safer inheritance
  ~HatchGen_IntersectionPoint() {}

protected:

  Standard_Integer myIndex;
  Standard_Real myParam;
  TopAbs_Orientation myPosit;
  TopAbs_State myBefore;
  TopAbs_State myAfter;
  Standard_Boolean mySegBeg;
  Standard_Boolean mySegEnd;
};

#endif // _HatchGen_IntersectionPoint_HeaderFile
