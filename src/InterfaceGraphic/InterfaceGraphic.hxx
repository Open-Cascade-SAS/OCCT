#ifndef __INTERFACE_GRAPHIC_HXX
# define __INTERFACE_GRAPHIC_HXX

#ifdef WNT
# include <InterfaceGraphic_WNT.hxx>
#else
# include <InterfaceGraphic_X11.hxx>
#endif //WNT

#endif  /* __INTERFACE_GRAPHIC_HXX */
