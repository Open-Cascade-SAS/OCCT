// Created on: 1991-12-13
// Created by: Remi GILET
// Copyright (c) 1991-1999 Matra Datavision
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

//=========================================================================
//   Creation d un cercle tangent a deux elements : Droite.               +
//                                                  Cercle.               +
//                                                  Point.                +
//                                                  Courbes.              +
//                        centre sur un troisieme : Droite.               +
//                                                  Cercle.               +
//                                                  Courbes.              +
//=========================================================================

#include <ElCLib.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_Circ2d2TanOnIter.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_FunctionTanCuCuOnCu.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <math_FunctionSetRoot.hxx>
#include <StdFail_NotDone.hxx>

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const GccEnt_QualifiedLin&  Qualified1 , 
                            const Geom2dGcc_QCurve&     Qualified2 , 
                            const gp_Lin2d&             OnLine     ,
                            const Standard_Real         Param1     ,
                            const Standard_Real         Param2     ,
                            const Standard_Real         Param3     ,
                            const Standard_Real         Tolang     ) {

                              TheSame1 = Standard_False;
                              TheSame2 = Standard_False;
                              par1sol = 0.;
                              par2sol = 0.;
                              pararg1 = 0.;
                              pararg2 = 0.;
                              parcen3 = 0.;

                              WellDone = Standard_False;
                              Standard_Real Tol = Abs(Tolang);
                              WellDone = Standard_False;
                              if (!(Qualified1.IsEnclosed() ||
                                Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                                !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                                Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                                  throw GccEnt_BadQualifier();
                                  return;
                              }
                              gp_Dir2d dirx(1.,0.);
                              gp_Lin2d L1 = Qualified1.Qualified();
                              Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                              math_Vector Umin(1,4);
                              math_Vector Umax(1,4);
                              math_Vector Ufirst(1,4);
                              math_Vector tol(1,4);
                              Umin(1) = RealFirst();
                              Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                              Umin(3) = RealFirst();
                              Umin(4) = 0.;
                              Umax(1) = RealLast();
                              Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                              Umax(3) = RealLast();
                              Umax(4) = RealLast();
                              Ufirst(1) = Param1;
                              Ufirst(2) = Param2;
                              Ufirst(3) = Param3;
                              tol(1) = 1.e-15;
                              tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Tolang);
                              tol(3) = tol(1);   
                              tol(4) = tol(1);   
                              gp_Pnt2d point1 = ElCLib::Value(Param1,L1);
                              gp_Pnt2d point2 = Geom2dGcc_CurveTool::Value(Cu2,Param2);
                              gp_Pnt2d point3 = ElCLib::Value(Param3,OnLine);
                              Ufirst(4) = (point3.Distance(point2)+point3.Distance(point1))/2.;
                              Geom2dGcc_FunctionTanCuCuOnCu Func(L1,Cu2,OnLine,Max(Ufirst(4), Tol));
                              math_FunctionSetRoot Root(Func, tol);
                              Root.Perform(Func, Ufirst, Umin, Umax);
                              Func.Value(Ufirst,Umin);
                              if (Root.IsDone()) {
                                Root.Root(Ufirst);
                                //     gp_Vec2d Tan1,Tan2,Nor1,Nor2;
                                gp_Vec2d Tan1,Tan2;
                                ElCLib::D1(Ufirst(1),L1,point1,Tan1);
                                Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
                                gp_Vec2d Tan3(OnLine.Direction().XY());
                                gp_Pnt2d point3new(OnLine.Location().XY()+Ufirst(3)*Tan3.XY());
                                Standard_Real dist1 = point3new.Distance(point1);
                                Standard_Real dist2 = point3new.Distance(point2);
                                if ( Abs(dist1-dist2)/2. <= Tol) {
                                  cirsol = gp_Circ2d(gp_Ax2d(point3new,dirx),(dist1+dist2)/2.);
                                  Standard_Real normetan2 = Tan2.Magnitude();
                                  gp_Vec2d Vec1(point1,point3new);
                                  gp_Vec2d Vec2(point2,point3new);
                                  Standard_Real normevec2 = Vec2.Magnitude();
                                  Standard_Real angle2;
                                  if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                    angle2 = Vec2.Angle(Tan2);
                                  }
                                  else { angle2 = 0.; }
                                  Standard_Real pscal=point3new.XY().Dot(gp_XY(-L1.Direction().Y(),
                                    L1.Direction().X()));
                                  if (Qualified1.IsUnqualified() ||
                                    (Qualified1.IsOutside() && pscal <= 0.) ||
                                    (Qualified1.IsEnclosed() && pscal >= 0.)) {
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&angle2<=0.)||
                                        (Qualified2.IsOutside() && angle2 >= 0) ||
                                        (Qualified2.IsEnclosed() && angle2 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          pnttg1sol = point1;
                                          pararg1 = Ufirst(1);
                                          par1sol = ElCLib::Parameter(cirsol,pnttg1sol);
                                          pnttg2sol = point2;
                                          pararg2 = Ufirst(2);
                                          par2sol = ElCLib::Parameter(cirsol,pnttg2sol);
                                          pntcen  = point3new;
                                          parcen3 = Ufirst(3);
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
}

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const Geom2dGcc_QCurve& Qualified1 , 
                            const Geom2dGcc_QCurve& Qualified2 , 
                            const gp_Lin2d&          OnLine     ,
                            const Standard_Real               Param1     ,
                            const Standard_Real               Param2     ,
                            const Standard_Real               Param3     ,
                            const Standard_Real               Tolerance  ) {
                              TheSame1 = Standard_False;
                              TheSame2 = Standard_False;
                              par1sol = 0.;
                              par2sol = 0.;
                              pararg1 = 0.;
                              pararg2 = 0.;
                              parcen3 = 0.;

                              WellDone = Standard_False;
                              if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                                Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                                !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                                Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                                  throw GccEnt_BadQualifier();
                                  return;
                              }
                              Standard_Real Tol = Abs(Tolerance);
                              gp_Dir2d dirx(1.,0.);
                              Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
                              Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                              math_Vector Umin(1,4);
                              math_Vector Umax(1,4);
                              math_Vector Ufirst(1,4);
                              math_Vector tol(1,4);
                              Umin(1) = Geom2dGcc_CurveTool::FirstParameter(Cu1);
                              Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                              Umin(3) = RealFirst();  
                              Umin(4) = 0.;
                              Umax(1) = Geom2dGcc_CurveTool::LastParameter(Cu1);
                              Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                              Umax(3) = RealLast();
                              Umax(4) = RealLast();
                              Ufirst(1) = Param1;
                              Ufirst(2) = Param2;
                              Ufirst(3) = Param3;
                              tol(1) = Geom2dGcc_CurveTool::EpsX(Cu1,Abs(Tolerance));
                              tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                              tol(3) = 1.e-15;   
                              tol(4) = Tol/10.;   
                              gp_Pnt2d point1 = Geom2dGcc_CurveTool::Value(Cu1,Param1);
                              gp_Pnt2d point2 = Geom2dGcc_CurveTool::Value(Cu2,Param2);
                              gp_Pnt2d point3 = ElCLib::Value(Param3,OnLine);
                              Ufirst(4) = (point3.Distance(point2)+point3.Distance(point1))/2.;
                              Geom2dGcc_FunctionTanCuCuOnCu Func(Cu1,Cu2,OnLine,Max(Ufirst(4), Tol));
                              math_FunctionSetRoot Root(Func, tol);
                              Root.Perform(Func, Ufirst, Umin, Umax);
                              Func.Value(Ufirst,Umin);
                              if (Root.IsDone()) {
                                Root.Root(Ufirst);
                                gp_Vec2d Tan1,Tan2;
                                Geom2dGcc_CurveTool::D1(Cu1,Ufirst(1),point1,Tan1);
                                Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
                                gp_Vec2d Tan3(OnLine.Direction().XY());
                                gp_Pnt2d point3new(OnLine.Location().XY()+Ufirst(3)*Tan3.XY());
                                Standard_Real dist1 = point3new.Distance(point1);
                                Standard_Real dist2 = point3new.Distance(point2);
                                if((dist1+dist2)/2. < Tol)
                                {
                                  return;
                                }
                                if ( Abs(dist1-dist2)/2. <= Tol) {
                                  cirsol = gp_Circ2d(gp_Ax2d(point3new,dirx),(dist1+dist2)/2.);
                                  Standard_Real normetan1 = Tan1.Magnitude();
                                  Standard_Real normetan2 = Tan2.Magnitude();
                                  gp_Vec2d Vec1(point1,point3new);
                                  gp_Vec2d Vec2(point2,point3new);
                                  Standard_Real normevec1 = Vec1.Magnitude();
                                  Standard_Real normevec2 = Vec2.Magnitude();
                                  Standard_Real angle1,angle2;
                                  if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                    angle1 = Vec1.Angle(Tan1);
                                  }
                                  else { angle1 = 0.; }
                                  if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                    angle2 = Vec2.Angle(Tan2);
                                  }
                                  else { angle2 = 0.; }
                                  if (Qualified1.IsUnqualified()||
                                    (Qualified1.IsEnclosing()&&angle1<=0.)||
                                    (Qualified1.IsOutside() && angle1 >= 0.) ||
                                    (Qualified1.IsEnclosed() && angle1 <= 0.)) {
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&angle2<=0.)||
                                        (Qualified2.IsOutside() && angle2 >= 0) ||
                                        (Qualified2.IsEnclosed() && angle2 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          pnttg1sol = point1;
                                          pararg1 = Ufirst(1);
                                          par1sol = ElCLib::Parameter(cirsol,pnttg1sol);
                                          pnttg2sol = point2;
                                          pararg2 = Ufirst(2);
                                          par2sol = ElCLib::Parameter(cirsol,pnttg2sol);
                                          pntcen  = point3new;
                                          parcen3 = Ufirst(3);
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
}

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const Geom2dGcc_QCurve& Qualified1 , 
                            const gp_Pnt2d&          Point2     , 
                            const gp_Lin2d&          OnLine     ,
                            const Standard_Real      Param1     ,
                            const Standard_Real      Param2     ,
                            const Standard_Real      Tolerance  ) {
                              TheSame1 = Standard_False;
                              TheSame2 = Standard_False;
                              par1sol = 0.;
                              par2sol = 0.;
                              pararg1 = 0.;
                              pararg2 = 0.;
                              parcen3 = 0.;

                              WellDone = Standard_False;
                              if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                                Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
                                  throw GccEnt_BadQualifier();
                                  return;
                              }
                              Standard_Real Tol = Abs(Tolerance);
                              gp_Dir2d dirx(1.,0.);
                              Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
                              math_Vector Umin(1,3);
                              math_Vector Umax(1,3);
                              math_Vector Ufirst(1,3);
                              math_Vector tol(1,3);
                              Umin(1) = Geom2dGcc_CurveTool::FirstParameter(Cu1);
                              Umin(2) = RealFirst();
                              Umin(3) = 0.;
                              Umax(1) = Geom2dGcc_CurveTool::LastParameter(Cu1);
                              Umax(2) = RealLast();
                              Umax(3) = RealLast();
                              Ufirst(1) = Param1;
                              Ufirst(2) = Param2;
                              tol(1) = Geom2dGcc_CurveTool::EpsX(Cu1,Abs(Tolerance));
                              tol(2) = 1.e-15;
                              tol(3) = Tol/10.;
                              gp_Pnt2d point1 = Geom2dGcc_CurveTool::Value(Cu1,Param1);
                              gp_Pnt2d point3 = ElCLib::Value(Param2,OnLine);
                              Ufirst(3) = (point3.Distance(Point2)+point3.Distance(point1))/2.;
                              Geom2dGcc_FunctionTanCuCuOnCu Func(Cu1,Point2,OnLine,Max(Ufirst(3), Tol));
                              math_FunctionSetRoot Root(Func, tol);
                              Root.Perform(Func, Ufirst, Umin, Umax);
                              Func.Value(Ufirst,Umin);
                              if (Root.IsDone()) {
                                Root.Root(Ufirst);
                                gp_Pnt2d point1new,point3new;
                                gp_Vec2d Tan1,Tan3;
                                Geom2dGcc_CurveTool::D1(Cu1,Ufirst(1),point1new,Tan1);
                                ElCLib::D1(Ufirst(2),OnLine,point3new,Tan3);
                                Standard_Real dist1 = point3new.Distance(point1new);
                                Standard_Real dist2 = point3new.Distance(Point2);
                                if ( Abs(dist1-dist2)/2. <= Tol) {
                                  cirsol = gp_Circ2d(gp_Ax2d(point3new,dirx),(dist1+dist2)/2.);
                                  Standard_Real normetan1 = Tan1.Magnitude();
                                  gp_Vec2d Vec1(point1new,point3new);
                                  Standard_Real normevec1 = Vec1.Magnitude();
                                  Standard_Real angle1;
                                  if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                    angle1 = Vec1.Angle(Tan1);
                                  }
                                  else { angle1 = 0.; }
                                  if (Qualified1.IsUnqualified()||
                                    (Qualified1.IsEnclosing()&&angle1<=0.)||
                                    (Qualified1.IsOutside() && angle1 >= 0.) ||
                                    (Qualified1.IsEnclosed() && angle1 <= 0.)) {
                                      qualifier1 = Qualified1.Qualifier();
                                      qualifier2 = GccEnt_noqualifier;
                                      pnttg1sol = point1new;
                                      pararg1 = Ufirst(1);
                                      par1sol = ElCLib::Parameter(cirsol,pnttg1sol);
                                      pnttg2sol = Point2;
                                      pararg2 = Ufirst(2);
                                      par2sol = ElCLib::Parameter(cirsol,pnttg2sol);
                                      pntcen  = point3new;
                                      parcen3 = Ufirst(3);
                                      WellDone = Standard_True;
                                  }
                                }
                              }
}

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const GccEnt_QualifiedCirc& Qualified1 , 
                            const Geom2dGcc_QCurve&     Qualified2 , 
                            const gp_Lin2d&             OnLine     ,
                            const Standard_Real         Param1     ,
                            const Standard_Real         Param2     ,
                            const Standard_Real         Param3     ,
                            const Standard_Real         Tolerance  ) {
                              TheSame1 = Standard_False;
                              TheSame2 = Standard_False;
                              par1sol = 0.;
                              par2sol = 0.;
                              pararg1 = 0.;
                              pararg2 = 0.;
                              parcen3 = 0.;

                              WellDone = Standard_False;
                              if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                                Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                                !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                                Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                                  throw GccEnt_BadQualifier();
                                  return;
                              }
                              Standard_Real Tol = Abs(Tolerance);
                              gp_Dir2d dirx(1.,0.);
                              gp_Circ2d C1 = Qualified1.Qualified();
                              Standard_Real R1 = C1.Radius();
                              Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                              math_Vector Umin(1,4);
                              math_Vector Umax(1,4);
                              math_Vector Ufirst(1,4);
                              math_Vector tol(1,4);
                              Umin(1) = RealFirst();
                              Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                              Umin(3) = RealFirst();
                              Umin(4) = 0.;
                              Umax(1) = RealLast();
                              Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                              Umax(3) = RealLast();
                              Umax(4) = RealLast();
                              Ufirst(1) = Param1;
                              Ufirst(2) = Param2;
                              Ufirst(3) = Param3;
                              tol(1) = 2.e-15*M_PI;
                              tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                              tol(3) = 1.e-15;
                              tol(4) = Tol/10.;
                              gp_Pnt2d point1 = ElCLib::Value(Param1,C1);
                              gp_Pnt2d point2 = Geom2dGcc_CurveTool::Value(Cu2,Param2);
                              gp_Pnt2d point3 = ElCLib::Value(Param3,OnLine);
                              Ufirst(4) = (point3.Distance(point2)+point3.Distance(point1))/2.;
                              Geom2dGcc_FunctionTanCuCuOnCu Func(C1,Cu2,OnLine,Max(Ufirst(4), Tol));
                              math_FunctionSetRoot Root(Func, tol);
                              Root.Perform(Func, Ufirst, Umin, Umax);
                              Func.Value(Ufirst,Umin);
                              if (Root.IsDone()) {
                                Root.Root(Ufirst);
                                //     gp_Vec2d Tan1,Tan2,Nor1,Nor2;
                                gp_Vec2d Tan1,Tan2,Nor2;
                                ElCLib::D2(Ufirst(1),C1,point1,Tan1,Nor2);
                                Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
#ifdef OCCT_DEBUG
                                gp_Vec2d Tan3(OnLine.Direction().XY());
#else
                                OnLine.Direction().XY();
#endif
                                point3 = ElCLib::Value(Ufirst(1),OnLine);
                                Standard_Real dist1 = point3.Distance(point1);
                                Standard_Real dist2 = point3.Distance(point2);
                                if ( Abs(dist1-dist2)/2. <= Tol) {
                                  cirsol = gp_Circ2d(gp_Ax2d(point3,dirx),(dist1+dist2)/2.);
                                  Standard_Real normetan2 = Tan2.Magnitude();
                                  gp_Vec2d Vec1(point1,point3);
                                  gp_Vec2d Vec2(point2,point3);
                                  Standard_Real normevec2 = Vec2.Magnitude();
                                  Standard_Real angle2;
                                  if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                    angle2 = Vec2.Angle(Tan2);
                                  }
                                  else { angle2 = 0.; }
                                  Standard_Real dist = C1.Location().Distance(point3);
                                  Standard_Real Rsol = cirsol.Radius();
                                  if (Qualified1.IsUnqualified() || 
                                    (Qualified1.IsEnclosing() && Rsol >= R1 && dist <= Rsol)||
                                    (Qualified1.IsOutside() && dist >= Rsol) ||
                                    (Qualified1.IsEnclosed() && Rsol <= R1 && dist <= Rsol)) {
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&angle2<=0.)||
                                        (Qualified2.IsOutside() && angle2 >= 0) ||
                                        (Qualified2.IsEnclosed() && angle2 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          pnttg1sol = point1;
                                          pararg1 = Ufirst(1);
                                          par1sol = ElCLib::Parameter(cirsol,pnttg1sol);
                                          pnttg2sol = point2;
                                          pararg2 = Ufirst(2);
                                          par2sol = ElCLib::Parameter(cirsol,pnttg2sol);
                                          pntcen  = point3;
                                          parcen3 = Ufirst(3);
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
}

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const GccEnt_QualifiedCirc& Qualified1 , 
                            const Geom2dGcc_QCurve&     Qualified2 , 
                            const gp_Circ2d&            OnCirc     ,
                            const Standard_Real         Param1     ,
                            const Standard_Real         Param2     ,
                            const Standard_Real         Param3     ,
                            const Standard_Real         Tolerance  ) {
                              TheSame1 = Standard_False;
                              TheSame2 = Standard_False;
                              par1sol = 0.;
                              par2sol = 0.;
                              pararg1 = 0.;
                              pararg2 = 0.;
                              parcen3 = 0.;

                              WellDone = Standard_False;
                              if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                                Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                                !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                                Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                                  throw GccEnt_BadQualifier();
                                  return;
                              }
                              Standard_Real Tol = Abs(Tolerance);
                              gp_Dir2d dirx(1.,0.);
                              gp_Circ2d C1 = Qualified1.Qualified();
                              Standard_Real R1 = C1.Radius();
                              Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                              math_Vector Umin(1,4);
                              math_Vector Umax(1,4);
                              math_Vector Ufirst(1,4);
                              math_Vector tol(1,4);
                              Umin(1) = RealFirst();
                              Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                              Umin(3) = RealFirst();
                              Umin(4) = 0.;
                              Umax(1) = RealLast();
                              Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                              Umax(3) = RealLast();
                              Umax(4) = RealLast();
                              Ufirst(1) = Param1;
                              Ufirst(2) = Param2;
                              Ufirst(3) = Param3;
                              tol(1) = 2.e-15*M_PI;
                              tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                              tol(3) = 2.e-15*M_PI;
                              tol(4) = Tol/10.;
                              gp_Pnt2d point1 = ElCLib::Value(Param1,C1);
                              gp_Pnt2d point2 = Geom2dGcc_CurveTool::Value(Cu2,Param2);
                              gp_Pnt2d point3 = ElCLib::Value(Param3,OnCirc);
                              Ufirst(4) = (point3.Distance(point2)+point3.Distance(point1))/2.;
                              Geom2dGcc_FunctionTanCuCuOnCu Func(C1,Cu2,OnCirc,Max(Ufirst(4), Tol));
                              math_FunctionSetRoot Root(Func, tol);
                              Root.Perform(Func, Ufirst, Umin, Umax);
                              Func.Value(Ufirst,Umin);
                              if (Root.IsDone()) {
                                Root.Root(Ufirst);
                                //     gp_Vec2d Tan1,Tan2,Nor1;
                                gp_Vec2d Tan1,Tan2;
                                ElCLib::D1(Ufirst(1),C1,point1,Tan1);
                                Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
#ifdef OCCT_DEBUG
                                gp_Vec2d Tan3(-Sin(Ufirst(3)),Cos(Ufirst(3)));
#endif
                                point3 = ElCLib::Value(Ufirst(3),OnCirc);
                                Standard_Real dist1 = point3.Distance(point1);
                                Standard_Real dist2 = point3.Distance(point2);
                                if ( Abs(dist1-dist2)/2. <= Tol) {
                                  cirsol = gp_Circ2d(gp_Ax2d(point3,dirx),(dist1+dist2)/2.);
                                  Standard_Real normetan2 = Tan2.Magnitude();
                                  gp_Vec2d Vec1(point1,point3);
                                  gp_Vec2d Vec2(point2,point3);
                                  Standard_Real normevec2 = Vec2.Magnitude();
                                  Standard_Real angle2;
                                  if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                    angle2 = Vec2.Angle(Tan2);
                                  }
                                  else { angle2 = 0.; }
                                  Standard_Real dist = C1.Location().Distance(point3);
                                  Standard_Real Rsol = cirsol.Radius();
                                  if (Qualified1.IsUnqualified() || 
                                    (Qualified1.IsEnclosing() && Rsol >= R1 && dist <= Rsol)||
                                    (Qualified1.IsOutside() && dist >= Rsol) ||
                                    (Qualified1.IsEnclosed() && Rsol <= R1 && dist <= Rsol)) {
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&angle2<=0.)||
                                        (Qualified2.IsOutside() && angle2 >= 0) ||
                                        (Qualified2.IsEnclosed() && angle2 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          pnttg1sol = point1;
                                          pararg1 = Ufirst(1);
                                          par1sol = ElCLib::Parameter(cirsol,pnttg1sol);
                                          pnttg2sol = point2;
                                          pararg2 = Ufirst(2);
                                          par2sol = ElCLib::Parameter(cirsol,pnttg2sol);
                                          pntcen  = point3;
                                          parcen3 = Ufirst(3);
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
}

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const GccEnt_QualifiedLin&  Qualified1 , 
                            const Geom2dGcc_QCurve&     Qualified2 , 
                            const gp_Circ2d&            OnCirc     ,
                            const Standard_Real         Param1     ,
                            const Standard_Real         Param2     ,
                            const Standard_Real         Param3     ,
                            const Standard_Real         Tolerance  ) {
                              TheSame1 = Standard_False;
                              TheSame2 = Standard_False;
                              par1sol = 0.;
                              par2sol = 0.;
                              pararg1 = 0.;
                              pararg2 = 0.;
                              parcen3 = 0.;

                              WellDone = Standard_False;
                              if (!(Qualified1.IsEnclosed() ||
                                Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                                !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                                Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                                  throw GccEnt_BadQualifier();
                                  return;
                              }
                              Standard_Real Tol = Abs(Tolerance);
                              gp_Dir2d dirx(1.,0.);
                              gp_Lin2d L1 = Qualified1.Qualified();
                              Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                              math_Vector Umin(1,4);
                              math_Vector Umax(1,4);
                              math_Vector Ufirst(1,4);
                              math_Vector tol(1,4);
                              Umin(1) = RealFirst();
                              Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                              Umin(3) = RealFirst();
                              Umin(4) = 0.;
                              Umax(1) = RealLast();
                              Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                              Umax(3) = RealLast();
                              Umax(4) = RealLast();
                              Ufirst(1) = Param1;
                              Ufirst(2) = Param2;
                              Ufirst(3) = Param3;
                              tol(1) = 1.e-15;
                              tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                              tol(3) = 2.e-15*M_PI;
                              tol(4) = Tol/10.;
                              gp_Pnt2d point1 = ElCLib::Value(Param1,L1);
                              gp_Pnt2d point2 = Geom2dGcc_CurveTool::Value(Cu2,Param2);
                              gp_Pnt2d point3 = ElCLib::Value(Param3,OnCirc);
                              Ufirst(4) = (point3.Distance(point2)+point3.Distance(point1))/2.;
                              Geom2dGcc_FunctionTanCuCuOnCu Func(L1,Cu2,OnCirc,Max(Ufirst(4), Tol));
                              math_FunctionSetRoot Root(Func, tol);
                              Root.Perform(Func, Ufirst, Umin, Umax);
                              Func.Value(Ufirst,Umin);
                              if (Root.IsDone()) {
                                Root.Root(Ufirst);
                                gp_Pnt2d point1new,point2new;
                                gp_Vec2d Tan1,Tan2;
                                ElCLib::D1(Ufirst(1),L1,point1new,Tan1);
                                Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2new,Tan2);
#ifdef OCCT_DEBUG
                                gp_Vec2d Tan3(-Sin(Ufirst(3)),Cos(Ufirst(3)));
#endif
                                point3 = ElCLib::Value(Ufirst(3),OnCirc);
                                Standard_Real dist1 = point3.Distance(point1new);
                                Standard_Real dist2 = point3.Distance(point2new);
                                if ( Abs(dist1-dist2)/2. <= Tol) {
                                  cirsol = gp_Circ2d(gp_Ax2d(point3,dirx),(dist1+dist2)/2.);
                                  Standard_Real normetan2 = Tan2.Magnitude();
                                  gp_Vec2d Vec1(point1new,point3);
                                  gp_Vec2d Vec2(point2new,point3);
                                  Standard_Real normevec2 = Vec2.Magnitude();
                                  Standard_Real angle2;
                                  if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                    angle2 = Vec2.Angle(Tan2);
                                  }
                                  else { angle2 = 0.; }
                                  Standard_Real pscal=point3.XY().Dot(gp_XY(-L1.Direction().Y(),
                                    L1.Direction().X()));
                                  if (Qualified1.IsUnqualified() ||
                                    (Qualified1.IsOutside() && pscal <= 0.) ||
                                    (Qualified1.IsEnclosed() && pscal >= 0.)) {
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&angle2<=0.)||
                                        (Qualified2.IsOutside() && angle2 >= 0) ||
                                        (Qualified2.IsEnclosed() && angle2 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          pnttg1sol = point1new;
                                          pararg1 = Ufirst(1);
                                          par1sol = ElCLib::Parameter(cirsol,pnttg1sol);
                                          pnttg2sol = point2new;
                                          pararg2 = Ufirst(2);
                                          par2sol = ElCLib::Parameter(cirsol,pnttg2sol);
                                          pntcen  = point3;
                                          parcen3 = Ufirst(3);
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
}

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const Geom2dGcc_QCurve& Qualified1 , 
                            const Geom2dGcc_QCurve& Qualified2 , 
                            const gp_Circ2d&         OnCirc     ,
                            const Standard_Real               Param1     ,
                            const Standard_Real               Param2     ,
                            const Standard_Real               Param3     ,
                            const Standard_Real               Tolerance  ) {
                              TheSame1 = Standard_False;
                              TheSame2 = Standard_False;
                              par1sol = 0.;
                              par2sol = 0.;
                              pararg1 = 0.;
                              pararg2 = 0.;
                              parcen3 = 0.;

                              WellDone = Standard_False;
                              if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                                Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                                !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                                Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                                  throw GccEnt_BadQualifier();
                                  return;
                              }
                              Standard_Real Tol = Abs(Tolerance);
                              gp_Dir2d dirx(1.,0.);
                              Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
                              Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                              math_Vector Umin(1,4);
                              math_Vector Umax(1,4);
                              math_Vector Ufirst(1,4);
                              math_Vector tol(1,4);
                              Umin(1) = Geom2dGcc_CurveTool::FirstParameter(Cu1);
                              Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                              Umin(3) = RealFirst();
                              Umin(4) = 0.;
                              Umax(1) = Geom2dGcc_CurveTool::LastParameter(Cu1);
                              Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                              Umax(3) = RealLast();
                              Umax(4) = RealLast();
                              Ufirst(1) = Param1;
                              Ufirst(2) = Param2;
                              Ufirst(3) = Param3;
                              tol(1) = Geom2dGcc_CurveTool::EpsX(Cu1,Abs(Tolerance));
                              tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                              tol(3) = 2.e-15*M_PI;
                              tol(4) = Tol/10.;
                              gp_Pnt2d point1 = Geom2dGcc_CurveTool::Value(Cu1,Param1);
                              gp_Pnt2d point2 = Geom2dGcc_CurveTool::Value(Cu2,Param2);
                              Standard_Real R1 = OnCirc.Radius();
                              gp_Pnt2d point3(OnCirc.Location().XY()+R1*gp_XY(Cos(Param3),Sin(Param3)));
                              Ufirst(4) = (point3.Distance(point2)+point3.Distance(point1))/2.;
                              Geom2dGcc_FunctionTanCuCuOnCu Func(Cu1,Cu2,OnCirc,Max(Ufirst(4), Tol));
                              math_FunctionSetRoot Root(Func, tol);
                              Root.Perform(Func, Ufirst, Umin, Umax);
                              Func.Value(Ufirst,Umin);
                              if (Root.IsDone()) {
                                Root.Root(Ufirst);
                                //     gp_Vec2d Tan1,Tan2,Nor1;
                                gp_Vec2d Tan1,Tan2;
                                Geom2dGcc_CurveTool::D1(Cu1,Ufirst(1),point1,Tan1);
                                Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
#ifdef OCCT_DEBUG
                                gp_Vec2d Tan3(-Sin(Ufirst(3)),Cos(Ufirst(3)));
#endif
                                point3 = gp_Pnt2d(OnCirc.Location().XY()+
                                  R1*gp_XY(Cos(Ufirst(3)),Sin(Ufirst(3))));
                                Standard_Real dist1 = point3.Distance(point1);
                                Standard_Real dist2 = point3.Distance(point2);
                                if ( Abs(dist1-dist2)/2. <= Tol) {
                                  cirsol = gp_Circ2d(gp_Ax2d(point3,dirx),(dist1+dist2)/2.);
                                  Standard_Real normetan1 = Tan1.Magnitude();
                                  Standard_Real normetan2 = Tan2.Magnitude();
                                  gp_Vec2d Vec1(point1,point3);
                                  gp_Vec2d Vec2(point2,point3);
                                  Standard_Real normevec1 = Vec1.Magnitude();
                                  Standard_Real normevec2 = Vec2.Magnitude();
                                  Standard_Real angle1,angle2;
                                  if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                    angle1 = Vec1.Angle(Tan1);
                                  }
                                  else { angle1 = 0.; }
                                  if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                    angle2 = Vec2.Angle(Tan2);
                                  }
                                  else { angle2 = 0.; }
                                  if (Qualified1.IsUnqualified()||
                                    (Qualified1.IsEnclosing()&&angle1<=0.)||
                                    (Qualified1.IsOutside() && angle1 >= 0.) ||
                                    (Qualified1.IsEnclosed() && angle1 <= 0.)) {
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&angle2<=0.)||
                                        (Qualified2.IsOutside() && angle2 >= 0) ||
                                        (Qualified2.IsEnclosed() && angle2 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          pararg1 = Ufirst(1);
                                          par1sol = 0.;
                                          pnttg1sol = point1;
                                          pararg2 = Ufirst(2);
                                          pnttg2sol = point2;
                                          par2sol = pnttg2sol.Distance(pnttg1sol);
                                          pntcen  = point3;
                                          parcen3 = Ufirst(3);
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
}

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const Geom2dGcc_QCurve&  Qualified1 ,
                            const gp_Pnt2d&          Point2     ,
                            const gp_Circ2d&         OnCirc     ,
                            const Standard_Real      Param1     ,
                            const Standard_Real      Param2     ,
                            const Standard_Real      Tolerance  ) {
                              TheSame1 = Standard_False;
                              TheSame2 = Standard_False;
                              par1sol = 0.;
                              par2sol = 0.;
                              pararg1 = 0.;
                              pararg2 = 0.;
                              parcen3 = 0.;

                              WellDone = Standard_False;
                              if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                                Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
                                  throw GccEnt_BadQualifier();
                                  return;
                              }
                              Standard_Real Tol = Abs(Tolerance);
                              gp_Dir2d dirx(1.,0.);
                              Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
                              math_Vector Umin(1,3);
                              math_Vector Umax(1,3);
                              math_Vector Ufirst(1,3);
                              math_Vector tol(1,3);
                              Umin(1) = Geom2dGcc_CurveTool::FirstParameter(Cu1);
                              Umin(2) = RealFirst();
                              Umin(3) = 0.;
                              Umax(1) = Geom2dGcc_CurveTool::LastParameter(Cu1);
                              Umax(2) = RealLast();
                              Umax(3) = RealLast();
                              Ufirst(1) = Param1;
                              Ufirst(2) = Param2;
                              tol(1) = Geom2dGcc_CurveTool::EpsX(Cu1,Abs(Tolerance));
                              tol(2) = 2.e-15*M_PI;
                              tol(3) = Tol/10.;
                              gp_Pnt2d point1 = Geom2dGcc_CurveTool::Value(Cu1,Param1);
                              gp_Pnt2d point3 = ElCLib::Value(Param2,OnCirc);
                              Ufirst(3) = (point3.Distance(Point2)+point3.Distance(point1))/2.;
                              Geom2dGcc_FunctionTanCuCuOnCu Func(Cu1,Point2,OnCirc,Max(Ufirst(3), Tol));
                              math_FunctionSetRoot Root(Func, tol);
                              Root.Perform(Func, Ufirst, Umin, Umax);
                              Func.Value(Ufirst,Umin);
                              if (Root.IsDone()) {
                                Root.Root(Ufirst);
                                gp_Pnt2d point1new,point3new;
                                gp_Vec2d Tan1,Tan3;
                                Geom2dGcc_CurveTool::D1(Cu1,Ufirst(1),point1new,Tan1);
                                ElCLib::D1(Ufirst(2),OnCirc,point3new,Tan3);
                                Standard_Real dist1 = point3new.Distance(point1new);
                                Standard_Real dist2 = point3new.Distance(Point2);
                                if ( Abs(dist1-dist2)/2. <= Tol) {
                                  cirsol = gp_Circ2d(gp_Ax2d(point3new,dirx),(dist1+dist2)/2.);
                                  Standard_Real normetan1 = Tan1.Magnitude();
                                  gp_Vec2d Vec1(point1new,point3new);
                                  Standard_Real normevec1 = Vec1.Magnitude();
                                  Standard_Real angle1;
                                  if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                    angle1 = Vec1.Angle(Tan1);
                                  }
                                  else { angle1 = 0.; }
                                  if (Qualified1.IsUnqualified()||
                                    (Qualified1.IsEnclosing()&&angle1<=0.)||
                                    (Qualified1.IsOutside() && angle1 >= 0.) ||
                                    (Qualified1.IsEnclosed() && angle1 <= 0.)) {
                                      qualifier1 = Qualified1.Qualifier();
                                      qualifier2 = GccEnt_noqualifier;
                                      pnttg1sol = point1new;
                                      pararg1 = Ufirst(1);
                                      par1sol = ElCLib::Parameter(cirsol,pnttg1sol);
                                      pnttg2sol = Point2;
                                      pararg2 = 0.;
                                      par2sol = ElCLib::Parameter(cirsol,pnttg2sol);
                                      pntcen  = point3new;
                                      parcen3 = Ufirst(3);
                                      WellDone = Standard_True;
                                  }
                                }
                              }
}

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const Geom2dGcc_QCurve& Qualified1 , 
                            const Geom2dGcc_QCurve& Qualified2 , 
                            const Geom2dAdaptor_Curve&          OnCurv     ,
                            const Standard_Real               Param1     ,
                            const Standard_Real               Param2     ,
                            const Standard_Real               Param3     ,
                            const Standard_Real               Tolerance  ) {
                              TheSame1 = Standard_False;
                              TheSame2 = Standard_False;
                              par1sol = 0.;
                              par2sol = 0.;
                              pararg1 = 0.;
                              pararg2 = 0.;
                              parcen3 = 0.;

                              WellDone = Standard_False;
                              Standard_Real Tol = Abs(Tolerance);
                              WellDone = Standard_False;
                              if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                                Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                                !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                                Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                                  throw GccEnt_BadQualifier();
                                  return;
                              }
                              gp_Dir2d dirx(1.,0.);
                              Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
                              Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                              math_Vector Umin(1,4);
                              math_Vector Umax(1,4);
                              math_Vector Ufirst(1,4);
                              math_Vector tol(1,4);
                              Umin(1) = Geom2dGcc_CurveTool::FirstParameter(Cu1);
                              Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                              Umin(3) = Geom2dGcc_CurveTool::FirstParameter(OnCurv);
                              Umin(4) = 0.;
                              Umax(1) = Geom2dGcc_CurveTool::LastParameter(Cu1);
                              Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                              Umax(3) = Geom2dGcc_CurveTool::LastParameter(OnCurv);
                              Umax(4) = RealLast();
                              Ufirst(1) = Param1;
                              Ufirst(2) = Param2;
                              Ufirst(3) = Param3;
                              tol(1) = Geom2dGcc_CurveTool::EpsX(Cu1,Abs(Tolerance));
                              tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                              tol(3) = Geom2dGcc_CurveTool::EpsX(OnCurv,Abs(Tolerance));
                              tol(4) = Tol/10.;
                              gp_Pnt2d point1 = Geom2dGcc_CurveTool::Value(Cu1,Param1);
                              gp_Pnt2d point2 = Geom2dGcc_CurveTool::Value(Cu2,Param2);
                              gp_Pnt2d point3 = Geom2dGcc_CurveTool::Value(OnCurv,Param3);
                              Ufirst(4) = (point3.Distance(point2)+point3.Distance(point1))/2.;
                              Geom2dGcc_FunctionTanCuCuOnCu Func(Cu1,Cu2,OnCurv,Max(Ufirst(4), Tol));
                              math_FunctionSetRoot Root(Func, tol);
                              Root.Perform(Func, Ufirst, Umin, Umax);
                              Func.Value(Ufirst,Umin);
                              if (Root.IsDone()) {
                                Root.Root(Ufirst);
                                gp_Vec2d Tan1,Tan2,Tan3;
                                Geom2dGcc_CurveTool::D1(Cu1,Ufirst(1),point1,Tan1);
                                Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
                                Geom2dGcc_CurveTool::D1(OnCurv,Ufirst(3),point3,Tan3);
                                Standard_Real dist1 = point3.Distance(point1);
                                Standard_Real dist2 = point3.Distance(point2);
                                if ( Abs(dist1-dist2)/2. <= Tol) {
                                  cirsol = gp_Circ2d(gp_Ax2d(point3,dirx),(dist1+dist2)/2.);
                                  Standard_Real normetan1 = Tan1.Magnitude();
                                  Standard_Real normetan2 = Tan2.Magnitude();
                                  gp_Vec2d Vec1(point1,point3);
                                  gp_Vec2d Vec2(point2,point3);
                                  Standard_Real normevec1 = Vec1.Magnitude();
                                  Standard_Real normevec2 = Vec2.Magnitude();
                                  Standard_Real angle1,angle2;
                                  if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                    angle1 = Vec1.Angle(Tan1);
                                  }
                                  else { angle1 = 0.; }
                                  if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                    angle2 = Vec2.Angle(Tan2);
                                  }
                                  else { angle2 = 0.; }
                                  if (Qualified1.IsUnqualified()||
                                    (Qualified1.IsEnclosing()&&angle1<=0.)||
                                    (Qualified1.IsOutside() && angle1 >= 0.) ||
                                    (Qualified1.IsEnclosed() && angle1 <= 0.)) {
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&angle2<=0.)||
                                        (Qualified2.IsOutside() && angle2 >= 0) ||
                                        (Qualified2.IsEnclosed() && angle2 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          pararg1 = Ufirst(1);
                                          par1sol = 0.;
                                          pnttg1sol = point1;
                                          pararg2 = Ufirst(2);
                                          pnttg2sol = point2;
                                          par2sol = pnttg2sol.Distance(pnttg1sol);
                                          pntcen  = point3;
                                          parcen3 = Ufirst(3);
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
}

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const GccEnt_QualifiedCirc& Qualified1 , 
                            const Geom2dGcc_QCurve&       Qualified2 , 
                            const Geom2dAdaptor_Curve&                OnCurv     ,
                            const Standard_Real                     Param1     ,
                            const Standard_Real                     Param2     ,
                            const Standard_Real                     ParamOn    ,
                            const Standard_Real                     Tolerance     ) {

                              TheSame1 = Standard_False;
                              TheSame2 = Standard_False;
                              par1sol = 0.;
                              par2sol = 0.;
                              pararg1 = 0.;
                              pararg2 = 0.;
                              parcen3 = 0.;

                              WellDone = Standard_False;
                              if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                                Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                                !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                                Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                                  throw GccEnt_BadQualifier();
                                  return;
                              }
                              Standard_Real Tol = Abs(Tolerance);
                              gp_Circ2d C1 = Qualified1.Qualified();
                              Standard_Real R1 = C1.Radius();
                              Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                              math_Vector Umin(1,4);
                              math_Vector Umax(1,4);
                              math_Vector Ufirst(1,4);
                              math_Vector tol(1,4);
                              Umin(1) = RealFirst();
                              Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                              Umin(3) = Geom2dGcc_CurveTool::FirstParameter(OnCurv);
                              Umin(4) = 0.;
                              Umax(1) = RealLast();
                              Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                              Umax(3) = Geom2dGcc_CurveTool::LastParameter(OnCurv);
                              Umax(4) = RealLast();
                              Ufirst(1) = Param1;
                              Ufirst(2) = Param2;
                              Ufirst(3) = ParamOn;
                              tol(1) = 2.e-15*M_PI;
                              tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                              tol(3) = Geom2dGcc_CurveTool::EpsX(OnCurv,Abs(Tolerance));
                              tol(4) = Tol/10.;;
                              gp_Pnt2d point1 = ElCLib::Value(Param1,C1);
                              gp_Pnt2d point2 = Geom2dGcc_CurveTool::Value(Cu2,Param2);
                              gp_Pnt2d point3 = Geom2dGcc_CurveTool::Value(OnCurv,ParamOn);
                              Ufirst(4) = (point3.Distance(point2)+point3.Distance(point1))/2.;
                              Geom2dGcc_FunctionTanCuCuOnCu Func(C1,Cu2,OnCurv,Max(Ufirst(4), Tol));
                              math_FunctionSetRoot Root(Func, tol);
                              Root.Perform(Func, Ufirst, Umin, Umax);
                              Func.Value(Ufirst,Umin);
                              if (Root.IsDone()) {
                                Root.Root(Ufirst);
                                gp_Vec2d Tan1,Tan2,Tan3;
                                Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
                                Geom2dGcc_CurveTool::D1(OnCurv,Ufirst(3),point3,Tan3);
                                ElCLib::D1(Ufirst(1),C1,point1,Tan1);
                                Standard_Real dist1 = point3.Distance(point1);
                                Standard_Real dist2 = point3.Distance(point2);
                                if ( Abs(dist1-dist2)/2. <= Tol) {
                                  gp_Dir2d dirx(1.,0.);
                                  cirsol = gp_Circ2d(gp_Ax2d(point3,dirx),(dist1+dist2)/2.);
                                  Standard_Real normetan2 = Tan2.Magnitude();
                                  gp_Vec2d Vec1(point1.XY(),point3.XY());
                                  gp_Vec2d Vec2(point2.XY(),point3.XY());
                                  Standard_Real normevec2 = Vec2.Magnitude();
                                  Standard_Real angle2;
                                  if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                    angle2 = Vec2.Angle(Tan2);
                                  }
                                  else { angle2 = 0.; }
                                  Standard_Real dist = C1.Location().Distance(point3);
                                  Standard_Real Rsol = cirsol.Radius();
                                  if (Qualified1.IsUnqualified() || 
                                    (Qualified1.IsEnclosing() && Rsol >= R1 && dist <= Rsol)||
                                    (Qualified1.IsOutside() && dist >= Rsol) ||
                                    (Qualified1.IsEnclosed() && Rsol <= R1 && dist <= Rsol)) {
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&angle2<=0.)||
                                        (Qualified2.IsOutside() && angle2 >= 0) ||
                                        (Qualified2.IsEnclosed() && angle2 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          pnttg1sol = point1;
                                          pararg1 = Ufirst(1);
                                          par1sol = ElCLib::Parameter(cirsol,pnttg1sol);
                                          pnttg2sol = point2;
                                          pararg2 = Ufirst(2);
                                          par2sol = ElCLib::Parameter(cirsol,pnttg2sol);
                                          pntcen  = point3;
                                          parcen3 = Ufirst(3);
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
}

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const GccEnt_QualifiedLin&  Qualified1 , 
                            const Geom2dGcc_QCurve&     Qualified2 , 
                            const Geom2dAdaptor_Curve&                OnCurv     ,
                            const Standard_Real                     Param1     ,
                            const Standard_Real                     Param2     ,
                            const Standard_Real                     ParamOn    ,
                            const Standard_Real                     Tolerance  ) {
                              TheSame1 = Standard_False;
                              TheSame2 = Standard_False;
                              par1sol = 0.;
                              par2sol = 0.;
                              pararg1 = 0.;
                              pararg2 = 0.;
                              parcen3 = 0.;

                              WellDone = Standard_False;
                              if (!(Qualified1.IsEnclosed() ||
                                Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                                !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                                Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                                  throw GccEnt_BadQualifier();
                                  return;
                              }
                              Standard_Real Tol = Abs(Tolerance);
                              gp_Dir2d dirx(1.,0.);
                              gp_Lin2d L1 = Qualified1.Qualified();
                              Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                              math_Vector Umin(1,4);
                              math_Vector Umax(1,4);
                              math_Vector Ufirst(1,4);
                              math_Vector tol(1,4);
                              Umin(1) = RealFirst();
                              Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                              Umin(3) = Geom2dGcc_CurveTool::FirstParameter(OnCurv);
                              Umin(4) = 0.;
                              Umax(1) = RealLast();
                              Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                              Umax(3) = Geom2dGcc_CurveTool::LastParameter(OnCurv);
                              Umax(4) = RealLast();
                              Ufirst(1) = Param1;
                              Ufirst(2) = Param2;
                              Ufirst(3) = ParamOn;
                              tol(1) = 1.e-15;
                              tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                              tol(3) = Geom2dGcc_CurveTool::EpsX(OnCurv,Abs(Tolerance));
                              tol(4) = Tol/10.;
                              gp_Pnt2d point1 = ElCLib::Value(Param1,L1);
                              gp_Pnt2d point2 = Geom2dGcc_CurveTool::Value(Cu2,Param2);
                              gp_Pnt2d point3 = Geom2dGcc_CurveTool::Value(OnCurv,ParamOn);
                              Ufirst(4) = (point3.Distance(point2)+point3.Distance(point1))/2.;
                              Geom2dGcc_FunctionTanCuCuOnCu Func(L1,Cu2,OnCurv,Max(Ufirst(4), Tol));
                              math_FunctionSetRoot Root(Func, tol);
                              Root.Perform(Func, Ufirst, Umin, Umax);
                              Func.Value(Ufirst,Umin);
                              if (Root.IsDone()) {
                                Root.Root(Ufirst);
                                gp_Vec2d Tan1,Tan2,Tan3;
                                ElCLib::D1(Ufirst(1),L1,point1,Tan1);
                                Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
                                Geom2dGcc_CurveTool::D1(OnCurv,Ufirst(3),point3,Tan3);
                                Standard_Real dist1 = point3.Distance(point1);
                                Standard_Real dist2 = point3.Distance(point2);
                                if ( Abs(dist1-dist2)/2. <= Tol) {
                                  cirsol = gp_Circ2d(gp_Ax2d(point3,dirx),(dist1+dist2)/2.);
                                  Standard_Real normetan2 = Tan2.Magnitude();
                                  gp_Vec2d Vec1(point1,point3);
                                  gp_Vec2d Vec2(point2,point3);
                                  Standard_Real normevec2 = Vec2.Magnitude();
                                  Standard_Real angle2;
                                  if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                    angle2 = Vec2.Angle(Tan2);
                                  }
                                  else { angle2 = 0.; }
                                  Standard_Real pscal=point3.XY().Dot(gp_XY(-L1.Direction().Y(),
                                    L1.Direction().X()));
                                  if (Qualified1.IsUnqualified() ||
                                    (Qualified1.IsOutside() && pscal <= 0.) ||
                                    (Qualified1.IsEnclosed() && pscal >= 0.)) {
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&angle2<=0.)||
                                        (Qualified2.IsOutside() && angle2 >= 0) ||
                                        (Qualified2.IsEnclosed() && angle2 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          pnttg1sol = point1;
                                          pararg1 = Ufirst(1);
                                          par1sol = ElCLib::Parameter(cirsol,pnttg1sol);
                                          pnttg2sol = point2;
                                          pararg2 = Ufirst(2);
                                          par2sol = ElCLib::Parameter(cirsol,pnttg2sol);
                                          pntcen  = point3;
                                          parcen3 = Ufirst(3);
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
}

Geom2dGcc_Circ2d2TanOnIter::
Geom2dGcc_Circ2d2TanOnIter (const Geom2dGcc_QCurve&    Qualified1 ,
                            const gp_Pnt2d&             Point2     ,
                            const Geom2dAdaptor_Curve&             OnCurv     ,
                            const Standard_Real                  Param1     ,
                            const Standard_Real                  ParamOn    ,
                            const Standard_Real                  Tolerance  ) 
{
  TheSame1 = Standard_False;
  TheSame2 = Standard_False;
  par1sol = 0.;
  par2sol = 0.;
  pararg1 = 0.;
  pararg2 = 0.;
  parcen3 = 0.;

  WellDone = Standard_False;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
    Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  Standard_Real Tol = Abs(Tolerance);
  gp_Dir2d dirx(1.,0.);
  Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
  math_Vector Umin(1,3);
  math_Vector Umax(1,3);
  math_Vector Ufirst(1,3);
  math_Vector tol(1,3);
  Umin(1) = Geom2dGcc_CurveTool::FirstParameter(Cu1);
  Umin(2) = RealFirst();
  Umin(3) = Geom2dGcc_CurveTool::FirstParameter(OnCurv);
  Umax(1) = Geom2dGcc_CurveTool::LastParameter(Cu1);
  Umax(2) = RealLast();
  Umax(3) = Geom2dGcc_CurveTool::LastParameter(OnCurv);
  Ufirst(1) = Param1;
  Ufirst(2) = ParamOn;
  tol(1) = Geom2dGcc_CurveTool::EpsX(Cu1,Abs(Tolerance));
  tol(2) = Geom2dGcc_CurveTool::EpsX(OnCurv,Abs(Tolerance));
  tol(3) = Tol/10.;
  gp_Pnt2d point1 = Geom2dGcc_CurveTool::Value(Cu1,Param1);
  gp_Pnt2d point3 = Geom2dGcc_CurveTool::Value(OnCurv,ParamOn);
  Ufirst(3) = (point3.Distance(Point2)+point3.Distance(point1))/2.;
  Geom2dGcc_FunctionTanCuCuOnCu Func(Cu1,Point2,OnCurv,Max(Ufirst(3), Tol));
  math_FunctionSetRoot Root(Func, tol);
  Root.Perform(Func, Ufirst, Umin, Umax);
  Func.Value(Ufirst,Umin);
  if (Root.IsDone()) {
    Root.Root(Ufirst);
    //    gp_Vec2d Tan1,Tan2,Tan3;
    gp_Vec2d Tan1,Tan3;
    Geom2dGcc_CurveTool::D1(Cu1,Ufirst(1),point1,Tan1);
    Geom2dGcc_CurveTool::D1(OnCurv,Ufirst(3),point3,Tan3);
    Standard_Real dist1 = point3.Distance(point1);
    Standard_Real dist2 = point3.Distance(Point2);
    if ( Abs(dist1-dist2)/2. <= Tol) {
      cirsol = gp_Circ2d(gp_Ax2d(point3,dirx),(dist1+dist2)/2.);
      Standard_Real normetan1 = Tan1.Magnitude();
      gp_Vec2d Vec1(point1,point3);
      Standard_Real normevec1 = Vec1.Magnitude();
      Standard_Real angle1;
      if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
        angle1 = Vec1.Angle(Tan1);
      }
      else { angle1 = 0.; }
      if (Qualified1.IsUnqualified()||
        (Qualified1.IsEnclosing()&&angle1<=0.)||
        (Qualified1.IsOutside() && angle1 >= 0.) ||
        (Qualified1.IsEnclosed() && angle1 <= 0.)) {
          qualifier1 = Qualified1.Qualifier();
          qualifier2 = GccEnt_noqualifier;
          pnttg1sol = point1;
          pararg1 = Ufirst(1);
          par1sol = ElCLib::Parameter(cirsol,pnttg1sol);
          pnttg2sol = Point2;
          pararg2 = 0.;
          par2sol = ElCLib::Parameter(cirsol,pnttg2sol);
          pntcen  = point3;
          parcen3 = Ufirst(3);
          WellDone = Standard_True;
      }
    }
  }
}

Standard_Boolean Geom2dGcc_Circ2d2TanOnIter::
IsDone () const{ return WellDone; }

gp_Circ2d Geom2dGcc_Circ2d2TanOnIter::
ThisSolution () const{ return cirsol; }

void Geom2dGcc_Circ2d2TanOnIter:: 
WhichQualifier (GccEnt_Position& Qualif1  ,
                GccEnt_Position& Qualif2  ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else {
    Qualif1 = qualifier1;
    Qualif2 = qualifier2;
  }
}

void Geom2dGcc_Circ2d2TanOnIter:: 
Tangency1 (Standard_Real&      ParSol         ,
           Standard_Real&      ParArg         ,
           gp_Pnt2d&  PntSol         ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else {
    if (TheSame1 == 0) {
      ParSol = 0;
      ParArg = 0;
      PntSol = pnttg1sol;
    }
    else { throw StdFail_NotDone(); }
  }
}

void Geom2dGcc_Circ2d2TanOnIter:: 
Tangency2 (Standard_Real&      ParSol         ,
           Standard_Real&      ParArg         ,
           gp_Pnt2d&  PntSol         ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else {
    ParSol = 0;
    ParArg = 0;
    PntSol = pnttg2sol;
  }
}

void Geom2dGcc_Circ2d2TanOnIter::
CenterOn3 (Standard_Real&      ParArg         ,
           gp_Pnt2d&  PntSol         ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else {
    ParArg = 0;
    PntSol = pntcen;
  }
}

Standard_Boolean Geom2dGcc_Circ2d2TanOnIter::
IsTheSame1 () const
{
  if (!WellDone) throw StdFail_NotDone();

  if (TheSame1 == 0) 
    return Standard_False;
  return Standard_True;
}


Standard_Boolean Geom2dGcc_Circ2d2TanOnIter::
IsTheSame2 () const
{
  if (!WellDone) throw StdFail_NotDone();
  return Standard_False;
}
