// Created on: 1994-08-31
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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

// Historique : 
// CRD : 15/04/97 : Correction warning de compil.
// CRD : 03/07/97 : Portage Windows NT.


#include <Dynamic.hxx>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef WNT
//#define strcasecmp _stricoll
#include <stdio.h>
#endif


//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================

Dynamic_ModeEnum Dynamic::Mode(const Standard_CString amode)
{
Dynamic_ModeEnum aMode=Dynamic_IN;
  if     (!strcasecmp(amode,"in"      )) aMode = Dynamic_IN;
  else if(!strcasecmp(amode,"out"     )) aMode = Dynamic_OUT;
  else if(!strcasecmp(amode,"inout"   )) aMode = Dynamic_INOUT;
  else if(!strcasecmp(amode,"Internal")) aMode = Dynamic_INTERNAL;
  else if(!strcasecmp(amode,"constant")) aMode = Dynamic_CONSTANT;
return aMode;
}




