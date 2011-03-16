// File:	TopOpeBRepBuild_kpresu.hxx
// Created:	Mon Apr  6 14:57:23 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>


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
