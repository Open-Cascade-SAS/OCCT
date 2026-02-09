// Created on: 1993-07-07
// Created by: Jean Claude VAUTHIER
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

// Version:
// pmn 24/09/96 Ajout du prolongement de courbe.
//              jct 15/04/97 Ajout du prolongement de surface.
//              jct 24/04/97 simplification ou suppression de calculs
//                           inutiles dans ExtendSurfByLength
//                           correction de Tbord et Continuity=0 accepte
//                           correction du calcul de lambda et appel a
//                           TangExtendToConstraint avec lambmin au lieu de 1.
//                           correction du passage Sr rat --> BSp nD
//              xab 26/06/97 treatement partiel anulation des derivees
//                           partiels du denonimateur des Surfaces BSplines Rationnelles
//                           dans le cas de valeurs proportionnelles des denominateurs
//                           en umin umax et/ou vmin vmax.
//              pmn 4/07/97  Gestion de la continuite dans BuildCurve3d (PRO9097)
//              xab 10/07/97 on revient en arriere sur l'ajout du 26/06/97
//              pmn 26/09/97 Ajout des parametres d'approx dans BuildCurve3d
//              xab 29/09/97 on reintegre l'ajout du 26/06/97
//              pmn 31/10/97 Ajoute AdjustExtremity
//              jct 26/11/98 blindage dans ExtendSurf qd NTgte = 0 (CTS21288)
//              jct 19/01/99 traitement de la periodicite dans ExtendSurf
// Design:
// Warning:      None
// References:   None
// Language:     C++2.0
// Purpose:
// Declarations:

#include <GeomLib.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_Surface.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <CSLib.hxx>
#include <CSLib_NormalStatus.hxx>
#include <ElCLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomLib_DenominatorMultiplier.hxx>
#include <GeomLib_DenominatorMultiplierPtr.hxx>
#include <GeomLib_LogSample.hxx>
#include <GeomLib_MakeCurvefromApprox.hxx>
#include <GeomLib_PolyFunc.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_TrsfForm.hxx>
#include <gp_Vec.hxx>
#include <Hermit.hxx>
#include <math.hxx>
#include <math_FunctionAllRoots.hxx>
#include <math_FunctionSample.hxx>
#include <math_Jacobi.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <NCollection_Array1.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>
//
static bool CompareWeightPoles(const NCollection_Array1<gp_Pnt>&       thePoles1,
                               const NCollection_Array1<double>* const theW1,
                               const NCollection_Array1<gp_Pnt>&       thePoles2,
                               const NCollection_Array1<double>* const theW2,
                               const double                            theTol);

//=======================================================================
// function : ComputeLambda
// purpose  : Calcul le facteur lambda qui minimise la variation de vittesse
//           sur une interpolation d'hermite d'ordre (i,0)
//=======================================================================
static void ComputeLambda(const math_Matrix& Constraint,
                          const math_Matrix& Hermit,
                          const double       Length,
                          double&            Lambda)
{
  int size       = Hermit.RowNumber();
  int Continuity = size - 2;
  int ii, jj, ip, pp;

  // Minimization
  math_Matrix HDer(1, size - 1, 1, size);
  for (jj = 1; jj <= size; jj++)
  {
    for (ii = 1; ii < size; ii++)
    {
      HDer(ii, jj) = ii * Hermit(jj, ii + 1);
    }
  }

  math_Vector V(1, size);
  math_Vector Vec1(1, Constraint.RowNumber());
  math_Vector Vec2(1, Constraint.RowNumber());
  math_Vector Vec3(1, Constraint.RowNumber());
  math_Vector Vec4(1, Constraint.RowNumber());

  double* polynome = &HDer(1, 1);
  double* valhder  = &V(1);
  Vec2             = Constraint.Col(2);
  Vec2 /= Length;
  double t, squared1 = Vec2.Norm2(), GW;
  //  math_Matrix Vec(1, Constraint.RowNumber(), 1, size-1);
  //  gp_Vec Vfirst(p0.XYZ()), Vlast(Point.XYZ());
  //  NCollection_Array1<gp_Vec> Der(2, 4);
  //  Der(2) = d1; Der(3) = d2; Der(4) = d3;

  int         GOrdre = 4 + 4 * Continuity, DDim = Continuity * (Continuity + 2);
  math_Vector GaussP(1, GOrdre), GaussW(1, GOrdre), pol2(1, 2 * Continuity + 1),
    pol4(1, 4 * Continuity + 1);
  math::GaussPoints(GOrdre, GaussP);
  math::GaussWeights(GOrdre, GaussW);
  pol4.Init(0.);

  for (ip = 1; ip <= GOrdre; ip++)
  {
    t  = (GaussP(ip) + 1.) / 2;
    GW = GaussW(ip);
    PLib::NoDerivativeEvalPolynomial(t, Continuity, Continuity + 2, DDim, polynome[0], valhder[0]);
    V /= Length; // Normalisation

    //                      i
    // C'(t) = SUM Vi*Lambda
    Vec1 = Constraint.Col(1);
    Vec1 *= V(1);
    Vec1 += V(size) * Constraint.Col(size);
    Vec2 = Constraint.Col(2);
    Vec2 *= V(2);
    if (Continuity > 1)
    {
      Vec3 = Constraint.Col(3);
      Vec3 *= V(3);
      if (Continuity > 2)
      {
        Vec4 = Constraint.Col(4);
        Vec4 *= V(4);
      }
    }

    //   2          2
    // C'(t) - C'(0)

    pol2(1) = Vec1.Norm2();
    pol2(2) = 2 * (Vec1.Multiplied(Vec2));
    pol2(3) = Vec2.Norm2() - squared1;
    if (Continuity > 1)
    {
      pol2(3) += 2 * (Vec1.Multiplied(Vec3));
      pol2(4) = 2 * (Vec2.Multiplied(Vec3));
      pol2(5) = Vec3.Norm2();
      if (Continuity > 2)
      {
        pol2(4) += 2 * (Vec1.Multiplied(Vec4));
        pol2(5) += 2 * (Vec2.Multiplied(Vec4));
        pol2(6) = 2 * (Vec3.Multiplied(Vec4));
        pol2(7) = Vec4.Norm2();
      }
    }

    //                     2      2  2
    // Integrale de ( C'(t) - C'(0) )
    for (ii = 1; ii <= pol2.Length(); ii++)
    {
      pp = ii;
      for (jj = 1; jj < ii; jj++, pp++)
      {
        pol4(pp) += 2 * GW * pol2(ii) * pol2(jj);
      }
      pol4(2 * ii - 1) += GW * std::pow(pol2(ii), 2);
    }
  }

  double EMin, E;
  PLib::NoDerivativeEvalPolynomial(Lambda, pol4.Length() - 1, 1, pol4.Length() - 1, pol4(1), EMin);

  if (EMin > Precision::Confusion())
  {
    // Recheche des extrema de la fonction
    GeomLib_PolyFunc      FF(pol4);
    GeomLib_LogSample     S(Lambda / 1000, 50 * Lambda, 100);
    math_FunctionAllRoots Solve(FF,
                                S,
                                Precision::Confusion(),
                                Precision::Confusion() * (Length + 1),
                                1.e-15);
    if (Solve.IsDone())
    {
      for (ii = 1; ii <= Solve.NbPoints(); ii++)
      {
        t = Solve.GetPoint(ii);
        PLib::NoDerivativeEvalPolynomial(t, pol4.Length() - 1, 1, pol4.Length() - 1, pol4(1), E);
        if (E < EMin)
        {
          Lambda = t;
          EMin   = E;
        }
      }
    }
  }
}

#include <Extrema_LocateExtPC.hxx>

//=================================================================================================

void GeomLib::RemovePointsFromArray(const int                                 NumPoints,
                                    const NCollection_Array1<double>&         InParameters,
                                    occ::handle<NCollection_HArray1<double>>& OutParameters)
{
  int    ii, jj, add_one_point, loc_num_points, num_points, index;
  double delta, current_parameter;

  loc_num_points = std::max(0, NumPoints - 2);
  delta          = InParameters(InParameters.Upper()) - InParameters(InParameters.Lower());
  delta /= (double)(loc_num_points + 1);
  num_points        = 1;
  current_parameter = InParameters(InParameters.Lower()) + delta * 0.5e0;
  ii                = InParameters.Lower() + 1;
  for (jj = 0; ii < InParameters.Upper() && jj < NumPoints; jj++)
  {
    add_one_point = 0;
    while (ii < InParameters.Upper() && InParameters(ii) < current_parameter)
    {
      ii += 1;
      add_one_point = 1;
    }
    num_points += add_one_point;
    current_parameter += delta;
  }
  if (NumPoints <= 2)
  {
    num_points = 2;
  }
  index                            = 2;
  current_parameter                = InParameters(InParameters.Lower()) + delta * 0.5e0;
  OutParameters                    = new NCollection_HArray1<double>(1, num_points);
  OutParameters->ChangeArray1()(1) = InParameters(InParameters.Lower());
  ii                               = InParameters.Lower() + 1;
  for (jj = 0; ii < InParameters.Upper() && jj < NumPoints; jj++)
  {
    add_one_point = 0;
    while (ii < InParameters.Upper() && InParameters(ii) < current_parameter)
    {
      ii += 1;
      add_one_point = 1;
    }
    if (add_one_point && index <= num_points)
    {
      OutParameters->ChangeArray1()(index) = InParameters(ii - 1);
      index += 1;
    }
    current_parameter += delta;
  }
  OutParameters->ChangeArray1()(num_points) = InParameters(InParameters.Upper());
}

//=================================================================================================

void GeomLib::DensifyArray1OfReal(const int                                 MinNumPoints,
                                  const NCollection_Array1<double>&         InParameters,
                                  occ::handle<NCollection_HArray1<double>>& OutParameters)
{
  int    ii, in_order, num_points, num_parameters_to_add, index;
  double delta, current_parameter;

  in_order = 1;
  if (MinNumPoints > InParameters.Length())
  {

    //
    // checks the parameters are in increasing order
    //
    for (ii = InParameters.Lower(); ii < InParameters.Upper(); ii++)
    {
      if (InParameters(ii) > InParameters(ii + 1))
      {
        in_order = 0;
        break;
      }
    }
    if (in_order)
    {
      num_parameters_to_add = MinNumPoints - InParameters.Length();
      delta = InParameters(InParameters.Upper()) - InParameters(InParameters.Lower());
      delta /= (double)(num_parameters_to_add + 1);
      num_points                           = MinNumPoints;
      OutParameters                        = new NCollection_HArray1<double>(1, num_points);
      index                                = 1;
      current_parameter                    = InParameters(InParameters.Lower());
      OutParameters->ChangeArray1()(index) = current_parameter;
      index += 1;
      current_parameter += delta;
      for (ii = InParameters.Lower() + 1; index <= num_points && ii <= InParameters.Upper(); ii++)
      {
        while (current_parameter < InParameters(ii) && index <= num_points)
        {
          OutParameters->ChangeArray1()(index) = current_parameter;
          index += 1;
          current_parameter += delta;
        }
        if (index <= num_points)
        {
          OutParameters->ChangeArray1()(index) = InParameters(ii);
        }
        index += 1;
      }
      //
      // beware of roundoff !
      //
      OutParameters->ChangeArray1()(num_points) = InParameters(InParameters.Upper());
    }
    else
    {
      index         = 1;
      num_points    = InParameters.Length();
      OutParameters = new NCollection_HArray1<double>(1, num_points);
      for (ii = InParameters.Lower(); ii <= InParameters.Upper(); ii++)
      {
        OutParameters->ChangeArray1()(index) = InParameters(ii);
        index += 1;
      }
    }
  }
  else
  {
    index         = 1;
    num_points    = InParameters.Length();
    OutParameters = new NCollection_HArray1<double>(1, num_points);
    for (ii = InParameters.Lower(); ii <= InParameters.Upper(); ii++)
    {
      OutParameters->ChangeArray1()(index) = InParameters(ii);
      index += 1;
    }
  }
}

//=================================================================================================

void GeomLib::FuseIntervals(const NCollection_Array1<double>& I1,
                            const NCollection_Array1<double>& I2,
                            NCollection_Sequence<double>&     Seq,
                            const double                      Epspar,
                            const bool                        IsAdjustToFirstInterval)
{
  int    ind1 = 1, ind2 = 1;
  double v1, v2;
  // Initialisations : les IND1 et IND2 pointent sur le 1er element
  // de chacune des 2 tables a traiter.INDS pointe sur le dernier
  // element cree de TABSOR

  //--- On remplit TABSOR en parcourant TABLE1 et TABLE2 simultanement ---
  //------------------ en eliminant les occurrences multiples ------------

  while ((ind1 <= I1.Upper()) && (ind2 <= I2.Upper()))
  {
    v1 = I1(ind1);
    v2 = I2(ind2);
    if (std::abs(v1 - v2) <= Epspar)
    {
      // Ici les elements de I1 et I2 conviennent .
      if (IsAdjustToFirstInterval)
      {
        Seq.Append(v1);
      }
      else
      {
        Seq.Append((v1 + v2) / 2);
      }
      ind1++;
      ind2++;
    }
    else if (v1 < v2)
    {
      // Ici l' element de I1 convient.
      Seq.Append(v1);
      ind1++;
    }
    else
    {
      // Ici l' element de TABLE2 convient.
      Seq.Append(v2);
      ind2++;
    }
  }

  if (ind1 > I1.Upper())
  {
    //----- Ici I1 est epuise, on complete avec la fin de TABLE2 -------

    for (; ind2 <= I2.Upper(); ind2++)
    {
      Seq.Append(I2(ind2));
    }
  }

  if (ind2 > I2.Upper())
  {
    //----- Ici I2 est epuise, on complete avec la fin de I1 -------
    for (; ind1 <= I1.Upper(); ind1++)
    {
      Seq.Append(I1(ind1));
    }
  }
}

