// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef XW_CEXTERN
#define XW_CEXTERN

#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#define XW_PROTOTYPE 1
#endif

#define XW_EXT_DISPLAY void
#define XW_EXT_WINDOW void
#define XW_EXT_COLORMAP void
#define XW_EXT_WIDTHMAP void
#define XW_EXT_TYPEMAP void
#define XW_EXT_FONTMAP void
#define XW_EXT_TILEMAP void
#define XW_EXT_MARKMAP void
#define XW_EXT_IMAGEDATA void
#define XW_EXT_COLOR void
#define XW_USERDATA void

#ifndef WNT
# include <Aspect_Handle.hxx>
#else
# define STRICT
# include <windows.h>
# define Aspect_Handle HANDLE
#endif  /* WNT */

#include <Aspect_Units.hxx>
#include <Aspect_FillMethod.hxx>

#ifndef _Xw_TypeOfVisual_HeaderFile
typedef enum { 
        Xw_TOV_STATICGRAY,
        Xw_TOV_GRAYSCALE,
        Xw_TOV_STATICCOLOR,
        Xw_TOV_PSEUDOCOLOR,
        Xw_TOV_TRUECOLOR,
        Xw_TOV_DIRECTCOLOR,
        Xw_TOV_DEFAULT,
        Xw_TOV_PREFERRED_PSEUDOCOLOR,
        Xw_TOV_PREFERRED_TRUECOLOR,
        Xw_TOV_PREFERRED_OVERLAY,
        Xw_TOV_OVERLAY
} Xw_TypeOfVisual ;
#endif

#ifndef _Xw_TypeOfMapping_HeaderFile
typedef enum { 
        Xw_TOM_HARDRAMP,
        Xw_TOM_SIMPLERAMP,
        Xw_TOM_BESTRAMP,
        Xw_TOM_COLORCUBE,
        Xw_TOM_READONLY 
} Xw_TypeOfMapping ;
#endif

typedef int XW_ATTRIB ;
#define XW_ALLATTRIB ~0 
#define XW_NOATTRIB  0

typedef enum { 
        XW_REPLACE,XW_ERASE,XW_XORBACK,XW_XORLIGHT,XW_HIGHLIGHT,XW_UNHIGHLIGHT
} XW_DRAWMODE ;

typedef enum {
        XW_DISABLE,XW_ENABLE
} XW_DOUBLEBUFFERMODE ;

typedef enum {
        XW_WS_UNKNOWN,XW_MAP,XW_POP,XW_PUSH,XW_ICONIFY
} XW_WINDOWSTATE ;

typedef enum {
        XW_MOUSEBUTTON,XW_MOUSEBUTTON1,XW_MOUSEBUTTON2,XW_MOUSEBUTTON3,
        XW_MOUSEMOVE,XW_MOUSEMOVEWITHBUTTON,XW_MOUSEMOVEWITHBUTTON1,
        XW_MOUSEMOVEWITHBUTTON2,XW_MOUSEMOVEWITHBUTTON3,
        XW_KEYBOARD,XW_ENTERWINDOW,XW_RESIZEWINDOW,
        XW_MOVEWINDOW,XW_ICONIFYWINDOW,XW_EXPOSEWINDOW,XW_ALLEVENT
} XW_EVENTTYPE ;

typedef struct {
        XW_EVENTTYPE type ;
        XW_EXT_WINDOW *awindow ;
} XW_ANYEVENT ;

typedef struct {
        XW_EVENTTYPE type ;
        XW_EXT_WINDOW *awindow ;
        int     modifier ;      /* Special Key code */
        int     button ;        /* Button number */
        int     state ;         /* Button state : True if Press */
        int     x,y ;           /* Mouse position */
} XW_MOUSEBUTTONEVENT ;

typedef struct {
        XW_EVENTTYPE type ;
        XW_EXT_WINDOW *awindow ;
        int     modifier ;      /* Special Key code */
        int     x,y ;           /* Mouse position */
} XW_MOUSEMOVEEVENT ;

typedef struct {
        XW_EVENTTYPE type ;
        XW_EXT_WINDOW *awindow ;
        int     modifier ;      /* Special Key code */
        int     button ;        /* Button number */
        int     state ;         /* Button state : True if Press */
        int     x,y ;           /* Mouse position */
} XW_MOUSEMOVEWITHBUTTONEVENT ;

typedef struct {
        XW_EVENTTYPE type ;
        XW_EXT_WINDOW *awindow ;
        int     modifier ;          /* Special Key code */
        int     keycode ;           /* Function Key code */
        int     state ;             /* Key state : True if Press */
        char    *string ;
        int     x,y ;               /* Mouse position */
} XW_KEYBOARDEVENT ;

typedef struct {
        XW_EVENTTYPE type ;
        XW_EXT_WINDOW *awindow ;
        int     xc,yc ;         /* Exposure center position */
        int     width,height ;  /* Exposure size */
} XW_WINDOWEXPOSEEVENT ;

typedef struct {
        XW_EVENTTYPE type ;
        XW_EXT_WINDOW *awindow ;
        int     xc,yc ;         /* Window center position */
        int     width,height ;  /* Window size */
} XW_WINDOWRESIZEEVENT ;

