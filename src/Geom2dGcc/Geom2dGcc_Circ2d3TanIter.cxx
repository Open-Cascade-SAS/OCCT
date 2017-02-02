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

#include <GccAna_Circ2d3Tan.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <Geom2dGcc_Circ2d3TanIter.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_FunctionTanCuCuCu.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <StdFail_NotDone.hxx>

Geom2dGcc_Circ2d3TanIter::
Geom2dGcc_Circ2d3TanIter (const Geom2dGcc_QCurve& Qualified1 , 
                          const Geom2dGcc_QCurve& Qualified2 ,
                          const Geom2dGcc_QCurve& Qualified3 , 
                          const Standard_Real      Param1     ,
                          const Standard_Real      Param2     ,
                          const Standard_Real      Param3     ,
                          const Standard_Real      Tolerance  ) {

                            TheSame1 = Standard_False;
                            TheSame2 = Standard_False;
                            TheSame3 = Standard_False;
                            par1sol = 0.;
                            par2sol = 0.;
                            par3sol = 0.;
                            pararg1 = 0.;
                            pararg2 = 0.;
                            pararg3 = 0.;

                            Standard_Real Tol = Abs(Tolerance);
                            WellDone = Standard_False;
                            if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                              Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                              !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                              Qualified2.IsOutside() || Qualified2.IsUnqualified()) ||
                              !(Qualified3.IsEnclosed() || Qualified3.IsEnclosing() || 
                              Qualified3.IsOutside() || Qualified3.IsUnqualified())) {
                                throw GccEnt_BadQualifier();
                                return;
                            }
                            Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
                            Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                            Geom2dAdaptor_Curve Cu3 = Qualified3.Qualified();
                            Geom2dGcc_FunctionTanCuCuCu Func(Cu1,Cu2,Cu3);
                            math_Vector Umin(1,3);
                            math_Vector Umax(1,3);
                            math_Vector Ufirst(1,3);
                            math_Vector tol(1,3);
                            Umin(1) = Geom2dGcc_CurveTool::FirstParameter(Cu1);
                            Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                            Umin(3) = Geom2dGcc_CurveTool::FirstParameter(Cu3);
                            Umax(1) = Geom2dGcc_CurveTool::LastParameter(Cu1);
                            Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                            Umax(3) = Geom2dGcc_CurveTool::LastParameter(Cu3);
                            Ufirst(1) = Param1;
                            Ufirst(2) = Param2;
                            Ufirst(3) = Param3;
                            tol(1) = Geom2dGcc_CurveTool::EpsX(Cu1,Abs(Tolerance));
                            tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                            tol(3) = Geom2dGcc_CurveTool::EpsX(Cu3,Abs(Tolerance));   
                            math_FunctionSetRoot Root(Func, tol);
                            Root.Perform(Func, Ufirst, Umin, Umax);
                            if (Root.IsDone()) {
                              Root.Root(Ufirst);
                              Func.Value(Ufirst,Umin);
                              gp_Pnt2d point1,point2,point3;
                              gp_Vec2d Tan1,Tan2,Tan3;
                              Geom2dGcc_CurveTool::D1(Cu1,Ufirst(1),point1,Tan1);
                              Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
                              Geom2dGcc_CurveTool::D1(Cu3,Ufirst(3),point3,Tan3);
                              GccAna_Circ2d3Tan circ(point1,point2,point3,Tol);
                              if (circ.IsDone()) { 
                                cirsol = circ.ThisSolution(1);
                                gp_Pnt2d centre = cirsol.Location();
                                Standard_Real normetan1 = Tan1.Magnitude();
                                Standard_Real normetan2 = Tan2.Magnitude();
                                Standard_Real normetan3 = Tan3.Magnitude();
                                gp_Vec2d Vec1(point1,centre);
                                gp_Vec2d Vec2(point2,centre);
                                gp_Vec2d Vec3(point3,centre);
                                Standard_Real normevec1 = Vec1.Magnitude();
                                Standard_Real normevec2 = Vec2.Magnitude();
                                Standard_Real normevec3 = Vec3.Magnitude();
                                Standard_Real dot1,dot2,dot3;
                                if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                  dot1 = Vec1.Dot(Tan1)/(normevec1*normetan1);
                                }
                                else { dot1 = 0.; }
                                if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                  dot2 = Vec2.Dot(Tan2)/(normevec2*normetan2);
                                }
                                else { dot2 = 0.; }
                                if (normevec3 >= gp::Resolution() && normetan3 >= gp::Resolution()) {
                                  dot3 = Vec3.Dot(Tan3)/(normevec3*normetan3);
                                }
                                else { dot3 = 0.; }
                                Tol = 1.e-12;
                                if (dot1 <= Tol && dot2 <=Tol && dot3 <= Tol) {
                                  Standard_Real Angle1 = Vec1.Angle(Tan1);
                                  if (Qualified1.IsUnqualified()||
                                    (Qualified1.IsEnclosing()&&Angle1<=0.)||
                                    (Qualified1.IsOutside() && Angle1 >= 0.) ||
                                    (Qualified1.IsEnclosed() && Angle1 <= 0.)) {
                                      Angle1 = Vec2.Angle(Tan2);
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&Angle1<=0.)||
                                        (Qualified2.IsOutside() && Angle1 >= 0) ||
                                        (Qualified2.IsEnclosed() && Angle1 <= 0.)) {
                                          Angle1 = Vec3.Angle(Tan3);
                                          if (Qualified3.IsUnqualified() || 
                                            (Qualified3.IsEnclosing()&&Angle1<=0.)||
                                            (Qualified3.IsOutside() && Angle1 >= 0) ||
                                            (Qualified3.IsEnclosed() && Angle1 <= 0.)) {
                                              qualifier1 = Qualified1.Qualifier();
                                              qualifier2 = Qualified2.Qualifier();
                                              qualifier3 = Qualified3.Qualifier();
                                              pararg1 = Ufirst(1);
                                              par1sol = 0.;
                                              pnttg1sol = point1;
                                              pararg2 = Ufirst(2);
                                              pnttg2sol = point2;
                                              par2sol = pnttg2sol.Distance(pnttg1sol);
                                              pnttg3sol = point3;
                                              pararg3 = Ufirst(3);
                                              par3sol = pnttg3sol.Distance(pnttg1sol);
                                              WellDone = Standard_True;
                                          }
                                      }
                                  }
                                }
                              }
                            }
}

