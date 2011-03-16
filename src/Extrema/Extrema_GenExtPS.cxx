// File:	Extrema_GenExtPS.cxx
// Created:	Tue Jul 18 08:21:34 1995
// Author:	Modelistation
//		<model@metrox>

//  Modified by skv - Thu Sep 30 15:21:07 2004 OCC593


#include <Extrema_GenExtPS.ixx>
#include <StdFail_NotDone.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array2OfInteger.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <GeomAbs_IsoType.hxx>




//=============================================================================

/*-----------------------------------------------------------------------------
Fonction:
   Recherche de toutes les distances extremales entre le point P et la surface
  S a partir d'un echantillonnage (NbU,NbV).

Methode:
   L'algorithme part de l'hypothese que l'echantillonnage est suffisamment fin
  pour que, s'il existe N distances extremales entre le point et la surface,
  alors il existe aussi N extrema entre le point et la grille.
  Ainsi, l'algorithme consiste a partir des extrema de la grille pour trouver
  les extrema de la surface.
   Les extrema sont calcules par l'algorithme math_FunctionSetRoot avec les
  arguments suivants:
  - F: Extrema_FuncExtPS cree a partir de P et S,
  - UV: math_Vector dont les composantes sont les parametres de l'extremum sur
    la grille,
  - Tol: Min(TolU,TolV), (Prov.:math_FunctionSetRoot n'autorise pas un vecteur)
  - UVinf: math_Vector dont les composantes sont les bornes inferieures de u et
    v,
  - UVsup: math_Vector dont les composantes sont les bornes superieures de u et
    v.

Traitement:
  a- Constitution du tableau des distances (TbDist(0,NbU+1,0,NbV+1)):
      Le tableau est volontairement etendu; les lignes 0 et NbU+1 et les
     colonnes 0 et NbV+1 seront initialisees a RealFirst() ou RealLast()
     pour simplifier les tests effectues dans l'etape b
     (on n'a pas besoin de tester si le point est sur un bord de la grille).
  b- Calcul des extrema:
      On recherche d'abord les minima et ensuite les maxima. Ces 2 traitements
     se passent de facon similaire:
  b.a- Initialisations:
      - des 'bords' du tableau TbDist (a RealLast() dans le cas des minima
        et a RealLast() dans le cas des maxima),
      - du tableau TbSel(0,NbU+1,0,NbV+1) de selection des points pour un
        calcul d'extremum local (a 0). Lorsqu'un point sera selectionne,
	il ne sera plus selectionnable, ainsi que ses points adjacents
	(8 au maximum). Les adresses correspondantes seront mises a 1.
  b.b- Calcul des minima (ou maxima):
       On boucle sur toutes les distances du tableau TbDist:
      - recherche d'un minimum (ou maximum) sur la grille,
      - calcul de l'extremum sur la surface,
      - mise a jour du tableau TbSel.
-----------------------------------------------------------------------------*/

static Standard_Boolean IsoIsDeg  (const Adaptor3d_Surface& S,
				   const Standard_Real      Param,
				   const GeomAbs_IsoType    IT,
				   const Standard_Real      TolMin,
				   const Standard_Real      TolMax) 
{
    Standard_Real U1=0.,U2=0.,V1=0.,V2=0.,T;
    Standard_Boolean Along = Standard_True;
    U1 = S.FirstUParameter();
    U2 = S.LastUParameter();
    V1 = S.FirstVParameter();
    V2 = S.LastVParameter();
    gp_Vec D1U,D1V;
    gp_Pnt P;
    Standard_Real Step,D1NormMax;
    if (IT == GeomAbs_IsoV) 
    {
      Step = (U2 - U1)/10;
      D1NormMax=0.;
      for (T=U1;T<=U2;T=T+Step) 
      {
        S.D1(T,Param,P,D1U,D1V);
        D1NormMax=Max(D1NormMax,D1U.Magnitude());
      }

      if (D1NormMax >TolMax || D1NormMax < TolMin ) 
           Along = Standard_False;
    }
    else 
    {
      Step = (V2 - V1)/10;
      D1NormMax=0.;
      for (T=V1;T<=V2;T=T+Step) 
      {
	S.D1(Param,T,P,D1U,D1V);
        D1NormMax=Max(D1NormMax,D1V.Magnitude());
      }

      if (D1NormMax >TolMax || D1NormMax < TolMin ) 
           Along = Standard_False;


    }
    return Along;
}
//----------------------------------------------------------
Extrema_GenExtPS::Extrema_GenExtPS() 
{
  myDone = Standard_False;
  myInit = Standard_False;
}