typedef struct {
        XW_EVENTTYPE type ;
        XW_EXT_WINDOW *awindow ;
        int     xc,yc ;         /* Window center position */
} XW_WINDOWMOVEEVENT ;

typedef struct {
        XW_EVENTTYPE type ;
        XW_EXT_WINDOW *awindow ;
} XW_WINDOWICONIFYEVENT ;

typedef struct {
        XW_EVENTTYPE type ;
        XW_EXT_WINDOW *awindow ;
} XW_WINDOWENTEREVENT ;

typedef union {
        XW_EVENTTYPE type ;
        XW_ANYEVENT  any ;
        XW_MOUSEBUTTONEVENT mousebutton ;
        XW_MOUSEMOVEEVENT mousemove ;
        XW_MOUSEMOVEWITHBUTTONEVENT mousemovewithbutton ;
        XW_KEYBOARDEVENT keyboard ;
        XW_WINDOWEXPOSEEVENT windowexpose ;
        XW_WINDOWRESIZEEVENT windowresize ;
        XW_WINDOWMOVEEVENT windowmove ;
        XW_WINDOWICONIFYEVENT windowiconify ;
        XW_WINDOWENTEREVENT windowenter ;
} XW_EVENT ;

typedef enum {
        XW_WITHOUT_CURSOR,XW_RUBBERLINE_CURSOR,
        XW_RUBBERBAND_CURSOR,XW_USERDEFINED_CURSOR
} XW_CURSORTYPE ;

typedef enum {
        XW_TOR_UNKNOWN,XW_NO_BORDER,XW_TOP_BORDER,
        XW_RIGHT_BORDER,XW_BOTTOM_BORDER,XW_LEFT_BORDER,
        XW_TOP_AND_RIGHT_BORDER,XW_RIGHT_AND_BOTTOM_BORDER,
        XW_BOTTOM_AND_LEFT_BORDER,XW_LEFT_AND_TOP_BORDER
} XW_RESIZETYPE ;

typedef enum {
        XW_ERROR,XW_SUCCESS
} XW_STATUS ;

#define XW_EXTENDEDTEXT_XSCALE 1
#define XW_EXTENDEDTEXT_YSCALE 2
#define XW_EXTENDEDTEXT_SLANT 4
#define XW_EXTENDEDTEXT_WIDTH 8
#define XW_EXTENDEDTEXT_HEIGHT 16
#define XW_EXTENDEDTEXT_MINHEIGHT 32
#define XW_EXTENDEDTEXT_UNDERLINE 64

#define XW_EXTENDEDTEXT_OUTLINE 256
#define XW_EXTENDEDTEXT_COMPOSITE 512

typedef struct {
        int     type ;
        float   xscale,yscale ;
        float   slant ;
        float   spacing ;
        float   width,height,minheight ;
        int     isUnderlined ;
} XW_EXTENDEDTEXT ;

typedef XW_STATUS (*XW_CALLBACK)(
#ifdef XW_PROTOTYPE         
                XW_EVENT* anyevent      ,
                XW_USERDATA* userdata
#endif
                                ) ;

/*
   Trace & Errors
*/

        void Xw_set_trace       (
#ifdef XW_PROTOTYPE         
                int             TraceLevel      ,
                int             ErrorLevel
#endif
                                ) ;

        char* Xw_get_error      (
#ifdef XW_PROTOTYPE         
                int*            Code ,
                int*            Gravity 
#endif
                                ) ;

        XW_STATUS Xw_print_error        (
                                ) ;

/* 
   Attributes selection 
*/

        XW_ATTRIB Xw_set_line_attrib            (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW* window_extension ,
                int             color   ,
                int             type    ,
                int             width   ,
                XW_DRAWMODE     mode
#endif
                                                );

        XW_ATTRIB Xw_get_line_attrib            (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW* window_extension ,
                int             *color  ,
                int             *type   ,
                int             *width  ,
                XW_DRAWMODE     *mode
#endif
                                                );


        XW_ATTRIB Xw_set_poly_attrib            (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             color   ,
                int             type    ,
                int             tile    ,
                XW_DRAWMODE     mode
#endif
                                                );

        XW_ATTRIB Xw_get_poly_attrib            (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             *color  ,
                int             *type   ,
                int             *tile   ,
                XW_DRAWMODE     *mode
#endif
                                                );


        XW_ATTRIB Xw_set_text_attrib            (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             color   ,
                int             type    ,
                int             font    ,
                XW_DRAWMODE     mode
#endif
                                                );

        XW_ATTRIB Xw_get_text_attrib            (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             *color  ,
                int             *type   ,
                int             *font   ,
                XW_DRAWMODE     *mode
#endif
                                                );

        XW_ATTRIB Xw_set_extended_text_attrib           (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             color   ,
                int             font    ,
                XW_DRAWMODE     mode    ,
                XW_EXTENDEDTEXT *attribs
#endif
                                                );

        XW_ATTRIB Xw_set_marker_attrib          (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             color   ,
                int             type    ,
                int             width   ,
                XW_DRAWMODE     mode
#endif
                                                );

        XW_ATTRIB Xw_get_marker_attrib          (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             *color  ,
                int             *type   ,
                int             *width  ,
                XW_DRAWMODE     *mode
#endif
                                                );