//=================================================================================================

void GeomLib::EvalMaxParametricDistance(const Adaptor3d_Curve& ACurve,
                                        const Adaptor3d_Curve& AReferenceCurve,
                                        //			       const double  Tolerance,
                                        const double,
                                        const NCollection_Array1<double>& Parameters,
                                        double&                           MaxDistance)
{
  int ii;

  double max_squared = 0.0e0,
         //    tolerance_squared,
    local_distance_squared;

  //  tolerance_squared = Tolerance * Tolerance ;
  gp_Pnt Point1;
  gp_Pnt Point2;
  for (ii = Parameters.Lower(); ii <= Parameters.Upper(); ii++)
  {
    ACurve.D0(Parameters(ii), Point1);
    AReferenceCurve.D0(Parameters(ii), Point2);
    local_distance_squared = Point1.SquareDistance(Point2);
    max_squared            = std::max(max_squared, local_distance_squared);
  }
  if (max_squared > 0.0e0)
  {
    MaxDistance = sqrt(max_squared);
  }
  else
  {
    MaxDistance = 0.0e0;
  }
}

//=================================================================================================

void GeomLib::EvalMaxDistanceAlongParameter(const Adaptor3d_Curve&            ACurve,
                                            const Adaptor3d_Curve&            AReferenceCurve,
                                            const double                      Tolerance,
                                            const NCollection_Array1<double>& Parameters,
                                            double&                           MaxDistance)
{
  int    ii;
  double max_squared = 0.0e0, tolerance_squared = Tolerance * Tolerance, other_parameter,
         para_tolerance, local_distance_squared;
  gp_Pnt Point1;
  gp_Pnt Point2;

  para_tolerance  = AReferenceCurve.Resolution(Tolerance);
  other_parameter = Parameters(Parameters.Lower());
  ACurve.D0(other_parameter, Point1);
  Extrema_LocateExtPC a_projector(Point1, AReferenceCurve, other_parameter, para_tolerance);
  for (ii = Parameters.Lower(); ii <= Parameters.Upper(); ii++)
  {
    ACurve.D0(Parameters(ii), Point1);
    AReferenceCurve.D0(Parameters(ii), Point2);
    local_distance_squared = Point1.SquareDistance(Point2);

    if (local_distance_squared > tolerance_squared)
    {

      a_projector.Perform(Point1, other_parameter);
      if (a_projector.IsDone())
      {
        other_parameter = a_projector.Point().Parameter();
        AReferenceCurve.D0(other_parameter, Point2);
        local_distance_squared = Point1.SquareDistance(Point2);
      }
      else
      {
        local_distance_squared = 0.0e0;
        other_parameter        = Parameters(ii);
      }
    }
    else
    {
      other_parameter = Parameters(ii);
    }

    max_squared = std::max(max_squared, local_distance_squared);
  }
  if (max_squared > tolerance_squared)
  {
    MaxDistance = sqrt(max_squared);
  }
  else
  {
    MaxDistance = Tolerance;
  }
}

// Aliases:

// Global data definitions:

// Methods :

//=================================================================================================

occ::handle<Geom_Curve> GeomLib::To3d(const gp_Ax2&                    Position,
                                      const occ::handle<Geom2d_Curve>& Curve2d)
{
  occ::handle<Geom_Curve>    Curve3d;
  occ::handle<Standard_Type> KindOfCurve = Curve2d->DynamicType();

  if (KindOfCurve == STANDARD_TYPE(Geom2d_TrimmedCurve))
  {
    occ::handle<Geom2d_TrimmedCurve> Ct       = occ::down_cast<Geom2d_TrimmedCurve>(Curve2d);
    double                           U1       = Ct->FirstParameter();
    double                           U2       = Ct->LastParameter();
    occ::handle<Geom2d_Curve>        CBasis2d = Ct->BasisCurve();
    occ::handle<Geom_Curve>          CC       = GeomLib::To3d(Position, CBasis2d);
    Curve3d                                   = new Geom_TrimmedCurve(CC, U1, U2);
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_OffsetCurve))
  {
    occ::handle<Geom2d_OffsetCurve> Co       = occ::down_cast<Geom2d_OffsetCurve>(Curve2d);
    double                          Offset   = Co->Offset();
    occ::handle<Geom2d_Curve>       CBasis2d = Co->BasisCurve();
    occ::handle<Geom_Curve>         CC       = GeomLib::To3d(Position, CBasis2d);
    Curve3d = new Geom_OffsetCurve(CC, Offset, Position.Direction());
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_BezierCurve))
  {
    occ::handle<Geom2d_BezierCurve>    CBez2d    = occ::down_cast<Geom2d_BezierCurve>(Curve2d);
    const NCollection_Array1<gp_Pnt2d>& Poles2d  = CBez2d->Poles();
    const int                           Nbpoles  = Poles2d.Length();
    NCollection_Array1<gp_Pnt>          Poles3d(1, Nbpoles);
    for (int i = 1; i <= Nbpoles; i++)
    {
      Poles3d(i) = ElCLib::To3d(Position, Poles2d(i));
    }
    occ::handle<Geom_BezierCurve> CBez3d;
    if (const NCollection_Array1<double>* pWeights = CBez2d->Weights())
    {
      CBez3d = new Geom_BezierCurve(Poles3d, *pWeights);
    }
    else
    {
      CBez3d = new Geom_BezierCurve(Poles3d);
    }
    Curve3d = CBez3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_BSplineCurve))
  {
    occ::handle<Geom2d_BSplineCurve>    CBSpl2d    = occ::down_cast<Geom2d_BSplineCurve>(Curve2d);
    const int                           TheDegree  = CBSpl2d->Degree();
    const bool                          IsPeriodic = CBSpl2d->IsPeriodic();
    const NCollection_Array1<gp_Pnt2d>& Poles2d    = CBSpl2d->Poles();
    const int                           Nbpoles    = Poles2d.Length();
    NCollection_Array1<gp_Pnt>          Poles3d(1, Nbpoles);
    for (int i = 1; i <= Nbpoles; i++)
    {
      Poles3d(i) = ElCLib::To3d(Position, Poles2d(i));
    }
    const NCollection_Array1<double>& TheKnots = CBSpl2d->Knots();
    const NCollection_Array1<int>&    TheMults = CBSpl2d->Multiplicities();
    occ::handle<Geom_BSplineCurve>    CBSpl3d;
    if (const NCollection_Array1<double>* pWeights = CBSpl2d->Weights())
    {
      CBSpl3d =
        new Geom_BSplineCurve(Poles3d, *pWeights, TheKnots, TheMults, TheDegree, IsPeriodic);
    }
    else
    {
      CBSpl3d = new Geom_BSplineCurve(Poles3d, TheKnots, TheMults, TheDegree, IsPeriodic);
    }
    Curve3d = CBSpl3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_Line))
  {
    occ::handle<Geom2d_Line> Line2d  = occ::down_cast<Geom2d_Line>(Curve2d);
    gp_Lin2d                 L2d     = Line2d->Lin2d();
    gp_Lin                   L3d     = ElCLib::To3d(Position, L2d);
    occ::handle<Geom_Line>   GeomL3d = new Geom_Line(L3d);
    Curve3d                          = GeomL3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_Circle))
  {
    occ::handle<Geom2d_Circle> Circle2d = occ::down_cast<Geom2d_Circle>(Curve2d);
    gp_Circ2d                  C2d      = Circle2d->Circ2d();
    gp_Circ                    C3d      = ElCLib::To3d(Position, C2d);
    occ::handle<Geom_Circle>   GeomC3d  = new Geom_Circle(C3d);
    Curve3d                             = GeomC3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_Ellipse))
  {
    occ::handle<Geom2d_Ellipse> Ellipse2d = occ::down_cast<Geom2d_Ellipse>(Curve2d);
    gp_Elips2d                  E2d       = Ellipse2d->Elips2d();
    gp_Elips                    E3d       = ElCLib::To3d(Position, E2d);
    occ::handle<Geom_Ellipse>   GeomE3d   = new Geom_Ellipse(E3d);
    Curve3d                               = GeomE3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_Parabola))
  {
    occ::handle<Geom2d_Parabola> Parabola2d = occ::down_cast<Geom2d_Parabola>(Curve2d);
    gp_Parab2d                   Prb2d      = Parabola2d->Parab2d();
    gp_Parab                     Prb3d      = ElCLib::To3d(Position, Prb2d);
    occ::handle<Geom_Parabola>   GeomPrb3d  = new Geom_Parabola(Prb3d);
    Curve3d                                 = GeomPrb3d;
  }
  else if (KindOfCurve == STANDARD_TYPE(Geom2d_Hyperbola))
  {
    occ::handle<Geom2d_Hyperbola> Hyperbola2d = occ::down_cast<Geom2d_Hyperbola>(Curve2d);
    gp_Hypr2d                     H2d         = Hyperbola2d->Hypr2d();
    gp_Hypr                       H3d         = ElCLib::To3d(Position, H2d);
    occ::handle<Geom_Hyperbola>   GeomH3d     = new Geom_Hyperbola(H3d);
    Curve3d                                   = GeomH3d;
  }
  else
  {
    throw Standard_NotImplemented();
  }

  return Curve3d;
}

//=================================================================================================

occ::handle<Geom2d_Curve> GeomLib::GTransform(const occ::handle<Geom2d_Curve>& Curve,
                                              const gp_GTrsf2d&                GTrsf)
{
  gp_TrsfForm Form = GTrsf.Form();

  if (Form != gp_Other)
  {

    // Alors, la GTrsf est en fait une Trsf.
    // La geometrie des courbes sera alors inchangee.

    occ::handle<Geom2d_Curve> C = occ::down_cast<Geom2d_Curve>(Curve->Transformed(GTrsf.Trsf2d()));
    return C;
  }
  else
  {

    // So, the GTrsf is an other Transformation.
    // The geometry of the curves is then changed, and the conics will have to
    // be converted to BSplines.

    occ::handle<Standard_Type> TheType = Curve->DynamicType();

    if (TheType == STANDARD_TYPE(Geom2d_TrimmedCurve))
    {

      // We will recurse on the BasisCurve

      occ::handle<Geom2d_TrimmedCurve> C = occ::down_cast<Geom2d_TrimmedCurve>(Curve->Copy());

      occ::handle<Standard_Type> TheBasisType = (C->BasisCurve())->DynamicType();

      if (TheBasisType == STANDARD_TYPE(Geom2d_BSplineCurve)
          || TheBasisType == STANDARD_TYPE(Geom2d_BezierCurve))
      {

        // Dans ces cas le parametrage est conserve sur la courbe transformee
        // on peut donc la trimmer avec les parametres de la courbe de base.

        double U1 = C->FirstParameter();
        double U2 = C->LastParameter();

        occ::handle<Geom2d_TrimmedCurve> result =
          new Geom2d_TrimmedCurve(GTransform(C->BasisCurve(), GTrsf), U1, U2);
        return result;
      }
      else if (TheBasisType == STANDARD_TYPE(Geom2d_Line))
      {

        // Dans ce cas, le parametrage n`est plus conserve.
        // Il faut recalculer les parametres de Trimming sur la courbe
        // resultante. ( Calcul par projection ( ElCLib) des points debut
        // et fin transformes)

        occ::handle<Geom2d_Line> L =
          occ::down_cast<Geom2d_Line>(GTransform(C->BasisCurve(), GTrsf));
        gp_Lin2d Lin = L->Lin2d();

        gp_Pnt2d P1 = C->StartPoint();
        gp_Pnt2d P2 = C->EndPoint();
        P1.SetXY(GTrsf.Transformed(P1.XY()));
        P2.SetXY(GTrsf.Transformed(P2.XY()));
        double U1 = ElCLib::Parameter(Lin, P1);
        double U2 = ElCLib::Parameter(Lin, P2);

        occ::handle<Geom2d_TrimmedCurve> result = new Geom2d_TrimmedCurve(L, U1, U2);
        return result;
      }
      else if (TheBasisType == STANDARD_TYPE(Geom2d_Circle)
               || TheBasisType == STANDARD_TYPE(Geom2d_Ellipse)
               || TheBasisType == STANDARD_TYPE(Geom2d_Parabola)
               || TheBasisType == STANDARD_TYPE(Geom2d_Hyperbola))
      {

        // Dans ces cas, la geometrie de la courbe n`est pas conservee
        // on la convertir en BSpline avant de lui appliquer la Trsf.

        occ::handle<Geom2d_BSplineCurve> BS = Geom2dConvert::CurveToBSplineCurve(C);
        return GTransform(BS, GTrsf);
      }
      else
      {

        // La transformee d`une OffsetCurve vaut ????? Sais pas faire !!

        occ::handle<Geom2d_Curve> dummy;
        return dummy;
      }
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Line))
    {

      occ::handle<Geom2d_Line> L   = occ::down_cast<Geom2d_Line>(Curve->Copy());
      gp_Lin2d                 Lin = L->Lin2d();
      gp_Pnt2d                 P   = Lin.Location();
      gp_Pnt2d                 PP  = L->Value(10.); // pourquoi pas !!
      P.SetXY(GTrsf.Transformed(P.XY()));
      PP.SetXY(GTrsf.Transformed(PP.XY()));
      L->SetLocation(P);
      gp_Vec2d V(P, PP);
      L->SetDirection(gp_Dir2d(V));
      return L;
    }
    else if (TheType == STANDARD_TYPE(Geom2d_BezierCurve))
    {

      // Les GTrsf etant des operation lineaires, la transformee d`une courbe
      // a poles est la courbe dont les poles sont la transformee des poles
      // de la courbe de base.

      occ::handle<Geom2d_BezierCurve> C       = occ::down_cast<Geom2d_BezierCurve>(Curve->Copy());
      const int                       NbPoles = C->NbPoles();
      NCollection_Array1<gp_Pnt2d>    Poles(C->Poles());
      for (int i = 1; i <= NbPoles; i++)
      {
        Poles(i).SetXY(GTrsf.Transformed(Poles(i).XY()));
        C->SetPole(i, Poles(i));
      }
      return C;
    }
    else if (TheType == STANDARD_TYPE(Geom2d_BSplineCurve))
    {

      // Voir commentaire pour les Bezier.

      occ::handle<Geom2d_BSplineCurve> C       = occ::down_cast<Geom2d_BSplineCurve>(Curve->Copy());
      const int                        NbPoles = C->NbPoles();
      NCollection_Array1<gp_Pnt2d>     Poles(C->Poles());
      for (int i = 1; i <= NbPoles; i++)
      {
        Poles(i).SetXY(GTrsf.Transformed(Poles(i).XY()));
        C->SetPole(i, Poles(i));
      }
      return C;
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Circle) || TheType == STANDARD_TYPE(Geom2d_Ellipse))
    {

      // Dans ces cas, la geometrie de la courbe n`est pas conservee
      // on la convertir en BSpline avant de lui appliquer la Trsf.

      occ::handle<Geom2d_BSplineCurve> C = Geom2dConvert::CurveToBSplineCurve(Curve);
      return GTransform(C, GTrsf);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Parabola) || TheType == STANDARD_TYPE(Geom2d_Hyperbola)
             || TheType == STANDARD_TYPE(Geom2d_OffsetCurve))
    {

      // On ne sait pas faire : return a null Handle;

      occ::handle<Geom2d_Curve> dummy;
      return dummy;
    }
  }

  occ::handle<Geom2d_Curve> WNT__; // portage Windows.
  return WNT__;
}

