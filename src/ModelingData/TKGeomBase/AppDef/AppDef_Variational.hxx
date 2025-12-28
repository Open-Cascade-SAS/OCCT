// Created on: 1996-05-14
// Created by: Philippe MANGIN / Jeannine PANCIATICI
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

#ifndef _AppDef_Variational_HeaderFile
#define _AppDef_Variational_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppDef_MultiLine.hxx>
#include <Standard_Integer.hxx>
#include <AppParCurves_ConstraintCouple.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomAbs_Shape.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <Standard_OStream.hxx>
#include <NCollection_Array1.hxx>
#include <math_Vector.hxx>
#include <AppParCurves_Constraint.hxx>
#include <PLib_HermitJacobi.hxx>
class AppDef_SmoothCriterion;
class math_Matrix;
class FEmTool_Curve;
class FEmTool_Assembly;

//! This class is used to smooth N points with constraints
//! by minimization of quadratic criterium but also
//! variational criterium in order to obtain " fair Curve "
//! Computes the approximation of a Multiline by
//! Variational optimization.
class AppDef_Variational
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor.
  //! Initialization of the fields.
  //! Warning:
  //! Nc0 : number of PassagePoint consraints
  //! Nc2 : number of TangencyPoint constraints
  //! Nc3 : number of CurvaturePoint constraints
  //! if ((MaxDegree-Continuity)*MaxSegment -Nc0 - 2*Nc1 -3*Nc2)
  //! is negative
  //! The problem is over-constrained.
  //!
  //! Limitation : The MultiLine from AppDef has to be composed by
  //! only one Line ( Dimension 2 or 3).
  Standard_EXPORT AppDef_Variational(
    const AppDef_MultiLine&                               SSP,
    const int                                FirstPoint,
    const int                                LastPoint,
    const occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>>& TheConstraints,
    const int                                MaxDegree    = 14,
    const int                                MaxSegment   = 100,
    const GeomAbs_Shape                                   Continuity   = GeomAbs_C2,
    const bool                                WithMinMax   = false,
    const bool                                WithCutting  = true,
    const double                                   Tolerance    = 1.0,
    const int                                NbIterations = 2);

  //! Makes the approximation with the current fields.
  Standard_EXPORT void Approximate();

  //! returns True if the creation is done
  //! and correspond to the current fields.
  Standard_EXPORT bool IsCreated() const;

  //! returns True if the approximation is ok
  //! and correspond to the current fields.
  Standard_EXPORT bool IsDone() const;

  //! returns True if the problem is overconstrained
  //! in this case, approximation cannot be done.
  Standard_EXPORT bool IsOverConstrained() const;

  //! returns all the BSpline curves approximating the
  //! MultiLine from AppDef SSP after minimization of the parameter.
  Standard_EXPORT AppParCurves_MultiBSpCurve Value() const;

  //! returns the maximum of the distances between
  //! the points of the multiline and the approximation
  //! curves.
  Standard_EXPORT double MaxError() const;

  //! returns the index of the MultiPoint of ErrorMax
  Standard_EXPORT int MaxErrorIndex() const;

  //! returns the quadratic average of the distances between
  //! the points of the multiline and the approximation
  //! curves.
  Standard_EXPORT double QuadraticError() const;

  //! returns the distances between the points of the
  //! multiline and the approximation curves.
  Standard_EXPORT void Distance(math_Matrix& mat);

  //! returns the average error between
  //! the MultiLine from AppDef and the approximation.
  Standard_EXPORT double AverageError() const;

  //! returns the parameters uses to the approximations
  Standard_EXPORT const occ::handle<NCollection_HArray1<double>>& Parameters() const;

  //! returns the knots uses to the approximations
  Standard_EXPORT const occ::handle<NCollection_HArray1<double>>& Knots() const;

  //! returns the values of the quality criterium.
  Standard_EXPORT void Criterium(double& VFirstOrder,
                                 double& VSecondOrder,
                                 double& VThirdOrder) const;

  //! returns the Weights (as percent) associed to the criterium used in
  //! the optimization.
  Standard_EXPORT void CriteriumWeight(double& Percent1,
                                       double& Percent2,
                                       double& Percent3) const;

  //! returns the Maximum Degree used in the approximation
  Standard_EXPORT int MaxDegree() const;

  //! returns the Maximum of segment used in the approximation
  Standard_EXPORT int MaxSegment() const;

  //! returns the Continuity used in the approximation
  Standard_EXPORT GeomAbs_Shape Continuity() const;

  //! returns if the approximation search to minimize the
  //! maximum Error or not.
  Standard_EXPORT bool WithMinMax() const;

  //! returns if the approximation can insert new Knots or not.
  Standard_EXPORT bool WithCutting() const;

  //! returns the tolerance used in the approximation.
  Standard_EXPORT double Tolerance() const;

  //! returns the number of iterations used in the approximation.
  Standard_EXPORT int NbIterations() const;

  //! Prints on the stream o information on the current state
  //! of the object.
  //! MaxError,MaxErrorIndex,AverageError,QuadraticError,Criterium
  //! Distances,Degre,Nombre de poles, parametres, noeuds
  Standard_EXPORT void Dump(Standard_OStream& o) const;

  //! Define the constraints to approximate
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT bool
    SetConstraints(const occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>>& aConstrainst);

  //! Defines the parameters used by the approximations.
  Standard_EXPORT void SetParameters(const occ::handle<NCollection_HArray1<double>>& param);

  //! Defines the knots used by the approximations
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT bool SetKnots(const occ::handle<NCollection_HArray1<double>>& knots);

  //! Define the Maximum Degree used in the approximation
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT bool SetMaxDegree(const int Degree);

  //! Define the maximum number of segments used in the approximation
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT bool SetMaxSegment(const int NbSegment);

  //! Define the Continuity used in the approximation
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT bool SetContinuity(const GeomAbs_Shape C);

  //! Define if the approximation search to minimize the
  //! maximum Error or not.
  Standard_EXPORT void SetWithMinMax(const bool MinMax);

  //! Define if the approximation can insert new Knots or not.
  //! If this value is incompatible with the others fields
  //! this method modify nothing and returns false
  Standard_EXPORT bool SetWithCutting(const bool Cutting);

  //! define the Weights (as percent) associed to the criterium used in
  //! the optimization.
  //!
  //! if Percent <= 0
  Standard_EXPORT void SetCriteriumWeight(const double Percent1,
                                          const double Percent2,
                                          const double Percent3);

  //! define the Weight (as percent) associed to the
  //! criterium Order used in the optimization : Others
  //! weights are updated.
  //! if Percent < 0
  //! if Order < 1 or Order > 3
  Standard_EXPORT void SetCriteriumWeight(const int Order,
                                          const double    Percent);

  //! define the tolerance used in the approximation.
  Standard_EXPORT void SetTolerance(const double Tol);

  //! define the number of iterations used in the approximation.
  //! if Iter < 1
  Standard_EXPORT void SetNbIterations(const int Iter);

