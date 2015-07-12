// Created on: 1998-01-27
// Created by: Laurent BUCHARD
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _IntCurvesFace_ShapeIntersector_HeaderFile
#define _IntCurvesFace_ShapeIntersector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Address.hxx>
#include <BRepTopAdaptor_SeqOfPtr.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Standard_Real.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>
#include <TopAbs_State.hxx>
class TopoDS_Shape;
class gp_Lin;
class Adaptor3d_HCurve;
class gp_Pnt;
class TopoDS_Face;



class IntCurvesFace_ShapeIntersector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntCurvesFace_ShapeIntersector();
  
  Standard_EXPORT void Load (const TopoDS_Shape& Sh, const Standard_Real Tol);
  
  //! Perform the intersection between the
  //! segment L and the loaded shape.
  //!
  //! PInf is the smallest parameter on the line
  //! PSup is the highest  parammter on the line
  //!
  //! For an infinite line PInf and PSup can be
  //! +/- RealLast.
  Standard_EXPORT void Perform (const gp_Lin& L, const Standard_Real PInf, const Standard_Real PSup);
  
  //! Perform the intersection between the
  //! segment L and the loaded shape.
  //!
  //! PInf is the smallest parameter on the line
  //! PSup is the highest  parammter on the line
  //!
  //! For an infinite line PInf and PSup can be
  //! +/- RealLast.
  Standard_EXPORT void PerformNearest (const gp_Lin& L, const Standard_Real PInf, const Standard_Real PSup);
  
  //! same method for a HCurve from Adaptor3d.
  //! PInf an PSup can also be - and + INF.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HCurve)& HCu, const Standard_Real PInf, const Standard_Real PSup);
  
  //! True is returned when the intersection have been computed.
    Standard_Boolean IsDone() const;
  
    Standard_Integer NbPnt() const;
  
  //! Returns the U parameter of the ith intersection point
  //! on the surface.
    Standard_Real UParameter (const Standard_Integer I) const;
  
  //! Returns the V parameter of the ith intersection point
  //! on the surface.
    Standard_Real VParameter (const Standard_Integer I) const;
  
  //! Returns the parameter of the ith intersection point
  //! on the line.
    Standard_Real WParameter (const Standard_Integer I) const;
  
  //! Returns the geometric point of the ith intersection
  //! between the line and the surface.
    const gp_Pnt& Pnt (const Standard_Integer I) const;
  
  //! Returns the ith transition of the line on the surface.
    IntCurveSurface_TransitionOnCurve Transition (const Standard_Integer I) const;
  
  //! Returns the ith state of the point on the face.
  //! The values can be either TopAbs_IN
  //! ( the point is in the face)
  //! or TopAbs_ON
  //! ( the point is on a boudary of the face).
    TopAbs_State State (const Standard_Integer I) const;
  
  //! Returns the significant face used to determine
  //! the intersection.
    const TopoDS_Face& Face (const Standard_Integer I) const;
  
  //! Internal method. Sort the result on the Curve
  //! parameter.
  Standard_EXPORT void SortResult();
  
  Standard_EXPORT void Destroy();
~IntCurvesFace_ShapeIntersector()
{
  Destroy();
}




protected:





private:



  Standard_Boolean done;
  Standard_Integer nbfaces;
  Standard_Address PtrJetons;
  Standard_Address PtrJetonsIndex;
  BRepTopAdaptor_SeqOfPtr PtrIntersector;
  TColStd_SequenceOfInteger IndexPt;
  TColStd_SequenceOfInteger IndexFace;
  TColStd_SequenceOfInteger IndexIntPnt;
  TColStd_SequenceOfReal IndexPar;


};


#include <IntCurvesFace_ShapeIntersector.lxx>





#endif // _IntCurvesFace_ShapeIntersector_HeaderFile
