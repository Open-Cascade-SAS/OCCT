// Created on: 1997-12-19
// Created by: Roman BORISOV /Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#include <GeomFill_CorrectedFrenet.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Plane.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomFill_Frenet.hxx>
#include <GeomFill_SnglrFunc.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <GeomLib.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Law_BSpFunc.hxx>
#include <Law_BSpline.hxx>
#include <Law_Composite.hxx>
#include <Law_Constant.hxx>
#include <Law_Function.hxx>
#include <Law_Interpolate.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>

#include <cstdio>
IMPLEMENT_STANDARD_RTTIEXT(GeomFill_CorrectedFrenet, GeomFill_TrihedronLaw)

// Patch
#ifdef OCCT_DEBUG
static bool Affich = 0;
#endif

static double ComputeTorsion(const double Param, const occ::handle<Adaptor3d_Curve>& aCurve)
{
  double Torsion;

  gp_Pnt aPoint;
  gp_Vec DC1, DC2, DC3;
  aCurve->D3(Param, aPoint, DC1, DC2, DC3);
  gp_Vec DC1crossDC2      = DC1 ^ DC2;
  double Norm_DC1crossDC2 = DC1crossDC2.Magnitude();

  double DC1DC2DC3 = DC1crossDC2 * DC3; // mixed product

  double Tol                    = gp::Resolution();
  double SquareNorm_DC1crossDC2 = Norm_DC1crossDC2 * Norm_DC1crossDC2;
  if (SquareNorm_DC1crossDC2 <= Tol)
    Torsion = 0.;
  else
    Torsion = DC1DC2DC3 / SquareNorm_DC1crossDC2;

  return Torsion;
}

//===============================================================
// Function : smoothlaw
// Purpose : to smooth a law : Reduce the number of knots
//===============================================================
static void smoothlaw(occ::handle<Law_BSpline>&                       Law,
                      const occ::handle<NCollection_HArray1<double>>& Points,
                      const occ::handle<NCollection_HArray1<double>>& Param,
                      const double                                    Tol)
{
  double                   tol, d;
  int                      ii, Nbk;
  bool                     B, Ok;
  occ::handle<Law_BSpline> BS = Law->Copy();

  Nbk = BS->NbKnots();
  tol = Tol / 10;
  Ok  = false;

  for (ii = Nbk - 1; ii > 1; ii--)
  { // Une premiere passe tolerance serres
    B = BS->RemoveKnot(ii, 0, tol);
    if (B)
      Ok = true;
  }

  if (Ok)
  { // controle
    tol = 0.;
    for (ii = 1; ii <= Param->Length() && Ok; ii++)
    {
      d = std::abs(BS->Value(Param->Value(ii)) - Points->Value(ii));
      if (d > tol)
        tol = d;
      Ok = (tol <= Tol);
    }
    if (Ok)
      tol = (Tol - tol) / 2;
    else
    {
#ifdef OCCT_DEBUG
      std::cout << "smooth law echec" << std::endl;
#endif
      return; // Echec
    }
  }
  else
  {
    tol = Tol / 2;
  }

  if (Ok)
    Law = BS;

  Ok  = false; // Une deuxieme passe tolerance desserre
  Nbk = BS->NbKnots();
  for (ii = Nbk - 1; ii > 1; ii--)
  {
    B = BS->RemoveKnot(ii, 0, tol);
    if (B)
      Ok = true;
  }

  if (Ok)
  { // controle
    tol = 0.;
    for (ii = 1; ii <= Param->Length() && Ok; ii++)
    {
      d = std::abs(BS->Value(Param->Value(ii)) - Points->Value(ii));
      if (d > tol)
        tol = d;
      Ok = (tol <= Tol);
    }
    if (!Ok)
    {
#ifdef OCCT_DEBUG
      std::cout << "smooth law echec" << std::endl;
#endif
    }
  }
  if (Ok)
    Law = BS;

#ifdef OCCT_DEBUG
  if (Affich)
  {
    std::cout << "Knots Law : " << std::endl;
    for (ii = 1; ii <= BS->NbKnots(); ii++)
    {
      std::cout << ii << " : " << BS->Knot(ii) << std::endl;
    }
  }
#endif
}

//=================================================================================================

