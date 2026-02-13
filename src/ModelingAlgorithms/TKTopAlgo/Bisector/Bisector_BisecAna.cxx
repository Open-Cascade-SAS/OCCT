// Created on: 1992-10-19
// Created by: Remi GILET
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

//  Modified by skv - Fri Jul  1 16:23:17 2005 IDEM(Airbus)
//  Modified by skv - Wed Jul  7 17:21:09 2004 IDEM(Airbus)

#include <Bisector_BisecAna.hxx>
#include <ElCLib.hxx>
#include <GccAna_Circ2dBisec.hxx>
#include <GccAna_CircLin2dBisec.hxx>
#include <GccAna_CircPnt2dBisec.hxx>
#include <GccAna_LinPnt2dBisec.hxx>
#include <GccAna_Pnt2dBisec.hxx>
#include <GccInt_BLine.hxx>
#include <GccInt_IType.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Point.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <StdFail_NotDone.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Bisector_BisecAna, Bisector_Curve)

static bool Degenerate(occ::handle<GccInt_Bisec>& aBisector, const double Tolerance);

//=============================================================================
// function :
//=============================================================================
Bisector_BisecAna::Bisector_BisecAna() = default;

//=============================================================================
//              calcul the distance between the point and the bissectrice.              +
//              and orientation of the bissectrice.                             +
//    apoint        :       point of passage.                                 +
//    abisector     :       calculated bissectrice.                          +
//    afirstvector  :       first vector. \                                +
//    asecondvector :       second vector./ to choose the proper sector.    +
//    adirection    :       shows if the bissectrice is interior or exterior.  +
//    aparameter    : out : the start parameter of the bissectrice.         +
//    asense        : out : the direction of the bissectrice.                        +
//    astatus       : out : shows if the bissectrice is preserved.               +
//=============================================================================
double Bisector_BisecAna::Distance(const gp_Pnt2d&                  apoint,
                                   const occ::handle<GccInt_Bisec>& abisector,
                                   const gp_Vec2d&                  afirstvector,
                                   const gp_Vec2d&                  asecondvector,
                                   const gp_Vec2d&                  VecRef,
                                   const double                     adirection,
                                   double&                          aparameter,
                                   bool&                            asense,
                                   bool&                            astatus,
                                   bool                             IsBisecOfTwoLines)
{
  astatus = true;

  gp_Hypr2d  gphyperbola;
  gp_Parab2d gpparabola;
  gp_Elips2d gpellipse;
  gp_Circ2d  gpcircle;
  gp_Lin2d   gpline;

  double   distance = 0.;
  gp_Vec2d tangent;
  gp_Pnt2d point;

  GccInt_IType type = abisector->ArcType();

  if (type == GccInt_Lin)
  {
    gpline     = abisector->Line();
    aparameter = ElCLib::Parameter(gpline, apoint);
    ElCLib::D1(aparameter, gpline, point, tangent);
  }
  else if (type == GccInt_Cir)
  {
    gpcircle   = abisector->Circle();
    aparameter = ElCLib::Parameter(gpcircle, apoint);
    ElCLib::D1(aparameter, gpcircle, point, tangent);
  }
  else if (type == GccInt_Hpr)
  {
    gphyperbola = abisector->Hyperbola();
    aparameter  = ElCLib::Parameter(gphyperbola, apoint);
    ElCLib::D1(aparameter, gphyperbola, point, tangent);
  }
  else if (type == GccInt_Par)
  {
    gpparabola = abisector->Parabola();
    aparameter = ElCLib::Parameter(gpparabola, apoint);
    ElCLib::D1(aparameter, gpparabola, point, tangent);
  }
  else if (type == GccInt_Ell)
  {
    gpellipse  = abisector->Ellipse();
    aparameter = ElCLib::Parameter(gpellipse, apoint);
    ElCLib::D1(aparameter, gpellipse, point, tangent);
  }

  distance = apoint.Distance(point);

  gp_Dir2d afirstdir(afirstvector);
  gp_Dir2d aseconddir(asecondvector);
  gp_Dir2d tangdir(tangent);
  gp_Dir2d secdirrev = aseconddir.Reversed();

  // 1st passage to learn if the curve is in the proper sector

  if (asense)
  {
    // the status is determined only in case on curve ie:
    // tangent to the bissectrice is bisectrice of two vectors.
    double SinPlat = 1.e-3;
    if (std::abs(afirstdir ^ aseconddir) < SinPlat)
    { // flat
      if (afirstdir * aseconddir >= 0.0)
      { // tangent mixed
        // correct if the scalar product is close to 1.
        if (std::abs(tangdir * afirstdir) < 0.5)
        {
          astatus = false;
        }
      }
      else
      { // opposed tangents.
        // correct if the scalar product is close to 0.
        if (std::abs(tangdir * afirstdir) > 0.5)
        {
          astatus = false;
        }
      }
    }
    else if ((afirstdir ^ tangdir) * (tangdir ^ aseconddir) < -1.E-8)
    {
      astatus = false;
    }
  }
  else
  {
    asense = true;

    if (!IsBisecOfTwoLines)
    {
      //  Modified by Sergey KHROMOV - Tue Oct 22 16:35:51 2002 Begin
      // Replacement of -1.E-8 for a tolerance 1.e-4
      double aTol = 1.e-4;

      if ((afirstdir ^ secdirrev) * adirection < -0.1)
      { // input
        if ((afirstdir ^ tangdir) * adirection < aTol && (secdirrev ^ tangdir) * adirection < aTol)
          asense = false;
      }
      else if ((afirstdir ^ secdirrev) * adirection > 0.1)
      { // output
        if ((afirstdir ^ tangdir) * adirection < aTol || (secdirrev ^ tangdir) * adirection < aTol)
          asense = false;
      }
      else
      { // flat
        if (afirstdir.Dot(secdirrev) > 0.)
        { // tangent
          if ((afirstdir ^ tangdir) * adirection < 0.)
            asense = false;
        }
        else
        { // turn back
          //  Modified by Sergey KHROMOV - Thu Oct 31 14:16:53 2002
          // 	if ((afirstdir.Dot(tangdir))*adirection > 0.) asense = false;
          if (afirstdir.Dot(tangdir) < 0.)
            asense = false;
          //  Modified by Sergey KHROMOV - Thu Oct 31 14:16:54 2002
        }
      }
      // jgv: for OCC26185
      if (VecRef.SquareMagnitude() != 0)
      {
        gp_Dir2d DirRef = VecRef;
        if (tangdir * DirRef < 0.)
          asense = false;
      }
      ///////////////////
      //  Modified by Sergey KHROMOV - Tue Oct 22 16:35:51 2002 End
    }
  }
  return distance;
}

