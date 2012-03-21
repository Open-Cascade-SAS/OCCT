// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef __WNTIO_H
# define __WNTIO_H

# ifdef __cplusplus
extern "C" {
# endif  /* __cplusplus */

# ifndef __InterfaceGraphic_API
#  ifdef __InterfaceGraphic_DLL
#   define __InterfaceGraphic_API __declspec( dllexport )
#  else
#   define __InterfaceGraphic_API __declspec( dllimport )
#  endif  /* __InterfaceGraphic_DLL */
# endif  /* __InterfaceGraphic_API */

__InterfaceGraphic_API int   cPrintf ( char*, ... );
__InterfaceGraphic_API int   fcPrintf ( int, char*, ... );
__InterfaceGraphic_API char* form ( char*, ... );
__InterfaceGraphic_API char* GetEnv ( char* );

# ifdef __cplusplus
}
# endif  /* __cplusplus */

# define printf  cPrintf
# define fprintf fcPrintf
# define getenv  GetEnv

#endif  /* __WNTIO_H */