static bool FindPlane(const occ::handle<Adaptor3d_Curve>& theC, occ::handle<Geom_Plane>& theP)
{
  bool                                     found = true;
  occ::handle<NCollection_HArray1<gp_Pnt>> TabP;

  switch (theC->GetType())
  {

    case GeomAbs_Line: {
      found = false;
    }
    break;

    case GeomAbs_Circle:
      theP = new Geom_Plane(gp_Ax3(theC->Circle().Position()));
      break;

    case GeomAbs_Ellipse:
      theP = new Geom_Plane(gp_Ax3(theC->Ellipse().Position()));
      break;

    case GeomAbs_Hyperbola:
      theP = new Geom_Plane(gp_Ax3(theC->Hyperbola().Position()));
      break;

    case GeomAbs_Parabola:
      theP = new Geom_Plane(gp_Ax3(theC->Parabola().Position()));
      break;

    case GeomAbs_BezierCurve: {
      occ::handle<Geom_BezierCurve> GC  = theC->Bezier();
      int                           nbp = GC->NbPoles();
      if (nbp < 2)
        found = false;
      else if (nbp == 2)
      {
        found = false;
      }
      else
      {
        TabP = new NCollection_HArray1<gp_Pnt>(GC->Poles());
      }
    }
    break;

    case GeomAbs_BSplineCurve: {
      occ::handle<Geom_BSplineCurve> GC  = theC->BSpline();
      int                            nbp = GC->NbPoles();
      if (nbp < 2)
        found = false;
      else if (nbp == 2)
      {
        found = false;
      }
      else
      {
        TabP = new NCollection_HArray1<gp_Pnt>(GC->Poles());
      }
    }
    break;

    default: { // On utilise un echantillonage
      int    nbp = 15 + theC->NbIntervals(GeomAbs_C3);
      double f, l, t, inv;
      int    ii;
      f    = theC->FirstParameter();
      l    = theC->LastParameter();
      inv  = 1. / (nbp - 1);
      TabP = new (NCollection_HArray1<gp_Pnt>)(1, nbp);
      for (ii = 1; ii <= nbp; ii++)
      {
        t = (f * (nbp - ii) + l * (ii - 1));
        t *= inv;
        TabP->SetValue(ii, theC->Value(t));
      }
    }
  }

  if (!TabP.IsNull())
  { // Recherche d'un plan moyen et controle
    bool   issingular;
    gp_Ax2 inertia;
    GeomLib::AxeOfInertia(TabP->Array1(), inertia, issingular);
    if (issingular)
    {
      found = false;
    }
    else
    {
      theP = new Geom_Plane(inertia);
    }
    if (found)
    {
      // control = Controle(TabP->Array1(), P,  myTolerance);
      //	bool isOnPlane;
      double a, b, c, d, dist;
      int    ii;
      theP->Coefficients(a, b, c, d);
      for (ii = 1; ii <= TabP->Length() && found; ii++)
      {
        const gp_XYZ& xyz = TabP->Value(ii).XYZ();
        dist              = a * xyz.X() + b * xyz.Y() + c * xyz.Z() + d;
        found             = (std::abs(dist) <= Precision::Confusion());
      }
      return found;
    }
  }

  return found;
}

//=================================================================================================

GeomFill_CorrectedFrenet::GeomFill_CorrectedFrenet()
    : isFrenet(false)
{
  frenet          = new GeomFill_Frenet();
  myForEvaluation = false;
}

//=================================================================================================

GeomFill_CorrectedFrenet::GeomFill_CorrectedFrenet(const bool ForEvaluation)
    : isFrenet(false)
{
  frenet          = new GeomFill_Frenet();
  myForEvaluation = ForEvaluation;
}

occ::handle<GeomFill_TrihedronLaw> GeomFill_CorrectedFrenet::Copy() const
{
  occ::handle<GeomFill_CorrectedFrenet> copy = new (GeomFill_CorrectedFrenet)();
  if (!myCurve.IsNull())
    copy->SetCurve(myCurve);
  return copy;
}

