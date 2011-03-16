
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#ifndef TRACE
//#define TRACE
#endif

#include <Xw_Cextern.hxx>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/XWDFile.h>

#ifndef min
#define min(a,b) 	(a>b ? b : a)
#endif

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_X11_EXTENSIONS_TRANSOLV_H
#include <X11/extensions/transovl.h>
#endif

#ifndef max
#define max(a,b) 	(a<b ? b : a)
#endif

#ifndef PI
#define PI 3.14159
#endif

#define DRAD (PI/180.)

#define MAXLINES 256
#define MAXPOLYS 256
#define MAXPOINTS 1024
#define MAXSEGMENTS 1024
#define MAXARCS 1024
#define MAXLTEXTS 256
#define MAXPTEXTS 256
#define MAXCHARS 1024
#define MAXIMAGES 8
#define MAXLMARKERS 256
#define MAXPMARKERS 256
#define MAXBUFFERS 8

#define MAXQG 32

#define MAXTYPE 256
#define MAXCOLOR 512
#define MINCOLOR 32
#define FRECOLOR 14
#define MAXWIDTH 256
#define MAXFONT 256
#define MAXTILE 256
#define MAXMARKER 256
#define MAXEVENT XW_ALLEVENT+1
#define MAXCURSOR 200
#define MAXANGLE     23040
#define MINTEXTSIZE 8

#define _ATTRIBUTES   	pwindow->attributes
#define _WINDOW  	pwindow->window
#define _PIXMAP  	pwindow->pixmap
#define _NWBUFFER       pwindow->nwbuffer
#define _FWBUFFER       pwindow->wbuffers[0]
#define _BWBUFFER       pwindow->wbuffers[1]
#define _DRAWABLE 	pwindow->drawable
#define _COLORMAP 	(pwindow->pcolormap)
#define _WIDTHMAP 	(pwindow->pwidthmap)
#define _TYPEMAP 	(pwindow->ptypemap)
#define _FONTMAP 	(pwindow->pfontmap)
#define _TILEMAP 	(pwindow->ptilemap)
#define _MARKMAP 	(pwindow->pmarkmap)
#define _ROOT  		(_ATTRIBUTES.root)
#define _SCREEN  	(_ATTRIBUTES.screen)
#define _VISUAL  	(_ATTRIBUTES.visual)
#if defined(__cplusplus) || defined(c_plusplus)
#define _CLASS 		(_VISUAL->c_class)
#else
#define _CLASS 		(_VISUAL->class)
#endif
#ifdef _X
#undef _X
#endif
#define _X	 	(_ATTRIBUTES.x)
#ifdef _Y
#undef _Y
#endif
#define _Y	 	(_ATTRIBUTES.y)
#define _WIDTH	 	(_ATTRIBUTES.width)
#define _HEIGHT	 	(_ATTRIBUTES.height)
#define _STATE	 	(_ATTRIBUTES.map_state)
#define _DEPTH	 	(_ATTRIBUTES.depth)
#define _EVENT_MASK	(_ATTRIBUTES.your_event_mask)

#define QGCODE(color,type,width,mode) ((color<<20)|(type<<12)|(width<<4)|mode) 
#define QGCOLOR(code) ((code>>20)&0xFFF)
#define QGSETCOLOR(code,color)	((code&0x000FFFFF)|(color<<20))
#define QGTYPE(code)  ((code>>12)&0xFF)
#define QGSETTYPE(code,type)	((code&0xFFF00FFF)|(type<<12))
#define QGWIDTH(code) ((code>> 4)&0xFF)
#define QGSETWIDTH(code,width)	((code&0xFFFFF00F)|(width<<4))
#define QGTILE(code) ((code>> 4)&0xFF)
#define QGSETTILE(code,mark)	((code&0xFFFFF00F)|(mark<<4))
#define QGFONT(code) ((code>> 4)&0xFF)
#define QGSETFONT(code,font)	((code&0xFFFFF00F)|(font<<4))
#define QGMODE(code)  (code&0xF)
#define QGSETMODE(code,mode)  ((code&0xFFFFFFF0)|mode)

#define ROUND(v) ( ((v) > 0.) ? ((v) < (float)(INT_MAX-1)) ? \
		 (int)((v)+.5) : INT_MAX : ((v) > (float)(INT_MIN+1)) ? \
		 (int)((v)-.5) : INT_MIN )
#ifdef OLD	/*S3593 OPTIMISATION*/
#define PXPOINT(x) (ROUND((x) / pwindow->xratio))
#define PYPOINT(y) (ROUND(_HEIGHT - ((y) / pwindow->yratio)))
#define PVALUE(v) (ROUND((v) / ((pwindow->xratio+pwindow->yratio)/2.)))
#define UVALUE(v) ((float)(v) * (pwindow->xratio+pwindow->yratio)/2.)
#else
#define PXPOINT(x) ((int)((x) / pwindow->xratio))
#define PYPOINT(y) ((int)(_HEIGHT - ((y) / pwindow->yratio)))
#define PVALUE(v) ((int)((v) / ((pwindow->xratio+pwindow->yratio)/2.)))
#define UVALUE(v) ((float)(v) * (pwindow->xratio+pwindow->yratio)/2.)
#endif

//OCC186
#undef PXPOINT
#undef PYPOINT
#undef PVALUE
//OCC186

#define UXPOINT(x) ((float)(x) * pwindow->xratio)
#define UYPOINT(y) ((float)(_HEIGHT - (y)) * pwindow->yratio)

