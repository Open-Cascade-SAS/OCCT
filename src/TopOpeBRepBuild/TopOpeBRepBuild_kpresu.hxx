// Created on: 1998-04-06
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#ifndef _TopOpeBRepBuild_kpresu_HeaderFile
#define _TopOpeBRepBuild_kpresu_HeaderFile

#define RESUNDEF   (-100)  // resultat indefini
#define RESNEWSHA2 (-12)   // nouveau shape de meme type
#define RESNEWSHA1 (-11)   // nouveau shape de meme type
#define RESNEWCOM  (-3)    // nouveau compound
#define RESNEWSOL  (-2)    // nouveau solide
#define RESNEWSHE  (-1)    // nouveau shell
#define RESNULL    (0)     // resultat = vide
#define RESSHAPE1  (1)     // resultat = shell accedant face tangente de 1
#define RESSHAPE2  (2)     // resultat = shell accedant face tangente de 2
#define RESSHAPE12 (3)     // resultat = shapes 1 et 2
#define RESFACE1   (11)    // resultat = face tangente de 1
#define RESFACE2   (12)    // resultat = face tangente de 2

#define SHEUNDEF    (-100) // indefini
#define SHEAUCU     (-1)   // ne prendre ni classifier aucun shell
#define SHECLASCOUR (1)    // classifier le shell courant
#define SHECLASAUTR (2)    // classifier tous les autres shells
#define SHECLASTOUS (3)    // classifier tous les shells
#define SHEGARDCOUR (4)    // prendre le shell courant sans classifier
#define SHEGARDAUTR (5)    // prendre tous les autres shells sans classifier
#define SHEGARDTOUS (6)    // prendre tous les shells sans classifier

#endif
