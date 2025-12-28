// Copyright (c) 1995-1999 Matra Datavision
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

#include <Adaptor2d_OffsetCurve.hxx>
#include <ElCLib.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <Geom2dGcc_Circ2d2TanRadGeo.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>

static const int aNbSolMAX = 16;

// circulaire tant a une courbe et une droite ,de rayon donne
//==============================================================

//========================================================================
// On initialise WellDone a false.                                       +
// On recupere la courbe Cu2 et la droite L1.                            +
// On sort en erreur dans les cas ou la construction est impossible.     +
// On fait la parallele a Cu2 dans le bon sens.                          +
// On fait la parallele a L1 dans le bon sens.                           +
// On intersecte les paralleles ==> point de centre de la solution.      +
// On cree la solution qu on ajoute aux solutions deja trouvees.         +
// On remplit les champs.                                                +
//========================================================================

Geom2dGcc_Circ2d2TanRadGeo::Geom2dGcc_Circ2d2TanRadGeo(const GccEnt_QualifiedLin& Qualified1,
                                                       const Geom2dGcc_QCurve&    Qualified2,
                                                       const double               Radius,
                                                       const double               Tolerance)
    :

      //========================================================================
      // initialisation des champs.                                            +
      //========================================================================

      cirsol(1, aNbSolMAX),
      qualifier1(1, aNbSolMAX),
      qualifier2(1, aNbSolMAX),
      TheSame1(1, aNbSolMAX),
      TheSame2(1, aNbSolMAX),
      pnttg1sol(1, aNbSolMAX),
      pnttg2sol(1, aNbSolMAX),
      par1sol(1, aNbSolMAX),
      par2sol(1, aNbSolMAX),
      pararg1(1, aNbSolMAX),
      pararg2(1, aNbSolMAX)
{

  //========================================================================
  // Traitement.                                                           +
  //========================================================================

  double                     Tol      = std::abs(Tolerance);
  double                     thefirst = -100000.;
  double                     thelast  = 100000.;
  double                     firstparam;
  double                     lastparam;
  gp_Dir2d                   dirx(gp_Dir2d::D::X);
  NCollection_Array1<double> cote1(1, 2);
  NCollection_Array1<double> cote2(1, 2);
  int                        nbrcote1 = 0;
  int                        nbrcote2 = 0;
  WellDone                            = false;
  NbrSol                              = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsOutside() || Qualified1.IsUnqualified())
      || !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || Qualified2.IsOutside()
           || Qualified2.IsUnqualified()))
  {

    throw GccEnt_BadQualifier();
    return;
  }
  gp_Lin2d            L1    = Qualified1.Qualified();
  double              x1dir = (L1.Direction()).X();
  double              y1dir = (L1.Direction()).Y();
  double              lxloc = (L1.Location()).X();
  double              lyloc = (L1.Location()).Y();
  gp_Pnt2d            origin1(lxloc, lyloc);
  gp_Dir2d            normL1(-y1dir, x1dir);
  Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
  if (Radius < 0.0)
  {
    throw Standard_NegativeValue();
  }
  else
  {
    if (Qualified1.IsEnclosed() && Qualified2.IsEnclosed())
    {
      //   =======================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote2(1) = Radius;
    }
    else if (Qualified1.IsEnclosed() && Qualified2.IsOutside())
    {
      //   ==========================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote2(1) = -Radius;
    }
    else if (Qualified1.IsOutside() && Qualified2.IsEnclosed())
    {
      //   ===========================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = -Radius;
      cote2(1) = Radius;
    }
    else if (Qualified1.IsOutside() && Qualified2.IsOutside())
    {
      //   =========================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = -Radius;
      cote2(1) = -Radius;
    }
    if (Qualified1.IsEnclosed() && Qualified2.IsUnqualified())
    {
      //   =========================================================
      nbrcote1 = 1;
      nbrcote2 = 2;
      cote1(1) = Radius;
      cote2(1) = Radius;
      cote2(2) = -Radius;
    }
    if (Qualified1.IsUnqualified() && Qualified2.IsEnclosed())
    {
      //   =========================================================
      nbrcote1 = 2;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote1(2) = -Radius;
      cote2(1) = Radius;
    }
    else if (Qualified1.IsOutside() && Qualified2.IsUnqualified())
    {
      //   =============================================================
      nbrcote1 = 1;
      nbrcote2 = 2;
      cote1(1) = -Radius;
      cote2(1) = Radius;
      cote2(2) = -Radius;
    }
    if (Qualified1.IsUnqualified() && Qualified2.IsOutside())
    {
      //   ========================================================
      nbrcote1 = 2;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote1(2) = -Radius;
      cote2(1) = -Radius;
    }
    else if (Qualified1.IsUnqualified() && Qualified2.IsUnqualified())
    {
      //   =================================================================
      nbrcote1 = 2;
      nbrcote2 = 2;
      cote1(1) = Radius;
      cote1(2) = -Radius;
      cote2(1) = Radius;
      cote2(2) = -Radius;
    }
    gp_Dir2d Dir(-y1dir, x1dir);
    for (int jcote1 = 1; jcote1 <= nbrcote1; jcote1++)
    {
      gp_Pnt2d        Point(L1.Location().XY() + cote1(jcote1) * Dir.XY());
      gp_Lin2d        Line(Point, L1.Direction()); // ligne avec deport.
      IntRes2d_Domain D1;
      for (int jcote2 = 1; jcote2 <= nbrcote2 && NbrSol < aNbSolMAX; jcote2++)
      {
        occ::handle<Geom2dAdaptor_Curve> HCu2 = new Geom2dAdaptor_Curve(Cu2);
        // Adaptor2d_OffsetCurve C2(HCu2,cote2(jcote2));
        Adaptor2d_OffsetCurve C2(HCu2, -cote2(jcote2));
        firstparam = std::max(C2.FirstParameter(), thefirst);
        lastparam  = std::min(C2.LastParameter(), thelast);
        IntRes2d_Domain                    D2(C2.Value(firstparam),
                           firstparam,
                           Tol,
                           C2.Value(lastparam),
                           lastparam,
                           Tol);
        Geom2dInt_TheIntConicCurveOfGInter Intp(Line, D1, C2, D2, Tol, Tol);
        if (Intp.IsDone())
        {
          if (!Intp.IsEmpty())
          {
            for (int i = 1; i <= Intp.NbPoints() && NbrSol < aNbSolMAX; i++)
            {
              NbrSol++;
              gp_Pnt2d Center(Intp.Point(i).Value());
              cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center, dirx), Radius);
              //             =======================================================
              gp_Dir2d dc1(origin1.XY() - Center.XY());
              qualifier2(NbrSol) = Qualified2.Qualifier();
              if (!Qualified1.IsUnqualified())
              {
                qualifier1(NbrSol) = Qualified1.Qualifier();
              }
              else if (dc1.Dot(normL1) > 0.0)
              {
                qualifier1(NbrSol) = GccEnt_outside;
              }
              else
              {
                qualifier1(NbrSol) = GccEnt_enclosed;
              }
              TheSame1(NbrSol)  = 0;
              TheSame2(NbrSol)  = 0;
              pararg1(NbrSol)   = Intp.Point(i).ParamOnFirst();
              pararg2(NbrSol)   = Intp.Point(i).ParamOnSecond();
              pnttg1sol(NbrSol) = ElCLib::Value(pararg1(NbrSol), L1);
              pnttg2sol(NbrSol) = Geom2dGcc_CurveTool::Value(Cu2, pararg2(NbrSol));
              par1sol(NbrSol)   = ElCLib::Parameter(cirsol(NbrSol), pnttg1sol(NbrSol));
              par2sol(NbrSol)   = ElCLib::Parameter(cirsol(NbrSol), pnttg2sol(NbrSol));
            }
          }
          WellDone = true;
        }
      }
    }
  }
}

