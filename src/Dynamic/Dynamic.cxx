// Created on: 1994-08-31
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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
#ifdef DEB
Dynamic_ModeEnum aMode;
#else
Dynamic_ModeEnum aMode=Dynamic_IN;
#endif
  if     (!strcasecmp(amode,"in"      )) aMode = Dynamic_IN;
  else if(!strcasecmp(amode,"out"     )) aMode = Dynamic_OUT;
  else if(!strcasecmp(amode,"inout"   )) aMode = Dynamic_INOUT;
  else if(!strcasecmp(amode,"Internal")) aMode = Dynamic_INTERNAL;
  else if(!strcasecmp(amode,"constant")) aMode = Dynamic_CONSTANT;
return aMode;
}