//=================================================================================================

void GeomLib::SameRange(const double                     Tolerance,
                        const occ::handle<Geom2d_Curve>& CurvePtr,
                        const double                     FirstOnCurve,
                        const double                     LastOnCurve,
                        const double                     RequestedFirst,
                        const double                     RequestedLast,
                        occ::handle<Geom2d_Curve>&       NewCurvePtr)
{
  if (CurvePtr.IsNull())
    throw Standard_Failure();
  if (std::abs(LastOnCurve - RequestedLast) <= Tolerance
      && std::abs(FirstOnCurve - RequestedFirst) <= Tolerance)
  {
    NewCurvePtr = CurvePtr;
    return;
  }

  // the parametrisation length  must at least be the same.
  if (std::abs(LastOnCurve - FirstOnCurve - RequestedLast + RequestedFirst) <= Tolerance)
  {
    if (CurvePtr->IsKind(STANDARD_TYPE(Geom2d_Line)))
    {
      occ::handle<Geom2d_Line> Line = occ::down_cast<Geom2d_Line>(CurvePtr->Copy());
      double                   dU   = FirstOnCurve - RequestedFirst;
      gp_Dir2d                 D    = Line->Direction();
      Line->Translate(dU * gp_Vec2d(D));
      NewCurvePtr = Line;
    }
    else if (CurvePtr->IsKind(STANDARD_TYPE(Geom2d_Circle)))
    {
      gp_Trsf2d Trsf;
      NewCurvePtr                     = occ::down_cast<Geom2d_Curve>(CurvePtr->Copy());
      occ::handle<Geom2d_Circle> Circ = occ::down_cast<Geom2d_Circle>(NewCurvePtr);
      gp_Pnt2d                   P    = Circ->Location();
      double                     dU;
      if (Circ->Circ2d().IsDirect())
      {
        dU = FirstOnCurve - RequestedFirst;
      }
      else
      {
        dU = RequestedFirst - FirstOnCurve;
      }
      Trsf.SetRotation(P, dU);
      NewCurvePtr->Transform(Trsf);
    }
    else if (CurvePtr->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      occ::handle<Geom2d_TrimmedCurve> TC = occ::down_cast<Geom2d_TrimmedCurve>(CurvePtr);
      GeomLib::SameRange(Tolerance,
                         TC->BasisCurve(),
                         FirstOnCurve,
                         LastOnCurve,
                         RequestedFirst,
                         RequestedLast,
                         NewCurvePtr);
      NewCurvePtr = new Geom2d_TrimmedCurve(NewCurvePtr, RequestedFirst, RequestedLast);
    }
    //
    //  attention a des problemes de limitation : utiliser le MEME test que dans
    //  Geom2d_TrimmedCurve::SetTrim car sinon comme on risque de relimite sur
    //  RequestedFirst et RequestedLast on aura un probleme
    //
    //
    else if (std::abs(LastOnCurve - FirstOnCurve) > Precision::PConfusion()
             || std::abs(RequestedLast + RequestedFirst) > Precision::PConfusion())
    {

      occ::handle<Geom2d_TrimmedCurve> TC =
        new Geom2d_TrimmedCurve(CurvePtr, FirstOnCurve, LastOnCurve);

      occ::handle<Geom2d_BSplineCurve> BS = Geom2dConvert::CurveToBSplineCurve(TC);
      NCollection_Array1<double>       Knots(BS->Knots());

      BSplCLib::Reparametrize(RequestedFirst, RequestedLast, Knots);

      BS->SetKnots(Knots);
      NewCurvePtr = BS;
    }
  }
  else
  { // We segment the result
    occ::handle<Geom2d_TrimmedCurve> TC;
    occ::handle<Geom2d_Curve>        aCCheck = CurvePtr;

    if (aCCheck->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      aCCheck = occ::down_cast<Geom2d_TrimmedCurve>(aCCheck)->BasisCurve();
    }

    if (aCCheck->IsPeriodic())
    {
      if (std::abs(LastOnCurve - FirstOnCurve) > Precision::PConfusion())
      {
        TC = new Geom2d_TrimmedCurve(CurvePtr, FirstOnCurve, LastOnCurve);
      }
      else
      {
        TC =
          new Geom2d_TrimmedCurve(CurvePtr, CurvePtr->FirstParameter(), CurvePtr->LastParameter());
      }
    }
    else
    {
      const double Udeb = std::max(CurvePtr->FirstParameter(), FirstOnCurve);
      const double Ufin = std::min(CurvePtr->LastParameter(), LastOnCurve);
      if (std::abs(Ufin - Udeb) > Precision::PConfusion())
      {
        TC = new Geom2d_TrimmedCurve(CurvePtr, Udeb, Ufin);
      }
      else
      {
        TC =
          new Geom2d_TrimmedCurve(CurvePtr, CurvePtr->FirstParameter(), CurvePtr->LastParameter());
      }
    }

    //
    occ::handle<Geom2d_BSplineCurve> BS = Geom2dConvert::CurveToBSplineCurve(TC);
    NCollection_Array1<double>       Knots(BS->Knots());

    BSplCLib::Reparametrize(RequestedFirst, RequestedLast, Knots);

    BS->SetKnots(Knots);
    NewCurvePtr = BS;
  }
}

//=======================================================================
// class : GeomLib_CurveOnSurfaceEvaluator
// purpose: The evaluator for the Curve 3D building
//=======================================================================

class GeomLib_CurveOnSurfaceEvaluator : public AdvApprox_EvaluatorFunction
{
public:
  GeomLib_CurveOnSurfaceEvaluator(Adaptor3d_CurveOnSurface& theCurveOnSurface,
                                  double                    theFirst,
                                  double                    theLast)
      : CurveOnSurface(theCurveOnSurface),
        FirstParam(theFirst),
        LastParam(theLast)
  {
  }

  void Evaluate(int*    Dimension,
                double  StartEnd[2],
                double* Parameter,
                int*    DerivativeRequest,
                double* Result, // [Dimension]
                int*    ErrorCode) override;

private:
  Adaptor3d_CurveOnSurface& CurveOnSurface;
  double                    FirstParam;
  double                    LastParam;

  occ::handle<Adaptor3d_Curve> TrimCurve;
};

void GeomLib_CurveOnSurfaceEvaluator::Evaluate(int*, /*Dimension*/
                                               double  DebutFin[2],
                                               double* Parameter,
                                               int*    DerivativeRequest,
                                               double* Result, // [Dimension]
                                               int*    ReturnCode)
{
  gp_Pnt Point;

  // Gestion des positionnements gauche / droite
  if ((DebutFin[0] != FirstParam) || (DebutFin[1] != LastParam))
  {
    TrimCurve  = CurveOnSurface.Trim(DebutFin[0], DebutFin[1], Precision::PConfusion());
    FirstParam = DebutFin[0];
    LastParam  = DebutFin[1];
  }

  // Positionemment
  if (*DerivativeRequest == 0)
  {
    TrimCurve->D0((*Parameter), Point);

    for (int ii = 0; ii < 3; ii++)
      Result[ii] = Point.Coord(ii + 1);
  }
  if (*DerivativeRequest == 1)
  {
    gp_Vec Vector;
    TrimCurve->D1((*Parameter), Point, Vector);
    for (int ii = 0; ii < 3; ii++)
      Result[ii] = Vector.Coord(ii + 1);
  }
  if (*DerivativeRequest == 2)
  {
    gp_Vec Vector, VecBis;
    TrimCurve->D2((*Parameter), Point, VecBis, Vector);
    for (int ii = 0; ii < 3; ii++)
      Result[ii] = Vector.Coord(ii + 1);
  }
  ReturnCode[0] = 0;
}

//=================================================================================================

void GeomLib::BuildCurve3d(const double              Tolerance,
                           Adaptor3d_CurveOnSurface& Curve,
                           const double              FirstParameter,
                           const double              LastParameter,
                           occ::handle<Geom_Curve>&  NewCurvePtr,
                           double&                   MaxDeviation,
                           double&                   AverageDeviation,
                           const GeomAbs_Shape       Continuity,
                           const int                 MaxDegree,
                           const int                 MaxSegment)

{

  MaxDeviation     = 0.0e0;
  AverageDeviation = 0.0e0;
  occ::handle<GeomAdaptor_Surface> geom_adaptor_surface_ptr(
    occ::down_cast<GeomAdaptor_Surface>(Curve.GetSurface()));
  occ::handle<Geom2dAdaptor_Curve> geom_adaptor_curve_ptr(
    occ::down_cast<Geom2dAdaptor_Curve>(Curve.GetCurve()));

  if (!geom_adaptor_curve_ptr.IsNull() && !geom_adaptor_surface_ptr.IsNull())
  {
    occ::handle<Geom_Plane>    P;
    const GeomAdaptor_Surface& geom_surface = *geom_adaptor_surface_ptr;

    occ::handle<Geom_RectangularTrimmedSurface> RT =
      occ::down_cast<Geom_RectangularTrimmedSurface>(geom_surface.Surface());
    if (RT.IsNull())
    {
      P = occ::down_cast<Geom_Plane>(geom_surface.Surface());
    }
    else
    {
      P = occ::down_cast<Geom_Plane>(RT->BasisSurface());
    }

    if (!P.IsNull())
    {
      // compute the 3d curve
      gp_Ax2                     axes         = P->Position().Ax2();
      const Geom2dAdaptor_Curve& geom2d_curve = *geom_adaptor_curve_ptr;
      NewCurvePtr                             = GeomLib::To3d(axes, geom2d_curve.Curve());
      return;
    }

    occ::handle<Adaptor2d_Curve2d> TrimmedC2D =
      geom_adaptor_curve_ptr->Trim(FirstParameter, LastParameter, Precision::PConfusion());

    bool   isU, isForward;
    double aParam;
    if (isIsoLine(TrimmedC2D, isU, aParam, isForward))
    {
      NewCurvePtr = buildC3dOnIsoLine(TrimmedC2D,
                                      geom_adaptor_surface_ptr,
                                      FirstParameter,
                                      LastParameter,
                                      Tolerance,
                                      isU,
                                      aParam,
                                      isForward);
      if (!NewCurvePtr.IsNull())
      {
        return;
      }
    }
  }

  //
  // Entree
  //
  occ::handle<NCollection_HArray1<double>> Tolerance1DPtr, Tolerance2DPtr;
  occ::handle<NCollection_HArray1<double>> Tolerance3DPtr = new NCollection_HArray1<double>(1, 1);
  Tolerance3DPtr->SetValue(1, Tolerance);

  // Recherche des discontinuitees
  int                        NbIntervalC2 = Curve.NbIntervals(GeomAbs_C2);
  NCollection_Array1<double> Param_de_decoupeC2(1, NbIntervalC2 + 1);
  Curve.Intervals(Param_de_decoupeC2, GeomAbs_C2);

  int                        NbIntervalC3 = Curve.NbIntervals(GeomAbs_C3);
  NCollection_Array1<double> Param_de_decoupeC3(1, NbIntervalC3 + 1);
  Curve.Intervals(Param_de_decoupeC3, GeomAbs_C3);

  // Note extension of the parametric range
  // To force Trim on first evaluator call
  GeomLib_CurveOnSurfaceEvaluator ev(Curve, FirstParameter - 1., LastParameter + 1.);

  // Approximation avec decoupe preferentiel
  AdvApprox_PrefAndRec      Preferentiel(Param_de_decoupeC2, Param_de_decoupeC3);
  AdvApprox_ApproxAFunction anApproximator(0,
                                           0,
                                           1,
                                           Tolerance1DPtr,
                                           Tolerance2DPtr,
                                           Tolerance3DPtr,
                                           FirstParameter,
                                           LastParameter,
                                           Continuity,
                                           MaxDegree,
                                           MaxSegment,
                                           ev,
                                           //					      CurveOnSurfaceEvaluator,
                                           Preferentiel);

  if (anApproximator.HasResult())
  {
    GeomLib_MakeCurvefromApprox aCurveBuilder(anApproximator);

    occ::handle<Geom_BSplineCurve> aCurvePtr = aCurveBuilder.Curve(1);
    // On rend les resultats de l'approx
    MaxDeviation     = anApproximator.MaxError(3, 1);
    AverageDeviation = anApproximator.AverageError(3, 1);
    NewCurvePtr      = aCurvePtr;
  }
}