/* DISPLAY */

#define DISPLAY_TYPE   0
#ifdef XW_EXT_DISPLAY
#undef XW_EXT_DISPLAY
#endif
#define _DDISPLAY 	pdisplay->display
#define _DSCREEN 	(pdisplay->screen)
#define _DVISUAL 	(pdisplay->visual)

#if defined(__cplusplus) || defined(c_plusplus)
#define _DCLASS 	(pdisplay->visual->c_class)
#else
#define _DCLASS 	(pdisplay->visual->class)
#endif
#define _DCOLORMAP	pdisplay->colormap
#define _DGC		pdisplay->gc
#define _DROOT		pdisplay->rootwindow
#define _DGRAB		pdisplay->grabwindow
#define _DWIDTH		pdisplay->width
#define _DHEIGHT	pdisplay->height
#define MMPXVALUE(v) ((float)((v)*WidthMMOfScreen(_DSCREEN))/(float)_DWIDTH)
#define MMPYVALUE(v) ((float)((v)*HeightMMOfScreen(_DSCREEN))/(float)_DHEIGHT)
#define PMMXVALUE(v) ((int)((v)*_DWIDTH/(float)WidthMMOfScreen(_DSCREEN)))
#define PMMYVALUE(v) ((int)((v)*_DHEIGHT/(float)HeightMMOfScreen(_DSCREEN)))
typedef enum {
	XW_SERVER_IS_UNKNOWN,XW_SERVER_IS_DEC,XW_SERVER_IS_SUN,
	XW_SERVER_IS_SGI,XW_SERVER_IS_NEC,XW_SERVER_IS_HP
} XW_SERVER_TYPE ;

typedef struct {
    /* Avoid 64-bit portability problems by being careful to use
       longs due to the way XGetWindowProperty is specified. Note
       that these parameters are passed as CARD32s over X
       protocol. */
   
    long overlay_visual;
    long transparent_type;
    long value;
    long layer;
} XOverlayVisualInfo ;

typedef struct {
	void 	*link ;			 
	int	 type ;				/* DISPLAY_TYPE */
	XW_SERVER_TYPE	 server ;
	Display  *display ;
	Screen	 *screen ;
	Visual	 *visual ;
	GC	 gc ;
	int	 width ;
	int	 height ;
	Colormap colormap ;
	Window   rootwindow ;
	Window   grabwindow ;
	float	 gamma ;
	char*	 gname ;
} XW_EXT_DISPLAY;
/* COLORMAP */

#ifdef XW_EXT_COLORMAP
#undef XW_EXT_COLORMAP
#endif

#define COLORMAP_TYPE   2
#define _CDISPLAY 	(pcolormap->connexion->display)
#define _CSCREEN 	(pcolormap->connexion->screen)
#define _CCOLORMAP 	(pcolormap->connexion->colormap)
#define _CROOT 		(pcolormap->connexion->rootwindow)
#define _CVISUAL	(pcolormap->visual)
#if defined(__cplusplus) || defined(c_plusplus)
#define _CCLASS 	(pcolormap->visual->c_class)
#else
#define _CCLASS 	(pcolormap->visual->class)
#endif
#define _CINFO		(pcolormap->info)
#define _CGINFO		(pcolormap->ginfo)
#define FREECOLOR 0		/* Free color Index if define[] = 0 */
#define USERCOLOR  1		/* User color Index if define[] = 1 */ 
#define IMAGECOLOR 2		/* Image color Index if define[] = 2 */
#define HIGHCOLOR 3		/* HighLight color Index if define[] = 3 */
#define BACKCOLOR 4		/* Background color Index if define[] = 4 */
#define SYSTEMCOLOR 5		/* System color Index if define[] = 5 */

typedef struct {
	void	 *link ;
	int	 type ;				/* COLORMAP_TYPE */
	XW_EXT_DISPLAY* connexion ;
	Visual   *visual ;
	XStandardColormap info,ginfo ;
	int 	 maxhcolor ;			/* Colormap Hard Length */
	int 	 maxucolor ;			/* Colormap User Length */
	int 	 maxcolor ;			/* Colormap Length */
	int	 maxwindow ;			/* Number of reference Window*/
	Xw_TypeOfMapping mapping ;		/* Colormap mapping */
	unsigned char define[MAXCOLOR] ;	/* 0 if FREE color */
	unsigned long highpixel ;		/* HighLightPixel Value */
	unsigned long backpixel ;		/* TransparentPixel Value */
	unsigned long pixels[MAXCOLOR] ;
} XW_EXT_COLORMAP ;

/* WIDTHMAP */

#define WIDTHMAP_TYPE   3
#ifdef XW_EXT_WIDTHMAP
#undef XW_EXT_WIDTHMAP
#endif

#define _WDISPLAY 	(pwidthmap->connexion->display)

typedef struct {
	void	 *link ;
	int	 type ;				/* WIDTHMAP_TYPE */
	XW_EXT_DISPLAY* connexion ;
	int 	 maxwidth ;			/* Widthmap Length */
	int	 maxwindow ;			/* Number of reference Window */
	unsigned char widths[MAXWIDTH] ;	/* Pixel width > 0 if defined*/
} XW_EXT_WIDTHMAP ;

/* TYPEMAP */

#define TYPEMAP_TYPE   4
#ifdef XW_EXT_TYPEMAP
#undef XW_EXT_TYPEMAP
#endif