Geom2dGcc_Circ2d3TanIter::
Geom2dGcc_Circ2d3TanIter (const GccEnt_QualifiedCirc& Qualified1 , 
                          const Geom2dGcc_QCurve& Qualified2 ,
                          const Geom2dGcc_QCurve& Qualified3 , 
                          const Standard_Real      Param1     ,
                          const Standard_Real      Param2     ,
                          const Standard_Real      Param3     ,
                          const Standard_Real      Tolerance     ) {

                            TheSame1 = Standard_False;
                            TheSame2 = Standard_False;
                            TheSame3 = Standard_False;
                            par1sol = 0.;
                            par2sol = 0.;
                            par3sol = 0.;
                            pararg1 = 0.;
                            pararg2 = 0.;
                            pararg3 = 0.;

                            Standard_Real Tol = Abs(Tolerance);
                            WellDone = Standard_False;
                            if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                              Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                              !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                              Qualified2.IsOutside() || Qualified2.IsUnqualified()) ||
                              !(Qualified3.IsEnclosed() || Qualified3.IsEnclosing() || 
                              Qualified3.IsOutside() || Qualified3.IsUnqualified())) {
                                throw GccEnt_BadQualifier();
                                return;
                            }
                            gp_Circ2d C1 = Qualified1.Qualified();
                            Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                            Geom2dAdaptor_Curve Cu3 = Qualified3.Qualified();
                            Geom2dGcc_FunctionTanCuCuCu Func(C1,Cu2,Cu3);
                            math_Vector Umin(1,3);
                            math_Vector Umax(1,3);
                            math_Vector Ufirst(1,3);
                            math_Vector tol(1,3);
                            Umin(1) = 0.;
                            Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                            Umin(3) = Geom2dGcc_CurveTool::FirstParameter(Cu3);
                            Umax(1) = 2*M_PI;
                            Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                            Umax(3) = Geom2dGcc_CurveTool::LastParameter(Cu3);
                            Ufirst(1) = Param1;
                            Ufirst(2) = Param2;
                            Ufirst(3) = Param3;
                            tol(1) = 2.e-15*M_PI;
                            tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                            tol(3) = Geom2dGcc_CurveTool::EpsX(Cu3,Abs(Tolerance));   
                            math_FunctionSetRoot Root(Func, tol);
                            Root.Perform(Func, Ufirst, Umin, Umax);
                            if (Root.IsDone()) {
                              Root.Root(Ufirst);
                              Func.Value(Ufirst,Umin);
                              gp_Pnt2d centre1(C1.Location());
                              Standard_Real R1 = C1.Radius();
                              gp_Pnt2d point1(centre1.XY()+R1*gp_XY(Cos(Ufirst(1)),Sin(Ufirst(1))));
                              gp_Vec2d Tan1(gp_XY(-Sin(Ufirst(1)),Cos(Ufirst(1))));
                              gp_Pnt2d point2,point3;
                              //     gp_Vec2d Tan2,Tan3,Nor2,Nor3;
                              gp_Vec2d Tan2,Tan3;
                              Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
                              Geom2dGcc_CurveTool::D1(Cu3,Ufirst(3),point3,Tan3);
                              GccAna_Circ2d3Tan circ(point1,point2,point3,Tol);
                              if (circ.IsDone()) {
                                cirsol = circ.ThisSolution(1);
                                gp_Pnt2d centre(cirsol.Location());
                                Standard_Real dist = centre1.Distance(centre);
                                Standard_Real Rsol = cirsol.Radius();
                                Standard_Real normetan1 = Tan1.Magnitude();
                                Standard_Real normetan2 = Tan2.Magnitude();
                                Standard_Real normetan3 = Tan3.Magnitude();
                                gp_Vec2d Vec1(point1,centre);
                                gp_Vec2d Vec2(point2,centre);
                                gp_Vec2d Vec3(point3,centre);
                                Standard_Real normevec1 = Vec1.Magnitude();
                                Standard_Real normevec2 = Vec2.Magnitude();
                                Standard_Real normevec3 = Vec3.Magnitude();
                                Standard_Real dot1,dot2,dot3;
                                if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                  dot1 = Vec1.Dot(Tan1)/(normevec1*normetan1);
                                }
                                else { dot1 = 0.; }
                                if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                  dot2 = Vec2.Dot(Tan2)/(normevec2*normetan2);
                                }
                                else { dot2 = 0.; }
                                if (normevec3 >= gp::Resolution() && normetan3 >= gp::Resolution()) {
                                  dot3 = Vec3.Dot(Tan3)/(normevec3*normetan3);
                                }
                                else { dot3 = 0.; }
                                Tol = 1.e-12;
                                if (dot1 <= Tol && dot2 <=Tol && dot3 <= Tol) {
                                  if (Qualified1.IsUnqualified() || 
                                    (Qualified1.IsEnclosing() && Rsol >= R1 && dist <= Rsol)||
                                    (Qualified1.IsOutside() && dist >= Rsol) ||
                                    (Qualified1.IsEnclosed() && Rsol <= R1 && dist <= Rsol)) {
                                      Standard_Real Angle1 = Vec2.Angle(Tan2);
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&Angle1<=0.)||
                                        (Qualified2.IsOutside() && Angle1 >= 0) ||
                                        (Qualified2.IsEnclosed() && Angle1 <= 0.)) {
                                          Angle1 = Vec3.Angle(Tan3);
                                          if (Qualified3.IsUnqualified() || 
                                            (Qualified3.IsEnclosing()&&Angle1<=0.)||
                                            (Qualified3.IsOutside() && Angle1 >= 0) ||
                                            (Qualified3.IsEnclosed() && Angle1 <= 0.)) {
                                              qualifier1 = Qualified1.Qualifier();
                                              qualifier2 = Qualified2.Qualifier();
                                              qualifier3 = Qualified3.Qualifier();
                                              pararg1 = Ufirst(1);
                                              par1sol = 0.;
                                              pnttg1sol = point1;
                                              pararg2 = Ufirst(2);
                                              pnttg2sol = point2;
                                              par2sol = 0.;
                                              pararg3 = Ufirst(3);
                                              pnttg3sol = point3;
                                              par3sol = 0.;
                                              WellDone = Standard_True;
                                          }
                                      }
                                  }
                                }
                              }
                            }
}