// circulaire tant a une courbe et un cercle ,de rayon donne
//=============================================================

//========================================================================
// On initialise WellDone a false.                                       +
// On recupere la courbe Cu2 et le cercle C1.                            +
// On sort en erreur dans les cas ou la construction est impossible.     +
// On fait la parallele a Cu2 dans le bon sens.                          +
// On fait la parallele a C1 dans le bon sens.                           +
// On intersecte les paralleles ==> point de centre de la solution.      +
// On cree la solution qu on ajoute aux solutions deja trouvees.         +
// On remplit les champs.                                                +
//========================================================================

Geom2dGcc_Circ2d2TanRadGeo::Geom2dGcc_Circ2d2TanRadGeo(const GccEnt_QualifiedCirc& Qualified1,
                                                       const Geom2dGcc_QCurve&     Qualified2,
                                                       const double                Radius,
                                                       const double                Tolerance)
    :

      //========================================================================
      // initialisation des champs.                                            +
      //========================================================================

      cirsol(1, aNbSolMAX),
      qualifier1(1, aNbSolMAX),
      qualifier2(1, aNbSolMAX),
      TheSame1(1, aNbSolMAX),
      TheSame2(1, aNbSolMAX),
      pnttg1sol(1, aNbSolMAX),
      pnttg2sol(1, aNbSolMAX),
      par1sol(1, aNbSolMAX),
      par2sol(1, aNbSolMAX),
      pararg1(1, aNbSolMAX),
      pararg2(1, aNbSolMAX)
{

  //========================================================================
  // Traitement.                                                           +
  //========================================================================

  double                     Tol      = std::abs(Tolerance);
  double                     thefirst = -100000.;
  double                     thelast  = 100000.;
  double                     firstparam;
  double                     lastparam;
  gp_Dir2d                   dirx(gp_Dir2d::D::X);
  NCollection_Array1<double> cote1(1, 2);
  NCollection_Array1<double> cote2(1, 2);
  int                        nbrcote1 = 0;
  int                        nbrcote2 = 0;
  WellDone                            = false;
  NbrSol                              = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || Qualified1.IsOutside()
        || Qualified1.IsUnqualified())
      || !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || Qualified2.IsOutside()
           || Qualified2.IsUnqualified()))
  {
    throw GccEnt_BadQualifier();
    return;
  }
  gp_Circ2d           C1 = Qualified1.Qualified();
  gp_Pnt2d            center1(C1.Location());
  Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
  if (Radius < 0.0)
  {
    throw Standard_NegativeValue();
  }
  else
  {
    if (Qualified1.IsEnclosed() && Qualified2.IsEnclosed())
    {
      //   =======================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote2(1) = Radius;
    }
    else if (Qualified1.IsEnclosed() && Qualified2.IsOutside())
    {
      //   ==========================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote2(1) = -Radius;
    }
    else if (Qualified1.IsOutside() && Qualified2.IsEnclosed())
    {
      //   ===========================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = -Radius;
      cote2(1) = Radius;
    }
    else if (Qualified1.IsOutside() && Qualified2.IsOutside())
    {
      //   =========================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = -Radius;
      cote2(1) = -Radius;
    }
    if (Qualified1.IsEnclosed() && Qualified2.IsUnqualified())
    {
      //   =========================================================
      nbrcote1 = 1;
      nbrcote2 = 2;
      cote1(1) = Radius;
      cote2(1) = Radius;
      cote2(2) = -Radius;
    }
    if (Qualified1.IsUnqualified() && Qualified2.IsEnclosed())
    {
      //   =========================================================
      nbrcote1 = 2;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote1(2) = -Radius;
      cote2(1) = Radius;
    }
    else if (Qualified1.IsOutside() && Qualified2.IsUnqualified())
    {
      //   =============================================================
      nbrcote1 = 1;
      nbrcote2 = 2;
      cote1(1) = -Radius;
      cote2(1) = Radius;
      cote2(2) = -Radius;
    }
    if (Qualified1.IsUnqualified() && Qualified2.IsOutside())
    {
      //   ========================================================
      nbrcote1 = 2;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote1(2) = -Radius;
      cote2(1) = -Radius;
    }
    else if (Qualified1.IsUnqualified() && Qualified2.IsUnqualified())
    {
      //   =================================================================
      nbrcote1 = 2;
      nbrcote2 = 2;
      cote1(1) = Radius;
      cote1(2) = -Radius;
      cote2(1) = Radius;
      cote2(2) = -Radius;
    }
    double                             R1 = C1.Radius();
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    for (int jcote1 = 1; jcote1 <= nbrcote1 && NbrSol < aNbSolMAX; jcote1++)
    {
      gp_Circ2d       Circ(C1.XAxis(), R1 + cote1(jcote1));
      IntRes2d_Domain D1(ElCLib::Value(0., Circ),
                         0.,
                         Tol,
                         ElCLib::Value(2. * M_PI, Circ),
                         2. * M_PI,
                         Tol);
      D1.SetEquivalentParameters(0., 2. * M_PI);
      for (int jcote2 = 1; jcote2 <= nbrcote2; jcote2++)
      {
        occ::handle<Geom2dAdaptor_Curve> HCu2 = new Geom2dAdaptor_Curve(Cu2);
        // Adaptor2d_OffsetCurve C2(HCu2,cote2(jcote2));
        Adaptor2d_OffsetCurve C2(HCu2, -cote2(jcote2));
        firstparam = std::max(C2.FirstParameter(), thefirst);
        lastparam  = std::min(C2.LastParameter(), thelast);
        IntRes2d_Domain D2(C2.Value(firstparam),
                           firstparam,
                           Tol,
                           C2.Value(lastparam),
                           lastparam,
                           Tol);
        Intp.Perform(Circ, D1, C2, D2, Tol, Tol);
        if (Intp.IsDone())
        {
          if (!Intp.IsEmpty())
          {
            for (int i = 1; i <= Intp.NbPoints() && NbrSol < aNbSolMAX; i++)
            {
              NbrSol++;
              gp_Pnt2d Center(Intp.Point(i).Value());
              cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center, dirx), Radius);
              //             =======================================================
#ifdef OCCT_DEBUG
              gp_Dir2d dir1(Center.XY() - center1.XY());
#else
              Center.XY();
              center1.XY();
#endif
              double distcc1 = Center.Distance(center1);
              if (!Qualified1.IsUnqualified())
              {
                qualifier1(NbrSol) = Qualified1.Qualifier();
              }
              else if (std::abs(distcc1 + Radius - R1) < Tol)
              {
                qualifier1(NbrSol) = GccEnt_enclosed;
              }
              else if (std::abs(distcc1 - R1 - Radius) < Tol)
              {
                qualifier1(NbrSol) = GccEnt_outside;
              }
              else
              {
                qualifier1(NbrSol) = GccEnt_enclosing;
              }
              qualifier2(NbrSol) = Qualified2.Qualifier();
              TheSame1(NbrSol)   = 0;
              TheSame2(NbrSol)   = 0;
              pararg1(NbrSol)    = Intp.Point(i).ParamOnFirst();
              pararg2(NbrSol)    = Intp.Point(i).ParamOnSecond();
              pnttg1sol(NbrSol)  = ElCLib::Value(pararg1(NbrSol), C1);
              pnttg2sol(NbrSol)  = Geom2dGcc_CurveTool::Value(Cu2, pararg2(NbrSol));
              par1sol(NbrSol)    = ElCLib::Parameter(cirsol(NbrSol), pnttg1sol(NbrSol));
              par2sol(NbrSol)    = ElCLib::Parameter(cirsol(NbrSol), pnttg2sol(NbrSol));
            }
          }
          WellDone = true;
        }
      }
    }
  }
}

