// Created on: 1992-04-03
// Created by: Isabelle GRIGNON
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

#ifndef _IntWalk_PWalking_HeaderFile
#define _IntWalk_PWalking_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <gp_Dir.hxx>
#include <IntImp_ConstIsoparametric.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <gp_Dir2d.hxx>
#include <IntWalk_TheInt2S.hxx>
#include <NCollection_Array1.hxx>
#include <IntWalk_StatusDeflection.hxx>

class gp_Pnt;

//! This class implements an algorithm to determine the
//! intersection between 2 parametrized surfaces, marching from
//! a starting point. The intersection line
//! starts and ends on the natural surface's boundaries.
class IntWalk_PWalking
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor used to set the data to compute intersection
  //! lines between Caro1 and Caro2.
  //! Deflection is the maximum deflection admitted between two
  //! consecutive points on the resulting polyline.
  //! TolTangency is the tolerance to find a tangent point.
  //! Func is the criterion which has to be evaluated at each
  //! solution point (each point of the line).
  //! It is necessary to call the Perform method to compute
  //! the intersection lines.
  //! The line found starts at a point on or in 2 natural domains
  //! of surfaces. It can be closed in the
  //! standard case if it is open it stops and begins at the
  //! border of one of the domains. If an open line
  //! stops at the middle of a domain, one stops at the tangent point.
  //! Epsilon is SquareTolerance of points confusion.
  Standard_EXPORT IntWalk_PWalking(const occ::handle<Adaptor3d_Surface>& Caro1,
                                   const occ::handle<Adaptor3d_Surface>& Caro2,
                                   const double                          TolTangency,
                                   const double                          Epsilon,
                                   const double                          Deflection,
                                   const double                          Increment);

  //! Returns the intersection line containing the exact
  //! point Poin. This line is a polygonal line.
  //! Deflection is the maximum deflection admitted between two
  //! consecutive points on the resulting polyline.
  //! TolTangency is the tolerance to find a tangent point.
  //! Func is the criterion which has to be evaluated at each
  //! solution point (each point of the line).
  //! The line found starts at a point on or in 2 natural domains
  //! of surfaces. It can be closed in the
  //! standard case if it is open it stops and begins at the
  //! border of one of the domains. If an open line
  //! stops at the middle of a domain, one stops at the tangent point.
  //! Epsilon is SquareTolerance of points confusion.
  Standard_EXPORT IntWalk_PWalking(const occ::handle<Adaptor3d_Surface>& Caro1,
                                   const occ::handle<Adaptor3d_Surface>& Caro2,
                                   const double                          TolTangency,
                                   const double                          Epsilon,
                                   const double                          Deflection,
                                   const double                          Increment,
                                   const double                          U1,
                                   const double                          V1,
                                   const double                          U2,
                                   const double                          V2);

  //! calculate the line of intersection
  Standard_EXPORT void Perform(const NCollection_Array1<double>& ParDep);

  //! calculate the line of intersection. The regulation
  //! of steps is done using min and max values on u and
  //! v. (if this data is not presented as in the
  //! previous method, the initial steps are calculated
  //! starting from min and max uv of faces).
  Standard_EXPORT void Perform(const NCollection_Array1<double>& ParDep,
                               const double                      u1min,
                               const double                      v1min,
                               const double                      u2min,
                               const double                      v2min,
                               const double                      u1max,
                               const double                      v1max,
                               const double                      u2max,
                               const double                      v2max);

  //! calculate the first point of a line of intersection
  Standard_EXPORT bool PerformFirstPoint(const NCollection_Array1<double>& ParDep,
                                         IntSurf_PntOn2S&                  FirstPoint);

  //! Returns true if the calculus was successful.
  bool IsDone() const;

  //! Returns the number of points of the resulting polyline.
  //! An exception is raised if IsDone returns False.
  int NbPoints() const;

  //! Returns the point of range Index on the polyline.
  //! An exception is raised if IsDone returns False.
  //! An exception is raised if Index<=0 or Index>NbPoints.
  const IntSurf_PntOn2S& Value(const int Index) const;

  const occ::handle<IntSurf_LineOn2S>& Line() const;

  //! Returns True if the surface are tangent at the first point
  //! of the line.
  //! An exception is raised if IsDone returns False.
  bool TangentAtFirst() const;

  //! Returns true if the surface are tangent at the last point
  //! of the line.
  //! An exception is raised if IsDone returns False.
  bool TangentAtLast() const;

  //! Returns True if the line is closed.
  //! An exception is raised if IsDone returns False.
  bool IsClosed() const;

  const gp_Dir& TangentAtLine(int& Index) const;

  Standard_EXPORT IntWalk_StatusDeflection TestDeflection(const IntImp_ConstIsoparametric ChoixIso,
                                                          const IntWalk_StatusDeflection theStatus);

  Standard_EXPORT bool TestArret(const bool                  DejaReparti,
                                 NCollection_Array1<double>& Param,
                                 IntImp_ConstIsoparametric&  ChoixIso);

  Standard_EXPORT void RepartirOuDiviser(bool&                      DejaReparti,
                                         IntImp_ConstIsoparametric& ChoixIso,
                                         bool&                      Arrive);

  //! Inserts thePOn2S in the end of line
  void AddAPoint(const IntSurf_PntOn2S& thePOn2S);

  //! Removes point with index theIndex from line.
  //! If theIndex is greater than the number of points in line
  //! then the last point will be removed.
  //! theIndex must be started with 1.
  void RemoveAPoint(const int theIndex)
  {
    const int anIdx = (std::min)(theIndex, line->NbPoints());

    if (anIdx < 1)
      return;

    if (anIdx <= myTangentIdx)
    {
      myTangentIdx--;

      if (myTangentIdx < 1)
        myTangentIdx = 1;
    }

    line->RemovePoint(anIdx);
  }

  Standard_EXPORT bool PutToBoundary(const occ::handle<Adaptor3d_Surface>& theASurf1,
                                     const occ::handle<Adaptor3d_Surface>& theASurf2);

  Standard_EXPORT bool SeekAdditionalPoints(const occ::handle<Adaptor3d_Surface>& theASurf1,
                                            const occ::handle<Adaptor3d_Surface>& theASurf2,
                                            const int                             theMinNbPoints);

  double MaxStep(int theIndex)
  {
    Standard_OutOfRange_Raise_if((theIndex < 0) || (theIndex > 3),
                                 "IntWalk_PWalking::MaxStep() - index is out of range");
    return pasInit[theIndex];
  }