//===========================================================================
//    calculate the bissectrice between two curves coming from a point.         +
//                                                                          +
//   afirstcurve   : \ curves the bissectrice between which will be calculated.      +
//   asecondcurve  : /                                          +
//   apoint        :   point through which the bissectrice should pass.         +
//   afirstvector  : \ vectors to find the sector where       +
//   asecondvector : / the bissectrice should be located.                      +
//   adirection    :   shows the side of the bissectrice to be preserved.       +
//   tolerance     :   threshold starting from which the bisectrices are degenerated +
//===========================================================================
void Bisector_BisecAna::Perform(const occ::handle<Geom2d_Curve>& afirstcurve,
                                const occ::handle<Geom2d_Curve>& asecondcurve,
                                const gp_Pnt2d&                  apoint,
                                const gp_Vec2d&                  afirstvector,
                                const gp_Vec2d&                  asecondvector,
                                const double                     adirection,
                                const GeomAbs_JoinType           ajointype,
                                const double                     tolerance,
                                const bool                       oncurve)
{

  bool             ok;
  double           distanceptsol, parameter, firstparameter = 0.;
  bool             thesense = false, sense;
  double           distancemini;
  int              nbsolution;
  constexpr double PreConf = Precision::Confusion();

  occ::handle<Standard_Type> type1 = afirstcurve->DynamicType();
  occ::handle<Standard_Type> type2 = asecondcurve->DynamicType();
  occ::handle<Geom2d_Curve>  CurveF;
  occ::handle<Geom2d_Curve>  CurveE;
  occ::handle<GccInt_Bisec>  TheSol;

  // jgv: for OCC26296
  gp_Vec2d LineBisVec(0., 0.);
  gp_Vec2d tan1, tan2;
  gp_Pnt2d Pnt1, Pnt2;
  afirstcurve->D1(afirstcurve->LastParameter(), Pnt1, tan1);
  asecondcurve->D1(asecondcurve->FirstParameter(), Pnt2, tan2);
  if (!oncurve)
  {
    LineBisVec = gp_Vec2d(Pnt1, Pnt2);
    LineBisVec.Rotate(M_PI / 2.);
  }
  ///////////////////

  tan1.Reverse();

  if (type1 == STANDARD_TYPE(Geom2d_TrimmedCurve))
    CurveF = occ::down_cast<Geom2d_TrimmedCurve>(afirstcurve)->BasisCurve();
  else
    CurveF = afirstcurve;

  if (type2 == STANDARD_TYPE(Geom2d_TrimmedCurve))
    CurveE = occ::down_cast<Geom2d_TrimmedCurve>(asecondcurve)->BasisCurve();
  else
    CurveE = asecondcurve;

  type1         = CurveF->DynamicType();
  type2         = CurveE->DynamicType();
  int       cas = 0;
  gp_Circ2d circle1, circle2;
  gp_Lin2d  line1, line2;

  //=============================================================================
  //                Determination of the nature of arguments.                   +
  //=============================================================================

  if (type1 == STANDARD_TYPE(Geom2d_Circle))
  {
    if (type2 == STANDARD_TYPE(Geom2d_Circle))
    {
      cas                           = 1;
      occ::handle<Geom2d_Circle> C1 = occ::down_cast<Geom2d_Circle>(CurveF);
      circle1                       = C1->Circ2d();
      occ::handle<Geom2d_Circle> C2 = occ::down_cast<Geom2d_Circle>(CurveE);
      circle2                       = C2->Circ2d();
    }
    else if (type2 == STANDARD_TYPE(Geom2d_Line))
    {
      cas                           = 2;
      occ::handle<Geom2d_Circle> C1 = occ::down_cast<Geom2d_Circle>(CurveF);
      circle1                       = C1->Circ2d();
      occ::handle<Geom2d_Line> L2   = occ::down_cast<Geom2d_Line>(CurveE);
      line2                         = L2->Lin2d();
    }
    else
    {
      std::cout << "Not yet implemented" << std::endl;
    }
  }
  else if (type1 == STANDARD_TYPE(Geom2d_Line))
  {
    if (type2 == STANDARD_TYPE(Geom2d_Circle))
    {
      cas                           = 2;
      occ::handle<Geom2d_Circle> C1 = occ::down_cast<Geom2d_Circle>(CurveE);
      circle1                       = C1->Circ2d();
      occ::handle<Geom2d_Line> L2   = occ::down_cast<Geom2d_Line>(CurveF);
      line2                         = L2->Lin2d();
    }
    else if (type2 == STANDARD_TYPE(Geom2d_Line))
    {
      cas                         = 3;
      occ::handle<Geom2d_Line> L1 = occ::down_cast<Geom2d_Line>(CurveF);
      line1                       = L1->Lin2d();
      occ::handle<Geom2d_Line> L2 = occ::down_cast<Geom2d_Line>(CurveE);
      line2                       = L2->Lin2d();
    }
    else
    {
      std::cout << "Not yet implemented" << std::endl;
    }
  }
  else
  {
    std::cout << "Not yet implemented" << std::endl;
  }

  switch (cas)
  {

      //=============================================================================
      //                       Bissectrice circle - circle.                         +
      //=============================================================================

    case 1: {
      double radius1 = circle1.Radius();
      double radius2 = circle2.Radius();

      //-----------------------------------------------------
      // Particular case when two circles are mixed.
      //-----------------------------------------------------
      if (circle1.Location().IsEqual(circle2.Location(), PreConf)
          && (std::abs(radius1 - radius2) <= PreConf))
      {
        gp_Pnt2d P1 = afirstcurve->Value(afirstcurve->LastParameter());
        gp_Pnt2d P2 = asecondcurve->Value(asecondcurve->FirstParameter());
        gp_Pnt2d PMil;
        gp_Lin2d line;
        PMil = gp_Pnt2d((P1.X() + P2.X()) / 2., (P1.Y() + P2.Y()) / 2.);
        //  Modified by skv - Fri Jul  1 16:23:32 2005 IDEM(Airbus) Begin
        //       line = gp_Lin2d(PMil,
        // 		      gp_Dir2d(circle1.Location().X() - PMil.X(),
        // 			       circle1.Location().Y() - PMil.Y()));
        if (!circle1.Location().IsEqual(PMil, PreConf))
        {
          // PMil doesn't coincide with the circle location.
          line = gp_Lin2d(
            PMil,
            gp_Dir2d(circle1.Location().X() - PMil.X(), circle1.Location().Y() - PMil.Y()));
        }
        else if (radius1 >= PreConf)
        {
          // PMil coincides with the circle location and radius is greater then 0.
          line =
            gp_Lin2d(circle1.Location(),
                     gp_Dir2d(P1.Y() - circle1.Location().Y(), circle1.Location().X() - P1.X()));
        }
        else
        {
          // radius is equal to 0. No matter what direction to chose.
          line = gp_Lin2d(circle1.Location(), gp_Dir2d(gp_Dir2d::D::X));
        }
        //  Modified by skv - Fri Jul  1 16:23:32 2005 IDEM(Airbus) End
        occ::handle<GccInt_Bisec> solution = new GccInt_BLine(line);
        sense                              = false;
        //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration Begin
        //       distanceptsol = Distance(apoint,solution,
        // 			       afirstvector,asecondvector,
        // 			       adirection,parameter,sense,ok);
        if (oncurve)
          distanceptsol =
            Distance(apoint, solution, tan2, tan1, LineBisVec, adirection, parameter, sense, ok);
        else
          distanceptsol = Distance(apoint,
                                   solution,
                                   afirstvector,
                                   asecondvector,
                                   LineBisVec,
                                   adirection,
                                   parameter,
                                   sense,
                                   ok);
        //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration End
        occ::handle<Geom2d_Curve> bisectorcurve = new Geom2d_Line(line);
        if (!sense)
          thebisector = new Geom2d_TrimmedCurve(bisectorcurve, parameter, -Precision::Infinite());
        else
        {
          double parameter2;
          parameter2 = ElCLib::Parameter(line, circle1.Location());
          parameter2 += 1.e-8;
          thebisector = new Geom2d_TrimmedCurve(bisectorcurve, parameter, parameter2);
        }
        break;
      } // end of case mixed circles.

      if (radius1 < radius2)
      {
        gp_Circ2d circle = circle1;
        circle1          = circle2;
        circle2          = circle;

        double radius = radius1;
        radius1       = radius2;
        radius2       = radius;
      }

      // small reframing of circles. in the case when the circles
      // are OnCurve , if they are almost tangent they become tangent.
      double EntreAxe       = circle1.Location().Distance(circle2.Location());
      double D1             = 0.5 * (radius1 - EntreAxe - radius2);
      bool   CirclesTangent = false;

      //  Modified by Sergey KHROMOV - Thu Oct 31 12:42:21 2002 End
      //     if ( oncurve && std::abs(D1) <  PreConf) {
      if (oncurve && std::abs(D1) < PreConf && tan1.IsParallel(tan2, 1.e-8))
      {
        //  Modified by Sergey KHROMOV - Thu Oct 31 12:42:22 2002 Begin
        // C2 included in C1 and tangent.
        circle1.SetRadius(radius1 - D1);
        circle2.SetRadius(radius2 + D1);
        CirclesTangent = true;
      }
      else
      {
        D1 = 0.5 * (radius1 - EntreAxe + radius2);
        //  Modified by Sergey KHROMOV - Thu Oct 31 12:44:24 2002 Begin
        //       if (oncurve && std::abs(D1) < PreConf) {
        if (oncurve && std::abs(D1) < PreConf && tan1.IsParallel(tan2, 1.e-8))
        {
          //  Modified by Sergey KHROMOV - Thu Oct 31 12:44:25 2002 End
          // C2 and C1 tangent and disconnected.
          circle1.SetRadius(radius1 - D1);
          circle2.SetRadius(radius2 - D1);
          CirclesTangent = true;
        }
      } // end of reframing.

      GccAna_Circ2dBisec Bisector(circle1, circle2);

      distancemini = Precision::Infinite();

      if (Bisector.IsDone())
      {
        nbsolution = Bisector.NbSolutions();
        for (int i = 1; i <= nbsolution; i++)
        {
          occ::handle<GccInt_Bisec> solution = Bisector.ThisSolution(i);
          Degenerate(solution, tolerance);
          sense = true;
          if (oncurve)
          {
            distanceptsol =
              Distance(apoint, solution, tan1, tan2, LineBisVec, adirection, parameter, sense, ok);
          }
          else
          {
            ok = true;
          }

          if (ok)
          {
            sense = false;
            //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration Begin
            //       distanceptsol = Distance(apoint,solution,
            // 			       afirstvector,asecondvector,
            // 			       adirection,parameter,sense,ok);
            if (oncurve)
              distanceptsol = Distance(apoint,
                                       solution,
                                       tan2,
                                       tan1,
                                       LineBisVec,
                                       adirection,
                                       parameter,
                                       sense,
                                       ok);
            else
              distanceptsol = Distance(apoint,
                                       solution,
                                       afirstvector,
                                       asecondvector,
                                       LineBisVec,
                                       adirection,
                                       parameter,
                                       sense,
                                       ok);
            //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration End
            if (distanceptsol <= distancemini)
            {
              TheSol         = solution;
              firstparameter = parameter;
              thesense       = sense;
              distancemini   = distanceptsol;
            }
          }
        }
        if (!TheSol.IsNull())
        {
          occ::handle<Geom2d_Curve> bisectorcurve;
          GccInt_IType              type = TheSol->ArcType();
          if (type == GccInt_Lin)
          {
            gp_Lin2d gpline = TheSol->Line();
            bisectorcurve   = new Geom2d_Line(gpline);

            double secondparameter = Precision::Infinite();
            if (!thesense)
              secondparameter = -Precision::Infinite();

            if (oncurve)
            {
              // bisectrice right and oncurve
              // is cut between two circle of the same radius if circles are tangent.

              // if tangent flat and the bissectrice at the side of the concavity
              // of one of the circles. the bissectrice is a segment of the point common to
              // first of 2 centers of circle that it meets.
              // in this case it is important to set a segmnent for
              // intersection in Tool2d.

              if (CirclesTangent)
              {
                //  Modified by skv - Tue Apr 13 17:23:31 2004 IDEM(Airbus) Begin
                //  Trying to correct the line if the distance between it
                //  and the reference point is too big.
                if (distancemini > tolerance)
                {
                  gp_Pnt2d aPloc = gpline.Location();
                  gp_Dir2d aNewDir(apoint.XY() - aPloc.XY());
                  gp_Lin2d aNewLin(aPloc, aNewDir);
                  gp_Pnt2d aCC2     = circle2.Location();
                  double   aNewDMin = aNewLin.Distance(apoint);
                  double   aTolConf = 1.e-3;
                  // Hope, aNewDMin is equal to 0...

                  if (aNewLin.Distance(aCC2) <= aTolConf)
                  {
                    distancemini   = aNewDMin;
                    firstparameter = ElCLib::Parameter(aNewLin, apoint);
                    bisectorcurve  = new Geom2d_Line(aNewLin);
                  }
                }
                //  Modified by skv - Tue Apr 13 17:23:32 2004 IDEM(Airbus) End
                if (tan1.Dot(tan2) < 0.)
                {
                  // flat and not turn back.
                  double Par1   = ElCLib::Parameter(gpline, circle1.Location());
                  double Par2   = ElCLib::Parameter(gpline, circle2.Location());
                  double MinPar = std::min(Par1, Par2);
                  double MaxPar = std::max(Par1, Par2);

                  if (!thesense)
                  {
                    if (MaxPar < firstparameter)
                      secondparameter = MaxPar - 1.E-8;
                    else if (MinPar < firstparameter)
                      secondparameter = MinPar - 1.E-8;
                  }
                  else
                  {
                    if (MinPar > firstparameter)
                      secondparameter = MinPar + 1.E-8;
                    else if (MaxPar > firstparameter)
                      secondparameter = MaxPar + 1.E-8;
                  }
                }
              }
            }

            thebisector = new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, secondparameter);
          }
          else if (type == GccInt_Cir)
          {
            bisectorcurve = new Geom2d_Circle(TheSol->Circle());
            if (!thesense)
              thebisector = new Geom2d_TrimmedCurve(bisectorcurve,
                                                    firstparameter - 2.0 * M_PI,
                                                    firstparameter,
                                                    thesense);
            else
              thebisector = new Geom2d_TrimmedCurve(bisectorcurve,
                                                    firstparameter,
                                                    firstparameter + 2.0 * M_PI,
                                                    thesense);
          }
          else if (type == GccInt_Hpr)
          {
            bisectorcurve = new Geom2d_Hyperbola(TheSol->Hyperbola());
            if (!thesense)
              thebisector =
                new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, -Precision::Infinite());
            else
              thebisector =
                new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, Precision::Infinite());
          }
          else if (type == GccInt_Ell)
          {
            bisectorcurve = new Geom2d_Ellipse(TheSol->Ellipse());
            if (!thesense)
              thebisector = new Geom2d_TrimmedCurve(bisectorcurve,
                                                    firstparameter - 2.0 * M_PI,
                                                    firstparameter,
                                                    thesense);
            else
              thebisector = new Geom2d_TrimmedCurve(bisectorcurve,
                                                    firstparameter,
                                                    firstparameter + 2.0 * M_PI,
                                                    thesense);
          }
        }
      }
    }
    break;

      //=============================================================================
      //                       Bissectrice circle - straight.                         +
      //=============================================================================

    case 2: {
      // small reframing of circles. in case OnCurve.
      // If the circle and the straight line are almost tangent they become tangent.
      if (oncurve)
      {
        double radius1 = circle1.Radius();
        double D1      = (line2.Distance(circle1.Location()) - radius1);
        //  Modified by Sergey KHROMOV - Wed Oct 30 14:48:43 2002 Begin
        //       if (std::abs(D1) < PreConf) {
        if (std::abs(D1) < PreConf && tan1.IsParallel(tan2, 1.e-8))
        {
          //  Modified by Sergey KHROMOV - Wed Oct 30 14:48:44 2002 End
          circle1.SetRadius(radius1 + D1);
        }
      }

      GccAna_CircLin2dBisec Bisector(circle1, line2);

      distancemini = Precision::Infinite();

      if (Bisector.IsDone())
      {
        nbsolution = Bisector.NbSolutions();
        for (int i = 1; i <= nbsolution; i++)
        {
          occ::handle<GccInt_Bisec> solution = Bisector.ThisSolution(i);
          Degenerate(solution, tolerance);
          sense = true;
          distanceptsol =
            Distance(apoint, solution, tan1, tan2, LineBisVec, adirection, parameter, sense, ok);
          if (ok || !oncurve)
          {
            sense = false;
            //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration Begin
            //       distanceptsol = Distance(apoint,solution,
            // 			       afirstvector,asecondvector,
            // 			       adirection,parameter,sense,ok);
            if (oncurve)
              distanceptsol = Distance(apoint,
                                       solution,
                                       tan2,
                                       tan1,
                                       LineBisVec,
                                       adirection,
                                       parameter,
                                       sense,
                                       ok);
            else
              distanceptsol = Distance(apoint,
                                       solution,
                                       afirstvector,
                                       asecondvector,
                                       LineBisVec,
                                       adirection,
                                       parameter,
                                       sense,
                                       ok);
            //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration End
            if (distanceptsol <= distancemini)
            {
              TheSol         = solution;
              firstparameter = parameter;
              thesense       = sense;
              distancemini   = distanceptsol + 1.e-8;
            }
          }
        }
        if (!TheSol.IsNull())
        {
          GccInt_IType              type = TheSol->ArcType();
          occ::handle<Geom2d_Curve> bisectorcurve;
          if (type == GccInt_Lin)
          {
            // -----------------------------------------------------------------
            // If the bisectrice is a line
            //       => the straight line is tangent to the circle.
            //       It the part of bisectrice concerned is at the side of the center.
            //       => the bisectrice is limited by the point and the center of the circle.
            // Note : In the latter case the bisectrice is a degenerated parabole.
            // -----------------------------------------------------------------
            gp_Pnt2d circlecenter;
            gp_Lin2d gpline;
            double   secondparameter;

            circlecenter    = circle1.Location();
            gpline          = TheSol->Line();
            secondparameter = ElCLib::Parameter(gpline, circlecenter);
            bisectorcurve   = new Geom2d_Line(gpline);

            if (!thesense)
            {
              if (secondparameter > firstparameter)
              {
                secondparameter = -Precision::Infinite();
              }
              else
              {
                secondparameter = secondparameter - 1.E-8;
              }
            }
            else
            {
              if (secondparameter < firstparameter)
              {
                secondparameter = Precision::Infinite();
              }
              else
              {
                secondparameter = secondparameter + 1.E-8;
              }
            }

            thebisector = new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, secondparameter);
          }
          else if (type == GccInt_Par)
          {
            bisectorcurve                = new Geom2d_Parabola(TheSol->Parabola());
            gp_Pnt2d     apex            = bisectorcurve->Value(0.);
            gp_Pnt2d     firstpnt        = bisectorcurve->Value(firstparameter);
            double       ChordLen        = apex.Distance(firstpnt);
            const double TolPar          = 1.e-5;
            double       secondparameter = Precision::Infinite();
            if (!thesense)
            {
              if (ajointype == GeomAbs_Intersection && TolPar < firstparameter
                  && ChordLen
                       >= circle1.Radius()) // first parameter is too far from peak of parabola
                secondparameter = 0.;
              thebisector =
                new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, -secondparameter);
            }
            else
            {
              if (ajointype == GeomAbs_Intersection && firstparameter < -TolPar
                  && ChordLen
                       >= circle1.Radius()) // first parameter is too far from peak of parabola
                secondparameter = 0.;
              thebisector = new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, secondparameter);
            }
          }
        }
      }
    }
    break;

      //=============================================================================
      //                       Bissectrice straight - straight.                     +
      //=============================================================================
    case 3: {
      gp_Dir2d Direc1(line1.Direction());
      gp_Dir2d Direc2(line2.Direction());
      gp_Lin2d line;
      distancemini = Precision::Infinite();

      //  Modified by Sergey KHROMOV - Tue Sep 10 15:58:43 2002 Begin
      //  Change to the same criterion as in MAT2d_Circuit.cxx:
      //         method MAT2d_Circuit::InitOpen(..)
      //     if (Direc1.IsParallel(Direc2,RealEpsilon())) {
      if (Direc1.IsParallel(Direc2, 1.e-8))
      {
        //  Modified by Sergey KHROMOV - Tue Sep 10 15:58:45 2002 End
        if (line1.Distance(line2.Location()) / 2. <= Precision::Confusion())
          line = gp_Lin2d(apoint, gp_Dir2d(-line1.Direction().Y(), line1.Direction().X()));
        else
          line = gp_Lin2d(apoint, line2.Direction());

        occ::handle<GccInt_Bisec> solution = new GccInt_BLine(line);
        //  Modified by skv - Wed Jul  7 17:21:09 2004 IDEM(Airbus) Begin
        //       sense = true;
        //       distanceptsol = Distance(apoint,solution,
        // 			       tan1,tan2,
        // 			       adirection,parameter,sense,ok);
        //       theSense = sense;
        //       if (ok || !oncurve) {
        sense = false;
        //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration Begin
        //       distanceptsol = Distance(apoint,solution,
        // 			       afirstvector,asecondvector,
        // 			       adirection,parameter,sense,ok);
        if (oncurve)
          distanceptsol =
            Distance(apoint, solution, tan2, tan1, LineBisVec, adirection, parameter, sense, ok);
        else
          distanceptsol = Distance(apoint,
                                   solution,
                                   afirstvector,
                                   asecondvector,
                                   LineBisVec,
                                   adirection,
                                   parameter,
                                   sense,
                                   ok);
        //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration End
        // 	if (distanceptsol <= distancemini) {
        firstparameter = parameter;
        occ::handle<Geom2d_Curve> bisectorcurve;
        bisectorcurve = new Geom2d_Line(line);
        if (!sense)
          thebisector =
            new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, -Precision::Infinite());
        else
          thebisector =
            new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, Precision::Infinite());
        // 	}
        //       }
        //  Modified by skv - Wed Jul  7 17:21:09 2004 IDEM(Airbus) End
      }
      else
      {
        gp_Lin2d                  l(apoint, gp_Dir2d(Direc2.XY() - Direc1.XY()));
        occ::handle<GccInt_Bisec> solution = new GccInt_BLine(l);
        bool                      isOk;
        sense = false;
        //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration Begin
        //       distanceptsol = Distance(apoint,solution,
        // 			       afirstvector,asecondvector,
        // 			       adirection,parameter,sense,ok);
        if (oncurve)
          distanceptsol =
            Distance(apoint, solution, tan2, tan1, LineBisVec, adirection, parameter, sense, isOk);
        else
          distanceptsol = Distance(apoint,
                                   solution,
                                   afirstvector,
                                   asecondvector,
                                   LineBisVec,
                                   adirection,
                                   parameter,
                                   sense,
                                   isOk,
                                   true);
        //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration End
        if (isOk || !oncurve)
        {
          thesense     = sense;
          distancemini = distanceptsol;
        }
        TheSol = new GccInt_BLine(l);
        occ::handle<Geom2d_Curve> bisectorcurve;
        bisectorcurve = new Geom2d_Line(TheSol->Line());
        if (!thesense)
          thebisector = new Geom2d_TrimmedCurve(bisectorcurve, 0., -Precision::Infinite());
        else
          thebisector = new Geom2d_TrimmedCurve(bisectorcurve, 0., Precision::Infinite());
      }
    }
    break;

    default:
      throw StdFail_NotDone();
      break;
  }
}

