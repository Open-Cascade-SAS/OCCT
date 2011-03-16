/***********************************************************************

FONCTION :
----------
File Include OpenGl_telem_util :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
22-04-96 : FMN ; Ajout TelReadImage TelDrawImage
23-12-96 : CAL ; Probleme dans la macro vecmag
12-02-97 : FMN ; Suppression TelEnquireFacilities()
07-10-97 : FMN ; Simplification WNT
23-12-97 : FMN ; Suppression TelBackInteriorStyle, TelBackInteriorStyleIndex

************************************************************************/

#ifndef  OPENGL_TELEM_UTIL_H
#define  OPENGL_TELEM_UTIL_H

#ifndef IMP190100
#define IMP190100       /*GG To avoid too many REDRAW in immediat mode,
//                      Add TelMakeFrontAndBackBufCurrent() function
*/
#endif

#define BUC60823    /* GG 05/03/01 Avoid to crash in normal computation
//        between confused points
*/

#include <math.h>
#include <GL/gl.h>
#ifndef WNT
#include <GL/glx.h>
#endif
#include <OpenGl_telem.hxx>

/*
* ShortRealLast () = 3.40282346638528860e+38
* ShortRealFirst () = 3.40282346638528860e+38
*/
#define  shortreallast()     (3.e+38)
#define  shortrealfirst()    (-3.e+38)

/* vector manipulation macros */
#define  square(a)      ((a)*(a))

/* add */
#define  vecadd(a,b,c)  { (a)[0]=(b)[0]+(c)[0]; \
  (a)[1]=(b)[1]+(c)[1]; \
  (a)[2]=(b)[2]+(c)[2]; }

/* subtract */
#define  vecsub(a,b,c)  { (a)[0]=(b)[0]-(c)[0]; \
  (a)[1]=(b)[1]-(c)[1]; \
  (a)[2]=(b)[2]-(c)[2]; }

/* dot product */
#define  vecdot(a,b)    ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])

/* cross product */
#define  veccrs(a,b,c)  { Tfloat x, y, z; \
  x = (b)[1]*(c)[2] - (b)[2]*(c)[1]; \
  y = (b)[2]*(c)[0] - (b)[0]*(c)[2]; \
  z = (b)[0]*(c)[1] - (b)[1]*(c)[0]; \
  (a)[0] = x; \
  (a)[1] = y; \
  (a)[2] = z; }
/* scale */
#define  vecscl(a,b)     { (b)[0] *= (a); (b)[1] *= (a); (b)[2] *= (a); }

/* magnitude square */
#define  vecmg2(a)       (square((a)[0])+square((a)[1])+square((a)[2]))

/* magnitude */
#define  vecmag(a)       (sqrt((double)vecmg2(a)))

/* normalize */
#ifdef BUC60823
#define  vecnrmd(a,d)    ( d = (Tfloat)vecmag(a), \
  ( d > 1e-10 ? (a[0] /= d, a[1] /= d, a[2] /= d, d) : (Tfloat)0. ) )
#define  vecnrm(a)       { Tfloat d; vecnrmd(a,d); }
#else
#define  vecnrm(a)       { Tfloat d; d = ( Tfloat )vecmag(a); \
  (a)[0] /= d; (a)[1] /= d; (a)[2] /= d; }
#endif

/* angle between two vectors */
#define  vecang(a,b,d)   { d = (Tfloat)(vecmag(a)*vecmag(b)); \
  d = vecdot(a,b)/d; \
  d = d < -1.0F ? -1.0F : d > 1.0F ? 1.0F : d; \
  d = ( Tfloat )acos(d); }

/* point along a vector at a distance */
#define  vecgnd(a,b,c,d) { Tfloat w; w = d/vecmag(c); \
  (a)[0] = (b)[0]+(c)[0]*w;  \
  (a)[1] = (b)[1]+(c)[1]*w;  \
  (a)[2] = (b)[2]+(c)[2]*w;  }
/* copy */
#define  veccpy(a,b)     ((a)[0]=(b)[0],(a)[1]=(b)[1],(a)[2]=(b)[2])

/* end vector macros */

typedef  struct
{
  Tmatrix3  mat;
}
Tmatrix3Struct;

#define  matcpy(d,s) { *((Tmatrix3Struct*)(d)) = *((Tmatrix3Struct*)(s)); }
/*
(d)[0][0]=(s)[0][0],(d)[0][1]=(s)[0][1],(d)[0][2]=(s)[0][2],(d)[0][3]=(s)[0][3],
(d)[1][0]=(s)[1][0],(d)[1][1]=(s)[1][1],(d)[1][2]=(s)[1][2],(d)[1][3]=(s)[1][3],
(d)[2][0]=(s)[2][0],(d)[2][1]=(s)[2][1],(d)[2][2]=(s)[2][2],(d)[2][3]=(s)[2][3],
(d)[3][0]=(s)[3][0],(d)[3][1]=(s)[3][1],(d)[3][2]=(s)[3][2],(d)[3][3]=(s)[3][3]
*/