//=================================================================================================

void GeomLib::AdjustExtremity(occ::handle<Geom_BoundedCurve>& Curve,
                              const gp_Pnt&                   P1,
                              const gp_Pnt&                   P2,
                              const gp_Vec&                   T1,
                              const gp_Vec&                   T2)
{
  // il faut Convertir l'entree (en preservant si possible le parametrage)
  occ::handle<Geom_BSplineCurve> aIn, aDef;
  aIn = GeomConvert::CurveToBSplineCurve(Curve, Convert_QuasiAngular);

  int                        ii, jj;
  gp_Pnt                     P;
  gp_Vec                     V, Vtan, DV;
  NCollection_Array1<gp_Pnt> PolesDef(1, 4), Coeffs(1, 4);
  NCollection_Array1<double> FK(1, 8);
  NCollection_Array1<double> Ti(1, 4);
  NCollection_Array1<int>    Contact(1, 4);

  Ti(1) = Ti(2) = aIn->FirstParameter();
  Ti(3) = Ti(4) = aIn->LastParameter();
  Contact(1) = Contact(3) = 0;
  Contact(2) = Contact(4) = 1;
  for (ii = 1; ii <= 4; ii++)
  {
    FK(ii) = aIn->FirstParameter();
    FK(ii) = aIn->LastParameter();
  }

  // Calculs des contraintes de deformations
  aIn->D1(Ti(1), P, V);
  PolesDef(1).ChangeCoord() = P1.XYZ() - P.XYZ();
  Vtan                      = T1;
  Vtan.Normalize();
  DV                        = Vtan * (Vtan * V) - V;
  PolesDef(2).ChangeCoord() = (Ti(4) - Ti(1)) * DV.XYZ();

  aIn->D1(Ti(4), P, V);
  PolesDef(3).ChangeCoord() = P2.XYZ() - P.XYZ();
  Vtan                      = T2;
  Vtan.Normalize();
  DV                        = Vtan * (Vtan * V) - V;
  PolesDef(4).ChangeCoord() = (Ti(4) - Ti(1)) * DV.XYZ();

  // Interpolation des contraintes
  math_Matrix Mat(1, 4, 1, 4);
  if (!PLib::HermiteCoefficients(0., 1., 1, 1, Mat))
    throw Standard_ConstructionError();

  for (jj = 1; jj <= 4; jj++)
  {
    gp_XYZ aux(0., 0., 0.);
    for (ii = 1; ii <= 4; ii++)
    {
      aux.SetLinearForm(Mat(ii, jj), PolesDef(ii).XYZ(), aux);
    }
    Coeffs(jj).SetXYZ(aux);
  }

  PLib::CoefficientsPoles(Coeffs, PLib::NoWeights(), PolesDef, PLib::NoWeights());

  // Ajout de la deformation
  NCollection_Array1<double> K(1, 2);
  NCollection_Array1<int>    M(1, 2);
  K(1) = Ti(1);
  K(2) = Ti(4);
  M.Init(4);

  aDef = new (Geom_BSplineCurve)(PolesDef, K, M, 3);
  if (aIn->Degree() < 3)
    aIn->IncreaseDegree(3);
  else
    aDef->IncreaseDegree(aIn->Degree());

  for (ii = 2; ii < aIn->NbKnots(); ii++)
  {
    aDef->InsertKnot(aIn->Knot(ii), aIn->Multiplicity(ii));
  }

  if (aDef->NbPoles() != aIn->NbPoles())
    throw Standard_ConstructionError("Inconsistent poles's number");

  for (ii = 1; ii <= aDef->NbPoles(); ii++)
  {
    P = aIn->Pole(ii);
    P.ChangeCoord() += aDef->Pole(ii).XYZ();
    aIn->SetPole(ii, P);
  }
  Curve = aIn;
}

//=================================================================================================

void GeomLib::ExtendCurveToPoint(occ::handle<Geom_BoundedCurve>& Curve,
                                 const gp_Pnt&                   Point,
                                 const int                       Continuity,
                                 const bool                      After)
{
  if (Continuity < 1 || Continuity > 3)
    return;
  int         size = Continuity + 2;
  double      Ubord, Tol = 1.e-6;
  math_Matrix MatCoefs(1, size, 1, size);
  double      Lambda, L1;
  int         ii, jj;
  gp_Vec      d1, d2, d3;
  gp_Pnt      p0;
  // il faut Convertir l'entree (en preservant si possible le parametrage)
  GeomConvert_CompCurveToBSplineCurve Concat(Curve, Convert_QuasiAngular);

  // Les contraintes de constructions
  NCollection_Array1<gp_XYZ> Cont(1, size);
  if (After)
  {
    Ubord = Curve->LastParameter();
  }
  else
  {
    Ubord = Curve->FirstParameter();
  }
  PLib::HermiteCoefficients(0,
                            1, // Les Bornes
                            Continuity,
                            0, // Les Ordres de contraintes
                            MatCoefs);

  Curve->D3(Ubord, p0, d1, d2, d3);
  if (!After)
  { // Inversion du parametrage
    d1 *= -1;
    d3 *= -1;
  }

  L1 = p0.Distance(Point);
  if (L1 > Tol)
  {
    // Lambda is the ratio to apply to the derivative of the curve
    // to obtain the derivative of the extension (arbitrarily fixed to the
    // length of the segment end of curve - target point.
    // We try to have on the extension the average velocity that we
    // have on the curve.
    gp_Vec daux;
    gp_Pnt pp;
    double f = Curve->FirstParameter(), t, dt, norm;
    dt       = (Curve->LastParameter() - f) / 9;
    norm     = d1.Magnitude();
    for (ii = 1, t = f + dt; ii <= 8; ii++, t += dt)
    {
      Curve->D1(t, pp, daux);
      norm += daux.Magnitude();
    }
    norm /= 9;
    dt = d1.Magnitude() / norm;
    if ((dt < 1.5) && (dt > 0.75))
    { // Le bord est dans la moyenne on le garde
      Lambda = ((double)1) / std::max(d1.Magnitude() / L1, Tol);
    }
    else
    {
      Lambda = ((double)1) / std::max(norm / L1, Tol);
    }
  }
  else
  {
    return; // Pas d'extension
  }

  // Optimisation du Lambda
  math_Matrix Cons(1, 3, 1, size);
  Cons(1, 1)    = p0.X();
  Cons(2, 1)    = p0.Y();
  Cons(3, 1)    = p0.Z();
  Cons(1, 2)    = d1.X();
  Cons(2, 2)    = d1.Y();
  Cons(3, 2)    = d1.Z();
  Cons(1, size) = Point.X();
  Cons(2, size) = Point.Y();
  Cons(3, size) = Point.Z();
  if (Continuity >= 2)
  {
    Cons(1, 3) = d2.X();
    Cons(2, 3) = d2.Y();
    Cons(3, 3) = d2.Z();
  }
  if (Continuity >= 3)
  {
    Cons(1, 4) = d3.X();
    Cons(2, 4) = d3.Y();
    Cons(3, 4) = d3.Z();
  }
  ComputeLambda(Cons, MatCoefs, L1, Lambda);

  // Construction dans la Base Polynomiale
  Cont(1) = p0.XYZ();
  Cont(2) = d1.XYZ() * Lambda;
  if (Continuity >= 2)
    Cont(3) = d2.XYZ() * std::pow(Lambda, 2);
  if (Continuity >= 3)
    Cont(4) = d3.XYZ() * std::pow(Lambda, 3);
  Cont(size) = Point.XYZ();

  NCollection_Array1<gp_Pnt> ExtrapPoles(1, size);
  NCollection_Array1<gp_Pnt> ExtraCoeffs(1, size);

  gp_Pnt PNull(0., 0., 0.);
  ExtraCoeffs.Init(PNull);
  for (ii = 1; ii <= size; ii++)
  {
    for (jj = 1; jj <= size; jj++)
    {
      ExtraCoeffs(jj).ChangeCoord() += MatCoefs(ii, jj) * Cont(ii);
    }
  }

  // Convertion Dans la Base de Bernstein
  PLib::CoefficientsPoles(ExtraCoeffs, PLib::NoWeights(), ExtrapPoles, PLib::NoWeights());

  occ::handle<Geom_BezierCurve> Bezier = new (Geom_BezierCurve)(ExtrapPoles);

  double dist = ExtrapPoles(1).Distance(p0);
  bool   Ok;
  Tol += dist;

  // Concatenation
  Ok = Concat.Add(Bezier, Tol, After);
  if (!Ok)
    throw Standard_ConstructionError("ExtendCurveToPoint");

  Curve = Concat.BSplineCurve();
}

//=======================================================================
// function : ExtendKPart
// purpose  : Extension par longueur des surfaces cannonique
//=======================================================================
static bool ExtendKPart(occ::handle<Geom_RectangularTrimmedSurface>& Surface,
                        const double                                 Length,
                        const bool                                   InU,
                        const bool                                   After)
{

  if (Surface.IsNull())
    return false;

  bool                      Ok = true;
  double                    Uf, Ul, Vf, Vl;
  occ::handle<Geom_Surface> Support = Surface->BasisSurface();
  GeomAbs_SurfaceType       Type;

  Surface->Bounds(Uf, Ul, Vf, Vl);
  GeomAdaptor_Surface AS(Surface);
  Type = AS.GetType();

  if (InU)
  {
    switch (Type)
    {
      case GeomAbs_Plane: {
        if (After)
          Ul += Length;
        else
          Uf -= Length;
        Surface = new (Geom_RectangularTrimmedSurface)(Support, Uf, Ul, Vf, Vl);
        break;
      }

      default:
        Ok = false;
    }
  }
  else
  {
    switch (Type)
    {
      case GeomAbs_Plane:
      case GeomAbs_Cylinder:
      case GeomAbs_SurfaceOfExtrusion: {
        if (After)
          Vl += Length;
        else
          Vf -= Length;
        Surface = new (Geom_RectangularTrimmedSurface)(Support, Uf, Ul, Vf, Vl);
        break;
      }
      default:
        Ok = false;
    }
  }

  return Ok;
}

//=================================================================================================

