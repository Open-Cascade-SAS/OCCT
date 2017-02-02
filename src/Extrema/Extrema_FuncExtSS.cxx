// Created on: 1996-01-09
// Created by: Laurent PAINNOT
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


#include <Adaptor3d_Surface.hxx>
#include <Extrema_FuncExtSS.hxx>
#include <Extrema_POnSurf.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>

/*----------------------------------------------------------------------------
 Si on note Du1s et Dv1s, les derivees en u1 et v1, les 2 fonctions a annuler sont:
 Si on note Du2s et Dv2s, les derivees en u2 et v2, les 2 fonctions a annuler sont:
 { F1(u1,v1,u2,v2) = (S1(u1,v1)-S2(u2,v2)).Du1s1(u1,v1) }
 { F2(u1,v1,u2,v2) = (S1(u1,v1)-S2(u2,v2)).Dv1s1(u1,v1) }
 { F3(u1,v1,u2,v2) = (S1(u1,v1)-S2(u2,v2)).Du2s2(u2,v2) }
 { F4(u1,v1,u2,v2) = (S1(u1,v1)-S2(u2,v2)).Dv2s2(u2,v2) }
 { du1f1(u1,v1,u2,v2) = Du1s1(u1,v1).Du1s1(u1,v1)+(S1(u1,v1)-S2(u2,v2)).Du1u1s1(u1,v1) 
                      = ||Du1s1(u1,v1)||**2      +(S1(u1,v1)-S2(u2,v2)).Du1u1s1(u1,v1) }
 { dv1f1(u1,v1,u2,v2) = Dv1s1(u1,v1).Du1s1(u1,v1)+(S1(u1,v1)-S2(u2,v2)).Du1v1s1(u1,v1) }
 { du2f1(u1,v1,u2,v2) = -Du2s2(u2,v2).Du1s1(u1,v1) }
 { dv2f1(u1,v1,u2,v2) = -Dv2s2(u2,v2).Du1s1(u1,v1) }
 { du1f2(u1,v1,u2,v2) = Du1s1(u1,v1).Dv1s1(u1,v1)+(S1(u1,v1)-S2(u2,v2)).Du1v1s1(u1,v1) }
 { dv1f2(u1,v1,u2,v2) = Dv1s1(u1,v1).Dv1s1(u1,v1)+(S1(u1,v1)-S2(u2,v2)).Dv1v1s1(u1,v1) 
                      = ||Dv1s1(u1,v1)||**2      +(S1(u1,v1)-S2(u2,v2)).Dv1v1s1(u1,v1) }
 { du2f2(u1,v1,u2,v2) = -Du2s2(u2,v2).Dv1s1(u1,v1) }
 { dv2f2(u1,v1,u2,v2) = -Dv2s2(u2,v2).Dv1s1(u1,v1) }
 { du1f3(u1,v1,u2,v2) = Du1s1(u1,v1).Du2s2(u2,v2) }
 { dv1f3(u1,v1,u2,v2) = Dv1s1(u1,v1).Du2s2(u2,v2) }
 { du2f3(u1,v1,u2,v2) = -Du2s2(u2,v2).Du2s2(u2,v2)+(S1(u1,v1)-S2(u2,v2)).Du2u2s2(u2,v2) 
                      = -||Du2s2(u2,v2)||**2      +(S1(u1,v1)-S2(u2,v2)).Du2u2s2(u2,v2) }
 { dv2f3(u1,v1,u2,v2) = -Dv2s2(u2,v2).Du2s2(u2,v2)+(S1(u1,v1)-S2(u2,v2)).Dv2u2s2(u2,v2) }
 { du1f4(u1,v1,u2,v2) = Du1s1(u1,v1).Dv2s2(u2,v2) }
 { dv1f4(u1,v1,u2,v2) = Dv1s1(u1,v1).Dv2s2(u2,v2) }
 { du2f4(u1,v1,u2,v2) = -Du2s2(u2,v2).Dv2s2(u2,v2)+(S1(u1,v1)-S2(u2,v2)).Du2v2s2(u2,v2) }
 { dv2f4(u1,v1,u2,v2) = -Dv2s2(u2,v2).Dv2s2(u2,v2)+(S1(u1,v1)-S2(u2,v2)).Dv2v2s2(u2,v2) 
                      = -||Dv2s2(u2,v2)||**2      +(S1(u1,v1)-S2(u2,v2)).Dv2v2s2(u2,v2) }
----------------------------------------------------------------------------*/
//=======================================================================
//function : Extrema_FuncExtSS
//purpose  : 
//=======================================================================
Extrema_FuncExtSS::Extrema_FuncExtSS ()
{
  myS1init = Standard_False;
  myS2init = Standard_False;
}

//=======================================================================
//function : Extrema_FuncExtSS
//purpose  : 
//=======================================================================

