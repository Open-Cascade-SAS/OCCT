// Created on: 1993-01-21
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

#ifndef _IntPatch_Intersection_HeaderFile
#define _IntPatch_Intersection_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <IntPatch_SequenceOfPoint.hxx>
#include <IntPatch_SequenceOfLine.hxx>
#include <IntSurf_ListOfPntOn2S.hxx>
#include <GeomAbs_SurfaceType.hxx>

class Adaptor3d_TopolTool;


//! This class provides a generic algorithm to intersect
//! 2 surfaces.
class IntPatch_Intersection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntPatch_Intersection();
  
  Standard_EXPORT IntPatch_Intersection(const Handle(Adaptor3d_Surface)& S1, const Handle(Adaptor3d_TopolTool)& D1, const Handle(Adaptor3d_Surface)& S2, const Handle(Adaptor3d_TopolTool)& D2, const Standard_Real TolArc, const Standard_Real TolTang);
  
  Standard_EXPORT IntPatch_Intersection(const Handle(Adaptor3d_Surface)& S1, const Handle(Adaptor3d_TopolTool)& D1, const Standard_Real TolArc, const Standard_Real TolTang);
  
  //! Set the tolerances used by the algorithms:
  //! --- Implicit   - Parametric
  //! --- Parametric - Parametric
  //! --- Implicit   - Implicit
  //!
  //! TolArc   is  used  to   compute  the intersections
  //! between the restrictions   of  a  surface   and  a
  //! walking line.
  //!
  //! TolTang is used to compute the points on a walking
  //! line, and in geometric algorithms.
  //!
  //! Fleche  is  a  parameter   used in    the  walking
  //! algorithms to provide small curvatures on a line.
  //!
  //! UVMaxStep is  a  parameter   used in  the  walking
  //! algorithms  to    compute the  distance between to
  //! points in their respective parametric spaces.
  Standard_EXPORT void SetTolerances (const Standard_Real TolArc, const Standard_Real TolTang, const Standard_Real UVMaxStep, const Standard_Real Fleche);
  
  //! Flag theIsReqToKeepRLine has been entered only for
  //! compatibility with TopOpeBRep package. It shall be deleted
  //! after deleting TopOpeBRep.
  //! When intersection result returns IntPatch_RLine and another
  //! IntPatch_Line (not restriction) we (in case of theIsReqToKeepRLine==TRUE)
  //! will always keep both lines even if they are coincided.
  //! Flag theIsReqToPostWLProc has been entered only for
  //! compatibility with TopOpeBRep package. It shall be deleted
  //! after deleting TopOpeBRep.
  //! If theIsReqToPostWLProc == FALSE, then we will work with Walking-line
  //! obtained after intersection algorithm directly (without any post-processing).
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& S1, const Handle(Adaptor3d_TopolTool)& D1, const Handle(Adaptor3d_Surface)& S2, const Handle(Adaptor3d_TopolTool)& D2, const Standard_Real TolArc, const Standard_Real TolTang, const Standard_Boolean isGeomInt = Standard_True, const Standard_Boolean theIsReqToKeepRLine = Standard_False, const Standard_Boolean theIsReqToPostWLProc = Standard_True);
  
  //! If isGeomInt == Standard_False, then method
  //! Param-Param intersection will be used.
  //! Flag theIsReqToKeepRLine has been entered only for
  //! compatibility with TopOpeBRep package. It shall be deleted
  //! after deleting TopOpeBRep.
  //! When intersection result returns IntPatch_RLine and another
  //! IntPatch_Line (not restriction) we (in case of theIsReqToKeepRLine==TRUE)
  //! will always keep both lines even if they are coincided.
  //! Flag theIsReqToPostWLProc has been entered only for
  //! compatibility with TopOpeBRep package. It shall be deleted
  //! after deleting TopOpeBRep.
  //! If theIsReqToPostWLProc == FALSE, then we will work with Walking-line
  //! obtained after intersection algorithm directly (without any post-processing). 
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& S1, const Handle(Adaptor3d_TopolTool)& D1, const Handle(Adaptor3d_Surface)& S2, const Handle(Adaptor3d_TopolTool)& D2, const Standard_Real TolArc, const Standard_Real TolTang, IntSurf_ListOfPntOn2S& LOfPnts, const Standard_Boolean isGeomInt = Standard_True, const Standard_Boolean theIsReqToKeepRLine = Standard_False, const Standard_Boolean theIsReqToPostWLProc = Standard_True);
  
  //! Perform with start point
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& S1, const Handle(Adaptor3d_TopolTool)& D1, const Handle(Adaptor3d_Surface)& S2, const Handle(Adaptor3d_TopolTool)& D2, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2, const Standard_Real TolArc, const Standard_Real TolTang);
  
  //! Uses for finding self-intersected surfaces.
  Standard_EXPORT void Perform (const Handle(Adaptor3d_Surface)& S1, const Handle(Adaptor3d_TopolTool)& D1, const Standard_Real TolArc, const Standard_Real TolTang);
  
  //! Returns True if the calculus was successful.
    Standard_Boolean IsDone() const;
  
  //! Returns true if the is no intersection.
    Standard_Boolean IsEmpty() const;
  
  //! Returns True if the two patches are considered as
  //! entirely tangent, i-e every restriction arc of one
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
  
  Standard_EXPORT const IntPatch_SequenceOfLine& SequenceOfLine() const;
  
  //! Dump of each result line.
  //! Mode for more accurate dumps.
  Standard_EXPORT void Dump (const Standard_Integer Mode, const Handle(Adaptor3d_Surface)& S1, const Handle(Adaptor3d_TopolTool)& D1, const Handle(Adaptor3d_Surface)& S2, const Handle(Adaptor3d_TopolTool)& D2) const;




