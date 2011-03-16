#ifndef __WNT_WindowData_HeaderFile
# define __WNT_WindowData_HeaderFile

# ifndef __WINDOWS_H_INCLUDED
#  define __WINDOWS_H_INCLUDED
#  ifndef STRICT
#   define STRICT
#  endif // STRICT
#  include <windows.h>
#  include <windowsx.h>

#  ifdef DrawText
#   undef DrawText
#  endif // DrawText

#  ifdef THIS
#   undef THIS
#  endif  // THIS
# endif  // __WINDOWS_H_INCLUDED

# ifndef __STANDARD_TYPE_HXX_INCLUDED
#  define __STANDARD_TYPE_HXX_INCLUDED
#  include <Standard_Type.hxx>
# endif  // __STANDARD_TYPE_HXX_INCLUDED 

#include <InterfaceGraphic_WNT.hxx>

typedef WINDOW_DATA WNT_WindowData;

extern const Handle( Standard_Type )& STANDARD_TYPE( WNT_WindowData );

#endif  // __WNT_WindowData_HeaderFile