Extrema_FuncExtSS::Extrema_FuncExtSS (const Adaptor3d_Surface& S1,
				      const Adaptor3d_Surface& S2)
{
  myS1 = (Adaptor3d_SurfacePtr)&S1;
  myS2 = (Adaptor3d_SurfacePtr)&S2;
  myS1init = Standard_True;
  myS2init = Standard_True;
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Extrema_FuncExtSS::Initialize(const Adaptor3d_Surface& S1,
				   const Adaptor3d_Surface& S2)
{
  myS1 = (Adaptor3d_SurfacePtr)&S1;
  myS2 = (Adaptor3d_SurfacePtr)&S2;
  myS1init = Standard_True;
  myS2init = Standard_True;
  myPoint1.Clear();
  myPoint2.Clear();
  mySqDist.Clear();
}



//=======================================================================
//function : NbVariables
//purpose  : 
//=======================================================================

Standard_Integer Extrema_FuncExtSS::NbVariables () const { return 4;}


//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer Extrema_FuncExtSS::NbEquations () const { return 4;}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_FuncExtSS::Value (const math_Vector& UV, 
					   math_Vector& F)
{
  if (!myS1init || !myS2init) throw Standard_TypeMismatch();
  myU1 = UV(1);
  myV1 = UV(2);
  myU2 = UV(3);
  myV2 = UV(4);
  gp_Vec Du1s1, Dv1s1;
  gp_Vec Du2s2, Dv2s2;
  myS1->D1(myU1,myV1,myP1,Du1s1,Dv1s1);
  myS2->D1(myU2,myV2,myP2,Du2s2,Dv2s2);

  gp_Vec P1P2 (myP2,myP1);

  F(1) = P1P2.Dot(Du1s1);
  F(2) = P1P2.Dot(Dv1s1);
  F(3) = P1P2.Dot(Du2s2);
  F(4) = P1P2.Dot(Dv2s2);

  return Standard_True;  
}

//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_FuncExtSS::Derivatives (const math_Vector& UV, 
						 math_Matrix& Df)
{
  math_Vector F(1,4);
  return Values(UV,F,Df);
}

//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_FuncExtSS::Values (const math_Vector& UV, 
					    math_Vector& F,
					    math_Matrix& Df)
{
  if (!myS1init || !myS2init) throw Standard_TypeMismatch();
  myU1 = UV(1);
  myV1 = UV(2);
  myU2 = UV(3);
  myV2 = UV(4);
  gp_Vec Du1s1, Dv1s1, Du1u1s1, Dv1v1s1, Du1v1s1;
  gp_Vec Du2s2, Dv2s2, Du2u2s2, Dv2v2s2, Du2v2s2;
  myS1->D2(myU1,myV1,myP1,Du1s1,Dv1s1,Du1u1s1,Dv1v1s1,Du1v1s1);
  myS2->D2(myU2,myV2,myP2,Du2s2,Dv2s2,Du2u2s2,Dv2v2s2,Du2v2s2);

  gp_Vec P1P2 (myP2,myP1);

  F(1) = P1P2.Dot(Du1s1);
  F(2) = P1P2.Dot(Dv1s1);
  F(3) = P1P2.Dot(Du2s2);
  F(4) = P1P2.Dot(Dv2s2);

  Df(1,1) = Du1s1.SquareMagnitude() + P1P2.Dot(Du1u1s1);
  Df(1,2) = Dv1s1.Dot(Du1s1) + P1P2.Dot(Du1v1s1);
  Df(1,3) = -Du2s2.Dot(Du1s1);
  Df(1,4) = -Dv2s2.Dot(Du1s1);

  Df(2,1) = Df(1, 2);   // Du1s1.Dot(Dv1s1) + P1P2.Dot(Du1v1s1);
  Df(2,2) = Dv1s1.SquareMagnitude() + P1P2.Dot(Dv1v1s1);
  Df(2,3) = -Du2s2.Dot(Dv1s1);
  Df(2,4) = -Dv2s2.Dot(Dv1s1);

  Df(3,1) = -Df(1,3);   // Du1s1.Dot(Du2s2);
  Df(3,2) = -Df(2,3);   // Dv1s1.Dot(Du2s2);
  Df(3,3) = -Du2s2.SquareMagnitude() + P1P2.Dot(Du2u2s2);
  Df(3,4) = -Dv2s2.Dot(Du2s2) + P1P2.Dot(Du2v2s2);

  Df(4,1) = -Df(1,4);   // Du1s1.Dot(Dv2s2);
  Df(4,2) = -Df(2,4);   // Dv1s1.Dot(Dv2s2);
  Df(4,3) = Df(3,4);    // -Du2s2.Dot(Dv2s2) + P1P2.Dot(Du2v2s2);
  Df(4,4) = -Dv2s2.SquareMagnitude() + P1P2.Dot(Dv2v2s2);

  return Standard_True;
}

//=======================================================================
//function : GetStateNumber
//purpose  : 
//=======================================================================

Standard_Integer Extrema_FuncExtSS::GetStateNumber ()
{
  if (!myS1init || !myS2init) throw Standard_TypeMismatch();
#if 0
  math_Vector Sol(1, 4), UVSol(1, 4);
  UVSol(1) = myU1; UVSol(2) = myV1; UVSol(3) = myU2; UVSol(4) = myV2;
  Value(UVSol, Sol);
  cout <<"F(1)= "<<Sol(1)<<" F(2)= "<<Sol(2)<<" F(3)= "<<Sol(3)<<" F(4)= "<<Sol(4)<<endl;
#endif

  mySqDist.Append(myP1.SquareDistance(myP2));
  myPoint1.Append(Extrema_POnSurf(myU1,myV1,myP1));
  myPoint2.Append(Extrema_POnSurf(myU2,myV2,myP2));
  return 0;
}

//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================

Standard_Integer Extrema_FuncExtSS::NbExt () const
{
  return mySqDist.Length();
}

//=======================================================================
//function : SquareDistance
//purpose  : 
//=======================================================================

Standard_Real Extrema_FuncExtSS::SquareDistance (const Standard_Integer N) const
{
  if (!myS1init || !myS2init) throw Standard_TypeMismatch();
  return mySqDist.Value(N);
}

//=======================================================================
//function : PointOnS1
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_FuncExtSS::PointOnS1 (const Standard_Integer N) const
{
  if (!myS1init || !myS2init) throw Standard_TypeMismatch();
  return myPoint1.Value(N);
}
//=======================================================================
//function : PointOnS2
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_FuncExtSS::PointOnS2 (const Standard_Integer N) const
{
  if (!myS1init || !myS2init) throw Standard_TypeMismatch();
  return myPoint2.Value(N);
}