#define _TDISPLAY 	(ptypemap->connexion->display)

typedef struct {
	void	 *link ;
	int	 type ;				/* TYPEMAP_TYPE */
	XW_EXT_DISPLAY* connexion ;
	int 	 maxtype ;			/* Typemap Length */
	int	 maxwindow ;			/* Number of reference Window */
	unsigned char *types[MAXTYPE] ;		/* Line Type Pixel lengths */
						/* NULL if type is not defined*/
} XW_EXT_TYPEMAP ;

/* FONTMAP */

#define FONTMAP_TYPE   5
#ifdef XW_EXT_FONTMAP
#undef XW_EXT_FONTMAP
#endif

#define _FDISPLAY 	(pfontmap->connexion->display)

typedef struct {
	void*		link ;
	int	 	type ;		/* FONTMAP_TYPE */
	XW_EXT_DISPLAY* connexion ;
	int 	 	maxfont ;	/* Fontmap Length */
	int	 	maxwindow ;	/* Number of reference Window */
	float 	gsizes[MAXFONT] ;	/* Generic Font size in MM */
	float 	fsizes[MAXFONT] ;	/* Font size in MM */
	float 	fratios[MAXFONT] ;	/* Font baseline ratio */
	float 	ssizex[MAXFONT] ;	/* Selected X Font size in MM */
	float 	ssizey[MAXFONT] ;	/* Selected Y Font size in MM */
        float  	gslants[MAXFONT] ;      /* Generic Font slant in RAD */
        float  	sslants[MAXFONT] ;      /* Selected Font slant in RAD */
	char*	gnames[MAXFONT] ;	/* Generic font name */
	char*	snames[MAXFONT] ;	/* Selected font name */
	XFontStruct*	fonts[MAXFONT] ;/* Selected Font structure */
					/* NULL if font is not defined*/
} XW_EXT_FONTMAP ;

/* TILEMAP */

#define TILEMAP_TYPE   6
#ifdef XW_EXT_TILEMAP
#undef XW_EXT_TILEMAP
#endif

#define _PDISPLAY 	(ptilemap->connexion->display)

typedef struct {
	void	 *link ;
	int	 type ;				/* TILEMAP_TYPE */
	XW_EXT_DISPLAY* connexion ;
	int 	 maxtile ;			/* Tilemap Length */
	int	 maxwindow ;			/* Number of reference Window */
	Pixmap   tiles[MAXTILE] ;		/* Tile pixmap */
						/* 0 if tile is not defined */
} XW_EXT_TILEMAP ;

/* MARKMAP */

#define MARKMAP_TYPE   7
#ifdef XW_EXT_MARKMAP
#undef XW_EXT_MARKMAP
#endif

#define _MDISPLAY 	(pmarkmap->connexion->display)

typedef struct {
	void	 *link ;
	int	 type ;				/* MARKMAP_TYPE */
	XW_EXT_DISPLAY* connexion ;
	int 	 maxmarker ;			/* Markmap Length */
	int	 maxwindow ;			/* Number of reference Window */
	unsigned int width[MAXMARKER] ;		/* Pmarker Size in pixels */
	unsigned int height[MAXMARKER] ;	/* Pmarker Size in pixels */
	Pixmap   marks[MAXMARKER] ;		/* Pmarker pixmap */
	int	npoint[MAXMARKER] ;		/* Lmarker length */
	int*    spoint[MAXMARKER] ;		/* Lmarker status   */
	float*  xpoint[MAXMARKER] ;		/* Lmarker x coords */
	float*  ypoint[MAXMARKER] ;		/* Lmarker y coords */
} XW_EXT_MARKMAP ;

/* IMAGEDATA*/

#define IMAGE_TYPE   8
#ifdef XW_EXT_IMAGEDATA
#undef XW_EXT_IMAGEDATA
#endif

#define _ICOLORMAP	(pimage->pcolormap)
#define _IVISUAL	(_ICOLORMAP->visual)
#define _IIMAGE		(pimage->pximage)
#define _ZIMAGE		(pimage->zximage)
#define _IINFO		(pimage->pimageinfo)

typedef struct {
	void	 *link ;
	int	 type ;				/* IMAGE_TYPE */
	XW_EXT_COLORMAP *pcolormap ;
	int	 maxwindow ;			/* Number of reference Window*/
	float	 zoom;				/* Zoom factor */
	XImage   *pximage ;			/* Ref image */
	XImage   *zximage ;			/* Zoomed image */
	XW_USERDATA *pimageinfo ;		/* external IMAGE Infos*/
} XW_EXT_IMAGEDATA ;

typedef struct {
	void*	link ;
	int	isupdated ;
	int	npoint ;
	XPoint  rpoints[MAXPOINTS] ;
	XPoint  upoints[MAXPOINTS] ;
} XW_EXT_POINT ;

typedef struct {
	void*	link ;
	int	nchar ;
	char	chars[MAXCHARS] ;
} XW_EXT_CHAR ;

typedef struct {
        void*   link ;
	int	isupdated ;
        int     nseg ;
        XSegment rsegments[MAXSEGMENTS] ;
        XSegment usegments[MAXSEGMENTS] ;
} XW_EXT_SEGMENT ;