Geom2dGcc_Circ2d3TanIter::
Geom2dGcc_Circ2d3TanIter (const GccEnt_QualifiedCirc& Qualified1 , 
                          const GccEnt_QualifiedCirc& Qualified2 , 
                          const Geom2dGcc_QCurve&    Qualified3 , 
                          const Standard_Real         Param1     ,
                          const Standard_Real         Param2     ,
                          const Standard_Real         Param3     ,
                          const Standard_Real         Tolerance     ) {

                            TheSame1 = Standard_False;
                            TheSame2 = Standard_False;
                            TheSame3 = Standard_False;
                            par1sol = 0.;
                            par2sol = 0.;
                            par3sol = 0.;
                            pararg1 = 0.;
                            pararg2 = 0.;
                            pararg3 = 0.;

                            Standard_Real Tol = Abs(Tolerance);
                            WellDone = Standard_False;
                            if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                              Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                              !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                              Qualified2.IsOutside() || Qualified2.IsUnqualified()) ||
                              !(Qualified3.IsEnclosed() || Qualified3.IsEnclosing() || 
                              Qualified3.IsOutside() || Qualified3.IsUnqualified())) {
                                throw GccEnt_BadQualifier();
                                return;
                            }
                            gp_Circ2d C1 = Qualified1.Qualified();
                            gp_Circ2d C2 = Qualified2.Qualified();
                            Geom2dAdaptor_Curve Cu3 = Qualified3.Qualified();
                            Geom2dGcc_FunctionTanCuCuCu Func(C1,C2,Cu3);
                            math_Vector Umin(1,3);
                            math_Vector Umax(1,3);
                            math_Vector Ufirst(1,3);
                            math_Vector tol(1,3);
                            Umin(1) = 0.;
                            Umin(2) = 0.;
                            Umin(3) = Geom2dGcc_CurveTool::FirstParameter(Cu3);
                            Umax(1) = 2*M_PI;
                            Umax(2) = 2*M_PI;
                            Umax(3) = Geom2dGcc_CurveTool::LastParameter(Cu3);
                            Ufirst(1) = Param1;
                            Ufirst(2) = Param2;
                            Ufirst(3) = Param3;
                            tol(1) = 2.e-15*M_PI;
                            tol(2) = 2.e-15*M_PI;
                            tol(3) = Geom2dGcc_CurveTool::EpsX(Cu3,Abs(Tolerance));   
                            math_FunctionSetRoot Root(Func, tol);
                            Root.Perform(Func, Ufirst, Umin, Umax);
                            if (Root.IsDone()) {
                              Root.Root(Ufirst);
                              Func.Value(Ufirst,Umin);
                              gp_Pnt2d centre1(C1.Location());
                              Standard_Real R1 = C1.Radius();
                              gp_Pnt2d point1(centre1.XY()+R1*gp_XY(Cos(Ufirst(1)),Sin(Ufirst(1))));
                              gp_Vec2d Tan1(gp_XY(-Sin(Ufirst(1)),Cos(Ufirst(1))));
                              gp_Pnt2d centre2(C2.Location());
                              Standard_Real R2 = C2.Radius();
                              gp_Pnt2d point2(centre2.XY()+R2*gp_XY(Cos(Ufirst(2)),Sin(Ufirst(2))));
                              gp_Vec2d Tan2(gp_XY(-Sin(Ufirst(2)),Cos(Ufirst(2))));
                              gp_Pnt2d point3;
                              gp_Vec2d Tan3;
                              Geom2dGcc_CurveTool::D1(Cu3,Ufirst(3),point3,Tan3);
                              GccAna_Circ2d3Tan circ(point1,point2,point3,Tol);
                              if (circ.IsDone()) {
                                cirsol = circ.ThisSolution(1);
                                gp_Pnt2d centre(cirsol.Location());
                                Standard_Real dist = centre1.Distance(centre);
                                Standard_Real Rsol = cirsol.Radius();
                                Standard_Real normetan1 = Tan1.Magnitude();
                                Standard_Real normetan2 = Tan2.Magnitude();
                                Standard_Real normetan3 = Tan3.Magnitude();
                                gp_Vec2d Vec1(point1,centre);
                                gp_Vec2d Vec2(point2,centre);
                                gp_Vec2d Vec3(point3,centre);
                                Standard_Real normevec1 = Vec1.Magnitude();
                                Standard_Real normevec2 = Vec2.Magnitude();
                                Standard_Real normevec3 = Vec3.Magnitude();
                                Standard_Real dot1,dot2,dot3;
                                if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                  dot1 = Vec1.Dot(Tan1)/(normevec1*normetan1);
                                }
                                else { dot1 = 0.; }
                                if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                  dot2 = Vec2.Dot(Tan2)/(normevec2*normetan2);
                                }
                                else { dot2 = 0.; }
                                if (normevec3 >= gp::Resolution() && normetan3 >= gp::Resolution()) {
                                  dot3 = Vec3.Dot(Tan3)/(normevec3*normetan3);
                                }
                                else { dot3 = 0.; }
                                Tol = 1.e-12;
                                if (dot1 <= Tol && dot2 <=Tol && dot3 <= Tol) {
                                  if (Qualified1.IsUnqualified() || 
                                    (Qualified1.IsEnclosing() && Rsol >= R1 && dist <= Rsol)||
                                    (Qualified1.IsOutside() && dist >= Rsol) ||
                                    (Qualified1.IsEnclosed() && Rsol <= R1 && dist <= Rsol)) {
                                      dist = centre2.Distance(centre);
                                      if (Qualified1.IsUnqualified() || 
                                        (Qualified1.IsEnclosing() && Rsol >= R2 && dist <= Rsol)||
                                        (Qualified1.IsOutside() && dist >= Rsol) ||
                                        (Qualified1.IsEnclosed() && Rsol <= R2 && dist <= Rsol)) {
                                          gp_Vec2d Vec(point3,centre);
                                          Standard_Real Angle1 = Vec.Angle(Tan3);
                                          if (Qualified3.IsUnqualified() || 
                                            (Qualified3.IsEnclosing()&&Angle1<=0.)||
                                            (Qualified3.IsOutside() && Angle1 >= 0) ||
                                            (Qualified3.IsEnclosed() && Angle1 <= 0.)) {
                                              qualifier1 = Qualified1.Qualifier();
                                              qualifier2 = Qualified2.Qualifier();
                                              qualifier3 = Qualified3.Qualifier();
                                              pararg1 = Ufirst(1);
                                              par1sol = 0.;
                                              pnttg1sol = point1;
                                              pararg2 = Ufirst(2);
                                              pnttg2sol = point2;
                                              par2sol = 0.;
                                              pararg3 = Ufirst(3);
                                              pnttg3sol = point3;
                                              par3sol = 0.;
                                              WellDone = Standard_True;
                                          }
                                      }
                                  }
                                }
                              }
                            }
}

Geom2dGcc_Circ2d3TanIter::
Geom2dGcc_Circ2d3TanIter (const GccEnt_QualifiedLin& Qualified1 , 
                          const Geom2dGcc_QCurve&   Qualified2 ,
                          const Geom2dGcc_QCurve&   Qualified3 , 
                          const Standard_Real        Param1     ,
                          const Standard_Real        Param2     ,
                          const Standard_Real        Param3     ,
                          const Standard_Real        Tolerance  ) {

                            TheSame1 = Standard_False;
                            TheSame2 = Standard_False;
                            TheSame3 = Standard_False;
                            par1sol = 0.;
                            par2sol = 0.;
                            par3sol = 0.;
                            pararg1 = 0.;
                            pararg2 = 0.;
                            pararg3 = 0.;

                            Standard_Real Tol = Abs(Tolerance);
                            WellDone = Standard_False;
                            if (!(Qualified1.IsEnclosed() ||
                              Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                              !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                              Qualified2.IsOutside() || Qualified2.IsUnqualified()) ||
                              !(Qualified3.IsEnclosed() || Qualified3.IsEnclosing() || 
                              Qualified3.IsOutside() || Qualified3.IsUnqualified())) {
                                throw GccEnt_BadQualifier();
                                return;
                            }
                            gp_Lin2d L1 = Qualified1.Qualified();
                            Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                            Geom2dAdaptor_Curve Cu3 = Qualified3.Qualified();
                            Geom2dGcc_FunctionTanCuCuCu Func(L1,Cu2,Cu3);
                            math_Vector Umin(1,3);
                            math_Vector Umax(1,3);
                            math_Vector Ufirst(1,3);
                            math_Vector tol(1,3);
                            Umin(1) = RealFirst();
                            Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                            Umin(3) = Geom2dGcc_CurveTool::FirstParameter(Cu3);
                            Umax(1) = RealLast();
                            Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                            Umax(3) = Geom2dGcc_CurveTool::LastParameter(Cu3);
                            Ufirst(1) = Param1;
                            Ufirst(2) = Param2;
                            Ufirst(3) = Param3;
                            tol(1) = 1.e-15;
                            tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                            tol(3) = Geom2dGcc_CurveTool::EpsX(Cu3,Abs(Tolerance));   
                            math_FunctionSetRoot Root(Func, tol);
                            Root.Perform(Func, Ufirst, Umin, Umax);
                            if (Root.IsDone()) {
                              Root.Root(Ufirst);
                              Func.Value(Ufirst,Umin);
                              gp_Pnt2d centre1(L1.Location());
                              gp_Pnt2d point1(centre1.XY()+Ufirst(1)*L1.Direction().XY());
                              gp_Pnt2d point2,point3;
                              gp_Vec2d Tan2,Tan3;
                              Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
                              Geom2dGcc_CurveTool::D1(Cu3,Ufirst(3),point3,Tan3);
                              GccAna_Circ2d3Tan circ(point1,point2,point3,Tol);
                              if (circ.IsDone()) {
                                cirsol = circ.ThisSolution(1);
                                gp_Pnt2d centre(cirsol.Location());

                                // creation vaariables intermediaires pour WNT
                                gp_XY dummy1 = centre.XY()-L1.Location().XY();
                                gp_XY dummy2 (-L1.Direction().Y(),L1.Direction().X());
                                Standard_Real pscal=dummy1.Dot(dummy2);

                                gp_Vec2d Tan1(L1.Direction().XY());
                                Standard_Real normetan1 = Tan1.Magnitude();
                                Standard_Real normetan2 = Tan2.Magnitude();
                                Standard_Real normetan3 = Tan3.Magnitude();
                                gp_Vec2d Vec1(point1,centre);
                                gp_Vec2d Vec2(point2,centre);
                                gp_Vec2d Vec3(point3,centre);
                                Standard_Real normevec1 = Vec1.Magnitude();
                                Standard_Real normevec2 = Vec2.Magnitude();
                                Standard_Real normevec3 = Vec3.Magnitude();
                                Standard_Real dot1,dot2,dot3;
                                if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                  dot1 = Vec1.Dot(Tan1)/(normevec1*normetan1);
                                }
                                else { dot1 = 0.; }
                                if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                  dot2 = Vec2.Dot(Tan2)/(normevec2*normetan2);
                                }
                                else { dot2 = 0.; }
                                if (normevec3 >= gp::Resolution() && normetan3 >= gp::Resolution()) {
                                  dot3 = Vec3.Dot(Tan3)/(normevec3*normetan3);
                                }
                                else { dot3 = 0.; }
                                Tol = 1.e-12;
                                if (dot1 <= Tol && dot2 <=Tol && dot3 <= Tol) {
                                  if (Qualified1.IsUnqualified() || 
                                    (Qualified1.IsOutside() && pscal <= 0.) ||
                                    (Qualified1.IsEnclosed() && pscal >= 0.)) {
                                      gp_Vec2d Vec(point2,centre);
                                      Standard_Real Angle1 = Vec.Angle(Tan2);
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&Angle1<=0.)||
                                        (Qualified2.IsOutside() && Angle1 >= 0) ||
                                        (Qualified2.IsEnclosed() && Angle1 <= 0.)) {
                                          Vec = gp_Vec2d(point3,centre);
                                          Angle1 = Vec.Angle(Tan3);
                                          if (Qualified3.IsUnqualified() || 
                                            (Qualified3.IsEnclosing()&&Angle1<=0.)||
                                            (Qualified3.IsOutside() && Angle1 >= 0) ||
                                            (Qualified3.IsEnclosed() && Angle1 <= 0.)) {
                                              qualifier1 = Qualified1.Qualifier();
                                              qualifier2 = Qualified2.Qualifier();
                                              qualifier3 = Qualified3.Qualifier();
                                              pararg1 = Ufirst(1);
                                              par1sol = 0.;
                                              pnttg1sol = point1;
                                              pararg2 = Ufirst(2);
                                              pnttg2sol = point2;
                                              par2sol = 0.;
                                              pararg3 = Ufirst(3);
                                              pnttg3sol = point3;
                                              par3sol = 0.;
                                              WellDone = Standard_True;
                                          }
                                      }
                                  }
                                }
                              }
                            }
}

