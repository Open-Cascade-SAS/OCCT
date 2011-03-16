#ifndef __OSD_WNT_BREAK_HXX
# define __OSD_WNT_BREAK_HXX

# ifdef WNT
#  include <OSD.hxx>
#  define _TSTBRK() OSD :: ControlBreak ()
# endif  // WNT

#endif  // __OSD_WNT_BREAK_HXX