//===========================================================================
//  calculate the bissectrice between a curve and a point and starting in a point. +
//                                                                          +
//   afirstcurve   : \ curve and point the bissectrice between which is calculated +
//   asecondpoint  : /                                          +
//   apoint        :   point through which the bissectrice should pass.         +
//   afirstvector  : \ vectors to determine the sector in which      +
//   asecondvector : / the bissectrice should be located.                      +
//   adirection    :   shows the side of the bissectrice to be preserved.       +
//   tolerance     :   threshold starting from which the bisectrices are degenerated+
//===========================================================================

void Bisector_BisecAna::Perform(const occ::handle<Geom2d_Curve>& afirstcurve,
                                const occ::handle<Geom2d_Point>& asecondpoint,
                                const gp_Pnt2d&                  apoint,
                                const gp_Vec2d&                  afirstvector,
                                const gp_Vec2d&                  asecondvector,
                                const double                     adirection,
                                const double                     tolerance,
                                const bool                       oncurve)
{
  bool                      ok;
  bool                      thesense = false, sense;
  double                    distanceptsol, parameter, firstparameter = 0., secondparameter;
  gp_Vec2d                  VecRef(0., 0.);
  occ::handle<Geom2d_Curve> curve;
  occ::handle<GccInt_Bisec> TheSol;

  gp_Circ2d circle;
  gp_Lin2d  line;
  gp_Pnt2d  circlecenter;

  int cas = 0;

  occ::handle<Standard_Type> aFirstCurveType = afirstcurve->DynamicType();

  if (aFirstCurveType == STANDARD_TYPE(Geom2d_TrimmedCurve))
  {
    curve = occ::down_cast<Geom2d_TrimmedCurve>(afirstcurve)->BasisCurve();
  }
  else
  {
    curve = afirstcurve;
  }

  aFirstCurveType = curve->DynamicType();
  const gp_Pnt2d aPoint(asecondpoint->Pnt2d());
  (void)aPoint;
  if (aFirstCurveType == STANDARD_TYPE(Geom2d_Circle))
  {
    cas                           = 1;
    occ::handle<Geom2d_Circle> C1 = occ::down_cast<Geom2d_Circle>(curve);
    circle                        = C1->Circ2d();
  }
  else if (aFirstCurveType == STANDARD_TYPE(Geom2d_Line))
  {
    cas                         = 2;
    occ::handle<Geom2d_Line> L1 = occ::down_cast<Geom2d_Line>(curve);
    line                        = L1->Lin2d();
  }
  else
  {
    std::cout << "Not yet implemented" << std::endl;
  }

  switch (cas)
  {

      //=============================================================================
      //                       Bissectrice point - circle.                          +
      //=============================================================================
    case 1: {
      GccAna_CircPnt2dBisec Bisector(circle, asecondpoint->Pnt2d(), tolerance);
      double                distancemini = Precision::Infinite();
      if (Bisector.IsDone())
      {
        int nbsolution = Bisector.NbSolutions();
        for (int i = 1; i <= nbsolution; i++)
        {
          occ::handle<GccInt_Bisec> solution = Bisector.ThisSolution(i);
          Degenerate(solution, tolerance);
          sense         = false;
          distanceptsol = Distance(apoint,
                                   solution,
                                   afirstvector,
                                   asecondvector,
                                   VecRef,
                                   adirection,
                                   parameter,
                                   sense,
                                   ok);

          if (distanceptsol <= distancemini)
          {
            TheSol         = solution;
            firstparameter = parameter;
            thesense       = sense;
            distancemini   = distanceptsol;
          }
        }
        if (!TheSol.IsNull())
        {
          GccInt_IType              aSolType = TheSol->ArcType();
          occ::handle<Geom2d_Curve> bisectorcurve;
          if (aSolType == GccInt_Lin)
          {

            // ----------------------------------------------------------------------------
            // If the bisectrice is a line
            //       => the point is on the circle.
            //       If the part of bisectrice concerned is at the side of the center.
            //       => the bisectrice is limited by the point and the center of the circle.
            // Note : In this latter case the bisectrice is actually an ellipse of small null axis.
            // ----------------------------------------------------------------------------

            circlecenter    = circle.Location();
            line            = TheSol->Line();
            secondparameter = ElCLib::Parameter(line, circlecenter);
            bisectorcurve   = new Geom2d_Line(line);

            if (!thesense)
            {
              if (secondparameter > firstparameter)
              {
                secondparameter = -Precision::Infinite();
              }
              else
              {
                secondparameter = secondparameter - 1.E-8;
              }
            }
            else
            {
              if (secondparameter < firstparameter)
              {
                secondparameter = Precision::Infinite();
              }
              else
              {
                secondparameter = secondparameter + 1.E-8;
              }
            }

            thebisector = new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, secondparameter);
          }
          else if (aSolType == GccInt_Cir)
          {
            bisectorcurve = new Geom2d_Circle(TheSol->Circle());
            if (!thesense)
              thebisector = new Geom2d_TrimmedCurve(bisectorcurve,
                                                    firstparameter - 2.0 * M_PI,
                                                    firstparameter,
                                                    thesense);
            else
              thebisector = new Geom2d_TrimmedCurve(bisectorcurve,
                                                    firstparameter,
                                                    firstparameter + 2.0 * M_PI,
                                                    thesense);
          }
          else if (aSolType == GccInt_Hpr)
          {
            bisectorcurve = new Geom2d_Hyperbola(TheSol->Hyperbola());
            if (!thesense)
              thebisector =
                new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, -Precision::Infinite());
            else
              thebisector =
                new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, Precision::Infinite());
          }
          else if (aSolType == GccInt_Ell)
          {
            bisectorcurve = new Geom2d_Ellipse(TheSol->Ellipse());
            if (!thesense)
              thebisector = new Geom2d_TrimmedCurve(bisectorcurve,
                                                    firstparameter - 2.0 * M_PI,
                                                    firstparameter,
                                                    thesense);
            else
              thebisector = new Geom2d_TrimmedCurve(bisectorcurve,
                                                    firstparameter,
                                                    firstparameter + 2.0 * M_PI,
                                                    thesense);
          }
        }
      }
    }
    break;

      //=============================================================================
      //                       Bissectrice point - straight.                          +
      //=============================================================================
    case 2: {
      GccAna_LinPnt2dBisec Bisector(line, asecondpoint->Pnt2d());

#ifdef OCCT_DEBUG
      gp_Vec2d V(line.Direction());
#else
      line.Direction();
#endif
      occ::handle<GccInt_Bisec> solution = Bisector.ThisSolution();
      Degenerate(solution, tolerance);
      GccInt_IType              type = solution->ArcType();
      occ::handle<Geom2d_Curve> bisectorcurve;

      if (type == GccInt_Lin)
      {
        bisectorcurve = new Geom2d_Line(solution->Line());
      }
      else if (type == GccInt_Par)
      {
        bisectorcurve = new Geom2d_Parabola(solution->Parabola());
      }
      sense         = false;
      distanceptsol = Distance(apoint,
                               solution,
                               afirstvector,
                               asecondvector,
                               VecRef,
                               adirection,
                               parameter,
                               sense,
                               ok);

      if (ok || !oncurve)
      {
        firstparameter = parameter;
        thesense       = sense;
      }

      if (!thesense)
        thebisector =
          new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, -Precision::Infinite());
      else
        thebisector = new Geom2d_TrimmedCurve(bisectorcurve, firstparameter, Precision::Infinite());
    }
    break;

    default: {
      std::cout << "Not yet implemented" << std::endl;
      break;
    }
  }
}