bool GeomFill_CorrectedFrenet::SetCurve(const occ::handle<Adaptor3d_Curve>& C)
{
  GeomFill_TrihedronLaw::SetCurve(C);
  if (!C.IsNull())
  {
    frenet->SetCurve(C);

    GeomAbs_CurveType type;
    type = C->GetType();
    switch (type)
    {
      case GeomAbs_Circle:
      case GeomAbs_Ellipse:
      case GeomAbs_Hyperbola:
      case GeomAbs_Parabola:
      case GeomAbs_Line: {
        // No probleme isFrenet
        isFrenet = true;
        break;
      }
      default: {
        // We have to search singularities
        isFrenet = true;
        Init();
      }
    }
  }
  return isFrenet;
}

//===============================================================
// Function : Init
// Purpose : Compute angle's law
//===============================================================
void GeomFill_CorrectedFrenet::Init()
{
  EvolAroundT                    = new Law_Composite();
  int                        NbI = frenet->NbIntervals(GeomAbs_C0), i;
  NCollection_Array1<double> T(1, NbI + 1);
  frenet->Intervals(T, GeomAbs_C0);
  occ::handle<Law_Function> Func;
  // OCC78
  NCollection_Sequence<double> SeqPoles, SeqAngle;
  NCollection_Sequence<gp_Vec> SeqTangent, SeqNormal;

  gp_Vec Tangent, Normal, BN;
  frenet->D0(myTrimmed->FirstParameter(), Tangent, Normal, BN);
  int NbStep;
  //  double StartAng = 0, AvStep, Step, t;
  double StartAng = 0, AvStep, Step;

  NbStep = 10;
  AvStep = (myTrimmed->LastParameter() - myTrimmed->FirstParameter()) / NbStep;
  for (i = 1; i <= NbI; i++)
  {
    NbStep = std::max(int((T(i + 1) - T(i)) / AvStep), 3);
    Step   = (T(i + 1) - T(i)) / NbStep;
    if (!InitInterval(T(i),
                      T(i + 1),
                      Step,
                      StartAng,
                      Tangent,
                      Normal,
                      AT,
                      AN,
                      Func,
                      SeqPoles,
                      SeqAngle,
                      SeqTangent,
                      SeqNormal))
    {
      if (isFrenet)
        isFrenet = false;
    }
    occ::down_cast<Law_Composite>(EvolAroundT)->ChangeLaws().Append(Func);
  }
  if (myTrimmed->IsPeriodic())
    occ::down_cast<Law_Composite>(EvolAroundT)->SetPeriodic();

  TLaw = EvolAroundT;
  // OCC78
  int iEnd = SeqPoles.Length();
  if (iEnd != 0)
  {
    HArrPoles   = new NCollection_HArray1<double>(1, iEnd);
    HArrAngle   = new NCollection_HArray1<double>(1, iEnd);
    HArrTangent = new NCollection_HArray1<gp_Vec>(1, iEnd);
    HArrNormal  = new NCollection_HArray1<gp_Vec>(1, iEnd);
    for (i = 1; i <= iEnd; i++)
    {
      HArrPoles->ChangeValue(i)   = SeqPoles(i);
      HArrAngle->ChangeValue(i)   = SeqAngle(i);
      HArrTangent->ChangeValue(i) = SeqTangent(i);
      HArrNormal->ChangeValue(i)  = SeqNormal(i);
    };
  }
}