// circulaire tant a une courbe et un point ,de rayon donne
//============================================================

//========================================================================
// On initialise WellDone a false.                                       +
// On recupere la courbe Cu1 et le point P2.                             +
// On sort en erreur dans les cas ou la construction est impossible.     +
// On fait la parallele a Cu1 dans le bon sens.                          +
// On fait la parallele a P2 dans le bon sens.                           +
// On intersecte les paralleles ==> point de centre de la solution.      +
// On cree la solution qu on ajoute aux solutions deja trouvees.         +
// On remplit les champs.                                                +
//========================================================================

Geom2dGcc_Circ2d2TanRadGeo::Geom2dGcc_Circ2d2TanRadGeo(const Geom2dGcc_QCurve& Qualified1,
                                                       const gp_Pnt2d&         Point2,
                                                       const double            Radius,
                                                       const double            Tolerance)
    :

      //========================================================================
      // initialisation des champs.                                            +
      //========================================================================

      cirsol(1, aNbSolMAX),
      qualifier1(1, aNbSolMAX),
      qualifier2(1, aNbSolMAX),
      TheSame1(1, aNbSolMAX),
      TheSame2(1, aNbSolMAX),
      pnttg1sol(1, aNbSolMAX),
      pnttg2sol(1, aNbSolMAX),
      par1sol(1, aNbSolMAX),
      par2sol(1, aNbSolMAX),
      pararg1(1, aNbSolMAX),
      pararg2(1, aNbSolMAX)
{

  //========================================================================
  // Traitement.                                                           +
  //========================================================================

  double                     Tol      = std::abs(Tolerance);
  double                     thefirst = -100000.;
  double                     thelast  = 100000.;
  double                     firstparam;
  double                     lastparam;
  gp_Dir2d                   dirx(gp_Dir2d::D::X);
  NCollection_Array1<double> cote1(1, 2);
  int                        nbrcote1 = 0;
  WellDone                            = false;
  NbrSol                              = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || Qualified1.IsOutside()
        || Qualified1.IsUnqualified()))
  {
    throw GccEnt_BadQualifier();
    return;
  }
  Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
  if (Radius < 0.0)
  {
    throw Standard_NegativeValue();
  }
  else
  {
    if (Qualified1.IsEnclosed())
    {
      //    ===========================
      nbrcote1 = 1;
      cote1(1) = Radius;
    }
    else if (Qualified1.IsOutside())
    {
      //   ===============================
      nbrcote1 = 1;
      cote1(1) = -Radius;
    }
    else if (Qualified1.IsUnqualified())
    {
      //   ===================================
      nbrcote1 = 2;
      cote1(1) = Radius;
      cote1(2) = -Radius;
    }
    gp_Circ2d       Circ(gp_Ax2d(Point2, gp_Dir2d(gp_Dir2d::D::X)), Radius);
    IntRes2d_Domain D1(ElCLib::Value(0., Circ),
                       0.,
                       Tol,
                       ElCLib::Value(M_PI + M_PI, Circ),
                       M_PI + M_PI,
                       Tol);
    D1.SetEquivalentParameters(0., M_PI + M_PI);
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    for (int jcote1 = 1; jcote1 <= nbrcote1 && NbrSol < aNbSolMAX; jcote1++)
    {
      occ::handle<Geom2dAdaptor_Curve> HCu1 = new Geom2dAdaptor_Curve(Cu1);
      // Adaptor2d_OffsetCurve Cu2(HCu1,cote1(jcote1));
      Adaptor2d_OffsetCurve Cu2(HCu1, -cote1(jcote1));
      firstparam = std::max(Cu2.FirstParameter(), thefirst);
      lastparam  = std::min(Cu2.LastParameter(), thelast);
      IntRes2d_Domain D2(Cu2.Value(firstparam),
                         firstparam,
                         Tol,
                         Cu2.Value(lastparam),
                         lastparam,
                         Tol);
      Intp.Perform(Circ, D1, Cu2, D2, Tol, Tol);
      if (Intp.IsDone())
      {
        if (!Intp.IsEmpty())
        {
          for (int i = 1; i <= Intp.NbPoints() && NbrSol < aNbSolMAX; i++)
          {
            NbrSol++;
            gp_Pnt2d Center(Intp.Point(i).Value());
            cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center, dirx), Radius);
            //           =======================================================
            qualifier1(NbrSol) = Qualified1.Qualifier();
            qualifier2(NbrSol) = GccEnt_noqualifier;
            TheSame1(NbrSol)   = 0;
            TheSame2(NbrSol)   = 0;
            pararg1(NbrSol)    = Intp.Point(i).ParamOnSecond();
            pararg2(NbrSol)    = 0.;
            pnttg1sol(NbrSol)  = Geom2dGcc_CurveTool::Value(Cu1, pararg1(NbrSol));
            pnttg2sol(NbrSol)  = Point2;
            par1sol(NbrSol)    = ElCLib::Parameter(cirsol(NbrSol), pnttg1sol(NbrSol));
            par2sol(NbrSol)    = ElCLib::Parameter(cirsol(NbrSol), pnttg2sol(NbrSol));
          }
        }
        WellDone = true;
      }
    }
  }
}

