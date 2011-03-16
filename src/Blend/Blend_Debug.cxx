// File:	Blend_Debug.cxx
// Created:	Mon Oct  3 10:15:31 1994
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#include <Standard_Boolean.hxx>

#ifndef __Blend_API
# if defined(WNT) && !defined(HAVE_NO_DLL)
#  ifdef __Blend_DLL
#   define __Blend_API __declspec( dllexport )
#  else
#   define __Blend_API __declspec( dllimport )
#  endif /*__Blend_DLL*/
# else
#  define __Blend_API
# endif /*WNT*/
#endif /*__Blend_API*/

//*************************************************
// recuperation des surfaces des conges approximes.
//*************************************************

static Standard_Boolean Blend_traceDRAWSECT = Standard_False;
__Blend_API extern void Blend_SettraceDRAWSECT(const Standard_Boolean b) 
{ Blend_traceDRAWSECT = b; }
__Blend_API extern Standard_Boolean Blend_GettraceDRAWSECT() 
{ return Blend_traceDRAWSECT; }

//*************************************************
// Contexte sans test de deflexion
//*************************************************

static Standard_Boolean Blend_contextNOTESTDEFL = Standard_False;
__Blend_API extern void Blend_SetcontextNOTESTDEFL(const Standard_Boolean b) 
{ Blend_contextNOTESTDEFL = b; }
__Blend_API extern Standard_Boolean Blend_GetcontextNOTESTDEFL() 
{ return Blend_contextNOTESTDEFL; }