/*
    Output primitives 
*/

        void Xw_set_clipping                    (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             flag    
#endif
                                                );

        int Xw_get_clipping                     (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_draw_line                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             npoint  ,
                float*          x       ,
                float*          y
#endif
                                                );

        XW_STATUS Xw_begin_line                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             npoint
#endif
                                                );

        XW_STATUS Xw_line_point                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                float           x       ,
                float           y
#endif
                                                );

        XW_STATUS Xw_close_line                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_draw_segment                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                float           x1              ,
                float           y1              ,
                float           x2              ,
                float           y2
#endif
                                                );

        XW_STATUS Xw_draw_segments                      (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             nsegments   ,
                float*          x1              ,
                float*          y1              ,
                float*          x2              ,
                float*          y2
#endif
                                                );

        XW_STATUS Xw_begin_segments                     (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             nsegment
#endif
                                                );

        XW_STATUS Xw_close_segments                     (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_draw_arc                   (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                float           xc              ,
                float           yc              ,
                float           xradius         ,
                float           yradius         ,
                float           start_angle     ,
                float           angle
#endif
                                                );

        XW_STATUS Xw_begin_arcs                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             narc
#endif
                                                );

        XW_STATUS Xw_close_arcs                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_draw_polyarc                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                float           xc              ,
                float           yc              ,
                float           xradius         ,
                float           yradius         ,
                float           start_angle     ,
                float           angle
#endif
                                                );

        XW_STATUS Xw_begin_polyarcs                     (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             narc
#endif
                                                );

        XW_STATUS Xw_close_polyarcs                     (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );


        XW_STATUS Xw_draw_poly                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             npoint  ,
                float*          x       ,
                float*          y
#endif
                                                );

        XW_STATUS Xw_begin_poly                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             npoint          ,
                int             npath
#endif
                                                );

        XW_STATUS Xw_poly_point                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                float           x       ,
                float           y
#endif
                                                );

        XW_STATUS Xw_close_poly                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_close_path                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_draw_text                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                float           x               , 
                float           y               ,
                char*           text            ,
                float           angle           ,
                int             mode
#endif
                                                );

        XW_STATUS Xw_draw_polytext                      (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                float           x               , 
                float           y               ,
                char*           text            ,
                float           angle           ,
                float           marge           ,
                int             mode
#endif
                                                );
        
        XW_STATUS Xw_get_text_size              (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW* window_extension ,
                int             Index   ,
                char*           string  ,
                float*          width   ,
                float*          height  ,
                float*          xoffset ,
                float*          yoffset
#endif
                                                );

        XW_STATUS Xw_draw_marker                        (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             Index   ,
                float           x       ,
                float           y       ,
                float           width   ,
                float           height  ,
                float           angle
#endif
                                                );

        XW_STATUS Xw_begin_markers                      (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             nmarker
#endif
                                                );

        XW_STATUS Xw_close_markers                      (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_draw_point                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                float           x       ,
                float           y       
#endif
                                                );

        XW_STATUS Xw_begin_points                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             npoint
#endif
                                                );

        XW_STATUS Xw_close_points                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_open_buffer                        (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension ,
                int             BufferId,
                float           Xpivot,
                float           Ypivot,
                int             widthindex,
                int             colorindex,
                int             fontindex,
                XW_DRAWMODE     drawmode
#endif
                                                );

        XW_STATUS Xw_close_buffer                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension ,
                int             BufferId
#endif
                                                );

        XW_STATUS Xw_set_buffer                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension ,
                int             BufferId
#endif
                                                );

        XW_STATUS Xw_draw_buffer                        (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension ,
                int             BufferId
#endif
                                                );

        XW_STATUS Xw_erase_buffer                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension ,
                int             BufferId
#endif
                                                );

        XW_STATUS Xw_move_buffer                        (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension ,
                int             BufferId,
                float           Xpivot,
                float           Ypivot
#endif
                                                );

        XW_STATUS Xw_scale_buffer                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension ,
                int             BufferId,
                float           Sx,
                float           Sy
#endif
                                                );

        XW_STATUS Xw_rotate_buffer                      (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension ,
                int             BufferId,
                float           Angle
#endif
                                                );

        XW_STATUS Xw_clear_buffer                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension ,
                int             BufferId
#endif
                                                );


        XW_STATUS Xw_get_buffer_info            (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension ,
                int     BufferId,
                float*  xpivot,
                float*  ypivot,
                float*  xscale,
                float*  yscale,
                float*  angle
#endif
                                                );

        XW_STATUS Xw_get_buffer_status          (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension ,
                int     BufferId,
                int*    isOpen,
                int*    isDrawn,
                int*    isEmpty
#endif
                                                );

/* 
   Window Miscellaneous
*/

        XW_STATUS Xw_def_background_color                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                float           r       ,
                float           g       ,
                float           b 
#endif
                                                );

        XW_STATUS Xw_set_background_index                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             index
#endif
                                                );

        XW_STATUS Xw_get_background_index                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int*            index
#endif
                                                );

        XW_STATUS Xw_get_background_color       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                float*          r       ,
                float*          g       ,
                float*          b 
#endif
                                                );
        XW_STATUS Xw_get_background_pixel                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW* window_extension ,
                unsigned long*          pixel