Geom2dGcc_Circ2d3TanIter::
Geom2dGcc_Circ2d3TanIter (const GccEnt_QualifiedLin&  Qualified1 , 
                          const GccEnt_QualifiedLin&  Qualified2 , 
                          const Geom2dGcc_QCurve&    Qualified3 , 
                          const Standard_Real         Param1     ,
                          const Standard_Real         Param2     ,
                          const Standard_Real         Param3     ,
                          const Standard_Real         Tolerance  ){

                            TheSame1 = Standard_False;
                            TheSame2 = Standard_False;
                            TheSame3 = Standard_False;
                            par1sol = 0.;
                            par2sol = 0.;
                            par3sol = 0.;
                            pararg1 = 0.;
                            pararg2 = 0.;
                            pararg3 = 0.;

                            Standard_Real Tol = Abs(Tolerance);
                            WellDone = Standard_False;
                            if (!(Qualified1.IsEnclosed() || 
                              Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                              !(Qualified2.IsEnclosed() || 
                              Qualified2.IsOutside() || Qualified2.IsUnqualified()) ||
                              !(Qualified3.IsEnclosed() || Qualified3.IsEnclosing() || 
                              Qualified3.IsOutside() || Qualified3.IsUnqualified())) {
                                throw GccEnt_BadQualifier();
                                return;
                            }
                            gp_Lin2d L1 = Qualified1.Qualified();
                            gp_Lin2d L2 = Qualified2.Qualified();
                            Geom2dAdaptor_Curve Cu3 = Qualified3.Qualified();
                            Geom2dGcc_FunctionTanCuCuCu Func(L1,L2,Cu3);
                            math_Vector Umin(1,3);
                            math_Vector Umax(1,3);
                            math_Vector Ufirst(1,3);
                            math_Vector tol(1,3);
                            Umin(1) = RealFirst();
                            Umin(2) = RealFirst();
                            Umin(3) = Geom2dGcc_CurveTool::FirstParameter(Cu3);
                            Umax(1) = RealLast();
                            Umax(2) = RealLast();
                            Umax(3) = Geom2dGcc_CurveTool::LastParameter(Cu3);
                            Ufirst(1) = Param1;
                            Ufirst(2) = Param2;
                            Ufirst(3) = Param3;
                            tol(1) = 1.e-15;
                            tol(2) = 1.e-15;
                            tol(3) = Geom2dGcc_CurveTool::EpsX(Cu3,Abs(Tolerance));   
                            math_FunctionSetRoot Root(Func, tol);
                            Root.Perform(Func, Ufirst, Umin, Umax);
                            if (Root.IsDone()) {
                              Root.Root(Ufirst);
                              Func.Value(Ufirst,Umin);
                              gp_Pnt2d centre1(L1.Location());
                              gp_Pnt2d point1(centre1.XY()+Ufirst(1)*L1.Direction().XY());
                              gp_Pnt2d centre2(L2.Location());
                              gp_Pnt2d point2(centre2.XY()+Ufirst(2)*L2.Direction().XY());
                              gp_Pnt2d point3;
                              gp_Vec2d Tan3;
                              Geom2dGcc_CurveTool::D1(Cu3,Ufirst(3),point3,Tan3);
                              GccAna_Circ2d3Tan circ(point1,point2,point3,Tol);
                              if (circ.IsDone()) {
                                cirsol = circ.ThisSolution(1);
                                gp_Pnt2d centre(cirsol.Location());
                                Standard_Real pscal=centre.XY().Dot(gp_XY(-L1.Direction().Y(),
                                  L1.Direction().X()));
                                if (Qualified1.IsUnqualified() || 
                                  (Qualified1.IsOutside() && pscal <= 0.) ||
                                  (Qualified1.IsEnclosed() && pscal >= 0.)) {
                                    gp_Vec2d Tan1(L1.Direction().XY());
                                    gp_Vec2d Tan2(L2.Direction().XY());
                                    Standard_Real normetan1 = Tan1.Magnitude();
                                    Standard_Real normetan2 = Tan2.Magnitude();
                                    Standard_Real normetan3 = Tan3.Magnitude();
                                    gp_Vec2d Vec1(point1,centre);
                                    gp_Vec2d Vec2(point2,centre);
                                    gp_Vec2d Vec3(point3,centre);
                                    Standard_Real normevec1 = Vec1.Magnitude();
                                    Standard_Real normevec2 = Vec2.Magnitude();
                                    Standard_Real normevec3 = Vec3.Magnitude();
                                    Standard_Real dot1,dot2,dot3;
                                    if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                      dot1 = Vec1.Dot(Tan1)/(normevec1*normetan1);
                                    }
                                    else { dot1 = 0.; }
                                    if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                      dot2 = Vec2.Dot(Tan2)/(normevec2*normetan2);
                                    }
                                    else { dot2 = 0.; }
                                    if (normevec3 >= gp::Resolution() && normetan3 >= gp::Resolution()) {
                                      dot3 = Vec3.Dot(Tan3)/(normevec3*normetan3);
                                    }
                                    else { dot3 = 0.; }
                                    Tol = 1.e-12;
                                    if (dot1 <= Tol && dot2 <=Tol && dot3 <= Tol) {
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsOutside() && pscal <= 0.) ||
                                        (Qualified2.IsEnclosed() && pscal >= 0.)) {
                                          Standard_Real Angle1 = Vec3.Angle(Tan3);
                                          if (Qualified3.IsUnqualified() || 
                                            (Qualified3.IsEnclosing()&&Angle1<=0.)||
                                            (Qualified3.IsOutside() && Angle1 >= 0) ||
                                            (Qualified3.IsEnclosed() && Angle1 <= 0.)) {
                                              qualifier1 = Qualified1.Qualifier();
                                              qualifier2 = Qualified2.Qualifier();
                                              qualifier3 = Qualified3.Qualifier();
                                              pararg1 = Ufirst(1);
                                              par1sol = 0.;
                                              pnttg1sol = point1;
                                              pararg2 = Ufirst(2);
                                              pnttg2sol = point2;
                                              par2sol = 0.;
                                              pararg3 = Ufirst(3);
                                              pnttg3sol = point3;
                                              par3sol = 0.;
                                              WellDone = Standard_True;
                                          }
                                      }
                                    }
                                }
                              }
                            }
}