//=======================================================================
// function : PrecRoot
// purpose  : In case, when curves has tangent zones, intersection point
//            found may be precised. This function uses precision algorithm
//            of Extrema Curve-Curve method (dot product between every
//            tangent vector and vector between points in two curves must
//            be equal to zero).
//=======================================================================
static void PrecRoot(const Adaptor2d_OffsetCurve& theC1,
                     const Adaptor2d_OffsetCurve& theC2,
                     const double                 theU0,
                     const double                 theV0,
                     double&                      theUfinal,
                     double&                      theVfinal)
{
  /*
  It is necessary for precision to solve the system

      \left\{\begin{matrix}
      (x_{1}(u)-x_{2}(v))*{x_{1}(u)}'+(y_{1}(u)-y_{2}(v))*{y_{1}(u)}'=0\\
      (x_{1}(u)-x_{2}(v))*{x_{2}(v)}'+(y_{1}(u)-y_{2}(v))*{y_{2}(v)}'=0
      \end{matrix}\right.

  Precision of any 2*2-system (two equation and two variables)

      \left\{\begin{matrix}
      S_{1}(u,v)=0\\
      S_{2}(u,v)=0
      \end{matrix}\right.

  by Newton method can be made as follows:

      u=u_{0}-\left (\frac{\frac{\partial S_{2}}{\partial v}*S_{1}-
        \frac{\partial S_{1}}{\partial v}*S_{2}}
        {\frac{\partial S_{1}}{\partial u}*
        \frac{\partial S_{2}}{\partial v}-
        \frac{\partial S_{1}}{\partial v}*
        \frac{\partial S_{2}}{\partial u}}  \right )_{u_{0},v_{0}}\\
      v=v_{0}-\left (\frac{\frac{\partial S_{1}}{\partial u}*S_{2}-
        \frac{\partial S_{2}}{\partial u}*S_{1}}
        {\frac{\partial S_{1}}{\partial u}*
        \frac{\partial S_{2}}{\partial v}-
        \frac{\partial S_{1}}{\partial v}*
        \frac{\partial S_{2}}{\partial u}}  \right )_{u_{0},v_{0}}
      \end{matrix}\right.

  where u_{0} and v_{0} are initial values or values computed on previous iteration.
  */

  theUfinal = theU0;
  theVfinal = theV0;

  const int aNbIterMax = 100;

  double   aU = theU0, aV = theV0;
  gp_Pnt2d aPu, aPv;
  gp_Vec2d aD1u, aD1v, aD2u, aD2v;

  int aNbIter = 0;

  double aStepU = 0.0, aStepV = 0.0;

  double aSQDistPrev = RealFirst();

  theC1.D2(aU, aPu, aD1u, aD2u);
  theC2.D2(aV, aPv, aD1v, aD2v);

  const double aCrProd = std::abs(aD1u.Crossed(aD1v));
  if (aCrProd * aCrProd > 1.0e-6 * aD1u.SquareMagnitude() * aD1v.SquareMagnitude())
  {
    // Curves are not tangent. Therefore, we consider that
    // 2D-intersection algorithm have found good point which
    // did not need in more precision.
    return;
  }

  do
  {
    aNbIter++;

    gp_Vec2d aVuv(aPv, aPu);

    double aSQDist = aVuv.SquareMagnitude();
    if (IsEqual(aSQDist, 0.0))
      break;

    if ((aNbIter == 1) || (aSQDist < aSQDistPrev))
    {
      aSQDistPrev = aSQDist;
      theUfinal   = aU;
      theVfinal   = aV;
    }

    double aG1 = aD1u.Magnitude();
    double aG2 = aD1v.Magnitude();

    if (IsEqual(aG1, 0.0) || IsEqual(aG2, 0.0))
    { // Here we do not processing singular cases.
      break;
    }

    double aF1 = aVuv.Dot(aD1u);
    double aF2 = aVuv.Dot(aD1v);

    double aFIu  = aVuv.Dot(aD2u);
    double aFIv  = aVuv.Dot(aD2v);
    double aPSIu = aD1u.Dot(aD2u);
    double aPSIv = aD1v.Dot(aD2v);

    double aTheta = aD1u * aD1v;

    double aS1 = aF1 / aG1;
    double aS2 = aF2 / aG2;

    double aDS1u = (aG1 * aG1 + aFIu) / aG1 - (aS1 * aPSIu / (aG1 * aG1));
    double aDS1v = -aTheta / aG1;
    double aDS2u = aTheta / aG2;
    double aDS2v = (aFIv - aG2 * aG2) / aG2 - (aS2 * aPSIv / (aG2 * aG2));

    double aDet = aDS1u * aDS2v - aDS1v * aDS2u;

    if (IsEqual(aDet, 0.0))
    {
      if (!IsEqual(aStepV, 0.0) && !IsEqual(aDS1u, 0.0))
      {
        aV += aStepV;
        aU = aU - (aDS1v * aStepV - aS1) / aDS1u;
      }
      else if (!IsEqual(aStepU, 0.0) && !IsEqual(aDS1v, 0.0))
      {
        aU += aStepU;
        aV = aV - (aDS1u * aStepU - aS1) / aDS1v;
      }
      else
      {
        break;
      }
    }
    else
    {
      aStepU = -(aS1 * aDS2v - aS2 * aDS1v) / aDet;
      aStepV = -(aS2 * aDS1u - aS1 * aDS2u) / aDet;

      if (std::abs(aStepU) < Epsilon(std::abs(aU)))
      {
        if (std::abs(aStepV) < Epsilon(std::abs(aV)))
        {
          break;
        }
      }

      aU += aStepU;
      aV += aStepV;
    }

    theC1.D2(aU, aPu, aD1u, aD2u);
    theC2.D2(aV, aPv, aD1v, aD2v);
  } while (aNbIter <= aNbIterMax);
}

