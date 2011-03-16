/***********************************************************************

FONCTION :
----------
File OpenGl_telem_util :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
07-02-96 : FMN ; - Ajout trace
- Suppression code inutile
08-03-96 : FMN ; - Ajout include manquant 
01-04-96 : CAL ; Integration MINSK portage WNT
15-04-96 : CAL ; Integration travail PIXMAP de Jim ROTH
22-04-96 : FMN ; Ajout TelReadImage TelDrawImage
10-05-96 : CAL ; Ajout d'un nouveau delta dans les copies
de pixels (voir CALL_DEF_DELTA)
25-06-96 : FMN ; Suppression utilisation de glScissor.
02-07-96 : FMN ; Suppression WSWSHeight et WSWSWidth
Suppression glViewport inutile.
18-07-96 : FMN ; Suppression TelFlush inutile.
08-07-96 : FMN ; Suppression de OPENGL_DEBUG inutile avec la nouvelle
version de ogldebug.
24-10-96 : CAL ; Portage WNT
23-01-97 : CAL ; Suppression de TelClearViews dans TelCopyBuffers
30-01-97 : FMN ; Ajout commentaires + WNT.
12-02-97 : FMN ; Suppression TelEnquireFacilities()
22-04-97 : FMN ; Ajout affichage du cadre pour la copie de buffer
30-06-97 : FMN ; Suppression OpenGl_telem_light.h
18-07-97 : FMN ; Utilisation de la toolkit sur les lights
07-10-97 : FMN ; Simplification WNT + correction Transient
05-12-97 : FMN ; PRO11168: Suppression TglActiveWs pour project/unproject
23-12-97 : FMN ; Suppression TelSetFrontFaceAttri et TelSetBackFaceAttri 
30-12-97 : FMN ; CTS18312: Correction back material
04-05-98 : CAL ; Contournement bug SGI octane bavure de pixels (PRO12899)
30-09-98 : CAL ; Optimisation pour eviter de charger inutilement
les matrices de la vue.
19-10-98 : FMN ; Suppression de glPixelTransferi dans TelEnable() car cela
rentre en conflit avec l'utilisation d'une image de fond.
02.14.100 : JR : Warnings on WNT truncations from double to float
08-03-01 : GG  ; BUC60823 Avoid crash in the normal computation method
when a face has confused or aligned points.

************************************************************************/

#define IMP190100 /*GG To avoid too many REDRAW in immediat mode,
//      Add TelMakeFrontAndBackBufCurrent() function
*/
#define QTOCC_PATCH

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef WNT
# include <X11/Xlib.h>
#else
# define STRICT
# include <windows.h>
#endif  /* WNT */


#include <GL/gl.h>
#include <GL/glu.h>
#ifndef WNT
#include <GL/glx.h>
#endif /* WNT */

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_tgl_subrvis.hxx>
#include <OpenGl_txgl.hxx>
#include <OpenGl_LightBox.hxx>
#include <OpenGl_Memory.hxx>

/*----------------------------------------------------------------------*/
/*
* Constantes
*/

#define NO_TRACE
#define CALL_DEF_DELTA 10
#define NO_COPYBUFFER

/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/

static Tint call_back_buffer_restored = TOff;

#ifndef WNT

/*
* Wrappers and utilities used to select between pixmap and gl double buffering
*/

/* Cache some pixmap info for pixmap double buffering */

static Display *window_display; /* the display in use */
static Window window_id;  /* the window to copy the pixmap to */
static Pixmap pixmap_id;  /* the pixmap id */
static GLXPixmap glx_pixmap;  /* the glx pixmap */
static int
window_width,   /* window width and height for XCopyArea */
window_height,
window_depth;

static GC window_gc;    /* GC for XCopyArea */

static GLXContext glx_context;  /* GL Context */

static int usePixmapDB = 0; /* True if doing pixmap double buffering */

/*----------------------------------------------------------------------*/

void TelSetPixmapDBParams(Display *dpy,
                          Window window,
                          int width, int height, int depth, GC gc,
                          Pixmap pixmap,
                          GLXPixmap glxpixmap,
                          GLXContext ctx)

