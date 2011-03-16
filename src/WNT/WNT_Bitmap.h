#ifndef __WNT_BITMAP_H
# define __WNT_BITMAP_H

# ifndef __WINDOWS_H_INCLUDED
#  define __WINDOWS_H_INCLUDED
#  ifndef STRICT
#   define STRICT
#  endif  /* STRICT */
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

#ifdef DrawText
#undef DrawText
#endif

#  ifdef THIS
#   undef THIS
#  endif  // THIS
# endif  // __WINDOWS_H_INCLUDED

typedef struct _wnt_bitmap {

                int     nUsed;
                HBITMAP hBmp;

               } WNT_Bitmap, *PWNT_Bitmap;

#endif  /* __WNT_SAFEBITMAP_H */