// circulaire tant a deux courbes ,de rayon donne
//==================================================

//========================================================================
// On initialise WellDone a false.                                       +
// On recupere les courbes Cu1 et Cu2.                                   +
// On sort en erreur dans les cas ou la construction est impossible.     +
// On fait la parallele a Cu1 dans le bon sens.                          +
// On fait la parallele a Cu2 dans le bon sens.                          +
// On intersecte les paralleles ==> point de centre de la solution.      +
// On cree la solution qu on ajoute aux solutions deja trouvees.         +
// On remplit les champs.                                                +
//========================================================================
Geom2dGcc_Circ2d2TanRadGeo::Geom2dGcc_Circ2d2TanRadGeo(const Geom2dGcc_QCurve& Qualified1,
                                                       const Geom2dGcc_QCurve& Qualified2,
                                                       const double            Radius,
                                                       const double            Tolerance)
    :

      //========================================================================
      // initialisation des champs.                                            +
      //========================================================================

      cirsol(1, aNbSolMAX),
      qualifier1(1, aNbSolMAX),
      qualifier2(1, aNbSolMAX),
      TheSame1(1, aNbSolMAX),
      TheSame2(1, aNbSolMAX),
      pnttg1sol(1, aNbSolMAX),
      pnttg2sol(1, aNbSolMAX),
      par1sol(1, aNbSolMAX),
      par2sol(1, aNbSolMAX),
      pararg1(1, aNbSolMAX),
      pararg2(1, aNbSolMAX)
{

  //========================================================================
  // Traitement.                                                           +
  //========================================================================

  double Tol = std::abs(Tolerance);
#ifdef OCCT_DEBUG
  const double thefirst = -100000.;
  const double thelast  = 100000.;
#endif
  gp_Dir2d                   dirx(gp_Dir2d::D::X);
  NCollection_Array1<double> cote1(1, 2);
  NCollection_Array1<double> cote2(1, 2);
  int                        nbrcote1 = 0;
  int                        nbrcote2 = 0;
  WellDone                            = false;
  NbrSol                              = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || Qualified1.IsOutside()
        || Qualified1.IsUnqualified())
      || !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || Qualified2.IsOutside()
           || Qualified2.IsUnqualified()))
  {
    throw GccEnt_BadQualifier();
    return;
  }
  Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
  Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
  if (Radius < 0.0)
  {
    throw Standard_NegativeValue();
  }
  else
  {
    if (Qualified1.IsEnclosed() && Qualified2.IsEnclosed())
    {
      //   =======================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote2(1) = Radius;
    }
    else if (Qualified1.IsEnclosed() && Qualified2.IsOutside())
    {
      //   ==========================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote2(1) = -Radius;
    }
    else if (Qualified1.IsOutside() && Qualified2.IsEnclosed())
    {
      //   ===========================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = -Radius;
      cote2(1) = Radius;
    }
    else if (Qualified1.IsOutside() && Qualified2.IsOutside())
    {
      //   =========================================================
      nbrcote1 = 1;
      nbrcote2 = 1;
      cote1(1) = -Radius;
      cote2(1) = -Radius;
    }
    if (Qualified1.IsEnclosed() && Qualified2.IsUnqualified())
    {
      //   =========================================================
      nbrcote1 = 1;
      nbrcote2 = 2;
      cote1(1) = Radius;
      cote2(1) = Radius;
      cote2(2) = -Radius;
    }
    if (Qualified1.IsUnqualified() && Qualified2.IsEnclosed())
    {
      //   =========================================================
      nbrcote1 = 2;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote1(2) = -Radius;
      cote2(1) = Radius;
    }
    else if (Qualified1.IsOutside() && Qualified2.IsUnqualified())
    {
      //   =============================================================
      nbrcote1 = 1;
      nbrcote2 = 2;
      cote1(1) = -Radius;
      cote2(1) = Radius;
      cote2(2) = -Radius;
    }
    if (Qualified1.IsUnqualified() && Qualified2.IsOutside())
    {
      //   ========================================================
      nbrcote1 = 2;
      nbrcote2 = 1;
      cote1(1) = Radius;
      cote1(2) = -Radius;
      cote2(1) = -Radius;
    }
    else if (Qualified1.IsUnqualified() && Qualified2.IsUnqualified())
    {
      //   =================================================================
      nbrcote1 = 2;
      nbrcote2 = 2;
      cote1(1) = Radius;
      cote1(2) = -Radius;
      cote2(1) = Radius;
      cote2(2) = -Radius;
    }
    Geom2dInt_GInter Intp;
    for (int jcote1 = 1; jcote1 <= nbrcote1; jcote1++)
    {
      occ::handle<Geom2dAdaptor_Curve> HCu1 = new Geom2dAdaptor_Curve(Cu1);
      // Adaptor2d_OffsetCurve C1(HCu1,cote1(jcote1));
      Adaptor2d_OffsetCurve C1(HCu1, -cote1(jcote1));
#ifdef OCCT_DEBUG
      double          firstparam = std::max(C1.FirstParameter(), thefirst);
      double          lastparam  = std::min(C1.LastParameter(), thelast);
      IntRes2d_Domain D2C1(C1.Value(firstparam),
                           firstparam,
                           Tol,
                           C1.Value(lastparam),
                           lastparam,
                           Tol);
#endif
      for (int jcote2 = 1; jcote2 <= nbrcote2 && NbrSol < aNbSolMAX; jcote2++)
      {
        occ::handle<Geom2dAdaptor_Curve> HCu2 = new Geom2dAdaptor_Curve(Cu2);
        // Adaptor2d_OffsetCurve C2(HCu2,cote2(jcote2));
        Adaptor2d_OffsetCurve C2(HCu2, -cote2(jcote2));
#ifdef OCCT_DEBUG
        firstparam = std::max(C2.FirstParameter(), thefirst);
        lastparam  = std::min(C2.LastParameter(), thelast);
        IntRes2d_Domain D2C2(C2.Value(firstparam),
                             firstparam,
                             Tol,
                             C2.Value(lastparam),
                             lastparam,
                             Tol);
#endif
        Intp.Perform(C1, C2, Tol, Tol);
        if (Intp.IsDone())
        {
          if (!Intp.IsEmpty())
          {
            constexpr double aSQApproxTol = Precision::Approximation() * Precision::Approximation();
            for (int i = 1; i <= Intp.NbPoints() && NbrSol < aNbSolMAX; i++)
            {
              double aU0 = Intp.Point(i).ParamOnFirst();
              double aV0 = Intp.Point(i).ParamOnSecond();

              double aU1 = aU0 - Precision::PApproximation();
              double aV1 = aV0 - Precision::PApproximation();

              double aU2 = aU0 + Precision::PApproximation();
              double aV2 = aV0 + Precision::PApproximation();

              gp_Pnt2d P11 = C1.Value(aU1);
              gp_Pnt2d P12 = C2.Value(aV1);
              gp_Pnt2d P21 = C1.Value(aU2);
              gp_Pnt2d P22 = C2.Value(aV2);

              double aDist1112 = P11.SquareDistance(P12);
              double aDist1122 = P11.SquareDistance(P22);

              double aDist1221 = P12.SquareDistance(P21);
              double aDist2122 = P21.SquareDistance(P22);

              if ((std::min(aDist1112, aDist1122) <= aSQApproxTol)
                  && (std::min(aDist1221, aDist2122) <= aSQApproxTol))
              {
                PrecRoot(C1, C2, aU0, aV0, aU0, aV0);
              }

              NbrSol++;
              gp_Pnt2d Center(C1.Value(aU0));
              cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center, dirx), Radius);
              //             =======================================================
              qualifier1(NbrSol) = Qualified1.Qualifier();
              qualifier1(NbrSol) = Qualified1.Qualifier();
              TheSame1(NbrSol)   = 0;
              TheSame2(NbrSol)   = 0;
              pararg1(NbrSol)    = Intp.Point(i).ParamOnFirst();
              pararg2(NbrSol)    = Intp.Point(i).ParamOnSecond();
              pnttg1sol(NbrSol)  = Geom2dGcc_CurveTool::Value(Cu1, pararg1(NbrSol));
              pnttg2sol(NbrSol)  = Geom2dGcc_CurveTool::Value(Cu2, pararg2(NbrSol));
              par1sol(NbrSol)    = ElCLib::Parameter(cirsol(NbrSol), pnttg1sol(NbrSol));
              par2sol(NbrSol)    = ElCLib::Parameter(cirsol(NbrSol), pnttg2sol(NbrSol));
            }
          }

          WellDone = true;
        }
      }
    }
  }
}