#endif
                                                );

        XW_STATUS Xw_open_pixmap                (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_close_pixmap               (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_open_background_pixmap     (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_close_background_pixmap    (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_set_double_buffer          (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                XW_DOUBLEBUFFERMODE     state
#endif
                                                );

        XW_DOUBLEBUFFERMODE Xw_get_double_buffer                (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_erase_window                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_erase_area                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             Xcenter         ,
                int             Ycenter         ,
                int             width   ,
                int             height
#endif
                                                );

        XW_RESIZETYPE Xw_resize_window                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_restore_area                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             Xcenter ,
                int             Ycenter ,
                int             width   ,
                int             height
#endif
                                                );

        XW_STATUS Xw_restore_window                             (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_flush                      (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension,
                int             synchronize
#endif
                                                );

        XW_EXT_WINDOW* Xw_def_window            (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display_extension       ,
                Aspect_Handle          XWindowID,
                int             istransparent
#endif
                                                );

        XW_STATUS Xw_isdefine_window            (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        Aspect_Handle Xw_open_window                   (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display_extension       ,
                Xw_TypeOfVisual pclass  ,
                Aspect_Handle          parent  ,
                float           xc      ,
                float           yc      ,
                float           width   ,
                float           height  ,
                char*           title   ,
                int             istransparent  
#endif
                                                );

        XW_STATUS Xw_close_window               (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension,
                int             destroy
#endif
                                                );

        XW_STATUS Xw_set_window_state           (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                XW_WINDOWSTATE  state
#endif
                                                );

        XW_STATUS Xw_set_window_ratio           (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                float           ratio
#endif
                                                );

        XW_STATUS Xw_set_window_position                (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int             Xcenter ,
                int             Ycenter ,
                int             width   ,
                int             height
#endif
                                                );

        XW_WINDOWSTATE Xw_get_window_position           (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int*            Xcenter ,
                int*            Ycenter ,
                int*            width   ,
                int*            height
#endif
                                                );

        XW_WINDOWSTATE Xw_get_window_state              (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        XW_STATUS Xw_get_window_size            (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int*            width   ,
                int*            height
#endif
                                                );

        Xw_TypeOfVisual Xw_get_window_visual    (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension
#endif
                                                );

        float Xw_get_window_pixelvalue          (
#ifdef XW_PROTOTYPE
                XW_EXT_WINDOW*  window_extension     ,
                int             Pv
#endif
                                                );

        XW_STATUS Xw_get_window_pixelcoord         (
#ifdef XW_PROTOTYPE
                XW_EXT_WINDOW*  window_extension     ,
                int             Px  ,
                int             Py  ,
                float*          Dx  ,
                float*          Dy
#endif
                                                );

        XW_STATUS Xw_get_window_info    (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension     ,
                Aspect_Handle*         window  ,
                Aspect_Handle*         pixmap  ,
                Aspect_Handle*         root    ,
                Aspect_Handle*         colormap,
                Xw_TypeOfVisual* pclass ,
                int*            depth   ,
                int*            visualid
#endif
                                                );

        XW_STATUS Xw_set_hard_cursor                    (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int     XcursorID       ,
                int     grab    ,
                float   red     ,
                float   green   ,
                float   blue
#endif
                                                );

        XW_STATUS Xw_set_soft_cursor                    (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                XW_CURSORTYPE   cursor  ,
                int             button 
#endif
                                                );

        XW_STATUS Xw_get_cursor_position        (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                int*    Xpos    ,
                int*    Ypos    
#endif
                                                );

/*
   Images Miscellaneous
*/

        XW_EXT_IMAGEDATA* Xw_open_image                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                XW_USERDATA*    aimageinfo,
                int             width   ,
                int             height
#endif
                                                );

        XW_STATUS Xw_isdefine_image             (
#ifdef XW_PROTOTYPE         
                XW_EXT_IMAGEDATA*       image_extension
#endif
                                                );

        XW_STATUS Xw_close_image                        (
#ifdef XW_PROTOTYPE         
                XW_EXT_IMAGEDATA*       image_extension
#endif
                                                );

        XW_STATUS Xw_draw_image                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension    ,
                XW_EXT_IMAGEDATA*       image_extension ,
                float           Xcenter ,
                float           Ycenter
#endif
                                                );

XW_STATUS Xw_draw_zoomed_image
(
#ifdef XW_PROTOTYPE         
  XW_EXT_WINDOW*    window_extension,
  XW_EXT_IMAGEDATA* image_extension,
  float             Xcenter,
  float             Ycenter,
  float             zoom
#endif
);

        XW_STATUS Xw_zoom_image                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_IMAGEDATA*       image_extension ,
                float           zoom    
#endif
                                                );

        XW_STATUS Xw_gamma_image                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_IMAGEDATA*       image_extension ,
                float           gamma    
#endif
                                                );


        XW_STATUS Xw_put_background_image       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension    ,
                XW_EXT_IMAGEDATA*       image_extension ,
		Aspect_FillMethod	method
#endif
                                                );

	XW_STATUS Xw_update_background_image	(
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension 
#endif
                                                );

        XW_STATUS Xw_set_background_pixmap      ( 
#ifdef XW_PROTOTYPE 
                XW_EXT_WINDOW * awindow, 
                Aspect_Handle apixmap
#endif
                                                );

        XW_STATUS Xw_get_background_pixmap      ( 
#ifdef XW_PROTOTYPE 
                XW_EXT_WINDOW * awindow, 
                Aspect_Handle& apixmap 
#endif
                                                );

        XW_STATUS Xw_clear_background_image     (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension   
#endif
                                                );

        XW_EXT_IMAGEDATA* Xw_get_image                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension    ,
                XW_USERDATA*    aimageinfo,
                int             Xcenter ,
                int             Ycenter ,
                int             width   ,
                int             height
#endif
                                                );

        XW_EXT_IMAGEDATA* Xw_get_image_from_pixmap (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension    ,
                XW_USERDATA*    aimageinfo,
		Aspect_Handle   pixmapID,
                int             Xcenter ,
                int             Ycenter ,
                int             width   ,
                int             height
#endif
                                                );

        XW_STATUS Xw_get_background_image       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension    ,
                int             Xcenter ,
                int             Ycenter ,
                int             width   ,
                int             height