Geom2dGcc_Circ2d3TanIter::
Geom2dGcc_Circ2d3TanIter (const Geom2dGcc_QCurve& Qualified1 ,
                          const Geom2dGcc_QCurve& Qualified2 , 
                          const gp_Pnt2d&          Point3     ,
                          const Standard_Real      Param1     ,
                          const Standard_Real      Param2     ,
                          const Standard_Real      Tolerance  ) {

                            TheSame1 = Standard_False;
                            TheSame2 = Standard_False;
                            TheSame3 = Standard_False;
                            par1sol = 0.;
                            par2sol = 0.;
                            par3sol = 0.;
                            pararg1 = 0.;
                            pararg2 = 0.;
                            pararg3 = 0.;

                            Standard_Real Tol = Abs(Tolerance);
                            WellDone = Standard_False;
                            if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                              Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                              !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                              Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                                throw GccEnt_BadQualifier();
                                return;
                            }
                            gp_Circ2d C1(gp_Ax2d(Point3,gp_Dir2d(1.,0.)),0.);
                            Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
                            Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                            Geom2dGcc_FunctionTanCuCuCu Func(C1,Cu1,Cu2);
                            math_Vector Umin(1,3);
                            math_Vector Umax(1,3);
                            math_Vector Ufirst(1,3);
                            math_Vector tol(1,3);
                            Umin(1) = 0.;
                            Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu1);
                            Umin(3) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                            Umax(1) = 2*M_PI;
                            Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu1);
                            Umax(3) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                            Ufirst(1) = M_PI;
                            Ufirst(2) = Param1;
                            Ufirst(3) = Param2;
                            tol(1) = 2.e-15*M_PI;
                            tol(2) = Geom2dGcc_CurveTool::EpsX(Cu1,Abs(Tolerance));
                            tol(3) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                            math_FunctionSetRoot Root(Func, tol);
                            Root.Perform(Func, Ufirst, Umin, Umax);
                            if (Root.IsDone()) {
                              Root.Root(Ufirst);
                              Func.Value(Ufirst,Umin);
                              gp_Pnt2d point1,point2;
                              //     gp_Vec2d Tan1,Tan2,Nor1,Nor2;
                              gp_Vec2d Tan1,Tan2;
                              Geom2dGcc_CurveTool::D1(Cu1,Ufirst(2),point1,Tan1);
                              Geom2dGcc_CurveTool::D1(Cu2,Ufirst(3),point2,Tan2);
                              GccAna_Circ2d3Tan circ(Point3,point1,point2,Tol);
                              if (circ.IsDone()) {
                                cirsol = circ.ThisSolution(1);
                                gp_Pnt2d centre(cirsol.Location());
                                gp_Vec2d Tan3(-Sin(Ufirst(1)),Cos(Ufirst(1)));
                                Standard_Real normetan1 = Tan1.Magnitude();
                                Standard_Real normetan2 = Tan2.Magnitude();
                                Standard_Real normetan3 = Tan3.Magnitude();
                                gp_Vec2d Vec1(point1,centre);
                                gp_Vec2d Vec2(point2,centre);
                                gp_Vec2d Vec3(Point3,centre);
                                Standard_Real normevec1 = Vec1.Magnitude();
                                Standard_Real normevec2 = Vec2.Magnitude();
                                Standard_Real normevec3 = Vec3.Magnitude();
                                Standard_Real dot1,dot2,dot3;
                                if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                  dot1 = Vec1.Dot(Tan1)/(normevec1*normetan1);
                                }
                                else { dot1 = 0.; }
                                if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                  dot2 = Vec2.Dot(Tan2)/(normevec2*normetan2);
                                }
                                else { dot2 = 0.; }
                                if (normevec3 >= gp::Resolution() && normetan3 >= gp::Resolution()) {
                                  dot3 = Vec3.Dot(Tan3)/(normevec3*normetan3);
                                }
                                else { dot3 = 0.; }
                                Tol = 1.e-12;
                                if (dot1 <= Tol && dot2 <=Tol && dot3 <= Tol) {
                                  Standard_Real Angle1 = Vec1.Angle(Tan1);
                                  if (Qualified1.IsUnqualified()||
                                    (Qualified1.IsEnclosing()&&Angle1<=0.)||
                                    (Qualified1.IsOutside() && Angle1 >= 0) ||
                                    (Qualified1.IsEnclosed() && Angle1 <= 0.)) {
                                      Angle1 = Vec2.Angle(Tan2);
                                      if (Qualified1.IsUnqualified() ||
                                        (Qualified1.IsEnclosing()&&Angle1<=0.)||
                                        (Qualified1.IsOutside() && Angle1 >= 0) ||
                                        (Qualified1.IsEnclosed() && Angle1 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          qualifier3 = GccEnt_noqualifier;
                                          pararg1 = Ufirst(2);
                                          par1sol = 0.;
                                          pnttg1sol = point1;
                                          pararg2 = Ufirst(3);
                                          pnttg2sol = point2;
                                          par2sol = 0.;
                                          pararg3 = 0.;
                                          pnttg3sol = Point3;
                                          par3sol = 0.;
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
                            }
}

Geom2dGcc_Circ2d3TanIter::
Geom2dGcc_Circ2d3TanIter (const Geom2dGcc_QCurve& Qualified1 ,
                          const gp_Pnt2d&          Point2     ,
                          const gp_Pnt2d&          Point3     ,
                          const Standard_Real      Param1     ,
                          const Standard_Real      Tolerance     ) {

                            TheSame1 = Standard_False;
                            TheSame2 = Standard_False;
                            TheSame3 = Standard_False;
                            par1sol = 0.;
                            par2sol = 0.;
                            par3sol = 0.;
                            pararg1 = 0.;
                            pararg2 = 0.;
                            pararg3 = 0.;

                            Standard_Real Tol = Abs(Tolerance);
                            WellDone = Standard_False;
                            if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                              Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
                                throw GccEnt_BadQualifier();
                                return;
                            }
                            gp_Dir2d dirx(1.,0.);
                            gp_Circ2d C1(gp_Ax2d(Point2,dirx),0.);
                            gp_Circ2d C2(gp_Ax2d(Point3,dirx),0.);
                            Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
                            Geom2dGcc_FunctionTanCuCuCu Func(C1,C2,Cu1);
                            math_Vector Umin(1,3);
                            math_Vector Umax(1,3);
                            math_Vector Ufirst(1,3);
                            math_Vector tol(1,3);
                            Umin(1) = 0.;
                            Umin(2) = 0.;
                            Umin(3) = Geom2dGcc_CurveTool::FirstParameter(Cu1);
                            Umax(1) = 2*M_PI;
                            Umax(2) = 2*M_PI;
                            Umax(3) = Geom2dGcc_CurveTool::LastParameter(Cu1);
                            Ufirst(1) = M_PI;
                            Ufirst(2) = M_PI;
                            Ufirst(3) = Param1;
                            tol(1) = 2.e-15*M_PI;
                            tol(2) = 2.e-15*M_PI;
                            tol(3) = Geom2dGcc_CurveTool::EpsX(Cu1,Abs(Tolerance));
                            math_FunctionSetRoot Root(Func, tol);
                            Root.Perform(Func, Ufirst, Umin, Umax);
                            if (Root.IsDone()) {
                              Root.Root(Ufirst);
                              Func.Value(Ufirst,Umin);
                              gp_Pnt2d point3;
                              //     gp_Vec2d Tan3,Nor3;
                              gp_Vec2d Tan3;
                              Geom2dGcc_CurveTool::D1(Cu1,Ufirst(3),point3,Tan3);
                              GccAna_Circ2d3Tan circ(Point2,Point3,point3,Tol);
                              if (circ.IsDone()) { 
                                cirsol = circ.ThisSolution(1);
                                gp_Pnt2d centre(cirsol.Location());
                                gp_Vec2d Tan2(-Sin(Ufirst(2)),Cos(Ufirst(2)));
                                gp_Vec2d Tan1(-Sin(Ufirst(1)),Cos(Ufirst(1)));
                                Standard_Real normetan1 = Tan1.Magnitude();
                                Standard_Real normetan2 = Tan2.Magnitude();
                                Standard_Real normetan3 = Tan3.Magnitude();
                                gp_Vec2d Vec1(Point2,centre);
                                gp_Vec2d Vec2(Point3,centre);
                                gp_Vec2d Vec3(point3,centre);
                                Standard_Real normevec1 = Vec1.Magnitude();
                                Standard_Real normevec2 = Vec2.Magnitude();
                                Standard_Real normevec3 = Vec3.Magnitude();
                                Standard_Real dot1,dot2,dot3;
                                if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                  dot1 = Vec1.Dot(Tan1)/(normevec1*normetan1);
                                }
                                else { dot1 = 0.; }
                                if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                  dot2 = Vec2.Dot(Tan2)/(normevec2*normetan2);
                                }
                                else { dot2 = 0.; }
                                if (normevec3 >= gp::Resolution() && normetan3 >= gp::Resolution()) {
                                  dot3 = Vec3.Dot(Tan3)/(normevec3*normetan3);
                                }
                                else { dot3 = 0.; }
                                Tol = 1.e-12;
                                if (dot1 <= Tol && dot2 <=Tol && dot3 <= Tol) {
                                  Standard_Real Angle1 = Vec1.Angle(Tan1);
                                  if (Qualified1.IsUnqualified() ||
                                    (Qualified1.IsEnclosing()&&Angle1<=0.)||
                                    (Qualified1.IsOutside() && Angle1 >= 0) ||
                                    (Qualified1.IsEnclosed() && Angle1 <= 0.)) {
                                      qualifier1 = Qualified1.Qualifier();
                                      qualifier2 = GccEnt_noqualifier;
                                      qualifier3 = GccEnt_noqualifier;
                                      pararg1 = Ufirst(3);
                                      par1sol = 0.;
                                      pnttg1sol = point3;
                                      pararg2 = 0.;
                                      pnttg2sol = Point2;
                                      par2sol = 0.;
                                      pararg3 = 0.;
                                      pnttg3sol = Point3;
                                      par3sol = 0.;
                                      WellDone = Standard_True;
                                  }
                                }
                              }
                            }
}