typedef struct {
	void*	link ;
	int	isupdated ;
	int	nmark ;
        int     nseg ;
	int	marks[MAXLMARKERS] ;		/* seg numbers */
	XPoint  rcenters[MAXLMARKERS] ;		/* marker center */
	XPoint  ucenters[MAXLMARKERS] ;		/* marker center */
        XSegment rsegments[MAXSEGMENTS] ;
        XSegment usegments[MAXSEGMENTS] ;
} XW_EXT_LMARKER ;

typedef struct {
	void*	link ;
	int	isupdated ;
	int	nmark ;
        int     npoint ;
	int	marks[MAXPMARKERS] ;		/* point numbers */
	XPoint  rcenters[MAXPMARKERS] ;		/* marker center */
	XPoint  ucenters[MAXPMARKERS] ;		/* marker center */
	XPoint  rpoints[MAXPOINTS] ;
	XPoint  upoints[MAXPOINTS] ;
} XW_EXT_PMARKER ;

typedef struct {
	void*	link ;
	int	isupdated ;
	int	nline ;
	int	lines[MAXLINES] ;		/* line point numbers */
	XPoint  *plines[MAXLINES] ;		/* LINE descriptor */
} XW_EXT_LINE ;

typedef struct {
	void*	link ;
	int	isupdated ;
	int	npoly ;
	int	polys[MAXPOLYS] ;		/* poly point numbers */
	int	paths[MAXPOLYS] ;		/* path point numbers */
	XPoint  *ppolys[MAXPOLYS] ;		/* POLY descriptor */
} XW_EXT_POLY ;

typedef struct {
	void*	link ;
	int	isupdated ;
	int	narc ;
	XArc    rarcs[MAXARCS] ;
	XArc    uarcs[MAXARCS] ;
} XW_EXT_ARC ;

typedef struct {
	void*	link ;
	int	isupdated ;
	int	ntext ;
	int	modes[MAXLTEXTS] ;		/* TEXT draw mode */
	int	texts[MAXLTEXTS] ;
	char	*ptexts[MAXLTEXTS] ;		/* TEXT descriptor */
	XPoint 	rpoints[MAXLTEXTS] ;
	XPoint 	upoints[MAXLTEXTS] ;
        float   rangles[MAXLTEXTS] ;            /* rotated string angle */
        float   uangles[MAXLTEXTS] ;             
        float   rscalex[MAXLTEXTS] ;            /* rotated string scale */
        float   uscalex[MAXLTEXTS] ;             
        float   rscaley[MAXLTEXTS] ;            /* rotated string scale */
        float   uscaley[MAXLTEXTS] ;             
        float   slants[MAXLTEXTS] ;             /* TEXT slant */
} XW_EXT_LTEXT ;

typedef struct {
	void*	link ;
	int	isupdated ;
	int	ntext ;
	int	modes[MAXLTEXTS] ;		/* TEXT draw mode */
	int	texts[MAXPTEXTS] ;
	char	*ptexts[MAXPTEXTS] ;		/* TEXT descriptor */
	XPoint 	rpoints[MAXPTEXTS] ;
	XPoint 	upoints[MAXPTEXTS] ;
        float   rangles[MAXPTEXTS] ;            /* rotated string angle */
        float   uangles[MAXPTEXTS] ;            
	float	marges[MAXPTEXTS] ;		/* polygon margins */
        float   rscalex[MAXLTEXTS] ;            /* rotated string scale */
        float   uscalex[MAXLTEXTS] ;             
        float   rscaley[MAXLTEXTS] ;            /* rotated string scale */
        float   uscaley[MAXLTEXTS] ;             
        float   slants[MAXLTEXTS] ;             /* TEXT slant */
} XW_EXT_PTEXT ;

typedef struct {
	void	*link ;
	int	isupdated ;
	int	nimage ;			/* Image number to drawn */
	XPoint 	rpoints[MAXIMAGES] ;
	XPoint 	upoints[MAXIMAGES] ;
	XW_EXT_IMAGEDATA *pimagdata[MAXIMAGES] ;
} XW_EXT_IMAGE ;

typedef struct {
	int	bufferid ;			/* Identification */
	GC gcf,gcb ;				/* Graphic context */
	XW_ATTRIB code ;			/* QG code as C,T,F,M*/
	int	isdrawn;			/* True if visible at screen */
	int	isempty;			/* True if empty */
	int	isretain;			/* True if retain drawing*/
	int	isupdated;			/* True if has been updated*/
	int	rxmin,rymin,rxmax,rymax;	/* Min Max ref. buffer */
	int	uxmin,uymin,uxmax,uymax;	/* Min Max upt. buffer */
	int	xpivot,ypivot ;			/* Buffer ref.  pivot point */
	int	dxpivot,dypivot ;		/* Current Buffer transl. vector */
	float	xscale,yscale;			/* Buffer ref. drawing scale */
	float	dxscale,dyscale;		/* Current Buffer drawing scale */
	float	angle;				/* Buffer ref. drawing angle */
	float	dangle;				/* Current Buffer drawing angle */
	XW_EXT_CHAR	*ptextdesc ;		/* Retain desc. texts list */
	XW_EXT_POINT	*plinedesc ;		/* Retain desc. lines list */
	XW_EXT_POINT 	*ppntlist ;		/* Retain Points list */
	XW_EXT_SEGMENT 	*pseglist ;		/* Retain Segments list */
	XW_EXT_LINE 	*plinelist ;		/* Retain Polylines list */
	XW_EXT_POLY 	*ppolylist ;		/* Retain Polygones list */
	XW_EXT_ARC 	*plarclist ;		/* Retain Arcs list */
	XW_EXT_ARC 	*pparclist ;		/* Retain poly Arcs list */
	XW_EXT_LTEXT	*pltextlist ;		/* Retain Texts list */
	XW_EXT_PTEXT	*pptextlist ;		/* Retain poly Texts list */
	XW_EXT_LMARKER	*plmarklist ;		/* Retain Markers list */
	XW_EXT_PMARKER	*ppmarklist ;		/* Retain poly Markers list */
	XW_EXT_IMAGE	*pimaglist ;		/* Retain Image list */
} XW_EXT_BUFFER ;