protected:





private:

  
  Standard_EXPORT void ParamParamPerfom (const Handle(Adaptor3d_Surface)& S1, const Handle(Adaptor3d_TopolTool)& D1, const Handle(Adaptor3d_Surface)& S2, const Handle(Adaptor3d_TopolTool)& D2, const Standard_Real TolArc, const Standard_Real TolTang, IntSurf_ListOfPntOn2S& LOfPnts, const GeomAbs_SurfaceType typs1, const GeomAbs_SurfaceType typs2);
  
  //! Flag theIsReqToKeepRLine has been entered only for
  //! compatibility with TopOpeBRep package. It shall be deleted
  //! after deleting TopOpeBRep.
  //! When intersection result returns IntPatch_RLine and another
  //! IntPatch_Line (not restriction) we (in case of theIsReqToKeepRLine==TRUE)
  //! will always keep both lines even if they are coincided.
  Standard_EXPORT void GeomGeomPerfom (const Handle(Adaptor3d_Surface)& S1, const Handle(Adaptor3d_TopolTool)& D1, const Handle(Adaptor3d_Surface)& S2, const Handle(Adaptor3d_TopolTool)& D2, const Standard_Real TolArc, const Standard_Real TolTang, IntSurf_ListOfPntOn2S& LOfPnts, const GeomAbs_SurfaceType typs1, const GeomAbs_SurfaceType typs2, const Standard_Boolean theIsReqToKeepRLine);
  
  Standard_EXPORT void GeomParamPerfom (const Handle(Adaptor3d_Surface)& S1, const Handle(Adaptor3d_TopolTool)& D1, const Handle(Adaptor3d_Surface)& S2, const Handle(Adaptor3d_TopolTool)& D2, const Standard_Boolean isNotAnalitical, const GeomAbs_SurfaceType typs1, const GeomAbs_SurfaceType typs2);


  Standard_Boolean done;
  Standard_Boolean empt;
  Standard_Boolean tgte;
  Standard_Boolean oppo;
  IntPatch_SequenceOfPoint spnt;
  IntPatch_SequenceOfLine slin;
  Standard_Real myTolArc;
  Standard_Real myTolTang;
  Standard_Real myUVMaxStep;
  Standard_Real myFleche;
  Standard_Boolean myIsStartPnt;
  Standard_Real myU1Start;
  Standard_Real myV1Start;
  Standard_Real myU2Start;
  Standard_Real myV2Start;


};


#include <IntPatch_Intersection.lxx>





#endif // _IntPatch_Intersection_HeaderFile