//===============================================================
// Function : InitInterval
// Purpose : Compute the angle law on a span
//===============================================================
bool GeomFill_CorrectedFrenet::InitInterval(const double                  First,
                                            const double                  Last,
                                            const double                  Step,
                                            double&                       startAng,
                                            gp_Vec&                       prevTangent,
                                            gp_Vec&                       prevNormal,
                                            gp_Vec&                       aT,
                                            gp_Vec&                       aN,
                                            occ::handle<Law_Function>&    FuncInt,
                                            NCollection_Sequence<double>& SeqPoles,
                                            NCollection_Sequence<double>& SeqAngle,
                                            NCollection_Sequence<gp_Vec>& SeqTangent,
                                            NCollection_Sequence<gp_Vec>& SeqNormal) const
{
  gp_Vec                       Tangent, Normal, BN, cross;
  NCollection_Sequence<double> parameters;
  NCollection_Sequence<double> EvolAT;
  double                       Param  = First;
  bool                         isZero = true, isConst = true;
  gp_Pnt                       PonC;
  gp_Vec                       D1;

  frenet->SetInterval(First, Last); // To have right evaluation at bounds
  GeomFill_SnglrFunc CS(myCurve);
  Bnd_Box            aCurveBoundBox;
  BndLib_Add3dCurve::Add(CS, First, Last, 1.e-2, aCurveBoundBox);
  const double LengthMin = aCurveBoundBox.GetGap() * 1.e-4;

  aT = gp_Vec(0, 0, 0);
  aN = gp_Vec(0, 0, 0);

  double angleAT  = 0.;
  double currStep = Step;

  occ::handle<Geom_Plane> aPlane;
  bool                    isPlanar = false;
  if (!myForEvaluation)
  {
    isPlanar = FindPlane(myCurve, aPlane);
  }

  int          i         = 1;
  double       currParam = Param;
  const double DLast     = Last - Precision::PConfusion();

  while (Param < Last)
  {
    if (currParam > DLast)
    {
      if (std::abs(DLast - Param) < Precision::SquareConfusion())
      {
        Param = currParam;
      }

      currStep  = DLast - Param;
      currParam = Last;
    }
    if (isPlanar)
    {
      currParam = Last;
    }

    frenet->D0(currParam, Tangent, Normal, BN);
    if (prevTangent.Angle(Tangent) < M_PI / 3 || i == 1)
    {
      parameters.Append(currParam);
      // OCC78
      SeqPoles.Append(Param);
      SeqAngle.Append(i > 1 ? EvolAT(i - 1) : startAng);
      SeqTangent.Append(prevTangent);
      SeqNormal.Append(prevNormal);
      angleAT = CalcAngleAT(Tangent, Normal, prevTangent, prevNormal);

      if (isConst && i > 1)
        if (std::abs(angleAT) > Precision::PConfusion())
          isConst = false;

      angleAT += (i > 1) ? EvolAT(i - 1) : startAng;
      EvolAT.Append(angleAT);
      prevNormal = Normal;

      if (isZero)
        if (std::abs(angleAT) > Precision::PConfusion())
          isZero = false;

      aT += Tangent;
      cross = Tangent.Crossed(Normal);
      aN.SetLinearForm(std::sin(angleAT),
                       cross,
                       1 - std::cos(angleAT),
                       Tangent.Crossed(cross),
                       Normal + aN);
      prevTangent = Tangent;
      Param       = currParam;
      i++;

      // Evaluate the Next step
      CS.D1(Param, PonC, D1);
      double L    = std::max(PonC.XYZ().Modulus() / 2, LengthMin);
      double norm = D1.Magnitude();
      if (norm < Precision::Confusion())
      {
        norm = Precision::Confusion();
      }
      currStep = L / norm;
      if (currStep > Step)
      {
        currStep = Step; // default value
      }
    }
    else
    {
      currStep /= 2; // Step too long !
    }

    currParam = Param + currStep;
  }

  if (!isPlanar)
  {
    aT /= parameters.Length() - 1;
    aN /= parameters.Length() - 1;
  }
  startAng = angleAT;

  // Interpolation
  if (isConst || isPlanar)
  {
    FuncInt = new Law_Constant();
    occ::down_cast<Law_Constant>(FuncInt)->Set(angleAT, First, Last);
  }

  else
  {
    int                                      Length = parameters.Length();
    occ::handle<NCollection_HArray1<double>> pararr = new NCollection_HArray1<double>(1, Length);
    occ::handle<NCollection_HArray1<double>> angleATarr =
      new NCollection_HArray1<double>(1, Length);

    for (int aParamIndex = 1; aParamIndex <= Length; ++aParamIndex)
    {
      pararr->ChangeValue(aParamIndex)     = parameters(aParamIndex);
      angleATarr->ChangeValue(aParamIndex) = EvolAT(aParamIndex);
    }

    Law_Interpolate lawAT(angleATarr, pararr, false, Precision::PConfusion());
    lawAT.Perform();
    occ::handle<Law_BSpline> BS = lawAT.Curve();
    smoothlaw(BS, angleATarr, pararr, 0.1);

    FuncInt = new Law_BSpFunc(BS, First, Last);
  }
  return isZero;
}