typedef struct {				/* Line GC */
	GC gc ;					/* Graphic context */
	unsigned int count ;			/* Used count info */
	XW_ATTRIB code ;			/* QG code as C,T,W,M*/
} XW_QGL ;

typedef struct {				/* Polygon GC */
	GC gc ;					/* Graphic context */
	unsigned int count ;			/* Used count info */
	XW_ATTRIB code ;			/* QG code as C,T,T,M*/
} XW_QGP ;

typedef struct {				/* Text GC */
	GC gc ;					/* Graphic context */
	unsigned int count ;			/* Used count info */
	XW_ATTRIB code ;			/* QG code as C,T,F,M*/
} XW_QGT ;

typedef struct {				/* Marker GC */
	GC gc ;					/* Graphic context */
	unsigned int count ;			/* Used count info */
	XW_ATTRIB code ;			/* QG code as C,T,T,M*/
} XW_QGM ;

#define _BIMAGE pwindow->qgwind.pimage
#define _BMETHOD pwindow->qgwind.method
#define _BPIXMAP pwindow->qgwind.pixmap
#define _BINDEX  pwindow->qgwind.cbufindex
#define _BUFFER(index) pwindow->qgwind.pbuflist[index]

typedef struct {				/* Window GC */
	GC gc ;					/* Graphic context */
	GC gcclear ;				/* Graphic context */
	GC gccopy ;				/* Graphic context */
	GC gchigh ;				/* Graphic context */
	XW_ATTRIB code ;			/* QG code as C,T,T,M*/
	XW_EXT_IMAGEDATA *pimage ;		/* Back image data's */
	Aspect_FillMethod method ;		/* Back fill method */
	Pixmap pixmap ;				/* Back Pixmap or 0 */
	int		cbufindex ;		/* Current buffer index */	
	XW_EXT_BUFFER   pbuflist[MAXBUFFERS]; 	/* Retain primitives list*/
} XW_QGW ;

/* CALLBACK */

typedef struct {
	XW_CALLBACK	function ;			/* CallBack Address */
	void		*userdata ;			/* CallBack UserData*/
} XW_ECBK ;

#ifdef XW_PROTOTYPE
typedef XW_STATUS (*XW_ICALLBACK)(XW_EVENT* anyevent) ;
#else
typedef XW_STATUS (*XW_ICALLBACK)() ;
#endif /*XW_PROTOTYPE*/

typedef struct {
	XW_ICALLBACK	function ;			/* CallBack Address */
} XW_ICBK ;

/* WINDOW */

#define WINDOW_TYPE   1
#ifdef XW_EXT_WINDOW
#undef XW_EXT_WINDOW
#endif
#define _DISPLAY 	(pwindow->connexion->display)
typedef struct {
	void *link ;			 
	int	 type ;				/* WINDOW_TYPE */
	XWindowAttributes attributes ;		/* Window Attributes */
	int	axleft ;			/* Actual Window position */
	int	aytop ;				/* Actual Window position */
	int	axright ;			/* Actual Window position */
	int	aybottom ;			/* Actual Window position */
	int	rwidth ;			/* Requested Window width*/
	int	rheight ;			/* Requested Window height*/
        float   xratio ;                         /* Window User/Pixel Ratio*/
        float   yratio ;                         /* Window User/Pixel Ratio*/
	XW_EXT_DISPLAY* connexion ;
	Window 	 window ;			/* Window ID */
	Pixmap	 pixmap ;			/* Associated Window Pixmap*/
        int      nwbuffer ;                     /* MultiBuffers number if any */
        XID      wbuffers[2] ;                  /* Associated Window Buffers*/
	Drawable drawable ;			/* Can be Window or Pixmap */
	int 	 backindex ;			/* Colormap background Index */
	int	 clipflag ;			/* Unable internal clipping*/ 
	XW_EXT_COLORMAP *pcolormap ;		/* Colormap Extension */
	XW_EXT_WIDTHMAP *pwidthmap ;		/* Widthmap Extension */
	XW_EXT_TYPEMAP  *ptypemap ;		/* Widthmap Extension */
	XW_EXT_FONTMAP	*pfontmap ;		/* Fontmap Extension */
	XW_EXT_TILEMAP  *ptilemap ;		/* Tilemap Extension */
	XW_EXT_MARKMAP  *pmarkmap ;		/* Markmap Extension */
	int	 	lineindex ;		/* Current QG line index used */
	XW_QGL		qgline[MAXQG] ;		/* QG Line Bundle indexs */
	int	 	polyindex ;		/* Current QG poly index used */
	XW_QGP		qgpoly[MAXQG] ;		/* QG poly Bundle indexs */
	int	 	textindex ;		/* Current QG text index used */
	XW_QGT		qgtext[MAXQG] ;		/* QG text Bundle indexs */
	int	 	markindex ;		/* Current QG markerindex used*/
	XW_QGM		qgmark[MAXQG] ;		/* QG marker Bundle indexs */
	XW_QGW		qgwind ;		/* QG Window Bundle */
	XW_ECBK		ecbk[MAXEVENT] ;	/* External CallBack EventList*/
	XW_ICBK		icbk[MAXEVENT] ;	/* Internal CallBack EventList*/
	void		*other ;		/* Other data extension */
} XW_EXT_WINDOW;