private:
  Standard_EXPORT void TheMotor(occ::handle<AppDef_SmoothCriterion>& J,
                                const double             WQuadratic,
                                const double             WQuality,
                                occ::handle<FEmTool_Curve>&          TheCurve,
                                NCollection_Array1<double>&           Ecarts);

  Standard_EXPORT void Adjusting(occ::handle<AppDef_SmoothCriterion>& J,
                                 double&                  WQuadratic,
                                 double&                  WQuality,
                                 occ::handle<FEmTool_Curve>&          TheCurve,
                                 NCollection_Array1<double>&           Ecarts);

  Standard_EXPORT void Optimization(occ::handle<AppDef_SmoothCriterion>& J,
                                    FEmTool_Assembly&               A,
                                    const bool          ToAssemble,
                                    const double             EpsDeg,
                                    occ::handle<FEmTool_Curve>&          Curve,
                                    const NCollection_Array1<double>&     Parameters) const;

  Standard_EXPORT void Project(const occ::handle<FEmTool_Curve>& C,
                               const NCollection_Array1<double>&  Ti,
                               NCollection_Array1<double>&        ProjTi,
                               NCollection_Array1<double>&        Distance,
                               int&            NumPoints,
                               double&               MaxErr,
                               double&               QuaErr,
                               double&               AveErr,
                               const int       NbIterations = 2) const;

  Standard_EXPORT void ACR(occ::handle<FEmTool_Curve>& Curve,
                           NCollection_Array1<double>&  Ti,
                           const int Decima) const;

  Standard_EXPORT void SplitCurve(const occ::handle<FEmTool_Curve>& InCurve,
                                  const NCollection_Array1<double>&  Ti,
                                  const double          CurveTol,
                                  occ::handle<FEmTool_Curve>&       OutCurve,
                                  bool&            iscut) const;

  Standard_EXPORT void Init();

  Standard_EXPORT void InitSmoothCriterion();

  Standard_EXPORT void InitParameters(double& Length);

  Standard_EXPORT void InitCriterionEstimations(const double Length,
                                                double&      J1,
                                                double&      J2,
                                                double&      J3) const;

  Standard_EXPORT void EstTangent(const int ipnt, math_Vector& VTang) const;

  Standard_EXPORT void EstSecnd(const int ipnt,
                                const math_Vector&     VTang1,
                                const math_Vector&     VTang2,
                                const double    Length,
                                math_Vector&           VScnd) const;

  Standard_EXPORT void InitCutting(const PLib_HermitJacobi& aBase,
                                   const double      CurvTol,
                                   occ::handle<FEmTool_Curve>&   aCurve) const;

  Standard_EXPORT void AssemblingConstraints(const occ::handle<FEmTool_Curve>& Curve,
                                             const NCollection_Array1<double>&  Parameters,
                                             const double          CBLONG,
                                             FEmTool_Assembly&            A) const;

  Standard_EXPORT bool InitTthetaF(const int        ndimen,
                                               const AppParCurves_Constraint typcon,
                                               const int        begin,
                                               const int        jndex);

  AppDef_MultiLine                               mySSP;
  int                               myNbP3d;
  int                               myNbP2d;
  int                               myDimension;
  int                               myFirstPoint;
  int                               myLastPoint;
  int                               myNbPoints;
  occ::handle<NCollection_HArray1<double>>                  myTabPoints;
  occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>> myConstraints;
  int                               myNbConstraints;
  occ::handle<NCollection_HArray1<double>>                  myTabConstraints;
  int                               myNbPassPoints;
  int                               myNbTangPoints;
  int                               myNbCurvPoints;
  occ::handle<NCollection_HArray1<int>>               myTypConstraints;
  occ::handle<NCollection_HArray1<double>>                  myTtheta;
  occ::handle<NCollection_HArray1<double>>                  myTfthet;
  int                               myMaxDegree;
  int                               myMaxSegment;
  int                               myNbIterations;
  double                                  myTolerance;
  GeomAbs_Shape                                  myContinuity;
  int                               myNivCont;
  bool                               myWithMinMax;
  bool                               myWithCutting;
  double                                  myPercent[3];
  double                                  myCriterium[4];
  occ::handle<AppDef_SmoothCriterion>                 mySmoothCriterion;
  occ::handle<NCollection_HArray1<double>>                  myParameters;
  occ::handle<NCollection_HArray1<double>>                  myKnots;
  AppParCurves_MultiBSpCurve                     myMBSpCurve;
  double                                  myMaxError;
  int                               myMaxErrorIndex;
  double                                  myAverageError;
  bool                               myIsCreated;
  bool                               myIsDone;
  bool                               myIsOverConstr;
};

#endif // _AppDef_Variational_HeaderFile