//=========================================================================

bool Geom2dGcc_Circ2d2TanRadGeo::IsDone() const
{
  return WellDone;
}

int Geom2dGcc_Circ2d2TanRadGeo::NbSolutions() const
{
  return NbrSol;
}

gp_Circ2d Geom2dGcc_Circ2d2TanRadGeo::ThisSolution(const int Index) const
{
  if (!WellDone)
  {
    throw StdFail_NotDone();
  }
  if (Index <= 0 || Index > NbrSol)
  {
    throw Standard_OutOfRange();
  }
  return cirsol(Index);
}

void Geom2dGcc_Circ2d2TanRadGeo::WhichQualifier(const int        Index,
                                                GccEnt_Position& Qualif1,
                                                GccEnt_Position& Qualif2) const
{
  if (!WellDone)
  {
    throw StdFail_NotDone();
  }
  else if (Index <= 0 || Index > NbrSol)
  {
    throw Standard_OutOfRange();
  }
  else
  {
    Qualif1 = qualifier1(Index);
    Qualif2 = qualifier2(Index);
  }
}

void Geom2dGcc_Circ2d2TanRadGeo::Tangency1(const int Index,
                                           double&   ParSol,
                                           double&   ParArg,
                                           gp_Pnt2d& PntSol) const
{
  if (!WellDone)
  {
    throw StdFail_NotDone();
  }
  else if (Index <= 0 || Index > NbrSol)
  {
    throw Standard_OutOfRange();
  }
  else
  {
    if (TheSame1(Index) == 0)
    {
      ParSol = par1sol(Index);
      ParArg = pararg1(Index);
      PntSol = gp_Pnt2d(pnttg1sol(Index));
    }
    else
    {
      throw StdFail_NotDone();
    }
  }
}

