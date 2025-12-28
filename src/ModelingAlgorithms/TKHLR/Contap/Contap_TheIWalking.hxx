// Created on: 1993-02-05
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

#ifndef _Contap_TheIWalking_HeaderFile
#define _Contap_TheIWalking_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <IntSurf_PathPoint.hxx>
#include <NCollection_Sequence.hxx>
#include <math_Vector.hxx>
#include <IntWalk_VectorOfWalkingData.hxx>
#include <IntWalk_VectorOfInteger.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir2d.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <Contap_TheIWLineOfTheIWalking.hxx>
#include <IntSurf_InteriorPoint.hxx>
#include <IntWalk_StatusDeflection.hxx>
#include <Bnd_Range.hxx>

class IntSurf_PathPoint;
class IntSurf_PathPointTool;
class IntSurf_InteriorPoint;
class IntSurf_InteriorPointTool;
class Adaptor3d_HSurfaceTool;
class Contap_SurfFunction;
class Contap_TheIWLineOfTheIWalking;
class math_FunctionSetRoot;

class Contap_TheIWalking
{
public:
  DEFINE_STANDARD_ALLOC

  //! Deflection is the maximum deflection admitted between two
  //! consecutive points on a resulting polyline.
  //! Step is the maximum increment admitted between two
  //! consecutive points (in 2d space).
  //! Epsilon is the tolerance beyond which 2 points
  //! are confused.
  //! theToFillHoles is the flag defining whether possible holes
  //! between resulting curves are filled or not
  //! in case of Contap walking theToFillHoles is True
  Standard_EXPORT Contap_TheIWalking(const double Epsilon,
                                     const double Deflection,
                                     const double Step,
                                     const bool   theToFillHoles = false);

  //! Deflection is the maximum deflection admitted between two
  //! consecutive points on a resulting polyline.
  //! Step is the maximum increment admitted between two
  //! consecutive points (in 2d space).
  //! Epsilon is the tolerance beyond which 2 points
  //! are confused
  void SetTolerance(const double Epsilon, const double Deflection, const double Step);

  //! Searches a set of polylines starting on a point of Pnts1
  //! or Pnts2.
  //! Each point on a resulting polyline verifies F(u,v)=0
  Standard_EXPORT void Perform(const NCollection_Sequence<IntSurf_PathPoint>&     Pnts1,
                               const NCollection_Sequence<IntSurf_InteriorPoint>& Pnts2,
                               Contap_SurfFunction&                               Func,
                               const occ::handle<Adaptor3d_Surface>&              S,
                               const bool                                         Reversed = false);

  //! Searches a set of polylines starting on a point of Pnts1.
  //! Each point on a resulting polyline verifies F(u,v)=0
  Standard_EXPORT void Perform(const NCollection_Sequence<IntSurf_PathPoint>& Pnts1,
                               Contap_SurfFunction&                           Func,
                               const occ::handle<Adaptor3d_Surface>&          S,
                               const bool                                     Reversed = false);

  //! Returns true if the calculus was successful.
  bool IsDone() const;

  //! Returns the number of resulting polylines.
  //! An exception is raised if IsDone returns False.
  int NbLines() const;

  //! Returns the polyline of range Index.
  //! An exception is raised if IsDone is False.
  //! An exception is raised if Index<=0 or Index>NbLines.
  const occ::handle<Contap_TheIWLineOfTheIWalking>& Value(const int Index) const;

  //! Returns the number of points belonging to Pnts on which no
  //! line starts or ends.
  //! An exception is raised if IsDone returns False.
  int NbSinglePnts() const;

  //! Returns the point of range Index .
  //! An exception is raised if IsDone returns False.
  //! An exception is raised if Index<=0 or
  //! Index > NbSinglePnts.
  const IntSurf_PathPoint& SinglePnt(const int Index) const;

protected:
  Standard_EXPORT bool Cadrage(math_Vector& BornInf,
                               math_Vector& BornSup,
                               math_Vector& UVap,
                               double&      Step,
                               const int    StepSign) const;

