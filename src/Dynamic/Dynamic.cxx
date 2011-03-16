// File:	Dynamic.cxx
// Created:	Wed Aug 31 15:24:04 1994
// Author:	Gilles DEBARBOUILLE
//		<gde@watson>
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