{
  window_display = dpy;
  window_id = window;
  window_width = width;
  window_height = height;
  window_depth = depth;
  window_gc = gc;

  pixmap_id = pixmap;

  glx_pixmap = glxpixmap;

  glx_context = ctx;
}

GLXPixmap TelGetGLXPixmap()
{
  return glx_pixmap;
}

void TelSetPixmapDB(int flag)
{
  usePixmapDB = flag;
}

int TelTestPixmapDB()
{
  return usePixmapDB;
}

void TelDrawBuffer(GLenum buf)
{
  if (usePixmapDB)
    glDrawBuffer(GL_FRONT);
  else
    glDrawBuffer(buf);
}

#endif /* WNT */

/*----------------------------------------------------------------------*/

void TelMakeFrontBufCurrent(Tint WsId)
{
#ifndef WNT
  if (usePixmapDB)
  {
    glXMakeCurrent(window_display,window_id, glx_context);
    glDrawBuffer(GL_FRONT);
  }
  else
#endif /* WNT */
  {
    glDrawBuffer(GL_FRONT);
  }
}

void TelMakeBackBufCurrent(Tint WsId)
{
#ifndef WNT
  if (usePixmapDB)
  {
    glXMakeCurrent(window_display,glx_pixmap, glx_context);
    glDrawBuffer(GL_BACK);
  }
  else
#endif /* WNT */
  {
    glDrawBuffer(GL_BACK);
  }
}

#ifdef IMP190100
void TelMakeFrontAndBackBufCurrent(Tint WsId)
{
#ifndef WNT
  if (usePixmapDB)
  {
    glXMakeCurrent(window_display,window_id, glx_context);
    glDrawBuffer(GL_FRONT_AND_BACK);
  }
  else
#endif /* WNT */
  {
    glDrawBuffer(GL_FRONT_AND_BACK);
  }
}
#endif

/*----------------------------------------------------------------------*/
Tint
TelRemdupnames(Tint *ls, Tint num )
{
  register  Tint  *ap, *bp, n;

  if( num < 2 )
    return num;

  ap = bp = ls+1;
  n = num-1;
  while( n-- )
  {
    if( ap[-1] != *bp )
      *ap++ = *bp++;
    else
      bp++;
  }

  return ap-ls;
}

/*----------------------------------------------------------------------*/
#ifdef BUC60823
#define GPRECIS 0.000001
Tint TelGetPolygonNormal(tel_point pnts, Tint* indexs, Tint npnt, Tfloat *norm ) {
  Tint status=0;

  norm[0] = norm[1] = norm[2] = 0.;
  if( npnt > 2 ) { 
    Tfloat a[3], b[3], c[3];
    Tint i,j,i0,ii=0,jj;

    i0 = 0; if( indexs ) i0 = indexs[0];
    for( i=1 ; i<npnt ; i++ ) {
      ii = i; if( indexs ) ii = indexs[i];
      vecsub( a, pnts[ii].xyz, pnts[i0].xyz );
      if( vecmg2(a) > GPRECIS ) break;
    }
    if( i < npnt-1 ) {
      for( j=i+1 ; j<npnt ; j++ ) {
        jj = j; if( indexs ) jj = indexs[j];
        vecsub( b, pnts[jj].xyz, pnts[i0].xyz );
        vecsub( c, pnts[jj].xyz, pnts[ii].xyz );
        if( (vecmg2(b) > GPRECIS) && (vecmg2(c) > GPRECIS) ) break;
      }
      if( j < npnt ) {
        Tfloat d;
        veccrs( norm, a, b );
        d = vecnrmd( norm, d );
        status = (d > 0.) ? 1 : 0;
      }
    }
  }
#ifdef DEB
  if( !status )
    printf(" *** OpenGl_TelGetPolygonNormal.has found confused or aligned points\n");
#endif

  return status;
}