Extrema_GenExtPS::Extrema_GenExtPS (const gp_Pnt&          P,
			      const Adaptor3d_Surface& S,
			      const Standard_Integer NbU, 
			      const Standard_Integer NbV,
			      const Standard_Real    TolU, 
			      const Standard_Real    TolV) 
: myF (P,S)
{
  Initialize(S, NbU, NbV, TolU, TolV);
  Perform(P);
}

Extrema_GenExtPS::Extrema_GenExtPS (const gp_Pnt&          P,
			      const Adaptor3d_Surface& S,
			      const Standard_Integer NbU, 
			      const Standard_Integer NbV,
			      const Standard_Real    Umin,
			      const Standard_Real    Usup,
			      const Standard_Real    Vmin,
			      const Standard_Real    Vsup,
			      const Standard_Real    TolU, 
			      const Standard_Real    TolV) 
: myF (P,S)
{
  Initialize(S, NbU, NbV, Umin, Usup, Vmin, Vsup, TolU, TolV);
  Perform(P);
}


void Extrema_GenExtPS::Initialize(const Adaptor3d_Surface& S,
				  const Standard_Integer NbU, 
				  const Standard_Integer NbV,
				  const Standard_Real    TolU, 
				  const Standard_Real    TolV)
{
  myumin = S.FirstUParameter();
  myusup = S.LastUParameter();
  myvmin = S.FirstVParameter();
  myvsup = S.LastVParameter();
  Initialize(S,NbU,NbV,myumin,myusup,myvmin,myvsup,TolU,TolV);
}


void Extrema_GenExtPS::Initialize(const Adaptor3d_Surface& S,
				  const Standard_Integer NbU, 
				  const Standard_Integer NbV,
				  const Standard_Real    Umin,
				  const Standard_Real    Usup,
				  const Standard_Real    Vmin,
				  const Standard_Real    Vsup,
				  const Standard_Real    TolU, 
				  const Standard_Real    TolV)
{
  myInit = Standard_True;
  myS = (Adaptor3d_SurfacePtr)&S;
  myusample = NbU;
  myvsample = NbV;
  mytolu = TolU;
  mytolv = TolV;
  myumin = Umin;
  myusup = Usup;
  myvmin = Vmin;
  myvsup = Vsup;

  if ((myusample < 2) ||
      (myvsample < 2)) { Standard_OutOfRange::Raise(); }

  myF.Initialize(S);

  mypoints = new TColgp_HArray2OfPnt(0,myusample+1,0,myvsample+1);

/*
a- Constitution du tableau des distances (TbDist(0,myusample+1,0,myvsample+1)):
   ---------------------------------------------------------------
*/

// Parametrage de l echantillon

  Standard_Real PasU = myusup - myumin;
  Standard_Real PasV = myvsup - myvmin;
  Standard_Real U0 = PasU / myusample / 100.;
  Standard_Real V0 = PasV / myvsample / 100.;
  gp_Pnt P1;
  PasU = (PasU - U0) / (myusample - 1);
  PasV = (PasV - V0) / (myvsample - 1);
  U0 = U0/2. + myumin;
  V0 = V0/2. + myvmin;

// Calcul des distances

  Standard_Integer NoU, NoV;
  Standard_Real U, V;
  for ( NoU = 1, U = U0; NoU <= myusample; NoU++, U += PasU) {
    for ( NoV = 1, V = V0; NoV <= myvsample; NoV++, V += PasV) {
      P1 = myS->Value(U, V);
      mypoints->SetValue(NoU,NoV,P1);
    }
  }
}