protected:
  Standard_EXPORT void ComputePasInit(const double theDeltaU1,
                                      const double theDeltaV1,
                                      const double theDeltaU2,
                                      const double theDeltaV2);

  //! Uses Gradient method in order to find intersection point between the given surfaces
  //! Arrays theInit (initial point to be precise) and theStep0 (steps-array) must contain
  //! four items and must be filled strictly in following order:
  //! {U-parameter on S1, V-parameter on S1, U-parameter on S2, V-parameter on S2}
  Standard_EXPORT bool DistanceMinimizeByGradient(const occ::handle<Adaptor3d_Surface>& theASurf1,
                                                  const occ::handle<Adaptor3d_Surface>& theASurf2,
                                                  NCollection_Array1<double>&           theInit,
                                                  const double* theStep0 = nullptr);

  //! Finds the point on theASurf which is the nearest point to theP0.
  //! theU0 and theV0 must be initialized (before calling the method) by initial
  //! parameters on theASurf. Their values are changed while algorithm being launched.
  //! Array theStep0 (steps-array) must contain two items and must be filled strictly in following
  //! order: {U-parameter, V-parameter}
  Standard_EXPORT bool DistanceMinimizeByExtrema(const occ::handle<Adaptor3d_Surface>& theASurf,
                                                 const gp_Pnt&                         theP0,
                                                 double&                               theU0,
                                                 double&                               theV0,
                                                 const double* theStep0 = nullptr);

  //! Searches an intersection point which lies on the some surface boundary.
  //! Found point (in case of successful result) is added in the line.
  //! theU1, theV1, theU2 and theV2 parameters are initial parameters in
  //! for used numeric algorithms. If isTheFirst == TRUE then
  //! a point on theASurf1 is searched. Otherwise, the point on theASurf2 is searched.
  //!
  //! ATTENTION!!!
  //!   This method can delete some points from the curve if it is necessary
  //!   (in order to obtain correct result after insertion).
  //!   Returns TRUE in case of success adding (i.e. can return FALSE even after
  //!   removing some points).
  Standard_EXPORT bool SeekPointOnBoundary(const occ::handle<Adaptor3d_Surface>& theASurf1,
                                           const occ::handle<Adaptor3d_Surface>& theASurf2,
                                           const double                          theU1,
                                           const double                          theV1,
                                           const double                          theU2,
                                           const double                          theV2,
                                           const bool                            isTheFirst);

  // Method to handle single singular point. Sub-method in SeekPointOnBoundary.
  Standard_EXPORT bool HandleSingleSingularPoint(const occ::handle<Adaptor3d_Surface>& theASurf1,
                                                 const occ::handle<Adaptor3d_Surface>& theASurf2,
                                                 const double                          the3DTol,
                                                 NCollection_Array1<double>&           thePnt);

  Standard_EXPORT bool ExtendLineInCommonZone(const IntImp_ConstIsoparametric theChoixIso,
                                              const bool                      theDirectionFlag);

private:
  bool                          done;
  occ::handle<IntSurf_LineOn2S> line;
  bool                          close;
  bool                          tgfirst;
  bool                          tglast;

  //! Index of point on the surface boundary.
  //! It is used for transition computation
  int myTangentIdx;

  //! Tangent to WLine in the point with index myTangentIdx
  gp_Dir tgdir;

  double                    fleche;
  double                    pasMax;
  double                    tolconf;
  double                    myTolTang;
  double                    pasuv[4];
  double                    myStepMin[4];
  double                    pasSav[4];
  double                    pasInit[4];
  double                    Um1;
  double                    UM1;
  double                    Vm1;
  double                    VM1;
  double                    Um2;
  double                    UM2;
  double                    Vm2;
  double                    VM2;
  double                    ResoU1;
  double                    ResoU2;
  double                    ResoV1;
  double                    ResoV2;
  int                       sensCheminement;
  IntImp_ConstIsoparametric choixIsoSav;
  IntSurf_PntOn2S           previousPoint;
  bool                      previoustg;
  gp_Dir                    previousd;
  gp_Dir2d                  previousd1;
  gp_Dir2d                  previousd2;
  gp_Dir2d                  firstd1;
  gp_Dir2d                  firstd2;
  IntWalk_TheInt2S          myIntersectionOn2S;
  int                       STATIC_BLOCAGE_SUR_PAS_TROP_GRAND;
  int                       STATIC_PRECEDENT_INFLEXION;
};

#include <IntWalk_PWalking.lxx>

#endif // _IntWalk_PWalking_HeaderFile
