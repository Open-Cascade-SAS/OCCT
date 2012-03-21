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

//#74 rln 10.03.99 S4135: new parameters, values and default values
//    gka 10.04.99 S4136: eliminate parameter 'lastpreci'
//S4181 pdn 23.04.99: adding new parameter handling writing of elementary surfaces
#include <Interface_Static.hxx>
#include <Message_MsgFile.hxx>
#include <TCollection_ExtendedString.hxx>
//tatouage de la librairie
#include <Precision.hxx>
#include <stdio.h>


static int deja = 0;

void  Interface_Static::Standards ()
{
  if (deja) return;  deja = 1;

//   read precision
  //#74 rln 10.03.99 S4135: new values and default value
  Interface_Static::Init ("XSTEP","read.precision.mode",'e',"");
  Interface_Static::Init ("XSTEP","read.precision.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","read.precision.mode",'&',"eval File");
  Interface_Static::Init ("XSTEP","read.precision.mode",'&',"eval User");
  Interface_Static::SetIVal ("read.precision.mode",0);

  Interface_Static::Init ("XSTEP","read.precision.val",'r',"1.e-03");

  Interface_Static::Init ("XSTEP","read.maxprecision.mode",'e',"");
  Interface_Static::Init ("XSTEP","read.maxprecision.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","read.maxprecision.mode",'&',"eval Preferred");
  Interface_Static::Init ("XSTEP","read.maxprecision.mode",'&',"eval Forced");
  Interface_Static::SetIVal ("read.maxprecision.mode",0);

  Interface_Static::Init ("XSTEP","read.maxprecision.val",'r',"1.");

//   encode regularity
//  negatif ou nul : ne rien faire. positif : on y va
  Interface_Static::Init ("XSTEP","read.encoderegularity.angle",'r',"0.01");

//   compute surface curves
//  0 : par defaut. 2 : ne garder que le 2D. 3 : ne garder que le 3D
  //gka S4054
  Interface_Static::Init("XSTEP"  ,"read.surfacecurve.mode", 'e',"");
  Interface_Static::Init("XSTEP"  ,"read.surfacecurve.mode", '&',"ematch -3");
  Interface_Static::Init("XSTEP"  ,"read.surfacecurve.mode", '&',"eval 3DUse_Forced");
  Interface_Static::Init("XSTEP"  ,"read.surfacecurve.mode", '&',"eval 2DUse_Forced");
  Interface_Static::Init("XSTEP"  ,"read.surfacecurve.mode", '&',"eval ?");
  Interface_Static::Init("XSTEP"  ,"read.surfacecurve.mode", '&',"eval Default");
  Interface_Static::Init("XSTEP"  ,"read.surfacecurve.mode", '&',"eval ?");
  Interface_Static::Init("XSTEP"  ,"read.surfacecurve.mode", '&',"eval 2DUse_Preferred");
  Interface_Static::Init("XSTEP"  ,"read.surfacecurve.mode", '&',"eval 3DUse_Preferred");
  Interface_Static::SetIVal ("read.surfacecurve.mode",0);

//   write precision
  Interface_Static::Init ("XSTEP","write.precision.mode",'e',"");
  Interface_Static::Init ("XSTEP","write.precision.mode",'&',"ematch -1");
  Interface_Static::Init ("XSTEP","write.precision.mode",'&',"eval Min");
  Interface_Static::Init ("XSTEP","write.precision.mode",'&',"eval Average");
  Interface_Static::Init ("XSTEP","write.precision.mode",'&',"eval Max");
  Interface_Static::Init ("XSTEP","write.precision.mode",'&',"eval User");
  Interface_Static::SetIVal ("write.precision.mode",0);

  Interface_Static::Init ("XSTEP","write.precision.val",'r',"1.e-03");

  // Write surface curves
  // 0: write (defaut), 1: do not write, 2: write except for analytical surfaces
  Interface_Static::Init("XSTEP"  ,"write.surfacecurve.mode", 'e',"");
  Interface_Static::Init("XSTEP"  ,"write.surfacecurve.mode", '&',"ematch 0");
  Interface_Static::Init("XSTEP"  ,"write.surfacecurve.mode", '&',"eval Off");
  Interface_Static::Init("XSTEP"  ,"write.surfacecurve.mode", '&',"eval On");
//  Interface_Static::Init("XSTEP"  ,"write.surfacecurve.mode", '&',"eval NoAnalytic");
  Interface_Static::SetIVal ("write.surfacecurve.mode",1);

//  lastpreci : pour recuperer la derniere valeur codee (cf XSControl)
//    (0 pour dire : pas codee)
//:S4136  Interface_Static::Init("std"    ,"lastpreci", 'r',"0.");


//  ****  MESSAGERIE DE BASE  ****

//  Chargement "manuel" au cas ou les fichiers, env, etc sont KO

  Message_MsgFile::AddMsg ("XSTEP_1","Beginning of IGES file memory loading.");
  Message_MsgFile::AddMsg ("XSTEP_2","File opening error");
  Message_MsgFile::AddMsg ("XSTEP_3","Reason : No such file or directory");
  Message_MsgFile::AddMsg ("XSTEP_4","Reason : Not enough space");
  Message_MsgFile::AddMsg ("XSTEP_5","Reason : Permission denied");
  Message_MsgFile::AddMsg ("XSTEP_6","Reason : Too many open files");
  Message_MsgFile::AddMsg ("XSTEP_7","Reason : Undetermined");
  Message_MsgFile::AddMsg ("XSTEP_8","End of loading IGES file to memory (Elapsed time : %s).");

  Message_MsgFile::AddMsg ("XSTEP_11","Internal error during the file header reading. The process continues");
  Message_MsgFile::AddMsg ("XSTEP_13","Internal error during the reading of the entity %d");
  Message_MsgFile::AddMsg ("XSTEP_14","Internal error during the reading of the entity %d (parameter %d)");
  Message_MsgFile::AddMsg ("XSTEP_15","Total number of loaded entities : %d.");
  Message_MsgFile::AddMsg ("XSTEP_16","Beginning of the model loading");
  Message_MsgFile::AddMsg ("XSTEP_17","End of the model loading");
  Message_MsgFile::AddMsg ("XSTEP_21","Number of ignored Null Entities : %d");
  Message_MsgFile::AddMsg ("XSTEP_22","Entity %s : unknown");
  Message_MsgFile::AddMsg ("XSTEP_23","Entity %s, Type %s : recovered");
  Message_MsgFile::AddMsg ("XSTEP_24","Report : %d unknown entities");
  Message_MsgFile::AddMsg ("XSTEP_25","Number of fail in memory loading : %d.");
  Message_MsgFile::AddMsg ("XSTEP_26","Number of warning in memory loading : %d.");

//  Chargement du vrai fichier langue
  Message_MsgFile::LoadFromEnv ("CSF_XSMessage","XSTEP");
}
