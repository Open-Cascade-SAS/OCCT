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