//===========================================================================
//  calculate the bissectrice between a curve and a point starting at a point. +
//                                                                          +
//   afirstpoint   : \ curves between which the                             +
//   asecondcurve  : / bissectrice is calculated.                           +
//   apoint        :   point through which the bissectrice should pass.     +
//   afirstvector  : \ vectors to determine the secteur in which            +
//   asecondvector : / the bissectrice should be located.                   +
//   adirection    :   shows the side of the bissectrice to be preserved.   +
//   tolerance     :   threshold at which the bisectrices become degenerated+
//===========================================================================

void Bisector_BisecAna::Perform(const occ::handle<Geom2d_Point>& afirstpoint,
                                const occ::handle<Geom2d_Curve>& asecondcurve,
                                const gp_Pnt2d&                  apoint,
                                const gp_Vec2d&                  afirstvector,
                                const gp_Vec2d&                  asecondvector,
                                const double                     adirection,
                                //				const double         tolerance    ,
                                const double,
                                const bool oncurve)

{
  double adirectionreverse = -adirection;
  Perform(asecondcurve,
          afirstpoint,
          apoint,
          asecondvector,
          afirstvector,
          adirectionreverse,
          //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration Begin
          0.,
          //  Modified by skv - Tue Feb 15 17:51:29 2005 Integration End
          oncurve);
}