Tint TelGetNormal(Tfloat *data1, Tfloat *data2, Tfloat *data3, Tfloat *norm ) {
  Tfloat a[3], b[3];
  Tint status=0;

  norm[0] = norm[1] = norm[2] = 0.;
  vecsub( a, data2, data1 );
  vecsub( b, data3, data2 );
  if( (vecmg2(a) > GPRECIS) && (vecmg2(b) > GPRECIS) ) {
    Tfloat d;
    veccrs( norm, a, b );
    d = vecnrmd( norm, d );
    status = (d > 0.) ? 1 : 0;
  }
#ifdef DEB
  if( !status )
    printf(" *** OpenGl_TelGetNormal.has found confused or aligned points\n");
#endif

  return status;
}
#else
void
TelGetNormal(Tfloat *data1, Tfloat *data2, Tfloat *data3, Tfloat *norm ) {
  Tfloat a[3], b[3];

  vecsub( a, data2, data1 );
  vecsub( b, data3, data2 );
  veccrs( norm, a, b );
}
#endif

/*----------------------------------------------------------------------*/
Tint
TelIsBackFace(Tmatrix3 n, Tfloat *nrm )
{
  Tfloat    r[4], m[4];

  veccpy(m,nrm);
  m[3] = ( float )1.0;

  TelTranpt3( r, m, n );

  return r[2] < 0.0;
}

/*----------------------------------------------------------------------*/
void
TelTransposemat3 (Tmatrix3 a)
{
  Tint row, col;
  Tmatrix3 res;
  Tint dim = 4;

  /* transposition de la sous-matrice dim x dim */
  for (row = 0; row < dim; row++)
    for (col = 0; col < dim; col++)
      res[row][col] = a[col][row];

  /* copie du resultat */
  matcpy (a, res);

  return;
}

/*----------------------------------------------------------------------*/
void
TelMultiplymat3 (Tmatrix3 c, Tmatrix3 a, Tmatrix3 b)
{
  Tint row, col, i;
  Tmatrix3 res;
  Tint dim = 4;

  /* on multiplie d'abord les 2 matrices dim x dim */
  for (row = 0; row < dim; row++) {
    for (col = 0; col < dim; col++) {
      Tfloat sum = ( float )0.0;
      for (i = 0; i < dim; i++)
        sum += a[row][i] * b[i][col];
      res[row][col] = sum;
    }
  }

  /* on copie ensuite le resultat */
  matcpy (c, res);

  return;
}

/*----------------------------------------------------------------------*/
void
TelTranpt3(Tfloat tpt[4], Tfloat pt[4], Tmatrix3 mat )
{
  register  long  i, j;
  Tfloat    sum;

  for( i = 0; i < 4; i++ )
  {
    for( j = 0, sum = ( float )0.0; j < 4; j++ )
    {
      sum += pt[j] * mat[j][i];
    }
    tpt[i] = sum;
  }
  return;
}