//===============================================================
// Function : CalcAngleAT (OCC78)
// Purpose : Calculate angle of rotation of trihedron normal and its derivatives relative
//           at any position on his curve
//===============================================================
double GeomFill_CorrectedFrenet::CalcAngleAT(const gp_Vec& Tangent,
                                             const gp_Vec& Normal,
                                             const gp_Vec& prevTangent,
                                             const gp_Vec& prevNormal) const
{
  double angle;
  gp_Vec Normal_rot, cross;
  angle = Tangent.Angle(prevTangent);
  if (std::abs(angle) > Precision::Angular() && std::abs(angle) < M_PI - Precision::Angular())
  {
    cross      = Tangent.Crossed(prevTangent).Normalized();
    Normal_rot = Normal + sin(angle) * cross.Crossed(Normal)
                 + (1 - cos(angle)) * cross.Crossed(cross.Crossed(Normal));
  }
  else
    Normal_rot = Normal;
  double angleAT = Normal_rot.Angle(prevNormal);
  if (angleAT > Precision::Angular() && M_PI - angleAT > Precision::Angular())
    if (Normal_rot.Crossed(prevNormal).IsOpposite(prevTangent, Precision::Angular()))
      angleAT = -angleAT;
  return angleAT;
}

//===============================================================
// Function : ... (OCC78)
// Purpose : This family of functions produce conversion of angle utility
//===============================================================
static double corr2PI_PI(double Ang)
{
  return Ang = (Ang < M_PI ? Ang : Ang - 2 * M_PI);
}

static double diffAng(double A, double Ao)
{
  double dA = (A - Ao) - std::floor((A - Ao) / 2.0 / M_PI) * 2.0 * M_PI;
  return dA = dA >= 0 ? corr2PI_PI(dA) : -corr2PI_PI(-dA);
}

//===============================================================
// Function : CalcAngleAT (OCC78)
// Purpose : Calculate angle of rotation of trihedron normal and its derivatives relative
//           at any position on his curve
//===============================================================
double GeomFill_CorrectedFrenet::GetAngleAT(const double Param) const
{
  // Search index of low margin from poles of TLaw by bisection method
  int iB = 1, iE = HArrPoles->Length(), iC = (iE + iB) / 2;
  if (Param == HArrPoles->Value(iB))
    return TLaw->Value(Param);
  if (Param > HArrPoles->Value(iE))
    iC = iE;
  if (iC < iE)
  {
    while (HArrPoles->Value(iC) > Param || Param > HArrPoles->Value(iC + 1))
    {
      if (HArrPoles->Value(iC) < Param)
        iB = iC;
      else
        iE = iC;
      iC = (iE + iB) / 2;
    };
    if (HArrPoles->Value(iC) == Param || Param == HArrPoles->Value(iC + 1))
      return TLaw->Value(Param);
  };
  //  Calculate differentiation between approximated and local values of AngleAT
  double AngP = TLaw->Value(Param), AngPo = HArrAngle->Value(iC), dAng = AngP - AngPo;
  gp_Vec Tangent, Normal, BN;
  frenet->D0(Param, Tangent, Normal, BN);
  double DAng = CalcAngleAT(Tangent, Normal, HArrTangent->Value(iC), HArrNormal->Value(iC));
  double DA   = diffAng(DAng, dAng);
  // The correction (there is core of OCC78 bug)
  if (std::abs(DA) > M_PI / 2.0)
  {
    AngP = AngPo + DAng;
  };
  return AngP;
}

//=================================================================================================

bool GeomFill_CorrectedFrenet::D0(const double Param,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      BiNormal)
{
  frenet->D0(Param, Tangent, Normal, BiNormal);
  if (isFrenet)
    return true;

  double angleAT;
  // angleAT = TLaw->Value(Param);
  angleAT = GetAngleAT(Param); // OCC78

  // rotation around Tangent
  gp_Vec cross;
  cross = Tangent.Crossed(Normal);
  Normal.SetLinearForm(std::sin(angleAT),
                       cross,
                       (1 - std::cos(angleAT)),
                       Tangent.Crossed(cross),
                       Normal);
  BiNormal = Tangent.Crossed(Normal);

  return true;
}

//=================================================================================================

