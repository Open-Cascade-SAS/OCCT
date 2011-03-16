#ifndef __WNT_ColorRef_HeaderFile
# define __WNT_ColorRef_HeaderFile

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

# ifndef __STANDARD_TYPE_HXX_INCLUDED
#  define __STANDARD_TYPE_HXX_INCLUDED
#  include <Standard_Type.hxx>
# endif  // __STANDARD_TYPE_HXX_INCLUDED 

typedef COLORREF WNT_ColorRef;

extern const Handle( Standard_Type )& STANDARD_TYPE( WNT_ColorRef );

#endif  // __WNT_ColorRef_HeaderFile
