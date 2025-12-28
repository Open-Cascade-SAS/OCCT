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
#include <IntPatch_Point.hxx>
#include <NCollection_Sequence.hxx>
#include <IntPatch_Line.hxx>
#include <NCollection_Sequence.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <NCollection_List.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <NCollection_Vector.hxx>

class Adaptor3d_TopolTool;

//! This class provides a generic algorithm to intersect
//! 2 surfaces.
class IntPatch_Intersection
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntPatch_Intersection();

  Standard_EXPORT IntPatch_Intersection(const occ::handle<Adaptor3d_Surface>&   S1,
                                        const occ::handle<Adaptor3d_TopolTool>& D1,
                                        const occ::handle<Adaptor3d_Surface>&   S2,
                                        const occ::handle<Adaptor3d_TopolTool>& D2,
                                        const double                TolArc,
                                        const double                TolTang);

  Standard_EXPORT IntPatch_Intersection(const occ::handle<Adaptor3d_Surface>&   S1,
                                        const occ::handle<Adaptor3d_TopolTool>& D1,
                                        const double                TolArc,
                                        const double                TolTang);

  //! Set the tolerances used by the algorithms:
  //! --- Implicit   - Parametric
  //! --- Parametric - Parametric
  //! --- Implicit   - Implicit
  //!
  //! TolArc is used to compute the intersections
  //! between the restrictions of a surface and a
  //! walking line.
  //!
  //! TolTang is used to compute the points on a walking
  //! line, and in geometric algorithms.
  //!
  //! Fleche is a parameter used in the walking
  //! algorithms to provide small curvatures on a line.
  //!
  //! UVMaxStep is a parameter used in the walking
  //! algorithms to compute the distance between to
  //! points in their respective parametric spaces.
  Standard_EXPORT void SetTolerances(const double TolArc,
                                     const double TolTang,
                                     const double UVMaxStep,
                                     const double Fleche);

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
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   S1,
                               const occ::handle<Adaptor3d_TopolTool>& D1,
                               const occ::handle<Adaptor3d_Surface>&   S2,
                               const occ::handle<Adaptor3d_TopolTool>& D2,
                               const double                TolArc,
                               const double                TolTang,
                               const bool             isGeomInt = true,
                               const bool theIsReqToKeepRLine   = false,
                               const bool theIsReqToPostWLProc  = true);

  //! If isGeomInt == false, then method
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
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   S1,
                               const occ::handle<Adaptor3d_TopolTool>& D1,
                               const occ::handle<Adaptor3d_Surface>&   S2,
                               const occ::handle<Adaptor3d_TopolTool>& D2,
                               const double                TolArc,
                               const double                TolTang,
                               NCollection_List<IntSurf_PntOn2S>&             LOfPnts,
                               const bool             isGeomInt = true,
                               const bool theIsReqToKeepRLine   = false,
                               const bool theIsReqToPostWLProc  = true);

  //! Perform with start point
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   S1,
                               const occ::handle<Adaptor3d_TopolTool>& D1,
                               const occ::handle<Adaptor3d_Surface>&   S2,
                               const occ::handle<Adaptor3d_TopolTool>& D2,
                               const double                U1,
                               const double                V1,
                               const double                U2,
                               const double                V2,
                               const double                TolArc,
                               const double                TolTang);

  //! Uses for finding self-intersected surfaces.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   S1,
                               const occ::handle<Adaptor3d_TopolTool>& D1,
                               const double                TolArc,
                               const double                TolTang);

  //! Returns True if the calculus was successful.
  bool IsDone() const;

  //! Returns true if the is no intersection.
  bool IsEmpty() const;

  //! Returns True if the two patches are considered as
  //! entirely tangent, i-e every restriction arc of one
  //! patch is inside the geometric base of the other patch.
  bool TangentFaces() const;

  //! Returns True when the TangentFaces returns True and the
  //! normal vectors evaluated at a point on the first and the
  //! second surface are opposite.
  //! The exception DomainError is raised if TangentFaces
  //! returns False.
  bool OppositeFaces() const;

  //! Returns the number of "single" points.
  int NbPnts() const;

  //! Returns the point of range Index.
  //! An exception is raised if Index<=0 or Index>NbPnt.
  const IntPatch_Point& Point(const int Index) const;

  //! Returns the number of intersection lines.
  int NbLines() const;

  //! Returns the line of range Index.
  //! An exception is raised if Index<=0 or Index>NbLine.
  const occ::handle<IntPatch_Line>& Line(const int Index) const;

  Standard_EXPORT const NCollection_Sequence<occ::handle<IntPatch_Line>>& SequenceOfLine() const;

  //! Dump of each result line.
  //! Mode for more accurate dumps.
  Standard_EXPORT void Dump(const int             Mode,
                            const occ::handle<Adaptor3d_Surface>&   S1,
                            const occ::handle<Adaptor3d_TopolTool>& D1,
                            const occ::handle<Adaptor3d_Surface>&   S2,
                            const occ::handle<Adaptor3d_TopolTool>& D2) const;

  //! Checks if surface theS1 has degenerated boundary (dS/du or dS/dv = 0) and
  //! calculates minimal distance between corresponding singular points and surface theS2
  //! If singular point exists the method returns "true" and stores minimal distance in theDist.
  Standard_EXPORT static bool CheckSingularPoints(
    const occ::handle<Adaptor3d_Surface>&   theS1,
    const occ::handle<Adaptor3d_TopolTool>& theD1,
    const occ::handle<Adaptor3d_Surface>&   theS2,
    double&                     theDist);

  //! Calculates recommended value for myUVMaxStep depending on surfaces and their domains
  Standard_EXPORT static double DefineUVMaxStep(const occ::handle<Adaptor3d_Surface>&   theS1,
                                                       const occ::handle<Adaptor3d_TopolTool>& theD1,
                                                       const occ::handle<Adaptor3d_Surface>&   theS2,
                                                       const occ::handle<Adaptor3d_TopolTool>& theD2);

  //! Prepares surfaces for intersection
  Standard_EXPORT static void PrepareSurfaces(
    const occ::handle<Adaptor3d_Surface>&               theS1,
    const occ::handle<Adaptor3d_TopolTool>&             theD1,
    const occ::handle<Adaptor3d_Surface>&               theS2,
    const occ::handle<Adaptor3d_TopolTool>&             theD2,
    const double                            Tol,
    NCollection_Vector<occ::handle<Adaptor3d_Surface>>& theSeqHS1,
    NCollection_Vector<occ::handle<Adaptor3d_Surface>>& theSeqHS2);

