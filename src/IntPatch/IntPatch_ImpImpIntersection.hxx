// Created on: 1992-05-07
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

#ifndef _IntPatch_ImpImpIntersection_HeaderFile
#define _IntPatch_ImpImpIntersection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <IntPatch_SequenceOfPoint.hxx>
#include <IntPatch_SequenceOfLine.hxx>
#include <IntPatch_TheSOnBounds.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_DomainError;
class Standard_ConstructionError;
class Adaptor3d_HSurface;
class Adaptor3d_TopolTool;
class IntPatch_Point;
class IntPatch_Line;


//! Implementation of the intersection between two
//! quadric patches : Plane, Cone, Cylinder or Sphere.
class IntPatch_ImpImpIntersection 
{
public:

  DEFINE_STANDARD_ALLOC

  enum IntStatus
  {
    //! OK. Good intersection result.
    IntStatus_OK,

    //! Intersection curve is too long (e.g. as in the bug #26894).
    //! We cannot provide precise computation of value and
    //! derivatives of this curve having used floating-point model
    //! determined by IEEE 754 standard. As result, OCCT algorithms
    //! cannot work with that curve correctly.
    IntStatus_InfiniteSectionCurve,

    //! Algorithm cannot finish correctly.
    IntStatus_Fail   
  };

  Standard_EXPORT IntPatch_ImpImpIntersection();
  
  //! Flag theIsReqToKeepRLine has been entered only for
  //! compatibility with TopOpeBRep package. It shall be deleted
  //! after deleting TopOpeBRep.
  //! When intersection result returns IntPatch_RLine and another
  //! IntPatch_Line (not restriction) we (in case of theIsReqToKeepRLine==TRUE)
  //! will always keep both lines even if they are coincided.
  Standard_EXPORT IntPatch_ImpImpIntersection(const Handle(Adaptor3d_HSurface)& S1, const Handle(Adaptor3d_TopolTool)& D1, const Handle(Adaptor3d_HSurface)& S2, const Handle(Adaptor3d_TopolTool)& D2, const Standard_Real TolArc, const Standard_Real TolTang, const Standard_Boolean theIsReqToKeepRLine = Standard_False);
  
  //! Flag theIsReqToKeepRLine has been entered only for
  //! compatibility with TopOpeBRep package. It shall be deleted
  //! after deleting TopOpeBRep.
  //! When intersection result returns IntPatch_RLine and another
  //! IntPatch_Line (not restriction) we (in case of theIsReqToKeepRLine==TRUE)
  //! will always keep both lines even if they are coincided.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HSurface)& S1,
                                const Handle(Adaptor3d_TopolTool)& D1,
                                const Handle(Adaptor3d_HSurface)& S2,
                                const Handle(Adaptor3d_TopolTool)& D2,
                                const Standard_Real TolArc,
                                const Standard_Real TolTang,
                                const Standard_Boolean theIsReqToKeepRLine =
                                                                  Standard_False);
  
  //! Returns True if the calculus was successful.
  Standard_Boolean IsDone() const;

  //! Returns status
  IntStatus GetStatus() const;
  
  //! Returns true if the is no intersection.
    Standard_Boolean IsEmpty() const;
  
  //! Returns True if the two patches are considered as
  //! entirely tangent, i.e every restriction arc of one
  //! patch is inside the geometric base of the other patch.
    Standard_Boolean TangentFaces() const;
  
  //! Returns True when the TangentFaces returns True and the
  //! normal vectors evaluated at a point on the first and the
  //! second surface are opposite.
  //! The exception DomainError is raised if TangentFaces
  //! returns False.
    Standard_Boolean OppositeFaces() const;
  
  //! Returns the number of "single" points.
    Standard_Integer NbPnts() const;
  
  //! Returns the point of range Index.
  //! An exception is raised if Index<=0 or Index>NbPnt.
    const IntPatch_Point& Point (const Standard_Integer Index) const;
  
  //! Returns the number of intersection lines.
    Standard_Integer NbLines() const;
  
  //! Returns the line of range Index.
  //! An exception is raised if Index<=0 or Index>NbLine.
    const Handle(IntPatch_Line)& Line (const Standard_Integer Index) const;




protected:





private:



  IntStatus myDone;
  Standard_Boolean empt;
  Standard_Boolean tgte;
  Standard_Boolean oppo;
  IntPatch_SequenceOfPoint spnt;
  IntPatch_SequenceOfLine slin;
  IntPatch_TheSOnBounds solrst;


};


#include <IntPatch_ImpImpIntersection.lxx>





#endif // _IntPatch_ImpImpIntersection_HeaderFile