Geom2dGcc_Circ2d3TanIter::
Geom2dGcc_Circ2d3TanIter (const GccEnt_QualifiedLin&  Qualified1 , 
                          const Geom2dGcc_QCurve&    Qualified2 ,
                          const gp_Pnt2d&             Point3     ,
                          const Standard_Real         Param1     ,
                          const Standard_Real         Param2     ,
                          const Standard_Real         Tolerance  ) {

                            TheSame1 = Standard_False;
                            TheSame2 = Standard_False;
                            TheSame3 = Standard_False;
                            par1sol = 0.;
                            par2sol = 0.;
                            par3sol = 0.;
                            pararg1 = 0.;
                            pararg2 = 0.;
                            pararg3 = 0.;

                            Standard_Real Tol = Abs(Tolerance);
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
                            gp_Circ2d C3(gp_Ax2d(Point3,dirx),0.);
                            Geom2dGcc_FunctionTanCuCuCu Func(C3,L1,Cu2);
                            math_Vector Umin(1,3);
                            math_Vector Umax(1,3);
                            math_Vector Ufirst(1,3);
                            math_Vector tol(1,3);
                            Umin(2) = RealFirst();
                            Umin(3) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                            Umin(1) = 0.;
                            Umax(2) = RealLast();
                            Umax(3) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                            Umax(1) = 2*M_PI;
                            Ufirst(2) = Param1;
                            Ufirst(3) = Param2;
                            Ufirst(1) = M_PI;
                            tol(1) = 2.e-15;
                            tol(2) = 1.e-15;
                            tol(3) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                            math_FunctionSetRoot Root(Func, tol);
                            Root.Perform(Func, Ufirst, Umin, Umax);
                            if (Root.IsDone()) {
                              Root.Root(Ufirst);
                              Func.Value(Ufirst,Umin);
                              gp_Pnt2d centre1(L1.Location());
                              gp_Pnt2d point1(centre1.XY()+Ufirst(2)*L1.Direction().XY());
                              gp_Pnt2d point2;
                              gp_Vec2d Tan2;
                              Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
                              GccAna_Circ2d3Tan circ(point1,point2,Point3,Tol);
                              if (circ.IsDone()) {
                                cirsol = circ.ThisSolution(1);
                                gp_Pnt2d centre(cirsol.Location());
                                Standard_Real pscal=centre.XY().Dot(gp_XY(-L1.Direction().Y(),
                                  L1.Direction().X()));
                                gp_Vec2d Tan1(L1.Direction().XY());
                                gp_Vec2d Tan3(-Sin(Ufirst(1)),Cos(Ufirst(1)));
                                Standard_Real normetan1 = Tan1.Magnitude();
                                Standard_Real normetan2 = Tan2.Magnitude();
                                Standard_Real normetan3 = Tan3.Magnitude();
                                gp_Vec2d Vec1(point1,centre);
                                gp_Vec2d Vec2(point2,centre);
                                gp_Vec2d Vec3(Point3,centre);
                                Standard_Real normevec1 = Vec1.Magnitude();
                                Standard_Real normevec2 = Vec2.Magnitude();
                                Standard_Real normevec3 = Vec3.Magnitude();
                                Standard_Real dot1,dot2,dot3;
                                if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                  dot1 = Vec1.Dot(Tan1)/(normevec1*normetan1);
                                }
                                else { dot1 = 0.; }
                                if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                  dot2 = Vec2.Dot(Tan2)/(normevec2*normetan2);
                                }
                                else { dot2 = 0.; }
                                if (normevec3 >= gp::Resolution() && normetan3 >= gp::Resolution()) {
                                  dot3 = Vec3.Dot(Tan3)/(normevec3*normetan3);
                                }
                                else { dot3 = 0.; }
                                Tol = 1.e-12;
                                if (dot1 <= Tol && dot2 <=Tol && dot3 <= Tol) {
                                  if (Qualified1.IsUnqualified() || 
                                    (Qualified1.IsOutside() && pscal <= 0.) ||
                                    (Qualified1.IsEnclosed() && pscal >= 0.)) {
                                      Standard_Real Angle1 = Vec2.Angle(Tan2);
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&Angle1<=0.)||
                                        (Qualified2.IsOutside() && Angle1 >= 0) ||
                                        (Qualified2.IsEnclosed() && Angle1 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          qualifier3 = GccEnt_noqualifier;
                                          pararg1 = Ufirst(2);
                                          par1sol = 0.;
                                          pnttg1sol = point1;
                                          pararg2 = Ufirst(3);
                                          pnttg2sol = point2;
                                          par2sol = 0.;
                                          pararg3 = 0.;
                                          pnttg3sol = Point3;
                                          par3sol = 0.;
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
                            }
}

Geom2dGcc_Circ2d3TanIter::
Geom2dGcc_Circ2d3TanIter (const GccEnt_QualifiedCirc& Qualified1 , 
                          const GccEnt_QualifiedLin&  Qualified2 , 
                          const Geom2dGcc_QCurve&     Qualified3 , 
                          const Standard_Real         Param1     ,
                          const Standard_Real         Param2     ,
                          const Standard_Real         Param3     ,
                          const Standard_Real         Tolerance  ) {

                            TheSame1 = Standard_False;
                            TheSame2 = Standard_False;
                            TheSame3 = Standard_False;
                            par1sol = 0.;
                            par2sol = 0.;
                            par3sol = 0.;
                            pararg1 = 0.;
                            pararg2 = 0.;
                            pararg3 = 0.;

                            Standard_Real Tol = Abs(Tolerance);
                            WellDone = Standard_False;
                            if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                              Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                              !(Qualified2.IsEnclosed() ||
                              Qualified2.IsOutside() || Qualified2.IsUnqualified()) ||
                              !(Qualified3.IsEnclosed() || Qualified3.IsEnclosing() || 
                              Qualified3.IsOutside() || Qualified3.IsUnqualified())) {
                                throw GccEnt_BadQualifier();
                                return;
                            }
                            gp_Circ2d C1 = Qualified1.Qualified();
                            gp_Lin2d L2 = Qualified2.Qualified();
                            Geom2dAdaptor_Curve Cu3 = Qualified3.Qualified();
                            Geom2dGcc_FunctionTanCuCuCu Func(C1,L2,Cu3);
                            math_Vector Umin(1,3);
                            math_Vector Umax(1,3);
                            math_Vector Ufirst(1,3);
                            math_Vector tol(1,3);
                            Umin(1) = 0.;
                            Umin(2) = RealFirst();
                            Umin(3) = Geom2dGcc_CurveTool::FirstParameter(Cu3);
                            Umax(1) = 2*M_PI;
                            Umax(2) = RealLast();
                            Umax(3) = Geom2dGcc_CurveTool::LastParameter(Cu3);
                            Ufirst(1) = Param1;
                            Ufirst(2) = Param2;
                            Ufirst(3) = Param3;
                            tol(1) = 2.e-15*M_PI;
                            tol(2) = 1.e-15;
                            tol(3) = Geom2dGcc_CurveTool::EpsX(Cu3,Abs(Tolerance));
                            math_FunctionSetRoot Root(Func, tol);
                            Root.Perform(Func, Ufirst, Umin, Umax);
                            if (Root.IsDone()) {
                              Func.Value(Ufirst,Umin);
                              Root.Root(Ufirst);
                              gp_Pnt2d centre1(C1.Location());
                              Standard_Real R1 = C1.Radius();
                              gp_Pnt2d point1(centre1.XY()+R1*gp_XY(Cos(Ufirst(1)),Sin(Ufirst(1))));
                              gp_Pnt2d centre2(L2.Location());
                              gp_Pnt2d point2(centre2.XY()+Ufirst(2)*L2.Direction().XY());
                              gp_Pnt2d point3;
                              gp_Vec2d Tan3;
                              Geom2dGcc_CurveTool::D1(Cu3,Ufirst(3),point3,Tan3);
                              GccAna_Circ2d3Tan circ(point1,point2,point3,Tol);
                              if (circ.IsDone()) {
                                cirsol = circ.ThisSolution(1);
                                gp_Pnt2d centre(cirsol.Location());
                                gp_Vec2d Tan1(-Sin(Ufirst(1)),Cos(Ufirst(1)));
                                gp_Vec2d Tan2(L2.Direction().XY());
                                Standard_Real normetan1 = Tan1.Magnitude();
                                Standard_Real normetan2 = Tan2.Magnitude();
                                Standard_Real normetan3 = Tan3.Magnitude();
                                gp_Vec2d Vec1(point1,centre);
                                gp_Vec2d Vec2(point2,centre);
                                gp_Vec2d Vec3(point3,centre);
                                Standard_Real normevec1 = Vec1.Magnitude();
                                Standard_Real normevec2 = Vec2.Magnitude();
                                Standard_Real normevec3 = Vec3.Magnitude();
                                Standard_Real dot1,dot2,dot3;
                                if (normevec1 >= gp::Resolution() && normetan1 >= gp::Resolution()) {
                                  dot1 = Vec1.Dot(Tan1)/(normevec1*normetan1);
                                }
                                else { dot1 = 0.; }
                                if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                  dot2 = Vec2.Dot(Tan2)/(normevec2*normetan2);
                                }
                                else { dot2 = 0.; }
                                if (normevec3 >= gp::Resolution() && normetan3 >= gp::Resolution()) {
                                  dot3 = Vec3.Dot(Tan3)/(normevec3*normetan3);
                                }
                                else { dot3 = 0.; }
                                Tol = 1.e-12;
                                if (dot1 <= Tol && dot2 <=Tol && dot3 <= Tol) {
                                  Standard_Real dist = centre1.Distance(centre);
                                  Standard_Real Rsol = cirsol.Radius();
                                  if (Qualified1.IsUnqualified() || 
                                    (Qualified1.IsEnclosing() && Rsol >= R1 && dist <= Rsol)||
                                    (Qualified1.IsOutside() && dist >= Rsol) ||
                                    (Qualified1.IsEnclosed() && Rsol <= R1 && dist <= Rsol)) {
                                      Standard_Real pscal=centre.XY().Dot(gp_XY(-L2.Direction().Y(),
                                        L2.Direction().X()));
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsOutside() && pscal <= 0.) ||
                                        (Qualified2.IsEnclosed() && pscal >= 0.)) {
                                          Standard_Real Angle1 = Vec3.Angle(Tan3);
                                          if (Qualified3.IsUnqualified() || 
                                            (Qualified3.IsEnclosing()&&Angle1<=0.)||
                                            (Qualified3.IsOutside() && Angle1 >= 0) ||
                                            (Qualified3.IsEnclosed() && Angle1 <= 0.)) {
                                              qualifier1 = Qualified1.Qualifier();
                                              qualifier2 = Qualified2.Qualifier();
                                              qualifier3 = Qualified3.Qualifier();
                                              pararg1 = Ufirst(1);
                                              par1sol = 0.;
                                              pnttg1sol = point1;
                                              pararg2 = Ufirst(2);
                                              pnttg2sol = point2;
                                              par2sol = 0.;
                                              pararg3 = Ufirst(3);
                                              pnttg3sol = point3;
                                              par3sol = 0.;
                                              WellDone = Standard_True;
                                          }
                                      }
                                  }
                                }
                              }
                            }
}

