// Created on: 1996-02-15
// Created by: Jacques GOUSSARD
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

#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <LocOpe_FindEdges.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================

void LocOpe_FindEdges::Set(const TopoDS_Shape& FFrom, const TopoDS_Shape& FTo)
{
  myFFrom = FFrom;
  myFTo   = FTo;
  myLFrom.Clear();
  myLTo.Clear();

  TopExp_Explorer            expf, expt;
  occ::handle<Geom_Curve>    Cf, Ct;
  TopLoc_Location            Loc;
  double                     ff, lf, ft, lt;
  occ::handle<Standard_Type> Tf, Tt;

  for (expf.Init(myFFrom, TopAbs_EDGE); expf.More(); expf.Next())
  {
    const TopoDS_Edge& edgf = TopoDS::Edge(expf.Current());
    Cf                      = BRep_Tool::Curve(edgf, Loc, ff, lf);
    if (!Loc.IsIdentity())
    {
      occ::handle<Geom_Geometry> GGf = Cf->Transformed(Loc.Transformation());
      Cf                             = occ::down_cast<Geom_Curve>(GGf);
    }
    Tf = Cf->DynamicType();
    if (Tf == STANDARD_TYPE(Geom_TrimmedCurve))
    {
      Cf = occ::down_cast<Geom_TrimmedCurve>(Cf)->BasisCurve();
      Tf = Cf->DynamicType();
    }
    if (Tf != STANDARD_TYPE(Geom_Line) && Tf != STANDARD_TYPE(Geom_Circle)
        && Tf != STANDARD_TYPE(Geom_Ellipse) && Tf != STANDARD_TYPE(Geom_BSplineCurve)
        && Tf != STANDARD_TYPE(Geom_BezierCurve))
    {
      continue;
    }
    for (expt.Init(myFTo, TopAbs_EDGE); expt.More(); expt.Next())
    {
      const TopoDS_Edge& edgt = TopoDS::Edge(expt.Current());
      Ct                      = BRep_Tool::Curve(edgt, Loc, ft, lt);
      if (!Loc.IsIdentity())
      {
        occ::handle<Geom_Geometry> GGt = Ct->Transformed(Loc.Transformation());
        Ct                             = occ::down_cast<Geom_Curve>(GGt);
      }
      Tt = Ct->DynamicType();
      if (Tt == STANDARD_TYPE(Geom_TrimmedCurve))
      {
        Ct = occ::down_cast<Geom_TrimmedCurve>(Ct)->BasisCurve();
        Tt = Ct->DynamicType();
      }
      if (Tt != Tf)
      {
        continue;
      }
      // On a presomption de confusion
      double Tol = Precision::Confusion();
      if (Tt == STANDARD_TYPE(Geom_Line))
      {
        gp_Lin lif  = occ::down_cast<Geom_Line>(Cf)->Lin();
        gp_Lin lit  = occ::down_cast<Geom_Line>(Ct)->Lin();
        gp_Pnt p1   = ElCLib::Value(ff, lif);
        gp_Pnt p2   = ElCLib::Value(lf, lif);
        double prm1 = ElCLib::Parameter(lit, p1);
        double prm2 = ElCLib::Parameter(lit, p2);
        if (prm1 >= ft - Tol && prm1 <= lt + Tol && prm2 >= ft - Tol && prm2 <= lt + Tol)
        {
          Tol *= Tol;
          gp_Pnt pt = ElCLib::Value(prm1, lit);
          if (pt.SquareDistance(p1) <= Tol)
          {
            pt = ElCLib::Value(prm2, lit);
            if (pt.SquareDistance(p2) <= Tol)
            {
              myLFrom.Append(edgf);
              myLTo.Append(edgt);
              break;
            }
          }
        }
      }
      else if (Tt == STANDARD_TYPE(Geom_Circle))
      {
        gp_Circ cif = occ::down_cast<Geom_Circle>(Cf)->Circ();
        gp_Circ cit = occ::down_cast<Geom_Circle>(Ct)->Circ();
        if (std::abs(cif.Radius() - cit.Radius()) <= Tol
            && cif.Location().SquareDistance(cit.Location()) <= Tol * Tol)
        {
          // Point debut, calage dans periode, et detection meme sens

          gp_Pnt p1, p2;
          gp_Vec tgf, tgt;
          ElCLib::D1(ff, cif, p1, tgf);
          p2 = ElCLib::Value(lf, cif);

          double prm1  = ElCLib::Parameter(cit, p1);
          double Tol2d = Precision::PConfusion();
          if (std::abs(prm1 - ft) <= Tol2d)
            prm1 = ft;
          prm1 = ElCLib::InPeriod(prm1, ft, ft + 2. * M_PI);
          ElCLib::D1(prm1, cit, p1, tgt);

          double prm2 = ElCLib::Parameter(cit, p2);
          if (tgt.Dot(tgf) > 0.)
          { // meme sens
            while (prm2 <= prm1)
            {
              prm2 += 2. * M_PI;
            }
          }
          else
          {
            if (std::abs(prm1 - ft) <= Precision::Angular())
            {
              prm1 += 2. * M_PI;
            }
            while (prm2 >= prm1)
            {
              prm2 -= 2. * M_PI;
            }
          }

          if (prm1 >= ft - Tol && prm1 <= lt + Tol && prm2 >= ft - Tol && prm2 <= lt + Tol)
          {
            myLFrom.Append(edgf);
            myLTo.Append(edgt);
            break;
          }
          else
          {
            // Cas non traite : on est a cheval
#ifdef OCCT_DEBUG
            std::cout << " cas a cheval." << std::endl;
#endif

            //	    myLFrom.Append(edgf);
            //	    myLTo.Append(edgt);
            //	    break;
          }
        }
      }
      else if (Tt == STANDARD_TYPE(Geom_Ellipse))
      {
        gp_Elips cif = occ::down_cast<Geom_Ellipse>(Cf)->Elips();
        gp_Elips cit = occ::down_cast<Geom_Ellipse>(Ct)->Elips();

        if (std::abs(cif.MajorRadius() - cit.MajorRadius()) <= Tol
            && std::abs(cif.MinorRadius() - cit.MinorRadius()) <= Tol
            && cif.Location().SquareDistance(cit.Location()) <= Tol * Tol)
        {
          // Point debut, calage dans periode, et detection meme sens

          gp_Pnt p1, p2;
          gp_Vec tgf, tgt;
          ElCLib::D1(ff, cif, p1, tgf);
          p2 = ElCLib::Value(lf, cif);

          double prm1 = ElCLib::Parameter(cit, p1);
          prm1        = ElCLib::InPeriod(prm1, ft, ft + 2. * M_PI);
          ElCLib::D1(prm1, cit, p1, tgt);

          double prm2 = ElCLib::Parameter(cit, p2);
          if (tgt.Dot(tgf) > 0.)
          { // meme sens
            while (prm2 <= prm1)
            {
              prm2 += 2. * M_PI;
            }
          }
          else
          {
            if (std::abs(prm1 - ft) <= Precision::Angular())
            {
              prm1 += 2. * M_PI;
            }
            while (prm2 >= prm1)
            {
              prm2 -= 2. * M_PI;
            }
          }

          if (prm1 >= ft - Tol && prm1 <= lt + Tol && prm2 >= ft - Tol && prm2 <= lt + Tol)
          {
            myLFrom.Append(edgf);
            myLTo.Append(edgt);
            break;
          }
          else
          {
            // Cas non traite : on est a cheval
#ifdef OCCT_DEBUG
            std::cout << " cas a cheval." << std::endl;
#endif
            //	    myLFrom.Append(edgf);
            //	    myLTo.Append(edgt);
          }
        }
      }
      else if (Tt == STANDARD_TYPE(Geom_BSplineCurve))
      {
        occ::handle<Geom_BSplineCurve> Bf = occ::down_cast<Geom_BSplineCurve>(Cf);
        occ::handle<Geom_BSplineCurve> Bt = occ::down_cast<Geom_BSplineCurve>(Ct);

        bool IsSame = true;

        int nbpoles = Bf->NbPoles();
        if (nbpoles != Bt->NbPoles())
        {
          IsSame = false;
        }

        if (IsSame)
        {
          int nbknots = Bf->NbKnots();
          if (nbknots != Bt->NbKnots())
          {
            IsSame = false;
          }

          if (IsSame)
          {
            const NCollection_Array1<gp_Pnt>& Pf = Bf->Poles();
            const NCollection_Array1<gp_Pnt>& Pt = Bt->Poles();

            double tol3d = BRep_Tool::Tolerance(edgt);
            for (int p = 1; p <= nbpoles; p++)
            {
              if ((Pf(p)).Distance(Pt(p)) > tol3d)
              {
                IsSame = false;
                break;
              }
            }

            if (IsSame)
            {
              const NCollection_Array1<double>& Kf = Bf->Knots();
              const NCollection_Array1<double>& Kt = Bt->Knots();

              const NCollection_Array1<int>& Mf = Bf->Multiplicities();
              const NCollection_Array1<int>& Mt = Bt->Multiplicities();

              for (int k = 1; k <= nbknots; k++)
              {
                if ((Kf(k) - Kt(k)) > Tol)
                {
                  IsSame = false;
                  break;
                }
                if (std::abs(Mf(k) - Mt(k)) > Tol)
                {
                  IsSame = false;
                  break;
                }
              }

              if (!Bf->IsRational())
              {
                if (Bt->IsRational())
                {
                  IsSame = false;
                }
              }
              else
              {
                if (!Bt->IsRational())
                {
                  IsSame = false;
                }
              }

              if (IsSame && Bf->IsRational())
              {
                const NCollection_Array1<double>& Wf = *Bf->Weights();
                const NCollection_Array1<double>& Wt = *Bt->Weights();

                for (int w = 1; w <= nbpoles; w++)
                {
                  if (std::abs(Wf(w) - Wt(w)) > Tol)
                  {
                    IsSame = false;
                    break;
                  }
                }
              }

              if (IsSame)
              {
#ifdef OCCT_DEBUG
                std::cout << "memes bsplines." << std::endl;
#endif
                myLFrom.Append(edgf);
                myLTo.Append(edgt);
                break;
              }
            }
          }
        }
      }
      else if (Tt == STANDARD_TYPE(Geom_BezierCurve))
      {
        occ::handle<Geom_BezierCurve> Bf = occ::down_cast<Geom_BezierCurve>(Cf);
        occ::handle<Geom_BezierCurve> Bt = occ::down_cast<Geom_BezierCurve>(Ct);

        bool IsSame = true;

        int nbpoles = Bf->NbPoles();
        if (nbpoles != Bt->NbPoles())
        {
          IsSame = false;
        }

        if (IsSame)
        {
          const NCollection_Array1<gp_Pnt>& Pf = Bf->Poles();
          const NCollection_Array1<gp_Pnt>& Pt = Bt->Poles();

          for (int p = 1; p <= nbpoles; p++)
          {
            if ((Pf(p)).Distance(Pt(p)) > Tol)
            {
              IsSame = false;
              break;
            }
          }

          if (IsSame)
          {
            if (!Bf->IsRational())
            {
              if (Bt->IsRational())
              {
                IsSame = false;
              }
            }
            else
            {
              if (!Bt->IsRational())
              {
                IsSame = false;
              }
            }

            if (IsSame && Bf->IsRational())
            {
              const NCollection_Array1<double>& Wf = *Bf->Weights();
              const NCollection_Array1<double>& Wt = *Bt->Weights();

              for (int w = 1; w <= nbpoles; w++)
              {
                if (std::abs(Wf(w) - Wt(w)) > Tol)
                {
                  IsSame = false;
                  break;
                }
              }
            }

            if (IsSame)
            {
#ifdef OCCT_DEBUG
              std::cout << "memes beziers." << std::endl;
#endif
              myLFrom.Append(edgf);
              myLTo.Append(edgt);
              break;
            }
          }
        }
      }
    }
  }
}