void GeomLib::ExtendSurfByLength(occ::handle<Geom_BoundedSurface>& Surface,
                                 const double                      Length,
                                 const int                         Continuity,
                                 const bool                        InU,
                                 const bool                        After)
{
  if (Continuity < 0 || Continuity > 3)
    return;
  int Cont = Continuity;

  // Kpart ?
  occ::handle<Geom_RectangularTrimmedSurface> TS =
    occ::down_cast<Geom_RectangularTrimmedSurface>(Surface);
  if (ExtendKPart(TS, Length, InU, After))
  {
    Surface = TS;
    return;
  }

  //  format BSplineSurface avec un degre suffisant pour la continuite voulue
  occ::handle<Geom_BSplineSurface> BS = occ::down_cast<Geom_BSplineSurface>(Surface);
  if (BS.IsNull())
  {
    // BS = GeomConvert::SurfaceToBSplineSurface(Surface);
    constexpr double                 Tol   = Precision::Confusion(); // 1.e-4;
    GeomAbs_Shape                    UCont = GeomAbs_C1, VCont = GeomAbs_C1;
    int                              degU = 14, degV = 14;
    int                              nmax    = 16;
    int                              thePrec = 1;
    const occ::handle<Geom_Surface>& aSurf   = Surface; // to resolve ambiguity
    GeomConvert_ApproxSurface        theApprox(aSurf, Tol, UCont, VCont, degU, degV, nmax, thePrec);
    if (theApprox.HasResult())
      BS = theApprox.Surface();
    else
      BS = GeomConvert::SurfaceToBSplineSurface(Surface);
  }
  if (InU && (BS->UDegree() < Continuity + 1))
    BS->IncreaseDegree(Continuity + 1, BS->VDegree());
  if (!InU && (BS->VDegree() < Continuity + 1))
    BS->IncreaseDegree(BS->UDegree(), Continuity + 1);

  // if BS was periodic in the extension direction, it will no longer be
  if ((InU && (BS->IsUPeriodic())) || (!InU && (BS->IsVPeriodic())))
  {
    double U0, U1, V0, V1;
    BS->Bounds(U0, U1, V0, V1);
    BS->Segment(U0, U1, V0, V1);
  }

  // IFV Fix OCC bug 0022694 - wrong result extrapolating rational surfaces
  //   bool rational = ( InU && BS->IsURational() )
  //                                   || ( !InU && BS->IsVRational() ) ;
  bool             rational = (BS->IsURational() || BS->IsVRational());
  constexpr double EpsW     = 10 * Precision::PConfusion();
  int              gap      = 3;
  if (rational)
    gap++;

  int                                      Cdeg = 0, Cdim = 0, NbP = 0, Ksize = 0, Psize = 1;
  int                                      ii, jj, ipole, Kount;
  double                                   Tbord, lambmin = Length;
  double*                                  Padr = nullptr;
  bool                                     Ok;
  occ::handle<NCollection_HArray1<double>> FKnots, Point, lambda, Tgte, Poles;

  for (Kount = 0, Ok = false; Kount <= 2 && !Ok; Kount++)
  {
    //  transformation de la surface en une BSpline non rationnelle a une variable
    //  de degre UDegree ou VDegree et de dimension 3 ou 4 x NbVpoles ou NbUpoles
    //  le nombre de poles egal a NbUpoles ou NbVpoles
    //  ATTENTION : dans le cas rationnel, un point de coordonnees (x,y,z)
    //              et de poids w devient un point de coordonnees (wx, wy, wz, w )

    if (InU)
    {
      Cdeg = BS->UDegree();
      NbP  = BS->NbUPoles();
      Cdim = BS->NbVPoles() * gap;
    }
    else
    {
      Cdeg = BS->VDegree();
      NbP  = BS->NbVPoles();
      Cdim = BS->NbUPoles() * gap;
    }

    //  the flat knots
    Ksize  = NbP + Cdeg + 1;
    if (InU)
      FKnots = new NCollection_HArray1<double>(BS->UKnotSequence());
    else
      FKnots = new NCollection_HArray1<double>(BS->VKnotSequence());

    //  the parameter of the connection knot
    if (After)
      Tbord = FKnots->Value(FKnots->Upper() - Cdeg);
    else
      Tbord = FKnots->Value(FKnots->Lower() + Cdeg);

    //  the poles
    Psize = Cdim * NbP;
    Poles = new (NCollection_HArray1<double>)(1, Psize);

    if (InU)
    {
      for (ii = 1, ipole = 1; ii <= NbP; ii++)
      {
        for (jj = 1; jj <= BS->NbVPoles(); jj++)
        {
          Poles->SetValue(ipole, BS->Pole(ii, jj).X());
          Poles->SetValue(ipole + 1, BS->Pole(ii, jj).Y());
          Poles->SetValue(ipole + 2, BS->Pole(ii, jj).Z());
          if (rational)
            Poles->SetValue(ipole + 3, BS->Weight(ii, jj));
          ipole += gap;
        }
      }
    }
    else
    {
      for (jj = 1, ipole = 1; jj <= NbP; jj++)
      {
        for (ii = 1; ii <= BS->NbUPoles(); ii++)
        {
          Poles->SetValue(ipole, BS->Pole(ii, jj).X());
          Poles->SetValue(ipole + 1, BS->Pole(ii, jj).Y());
          Poles->SetValue(ipole + 2, BS->Pole(ii, jj).Z());
          if (rational)
            Poles->SetValue(ipole + 3, BS->Weight(ii, jj));
          ipole += gap;
        }
      }
    }
    Padr = (double*)&Poles->ChangeValue(1);

    //  calculation of the connection point and tangent
    Point  = new (NCollection_HArray1<double>)(1, Cdim);
    Tgte   = new (NCollection_HArray1<double>)(1, Cdim);
    lambda = new (NCollection_HArray1<double>)(1, Cdim);

    bool periodic_flag = false;
    int  extrap_mode[2], derivative_request = std::max(Continuity, 1);
    extrap_mode[0] = extrap_mode[1] = Cdeg;
    NCollection_Array1<double> Result(1, Cdim * (derivative_request + 1));

    NCollection_Array1<double>& tgte  = Tgte->ChangeArray1();
    NCollection_Array1<double>& point = Point->ChangeArray1();
    NCollection_Array1<double>& lamb  = lambda->ChangeArray1();

    double* Radr = (double*)&Result(1);

    BSplCLib::Eval(Tbord,
                   periodic_flag,
                   derivative_request,
                   extrap_mode[0],
                   Cdeg,
                   FKnots->Array1(),
                   Cdim,
                   *Padr,
                   *Radr);
    Ok = true;
    for (ii = 1; ii <= Cdim; ii++)
    {
      point(ii) = Result(ii);
      tgte(ii)  = Result(ii + Cdim);
    }

    //  calculation of the constraint to reach

    gp_Vec CurT, OldT;

    double NTgte, val, Tgtol = 1.e-12, OldN = 0.0;
    if (rational)
    {
      for (ii = gap; ii <= Cdim; ii += gap)
      {
        tgte(ii) = 0.;
      }
      for (ii = gap; ii <= Cdim; ii += gap)
      {
        CurT.SetCoord(tgte(ii - 3), tgte(ii - 2), tgte(ii - 1));
        NTgte = CurT.Magnitude();
        if (NTgte > Tgtol)
        {
          val = Length / NTgte;
          // Attention to cases where the segment given by the poles
          // is opposite to the direction of the derivative
          // Example: Certain portions of torus.
          if ((OldN > Tgtol) && (CurT.Angle(OldT) > 2))
          {
            Ok = false;
          }

          lamb(ii - 1) = lamb(ii - 2) = lamb(ii - 3) = val;
          lamb(ii)                                   = 0.;
          lambmin                                    = std::min(lambmin, val);
        }
        else
        {
          lamb(ii - 1) = lamb(ii - 2) = lamb(ii - 3) = 0.;
          lamb(ii)                                   = 0.;
        }
        OldT = CurT;
        OldN = NTgte;
      }
    }
    else
    {
      for (ii = gap; ii <= Cdim; ii += gap)
      {
        CurT.SetCoord(tgte(ii - 2), tgte(ii - 1), tgte(ii));
        NTgte = CurT.Magnitude();
        if (NTgte > Tgtol)
        {
          val = Length / NTgte;
          // Attention to cases where the segment given by the poles
          // is opposite to the direction of the derivative
          // Example: Certain portions of torus.
          if ((OldN > Tgtol) && (CurT.Angle(OldT) > 2))
          {
            Ok = false;
          }
          lamb(ii) = lamb(ii - 1) = lamb(ii - 2) = val;
          lambmin                                = std::min(lambmin, val);
        }
        else
        {
          lamb(ii) = lamb(ii - 1) = lamb(ii - 2) = 0.;
        }
        OldT = CurT;
        OldN = NTgte;
      }
    }
    if (!Ok && Kount < 2)
    {
      // We increase the degree of the border iso to bring the poles of the surface closer
      // And we retry
      if (InU)
        BS->IncreaseDegree(BS->UDegree(), BS->VDegree() + 2);
      else
        BS->IncreaseDegree(BS->UDegree() + 2, BS->VDegree());
    }
  }

  NCollection_Array1<double> ConstraintPoint(1, Cdim);
  if (After)
  {
    for (ii = 1; ii <= Cdim; ii++)
    {
      ConstraintPoint(ii) = Point->Value(ii) + lambda->Value(ii) * Tgte->Value(ii);
    }
  }
  else
  {
    for (ii = 1; ii <= Cdim; ii++)
    {
      ConstraintPoint(ii) = Point->Value(ii) - lambda->Value(ii) * Tgte->Value(ii);
    }
  }

  //  special case of rational
  if (rational)
  {
    for (ipole = 1; ipole <= Psize; ipole += gap)
    {
      Poles->ChangeValue(ipole) *= Poles->Value(ipole + 3);
      Poles->ChangeValue(ipole + 1) *= Poles->Value(ipole + 3);
      Poles->ChangeValue(ipole + 2) *= Poles->Value(ipole + 3);
    }
    for (ii = 1; ii <= Cdim; ii += gap)
    {
      ConstraintPoint(ii) *= ConstraintPoint(ii + 3);
      ConstraintPoint(ii + 1) *= ConstraintPoint(ii + 3);
      ConstraintPoint(ii + 2) *= ConstraintPoint(ii + 3);
    }
  }

  //  arrays needed for the extension
  int                        Ksize2 = Ksize + Cdeg, NbPoles, NbKnots = 0;
  NCollection_Array1<double> FK(1, Ksize2);
  double*                    FKRadr = &FK(1);

  int                                      Psize2 = Psize + Cdeg * Cdim;
  NCollection_Array1<double>               PRes(1, Psize2);
  double*                                  PRadr = &PRes(1);
  double                                   ww;
  bool                                     ExtOk = false;
  occ::handle<NCollection_HArray2<gp_Pnt>> NewPoles;
  occ::handle<NCollection_HArray2<double>> NewWeights;

  for (Kount = 1; Kount <= 5 && !ExtOk; Kount++)
  {
    //  extension
    BSplCLib::TangExtendToConstraint(FKnots->Array1(),
                                     lambmin,
                                     NbP,
                                     *Padr,
                                     Cdim,
                                     Cdeg,
                                     ConstraintPoint,
                                     Cont,
                                     After,
                                     NbPoles,
                                     NbKnots,
                                     *FKRadr,
                                     *PRadr);

    //  recopie des poles du resultat sous forme de points 3D et de poids
    int NU, NV, indice;
    if (InU)
    {
      NU = NbPoles;
      NV = BS->NbVPoles();
    }
    else
    {
      NU = BS->NbUPoles();
      NV = NbPoles;
    }

    NewPoles                         = new (NCollection_HArray2<gp_Pnt>)(1, NU, 1, NV);
    NCollection_Array2<gp_Pnt>& NewP = NewPoles->ChangeArray2();
    NewWeights                       = new (NCollection_HArray2<double>)(1, NU, 1, NV);
    NCollection_Array2<double>& NewW = NewWeights->ChangeArray2();

    if (!rational)
      NewW.Init(1.);
    bool NullWeight = false;

    if (InU)
    {
      for (ii = 1; ii <= NU && !NullWeight; ii++)
      {
        for (jj = 1; jj <= NV && !NullWeight; jj++)
        {
          indice = 1 + (ii - 1) * Cdim + (jj - 1) * gap;
          NewP(ii, jj).SetCoord(1, PRes(indice));
          NewP(ii, jj).SetCoord(2, PRes(indice + 1));
          NewP(ii, jj).SetCoord(3, PRes(indice + 2));
          if (rational)
          {
            ww = PRes(indice + 3);
            if (std::abs(ww - 1.0) < EpsW)
              ww = 1.0;
            if (ww < EpsW)
            {
              NullWeight = true;
            }
            else
            {
              NewW(ii, jj) = ww;
              NewP(ii, jj).ChangeCoord() /= ww;
            }
          }
        }
      }
    }
    else
    {
      for (jj = 1; jj <= NV && !NullWeight; jj++)
      {
        for (ii = 1; ii <= NU && !NullWeight; ii++)
        {
          indice = 1 + (ii - 1) * gap + (jj - 1) * Cdim;
          NewP(ii, jj).SetCoord(1, PRes(indice));
          NewP(ii, jj).SetCoord(2, PRes(indice + 1));
          NewP(ii, jj).SetCoord(3, PRes(indice + 2));
          if (rational)
          {
            ww = PRes(indice + 3);
            if (std::abs(ww - 1.0) < EpsW)
              ww = 1.0;
            if (ww < EpsW)
            {
              NullWeight = true;
            }
            else
            {
              NewW(ii, jj) = ww;
              NewP(ii, jj).ChangeCoord() /= ww;
            }
          }
        }
      }
    }

    if (NullWeight)
    {
#ifdef OCCT_DEBUG
      std::cout << "Echec de l'Extension rationnelle" << std::endl;
#endif
      lambmin /= 3.;
    }
    else
    {
      ExtOk = true;
    }
  }

  // recopie des noeuds plats sous forme de noeuds avec leurs multiplicites
  // calcul des degres du resultat
  int Usize = BS->NbUKnots(), Vsize = BS->NbVKnots(), UDeg, VDeg;
  if (InU)
    Usize++;
  else
    Vsize++;
  NCollection_Array1<double> UKnots(1, Usize);
  NCollection_Array1<double> VKnots(1, Vsize);
  NCollection_Array1<int>    UMults(1, Usize);
  NCollection_Array1<int>    VMults(1, Vsize);
  NCollection_Array1<double> FKRes(1, NbKnots);

  for (ii = 1; ii <= NbKnots; ii++)
    FKRes(ii) = FK(ii);

  if (InU)
  {
    BSplCLib::Knots(FKRes, UKnots, UMults);
    UDeg          = Cdeg;
    UMults(Usize) = UDeg + 1; // Petite verrue utile quand la continuite
                              // n'est pas ok.
    VKnots = BS->VKnots();
    VMults = BS->VMultiplicities();
    VDeg   = BS->VDegree();
  }
  else
  {
    BSplCLib::Knots(FKRes, VKnots, VMults);
    VDeg          = Cdeg;
    VMults(Vsize) = VDeg + 1;
    UKnots = BS->UKnots();
    UMults = BS->UMultiplicities();
    UDeg   = BS->UDegree();
  }

  //  construction de la surface BSpline resultat
  occ::handle<Geom_BSplineSurface> Res = new (Geom_BSplineSurface)(NewPoles->Array2(),
                                                                   NewWeights->Array2(),
                                                                   UKnots,
                                                                   VKnots,
                                                                   UMults,
                                                                   VMults,
                                                                   UDeg,
                                                                   VDeg,
                                                                   BS->IsUPeriodic(),
                                                                   BS->IsVPeriodic());
  Surface                              = Res;
}

//=================================================================================================

void GeomLib::Inertia(const NCollection_Array1<gp_Pnt>& Points,
                      gp_Pnt&                           Bary,
                      gp_Dir&                           XDir,
                      gp_Dir&                           YDir,
                      double&                           Xgap,
                      double&                           Ygap,
                      double&                           Zgap)
{
  gp_XYZ GB(0., 0., 0.), Diff;
  //  gp_Vec A,B,C,D;

  int i, nb = Points.Length();
  GB.SetCoord(0., 0., 0.);
  for (i = 1; i <= nb; i++)
    GB += Points(i).XYZ();

  GB /= nb;

  math_Matrix M(1, 3, 1, 3);
  M.Init(0.);
  for (i = 1; i <= nb; i++)
  {
    Diff.SetLinearForm(-1, Points(i).XYZ(), GB);
    M(1, 1) += Diff.X() * Diff.X();
    M(2, 2) += Diff.Y() * Diff.Y();
    M(3, 3) += Diff.Z() * Diff.Z();
    M(1, 2) += Diff.X() * Diff.Y();
    M(1, 3) += Diff.X() * Diff.Z();
    M(2, 3) += Diff.Y() * Diff.Z();
  }

  M(2, 1) = M(1, 2);
  M(3, 1) = M(1, 3);
  M(3, 2) = M(2, 3);

  M /= nb;

  math_Jacobi J(M);
  if (!J.IsDone())
  {
#ifdef OCCT_DEBUG
    std::cout << "Erreur dans Jacobbi" << std::endl;
    M.Dump(std::cout);
#endif
  }

  double n1, n2, n3;

  n1 = J.Value(1);
  n2 = J.Value(2);
  n3 = J.Value(3);

  double r1 = std::min(std::min(n1, n2), n3), r2;
  int    m1, m2, m3;
  if (r1 == n1)
  {
    m1 = 1;
    r2 = std::min(n2, n3);
    if (r2 == n2)
    {
      m2 = 2;
      m3 = 3;
    }
    else
    {
      m2 = 3;
      m3 = 2;
    }
  }
  else
  {
    if (r1 == n2)
    {
      m1 = 2;
      r2 = std::min(n1, n3);
      if (r2 == n1)
      {
        m2 = 1;
        m3 = 3;
      }
      else
      {
        m2 = 3;
        m3 = 1;
      }
    }
    else
    {
      m1 = 3;
      r2 = std::min(n1, n2);
      if (r2 == n1)
      {
        m2 = 1;
        m3 = 2;
      }
      else
      {
        m2 = 2;
        m3 = 1;
      }
    }
  }

  math_Vector V2(1, 3), V3(1, 3);
  J.Vector(m2, V2);
  J.Vector(m3, V3);

  Bary.SetXYZ(GB);
  XDir.SetCoord(V3(1), V3(2), V3(3));
  YDir.SetCoord(V2(1), V2(2), V2(3));

  Zgap = sqrt(std::abs(J.Value(m1)));
  Ygap = sqrt(std::abs(J.Value(m2)));
  Xgap = sqrt(std::abs(J.Value(m3)));
}

