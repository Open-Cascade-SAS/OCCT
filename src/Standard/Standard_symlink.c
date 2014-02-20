/*
 Created by: PLOTNIKOV Eugeny
 Copyright (c) 1992-1999 Matra Datavision
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
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