bool GeomFill_CorrectedFrenet::D1(const double Param,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      DTangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      DNormal,
                                  gp_Vec&      BiNormal,
                                  gp_Vec&      DBiNormal)
{
  frenet->D1(Param, Tangent, DTangent, Normal, DNormal, BiNormal, DBiNormal);
  if (isFrenet)
    return true;

  double angleAT, d_angleAT;
  double sina, cosa;

  TLaw->D1(Param, angleAT, d_angleAT);
  angleAT = GetAngleAT(Param); // OCC78

  gp_Vec cross, dcross, tcross, dtcross, aux;
  sina = std::sin(angleAT);
  cosa = std::cos(angleAT);

  cross = Tangent.Crossed(Normal);
  dcross.SetLinearForm(1, DTangent.Crossed(Normal), Tangent.Crossed(DNormal));

  tcross = Tangent.Crossed(cross);
  dtcross.SetLinearForm(1, DTangent.Crossed(cross), Tangent.Crossed(dcross));

  aux.SetLinearForm(sina, dcross, cosa * d_angleAT, cross);
  aux.SetLinearForm(1 - cosa, dtcross, sina * d_angleAT, tcross, aux);
  DNormal += aux;

  Normal.SetLinearForm(sina, cross, (1 - cosa), tcross, Normal);

  BiNormal = Tangent.Crossed(Normal);

  DBiNormal.SetLinearForm(1, DTangent.Crossed(Normal), Tangent.Crossed(DNormal));

  // for test
  /*  gp_Vec FDN, Tf, Nf, BNf;
    double h;
    h = 1.0e-8;
    if (Param + h > myTrimmed->LastParameter()) h = -h;
    D0(Param + h, Tf, Nf, BNf);
    FDN = (Nf - Normal)/h;
    std::cout<<"Param = "<<Param<<std::endl;
    std::cout<<"DN = ("<<DNormal.X()<<", "<<DNormal.Y()<<", "<<DNormal.Z()<<")"<<std::endl;
    std::cout<<"FDN = ("<<FDN.X()<<", "<<FDN.Y()<<", "<<FDN.Z()<<")"<<std::endl;
  */

  return true;
}

//=================================================================================================