/*----------------------------------------------------------------------*/
void
TelInitWS(Tint ws, Tint w, Tint h, Tfloat bgcolr, Tfloat bgcolg, Tfloat bgcolb )
{
  CMN_KEY_DATA data;

  TsmGetWSAttri( ws, WSDbuff, &data );

  glMatrixMode(GL_MODELVIEW);
  glViewport( 0, 0, w, h);

  /*
  * CAL mai 1998
  * Contournement bug SGI sur Octane (PRO12899)
  * Bavures de pixels lors de la copie de vue 3d
  */
  glDisable (GL_SCISSOR_TEST);

#ifdef TRACE
  printf("OPENGL: TelInitWS: glClearColor %d \n", ws);
#endif
  if( data.ldata == TOn )
  {
#ifndef WNT
    if (TelTestPixmapDB())
    {
      glDrawBuffer(GL_FRONT);
      glClearColor(bgcolr, bgcolg, bgcolb, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
    }
    else
    {
      /* QTOCC_PATCH by PCD: the frame buffer should not be cleared here
      to avoid flicker. It is cleared properly in TelClearViews() 
      called by call_func_redraw_all_structs_begin() */
      glDrawBuffer(GL_BACK);
    }
#else
    /* QTOCC_PATCH by PCD: the frame buffer should not be cleared here
    to avoid flicker. It is cleared properly in TelClearViews() 
    called by call_func_redraw_all_structs_begin() */
    glDrawBuffer(GL_BACK);
#endif /* WNT */
  }
  else
  {
    glClearColor(bgcolr, bgcolg, bgcolb, ( float )1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }
  return;

}


/*----------------------------------------------------------------------*/
void TelSwapBuffers ( Tint ws ) {

#ifndef WNT

  CMN_KEY_DATA data;

  if (TelTestPixmapDB())
  {
    glFlush();
    XCopyArea(call_thedisplay, pixmap_id, window_id,
      window_gc, 0, 0,
      window_width, window_height, 0, 0);
  }
  else
  {
    TsmGetWSAttri( ws, WSWindow, &data );
    glXSwapBuffers ( call_thedisplay, data.ldata );
  }

#else

  SwapBuffers ( wglGetCurrentDC () );
  TelFlush(0);

#endif  /* WNT */

  TelSetBackBufferRestored (TOff);

#ifdef TRACE
  printf("OPENGL: TelSwapBuffers: glXSwapBuffers %d \n", ws);
#endif

  return;

}  /* end TelSwapBuffers */

/*----------------------------------------------------------------------*/
Tint
TelBackBufferRestored ()
{
  return call_back_buffer_restored;
}

/*----------------------------------------------------------------------*/
void
TelSetBackBufferRestored (Tint flag)
{
#ifdef TRACE
  printf("OPENGL: TelSetBackBufferRestored(%d): \n",flag);
#endif
  call_back_buffer_restored = flag;
}

/*----------------------------------------------------------------------*/
void
TelCopyBuffers(Tint ws, GLenum from, GLenum to,
               Tfloat xm, Tfloat ym, Tfloat zm, Tfloat XM, Tfloat YM, Tfloat ZM, Tint flag)
{
  CMN_KEY_DATA key;
  Tint w, h;

#ifdef TRACE
  printf("OPENGL: TelCopyBuffers: \n");
#endif

  if (to == GL_BACK) TelSetBackBufferRestored (TOff);

#ifndef WNT
  if (TelTestPixmapDB())
  {
#ifdef TRACE
    printf("OPENGL: TelSwapBuffers: glFlush \n");
#endif
    glFlush();
    XCopyArea(window_display, pixmap_id, window_id,
      window_gc, 0, 0,
      window_width, window_height, 0, 0);
    return;
  }
#endif /* WNT */

  TsmGetWSAttri (ws, WSWidth, &key);
  w = key.ldata;
  TsmGetWSAttri (ws, WSHeight, &key);
  h = key.ldata;

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();
  gluOrtho2D ((GLdouble) 0., (GLdouble) w, 0., (GLdouble) h);
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadIdentity ();

  TelDisable (ws);
  if (flag) 
  {
    /*
    * calcul de la projection de la boite
    * et copie du rectangle projete
    */
    Tint i;
    GLsizei width, height;
    Tfloat xmr, ymr, XMR, YMR;
    Tfloat xr[8], yr[8];
    /*
    * Projection de la boite englobante
    */
    if ((TelProjectionRaster (ws, xm, ym, zm, &xr[0], &yr[0]) == TSuccess)
      &&  (TelProjectionRaster (ws, xm, YM, zm, &xr[1], &yr[1]) == TSuccess)
      &&  (TelProjectionRaster (ws, XM, YM, zm, &xr[2], &yr[2]) == TSuccess)
      &&  (TelProjectionRaster (ws, XM, ym, zm, &xr[3], &yr[3]) == TSuccess)
      &&  (TelProjectionRaster (ws, xm, ym, ZM, &xr[4], &yr[4]) == TSuccess)
      &&  (TelProjectionRaster (ws, xm, YM, ZM, &xr[5], &yr[5]) == TSuccess)
      &&  (TelProjectionRaster (ws, XM, YM, ZM, &xr[6], &yr[6]) == TSuccess)
      &&  (TelProjectionRaster (ws, XM, ym, ZM, &xr[7], &yr[7]) == TSuccess)) 
    {
      xmr = ymr = (float ) shortreallast ();
      XMR = YMR = (float ) shortrealfirst ();
      /*
      * Recherche du rectangle projete
      */
      for (i=0; i<8; i++) {
        if (xmr > xr[i]) xmr = xr[i];
        if (ymr > yr[i]) ymr = yr[i];
        if (XMR < xr[i]) XMR = xr[i];
        if (YMR < yr[i]) YMR = yr[i];
      }
      /* pour eviter les bavures de pixels ! */
      xmr--;ymr--;
      XMR++;YMR++;

      /*
      * Ajout CAL : 10/05/96
      * Si les MinMax viennent d'un ensemble de markers
      * on ne tient pas compte du scale factor de ceux-ci
      * dans les valeurs de MinMax. En effet, ce facteur
      * est dans l'espace pixel et les MinMax dans l'espace
      * du modele. Donc ajout d'un delta de pixels
      * en esperant que les applis n'utilisent pas des
      * markers tres gros !
      */
      xmr -= CALL_DEF_DELTA; ymr -= CALL_DEF_DELTA;
      XMR += CALL_DEF_DELTA; YMR += CALL_DEF_DELTA;

      /*
      * Le rectangle projete peut-etre clippe
      */
      width = (GLsizei) (XMR-xmr+1);
      height = (GLsizei) (YMR-ymr+1);
#ifdef COPYBUFFER
      printf ("avant clipping\n");
      printf ("xm, ym, zm : %f, %f, %f\n", xm, ym, zm);
      printf ("XM, YM, ZM : %f, %f, %f\n", XM, YM, ZM);
      printf ("taille fenetre : %d, %d\n", w, h);
      printf ("xmr, ymr by GLU : %f, %f\n", xmr, ymr);
      printf ("YMR, YMR by GLU : %f, %f\n", XMR, YMR);
      printf ("copie x, y, dx, dy : %d, %d, %d, %d\n\n",
        (int) xmr, (int) ymr, (int) width, (int) height);
#endif
      /*
      * (xmr,ymr) coin inferieur gauche
      * (XMR,YMR) coin superieur droit
      */
      /* cas ou 1 coin est en dehors de la fenetre */
      if (xmr < 0) { width  = (GLsizei) (XMR+1); xmr = 0; }
      if (ymr < 0) { height = (GLsizei) (YMR+1); ymr = 0; }
      if (XMR > w) { width  = (GLsizei) (w-xmr+1); }
      if (YMR > h) { height = (GLsizei) (h-ymr+1); }

      /* cas ou les 2 coins sont en dehors de la fenetre */
      if (XMR < 0) { xmr = 0; width = height = 1; }
      if (YMR < 0) { ymr = 0; width = height = 1; }
      if (xmr > w) { xmr = 0; width = height = 1; }
      if (ymr > h) { ymr = 0; width = height = 1; }
#ifdef COPYBUFFER
      printf ("apres clipping\n");
      printf ("xmr, ymr by GLU : %f, %f\n", xmr, ymr);
      printf ("YMR, YMR by GLU : %f, %f\n", XMR, YMR);
      printf ("copie x, y, dx, dy : %d, %d, %d, %d\n\n",
        (int) xmr, (int) ymr, (int) width, (int) height);
#endif
      glDrawBuffer (to);
      glReadBuffer (from);
      /* copie partielle */
      glRasterPos2i ((GLint) xmr, (GLint) ymr);
      glCopyPixels ((GLint) xmr, (GLint) ymr, width, height, GL_COLOR);
      /* TelFlush (1); */
    }
    else 
    {
      glDrawBuffer (to);
      /* TelClearViews (ws); */
      glReadBuffer (from);
      /* copie complete */
      glRasterPos2i (0, 0);
      glCopyPixels (0, 0, w+1, h+1, GL_COLOR);
      /* TelFlush (1); */
    }
  }
  else 
  {
    glDrawBuffer (to);
    /* TelClearViews (ws); */
    glReadBuffer (from);
    /* copie complete */
    glRasterPos2i (0, 0);
    glCopyPixels (0, 0, w+1, h+1, GL_COLOR);
    /* TelFlush (1); */
  }
  TelEnable (ws);

  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glMatrixMode (GL_MODELVIEW);
  glPopMatrix ();

  glDrawBuffer (GL_BACK);
  return;
}

/*----------------------------------------------------------------------*/
void
TelReadImage(Tint ws, GLenum from, Tint posx, Tint posy, Tint width, Tint height, unsigned int *image)
{
  CMN_KEY_DATA key;
  Tint w, h;

#ifdef TRACE
  printf("OPENGL: TelReadImage: %d %d %d %d \n", posx, posy, width, height);
#endif

  if (image !=NULL)
  {
    TsmGetWSAttri (ws, WSWidth, &key);
    w = key.ldata;
    TsmGetWSAttri (ws, WSHeight, &key);
    h = key.ldata;

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D ((GLdouble) 0., (GLdouble) w, 0., (GLdouble) h);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    glReadBuffer(from); 

    glRasterPos2i (posx, posy);
    TelDisable (ws);
    glReadPixels (posx, posy, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
    TelEnable (ws);

    glReadBuffer(GL_BACK);        
  }

  return;
}

/*----------------------------------------------------------------------*/
void
TelDrawImage(Tint ws, GLenum to, Tint posx, Tint posy, Tint width, Tint height, unsigned int *image)
{
  CMN_KEY_DATA key;
  Tint w, h;

#ifdef TRACE
  printf("OPENGL: TelDrawImage: %d %d %d %d \n", posx, posy, width, height);
#endif

  if (image !=NULL)
  {
    TsmGetWSAttri (ws, WSWidth, &key);
    w = key.ldata;
    TsmGetWSAttri (ws, WSHeight, &key);
    h = key.ldata;

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D ((GLdouble) 0., (GLdouble) w, 0., (GLdouble) h);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    glDrawBuffer(to);

    glRasterPos2i (posx, posy);
    TelDisable (ws);
    glDrawPixels (width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
    TelEnable (ws);

    glDrawBuffer(GL_BACK);     
  }     
  return;
}

/*----------------------------------------------------------------------*/
void
TelReadDepths(Tint ws, Tint posx, Tint posy, Tint width, Tint height, float *depths)
{
  CMN_KEY_DATA key;
  Tint w, h;

#ifdef TRACE
  printf("OPENGL: TelReadDepths: %d %d %d %d \n", posx, posy, width, height);
#endif

  if ( TsmGetWSAttri (ws, WSWindow, &key) != TSuccess ) return;

  if (depths != NULL && TxglWinset (call_thedisplay, (WINDOW) key.ldata) == TSuccess)
  {
    TsmGetWSAttri (ws, WSWidth, &key);
    w = key.ldata;
    TsmGetWSAttri (ws, WSHeight, &key);
    h = key.ldata;

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D ((GLdouble) 0., (GLdouble) w, 0., (GLdouble) h);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    glRasterPos2i (posx, posy);
    TelDisable (ws);
    glReadPixels (posx, posy, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depths);
    TelEnable (ws);
  }

  return;
}

/*----------------------------------------------------------------------*/
void
TelEnable (Tint ws)
{
  /*glPixelTransferi (GL_MAP_COLOR, GL_TRUE);*/

  /* GL_DITHER on/off pour le trace */
  if (TxglGetDither())
    glEnable (GL_DITHER);
  else
    glDisable (GL_DITHER);

  return;
}

/*----------------------------------------------------------------------*/
void
TelDisable (Tint ws)
{
  glDisable (GL_DITHER);
  glPixelTransferi (GL_MAP_COLOR, GL_FALSE);

  /*
  * Disable stuff that's likely to slow down glDrawPixels.
  * (Omit as much of this as possible, when you know in advance
  * that the OpenGL state will already be set correctly.)
  */
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_FOG);
  LightOff();

  glDisable(GL_LOGIC_OP);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);
  glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
  glPixelTransferi(GL_RED_SCALE, 1);
  glPixelTransferi(GL_RED_BIAS, 0);
  glPixelTransferi(GL_GREEN_SCALE, 1);
  glPixelTransferi(GL_GREEN_BIAS, 0);
  glPixelTransferi(GL_BLUE_SCALE, 1);
  glPixelTransferi(GL_BLUE_BIAS, 0);
  glPixelTransferi(GL_ALPHA_SCALE, 1);
  glPixelTransferi(GL_ALPHA_BIAS, 0);

  /*
  * Disable extensions that could slow down glDrawPixels.
  * (Actually, you should check for the presence of the proper
  * extension before making these calls.  I've omitted that
  * code for simplicity.)
  */

#ifdef GL_EXT_convolution
  glDisable(GL_CONVOLUTION_1D_EXT);
  glDisable(GL_CONVOLUTION_2D_EXT);
  glDisable(GL_SEPARABLE_2D_EXT);
#endif

#ifdef GL_EXT_histogram
  glDisable(GL_HISTOGRAM_EXT);
  glDisable(GL_MINMAX_EXT);
#endif

#ifdef GL_EXT_texture3D
  glDisable(GL_TEXTURE_3D_EXT);
#endif

  return;
}

/*----------------------------------------------------------------------*/
TStatus
TelProjectionRaster(Tint ws, Tfloat x, Tfloat y, Tfloat z, Tfloat *xr, Tfloat *yr)
{
  Tint w, h;
  CMN_KEY_DATA key;
  Tint vid;          /* View index */
  TEL_VIEW_REP vrep; /* View definition */

  GLint status;

  int i, j, k;
  GLdouble objx, objy, objz;
  GLdouble modelMatrix[16], projMatrix[16];
  GLint viewport[4];
  GLdouble winx, winy, winz;

  vid = ws;

  if (TelGetViewRepresentation (ws, vid, &vrep) != TSuccess)
    return TFailure;

  TsmGetWSAttri (ws, WSWidth, &key);
  w = key.ldata;
  TsmGetWSAttri (ws, WSHeight, &key);
  h = key.ldata;

  objx = ( GLdouble )x; objy = ( GLdouble )y; objz = ( GLdouble )z;

  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      modelMatrix[k] = ( GLdouble )vrep.orientation_matrix[i][j];

  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      projMatrix[k] = ( GLdouble )vrep.mapping_matrix[i][j];

  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = w;
  viewport[3] = h;

  /*
  * glGetIntegerv (GL_VIEWPORT, viewport);
  * glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
  * glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);
  */

  status = gluProject (objx, objy, objz,
    modelMatrix, projMatrix, viewport,
    &winx, &winy, &winz);

  if (status == GL_TRUE) {
    *xr = ( Tfloat )winx;
    *yr = ( Tfloat )winy;
    return TSuccess;
  }
  else 
  {
    *xr = 0.0F;
    *yr = 0.0F;
    return TFailure;
  }
}

/*----------------------------------------------------------------------*/
TStatus
TelUnProjectionRaster(Tint ws, Tint xr, Tint yr, Tfloat *x, Tfloat *y, Tfloat *z)
{
  Tint w, h;
  CMN_KEY_DATA key;
  Tint vid;          /* View index */
  TEL_VIEW_REP vrep; /* View definition */

  int i, j, k;
  GLdouble objx, objy, objz;
  GLdouble modelMatrix[16], projMatrix[16];
  GLint viewport[4];
  GLdouble winx, winy, winz;
  GLint status;

  vid = ws;

  if (TelGetViewRepresentation (ws, vid, &vrep) != TSuccess)
    return TFailure;

  TsmGetWSAttri (ws, WSWidth, &key);
  w = key.ldata;
  TsmGetWSAttri (ws, WSHeight, &key);
  h = key.ldata;

  winx = ( GLdouble )xr; winy = ( GLdouble )yr; winz = ( GLdouble )0.0;

  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      modelMatrix[k] = ( GLdouble )vrep.orientation_matrix[i][j];

  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      projMatrix[k] = ( GLdouble )vrep.mapping_matrix[i][j];

  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = w;
  viewport[3] = h;

  /*
  * glGetIntegerv (GL_VIEWPORT, viewport);
  * glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
  * glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);
  */

  status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
    &objx, &objy, &objz);

  if (status == GL_TRUE) {
    *x = ( Tfloat )objx;
    *y = ( Tfloat )objy;
    *z = ( Tfloat )objz;

    return TSuccess;
  }
  else {
    *x = 0.0F;
    *y = 0.0F;
    *z = 0.0F;

    return TFailure;
  }
}

/*----------------------------------------------------------------------*/
TStatus
TelUnProjectionRasterWithRay(Tint ws, Tint xr, Tint yr, Tfloat *x, Tfloat *y, Tfloat *z,
                             Tfloat *dx, Tfloat *dy, Tfloat *dz)
{
  Tint w, h;
  CMN_KEY_DATA key;
  Tint vid;          /* View index */
  TEL_VIEW_REP vrep; /* View definition */

  int i, j, k;
  GLdouble objx, objy, objz;
  GLdouble objx1, objy1, objz1;
  GLdouble modelMatrix[16], projMatrix[16];
  GLint viewport[4];
  GLdouble winx, winy, winz;
  GLint status;

  vid = ws;

  if (TelGetViewRepresentation (ws, vid, &vrep) != TSuccess)
    return TFailure;

  TsmGetWSAttri (ws, WSWidth, &key);
  w = key.ldata;
  TsmGetWSAttri (ws, WSHeight, &key);
  h = key.ldata;

  winx = ( GLdouble )xr; winy = ( GLdouble )yr; winz = ( GLdouble )0.0;

  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      modelMatrix[k] = ( GLdouble )vrep.orientation_matrix[i][j];

  for (k = 0, i = 0; i < 4; i++)
    for (j = 0; j < 4; j++, k++)
      projMatrix[k] = ( GLdouble )vrep.mapping_matrix[i][j];

  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = w;
  viewport[3] = h;

  /*
  * glGetIntegerv (GL_VIEWPORT, viewport);
  * glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
  * glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);
  */

  status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
    &objx, &objy, &objz);

  if (status == GL_TRUE) {
    *x = ( Tfloat )objx;
    *y = ( Tfloat )objy;
    *z = ( Tfloat )objz;

    winz = ( GLdouble ) -10.0;
    status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
      &objx1, &objy1, &objz1);

    if (status == GL_TRUE) {
      *dx = ( Tfloat )(objx-objx1);
      *dy = ( Tfloat )(objy-objy1);
      *dz = ( Tfloat )(objz-objz1);

      return TSuccess;
    }
    else {
      *dx = 0.0F;
      *dy = 0.0F;
      *dz = 0.0F;

      return TFailure;
    }
  }
  else {
    *x = 0.0F;
    *y = 0.0F;
    *z = 0.0F;
    *dx = 0.0F;
    *dy = 0.0F;
    *dz = 0.0F;

    return TFailure;
  }
}