#define matdump(m) { \
  int i, j; \
  for (i=0; i<4; i++) {\
  printf ("\t"); \
  for (j=0; j<4; j++) \
  printf ("%f ", m[i][j]); \
  printf ("\n"); \
  } \
}

extern Tint TelRemdupnames( Tint*, Tint ); /* list, num */
#ifdef BUC60823
extern int TelGetPolygonNormal( tel_point, Tint*, Tint, Tfloat* );
extern int TelGetNormal( Tfloat*, Tfloat*, Tfloat*, Tfloat* );
#else
extern void TelGetNormal( Tfloat*, Tfloat*, Tfloat*, Tfloat* );
#endif
/* point1,  point2,  point3,  normal */
extern Tint TelIsBackFace( Tmatrix3, Tfloat* ); /* normal */
extern Tint TelIsBackFacePerspective( Tmatrix3, Tfloat*, Tfloat*, Tfloat* ); 
/* matrix,  point 1, point 2, point 3 */
extern void TelMultiplymat3( Tmatrix3, Tmatrix3, Tmatrix3 );
/* mat out, mat in, mat in */
extern void TelTransposemat3( Tmatrix3 ); /* mat in out */
extern void TelTranpt3( Tfloat [4], Tfloat [4], Tmatrix3 ); /* out, in, mat */

extern  void  TelInitWS( Tint, Tint, Tint, Tfloat, Tfloat, Tfloat );
/* ws, width, height, bgcolr, bgcolg, bgcolb */
extern  void  TelSwapBuffers( Tint );
extern  void  TelCopyBuffers( Tint, GLenum, GLenum,
                             Tfloat, Tfloat, Tfloat, Tfloat, Tfloat, Tfloat, Tint );
extern  TStatus TelProjectionRaster( Tint ws, Tfloat x, Tfloat y, Tfloat z,
                                    Tfloat *xr, Tfloat *yr);
extern  TStatus TelUnProjectionRaster( Tint ws, Tint xr, Tint yr,
                                      Tfloat *x, Tfloat *y, Tfloat *z);
TStatus
TelUnProjectionRasterWithRay(Tint ws, Tint xr, Tint yr, Tfloat *x, Tfloat *y, Tfloat *z,
                             Tfloat *dx, Tfloat *dy, Tfloat *dz);
extern  Tint  TelBackBufferRestored(void);
extern  void  TelSetBackBufferRestored( Tint );
extern  void  TelEnable( Tint );
extern  void  TelDisable( Tint );
extern  void  TelFlush( Tint );

extern void TelSetFrontFaceAttri(
                                 Tint,          /* interior_style */
                                 Tint,          /* back_interior_style */
                                 Tint,          /* interior_index */
                                 Tint,          /* back_interior_index */
                                 Tint,          /* front_shading_method */
                                 Tint,          /* back_shading_method */
                                 Tint,          /* front_lighting_model */
                                 Tint,          /* back_lighting_model */
                                 tel_surf_prop, /* surf_prop */
                                 tel_surf_prop, /* back_surf_prop */
                                 tel_colour,    /* interior_colour */
                                 tel_colour    /* back_interior_colour */
                                 );
extern void TelSetBackFaceAttri(
                                Tint,          /* interior_style */
                                Tint,          /* back_interior_style */
                                Tint,          /* interior_index */
                                Tint,          /* back_interior_index */
                                Tint,          /* front_shading_method */
                                Tint,          /* back_shading_method */
                                Tint,          /* front_lighting_model */
                                Tint,          /* back_lighting_model */
                                tel_surf_prop, /* surf_prop */
                                tel_surf_prop, /* back_surf_prop */
                                tel_colour,    /* interior_colour */
                                tel_colour    /* back_interior_colour */
                                );

extern  void  TelReadImage(Tint , GLenum , Tint , Tint , Tint , Tint , unsigned int *);
extern  void  TelDrawImage(Tint , GLenum , Tint , Tint , Tint , Tint , unsigned int *);
extern  void  TelReadDepths(Tint , Tint , Tint , Tint , Tint , float *);

extern  void TelMakeFrontBufCurrent(Tint );
extern  void TelMakeBackBufCurrent(Tint );
#ifdef IMP190100
extern  void TelMakeFrontAndBackBufCurrent(Tint );
#endif

#ifndef WNT
extern  void TelSetPixmapDBParams(Display *dpy,
                                  Window window,
                                  int width, int height, int depth, GC gc,
                                  Pixmap pixmap,
                                  GLXPixmap glxpixmap,
                                  GLXContext ctx);
extern  GLXPixmap TelGetGLXPixmap(void);
extern  void TelSetPixmapDB(int flag);
extern  int TelTestPixmapDB(void);
extern  void TelDrawBuffer(GLenum buf);
#endif  /* WNT*/

#endif
