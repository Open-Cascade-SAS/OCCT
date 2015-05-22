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

#include <IntImp_ComputeTangence.hxx>
#include <IntImp_ConstIsoparametric.hxx>

static const IntImp_ConstIsoparametric staticChoixRef [4] = {
IntImp_UIsoparametricOnCaro1,
IntImp_VIsoparametricOnCaro1,
IntImp_UIsoparametricOnCaro2,
IntImp_VIsoparametricOnCaro2,
};

Standard_EXPORT const IntImp_ConstIsoparametric *ChoixRef = staticChoixRef ;

//=======================================================================
//function : IntImp_ComputeTangence
//purpose  : 
//=======================================================================
Standard_Boolean IntImp_ComputeTangence(const gp_Vec DPuv[],
					const Standard_Real EpsUV[],
					Standard_Real Tgduv[],
					IntImp_ConstIsoparametric TabIso[]) 
//                 arguments d entree:
// DPuv [0] =derivee en u sur caro 1    
// DPuv [1] =derivee en v sur caro 1
// DPuv [2] =derivee en u sur caro 2
// DPuv [3] =derivee en v sur caro 2
// EpsUV[0] tolerance en u sur caro1
// EpsUV[1] tolerance en v sur caro1
// EpsUV[2] tolerance en u sur caro2
// EpsUV[3] tolerance en v sur caro2
//                 arguments de sortie:
// Tgduv[0] composante sup dp/du de caro1 de la tangente a l intersection
// Tgduv[1] composante sup dp/dv de caro1 de la tangente a l intersection
// Tgduv[2] composante sup dp/du de caro2 de la tangente a l intersection
// Tgduv[3] composante sup dp/dv de caro2 de la tangente a l intersection
// TabIso[0...3] meilleure iso range par ordre decroissant candidate 
//               a l intersection
//         algorithme
// calculer la tangente a l 'intersection ;en utilisant la propriete suivante
// du produit scalaire a^(b^c)=b(ac)-c(ab) on obtient les composantes de la
// tangente a l intersection dans les 2 plans tangents (t=n1^n2 ou n1 normale
// au premier carreau n2 au 2ieme)
// on s assurera que les plans tangents des 2 carreaux ne sont pas //
// les composantes de l intersection dans les plans tangents permettent de 
//determiner l  angle entre les isoparametriques d un carreau avec le carreau
//reciproque 
//on triera par ordre croissant les cosinus :le plus petit cosinus determine le
// meilleure angle donc la meilleure iso a choisir pour trouver
// l intersection 

{
   Standard_Real NormDuv[4], aM2, aTol2;
   Standard_Integer i;
   //
   aTol2=1.e-32;
   //
   for (i=0; i<4; ++i) {
     NormDuv[i] = DPuv[i].SquareMagnitude();    
     if(NormDuv[i]<=aTol2) { 
       return Standard_True;
     } 
   }
   //
   //-------------------------------------------------
   gp_Vec N1 = DPuv[0];
   N1.Cross(DPuv[1]);
   //
   //modified by NIZNHY-PKV Tue Nov 01 08:37:32 2011f
   aM2=N1.SquareMagnitude();
   if (aM2<aTol2) {
     return Standard_True;
   }
   //modified by NIZNHY-PKV Tue Nov 01 08:37:34 2011t
   N1.Normalize();
   //-------------------------------------------------
   gp_Vec N2 = DPuv[2];
   N2.Cross(DPuv[3]);
   //modified by NIZNHY-PKV Tue Nov 01 08:37:32 2011f
   aM2=N2.SquareMagnitude();
   if (aM2<aTol2) {
     return Standard_True;
   }
   //modified by NIZNHY-PKV Tue Nov 01 08:37:34 2011t
   N2.Normalize();
   //
   //modified by NIZNHY-PKV Tue Nov 01 08:31:25 2011f
   for (i=0; i<4; ++i) {
     NormDuv[i]=sqrt(NormDuv[i]);
   }  
   //modified by NIZNHY-PKV Tue Nov 01 08:31:29 2011t
   Tgduv[0] = -DPuv[1].Dot(N2);
   Tgduv[1] =  DPuv[0].Dot(N2);
   Tgduv[2] =  DPuv[3].Dot(N1); 
   Tgduv[3] = -DPuv[2].Dot(N1);


   Standard_Boolean tangent = (Abs(Tgduv[0]) <= EpsUV[0]*NormDuv[1] &&
			       Abs(Tgduv[1]) <= EpsUV[1]*NormDuv[0] &&
			       Abs(Tgduv[2]) <= EpsUV[2]*NormDuv[3] &&
			       Abs(Tgduv[3]) <= EpsUV[3]*NormDuv[2]  );
   if(!tangent) { 
     Standard_Real t=N1.Dot(N2);
     if(t<0.0) t=-t;
     if(t>0.999999999) { 
       tangent=Standard_True; 
     }
   }

   if (!tangent) {
     NormDuv[0] = Abs(Tgduv[1]) /NormDuv[0]; //iso u sur caro1
     NormDuv[1] = Abs(Tgduv[0]) /NormDuv[1]; //iso v sur caro1
     NormDuv[2] = Abs(Tgduv[3]) /NormDuv[2]; // iso u sur caro2
     NormDuv[3] = Abs(Tgduv[2]) /NormDuv[3]; //iso v sur caro2



     //-- Tri sur NormDuv  ( en para. avec ChoixRef ) 
     Standard_Boolean triOk = Standard_False;
     Standard_Real t;
     IntImp_ConstIsoparametric ti;
     for ( i=0;i<=3;i++)  TabIso[i] = ChoixRef[i];
     do { 
       triOk = Standard_True;
       for(i=1;i<=3;i++) { 
	 if(NormDuv[i-1]>NormDuv[i]) { 
	   triOk=Standard_False;
	   t=NormDuv[i]; 
	   NormDuv[i]=NormDuv[i-1];
	   NormDuv[i-1]=t;
	   
	   ti = TabIso[i];
	   TabIso[i] = TabIso[i-1];
	   TabIso[i-1] = ti;
	 }
       }
     }
     while(!triOk);


#if 0     
     // trier par ordre croissant le tableau NormDuv
     Standard_Integer II;
     for (j =0;j<=3;j++) Irang[j]=j;
     for (j =0;j<=3;j++) {
       Tampon = NormDuv[j];
       II=j;
       for (i =j+1;i<=3;i++) {
         if (NormDuv[i] < Tampon) {
	   Tampon = NormDuv[i];
	   II = i;
         }
       }
       Irang[j] = Irang[II];
       Irang[II] = j;
       NormDuv[II] = NormDuv[j];
       NormDuv[j] = Tampon;
     }
     for (j=0; j<=3;j++) TabIso[j]=ChoixRef[Irang[j]];     
#endif
   }
   return tangent;
 }