//===========================================================================
//        calculate the bissectrice between two points starting at a point. +
//                                                                          +
//   afirstpoint   : \ curves between which the                             +
//   asecondpoint  : / bissectrice is calculated.                           +
//   apoint        :   point through which the bissectrice should pass.     +
//   afirstvector  : \ vectors to determine the sector in which the         +
//   asecondvector : / bissectrice should be located.                       +
//   adirection    :   shows the side of the bissectrice to be preserved.   +
//===========================================================================

void Bisector_BisecAna::Perform(const occ::handle<Geom2d_Point>& afirstpoint,
                                const occ::handle<Geom2d_Point>& asecondpoint,
                                const gp_Pnt2d&                  apoint,
                                const gp_Vec2d&                  afirstvector,
                                const gp_Vec2d&                  asecondvector,
                                const double                     adirection,
                                //				const double         tolerance    ,
                                const double,
                                const bool oncurve)
{
  bool     sense, ok;
  double   parameter;
  gp_Vec2d VecRef(0., 0.);

  GccAna_Pnt2dBisec         bisector(afirstpoint->Pnt2d(), asecondpoint->Pnt2d());
  gp_Lin2d                  line     = bisector.ThisSolution();
  occ::handle<GccInt_Bisec> solution = new GccInt_BLine(line);

  sense = false;
  Distance(apoint, solution, afirstvector, asecondvector, VecRef, adirection, parameter, sense, ok);
  if (ok || !oncurve)
  {
    occ::handle<Geom2d_Curve> bisectorcurve = new Geom2d_Line(line);
    if (!sense)
      thebisector = new Geom2d_TrimmedCurve(bisectorcurve, parameter, -Precision::Infinite());
    else
      thebisector = new Geom2d_TrimmedCurve(bisectorcurve, parameter, Precision::Infinite());
  }
}