//=================================================================================================

void GeomLib::AxeOfInertia(const NCollection_Array1<gp_Pnt>& Points,
                           gp_Ax2&                           Axe,
                           bool&                             IsSingular,
                           const double                      Tol)
{
  gp_Pnt Bary;
  gp_Dir OX, OY, OZ;
  double gx, gy, gz;

  GeomLib::Inertia(Points, Bary, OX, OY, gx, gy, gz);

  if (gy * Points.Length() <= Tol)
  {
    gp_Ax2 axe(Bary, OX);
    OY         = axe.XDirection();
    IsSingular = true;
  }
  else
  {
    IsSingular = false;
  }

  OZ = OX ^ OY;
  gp_Ax2 TheAxe(Bary, OZ, OX);
  Axe = TheAxe;
}

//=======================================================================
// function : CanBeTreated
// purpose  : indicates if the surface can be treated(if the conditions are
//           filled) and need to be treated(if the surface hasn't been yet
//           treated or if the surface is rational and non periodic)
//=======================================================================

static bool CanBeTreated(occ::handle<Geom_BSplineSurface>& BSurf)

{
  int    i;
  double lambda; // proportionnality coefficient
  bool   AlreadyTreated = true;

  if (!BSurf->IsURational() || (BSurf->IsUPeriodic()))
    return false;
  else
  {
    lambda = (BSurf->Weight(1, 1) / BSurf->Weight(BSurf->NbUPoles(), 1));
    // clang-format off
   for (i=1;i<=BSurf->NbVPoles();i++)      //test of the proportionnality of the denominator on the boundaries
      // clang-format on
      if ((BSurf->Weight(1, i) / (lambda * BSurf->Weight(BSurf->NbUPoles(), i))
           < (1 - Precision::Confusion()))
          || (BSurf->Weight(1, i) / (lambda * BSurf->Weight(BSurf->NbUPoles(), i))
              > (1 + Precision::Confusion())))
        return false;
    i = 1;
    while ((AlreadyTreated) && (i <= BSurf->NbVPoles()))
    { // tests if the surface has already been treated
      if (((BSurf->Weight(1, i) / (BSurf->Weight(2, i))) < (1 - Precision::Confusion()))
          || ((BSurf->Weight(1, i) / (BSurf->Weight(2, i))) > (1 + Precision::Confusion()))
          || ((BSurf->Weight(BSurf->NbUPoles() - 1, i) / (BSurf->Weight(BSurf->NbUPoles(), i)))
              < (1 - Precision::Confusion()))
          || ((BSurf->Weight(BSurf->NbUPoles() - 1, i) / (BSurf->Weight(BSurf->NbUPoles(), i)))
              > (1 + Precision::Confusion())))
        AlreadyTreated = false;
      i++;
    }
    if (AlreadyTreated)
      return false;
  }
  return true;
}

//=======================================================================
// class   : law_evaluator
// purpose : useful to estimate the value of a function of 2 variables
//=======================================================================

class law_evaluator : public BSplSLib_EvaluatorFunction
{

public:
  law_evaluator(const GeomLib_DenominatorMultiplierPtr theDenominatorPtr)
      : myDenominator(theDenominatorPtr)
  {
  }

  void Evaluate(const int    theDerivativeRequest,
                const double theUParameter,
                const double theVParameter,
                double&      theResult,
                int&         theErrorCode) const override
  {
    if ((myDenominator != nullptr) && (theDerivativeRequest == 0))
    {
      theResult    = myDenominator->Value(theUParameter, theVParameter);
      theErrorCode = 0;
    }
    else
    {
      theErrorCode = 1;
    }
  }

private:
  GeomLib_DenominatorMultiplierPtr myDenominator;
};

//=======================================================================
// function : CheckIfKnotExists
// purpose  : true if the knot already exists in the knot sequence
//=======================================================================

static bool CheckIfKnotExists(const NCollection_Array1<double>& surface_knots, const double knot)

{
  int i;
  for (i = 1; i <= surface_knots.Length(); i++)
    if ((surface_knots(i) - Precision::Confusion() <= knot)
        && (surface_knots(i) + Precision::Confusion() >= knot))
      return true;
  return false;
}

//=======================================================================
// function : AddAKnot
// purpose  : add a knot and its multiplicity to the knot sequence. This knot
//           will be C2 and the degree is increased of deltasurface_degree
//=======================================================================

static void AddAKnot(const NCollection_Array1<double>&         knots,
                     const NCollection_Array1<int>&            mults,
                     const double                              knotinserted,
                     const int                                 deltasurface_degree,
                     const int                                 finalsurfacedegree,
                     occ::handle<NCollection_HArray1<double>>& newknots,
                     occ::handle<NCollection_HArray1<int>>&    newmults)

{
  int i;

  newknots = new NCollection_HArray1<double>(1, knots.Length() + 1);
  newmults = new NCollection_HArray1<int>(1, knots.Length() + 1);
  i        = 1;
  while (knots(i) < knotinserted)
  {
    newknots->SetValue(i, knots(i));
    newmults->SetValue(i, mults(i) + deltasurface_degree);
    i++;
  }
  newknots->SetValue(i, knotinserted); // insertion of the new knot
  newmults->SetValue(i, finalsurfacedegree - 2);
  i++;
  while (i <= newknots->Length())
  {
    newknots->SetValue(i, knots(i - 1));
    newmults->SetValue(i, mults(i - 1) + deltasurface_degree);
    i++;
  }
}

//=======================================================================
// function : Sort
// purpose  : give the new flat knots(u or v) of the surface
//=======================================================================

static void BuildFlatKnot(const NCollection_Array1<double>&         surface_knots,
                          const NCollection_Array1<int>&            surface_mults,
                          const int                                 deltasurface_degree,
                          const int                                 finalsurface_degree,
                          const double                              knotmin,
                          const double                              knotmax,
                          occ::handle<NCollection_HArray1<double>>& ResultKnots,
                          occ::handle<NCollection_HArray1<int>>&    ResultMults)

{
  int i;

  if (CheckIfKnotExists(surface_knots, knotmin) && CheckIfKnotExists(surface_knots, knotmax))
  {
    ResultKnots = new NCollection_HArray1<double>(1, surface_knots.Length());
    ResultMults = new NCollection_HArray1<int>(1, surface_knots.Length());
    for (i = 1; i <= surface_knots.Length(); i++)
    {
      ResultKnots->SetValue(i, surface_knots(i));
      ResultMults->SetValue(i, surface_mults(i) + deltasurface_degree);
    }
  }
  else
  {
    if ((CheckIfKnotExists(surface_knots, knotmin)) && (!CheckIfKnotExists(surface_knots, knotmax)))
      AddAKnot(surface_knots,
               surface_mults,
               knotmax,
               deltasurface_degree,
               finalsurface_degree,
               ResultKnots,
               ResultMults);
    else
    {
      if ((!CheckIfKnotExists(surface_knots, knotmin))
          && (CheckIfKnotExists(surface_knots, knotmax)))
        AddAKnot(surface_knots,
                 surface_mults,
                 knotmin,
                 deltasurface_degree,
                 finalsurface_degree,
                 ResultKnots,
                 ResultMults);
      else
      {
        if ((!CheckIfKnotExists(surface_knots, knotmin))
            && (!CheckIfKnotExists(surface_knots, knotmax)) && (knotmin == knotmax))
        {
          AddAKnot(surface_knots,
                   surface_mults,
                   knotmin,
                   deltasurface_degree,
                   finalsurface_degree,
                   ResultKnots,
                   ResultMults);
        }
        else
        {
          occ::handle<NCollection_HArray1<double>> IntermedKnots;
          occ::handle<NCollection_HArray1<int>>    IntermedMults;
          AddAKnot(surface_knots,
                   surface_mults,
                   knotmin,
                   deltasurface_degree,
                   finalsurface_degree,
                   IntermedKnots,
                   IntermedMults);
          AddAKnot(IntermedKnots->ChangeArray1(),
                   IntermedMults->ChangeArray1(),
                   knotmax,
                   0,
                   finalsurface_degree,
                   ResultKnots,
                   ResultMults);
        }
      }
    }
  }
}

//=======================================================================
// function : FunctionMultiply
// purpose  : multiply the surface BSurf by a(u,v) (law_evaluator) on its
//           numerator and denominator
//=======================================================================

static void FunctionMultiply(occ::handle<Geom_BSplineSurface>& BSurf,
                             const double                      knotmin,
                             const double                      knotmax)

{
  const NCollection_Array1<double>& surface_u_knots  = BSurf->UKnots();
  const NCollection_Array1<int>&    surface_u_mults  = BSurf->UMultiplicities();
  const NCollection_Array1<double>& surface_v_knots  = BSurf->VKnots();
  const NCollection_Array1<int>&    surface_v_mults  = BSurf->VMultiplicities();
  const NCollection_Array2<gp_Pnt>& surface_poles    = BSurf->Poles();
  const NCollection_Array2<double>* surface_weights  = BSurf->Weights();
  int                        i, j, k, status, new_num_u_poles, new_num_v_poles, length = 0;
  occ::handle<NCollection_HArray1<double>> newuknots, newvknots;
  occ::handle<NCollection_HArray1<int>>    newumults, newvmults;

  NCollection_Array1<double>               Knots(1, 2);
  NCollection_Array1<int>                  Mults(1, 2);
  occ::handle<NCollection_HArray1<double>> NewKnots;
  occ::handle<NCollection_HArray1<int>>    NewMults;

  Knots(1) = 0;
  Knots(2) = 1;
  Mults(1) = 4;
  Mults(2) = 4;
  BuildFlatKnot(Knots, Mults, 0, 3, knotmin, knotmax, NewKnots, NewMults);

  for (i = 1; i <= NewMults->Length(); i++)
    length += NewMults->Value(i);
  NCollection_Array1<double> FlatKnots(1, length);
  BSplCLib::KnotSequence(NewKnots->ChangeArray1(), NewMults->ChangeArray1(), FlatKnots);

  GeomLib_DenominatorMultiplier aDenominator(BSurf, FlatKnots);

  BuildFlatKnot(surface_u_knots,
                surface_u_mults,
                3,
                BSurf->UDegree() + 3,
                knotmin,
                knotmax,
                newuknots,
                newumults);
  BuildFlatKnot(surface_v_knots,
                surface_v_mults,
                BSurf->VDegree(),
                2 * (BSurf->VDegree()),
                1.0,
                0.0,
                newvknots,
                newvmults);
  length = 0;
  for (i = 1; i <= newumults->Length(); i++)
    length += newumults->Value(i);
  new_num_u_poles = (length - BSurf->UDegree() - 3 - 1);
  NCollection_Array1<double> newuflatknots(1, length);
  length = 0;
  for (i = 1; i <= newvmults->Length(); i++)
    length += newvmults->Value(i);
  new_num_v_poles = (length - 2 * BSurf->VDegree() - 1);
  NCollection_Array1<double> newvflatknots(1, length);

  NCollection_Array2<gp_Pnt> NewNumerator(1, new_num_u_poles, 1, new_num_v_poles);
  NCollection_Array2<double> NewDenominator(1, new_num_u_poles, 1, new_num_v_poles);

  BSplCLib::KnotSequence(newuknots->ChangeArray1(), newumults->ChangeArray1(), newuflatknots);
  BSplCLib::KnotSequence(newvknots->ChangeArray1(), newvmults->ChangeArray1(), newvflatknots);
  // POP pour WNT
  law_evaluator ev(&aDenominator);
  // BSplSLib::FunctionMultiply(law_evaluator,               //multiplication
  BSplSLib::FunctionMultiply(ev, // multiplication
                             BSurf->UDegree(),
                             BSurf->VDegree(),
                             surface_u_knots,
                             surface_v_knots,
                             &surface_u_mults,
                             &surface_v_mults,
                             surface_poles,
                             surface_weights,
                             newuflatknots,
                             newvflatknots,
                             BSurf->UDegree() + 3,
                             2 * (BSurf->VDegree()),
                             NewNumerator,
                             NewDenominator,
                             status);
  if (status != 0)
    throw Standard_ConstructionError("GeomLib Multiplication Error");
  for (i = 1; i <= new_num_u_poles; i++)
  {
    for (j = 1; j <= new_num_v_poles; j++)
    {
      for (k = 1; k <= 3; k++)
      {
        NewNumerator(i, j).SetCoord(k, NewNumerator(i, j).Coord(k) / NewDenominator(i, j));
      }
    }
  }
  BSurf = new Geom_BSplineSurface(NewNumerator,
                                  NewDenominator,
                                  newuknots->ChangeArray1(),
                                  newvknots->ChangeArray1(),
                                  newumults->ChangeArray1(),
                                  newvmults->ChangeArray1(),
                                  BSurf->UDegree() + 3,
                                  2 * (BSurf->VDegree()));
}