bool GeomFill_CorrectedFrenet::D2(const double Param,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      DTangent,
                                  gp_Vec&      D2Tangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      DNormal,
                                  gp_Vec&      D2Normal,
                                  gp_Vec&      BiNormal,
                                  gp_Vec&      DBiNormal,
                                  gp_Vec&      D2BiNormal)
{
  frenet->D2(Param,
             Tangent,
             DTangent,
             D2Tangent,
             Normal,
             DNormal,
             D2Normal,
             BiNormal,
             DBiNormal,
             D2BiNormal);
  if (isFrenet)
    return true;

  double angleAT, d_angleAT, d2_angleAT;
  double sina, cosa;
  TLaw->D2(Param, angleAT, d_angleAT, d2_angleAT);
  angleAT = GetAngleAT(Param); // OCC78

  gp_Vec cross, dcross, d2cross, tcross, dtcross, d2tcross, aux;
  sina  = std::sin(angleAT);
  cosa  = std::cos(angleAT);
  cross = Tangent.Crossed(Normal);
  dcross.SetLinearForm(1, DTangent.Crossed(Normal), Tangent.Crossed(DNormal));
  d2cross.SetLinearForm(1,
                        D2Tangent.Crossed(Normal),
                        2,
                        DTangent.Crossed(DNormal),
                        Tangent.Crossed(D2Normal));

  tcross = Tangent.Crossed(cross);
  dtcross.SetLinearForm(1, DTangent.Crossed(cross), Tangent.Crossed(dcross));
  d2tcross.SetLinearForm(1,
                         D2Tangent.Crossed(cross),
                         2,
                         DTangent.Crossed(dcross),
                         Tangent.Crossed(d2cross));

  aux.SetLinearForm(sina,
                    d2cross,
                    2 * cosa * d_angleAT,
                    dcross,
                    cosa * d2_angleAT - sina * d_angleAT * d_angleAT,
                    cross);

  aux.SetLinearForm(1 - cosa,
                    d2tcross,
                    2 * sina * d_angleAT,
                    dtcross,
                    cosa * d_angleAT * d_angleAT + sina * d2_angleAT,
                    tcross,
                    aux);
  D2Normal += aux;

  /*  D2Normal += sina*(D2Tangent.Crossed(Normal) + 2*DTangent.Crossed(DNormal) +
  Tangent.Crossed(D2Normal)) + 2*cosa*d_angleAT*(DTangent.Crossed(Normal) +
  Tangent.Crossed(DNormal)) + (cosa*d2_angleAT - sina*d_angleAT*d_angleAT)*Tangent.Crossed(Normal) +
  2*sina*d_angleAT*(DTangent.Crossed(Tangent.Crossed(Normal)) +
  Tangent.Crossed(DTangent.Crossed(Normal)) + Tangent.Crossed(Tangent.Crossed(DNormal))) + (1 -
  cosa)*(D2Tangent.Crossed(Tangent.Crossed(Normal)) + Tangent.Crossed(D2Tangent.Crossed(Normal)) +
  Tangent.Crossed(Tangent.Crossed(D2Normal)) + 2*DTangent.Crossed(DTangent.Crossed(Normal)) +
  2*DTangent.Crossed(Tangent.Crossed(DNormal)) + 2*Tangent.Crossed(DTangent.Crossed(DNormal)))
  +
  (cosa*d_angleAT*d_angleAT + sina*d2_angleAT)*Tangent.Crossed(Tangent.Crossed(Normal));*/

  aux.SetLinearForm(sina, dcross, cosa * d_angleAT, cross);
  aux.SetLinearForm(1 - cosa, dtcross, sina * d_angleAT, tcross, aux);
  DNormal += aux;

  Normal.SetLinearForm(sina, cross, (1 - cosa), tcross, Normal);

  BiNormal = Tangent.Crossed(Normal);

  DBiNormal.SetLinearForm(1, DTangent.Crossed(Normal), Tangent.Crossed(DNormal));

  D2BiNormal.SetLinearForm(1,
                           D2Tangent.Crossed(Normal),
                           2,
                           DTangent.Crossed(DNormal),
                           Tangent.Crossed(D2Normal));

  // for test
  /*  gp_Vec FD2N, FD2T, FD2BN, Tf, DTf, Nf, DNf, BNf, DBNf;
    double h;
    h = 1.0e-8;
    if (Param + h > myTrimmed->LastParameter()) h = -h;
    D1(Param + h, Tf, DTf, Nf, DNf, BNf, DBNf);
    FD2N = (DNf - DNormal)/h;
    FD2T = (DTf - DTangent)/h;
    FD2BN = (DBNf - DBiNormal)/h;
    std::cout<<"Param = "<<Param<<std::endl;
    std::cout<<"D2N = ("<<D2Normal.X()<<", "<<D2Normal.Y()<<", "<<D2Normal.Z()<<")"<<std::endl;
    std::cout<<"FD2N = ("<<FD2N.X()<<", "<<FD2N.Y()<<", "<<FD2N.Z()<<")"<<std::endl<<std::endl;
    std::cout<<"D2T = ("<<D2Tangent.X()<<", "<<D2Tangent.Y()<<", "<<D2Tangent.Z()<<")"<<std::endl;
    std::cout<<"FD2T = ("<<FD2T.X()<<", "<<FD2T.Y()<<", "<<FD2T.Z()<<")"<<std::endl<<std::endl;
    std::cout<<"D2BN = ("<<D2BiNormal.X()<<", "<<D2BiNormal.Y()<<",
    "<<D2BiNormal.Z()<<")"<<std::endl; std::cout<<"FD2BN = ("<<FD2BN.X()<<", "<<FD2BN.Y()<<",
    "<<FD2BN.Z()<<")"<<std::endl<<std::endl;
  */
  //
  return true;
}

//=================================================================================================

int GeomFill_CorrectedFrenet::NbIntervals(const GeomAbs_Shape S) const
{
  int NbFrenet, NbLaw;
  NbFrenet = frenet->NbIntervals(S);
  if (isFrenet)
    return NbFrenet;

  NbLaw = EvolAroundT->NbIntervals(S);
  if (NbFrenet == 1)
    return NbLaw;

  NCollection_Array1<double>   FrenetInt(1, NbFrenet + 1);
  NCollection_Array1<double>   LawInt(1, NbLaw + 1);
  NCollection_Sequence<double> Fusion;

  frenet->Intervals(FrenetInt, S);
  EvolAroundT->Intervals(LawInt, S);
  GeomLib::FuseIntervals(FrenetInt, LawInt, Fusion, Precision::PConfusion(), true);

  return Fusion.Length() - 1;
}