  Standard_EXPORT bool TestArretPassage(const NCollection_Sequence<double>& Umult,
                                        const NCollection_Sequence<double>& Vmult,
                                        Contap_SurfFunction&                Section,
                                        math_Vector&                        UV,
                                        int&                                Irang);

  Standard_EXPORT bool TestArretPassage(const NCollection_Sequence<double>& Umult,
                                        const NCollection_Sequence<double>& Vmult,
                                        const math_Vector&                  UV,
                                        const int                           Index,
                                        int&                                Irang);

  Standard_EXPORT bool TestArretAjout(Contap_SurfFunction& Section,
                                      math_Vector&         UV,
                                      int&                 Irang,
                                      IntSurf_PntOn2S&     PSol);

  Standard_EXPORT void FillPntsInHoles(Contap_SurfFunction&                         Section,
                                       NCollection_Sequence<int>&                   CopySeqAlone,
                                       NCollection_Sequence<IntSurf_InteriorPoint>& PntsInHoles);

  Standard_EXPORT void TestArretCadre(const NCollection_Sequence<double>&               Umult,
                                      const NCollection_Sequence<double>&               Vmult,
                                      const occ::handle<Contap_TheIWLineOfTheIWalking>& Line,
                                      Contap_SurfFunction&                              Section,
                                      math_Vector&                                      UV,
                                      int&                                              Irang);

  Standard_EXPORT IntWalk_StatusDeflection
    TestDeflection(Contap_SurfFunction&           Section,
                   const bool                     Finished,
                   const math_Vector&             UV,
                   const IntWalk_StatusDeflection StatusPrecedent,
                   int&                           NbDivision,
                   double&                        Step,
                   const int                      StepSign);

  Standard_EXPORT void ComputeOpenLine(const NCollection_Sequence<double>&            Umult,
                                       const NCollection_Sequence<double>&            Vmult,
                                       const NCollection_Sequence<IntSurf_PathPoint>& Pnts1,
                                       Contap_SurfFunction&                           Section,
                                       bool&                                          Rajout);

  Standard_EXPORT void OpenLine(const int                                         N,
                                const IntSurf_PntOn2S&                            Psol,
                                const NCollection_Sequence<IntSurf_PathPoint>&    Pnts1,
                                Contap_SurfFunction&                              Section,
                                const occ::handle<Contap_TheIWLineOfTheIWalking>& Line);

  Standard_EXPORT bool IsValidEndPoint(const int IndOfPoint, const int IndOfLine);

  Standard_EXPORT void RemoveTwoEndPoints(const int IndOfPoint);

  Standard_EXPORT bool IsPointOnLine(const gp_Pnt2d& theP2d, const int Irang);

  Standard_EXPORT void ComputeCloseLine(const NCollection_Sequence<double>&                Umult,
                                        const NCollection_Sequence<double>&                Vmult,
                                        const NCollection_Sequence<IntSurf_PathPoint>&     Pnts1,
                                        const NCollection_Sequence<IntSurf_InteriorPoint>& Pnts2,
                                        Contap_SurfFunction&                               Section,
                                        bool&                                              Rajout);

  Standard_EXPORT void AddPointInCurrentLine(
    const int                                         N,
    const IntSurf_PathPoint&                          PathPnt,
    const occ::handle<Contap_TheIWLineOfTheIWalking>& CurrentLine) const;

  Standard_EXPORT void MakeWalkingPoint(const int            Case,
                                        const double         U,
                                        const double         V,
                                        Contap_SurfFunction& Section,
                                        IntSurf_PntOn2S&     Psol);

  //! Clears up internal containers
  Standard_EXPORT void Clear();

