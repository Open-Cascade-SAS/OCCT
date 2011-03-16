// File:	Extrema_GenLocateExtPS.cxx
// Created:	Tue Jul 18 08:23:33 1995
// Author:	Modelistation
//		<model@metrox>


#include <Extrema_GenLocateExtPS.ixx>
#include <Extrema_FuncExtPS.hxx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_Vector.hxx>

//=============================================================================

Extrema_GenLocateExtPS::Extrema_GenLocateExtPS () { myDone = Standard_False; }
//=============================================================================

Extrema_GenLocateExtPS::Extrema_GenLocateExtPS (const gp_Pnt&          P,
						const Adaptor3d_Surface& S, 
						const Standard_Real    U0, 
						const Standard_Real    V0,
						const Standard_Real    TolU, 
						const Standard_Real    TolV)
/*-----------------------------------------------------------------------------
Fonction:
  Recherche de (U,V) proche de (U0,V0) tel que dist(S(U,V),P) soit extremale.

Methode:
  Si (u,v) est solution, on peut ecrire:
   { F1(u,v) = (S(u,v)-P).dS/du(u,v) = 0.
   { F2(u,v) = (S(u,v)-P).dS/dv(u,v) = 0.
  Le probleme consiste a rechercher, dans l'intervalle de definition de la 
  surface, la racine du systeme la plus proche de (U0,V0).
  On utilise la classe math_FunctionSetRoot avec les arguments de construction
  suivants:
  - F: Extrema_FuncExtPS cree a partir de P et S,
  - U0V0: math_Vector (U0,V0),
  - Tol: Min(TolU,TolV),            
				    
  - math_Vector (Uinf,Usup),
  - math_Vector (Vinf,Vsup),
  - 100. .
---------------------------------------------------------------------------*/  
{
  myDone = Standard_False;

  Standard_Real Uinf, Usup, Vinf, Vsup;
  Uinf = S.FirstUParameter();
  Usup = S.LastUParameter();
  Vinf = S.FirstVParameter();
  Vsup = S.LastVParameter();

  Extrema_FuncExtPS F (P,S);
  math_Vector Tol(1, 2), Start(1, 2), BInf(1, 2), BSup(1, 2);

  Tol(1) = TolU;
  Tol(2) = TolV;

  Start(1) = U0;
  Start(2) = V0;

  BInf(1) = Uinf;
  BInf(2) = Vinf;
  BSup(1) = Usup;
  BSup(2) = Vsup;

  math_FunctionSetRoot SR (F, Start,Tol, BInf, BSup);
  if (!SR.IsDone()) 
    return;

  mySqDist = F.SquareDistance(1);
  myPoint = F.Point(1);
  myDone = Standard_True;
}
//=============================================================================

Standard_Boolean Extrema_GenLocateExtPS::IsDone () const { return myDone; }
//=============================================================================

Standard_Real Extrema_GenLocateExtPS::SquareDistance () const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return mySqDist;
}
//=============================================================================

Extrema_POnSurf Extrema_GenLocateExtPS::Point () const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myPoint;
}
//=============================================================================