//=================================================================================================

bool Bisector_BisecAna::IsExtendAtStart() const
{
  return false;
}

//=================================================================================================

bool Bisector_BisecAna::IsExtendAtEnd() const
{
  return false;
}

//=============================================================================
// function : SetTrim
// purpose  : Restriction of the bissectrice by the domain of the curve Cu.
//           The domain of the curve is the set of points that are closer to the
//           than to its extremities.
//           For the calculation the domain is extended. Extension of Epsilon1 of the
//           First and the Last parameter of the curve.
//=============================================================================
// void Bisector_BisecAna::SetTrim(const occ::handle<Geom2d_Curve>& Cu)
void Bisector_BisecAna::SetTrim(const occ::handle<Geom2d_Curve>&)
{
  /*
    occ::handle<Standard_Type>       Type;
    occ::handle<Geom2d_Curve>        TheCurve;
    occ::handle<Geom2d_Circle>       CircleCu;
    occ::handle<Geom2d_Line>         LineCu;
    occ::handle<Geom2d_Curve>        FirstLimit;
    occ::handle<Geom2d_Curve>        LastLimit;

    gp_Lin2d                    gpLine;
    gp_Pnt2d                    P, PFirst, PLast, FirstPointBisector, Center;
    gp_Vec2d                    TanFirst, TanLast;

    IntRes2d_Domain             FirstDomain;
    IntRes2d_Domain             LastDomain ;

    double   UFirst, ULast, UB1, UB2;
    double   UBisInt1, UBisInt2, Utrim;
    double   Distance;
    double   Radius;

    double Epsilon1  = 1.E-6; // Epsilon sur le parametre de la courbe.
    double Tolerance = 1.E-8; // Tolerance pour les intersections.

     Type = Cu->DynamicType();

    if (Type == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
      TheCurve = occ::down_cast<Geom2d_TrimmedCurve>(Cu)->BasisCurve();
      Type     = TheCurve->DynamicType();
    }
    else {
      TheCurve = Cu;
    }

    if (Type == STANDARD_TYPE(Geom2d_Circle)) {
      CircleCu = occ::down_cast<Geom2d_Circle>(TheCurve);
    }
    else {
      LineCu = occ::down_cast<Geom2d_Line>(TheCurve);
    }

    // Recuperation de UFirst, ULast.
    // -------------------------------
    UFirst   = Cu->FirstParameter();
    ULast    = Cu->LastParameter();

    // Creation des lignes Limites du domaine si elles existent.
    // et Determination de leur domaine d intersection.
    // ---------------------------------------------------------
    if (Type == STANDARD_TYPE(Geom2d_Circle)) {
      CircleCu->D1(UFirst,PFirst,TanFirst);
      CircleCu->D1(ULast ,PLast ,TanLast);
      Radius = CircleCu->Radius();

      if (PFirst.Distance(PLast) > 2.*Epsilon1 && Radius > Epsilon1) {
        Center     = CircleCu->Location();
        P          = PFirst.Translated( - (Epsilon1/Radius)*TanFirst );

        FirstLimit = new Geom2d_Line(P,
                     gp_Dir2d(PFirst.X() - Center.X(),
                          PFirst.Y() - Center.Y()));
        P          = PLast .Translated( (Epsilon1/Radius)*TanLast );

        LastLimit  = new Geom2d_Line(P,
                     gp_Dir2d(PLast.X() - Center.X(),
                          PLast.Y() - Center.Y()));

        Geom2dAdaptor_Curve AFirstLimit(FirstLimit);
        Geom2dAdaptor_Curve ALastLimit (LastLimit);
        Geom2dInt_GInter Intersect(AFirstLimit , FirstDomain,
                   ALastLimit  , LastDomain ,
                   Tolerance   , Tolerance     );

        if (Intersect.IsDone() && !Intersect.IsEmpty()) {
      if (Intersect.NbPoints() >= 1) {
        FirstDomain.SetValues(Intersect.Point(1).Value(),
                  Intersect.Point(1).ParamOnFirst(),
                  Tolerance,true);
        LastDomain. SetValues(Intersect.Point(1).Value(),
                  Intersect.Point(1).ParamOnSecond(),
                  Tolerance,true);
      }
        }
      }
    }
    else if (Type == STANDARD_TYPE(Geom2d_Line)) {
      gpLine = LineCu->Lin2d();
      if (UFirst > - Precision::Infinite()){
        P          = LineCu->Value(UFirst - Epsilon1);
        FirstLimit = new Geom2d_Line(gpLine.Normal(P)) ;
      }
      if (ULast < Precision::Infinite()) {
        P         = LineCu->Value(ULast + Epsilon1);
        LastLimit = new Geom2d_Line(gpLine.Normal(P));
      }
    }
    else {
      throw Standard_NotImplemented();
    }

    // Determination domaine d intersection de la Bissectrice.
    // -------------------------------------------------------
    UB1 = thebisector->FirstParameter();
    UB2 = thebisector->LastParameter();
    if (UB2 > 10000.) {
      UB2 = 10000.;
      occ::handle<Geom2d_Curve>  BasisCurve = thebisector->BasisCurve();
      occ::handle<Standard_Type> Type1 = BasisCurve->DynamicType();
      gp_Parab2d gpParabola;
      gp_Hypr2d  gpHyperbola;
      double Focus;
      double Limit = 50000.;
      if (Type1 == STANDARD_TYPE(Geom2d_Parabola)) {
        gpParabola = occ::down_cast<Geom2d_Parabola>(BasisCurve)->Parab2d();
        Focus = gpParabola.Focal();
        double Val1 = std::sqrt(Limit*Focus);
        double Val2 = std::sqrt(Limit*Limit);
        UB2 = (Val1 <= Val2 ? Val1:Val2);
      }
      else if (Type1 == STANDARD_TYPE(Geom2d_Hyperbola)) {
        gpHyperbola = occ::down_cast<Geom2d_Hyperbola>(BasisCurve)->Hypr2d();
        double Majr  = gpHyperbola.MajorRadius();
        double Minr  = gpHyperbola.MinorRadius();
        double Valu1 = Limit/Majr;
        double Valu2 = Limit/Minr;
        double Val1  = Log(Valu1+std::sqrt(Valu1*Valu1-1));
        double Val2  = Log(Valu2+std::sqrt(Valu2*Valu2+1));
        UB2 = (Val1 <= Val2 ? Val1:Val2);
      }
    }

    IntRes2d_Domain DomainBisector(thebisector->Value(UB1), UB1, Tolerance,
                   thebisector->Value(UB2), UB2, Tolerance);

    if (thebisector->BasisCurve()->IsPeriodic()) {
      DomainBisector.SetEquivalentParameters(0.0,2.*M_PI);
    }
    FirstPointBisector = thebisector->Value(UB1);

    // Intersection Bisectrice avec FirstLimit => UBisInt1.
    // ----------------------------------------------------
    UBisInt1 = Precision::Infinite();
    if (!FirstLimit.IsNull()) {
      Geom2dAdaptor_Curve AdapBis    (thebisector);
      Geom2dAdaptor_Curve AFirstLimit(FirstLimit);
      Geom2dInt_GInter Intersect(AFirstLimit , FirstDomain,
                     AdapBis     , DomainBisector,
                     Tolerance   , Tolerance     );

      if (Intersect.IsDone() && !Intersect.IsEmpty()) {
        for (int i=1; i<=Intersect.NbPoints(); i++) {
      Distance = FirstPointBisector.Distance(Intersect.Point(i).Value());
      if (Distance > 2.*Tolerance) {
        UBisInt1 = Intersect.Point(i).ParamOnSecond();
        break;
      }
        }
      }
    }
    // Intersection Bisectrice avec LastLimit => UBisInt2.
    // ---------------------------------------------------
    UBisInt2 = Precision::Infinite();
    if (!LastLimit.IsNull()) {
      Geom2dAdaptor_Curve AdapBis    (thebisector);
      Geom2dAdaptor_Curve ALastLimit (LastLimit);
      Geom2dInt_GInter Intersect(ALastLimit , LastDomain    ,
                     AdapBis    , DomainBisector,
                     Tolerance  , Tolerance     );

      if (Intersect.IsDone() && !Intersect.IsEmpty()) {
        for (int i=1; i<=Intersect.NbPoints(); i++) {
      Distance = FirstPointBisector.Distance(Intersect.Point(i).Value());
      if (Distance > 2.*Tolerance) {
        UBisInt2 = Intersect.Point(i).ParamOnSecond();
        break;
      }
        }
      }
    }
    // Restriction de la Bissectrice par le point d intersection de plus petit
    // parametre.
    //------------------------------------------------------------------------
    Utrim = (UBisInt1 < UBisInt2) ? UBisInt1 : UBisInt2;

    if (Utrim < UB2 && Utrim > UB1) thebisector->SetTrim(UB1,Utrim);
  */
}