#endif
                                                );

        XW_EXT_IMAGEDATA* Xw_get_image_handle   (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension    ,
                XW_USERDATA*    aimageinfo
#endif
                                                );

        XW_EXT_IMAGEDATA* Xw_load_image                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension    ,
                XW_USERDATA*            aimageinfo      ,
                char*           FileName
#endif
                                                );

        XW_STATUS Xw_convert_image                      (
#ifdef XW_PROTOTYPE
                XW_EXT_WINDOW*  window_extension    ,
                XW_EXT_IMAGEDATA*       image_extension,
                XW_EXT_COLOR*   color_extension,
                int             ncolors                
#endif
                                                );

        XW_STATUS Xw_get_image_info                     (
#ifdef XW_PROTOTYPE         
                XW_EXT_IMAGEDATA*       image_extension,
                float*          zoom    ,
                int*            width   ,
                int*            height  ,
                int*            depth
#endif
                                                );

        XW_STATUS Xw_save_image                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension    ,
                XW_EXT_IMAGEDATA*       image_extension ,
                char*           FileName
#endif
                                                );

        XW_STATUS Xw_put_pixel                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_IMAGEDATA*       image_extension ,
                int             x               ,
                int             y               ,
                int             index   ,
                int             npixel
#endif
                                                );

        XW_STATUS Xw_put_rgbpixel                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_IMAGEDATA*       image_extension ,
                int             x               ,
                int             y               ,
                float           r       ,
                float           g       ,
                float           b       ,
                int             npixel
#endif
                                                );

        XW_STATUS Xw_get_pixel                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_IMAGEDATA*       image_extension ,
                int             x               ,
                int             y               ,
                int*            index   ,
                int*            npixel
#endif
                                                );

        XW_STATUS Xw_get_rgbpixel                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_IMAGEDATA*       image_extension ,
                int             x               ,
                int             y               ,
                float*          r               ,
                float*          g       ,
                float*          b       ,
                int*            npixel
#endif
                                                );

        XW_STATUS Xw_alloc_pixel                        (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP*        colormap_extension      ,
                unsigned long   *pixel  
#endif
                                                );

        XW_STATUS Xw_free_pixel                 (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP*        colormap_extension      ,
                unsigned long   pixel   
#endif
                                                );

        XW_STATUS Xw_set_pixel                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP*        colormap_extension      ,
                unsigned long   pixel   ,
                float           r       ,
                float           g       ,
                float           b               
#endif
                                                );

        XW_STATUS Xw_set_pixel                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP*        colormap_extension      ,
                unsigned long   pixel   ,
                float           r       ,
                float           g       ,
                float           b               
#endif
                                                );

/*
    Display miscellaneous
*/

        XW_EXT_DISPLAY* Xw_set_display          (
#ifdef XW_PROTOTYPE         
                void*    	pDisplay       
#endif
                                                );

        XW_EXT_DISPLAY* Xw_open_display         (
#ifdef XW_PROTOTYPE         
                char*           connexion       
#endif
                                                );

        char* Xw_get_display_name               (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* adisplay  
#endif
                                                );

        int Xw_get_plane_layer                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* adisplay        ,
                int     visualid
#endif
                                                );

        XW_EXT_DISPLAY* Xw_get_display          (
#ifdef XW_PROTOTYPE         
                char*           connexion       
#endif
                                                );

        XW_STATUS Xw_close_display              (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* adisplay        
#endif
                                                );

        XW_STATUS Xw_get_screen_size            (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* adisplay        ,
                int*            width   ,
                int*            height
#endif
                                                );

        XW_STATUS Xw_get_mmscreen_size          (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* adisplay        ,
                float*          width   ,
                float*          height
#endif
                                                );

        XW_STATUS Xw_get_screen_pixelcoord      (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display ,
                int             px              ,
                int             py              ,
                float*          sx              ,
                float*          sy
#endif
                                                );

        XW_STATUS Xw_get_pixel_screencoord      (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display ,
                float           sx              ,
                float           sy              ,
                int*            px              ,
                int*            py 
#endif
                                                );

        float Xw_get_screen_pixelvalue          (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display ,
                int             pv
#endif
                                                );

        int Xw_get_pixel_screenvalue            (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display ,
                float           sv
#endif
                                                );

        float Xw_get_mmscreen_pixelvalue        (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display ,
                int             pv 
#endif
                                                );

        int Xw_get_pixel_mmscreenvalue          (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* adisplay        ,
                float           mv 
#endif
                                                );

        XW_STATUS Xw_isdefine_display           (
#ifdef XW_PROTOTYPE
                XW_EXT_DISPLAY*         display_extension
#endif
                                                );