/*----------------------------------------------------------------------*/
void
TelFlush(Tint wait)
{
  if (wait)
  {    
#ifdef TRACE
    printf("OPENGL: TelFlush: glFinish \n");
#endif
    glFinish ();
  }
  else
  {
#ifdef TRACE
    printf("OPENGL: TelFlush: glFlush \n");
#endif
    glFlush();
  }
  return;
}

/*----------------------------------------------------------------------*/
Tint
TelIsBackFacePerspective(Tmatrix3 n, Tfloat *p1, Tfloat *p2, Tfloat *p3 )
{
  Tfloat    r1[4], r2[4], r3[4], m[4], norm[4];

  veccpy( m, p1 );
  m[3] = ( float )1.0;
  TelTranpt3( r1, m, n );
  r1[0] /= r1[3];
  r1[1] /= r1[3];
  r1[2] /= r1[3];

  veccpy( m, p2 );
  m[3] = ( float )1.0;
  TelTranpt3( r2, m, n );
  r2[0] /= r2[3];
  r2[1] /= r2[3];
  r2[2] /= r2[3];

  veccpy( m, p3 );
  m[3] = ( float )1.0;
  TelTranpt3( r3, m, n );
  r3[0] /= r3[3];
  r3[1] /= r3[3];
  r3[2] /= r3[3];
  TelGetNormal( r1, r2, r3, norm );

  return norm[2] < 0.0;
}

/*----------------------------------------------------------------------*/