/* ICONS */

#define ICON_TYPE   10

typedef struct {
	void 	*link ;			 
	int	type ;				/* ICON_TYPE */
	char	*pname ;
	char	*pfile ;
	XW_EXT_IMAGEDATA *pimage ;	/* Associated Image data's */
	Pixmap  pixmap ;		/* Associated Pixmap if any */
	int	update;
} XW_EXT_ICON ;

/*
	Internal Functions
*/

#define Xw_malloc malloc
#define Xw_calloc calloc
#define Xw_free free

#ifdef XW_PROTOTYPE
#ifdef __cplusplus
extern "C" {
#endif
void _XInitImageFuncPtrs(XImage* pximage);
#ifdef __cplusplus
}
#endif
XVisualInfo* Xw_get_visual_info(XW_EXT_DISPLAY* display,Xw_TypeOfVisual tclass);
XVisualInfo* Xw_get_overlay_visual_info(XW_EXT_DISPLAY* display,Xw_TypeOfVisual tclass,unsigned long* backpixel) ;

XW_EXT_DISPLAY* Xw_get_display_structure(Display* display) ;
XW_EXT_DISPLAY* Xw_add_display_structure(int size) ;
XW_STATUS Xw_del_display_structure(XW_EXT_DISPLAY* adisplay) ;

XW_EXT_WINDOW* Xw_get_window_structure(Window window) ;
XW_EXT_WINDOW* Xw_add_window_structure(int size) ;
XW_STATUS Xw_del_window_structure(XW_EXT_WINDOW* awindow) ;
XW_STATUS Xw_reset_attrib(XW_EXT_WINDOW* awindow) ;

XW_STATUS Xw_redraw_buffer(XW_EXT_WINDOW *pwindow,XW_EXT_BUFFER *pbuffer) ;
XW_STATUS Xw_update_buffer(XW_EXT_WINDOW *pwindow,XW_EXT_BUFFER *pbuffer) ;
XW_EXT_BUFFER* Xw_get_buffer_structure(XW_EXT_WINDOW *pwindow,int bufferid) ;

XW_EXT_CHAR* Xw_add_text_desc_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_text_desc_structure(XW_EXT_BUFFER *pbuflist) ;

XW_EXT_POINT* Xw_add_line_desc_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_line_desc_structure(XW_EXT_BUFFER *pbuflist) ;

XW_EXT_POINT* Xw_add_point_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_point_structure(XW_EXT_BUFFER *pbuflist) ;
void Xw_draw_pixel_points(XW_EXT_WINDOW* pwindow,XW_EXT_POINT* ppntlist,GC gc) ;

XW_EXT_SEGMENT* Xw_add_segment_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_segment_structure(XW_EXT_BUFFER *pbuflist) ;
void Xw_draw_pixel_segments(XW_EXT_WINDOW* pwindow,XW_EXT_SEGMENT* pseglist,GC gc) ;

XW_EXT_LINE* Xw_add_polyline_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_polyline_structure(XW_EXT_BUFFER *pbuflist) ;
void Xw_draw_pixel_lines(XW_EXT_WINDOW* pwindow,XW_EXT_LINE* plinelist,GC gc) ;

XW_EXT_ARC* Xw_add_arc_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_arc_structure(XW_EXT_BUFFER *pbuflist) ;
void Xw_draw_pixel_arcs(XW_EXT_WINDOW* pwindow,XW_EXT_ARC* parclist,GC gc) ;

XW_EXT_ARC* Xw_add_polyarc_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_polyarc_structure(XW_EXT_BUFFER *pbuflist) ;
void Xw_draw_pixel_polyarcs(XW_EXT_WINDOW* pwindow,XW_EXT_ARC* parclist,
							GC gcpoly,GC gcline) ;

XW_EXT_POLY* Xw_add_polygone_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_polygone_structure(XW_EXT_BUFFER *pbuflist) ;
void Xw_draw_pixel_polys(XW_EXT_WINDOW* pwindow,XW_EXT_POLY* ppolylist,
							GC gcpoly,GC gcline) ;

XW_EXT_LTEXT* Xw_add_text_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_text_structure(XW_EXT_BUFFER *pbuflist) ;
void Xw_draw_pixel_texts(XW_EXT_WINDOW* pwindow,XW_EXT_LTEXT* ptextlist,
						GC gc,XW_ATTRIB code) ;

XW_EXT_PTEXT* Xw_add_polytext_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_polytext_structure(XW_EXT_BUFFER *pbuflist) ;
void Xw_draw_pixel_polytexts(XW_EXT_WINDOW* pwindow,XW_EXT_PTEXT* ptextlist,
			GC gctext,GC gcpoly,GC gcline,XW_ATTRIB code) ;