#if defined(XW_PROTOTYPE) && ( defined(__cplusplus) || defined(c_plusplus) )
extern "C" {
#endif
        XW_STATUS Xw_get_display_info           (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display_extension       ,
                Aspect_Handle**        display         ,
                Aspect_Handle*         root    ,
                Aspect_Handle*         colormap        ,
                Xw_TypeOfVisual*        pclass  ,
                int*            depth
#endif
                                                );
#if defined(XW_PROTOTYPE) && ( defined(__cplusplus) || defined(c_plusplus) )
      }
#endif

        Xw_TypeOfVisual Xw_get_display_visual   (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY*  display_extension      ,
                Xw_TypeOfVisual  visual
#endif
                                                );
/*
    IconBox
*/

        int Xw_load_icons       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  iconbox_extension       ,
                char*           filename
#endif
                                                );

        int Xw_clear_icons      (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  iconbox_extension       ,
                char*           filename
#endif
                                                );

        XW_STATUS Xw_show_icons (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  iconbox_extension       
#endif
                                                );

        int Xw_save_icons       (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  iconbox_extension
#endif
                                                );

        Aspect_Handle Xw_get_icon_pixmap        (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  iconbox_extension       ,
                int             width   ,
                int             height  ,
                char*           iconname
#endif
                                                );

        XW_STATUS Xw_get_icon_size              (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  iconbox_extension       ,
                char*           iconname,
                int*            width   ,
                int*            height  
#endif
                                                );

        char* Xw_get_icon_name          (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  iconbox_extension       ,
                int             index   
#endif
                                                );

        XW_STATUS Xw_put_window_icon    (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  iconbox_extension       ,
                XW_EXT_WINDOW*  window_extension        ,
                char*           iconname        ,
                int             width   ,
                int             height
#endif
                                                );


/*
    Colormap & Color attributs definition 
*/

        XW_EXT_COLORMAP* Xw_def_colormap        (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display_extension       ,
                Xw_TypeOfVisual     pclass      , 
                int             ncolor  ,
                Aspect_Handle          BaseMap ,
                Xw_TypeOfMapping mapping
#endif
                                                );

        XW_STATUS Xw_close_colormap             (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension     
#endif
                                                );

        XW_STATUS Xw_set_colormap_mapping       (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension,
                Xw_TypeOfMapping mapping
#endif
                                                );

        XW_STATUS Xw_isdefine_colormap          (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension
#endif
                                                );

        XW_STATUS Xw_set_colormap               (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                XW_EXT_COLORMAP* colormap_extension
#endif
                                                );

        Xw_TypeOfVisual Xw_get_colormap_visual  (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension
#endif
                                                );

        Aspect_Handle Xw_get_colormap_xid       (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension
#endif
                                                );

        XW_STATUS Xw_get_colormap_colorcube     (
#ifdef XW_PROTOTYPE
                XW_EXT_COLORMAP* colormap_extension,
                Aspect_Handle *colormap,
                int *visualid,
                int *base_pixel,
                int *red_max,
                int *red_mult,
                int *green_max,
                int *green_mult,
                int *blue_max,
                int *blue_mult
#endif
                                                );

        XW_STATUS Xw_get_colormap_grayramp      (
#ifdef XW_PROTOTYPE
                XW_EXT_COLORMAP* colormap_extension,
                Aspect_Handle *colormap,
                int *visualid,
                int *base_pixel,
                int *gray_max,
                int *gray_mult
#endif
                                                );

        XW_STATUS Xw_get_colormap_info          (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension     ,
                Aspect_Handle**        visual,
                Xw_TypeOfVisual* pclass ,
                int*            visualid,
                int*            maxcolor        ,
                int*            systemcolor ,
                int*            usercolor ,
                int*            definecolor ,
                int*            firstfreecolorindex
#endif
                                                );

        XW_STATUS Xw_def_color                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension     ,
                int              index  ,
                float           r               ,
                float           g               ,
                float           b 
#endif
                                                );

        XW_STATUS Xw_def_highlight_color        (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension     ,
                float           r               ,
                float           g               ,
                float           b 
#endif
                                                );

        XW_STATUS Xw_get_color                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension     ,
                int             index   ,
                float*          r               ,
                float*          g               ,
                float*          b               ,
                unsigned long* pixel
#endif
                                                );

        XW_STATUS Xw_get_color_index            (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension     ,
                float           r               ,
                float           g               ,
                float           b               ,
                int*            index
#endif
                                                );

        XW_STATUS Xw_get_color_pixel            (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension     ,
                float           r               ,
                float           g               ,
                float           b               ,
                unsigned long*          pixel   ,
                int*            isapproximate
#endif
                                                );

        XW_STATUS Xw_get_color_name             (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension     ,
                const char*     name    ,
                float*          r               ,
                float*          g               ,
                float*          b 
#endif
                                                );

        XW_STATUS Xw_get_highlight_color        (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension     ,
                float*          r               ,
                float*          g               ,
                float*          b 
#endif
                                                );

        XW_STATUS Xw_get_highlight_pixel        (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension     ,
                unsigned long*          pixel 
#endif
                                                );

        XW_STATUS Xw_isdefine_color             (
#ifdef XW_PROTOTYPE         
                XW_EXT_COLORMAP* colormap_extension     ,
                int             index 
#endif
                                                );
