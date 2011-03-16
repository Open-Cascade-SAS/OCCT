#ifdef WNT
/******************************************************************************/
/* File:      symlink.c                                                       */
/* Purpose:   Dummy routine to simulate UNIX's 'symlink'.                     */
/* Author:    PLOTNIKOV Eugeny                                                */
/*            <eugeny@vortex>                                                 */
/* Warning:   Symbolic links are not supported on Windows NT yet. So, in this */
/*            case simple copying of file is performing.                      */
/* Copyright: MATRA Datavision (C)1996                                        */
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