XW_STATUS Xw_isdefine_markerindex(XW_EXT_MARKMAP* amarkmap,int index) ;
XW_EXT_LMARKER* Xw_add_lmarker_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_lmarker_structure(XW_EXT_BUFFER *pbuflist) ;
void Xw_draw_pixel_lmarkers(XW_EXT_WINDOW* pwindow,XW_EXT_LMARKER* pmarklist,GC gc) ;
XW_EXT_PMARKER* Xw_add_pmarker_structure(XW_EXT_BUFFER *pbuflist) ;
XW_STATUS Xw_del_pmarker_structure(XW_EXT_BUFFER *pbuflist) ;
void Xw_draw_pixel_pmarkers(XW_EXT_WINDOW* pwindow,XW_EXT_PMARKER* pmarklist,GC gc) ;
Colormap Xw_create_colormap(XW_EXT_COLORMAP *pcolormap) ;
XW_EXT_COLORMAP* Xw_add_colormap_structure(int size) ;
XW_EXT_COLORMAP* Xw_get_colormap_structure(XW_EXT_COLORMAP* acolormap) ;
XW_STATUS Xw_del_colormap_structure(XW_EXT_COLORMAP* acolormap) ;
XW_STATUS Xw_get_color_attrib(XW_EXT_WINDOW* awindow,
		XW_DRAWMODE mode,int color,unsigned long* hcolor,
		int* function,unsigned long* planemask) ;
XW_STATUS Xw_isdefine_colorindex(XW_EXT_COLORMAP* acolormap,int index) ;
XW_STATUS Xw_ifsystem_colorindex(XW_EXT_COLORMAP* acolormap,int index) ;
XW_STATUS Xw_ifimage_colorindex(XW_EXT_COLORMAP* acolormap,int index) ;
XW_STATUS Xw_ifbackground_colorindex(XW_EXT_COLORMAP* acolormap,int index) ;
XW_STATUS Xw_ifhighlight_colorindex(XW_EXT_COLORMAP* acolormap,int index) ;
char* Xw_get_filename(char* filename,const char *extension) ;

XW_EXT_WIDTHMAP* Xw_add_widthmap_structure(int size) ;
XW_STATUS Xw_del_widthmap_structure(XW_EXT_WIDTHMAP* awidthmap) ;
XW_STATUS Xw_isdefine_widthindex(XW_EXT_WIDTHMAP* awidthmap,int index) ;

XW_EXT_TYPEMAP* Xw_add_typemap_structure(int size) ;
XW_STATUS Xw_del_typemap_structure(XW_EXT_TYPEMAP* atypemap) ;
XW_STATUS Xw_isdefine_typeindex(XW_EXT_TYPEMAP* atypemap,int index) ;

XW_EXT_FONTMAP* Xw_add_fontmap_structure(int size) ;
XW_STATUS Xw_del_fontmap_structure(XW_EXT_FONTMAP* afontmap) ;
XW_STATUS Xw_isdefine_fontindex(XW_EXT_FONTMAP* afontmap,int index) ;

XW_EXT_TILEMAP* Xw_add_tilemap_structure(int size) ;
XW_STATUS Xw_del_tilemap_structure(XW_EXT_TILEMAP* atilemap) ;
XW_STATUS Xw_isdefine_tileindex(XW_EXT_TILEMAP* atilemap,int index) ;

XW_EXT_MARKMAP* Xw_add_markmap_structure(int size) ;
XW_STATUS Xw_del_markmap_structure(XW_EXT_MARKMAP* amarkmap) ;

XW_EXT_IMAGEDATA* Xw_add_imagedata_structure(int size) ;
XW_STATUS Xw_del_imagedata_structure(XW_EXT_IMAGEDATA* aimagedata) ;

XW_EXT_IMAGE* Xw_add_image_structure(XW_EXT_BUFFER* pbuflist) ;
XW_STATUS Xw_del_image_structure(XW_EXT_BUFFER* pbuflist) ;
void Xw_draw_pixel_images(XW_EXT_WINDOW* pwindow,XW_EXT_IMAGE *pimaglist,
							GC gcimag,GC gcline) ;

XW_STATUS Xw_set_internal_event(XW_EXT_WINDOW* awindow,
				XW_EVENTTYPE type,XW_ICALLBACK call_back) ;

XW_STATUS Xw_alloc_color(XW_EXT_COLORMAP* acolormap,float r,float g,float b,unsigned long *pixel,int *isapproximate);

int Xw_clip_segment(XW_EXT_WINDOW* pwindow,int x1,int y1,int x2,int y2,
XSegment* segment);
int Xw_get_trace() ;
int Xw_test(unsigned long mask) ;
void Xw_set_error(int code,const char* message,void* pvalue) ;
int Xw_error_handler(Display* display,XErrorEvent* event) ;
void Xw_set_synchronize(Display* display,int state) ;
void Xw_longcopy(register long* from,register long* to,register unsigned n) ;
void Xw_shortcopy(register short* from,register short* to,register unsigned n) ;
void Xw_bytecopy(register char* from,register char* to,register unsigned n) ;

#ifndef Xw_malloc
void* Xw_malloc(int size) ;
#endif

#ifndef Xw_calloc
void* Xw_calloc(int length,int size) ;
#endif

#ifndef Xw_free
void Xw_free(void* address) ;
#endif

//OCC186
int PXPOINT( double x, double ratio );
int PYPOINT( double y, double height, double ratio );
int PVALUE( double v, double xratio, double yratio );
//OCC186

#else /*XW_PROTOTYPE*/

