#ifndef __WNT_H
# define __WNT_H

# ifdef __cplusplus
extern "C" {
# endif  /* __cplusplus */

# ifndef __WNT_API
#  if !defined(HAVE_NO_DLL)
#   ifdef __WNT_DLL
#    define __WNT_API __declspec( dllexport )
#   else
#    define __WNT_API __declspec( dllimport )
#   endif  /* __WNT_DLL */
#  else
#   define __WNT_API
#  endif
# endif  /* __WNT_API */

__WNT_API int WNT_SysPalInUse ( void );

# ifdef __cplusplus
}
# endif  /* __cplusplus */

#endif  /* __WNT_H */
