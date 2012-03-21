/*
 Created by: PLOTNIKOV Eugeny
 Copyright (c) 1992-1999 Matra Datavision
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

#ifdef WNT
/******************************************************************************/
/* Purpose:   Dummy routine to simulate UNIX's 'symlink'.                     */
/* Warning:   Symbolic links are not supported on Windows NT yet. So, in this */
/*            case simple copying of file is performing.                      */
/******************************************************************************/
/***/
#define STRICT
#include <windows.h>
#include <tchar.h>

#ifdef __cplusplus
extern "C" int symlink ( const _TCHAR*, const _TCHAR* );
#endif  /* __cplusplus */
/***/
/******************************* symlink **************************************/
/***/
__declspec( dllexport ) int symlink ( const _TCHAR* name1, const _TCHAR* name2 ) {

 return CopyFile ( name1, name2, FALSE ) == TRUE ? 0 : -1;

}  /* end symlink */
/***/
/******************************************************************************/
#endif

