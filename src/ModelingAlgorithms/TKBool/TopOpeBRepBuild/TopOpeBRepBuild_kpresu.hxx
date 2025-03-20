// Created on: 1998-04-06
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TopOpeBRepBuild_kpresu_HeaderFile
#define _TopOpeBRepBuild_kpresu_HeaderFile

#define RESUNDEF (-100)  // resultat indefini
#define RESNEWSHA2 (-12) // nouveau shape de meme type
#define RESNEWSHA1 (-11) // nouveau shape de meme type
#define RESNEWCOM (-3)   // nouveau compound
#define RESNEWSOL (-2)   // nouveau solide
#define RESNEWSHE (-1)   // nouveau shell
#define RESNULL (0)      // resultat = vide
#define RESSHAPE1 (1)    // resultat = shell accedant face tangente de 1
#define RESSHAPE2 (2)    // resultat = shell accedant face tangente de 2
#define RESSHAPE12 (3)   // resultat = shapes 1 et 2
#define RESFACE1 (11)    // resultat = face tangente de 1
#define RESFACE2 (12)    // resultat = face tangente de 2

#define SHEUNDEF (-100) // indefini
#define SHEAUCU (-1)    // ne prendre ni classifier aucun shell
#define SHECLASCOUR (1) // classifier le shell courant
#define SHECLASAUTR (2) // classifier tous les autres shells
#define SHECLASTOUS (3) // classifier tous les shells
#define SHEGARDCOUR (4) // prendre le shell courant sans classifier
#define SHEGARDAUTR (5) // prendre tous les autres shells sans classifier
#define SHEGARDTOUS (6) // prendre tous les shells sans classifier

#endif
