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

//============================================ IntAna2d_AnaIntersection_8.cxx
//============================================================================

#include <gp_Circ2d.hxx>
#include <gp_Hypr2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <IntAna2d_Outils.hxx>
#include <StdFail_NotDone.hxx>

// -----------------------------------------------------------------
// ------ Verification de la validite des points obtenus  ----------
// --- Methode a implementer dans les autres routines si on constate
// --- des problemes d'instabilite numerique sur
// ---      * la construction des polynomes en t (t:parametre)
// ---      * la resolution du polynome
// ---      * le retour : parametre t -> point d'intersection
// --- Probleme : A partir de quelle Tolerance un point n'est
// ---            plus un point de la courbe. (f(x,y)=1e-10 ??)
// ---            ne donne pas d'info. sur la dist. du pt a la courbe
// -----------------------------------------------------------------
// ------ Methode non implementee pour les autres Intersections
// --- Si un probleme est constate : Dupliquer le code entre les
// --- commentaires VERIF-VALID
// -----------------------------------------------------------------
void IntAna2d_AnaIntersection::Perform(const gp_Hypr2d& H, const IntAna2d_Conic& Conic)
{
  bool   HIsDirect = H.IsDirect();
  double A, B, C, D, E, F;
  double px0, px1, px2, px3, px4;
  double minor_radius = H.MinorRadius();
  double major_radius = H.MajorRadius();
  int    i;
  double tx, ty, S;

  done = false;
  nbp  = 0;
  para = false;
  iden = false;
  empt = false;

  gp_Ax2d Axe_rep(H.XAxis());
  Conic.Coefficients(A, B, C, D, E, F);
  Conic.NewCoefficients(A, B, C, D, E, F, Axe_rep);

  double A_major_radiusP2       = A * major_radius * major_radius;
  double B_minor_radiusP2       = B * minor_radius * minor_radius;
  double C_2_major_minor_radius = C * 2.0 * major_radius * minor_radius;

  // Parametre : t avec x=MajorRadius*Ch(t)  y=:minorRadius*Sh(t)
  // Le polynome est reecrit en Exp(t)
  // Suivent les Coeffs du polynome P multiplie par 4*Exp(t)^2

  px0 = A_major_radiusP2 - C_2_major_minor_radius + B_minor_radiusP2;
  px1 = 4.0 * (D * major_radius - E * minor_radius);
  px2 = 2.0 * (A_major_radiusP2 + 2.0 * F - B_minor_radiusP2);
  px3 = 4.0 * (D * major_radius + E * minor_radius);
  px4 = A_major_radiusP2 + C_2_major_minor_radius + B_minor_radiusP2;

  MyDirectPolynomialRoots Sol(px4, px3, px2, px1, px0);

  if (!Sol.IsDone())
  {
    //-- std::cout<<" Done = False ds IntAna2d_AnaIntersection_8.cxx "<<std::endl;
    done = false;
    return;
  }
  else
  {
    if (Sol.InfiniteRoots())
    {
      iden = true;
      done = true;
      return;
    }
    // On a X=(CosH(t)*major_radius)/2 , Y=(SinH(t)*minor_radius)/2
    //      la Resolution est en S=Exp(t)
    nbp                = Sol.NbSolutions();
    int nb_sol_valides = 0;
    for (i = 1; i <= nbp; i++)
    {
      S = Sol.Value(i);
      if (S > RealEpsilon())
      {
        tx = 0.5 * major_radius * (S + 1 / S);
        ty = 0.5 * minor_radius * (S - 1 / S);

        //--- Est-on sur la bonne branche de l'Hyperbole
        //--------------- VERIF-VALIDITE-INTERSECTION ----------
        //--- On Suppose que l'ecart sur la courbe1 est nul
        //--- (le point a ete obtenu par parametrage)
        //--- ??? la tolerance a ete fixee a 1e-10 ?????????????


        nb_sol_valides++;
        Coord_Ancien_Repere(tx, ty, Axe_rep);
        S = std::log(S);
        if (!HIsDirect)
          S = -S;
        lpnt[nb_sol_valides - 1].SetValue(tx, ty, S);
      }
    }
    nbp = nb_sol_valides;
    Traitement_Points_Confondus(nbp, lpnt);
  }
  done = true;
}