private:
  Standard_EXPORT void ParamParamPerfom(const occ::handle<Adaptor3d_Surface>&   S1,
                                        const occ::handle<Adaptor3d_TopolTool>& D1,
                                        const occ::handle<Adaptor3d_Surface>&   S2,
                                        const occ::handle<Adaptor3d_TopolTool>& D2,
                                        const double                TolArc,
                                        const double                TolTang,
                                        NCollection_List<IntSurf_PntOn2S>&             LOfPnts,
                                        const GeomAbs_SurfaceType          typs1,
                                        const GeomAbs_SurfaceType          typs2);

  //! Flag theIsReqToKeepRLine has been entered only for
  //! compatibility with TopOpeBRep package. It shall be deleted
  //! after deleting TopOpeBRep.
  //! When intersection result returns IntPatch_RLine and another
  //! IntPatch_Line (not restriction) we (in case of theIsReqToKeepRLine==TRUE)
  //! will always keep both lines even if they are coincided.
  Standard_EXPORT void GeomGeomPerfom(const occ::handle<Adaptor3d_Surface>&   S1,
                                      const occ::handle<Adaptor3d_TopolTool>& D1,
                                      const occ::handle<Adaptor3d_Surface>&   S2,
                                      const occ::handle<Adaptor3d_TopolTool>& D2,
                                      const double                TolArc,
                                      const double                TolTang,
                                      NCollection_List<IntSurf_PntOn2S>&             LOfPnts,
                                      const GeomAbs_SurfaceType          typs1,
                                      const GeomAbs_SurfaceType          typs2,
                                      const bool             theIsReqToKeepRLine);

  Standard_EXPORT void GeomParamPerfom(const occ::handle<Adaptor3d_Surface>&   S1,
                                       const occ::handle<Adaptor3d_TopolTool>& D1,
                                       const occ::handle<Adaptor3d_Surface>&   S2,
                                       const occ::handle<Adaptor3d_TopolTool>& D2,
                                       const bool             isNotAnalitical,
                                       const GeomAbs_SurfaceType          typs1,
                                       const GeomAbs_SurfaceType          typs2);

  bool         done;
  bool         empt;
  bool         tgte;
  bool         oppo;
  NCollection_Sequence<IntPatch_Point> spnt;
  NCollection_Sequence<occ::handle<IntPatch_Line>>  slin;
  double            myTolArc;
  double            myTolTang;
  double            myUVMaxStep;
  double            myFleche;
  bool         myIsStartPnt;
  double            myU1Start;
  double            myV1Start;
  double            myU2Start;
  double            myV2Start;
};

#include <IntPatch_Intersection.lxx>

#endif // _IntPatch_Intersection_HeaderFile