void Extrema_GenExtPS::Perform(const gp_Pnt& P) 
{  
  myDone = Standard_False;
  myF.SetPoint(P);


/*
b- Calcul des minima:
   -----------------
   b.a) Initialisations:
*/

// Parametrage de l echantillon

  Standard_Real PasU = myusup - myumin;
  Standard_Real PasV = myvsup - myvmin;
  Standard_Real U, U0 = PasU / myusample / 100.;
  Standard_Real V, V0 = PasV / myvsample / 100.;
  PasU = (PasU - U0) / (myusample - 1);
  PasV = (PasV - V0) / (myvsample - 1);
  U0 = U0/2.+myumin;
  V0 = V0/2.+myvmin;

//     - generales
  math_Vector Tol(1, 2);
  Tol(1) = mytolu;
  Tol(2) = mytolv;
  math_Vector UV(1,2), UVinf(1,2), UVsup(1,2);
  UVinf(1) = myumin;
  UVinf(2) = myvmin;
  UVsup(1) = myusup;
  UVsup(2) = myvsup;

//     - des 'bords' du tableau mytbdist
  Standard_Integer NoU,NoV;

  TColStd_Array2OfReal TheDist(0, myusample+1, 0, myvsample+1);
  for ( NoU = 1, U = U0; NoU <= myusample; NoU++, U += PasU) {
    for ( NoV = 1, V = V0; NoV <= myvsample; NoV++, V += PasV) {
      TheDist(NoU, NoV) = P.SquareDistance(mypoints->Value(NoU, NoV));
    }
  }
  
  for (NoV = 0; NoV <= myvsample+1; NoV++) {
    TheDist(0,NoV) = RealLast();
    TheDist(myusample+1,NoV) = RealLast();
  }
  for (NoU = 1; NoU <= myusample; NoU++) {
    TheDist(NoU,0) = RealLast();
    TheDist(NoU,myvsample+1) = RealLast();
  }

//     - du tableau TbSel(0,myusample+1,0,myvsample+1) de selection des points
  TColStd_Array2OfInteger TbSel(0,myusample+1,0,myvsample+1);
  TbSel.Init(0);
/*
   b.b) Calcul des minima:
*/
//     - recherche d un minimum sur la grille
  math_Vector errors(1,2);
  math_Vector root(1, 2);
  Standard_Real eps = 1.e-9;
  Standard_Integer nbsubsample = 11;
  Standard_Integer Nu, Nv;
  Standard_Real Dist;
  for (NoU = 1; NoU <= myusample; NoU++) {
    for (NoV = 1; NoV <= myvsample; NoV++) {
      if (TbSel(NoU,NoV) == 0) {
	Dist = TheDist(NoU,NoV);
	if ((TheDist(NoU-1,NoV-1) >= Dist) &&
	    (TheDist(NoU-1,NoV  ) >= Dist) &&
	    (TheDist(NoU-1,NoV+1) >= Dist) &&
	    (TheDist(NoU  ,NoV-1) >= Dist) &&
	    (TheDist(NoU  ,NoV+1) >= Dist) &&
	    (TheDist(NoU+1,NoV-1) >= Dist) &&
	    (TheDist(NoU+1,NoV  ) >= Dist) &&
	    (TheDist(NoU+1,NoV+1) >= Dist)) {

//     - calcul de l extremum sur la surface:
	  UV(1) = U0 + (NoU - 1) * PasU;
	  UV(2) = V0 + (NoV - 1) * PasV;
//  Modified by skv - Thu Sep 30 15:21:07 2004 OCC593 Begin
// 	  math_FunctionSetRoot S (myF,UV,Tol,UVinf,UVsup);
	   // Default value for math_FunctionSetRoot.
	  Standard_Integer aNbMaxIter = 100;

	  if (myF.HasDegIso())
	    aNbMaxIter = 150;

	  math_FunctionSetRoot S (myF,UV,Tol,UVinf,UVsup, aNbMaxIter);
	  if(S.IsDone()) {
	    root = S.Root();
	    myF.Value(root, errors);
	    if(Abs(errors(1)) > eps || Abs(errors(2)) > eps) {
	      //try to improve solution on subgrid of sample points
	      gp_Pnt PSol = myS->Value(root(1), root(2));
	      Standard_Real DistSol = P.SquareDistance(PSol);

	      Standard_Real u1 = Max(UV(1) - PasU, myumin), u2 = Min(UV(1) + PasU, myusup);
	      Standard_Real v1 = Max(UV(2) - PasV, myvmin), v2 = Min(UV(2) + PasV, myvsup);
	      
	      if(u2 - u1 < 2.*PasU) {
		if(Abs(u1 - myumin) < 1.e-9) {
		  u2 = u1 + 2.*PasU;
		  u2 = Min(u2, myusup);
		}
		if(Abs(u2 - myusup) < 1.e-9) {
		  u1 = u2 - 2.*PasU;
		  u1 = Max(u1, myumin);
		}
	      }

	      if(v2 - v1 < 2.*PasV) {
		if(Abs(v1 - myvmin) < 1.e-9) {
		  v2 = v1 + 2.*PasV;
		  v2 = Min(v2, myvsup);
		}
		if(Abs(v2 - myvsup) < 1.e-9) {
		  v1 = v2 - 2.*PasV;
		  v1 = Max(v1, myvmin);
		}
	      }

	      Standard_Real du = (u2 - u1)/(nbsubsample-1);
	      Standard_Real dv = (v2 - v1)/(nbsubsample-1);
	      Standard_Real u, v;
	      Standard_Real dist;

	      Standard_Boolean NewSolution = Standard_False;
	      for (Nu = 1, u = u1; Nu < nbsubsample; Nu++, u += du) {
		for (Nv = 1, v = v1; Nv < nbsubsample; Nv++, v += dv) {
		  gp_Pnt Puv = myS->Value(u, v);
		  dist = P.SquareDistance(Puv);
		  
		  if(dist < DistSol) {
		    UV(1) = u;
		    UV(2) = v;
		    NewSolution = Standard_True;
		    DistSol = dist;
		  }
		}
	      }
	      if(NewSolution) {
		//try to precise
		math_FunctionSetRoot S (myF,UV,Tol,UVinf,UVsup, aNbMaxIter);
	      }

	    }
	  }
//  Modified by skv - Thu Sep 30 15:21:07 2004 OCC593 End
      
//     - mise a jour du tableau TbSel 	  
	  for (Nu = NoU-1; Nu <= NoU+1; Nu++) {
	    for (Nv = NoV-1; Nv <= NoV+1; Nv++) {
	      TbSel(Nu,Nv) = 1;
	    }
	  }
	}
      } // if (TbSel(NoU,NoV)
    } // for (NoV = 1; ...
  } // for (NoU = 1; ...
/*
c- Calcul des maxima:
   -----------------
   c.a) Initialisations:
*/
//     - des 'bords' du tableau mytbdist
  for (NoV = 0; NoV <= myvsample+1; NoV++) {
    TheDist(0,NoV) = RealFirst();
    TheDist(myusample+1,NoV) = RealFirst();
  }
  for (NoU = 1; NoU <= myusample; NoU++) {
    TheDist(NoU,0) = RealFirst();
    TheDist(NoU,myvsample+1) = RealFirst();
  }

//     - du tableau TbSel(0,myusample+1,0,myvsample+1) de selection des points
  for (NoU = 0; NoU <= myusample+1; NoU++) {
    for (NoV = 0; NoV <= myvsample+1; NoV++) {
      TbSel(NoU,NoV) = 0;
    }
  }
/*
   c.b) Calcul des maxima:
*/
//     - recherche d un maximum sur la grille
  for (NoU = 1; NoU <= myusample; NoU++) {
    for (NoV = 1; NoV <= myvsample; NoV++) {
      if (TbSel(NoU,NoV) == 0) {
	Dist = TheDist(NoU,NoV);
	if ((TheDist(NoU-1,NoV-1) <= Dist) &&
	    (TheDist(NoU-1,NoV  ) <= Dist) &&
	    (TheDist(NoU-1,NoV+1) <= Dist) &&
	    (TheDist(NoU  ,NoV-1) <= Dist) &&
	    (TheDist(NoU  ,NoV+1) <= Dist) &&
	    (TheDist(NoU+1,NoV-1) <= Dist) &&
	    (TheDist(NoU+1,NoV  ) <= Dist) &&
	    (TheDist(NoU+1,NoV+1) <= Dist)) {

//     - calcul de l extremum sur la surface:
	  UV(1) = U0 + (NoU - 1) * PasU;
	  UV(2) = V0 + (NoV - 1) * PasV;
//  Modified by skv - Thu Sep 30 15:21:07 2004 OCC593 Begin
// 	  math_FunctionSetRoot S (myF,UV,Tol,UVinf,UVsup);
	   // Default value for math_FunctionSetRoot.
	  Standard_Integer aNbMaxIter = 100;

	  if (myF.HasDegIso())
	    aNbMaxIter = 150;

	  math_FunctionSetRoot S (myF,UV,Tol,UVinf,UVsup, aNbMaxIter);
	  if(S.IsDone()) {
	    root = S.Root();
	    myF.Value(root, errors);
	  }
//  Modified by skv - Thu Sep 30 15:21:07 2004 OCC593 End
      
//     - mise a jour du tableau TbSel 	  
	  for (Nu = NoU-1; Nu <= NoU+1; Nu++) {
	    for (Nv = NoV-1; Nv <= NoV+1; Nv++) {
	      TbSel(Nu,Nv) = 1;
	    }
	  }
	}
      } // if (TbSel(NoU,NoV))
    } // for (NoV = 1; ...)
  } // for (NoU = 1; ...)
  myDone = Standard_True;
}
//=============================================================================

Standard_Boolean Extrema_GenExtPS::IsDone () const { return myDone; }
//=============================================================================

Standard_Integer Extrema_GenExtPS::NbExt () const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.NbExt();
}
//=============================================================================

Standard_Real Extrema_GenExtPS::SquareDistance (const Standard_Integer N) const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.SquareDistance(N);
}
//=============================================================================

Extrema_POnSurf Extrema_GenExtPS::Point (const Standard_Integer N) const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.Point(N);
}
//=============================================================================