/*
    Widthmap & Width attributs definition 
*/

        XW_EXT_WIDTHMAP* Xw_def_widthmap        (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display_extension       ,
                int             nwidth
#endif
                                                );

        XW_STATUS Xw_close_widthmap             (
#ifdef XW_PROTOTYPE         
                XW_EXT_WIDTHMAP* widthmap_extension     
#endif
                                                );

        XW_STATUS Xw_isdefine_widthmap          (
#ifdef XW_PROTOTYPE         
                XW_EXT_WIDTHMAP* widthmap_extension
#endif
                                                );

        XW_STATUS Xw_set_widthmap               (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW* window_extension ,
                XW_EXT_WIDTHMAP* widthmap_extension
#endif
                                                );

        XW_STATUS Xw_get_widthmap_info          (
#ifdef XW_PROTOTYPE         
                XW_EXT_WIDTHMAP* widthmap_extension     ,
                int*            maxwidth        ,
                int*            userwidth ,
                int*            definewidth ,
                int*            firstfreewidthindex
#endif
                                                );

        XW_STATUS Xw_def_width                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_WIDTHMAP* widthmap_extension     ,
                int             index   ,
                float           MMwidth
#endif
                                                );

        XW_STATUS Xw_get_width_index            (
#ifdef XW_PROTOTYPE         
                XW_EXT_WIDTHMAP* widthmap_extension     ,
                float           Width   ,
                int*            index
#endif
                                                );

        XW_STATUS Xw_get_width                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_WIDTHMAP* widthmap_extension     ,
                int             index   ,
                float*          MMwidth
#endif
                                                );

        XW_STATUS Xw_isdefine_width             (
#ifdef XW_PROTOTYPE         
                XW_EXT_WIDTHMAP* widthmap_extension     ,
                int             index 
#endif
                                                );

/*
    Typemap & Type attributs definition 
*/

        XW_EXT_TYPEMAP* Xw_def_typemap          (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display_extension       ,
                int             ntype
#endif
                                                );

        XW_STATUS Xw_close_typemap              (
#ifdef XW_PROTOTYPE         
                XW_EXT_TYPEMAP* typemap_extension       
#endif
                                                );

        XW_STATUS Xw_isdefine_typemap           (
#ifdef XW_PROTOTYPE         
                XW_EXT_TYPEMAP* typemap_extension
#endif
                                                );

        XW_STATUS Xw_set_typemap                (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                XW_EXT_TYPEMAP* typemap_extension
#endif
                                                );

        XW_STATUS Xw_get_typemap_info           (
#ifdef XW_PROTOTYPE         
                XW_EXT_TYPEMAP* typemap_extension       ,
                int*            maxtype ,
                int*            usertype ,
                int*            definetype ,
                int*            firstfreetypeindex
#endif
                                                );

        XW_STATUS Xw_def_type                   (
#ifdef XW_PROTOTYPE         
                XW_EXT_TYPEMAP* typemap_extension       ,
                int             index   ,
                int             DescriptorLength        ,
                float*          TypeDescriptor
#endif
                                                );

        XW_STATUS Xw_get_type                   (
#ifdef XW_PROTOTYPE         
                XW_EXT_TYPEMAP* typemap_extension       ,
                int             index   ,
                int*            DescriptorLength        ,
                float*          TypeDescriptor
#endif
                                                );

        XW_STATUS Xw_get_type_index             (
#ifdef XW_PROTOTYPE         
                XW_EXT_TYPEMAP* typemap_extension       ,
                float*          TypeDescriptor  ,
                int             DescriptorLength        ,
                int*            index
#endif
                                                );

        XW_STATUS Xw_isdefine_type              (
#ifdef XW_PROTOTYPE         
                XW_EXT_TYPEMAP* typemap_extension       ,
                int             index 
#endif
                                                );

/*
    Fontmap & Font attributs definition 
*/

        XW_EXT_FONTMAP* Xw_def_fontmap          (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display_extension       ,
                int             nfont
#endif
                                                );

        XW_STATUS Xw_close_fontmap              (
#ifdef XW_PROTOTYPE         
                XW_EXT_FONTMAP* fontmap_extension       
#endif
                                                );

        XW_STATUS Xw_isdefine_fontmap           (
#ifdef XW_PROTOTYPE         
                  XW_EXT_FONTMAP* fontmap_extension
#endif
                                                );

        XW_STATUS Xw_set_fontmap                        (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                XW_EXT_FONTMAP* fontmap_extension
#endif
                                                );

        XW_STATUS Xw_get_fontmap_info                   (
#ifdef XW_PROTOTYPE         
                XW_EXT_FONTMAP* fontmap_extension       ,
                int*            maxfont ,
                int*            userfont ,
                int*            definefont ,
                int*            firstfreefontindex
#endif
                                                );

        XW_STATUS Xw_def_font                   (
#ifdef XW_PROTOTYPE         
                XW_EXT_FONTMAP* fontmap_extension       ,
                int             index   ,
                float           size    ,
                char*           FontName 
#endif
                                                );

        XW_STATUS Xw_get_font_index                     (
#ifdef XW_PROTOTYPE         
                XW_EXT_FONTMAP* fontmap_extension       ,
                float           size    ,
                char*           FontName        ,
                int*            index
#endif
                                                );


        char* Xw_get_font                       (
#ifdef XW_PROTOTYPE         
                XW_EXT_FONTMAP* fontmap_extension       ,
                int             index   ,
                float*          size    ,
                float*          bheight 
#endif
                                                );

        XW_STATUS Xw_isdefine_font                      (
#ifdef XW_PROTOTYPE         
                XW_EXT_FONTMAP* fontmap_extension       ,
                int             index 
#endif
                                                );