//=================================================================================================

void GeomFill_CorrectedFrenet::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  int NbFrenet, NbLaw;
  if (isFrenet)
  {
    frenet->Intervals(T, S);
    return;
  }

  NbFrenet = frenet->NbIntervals(S);
  if (NbFrenet == 1)
  {
    EvolAroundT->Intervals(T, S);
  }

  NbLaw = EvolAroundT->NbIntervals(S);

  NCollection_Array1<double>   FrenetInt(1, NbFrenet + 1);
  NCollection_Array1<double>   LawInt(1, NbLaw + 1);
  NCollection_Sequence<double> Fusion;

  frenet->Intervals(FrenetInt, S);
  EvolAroundT->Intervals(LawInt, S);
  GeomLib::FuseIntervals(FrenetInt, LawInt, Fusion, Precision::PConfusion(), true);

  for (int i = 1; i <= Fusion.Length(); i++)
    T.ChangeValue(i) = Fusion.Value(i);
}

//=================================================================================================

void GeomFill_CorrectedFrenet::SetInterval(const double First, const double Last)
{
  GeomFill_TrihedronLaw::SetInterval(First, Last);
  frenet->SetInterval(First, Last);
  if (!isFrenet)
    TLaw = EvolAroundT->Trim(First, Last, Precision::PConfusion() / 2);
}

//=================================================================================================

GeomFill_Trihedron GeomFill_CorrectedFrenet::EvaluateBestMode()
{
  if (EvolAroundT.IsNull())
    return GeomFill_IsFrenet; // Frenet

  const double MaxAngle   = 3. * M_PI / 4.;
  const double MaxTorsion = 100.;

  double Step, u, v, tmin, tmax;
  int    NbInt, i, j, k = 1;
  NbInt = EvolAroundT->NbIntervals(GeomAbs_CN);
  NCollection_Array1<double> Int(1, NbInt + 1);
  EvolAroundT->Intervals(Int, GeomAbs_CN);
  gp_Pnt2d old;
  gp_Vec2d aVec, PrevVec;

  int NbSamples = 10;
  for (i = 1; i <= NbInt; i++)
  {
    tmin           = Int(i);
    tmax           = Int(i + 1);
    double Torsion = ComputeTorsion(tmin, myTrimmed);
    if (std::abs(Torsion) > MaxTorsion)
      return GeomFill_IsDiscreteTrihedron; // DiscreteTrihedron

    occ::handle<Law_Function> trimmedlaw =
      EvolAroundT->Trim(tmin, tmax, Precision::PConfusion() / 2);
    Step = (Int(i + 1) - Int(i)) / NbSamples;
    for (j = 0; j <= NbSamples; j++)
    {
      u = tmin + j * Step;
      v = trimmedlaw->Value(u);
      gp_Pnt2d point2d(u, v);
      if (j != 0)
      {
        aVec.SetXY(point2d.XY() - old.XY());
        if (k > 2)
        {
          double theAngle = PrevVec.Angle(aVec);
          if (std::abs(theAngle) > MaxAngle)
            return GeomFill_IsDiscreteTrihedron; // DiscreteTrihedron
        }
        PrevVec = aVec;
      }
      old = point2d;
      k++;
    }
  }

  return GeomFill_IsCorrectedFrenet; // CorrectedFrenet
}

//=================================================================================================

void GeomFill_CorrectedFrenet::GetAverageLaw(gp_Vec& ATangent, gp_Vec& ANormal, gp_Vec& ABiNormal)
{
  if (isFrenet)
    frenet->GetAverageLaw(ATangent, ANormal, ABiNormal);
  else
  {
    ATangent  = AT;
    ANormal   = AN;
    ABiNormal = ATangent;
    ABiNormal.Cross(ANormal);
  }
}

//=================================================================================================

bool GeomFill_CorrectedFrenet::IsConstant() const
{
  return (myCurve->GetType() == GeomAbs_Line);
}

//=================================================================================================

bool GeomFill_CorrectedFrenet::IsOnlyBy3dCurve() const
{
  return true;
}
