// Created on: 1996-05-31
// Created by: Laurent BUCHARD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _IntCurvesFace_Intersector_HeaderFile
#define _IntCurvesFace_Intersector_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <IntCurveSurface_SequenceOfPnt.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TopoDS_Face.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>
#include <TopAbs_State.hxx>

class BRepTopAdaptor_TopolTool;
class gp_Lin;
class gp_Pnt;
class IntCurveSurface_HInter;
class gp_Pnt2d;
class Bnd_Box;

class IntCurvesFace_Intersector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Load a Face.
  //!
  //! The Tolerance <Tol> is used to determine if the
  //! first point of the segment is near the face. In
  //! that case, the parameter of the intersection point
  //! on the line can be a negative value (greater than -Tol).
  //! If aRestr = true UV bounding box of face is used to restrict 
  //! it's underlined surface,
  //! otherwise surface is not restricted.
  //! If UseBToler = false then the 2d-point of intersection is classified with null-tolerance
  //! (relative to face);
  //! otherwise it's using maximum between input tolerance(aTol) and tolerances of face bounds (edges).
  Standard_EXPORT IntCurvesFace_Intersector(const TopoDS_Face& F, const Standard_Real aTol, 
                                            const Standard_Boolean aRestr = Standard_True,
                                            const Standard_Boolean UseBToler = Standard_True);
  
  //! Perform the intersection between the
  //! segment L and the loaded face.
  //!
  //! PInf is the smallest parameter on the line
  //! PSup is the highest  parameter on the line
  //!
  //! For an infinite line PInf and PSup can be
  //! +/- RealLast.
  Standard_EXPORT void Perform (const gp_Lin& L, const Standard_Real PInf, const Standard_Real PSup);
  
  //! same method for a HCurve from Adaptor3d.
  //! PInf an PSup can also be - and + INF.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Curve)& HCu, const Standard_Real PInf, const Standard_Real PSup);
  
  //! Return the surface type
  Standard_EXPORT GeomAbs_SurfaceType SurfaceType() const;
  
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
  //! ( the point is on a boundary of the face).
    TopAbs_State State (const Standard_Integer I) const;

  //! Returns true if curve is parallel or belongs face surface
  //! This case is recognized only for some pairs 
  //! of analytical curves and surfaces (plane - line, ...)
    Standard_Boolean IsParallel() const;

  //! Returns the significant face used to determine
  //! the intersection.
    const TopoDS_Face& Face() const;
  
  Standard_EXPORT TopAbs_State ClassifyUVPoint (const gp_Pnt2d& Puv) const;
  
  Standard_EXPORT Bnd_Box Bounding() const;

  //! Sets the boundary tolerance flag
  Standard_EXPORT void SetUseBoundToler(Standard_Boolean UseBToler );

  //! Returns the boundary tolerance flag
  Standard_EXPORT Standard_Boolean GetUseBoundToler() const;
  
  Standard_EXPORT void Destroy();
~IntCurvesFace_Intersector()
{
  Destroy();
}




protected:





private:

  
  Standard_EXPORT void InternalCall (const IntCurveSurface_HInter& HICS, const Standard_Real pinf, const Standard_Real psup);


  Handle(BRepTopAdaptor_TopolTool) myTopolTool;
  Handle(BRepAdaptor_Surface) Hsurface;
  Standard_Real Tol;
  IntCurveSurface_SequenceOfPnt SeqPnt;
  TColStd_SequenceOfInteger mySeqState;
  Standard_Boolean done;
  Standard_Boolean myReady;
  Standard_Integer nbpnt;
  TopoDS_Face face;
  Standard_Address PtrOnPolyhedron;
  Standard_Address PtrOnBndBounding;
  Standard_Boolean myUseBoundTol;
  Standard_Boolean myIsParallel; //Curve is "parallel" face surface
                                 //This case is recognized only for some pairs 
                                 //of analytical curves and surfaces (plane - line, ...)


};


#include <IntCurvesFace_Intersector.lxx>





#endif // _IntCurvesFace_Intersector_HeaderFile
