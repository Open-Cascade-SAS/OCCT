#ifndef __OSD_WNT_HXX
# define __OSD_WNT_HXX

#include <Standard_Stream.hxx>

# ifndef OSDAPI
#  if !defined(HAVE_NO_DLL)
#   ifdef __OSD_DLL
#    define OSDAPI __declspec( dllexport )
#   else
#    define OSDAPI __declspec( dllimport )
#   endif  // __OSD_DLL
#  else
#   define OSDAPI
#  endif
# endif  // OSDAPI

void     OSDAPI SetErrorStream ( ostream* );
//ostream* OSDAPI ErrorStream    ( void     );
ostream OSDAPI *ErrorStream    ( void     );    // for VisualAge
void     OSDAPI EnablePrefix   ( int      );
int      OSDAPI ErrorPrefix    ( void     );

#endif  // __OSD_WNT_HXX