void Bisector_BisecAna::SetTrim(const double uf, const double ul)
{
  thebisector->SetTrim(uf, ul);
}

//=================================================================================================

void Bisector_BisecAna::Reverse()
{
  thebisector->Reverse();
}

//=================================================================================================

double Bisector_BisecAna::ReversedParameter(const double U) const
{
  return thebisector->ReversedParameter(U);
}

//=================================================================================================

bool Bisector_BisecAna::IsCN(const int N) const
{
  return thebisector->IsCN(N);
}

//=================================================================================================

occ::handle<Geom2d_Geometry> Bisector_BisecAna::Copy() const
{
  occ::handle<Bisector_BisecAna> C = new Bisector_BisecAna();
  C->Init(occ::down_cast<Geom2d_TrimmedCurve>(thebisector->Copy()));
  return C;
}

//=================================================================================================

void Bisector_BisecAna::Transform(const gp_Trsf2d& T)
{
  thebisector->Transform(T);
}

//=================================================================================================

double Bisector_BisecAna::FirstParameter() const
{
  //  modified by NIZHNY-EAP Thu Feb  3 17:23:42 2000 ___BEGIN___
  //  return thebisector->BasisCurve()->FirstParameter();
  return thebisector->FirstParameter();
  //  modified by NIZHNY-EAP Thu Feb  3 17:23:48 2000 ___END___
}