/*
    Tilemap & Tile attributs definition 
*/

        XW_EXT_TILEMAP* Xw_def_tilemap                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display_extension       ,
                int             ntile
#endif
                                                );

        XW_STATUS Xw_isdefine_tilemap           (
#ifdef XW_PROTOTYPE         
                  XW_EXT_TILEMAP* tilemap_extension
#endif
                                                );

        XW_STATUS Xw_set_tilemap                        (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                XW_EXT_TILEMAP* tilemap_extension
#endif
                                                );

        XW_STATUS Xw_get_tilemap_info                   (
#ifdef XW_PROTOTYPE         
                XW_EXT_TILEMAP* tilemap_extension       ,
                int*            maxtile ,
                int*            usertile ,
                int*            definetile ,
                int*            firstfreetileindex 
#endif
                                                );

        XW_STATUS Xw_def_tile                   (
#ifdef XW_PROTOTYPE         
                XW_EXT_TILEMAP* tilemap_extension       ,
                int             index   ,
                int             width   ,
                int             height  ,
                char*           tiledata
#endif
                                                );

        XW_STATUS Xw_isdefine_tile                      (
#ifdef XW_PROTOTYPE         
                XW_EXT_TILEMAP* tilemap_extension       ,
                int             index 
#endif
                                                );
/*
    Markmap & Marker attributs definition 
*/

        XW_EXT_MARKMAP* Xw_def_markmap                  (
#ifdef XW_PROTOTYPE         
                XW_EXT_DISPLAY* display_extension       ,
                int             nmarker
#endif
                                                );

        XW_STATUS Xw_close_markmap                      (
#ifdef XW_PROTOTYPE         
                XW_EXT_MARKMAP* markmap_extension       
#endif
                                                );

        XW_STATUS Xw_isdefine_markmap           (
#ifdef XW_PROTOTYPE         
                  XW_EXT_MARKMAP* markmap_extension
#endif
                                                );

        XW_STATUS Xw_set_markmap                        (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW*  window_extension        ,
                XW_EXT_MARKMAP* markmap_extension
#endif
                                                );

        XW_STATUS Xw_get_markmap_info                   (
#ifdef XW_PROTOTYPE         
                XW_EXT_MARKMAP* markmap_extension       ,
                int*            maxmarker ,
                int*            usermarker ,
                int*            definemarker ,
                int*            firstfreemarkerindex
#endif
                                                );


        XW_STATUS Xw_def_marker                         (
#ifdef XW_PROTOTYPE         
                XW_EXT_MARKMAP* markmap_extension       ,
                int             index   ,
                int             length  ,
                int*            spoint  ,
                float*          xpoint  ,
                float*          ypoint  
#endif
                                                );

        XW_STATUS Xw_isdefine_marker                    (
#ifdef XW_PROTOTYPE         
                XW_EXT_MARKMAP* markmap_extension       ,
                int             index 
#endif
                                                );

        XW_STATUS Xw_isdefine_marker                    (
#ifdef XW_PROTOTYPE         
                XW_EXT_MARKMAP* markmap_extension       ,
                int             index 
#endif
                                                );

        XW_STATUS Xw_get_marker_index                   (
#ifdef XW_PROTOTYPE         
                XW_EXT_MARKMAP* markmap_extension       ,
                int             DescriptorLength        ,
                int*            sVector ,
                float*          xVector ,
                float*          yVector ,
                int*            index
#endif
                                                );
/*
    Event  management
*/

        XW_STATUS Xw_set_event          (
#ifdef XW_PROTOTYPE         
                XW_EXT_WINDOW* window_extension ,
                XW_EVENTTYPE    type    ,
                XW_CALLBACK     call_back_routine,
                XW_USERDATA*    user_data
#endif
                                                );

        XW_STATUS Xw_wait_event                 (
#ifdef XW_PROTOTYPE       
                XW_EXT_WINDOW* window_extension
#endif
                                                );

/* 
    Mapping 
*/


        XW_STATUS Xw_get_pixel_windowcoord              (
#ifdef XW_PROTOTYPE         
                  XW_EXT_WINDOW* window_extension       ,
                  float         ux              ,
                  float         uy              ,
                  int*          px              ,
                  int*          py 
#endif
                                                );

        int Xw_get_pixel_windowvalue            (
#ifdef XW_PROTOTYPE         
                  XW_EXT_WINDOW* window_extension       ,
                  float         uv
#endif
                                                );

/*
   Environment
*/

        XW_STATUS Xw_get_env(
#ifdef XW_PROTOTYPE         
                const char* symbol,
                char* value,
                int length
#endif
        );

#endif /*XW_CEXTERN*/