Geom2dGcc_Circ2d3TanIter::
Geom2dGcc_Circ2d3TanIter (const GccEnt_QualifiedCirc& Qualified1 , 
                          const Geom2dGcc_QCurve&     Qualified2 ,
                          const gp_Pnt2d&             Point3     ,
                          const Standard_Real         Param1     ,
                          const Standard_Real         Param2     ,
                          const Standard_Real         Tolerance  ) {

                            TheSame1 = Standard_False;
                            TheSame2 = Standard_False;
                            TheSame3 = Standard_False;
                            par1sol = 0.;
                            par2sol = 0.;
                            par3sol = 0.;
                            pararg1 = 0.;
                            pararg2 = 0.;
                            pararg3 = 0.;

                            Standard_Real Tol = Abs(Tolerance);
                            WellDone = Standard_False;
                            if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                              Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                              !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                              Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                                throw GccEnt_BadQualifier();
                                return;
                            }
                            gp_Circ2d C1 = Qualified1.Qualified();
                            Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                            gp_Dir2d dirx(1.,0.);
                            gp_Circ2d C3(gp_Ax2d(Point3,dirx),0.);
                            Geom2dGcc_FunctionTanCuCuCu Func(C1,C3,Cu2);
                            math_Vector Umin(1,3);
                            math_Vector Umax(1,3);
                            math_Vector Ufirst(1,3);
                            math_Vector tol(1,3);
                            Umin(1) = 0.;
                            Umin(3) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                            Umin(2) = 0.;
                            Umax(1) = 2*M_PI;
                            Umax(3) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                            Umax(2) = 2*M_PI;
                            Ufirst(1) = Param1;
                            Ufirst(2) = M_PI;
                            Ufirst(3) = Param2;
                            tol(1) = 2.e-15*M_PI;
                            tol(2) = 2.e-15*M_PI;
                            tol(3) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolerance));
                            math_FunctionSetRoot Root(Func, tol);
                            Root.Perform(Func, Ufirst, Umin, Umax);
                            if (Root.IsDone()) {
                              Root.Root(Ufirst);
                              Func.Value(Ufirst,Umin);
                              gp_Pnt2d centre1(C1.Location());
                              Standard_Real R1 = C1.Radius();
                              gp_Pnt2d point1(centre1.XY()+R1*gp_XY(Cos(Ufirst(1)),Sin(Ufirst(1))));
                              gp_Pnt2d point2;
                              //     gp_Vec2d Tan2,Nor2;
                              gp_Vec2d Tan2;
                              Geom2dGcc_CurveTool::D1(Cu2,Ufirst(2),point2,Tan2);
                              GccAna_Circ2d3Tan circ(point1,point2,Point3,Tol);
                              if (circ.IsDone()) {
                                cirsol = circ.ThisSolution(1);
                                gp_Pnt2d centre(cirsol.Location());
                                gp_Vec2d Tan1(-Sin(Ufirst(1)),Cos(Ufirst(1)));
                                gp_Vec2d Tan3(-Sin(Ufirst(3)),Cos(Ufirst(3)));
                                Standard_Real normetan2 = Tan2.Magnitude();
                                gp_Vec2d Vec1(point1,centre);
                                gp_Vec2d Vec2(point2,centre);
                                gp_Vec2d Vec3(Point3,centre);
                                Standard_Real normevec1 = Vec1.Magnitude();
                                Standard_Real normevec2 = Vec2.Magnitude();
                                Standard_Real normevec3 = Vec3.Magnitude();
                                Standard_Real dot1,dot2,dot3;
                                if (normevec1 >= gp::Resolution()) {
                                  dot1 = Vec1.Dot(Tan1)/(normevec1);
                                }
                                else { dot1 = 0.; }
                                if (normevec2 >= gp::Resolution() && normetan2 >= gp::Resolution()) {
                                  dot2 = Vec2.Dot(Tan2)/(normevec2*normetan2);
                                }
                                else { dot2 = 0.; }
                                if (normevec3 >= gp::Resolution()) {
                                  dot3 = Vec3.Dot(Tan3)/(normevec3);
                                }
                                else { dot3 = 0.; }
                                Tol = 1.e-12;
                                if (dot1 <= Tol && dot2 <=Tol && dot3 <= Tol) {
                                  Standard_Real dist = centre1.Distance(centre);
                                  Standard_Real Rsol = cirsol.Radius();
                                  if (Qualified1.IsUnqualified() || 
                                    (Qualified1.IsEnclosing() && Rsol >= R1 && dist <= Rsol)||
                                    (Qualified1.IsOutside() && dist >= Rsol) ||
                                    (Qualified1.IsEnclosed() && Rsol <= R1 && dist <= Rsol)) {
                                      Standard_Real Angle1 = Vec2.Angle(Tan2);
                                      if (Qualified2.IsUnqualified() || 
                                        (Qualified2.IsEnclosing()&&Angle1<=0.)||
                                        (Qualified2.IsOutside() && Angle1 >= 0) ||
                                        (Qualified2.IsEnclosed() && Angle1 <= 0.)) {
                                          qualifier1 = Qualified1.Qualifier();
                                          qualifier2 = Qualified2.Qualifier();
                                          qualifier3 = GccEnt_noqualifier;
                                          pararg1 = Ufirst(1);
                                          par1sol = 0.;
                                          pnttg1sol = point1;
                                          pararg2 = Ufirst(2);
                                          pnttg2sol = point2;
                                          par2sol = 0.;
                                          pararg3 = 0.;
                                          pnttg3sol = Point3;
                                          par3sol = 0.;
                                          WellDone = Standard_True;
                                      }
                                  }
                                }
                              }
                            }
}