//=======================================================================
// function : CancelDenominatorDerivative1D
// purpose  : cancel the denominator derivative in one direction
//=======================================================================

static void CancelDenominatorDerivative1D(occ::handle<Geom_BSplineSurface>& BSurf)

{
  int    i, j;
  double uknotmin = 1.0, uknotmax = 0.0, x, y, startu_value, endu_value;

  startu_value = BSurf->UKnot(1);
  endu_value   = BSurf->UKnot(BSurf->NbUKnots());
  NCollection_Array1<double> BSurf_u_knots(BSurf->UKnots());
  BSplCLib::Reparametrize(0.0, 1.0, BSurf_u_knots);
  BSurf->SetUKnots(BSurf_u_knots); // reparametrisation of the surface
  occ::handle<Geom_BSplineCurve> BCurve;
  NCollection_Array1<double>     BCurveWeights(1, BSurf->NbUPoles());
  NCollection_Array1<gp_Pnt>     BCurvePoles(1, BSurf->NbUPoles());

  if (CanBeTreated(BSurf))
  {
    const NCollection_Array1<double>& BCurveKnots = BSurf->UKnots();
    const NCollection_Array1<int>&    BCurveMults = BSurf->UMultiplicities();
    for (i = 1; i <= BSurf->NbVPoles(); i++)
    { // loop on each pole function
      x = 1.0;
      y = 0.0;
      for (j = 1; j <= BSurf->NbUPoles(); j++)
      {
        BCurveWeights(j) = BSurf->Weight(j, i);
        BCurvePoles(j)   = BSurf->Pole(j, i);
      }
      BCurve = new Geom_BSplineCurve(BCurvePoles, // building of a pole function
                                     BCurveWeights,
                                     BCurveKnots,
                                     BCurveMults,
                                     BSurf->UDegree());
      Hermit::Solutionbis(BCurve, x, y, Precision::Confusion(), Precision::Confusion());
      if (x < uknotmin)
        uknotmin = x; // uknotmin,uknotmax:extremal knots
      if ((x != 1.0) && (x > uknotmax))
        uknotmax = x;
      if ((y != 0.0) && (y < uknotmin))
        uknotmin = y;
      if (y > uknotmax)
        uknotmax = y;
    }

    FunctionMultiply(BSurf, uknotmin, uknotmax); // multiplication

    BSurf_u_knots = BSurf->UKnots();
    BSplCLib::Reparametrize(startu_value, endu_value, BSurf_u_knots);
    BSurf->SetUKnots(BSurf_u_knots);
  }
}

//=================================================================================================

void GeomLib::CancelDenominatorDerivative(occ::handle<Geom_BSplineSurface>& BSurf,
                                          const bool                        udirection,
                                          const bool                        vdirection)

{
  if (udirection && !vdirection)
    CancelDenominatorDerivative1D(BSurf);
  else
  {
    if (!udirection && vdirection)
    {
      BSurf->ExchangeUV();
      CancelDenominatorDerivative1D(BSurf);
      BSurf->ExchangeUV();
    }
    else
    {
      if (udirection && vdirection)
      { // optimize the treatment
        if (BSurf->UDegree() <= BSurf->VDegree())
        {
          CancelDenominatorDerivative1D(BSurf);
          BSurf->ExchangeUV();
          CancelDenominatorDerivative1D(BSurf);
          BSurf->ExchangeUV();
        }
        else
        {
          BSurf->ExchangeUV();
          CancelDenominatorDerivative1D(BSurf);
          BSurf->ExchangeUV();
          CancelDenominatorDerivative1D(BSurf);
        }
      }
    }
  }
}

//=================================================================================================

int GeomLib::NormEstim(const occ::handle<Geom_Surface>& theSurf,
                       const gp_Pnt2d&                  theUV,
                       const double                     theTol,
                       gp_Dir&                          theNorm)
{
  const double aTol2 = Square(theTol);

  gp_Vec DU, DV;
  gp_Pnt aDummyPnt;
  theSurf->D1(theUV.X(), theUV.Y(), aDummyPnt, DU, DV);

  const double MDU = DU.SquareMagnitude(), MDV = DV.SquareMagnitude();
  if (MDU >= aTol2 && MDV >= aTol2)
  {
    gp_Vec aNorm = DU ^ DV;
    double aMagn = aNorm.SquareMagnitude();
    if (aMagn < aTol2)
    {
      return 3;
    }

    theNorm.SetXYZ(aNorm.XYZ());
    return 0;
  }

  gp_Vec             D2U, D2V, D2UV;
  bool               isDone = false;
  CSLib_NormalStatus aStatus;
  gp_Dir             aNormal;

  theSurf->D2(theUV.X(), theUV.Y(), aDummyPnt, DU, DV, D2U, D2V, D2UV);
  CSLib::Normal(DU, DV, D2U, D2V, D2UV, theTol, isDone, aStatus, aNormal);
  if (!isDone)
  {
    // computation is impossible
    return aStatus == CSLib_D1NIsNull ? 2 : 3;
  }

  double Umin, Umax, Vmin, Vmax;
  double step = 1.0e-5;
  double eps  = 1.0e-16;
  double sign = -1.0;
  theSurf->Bounds(Umin, Umax, Vmin, Vmax);

  // check for cone apex singularity point
  if ((theUV.Y() > Vmin + step) && (theUV.Y() < Vmax - step))
  {
    gp_Dir aNormal1, aNormal2;
    double aConeSingularityAngleEps = 1.0e-4;
    theSurf->D1(theUV.X(), theUV.Y() - sign * step, aDummyPnt, DU, DV);
    if ((DU.XYZ().SquareModulus() > eps) && (DV.XYZ().SquareModulus() > eps))
    {
      aNormal1 = DU ^ DV;
      theSurf->D1(theUV.X(), theUV.Y() + sign * step, aDummyPnt, DU, DV);
      if ((DU.XYZ().SquareModulus() > eps) && (DV.XYZ().SquareModulus() > eps))
      {
        aNormal2 = DU ^ DV;
        if (aNormal1.IsOpposite(aNormal2, aConeSingularityAngleEps))
        {
          return 2;
        }
      }
    }
  }

  // Along V
  if (MDU < aTol2 && MDV >= aTol2)
  {
    if ((Vmax - theUV.Y()) > (theUV.Y() - Vmin))
    {
      sign = 1.0;
    }

    theSurf->D1(theUV.X(), theUV.Y() + sign * step, aDummyPnt, DU, DV);
    gp_Vec Norm = DU ^ DV;
    if (Norm.SquareMagnitude() < eps)
    {
      double sign1 = -1.0;
      if ((Umax - theUV.X()) > (theUV.X() - Umin))
      {
        sign1 = 1.0;
      }
      theSurf->D1(theUV.X() + sign1 * step, theUV.Y() + sign * step, aDummyPnt, DU, DV);
      Norm = DU ^ DV;
    }
    if (Norm.SquareMagnitude() >= eps && Norm.Dot(aNormal) < 0.0)
    {
      aNormal.Reverse();
    }
  }

  // Along U
  if (MDV < aTol2 && MDU >= aTol2)
  {
    if ((Umax - theUV.X()) > (theUV.X() - Umin))
    {
      sign = 1.0;
    }

    theSurf->D1(theUV.X() + sign * step, theUV.Y(), aDummyPnt, DU, DV);
    gp_Vec Norm = DU ^ DV;
    if (Norm.SquareMagnitude() < eps)
    {
      double sign1 = -1.0;
      if ((Vmax - theUV.Y()) > (theUV.Y() - Vmin))
      {
        sign1 = 1.0;
      }

      theSurf->D1(theUV.X() + sign * step, theUV.Y() + sign1 * step, aDummyPnt, DU, DV);
      Norm = DU ^ DV;
    }
    if (Norm.SquareMagnitude() >= eps && Norm.Dot(aNormal) < 0.0)
    {
      aNormal.Reverse();
    }
  }

  // quasysingular
  if (aStatus == CSLib_D1NuIsNull || aStatus == CSLib_D1NvIsNull
      || aStatus == CSLib_D1NuIsParallelD1Nv)
  {
    theNorm.SetXYZ(aNormal.XYZ());
    return 1;
  }

  return aStatus == CSLib_InfinityOfSolutions ? 2 : 3;
}

//=================================================================================================

void GeomLib::IsClosed(const occ::handle<Geom_Surface>& S,
                       const double                     Tol,
                       bool&                            isUClosed,
                       bool&                            isVClosed)
{
  isUClosed = false;
  isVClosed = false;
  //
  GeomAdaptor_Surface aGAS(S);
  GeomAbs_SurfaceType aSType = aGAS.GetType();
  //
  double u1, u2, v1, v2;
  u1 = aGAS.FirstUParameter();
  u2 = aGAS.LastUParameter();
  v1 = aGAS.FirstVParameter();
  v2 = aGAS.LastVParameter();
  //
  double Tol2 = Tol * Tol;
  switch (aSType)
  {
    case GeomAbs_Plane: {
      return;
    }
    case GeomAbs_SurfaceOfExtrusion: {
      if (Precision::IsInfinite(u1) || Precision::IsInfinite(u2))
      {
        // not closed
        return;
      }
    }
      [[fallthrough]];
    case GeomAbs_Cylinder: {
      if (Precision::IsInfinite(v1))
        v1 = 0.;
      gp_Pnt p1 = aGAS.Value(u1, v1);
      gp_Pnt p2 = aGAS.Value(u2, v1);
      isUClosed = p1.SquareDistance(p2) <= Tol2;
      return;
    }
    case GeomAbs_Cone: {
      // find v with maximal distance from axis
      if (!(Precision::IsInfinite(v1) || Precision::IsInfinite(v2)))
      {
        gp_Cone aCone  = aGAS.Cone();
        gp_Pnt  anApex = aCone.Apex();
        gp_Pnt  P1     = aGAS.Value(u1, v1);
        gp_Pnt  P2     = aGAS.Value(u1, v2);
        if (P2.SquareDistance(anApex) > P1.SquareDistance(anApex))
        {
          v1 = v2;
        }
      }
      else
      {
        v1 = 0.;
      }
      gp_Pnt p1 = aGAS.Value(u1, v1);
      gp_Pnt p2 = aGAS.Value(u2, v1);
      isUClosed = p1.SquareDistance(p2) <= Tol2;
      return;
    }
    case GeomAbs_Sphere: {
      // find v with maximal distance from axis
      if (v1 * v2 <= 0.)
      {
        v1 = 0.;
      }
      else
      {
        if (v1 < 0.)
        {
          v1 = v2;
        }
      }
      gp_Pnt p1 = aGAS.Value(u1, v1);
      gp_Pnt p2 = aGAS.Value(u2, v1);
      isUClosed = p1.SquareDistance(p2) <= Tol2;
      return;
    }
    case GeomAbs_Torus: {
      double ures = aGAS.UResolution(Tol);
      double vres = aGAS.VResolution(Tol);
      //
      isUClosed = (u2 - u1) >= aGAS.UPeriod() - ures;
      isVClosed = (v2 - v1) >= aGAS.VPeriod() - vres;
      return;
    }
    case GeomAbs_BSplineSurface: {
      occ::handle<Geom_BSplineSurface> aBSpl = aGAS.BSpline();
      isUClosed                              = GeomLib::IsBSplUClosed(aBSpl, u1, u2, Tol);
      isVClosed                              = GeomLib::IsBSplVClosed(aBSpl, v1, v2, Tol);
      return;
    }
    case GeomAbs_BezierSurface: {
      occ::handle<Geom_BezierSurface> aBz = aGAS.Bezier();
      isUClosed                           = GeomLib::IsBzUClosed(aBz, u1, u2, Tol);
      isVClosed                           = GeomLib::IsBzVClosed(aBz, v1, v2, Tol);
      return;
    }
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_OffsetSurface:
    case GeomAbs_OtherSurface: {
      int nbp = 23;
      if (Precision::IsInfinite(v1))
      {
        v1 = std::copysign(1., v1);
      }
      if (Precision::IsInfinite(v2))
      {
        v2 = std::copysign(1., v2);
      }
      //
      if (aSType == GeomAbs_OffsetSurface || aSType == GeomAbs_OtherSurface)
      {
        if (Precision::IsInfinite(u1))
        {
          u1 = std::copysign(1., u1);
        }
        if (Precision::IsInfinite(u2))
        {
          u2 = std::copysign(1., u2);
        }
      }
      isUClosed  = true;
      double dt  = (v2 - v1) / (nbp - 1);
      double res = std::max(aGAS.UResolution(Tol), Precision::PConfusion());
      if (dt <= res)
      {
        nbp = RealToInt((v2 - v1) / (2. * res)) + 1;
        nbp = std::max(nbp, 2);
        dt  = (v2 - v1) / (nbp - 1);
      }
      double t;
      int    i;
      for (i = 0; i < nbp; ++i)
      {
        t         = (i == nbp - 1 ? v2 : v1 + i * dt);
        gp_Pnt p1 = aGAS.Value(u1, t);
        gp_Pnt p2 = aGAS.Value(u2, t);
        if (p1.SquareDistance(p2) > Tol2)
        {
          isUClosed = false;
          break;
        }
      }
      //
      nbp       = 23;
      isVClosed = true;
      dt        = (u2 - u1) / (nbp - 1);
      res       = std::max(aGAS.VResolution(Tol), Precision::PConfusion());
      if (dt <= res)
      {
        nbp = RealToInt((u2 - u1) / (2. * res)) + 1;
        nbp = std::max(nbp, 2);
        dt  = (u2 - u1) / (nbp - 1);
      }
      for (i = 0; i < nbp; ++i)
      {
        t         = (i == nbp - 1 ? u2 : u1 + i * dt);
        gp_Pnt p1 = aGAS.Value(t, v1);
        gp_Pnt p2 = aGAS.Value(t, v2);
        if (p1.SquareDistance(p2) > Tol2)
        {
          isVClosed = false;
          break;
        }
      }
      return;
    }
    default: {
      return;
    }
  }
}