  //! Returns TRUE if thePOn2S is in one of existing lines.
  Standard_EXPORT bool IsPointOnLine(const IntSurf_PntOn2S& thePOn2S,
                                     const math_Vector&     theInfBounds,
                                     const math_Vector&     theSupBounds,
                                     math_FunctionSetRoot&  theSolver,
                                     Contap_SurfFunction&   theFunc);

private:
  bool                                            done;
  NCollection_Sequence<IntSurf_PathPoint>         seqSingle;
  double                                          fleche;
  double                                          pas;
  math_Vector                                     tolerance;
  double                                          epsilon;
  bool                                            reversed;
  IntWalk_VectorOfWalkingData                     wd1;
  IntWalk_VectorOfWalkingData                     wd2;
  IntWalk_VectorOfInteger                         nbMultiplicities;
  Bnd_Range                                       mySRangeU; // Estimated U-range for section curve
  Bnd_Range                                       mySRangeV; // Estimated V-range for section curve
  double                                          Um;
  double                                          UM;
  double                                          Vm;
  double                                          VM;
  IntSurf_PntOn2S                                 previousPoint;
  gp_Vec                                          previousd3d;
  gp_Dir2d                                        previousd2d;
  NCollection_Sequence<int>                       seqAjout;
  NCollection_Sequence<int>                       seqAlone;
  NCollection_DataMap<int, NCollection_List<int>> PointLineLine;
  NCollection_Sequence<occ::handle<Contap_TheIWLineOfTheIWalking>> lines;
  bool                                                             ToFillHoles;
};

#define ThePointOfPath IntSurf_PathPoint
#define ThePointOfPath_hxx <IntSurf_PathPoint.hxx>
#define ThePointOfPathTool IntSurf_PathPointTool
#define ThePointOfPathTool_hxx <IntSurf_PathPointTool.hxx>
#define ThePOPIterator NCollection_Sequence<IntSurf_PathPoint>
#define ThePOPIterator_hxx <NCollection_Sequence<IntSurf_PathPoint>.hxx>
#define ThePointOfLoop IntSurf_InteriorPoint
#define ThePointOfLoop_hxx <IntSurf_InteriorPoint.hxx>
#define ThePointOfLoopTool IntSurf_InteriorPointTool
#define ThePointOfLoopTool_hxx <IntSurf_InteriorPointTool.hxx>
#define ThePOLIterator NCollection_Sequence<IntSurf_InteriorPoint>
#define ThePOLIterator_hxx <NCollection_Sequence<IntSurf_InteriorPoint>.hxx>
#define ThePSurface occ::handle<Adaptor3d_Surface>
#define ThePSurface_hxx <Adaptor3d_Surface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define TheIWFunction Contap_SurfFunction
#define TheIWFunction_hxx <Contap_SurfFunction.hxx>
#define IntWalk_TheIWLine Contap_TheIWLineOfTheIWalking
#define IntWalk_TheIWLine_hxx <Contap_TheIWLineOfTheIWalking.hxx>
#define IntWalk_SequenceOfIWLine NCollection_Sequence<occ::handle<Contap_TheIWLineOfTheIWalking>>
#define IntWalk_SequenceOfIWLine_hxx                                                               \
  <NCollection_Sequence<occ::handle<Contap_TheIWLineOfTheIWalking>>.hxx>
#define Handle_IntWalk_TheIWLine occ::handle<Contap_TheIWLineOfTheIWalking>
#define IntWalk_IWalking Contap_TheIWalking
#define IntWalk_IWalking_hxx <Contap_TheIWalking.hxx>

#include <IntWalk_IWalking.lxx>

#undef ThePointOfPath
#undef ThePointOfPath_hxx
#undef ThePointOfPathTool
#undef ThePointOfPathTool_hxx
#undef ThePOPIterator
#undef ThePOPIterator_hxx
#undef ThePointOfLoop
#undef ThePointOfLoop_hxx
#undef ThePointOfLoopTool
#undef ThePointOfLoopTool_hxx
#undef ThePOLIterator
#undef ThePOLIterator_hxx
#undef ThePSurface
#undef ThePSurface_hxx
#undef ThePSurfaceTool
#undef ThePSurfaceTool_hxx
#undef TheIWFunction
#undef TheIWFunction_hxx
#undef IntWalk_TheIWLine
#undef IntWalk_TheIWLine_hxx
#undef IntWalk_SequenceOfIWLine
#undef IntWalk_SequenceOfIWLine_hxx
#undef Handle_IntWalk_TheIWLine
#undef IntWalk_IWalking
#undef IntWalk_IWalking_hxx

#endif // _Contap_TheIWalking_HeaderFile