Standard_Boolean Geom2dGcc_Circ2d3TanIter::
IsDone () const{ return WellDone; }

gp_Circ2d Geom2dGcc_Circ2d3TanIter::
ThisSolution () const{ return cirsol; }

void Geom2dGcc_Circ2d3TanIter:: 
WhichQualifier (GccEnt_Position& Qualif1  ,
                GccEnt_Position& Qualif2  ,
                GccEnt_Position& Qualif3  ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else {
    Qualif1 = qualifier1;
    Qualif2 = qualifier2;
    Qualif3 = qualifier3;
  }
}

void Geom2dGcc_Circ2d3TanIter:: 
Tangency1 (Standard_Real&      ParSol ,
           Standard_Real&      ParArg ,
           gp_Pnt2d&           PntSol ) const{
             if (!WellDone) { throw StdFail_NotDone(); }
             else {
               if (TheSame1 == 0) {
                 ParSol = par1sol;
                 ParArg = pararg1;
                 PntSol = pnttg1sol;
               }
               else { throw StdFail_NotDone(); }
             }
}

void Geom2dGcc_Circ2d3TanIter:: 
Tangency2 (Standard_Real&      ParSol         ,
           Standard_Real&      ParArg         ,
           gp_Pnt2d&  PntSol         ) const{
             if (!WellDone) { throw StdFail_NotDone(); }
             else {
               ParSol = par2sol;
               ParArg = pararg2;
               PntSol = pnttg2sol;
             }
}

void Geom2dGcc_Circ2d3TanIter:: 
Tangency3 (Standard_Real&      ParSol         ,
           Standard_Real&      ParArg         ,
           gp_Pnt2d&  PntSol         ) const{
             if (!WellDone) { throw StdFail_NotDone(); }
             else {
               ParSol = par3sol;
               ParArg = pararg3;
               PntSol = pnttg3sol;
             }
}

Standard_Boolean Geom2dGcc_Circ2d3TanIter::
IsTheSame1 () const
{
  if (!WellDone) throw StdFail_NotDone();

  if (TheSame1 == 0) 
    return Standard_False;

  return Standard_True;
}


Standard_Boolean Geom2dGcc_Circ2d3TanIter::
IsTheSame2 () const
{
  if (!WellDone) throw StdFail_NotDone();

  if (TheSame3 == 0) 
    return Standard_False;

  return Standard_True;

}

Standard_Boolean Geom2dGcc_Circ2d3TanIter::
IsTheSame3 () const
{
  if (!WellDone) throw StdFail_NotDone();

  return Standard_True;
}