//=================================================================================================

bool GeomLib::IsBSplUClosed(const occ::handle<Geom_BSplineSurface>& S,
                            const double                            U1,
                            const double                            U2,
                            const double                            Tol)
{
  occ::handle<Geom_Curve> aCUF = S->UIso(U1);
  occ::handle<Geom_Curve> aCUL = S->UIso(U2);
  if (aCUF.IsNull() || aCUL.IsNull())
    return false;
  double                            Tol2 = 2. * Tol;
  occ::handle<Geom_BSplineCurve>    aBsF = occ::down_cast<Geom_BSplineCurve>(aCUF);
  occ::handle<Geom_BSplineCurve>    aBsL = occ::down_cast<Geom_BSplineCurve>(aCUL);
  const NCollection_Array1<gp_Pnt>& aPF  = aBsF->Poles();
  const NCollection_Array1<gp_Pnt>& aPL  = aBsL->Poles();
  const NCollection_Array1<double>* WF   = aBsF->Weights();
  const NCollection_Array1<double>* WL   = aBsL->Weights();
  return CompareWeightPoles(aPF, WF, aPL, WL, Tol2);
}

//=================================================================================================

bool GeomLib::IsBSplVClosed(const occ::handle<Geom_BSplineSurface>& S,
                            const double                            V1,
                            const double                            V2,
                            const double                            Tol)
{
  occ::handle<Geom_Curve> aCVF = S->VIso(V1);
  occ::handle<Geom_Curve> aCVL = S->VIso(V2);
  if (aCVF.IsNull() || aCVL.IsNull())
    return false;
  double                            Tol2 = 2. * Tol;
  occ::handle<Geom_BSplineCurve>    aBsF = occ::down_cast<Geom_BSplineCurve>(aCVF);
  occ::handle<Geom_BSplineCurve>    aBsL = occ::down_cast<Geom_BSplineCurve>(aCVL);
  const NCollection_Array1<gp_Pnt>& aPF  = aBsF->Poles();
  const NCollection_Array1<gp_Pnt>& aPL  = aBsL->Poles();
  const NCollection_Array1<double>* WF   = aBsF->Weights();
  const NCollection_Array1<double>* WL   = aBsL->Weights();
  return CompareWeightPoles(aPF, WF, aPL, WL, Tol2);
}

//=================================================================================================

bool GeomLib::IsBzUClosed(const occ::handle<Geom_BezierSurface>& S,
                          const double                           U1,
                          const double                           U2,
                          const double                           Tol)
{
  occ::handle<Geom_Curve> aCUF = S->UIso(U1);
  occ::handle<Geom_Curve> aCUL = S->UIso(U2);
  if (aCUF.IsNull() || aCUL.IsNull())
    return false;
  double                            Tol2 = 2. * Tol;
  occ::handle<Geom_BezierCurve>     aBzF = occ::down_cast<Geom_BezierCurve>(aCUF);
  occ::handle<Geom_BezierCurve>     aBzL = occ::down_cast<Geom_BezierCurve>(aCUL);
  const NCollection_Array1<gp_Pnt>& aPF  = aBzF->Poles();
  const NCollection_Array1<gp_Pnt>& aPL  = aBzL->Poles();
  //
  return CompareWeightPoles(aPF, nullptr, aPL, nullptr, Tol2);
}

//=================================================================================================

bool GeomLib::IsBzVClosed(const occ::handle<Geom_BezierSurface>& S,
                          const double                           V1,
                          const double                           V2,
                          const double                           Tol)
{
  occ::handle<Geom_Curve> aCVF = S->VIso(V1);
  occ::handle<Geom_Curve> aCVL = S->VIso(V2);
  if (aCVF.IsNull() || aCVL.IsNull())
    return false;
  double                            Tol2 = 2. * Tol;
  occ::handle<Geom_BezierCurve>     aBzF = occ::down_cast<Geom_BezierCurve>(aCVF);
  occ::handle<Geom_BezierCurve>     aBzL = occ::down_cast<Geom_BezierCurve>(aCVL);
  const NCollection_Array1<gp_Pnt>& aPF  = aBzF->Poles();
  const NCollection_Array1<gp_Pnt>& aPL  = aBzL->Poles();
  //
  return CompareWeightPoles(aPF, nullptr, aPL, nullptr, Tol2);
}

//=======================================================================
// function : CompareWeightPoles
// purpose  : Checks if thePoles1(i)*theW1(i) is equal to thePoles2(i)*theW2(i)
//            with tolerance theTol.
//           It is necessary for non-rational B-splines and Bezier curves
//            to set theW1 and theW2 addresses to zero.
//=======================================================================
static bool CompareWeightPoles(const NCollection_Array1<gp_Pnt>&       thePoles1,
                               const NCollection_Array1<double>* const theW1,
                               const NCollection_Array1<gp_Pnt>&       thePoles2,
                               const NCollection_Array1<double>* const theW2,
                               const double                            theTol)
{
  if (thePoles1.Length() != thePoles2.Length())
  {
    return false;
  }
  //
  int i = 1;
  for (i = 1; i <= thePoles1.Length(); i++)
  {
    const double aW1 = (theW1 == nullptr) ? 1.0 : theW1->Value(i);
    const double aW2 = (theW2 == nullptr) ? 1.0 : theW2->Value(i);

    gp_XYZ aPole1 = thePoles1.Value(i).XYZ() * aW1;
    gp_XYZ aPole2 = thePoles2.Value(i).XYZ() * aW2;
    if (!aPole1.IsEqual(aPole2, theTol))
      return false;
  }
  //
  return true;
}

//=================================================================================================

bool GeomLib::isIsoLine(const occ::handle<Adaptor2d_Curve2d>& theC2D,
                        bool&                                 theIsU,
                        double&                               theParam,
                        bool&                                 theIsForward)
{
  // These variables are used to check line state (vertical or horizontal).
  bool     isAppropriateType = false;
  gp_Pnt2d aLoc2d;
  gp_Dir2d aDir2d;

  // Test type.
  const GeomAbs_CurveType aType = theC2D->GetType();
  if (aType == GeomAbs_Line)
  {
    gp_Lin2d aLin2d   = theC2D->Line();
    aLoc2d            = aLin2d.Location();
    aDir2d            = aLin2d.Direction();
    isAppropriateType = true;
  }
  else if (aType == GeomAbs_BSplineCurve)
  {
    occ::handle<Geom2d_BSplineCurve> aBSpline2d = theC2D->BSpline();
    if (aBSpline2d->Degree() != 1 || aBSpline2d->NbPoles() != 2)
      return false; // Not a line or uneven parameterization.

    aLoc2d = aBSpline2d->Pole(1);

    // Vector should be non-degenerated.
    gp_Vec2d aVec2d(aBSpline2d->Pole(1), aBSpline2d->Pole(2));
    if (aVec2d.SquareMagnitude() < Precision::Confusion())
      return false; // Degenerated spline.
    aDir2d = aVec2d;

    isAppropriateType = true;
  }
  else if (aType == GeomAbs_BezierCurve)
  {
    occ::handle<Geom2d_BezierCurve> aBezier2d = theC2D->Bezier();
    if (aBezier2d->Degree() != 1 || aBezier2d->NbPoles() != 2)
      return false; // Not a line or uneven parameterization.

    aLoc2d = aBezier2d->Pole(1);

    // Vector should be non-degenerated.
    gp_Vec2d aVec2d(aBezier2d->Pole(1), aBezier2d->Pole(2));
    if (aVec2d.SquareMagnitude() < Precision::Confusion())
      return false; // Degenerated spline.
    aDir2d = aVec2d;

    isAppropriateType = true;
  }

  if (!isAppropriateType)
    return false;

  // Check line to be vertical or horizontal.
  if (aDir2d.IsParallel(gp::DX2d(), Precision::Angular()))
  {
    // Horizontal line. V = const.
    theIsU       = false;
    theParam     = aLoc2d.Y();
    theIsForward = aDir2d.Dot(gp::DX2d()) > 0.0;
    return true;
  }
  else if (aDir2d.IsParallel(gp::DY2d(), Precision::Angular()))
  {
    // Vertical line. U = const.
    theIsU       = true;
    theParam     = aLoc2d.X();
    theIsForward = aDir2d.Dot(gp::DY2d()) > 0.0;
    return true;
  }

  return false;
}

//=================================================================================================

occ::handle<Geom_Curve> GeomLib::buildC3dOnIsoLine(const occ::handle<Adaptor2d_Curve2d>& theC2D,
                                                   const occ::handle<Adaptor3d_Surface>& theSurf,
                                                   const double                          theFirst,
                                                   const double                          theLast,
                                                   const double theTolerance,
                                                   const bool   theIsU,
                                                   const double theParam,
                                                   const bool   theIsForward)
{
  // Convert adapter to the appropriate type.
  occ::handle<GeomAdaptor_Surface> aGeomAdapter = occ::down_cast<GeomAdaptor_Surface>(theSurf);
  if (aGeomAdapter.IsNull())
    return occ::handle<Geom_Curve>();

  if (theSurf->GetType() == GeomAbs_Sphere)
    return occ::handle<Geom_Curve>();

  // Extract isoline
  occ::handle<Geom_Surface> aSurf = aGeomAdapter->Surface();
  occ::handle<Geom_Curve>   aC3d;

  gp_Pnt2d aF2d = theC2D->Value(theC2D->FirstParameter());
  gp_Pnt2d aL2d = theC2D->Value(theC2D->LastParameter());

  bool   isToTrim = true;
  double U1, U2, V1, V2;
  aSurf->Bounds(U1, U2, V1, V2);

  if (theIsU)
  {
    double aV1Param = std::min(aF2d.Y(), aL2d.Y());
    double aV2Param = std::max(aF2d.Y(), aL2d.Y());
    if (aV2Param < V1 - theTolerance || aV1Param > V2 + theTolerance)
    {
      return occ::handle<Geom_Curve>();
    }
    else if (Precision::IsInfinite(V1) || Precision::IsInfinite(V2))
    {
      if (std::abs(aV2Param - aV1Param) < Precision::PConfusion())
      {
        return occ::handle<Geom_Curve>();
      }
      aSurf    = new Geom_RectangularTrimmedSurface(aSurf, U1, U2, aV1Param, aV2Param);
      isToTrim = false;
    }
    else
    {
      aV1Param = std::max(aV1Param, V1);
      aV2Param = std::min(aV2Param, V2);
      if (std::abs(aV2Param - aV1Param) < Precision::PConfusion())
      {
        return occ::handle<Geom_Curve>();
      }
    }
    aC3d = aSurf->UIso(theParam);
    if (isToTrim)
      aC3d = new Geom_TrimmedCurve(aC3d, aV1Param, aV2Param);
  }
  else
  {
    double aU1Param = std::min(aF2d.X(), aL2d.X());
    double aU2Param = std::max(aF2d.X(), aL2d.X());
    if (aU2Param < U1 - theTolerance || aU1Param > U2 + theTolerance)
    {
      return occ::handle<Geom_Curve>();
    }
    else if (Precision::IsInfinite(U1) || Precision::IsInfinite(U2))
    {
      if (std::abs(aU2Param - aU1Param) < Precision::PConfusion())
      {
        return occ::handle<Geom_Curve>();
      }
      aSurf    = new Geom_RectangularTrimmedSurface(aSurf, aU1Param, aU2Param, V1, V2);
      isToTrim = false;
    }
    else
    {
      aU1Param = std::max(aU1Param, U1);
      aU2Param = std::min(aU2Param, U2);
      if (std::abs(aU2Param - aU1Param) < Precision::PConfusion())
      {
        return occ::handle<Geom_Curve>();
      }
    }
    aC3d = aSurf->VIso(theParam);
    if (isToTrim)
      aC3d = new Geom_TrimmedCurve(aC3d, aU1Param, aU2Param);
  }

  // Convert arbitrary curve type to the b-spline.
  occ::handle<Geom_BSplineCurve> aCurve3d =
    GeomConvert::CurveToBSplineCurve(aC3d, Convert_QuasiAngular);
  if (!theIsForward)
    aCurve3d->Reverse();

  // Rebuild parameterization for the 3d curve to have the same parameterization with
  // a two-dimensional curve.
  NCollection_Array1<double> aKnots = aCurve3d->Knots();
  BSplCLib::Reparametrize(theC2D->FirstParameter(), theC2D->LastParameter(), aKnots);
  aCurve3d->SetKnots(aKnots);

  // Evaluate error.
  double anError3d = 0.0;

  const double aParF  = theFirst;
  const double aParL  = theLast;
  const int    aNbPnt = 23;
  for (int anIdx = 0; anIdx <= aNbPnt; ++anIdx)
  {
    const double aPar = aParF + ((aParL - aParF) * anIdx) / aNbPnt;

    const gp_Pnt2d aPnt2d = theC2D->Value(aPar);

    const gp_Pnt aPntC3D = aCurve3d->Value(aPar);
    const gp_Pnt aPntC2D = theSurf->Value(aPnt2d.X(), aPnt2d.Y());

    const double aSqDeviation = aPntC3D.SquareDistance(aPntC2D);
    anError3d                 = std::max(aSqDeviation, anError3d);
  }

  anError3d = std::sqrt(anError3d);

  // Target tolerance is not obtained. This situation happens for isolines on the sphere.
  // OCCT is unable to convert it keeping original parameterization, while the geometric
  // form of the result is entirely identical. In that case, it is better to utilize
  // a general-purpose approach.
  if (anError3d > theTolerance)
    return occ::handle<Geom_Curve>();

  return aCurve3d;
}