//=================================================================================================

double Bisector_BisecAna::LastParameter() const
{
  return thebisector->LastParameter();
}

//=================================================================================================

bool Bisector_BisecAna::IsClosed() const
{
  return thebisector->BasisCurve()->IsClosed();
}

//=================================================================================================

bool Bisector_BisecAna::IsPeriodic() const
{
  return thebisector->BasisCurve()->IsPeriodic();
}

//=================================================================================================

GeomAbs_Shape Bisector_BisecAna::Continuity() const
{
  return thebisector->Continuity();
}

//=================================================================================================

std::optional<gp_Pnt2d> Bisector_BisecAna::EvalD0(const double U) const
{
  return thebisector->BasisCurve()->EvalD0(U);
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD1> Bisector_BisecAna::EvalD1(const double U) const
{
  return thebisector->BasisCurve()->EvalD1(U);
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD2> Bisector_BisecAna::EvalD2(const double U) const
{
  return thebisector->BasisCurve()->EvalD2(U);
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD3> Bisector_BisecAna::EvalD3(const double U) const
{
  return thebisector->BasisCurve()->EvalD3(U);
}

//=================================================================================================

std::optional<gp_Vec2d> Bisector_BisecAna::EvalDN(const double U, const int N) const
{
  return thebisector->BasisCurve()->EvalDN(U, N);
}

//=================================================================================================

occ::handle<Geom2d_Curve> Bisector_BisecAna::Geom2dCurve() const
{
  return thebisector->BasisCurve();
}

//=================================================================================================

double Bisector_BisecAna::ParameterOfStartPoint() const
{
  return thebisector->FirstParameter();
}

//=================================================================================================

double Bisector_BisecAna::ParameterOfEndPoint() const
{
  return thebisector->LastParameter();
}

//=================================================================================================

double Bisector_BisecAna::Parameter(const gp_Pnt2d& P) const
{
  gp_Hypr2d  gphyperbola;
  gp_Parab2d gpparabola;
  gp_Elips2d gpellipse;
  gp_Circ2d  gpcircle;
  gp_Lin2d   gpline;

  occ::handle<Geom2d_Curve>  BasisCurve = thebisector->BasisCurve();
  occ::handle<Standard_Type> Type       = BasisCurve->DynamicType();

  if (Type == STANDARD_TYPE(Geom2d_Line))
  {
    gpline = occ::down_cast<Geom2d_Line>(BasisCurve)->Lin2d();
    return ElCLib::Parameter(gpline, P);
  }
  else if (Type == STANDARD_TYPE(Geom2d_Circle))
  {
    gpcircle = occ::down_cast<Geom2d_Circle>(BasisCurve)->Circ2d();
    return ElCLib::Parameter(gpcircle, P);
  }
  else if (Type == STANDARD_TYPE(Geom2d_Hyperbola))
  {
    gphyperbola = occ::down_cast<Geom2d_Hyperbola>(BasisCurve)->Hypr2d();
    return ElCLib::Parameter(gphyperbola, P);
  }
  else if (Type == STANDARD_TYPE(Geom2d_Parabola))
  {
    gpparabola = occ::down_cast<Geom2d_Parabola>(BasisCurve)->Parab2d();
    return ElCLib::Parameter(gpparabola, P);
  }
  else if (Type == STANDARD_TYPE(Geom2d_Ellipse))
  {
    gpellipse = occ::down_cast<Geom2d_Ellipse>(BasisCurve)->Elips2d();
    return ElCLib::Parameter(gpellipse, P);
  }
  return 0.;
}

//=================================================================================================

int Bisector_BisecAna::NbIntervals() const
{
  return 1;
}

//=================================================================================================

double Bisector_BisecAna::IntervalFirst(const int I) const
{
  if (I != 1)
    throw Standard_OutOfRange();
  return FirstParameter();
}

//=================================================================================================

double Bisector_BisecAna::IntervalLast(const int I) const
{
  if (I != 1)
    throw Standard_OutOfRange();
  return LastParameter();
}

//=============================================================================
// function :
//=============================================================================
void Bisector_BisecAna::Init(const occ::handle<Geom2d_TrimmedCurve>& Bis)
{
  thebisector = Bis;
}

//=============================================================================
// function : Degenerate
// purpose  : Replace the bisectrice by a straight line,
//           if the bisectrice is an ellipse, a parabole or a degenerated ellipse.
//=============================================================================
bool Degenerate(occ::handle<GccInt_Bisec>& aBisector, const double Tolerance)
{
  bool Degeneree = false;

  gp_Hypr2d  gphyperbola;
  gp_Parab2d gpparabola;
  gp_Elips2d gpellipse;
  // gp_Circ2d  gpcircle   ;

  occ::handle<GccInt_Bisec> NewBisector;

  GccInt_IType type = aBisector->ArcType();

  if (type == GccInt_Hpr)
  {
    gphyperbola = aBisector->Hyperbola();

    // If the Hyperbola is degenerated, it is replaced by the straight line
    // with direction to the axis if symmetry.

    if (gphyperbola.MajorRadius() < Tolerance)
    {
      gp_Lin2d gpline(gphyperbola.YAxis());
      NewBisector = new GccInt_BLine(gpline);
      aBisector   = NewBisector;
      Degeneree   = true;
    }
    if (gphyperbola.MinorRadius() < Tolerance)
    {
      gp_Lin2d gpline(gphyperbola.XAxis());
      NewBisector = new GccInt_BLine(gpline);
      aBisector   = NewBisector;
      Degeneree   = true;
    }
  }
  else if (type == GccInt_Par)
  {
    gpparabola = aBisector->Parabola();

    // If the parabole is degenerated, it is replaces by the straight
    // line starting at the Top and with direction of the axis of symmetry.

    if (gpparabola.Focal() < Tolerance)
    {
      gp_Lin2d gpline(gpparabola.MirrorAxis());
      NewBisector = new GccInt_BLine(gpline);
      aBisector   = NewBisector;
      Degeneree   = true;
    }
  }
  else if (type == GccInt_Ell)
  {
    gpellipse = aBisector->Ellipse();

    // If the ellipse is degenerated, it is replaced by the straight line
    // defined by the great axis.

    if (gpellipse.MinorRadius() < Tolerance)
    {
      gp_Lin2d gpline(gpellipse.XAxis());
      NewBisector = new GccInt_BLine(gpline);
      aBisector   = NewBisector;
      Degeneree   = true;
    }
  }
  return Degeneree;
}

static void Indent(const int Offset)
{
  if (Offset > 0)
  {
    for (int i = 0; i < Offset; i++)
    {
      std::cout << " ";
    }
  }
}

//=================================================================================================

// void Bisector_BisecAna::Dump(const int Deep,
void Bisector_BisecAna::Dump(const int, const int Offset) const
{
  Indent(Offset);
  std::cout << "Bisector_BisecAna" << std::endl;
  Indent(Offset);
  //  thebisector->Dump();
}