extern "C" {
void _XInitImageFuncPtrs();
}
XVisualInfo* Xw_get_visual_info() ;
XVisualInfo* Xw_get_overlay_visual_info() ;

XW_EXT_DISPLAY* Xw_get_display_structure() ;
XW_EXT_DISPLAY* Xw_add_display_structure() ;
XW_STATUS Xw_del_display_structure() ;

XW_EXT_WINDOW* Xw_get_window_structure() ;
XW_EXT_WINDOW* Xw_add_window_structure() ;
XW_STATUS Xw_del_window_structure() ;
XW_STATUS Xw_reset_attrib() ;

XW_STATUS Xw_redraw_buffer() ;
XW_STATUS Xw_update_buffer() ;
XW_EXT_BUFFER* Xw_get_buffer_structure() ;

XW_EXT_CHAR* Xw_add_text_desc_structure() ;
XW_STATUS Xw_del_text_desc_structure() ;

XW_EXT_POINT* Xw_add_line_desc_structure() ;
XW_STATUS Xw_del_line_desc_structure() ;

XW_EXT_POINT* Xw_add_point_structure() ;
XW_STATUS Xw_del_point_structure() ;
void Xw_draw_pixel_points() ;

XW_EXT_SEGMENT* Xw_add_segment_structure() ;
XW_STATUS Xw_del_segment_structure() ;
void Xw_draw_pixel_segments() ;

XW_EXT_LINE* Xw_add_polyline_structure() ;
XW_STATUS Xw_del_polyline_structure() ;
void Xw_draw_pixel_lines() ;

XW_EXT_ARC* Xw_add_arc_structure() ;
XW_STATUS Xw_del_arc_structure() ;
void Xw_draw_pixel_arcs() ;

XW_EXT_ARC* Xw_add_polyarc_structure() ;
XW_STATUS Xw_del_polyarc_structure() ;
void Xw_draw_pixel_polyarcs() ;

XW_EXT_POLY* Xw_add_polygone_structure() ;
XW_STATUS Xw_del_polygone_structure() ;
void Xw_draw_pixel_polys() ;

XW_EXT_LTEXT* Xw_add_text_structure() ;
XW_STATUS Xw_del_text_structure() ;
void Xw_draw_pixel_texts() ;

XW_EXT_PTEXT* Xw_add_polytext_structure() ;
XW_STATUS Xw_del_polytext_structure() ;
void Xw_draw_pixel_polytexts() ;

XW_STATUS Xw_isdefine_markerindex() ;
XW_EXT_LMARKER* Xw_add_lmarker_structure() ;
XW_STATUS Xw_del_lmarker_structure() ;
void Xw_draw_pixel_lmarkers() ;
XW_EXT_PMARKER* Xw_add_pmarker_structure() ;
XW_STATUS Xw_del_pmarker_structure() ;
void Xw_draw_pixel_pmarkers() ;

Colormap Xw_create_colormap() ;
XW_EXT_COLORMAP* Xw_add_colormap_structure() ;
XW_EXT_COLORMAP* Xw_get_colormap_structure() ;
XW_STATUS Xw_del_colormap_structure() ;
XW_STATUS Xw_get_color_attrib() ;
XW_STATUS Xw_isdefine_colorindex() ;
XW_STATUS Xw_ifsystem_colorindex() ;
XW_STATUS Xw_ifimage_colorindex() ;
XW_STATUS Xw_ifbackground_colorindex() ;
XW_STATUS Xw_ifhighlight_colorindex() ;
char* Xw_get_filename() ;

XW_EXT_WIDTHMAP* Xw_add_widthmap_structure() ;
XW_STATUS Xw_del_widthmap_structure() ;
XW_STATUS Xw_isdefine_widthindex() ;

XW_EXT_TYPEMAP* Xw_add_typemap_structure() ;
XW_STATUS Xw_del_typemap_structure() ;
XW_STATUS Xw_isdefine_typeindex() ;

XW_EXT_FONTMAP* Xw_add_fontmap_structure() ;
XW_STATUS Xw_del_fontmap_structure() ;
XW_STATUS Xw_isdefine_fontindex() ;

XW_EXT_TILEMAP* Xw_add_tilemap_structure() ;
XW_STATUS Xw_del_tilemap_structure() ;
XW_STATUS Xw_isdefine_tileindex() ;

XW_EXT_MARKMAP* Xw_add_markmap_structure() ;
XW_STATUS Xw_del_markmap_structure() ;

XW_EXT_IMAGEDATA* Xw_add_imagedata_structure() ;
XW_STATUS Xw_del_imagedata_structure() ;

XW_EXT_IMAGE* Xw_add_image_structure() ;
XW_STATUS Xw_del_image_structure() ;
void Xw_draw_pixel_images() ;

XW_STATUS Xw_set_internal_event() ;

XW_STATUS Xw_alloc_color();

int Xw_clip_segment();
int Xw_test() ;
int Xw_get_trace() ;
void Xw_set_error() ;
int Xw_error_handler() ;
void Xw_set_synchronize() ;
void Xw_longcopy() ;
void Xw_shortcopy() ;
void Xw_bytecopy() ;

#ifndef Xw_malloc
void* Xw_malloc() ;
#endif

#ifndef Xw_calloc
void* Xw_calloc() ;
#endif

#ifndef Xw_free
void Xw_free() ;
#endif

//OCC186
int PXPOINT();
int PYPOINT();
int PVALUE();
//OCC186

#endif /*XW_PROTOTYPE*/
