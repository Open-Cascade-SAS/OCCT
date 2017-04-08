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

#include <Interface_Static.hxx>

#include <Message_MsgFile.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Precision.hxx>

#include <stdio.h>

#include "../XSMessage/XSMessage_XSTEP_us.pxx"

static int THE_Interface_Static_deja = 0;

void  Interface_Static::Standards ()
{
  if (THE_Interface_Static_deja)
  {
    return;
  }

  THE_Interface_Static_deja = 1;

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

  // load messages if needed
  if (!Message_MsgFile::HasMsg ("XSTEP_1"))
  {
    if (!Message_MsgFile::LoadFromEnv ("CSF_XSMessage", "XSTEP"))
    {
      Message_MsgFile::LoadFromString (XSMessage_XSTEP_us, sizeof(XSMessage_XSTEP_us) - 1);
    }
    if (!Message_MsgFile::HasMsg ("XSTEP_1"))
    {
      throw Standard_ProgramError("Critical Error - message resources for Interface_Static are invalid or undefined!");
    }
  }
}
