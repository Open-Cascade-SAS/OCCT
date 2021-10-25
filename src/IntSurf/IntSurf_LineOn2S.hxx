// Created on: 1993-02-22
// Created by: Jacques GOUSSARD
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

#ifndef _IntSurf_LineOn2S_HeaderFile
#define _IntSurf_LineOn2S_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <IntSurf_SequenceOfPntOn2S.hxx>
#include <Standard_Transient.hxx>
#include <IntSurf_Allocator.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class IntSurf_PntOn2S;


class IntSurf_LineOn2S;
DEFINE_STANDARD_HANDLE(IntSurf_LineOn2S, Standard_Transient)


class IntSurf_LineOn2S : public Standard_Transient
{

public:

  
  Standard_EXPORT IntSurf_LineOn2S(const IntSurf_Allocator& theAllocator = 0);
  
  //! Adds a point in the line.
  Standard_EXPORT void Add(const IntSurf_PntOn2S& P);
  
  //! Returns the number of points in the line.
    Standard_Integer NbPoints() const;
  
  //! Returns the point of range Index in the line.
    const IntSurf_PntOn2S& Value (const Standard_Integer Index) const;
  
  //! Reverses the order of points of the line.
    void Reverse();
  
  //! Keeps in <me> the points 1 to Index-1, and returns
  //! the items Index to the end.
  Standard_EXPORT Handle(IntSurf_LineOn2S) Split (const Standard_Integer Index);
  
  //! Replaces the point of range Index in the line.
    void Value (const Standard_Integer Index, const IntSurf_PntOn2S& P);
  
  //! Sets the parametric coordinates on one of the surfaces
  //! of the point of range Index in the line.
  Standard_EXPORT void SetUV(const Standard_Integer Index, const Standard_Boolean OnFirst, const Standard_Real U, const Standard_Real V);
  
    void Clear();
  
  Standard_EXPORT void InsertBefore (const Standard_Integer I, const IntSurf_PntOn2S& P);
  
  Standard_EXPORT void RemovePoint (const Standard_Integer I);

  //! Returns TRUE if theP is out of the box built from
  //! the points on 1st surface
  Standard_EXPORT Standard_Boolean IsOutSurf1Box(const gp_Pnt2d& theP);

  //! Returns TRUE if theP is out of the box built from
  //! the points on 2nd surface
  Standard_EXPORT Standard_Boolean IsOutSurf2Box(const gp_Pnt2d& theP);

  //! Returns TRUE if theP is out of the box built from 3D-points.
  Standard_EXPORT Standard_Boolean IsOutBox(const gp_Pnt& theP);

  DEFINE_STANDARD_RTTIEXT(IntSurf_LineOn2S,Standard_Transient)

protected:




private:


  IntSurf_SequenceOfPntOn2S mySeq;
  Bnd_Box2d myBuv1;
  Bnd_Box2d myBuv2;
  Bnd_Box myBxyz;

};


#include <IntSurf_LineOn2S.lxx>





#endif // _IntSurf_LineOn2S_HeaderFile