void Geom2dGcc_Circ2d2TanRadGeo::Tangency2(const int Index,
                                           double&   ParSol,
                                           double&   ParArg,
                                           gp_Pnt2d& PntSol) const
{
  if (!WellDone)
  {
    throw StdFail_NotDone();
  }
  else if (Index <= 0 || Index > NbrSol)
  {
    throw Standard_OutOfRange();
  }
  else
  {
    if (TheSame2(Index) == 0)
    {
      ParSol = par2sol(Index);
      ParArg = pararg2(Index);
      PntSol = gp_Pnt2d(pnttg2sol(Index));
    }
    else
    {
      throw StdFail_NotDone();
    }
  }
}

bool Geom2dGcc_Circ2d2TanRadGeo::IsTheSame1(const int Index) const
{
  if (!WellDone)
  {
    throw StdFail_NotDone();
  }
  if (Index <= 0 || Index > NbrSol)
  {
    throw Standard_OutOfRange();
  }

  if (TheSame1(Index) == 0)
  {
    return false;
  }
  return true;
}

bool Geom2dGcc_Circ2d2TanRadGeo::IsTheSame2(const int Index) const
{
  if (!WellDone)
  {
    throw StdFail_NotDone();
  }
  if (Index <= 0 || Index > NbrSol)
  {
    throw Standard_OutOfRange();
  }

  if (TheSame2(Index) == 0)
  {
    return false;
  }
  return true;
}
