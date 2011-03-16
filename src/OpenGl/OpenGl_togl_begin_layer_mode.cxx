/***********************************************************************

FONCTION :
----------
file OpenGl_togl_begin_layer_mode.c :

REMARQUES:
----------

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
01-12-98 : CAL ; Creation. S4062. Ajout des layers.
02.14.100 : JR : Warnings on WNT truncations from double to float

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if defined(HAVE_MALLOC_H) || defined(WNT)
#include <malloc.h>
#endif

#include <OpenGl_tgl_all.hxx>

#include <math.h>
#include <GL/gl.h>
#include <stdio.h>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_txgl.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_telem_filters.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_telem_view.hxx>

#include <OpenGl_TextRender.hxx>

/*OCC6247*/
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_LightBox.hxx>

#include <Visual3d_Layer.hxx>

#include <OpenGl_Extension.hxx>

/*----------------------------------------------------------------------*/
/*
* Constantes
*/

#define NO_PRINT
#define NOTRACE
#define NO_DEBUG
#define NO_TRACE_MAT
#define NO_TRACE_TEXT

#define IsNotModified 0
#define IsModified 1

/*----------------------------------------------------------------------*/
/*
* Variables externes
*/

extern GLuint linestyleBase;
extern GLuint fontBase;

#if defined(__sun)  || defined (__osf__) || defined (__hp) || defined (__sgi)
#else
extern PFNGLBLENDEQUATIONEXTPROC glBlendEquationOp;
#endif
/*----------------------------------------------------------------------*/
/*
* Prototypes fonctions
*/

/*----------------------------------------------------------------------*/
/*
* Prototypes Private functions
*/

/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/

static CALL_DEF_LAYER ACLayer;

static GLboolean layerOpen      = GL_FALSE;
static float     layerHeight    = -1.;
static float     layerWidth     = -1.;
static int       layerPoints    = 0;
static Tchar    *layerFont      = NULL;
static int       layerType      = -1;
static int       layerFontFlag  = IsModified;
static int       layerFontType  = 0;
static float     layerRed       = -1.;
static float     layerGreen     = -1.;
static float     layerBlue      = -1.;
static float     layerFontRed   = -1.;
static float     layerFontGreen = -1.;
static float     layerFontBlue  = -1.;


static OSD_FontAspect FTGLLayerFontAspect = OSD_FA_Regular;
static Tint           FTGLLayerFontHeight = 16;
static Tint           FTGLLayerFontAscent = 0;
static Tint           FTGLLayerFontDescent = 0;
static Tint           FTGLLayerFontWidth = 0;
static Tfloat         FTGLLayerFontXScale = 1.f;
static Tfloat         FTGLLayerFontYScale = 1.f;
static Tchar          FTGLLayerFontName[100];
static Tint           FTGLLayerFontCurrent = 0;

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_layer2d
(
 CALL_DEF_LAYER * alayer
 )
{
  //call_def_ptrLayer ptrLayer;

#ifdef PRINT
  printf ("call_togl_layer2d\n");
#endif
  alayer->ptrLayer = (call_def_ptrLayer) malloc (sizeof (CALL_DEF_PTRLAYER)); 
  alayer->ptrLayer->listIndex = glGenLists (1);
  ACLayer = *alayer;
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_removelayer2d
(
 CALL_DEF_LAYER * alayer
 )
{
  if (alayer->ptrLayer == NULL) return;
  if (alayer->ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_removelayer2d %d\n", ptrLayer->listIndex);
#endif

  /*
  * On detruit la display-list associee au layer.
  */
  glDeleteLists (alayer->ptrLayer->listIndex, 1);
  alayer->ptrLayer->listIndex = 0;
  ACLayer = *alayer;
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_clear_layer2d (
                         CALL_DEF_LAYER * aLayer
                         )
{
  if (aLayer->ptrLayer == NULL) return;
  if (aLayer->ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_clear_layer2d %d\n", ptrLayer->listIndex);
#endif

  /*
  * On clear la display-list associee au layer.
  */
  /*
  * On detruit la display-list associee au layer.
  */
  glDeleteLists (aLayer->ptrLayer->listIndex, 1);
  aLayer->ptrLayer->listIndex = 0;
  /*
  * On cree la display-list associee au layer.
  */
  aLayer->ptrLayer->listIndex = glGenLists (1);
  //alayer->ptrLayer = /*(void *) */ptrLayer;
  ACLayer = *aLayer;
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_redraw_layer2d (
                          CALL_DEF_VIEW * aview,
                          CALL_DEF_LAYER * alayer
                          )
{
  float left, right, bottom, top;
  float delta, ratio;
  int attach;
#ifdef TRACE_MAT
  GLdouble modelMatrix[16], projMatrix[16];
  GLint viewport[4];
  int i, j;
#endif
  GLsizei dispWidth, dispHeight;
  GLboolean isl = IsLightOn(); /*OCC6247*/

  if ( aview->DefBitmap.bitmap ) {
    dispWidth = aview->DefBitmap.width;
    dispHeight = aview->DefBitmap.height;
  }
  else {
    dispWidth = (GLsizei) alayer->viewport[0];
    dispHeight = (GLsizei) alayer->viewport[1];
  }

  if (alayer->ptrLayer == NULL) return;
  if (alayer->ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_redraw_layer2d %d\n", ptrLayer->listIndex);
  printf ("\twindow size %f %f\n",
    aview->DefWindow.dx, aview->DefWindow.dy);
  printf ("\tviewport %f %f\n", (float) dispWidth, (float) dispHeight);
#endif

  if(isl) {
    LightOff(); /*OCC6247*/
  }

  /*
  * On positionne la projection
  */
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix ();
  glLoadIdentity ();

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();

  if (alayer->sizeDependent == 0)
    glViewport (0, 0, dispWidth, dispHeight);

  left = alayer->ortho[0],
    right = alayer->ortho[1],
    bottom = alayer->ortho[2],
    top = alayer->ortho[3];
  attach = alayer->attach;
#ifdef TRACE
  printf ("\tattach %d ortho %f %f %f %f %d\n",
    attach, left, right, bottom, top);
#endif
  if (alayer->sizeDependent == 0)
    ratio = (float) dispWidth/dispHeight;
  else
    ratio = aview->DefWindow.dx/aview->DefWindow.dy;
  if (ratio >= 1.0) { /* fenetre horizontale */
    delta = (float )((top - bottom)/2.0);
    switch (attach) {
      case 0: /* Aspect_TOC_BOTTOM_LEFT */
        top = bottom + 2*delta/ratio;
        break;
      case 1: /* Aspect_TOC_BOTTOM_RIGHT */
        top = bottom + 2*delta/ratio;
        break;
      case 2: /* Aspect_TOC_TOP_LEFT */
        bottom = top - 2*delta/ratio;
        break;
      case 3: /* Aspect_TOC_TOP_RIGHT */
        bottom = top - 2*delta/ratio;
        break;
    }
  }
  else { /* fenetre verticale */
    delta = (float )((right - left)/2.0);
    switch (attach) {
      case 0: /* Aspect_TOC_BOTTOM_LEFT */
        right = left + 2*delta*ratio;
        break;
      case 1: /* Aspect_TOC_BOTTOM_RIGHT */
        left = right - 2*delta*ratio;
        break;
      case 2: /* Aspect_TOC_TOP_LEFT */
        right = left + 2*delta*ratio;
        break;
      case 3: /* Aspect_TOC_TOP_RIGHT */
        left = right - 2*delta*ratio;
        break;
    }
  }
#ifdef TRACE
  printf ("\tratio %f new ortho %f %f %f %f\n",
    ratio, left, right, bottom, top);
#endif
  glOrtho (left, right, bottom, top, -1.0, 1.0);

#ifdef TRACE_MAT
  glGetIntegerv (GL_VIEWPORT, viewport);
  glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
  glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);
  printf ("viewport :\n");
  printf ("\t");
  for (i = 0; i < 4; i++)
    printf ("%d ", viewport[i]);
  printf ("\n");
  printf ("modelMatrix :\n");
  for (i = 0; i < 4; i++) {
    printf ("\t");
    for (j = 0; j < 4; j++)
      printf ("%f ", modelMatrix[i*4+j]);
    printf ("\n");
  }
  printf ("projMatrix :\n");
  for (i = 0; i < 4; i++) {
    printf ("\t");
    for (j = 0; j < 4; j++)
      printf ("%f ", projMatrix[i*4+j]);
    printf ("\n");
  }
#endif

  /*
  * On trace la display-list associee au layer.
  */
#ifdef TRACE
  if (glIsList (alayer->ptrLayer->listIndex))
    printf ("\t%d is a list\n", alayer->ptrLayer->listIndex);
  else
    printf ("\t%d is not a list\n", alayer->ptrLayer->listIndex);
#endif
  glPushAttrib (
    GL_LIGHTING_BIT | GL_LINE_BIT | GL_POLYGON_BIT |
    GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT);
  glDisable (GL_DEPTH_TEST);
  glCallList (alayer->ptrLayer->listIndex);


  //calling dynamic render of LayerItems
  if ( alayer->ptrLayer->layerData ) {
    ACLayer = *alayer;
    Visual3d_Layer* layer = (Visual3d_Layer*)alayer->ptrLayer->layerData;
    layer->RenderLayerItems();
    ACLayer.ptrLayer = NULL;
  }


  glPopAttrib ();

  /*
  * On retire la projection
  */
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();

  glMatrixMode( GL_MODELVIEW );
  glPopMatrix ();

  /*
  * Restauration du Viewport en cas de modification
  */
  if (alayer->sizeDependent == 0) 
    glViewport (0, 0,
    (GLsizei) aview->DefWindow.dx,
    (GLsizei) aview->DefWindow.dy);

  glFlush ();

  if(isl) {
    LightOn(); /*OCC6247*/
  }

}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_begin_layer2d
(
 CALL_DEF_LAYER * alayer
 )
{
  call_def_ptrLayer ptrLayer;

  ACLayer = *alayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_begin_layer2d %d\n", ptrLayer->listIndex);
#endif

  //abd
  glEnable(GL_TEXTURE_2D);
  GLboolean stat = glIsEnabled( GL_TEXTURE_2D );

  glNewList (ptrLayer->listIndex, GL_COMPILE);
  layerOpen = GL_TRUE;
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_end_layer2d
(
 )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_end_layer2d %d\n", ptrLayer->listIndex);
#endif

  /*
  * On ferme la display-list associee au layer.
  */
  if (layerOpen) 
    glEndList ();
  layerOpen = GL_FALSE;

  ACLayer.ptrLayer = NULL;
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_begin_polyline2d (
                            )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_begin_polyline2d %d\n", ptrLayer->listIndex);
#endif

  layerPoints = 0;
  glBegin (GL_LINE_STRIP);
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_end_polyline2d (
                          )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_end_polyline2d %d\n", ptrLayer->listIndex);
#endif

  glEnd ();
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_begin_polygon2d (
                           )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_begin_polygon2d %d\n", ptrLayer->listIndex);
#endif

  layerPoints = 0;
  glBegin (GL_POLYGON);
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_end_polygon2d (
                         )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_end_polygon2d %d\n", ptrLayer->listIndex);
#endif

  glEnd ();
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_edge2d (
                  float x,
                  float y
                  )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_edge2d %d\n", ptrLayer->listIndex);
  printf ("\tcoord %f %f\n", x, y);
#endif

  layerPoints ++;
  glVertex3f (x, y, 0.);
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_draw2d
(
 float x,
 float y
 )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_draw2d %d\n", ptrLayer->listIndex);
  printf ("\tcoord %f %f\n", x, y);
#endif

  layerPoints ++;
  glVertex3f (x, y, 0.);
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_move2d
(
 float x,
 float y
 )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_move2d %d\n", ptrLayer->listIndex);
  printf ("\tcoord %f %f\n", x, y);
#endif

  if (layerPoints != 0) {
    call_togl_end_polyline2d ();
    call_togl_begin_polyline2d ();
  }

  layerPoints ++;
  glVertex3f (x, y, 0.);
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_rectangle2d
(
 float x,
 float y,
 float w,
 float h
 )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_rectangle2d %d\n", ptrLayer->listIndex);
  printf ("\trectangle %f %f %f %f\n", x, y, w, h);
#endif

  glRectf (x, y, x + w, y + h);
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_text2d ( char *s,
                  float x,
                  float y,
                  float height )
{
  call_def_ptrLayer ptrLayer;
#ifndef WNT
  Tint dispWidth, dispHeight;
#endif
  GLdouble objx1, objy1, objz1;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef TRACE_TEXT
  printf ("call_togl_text2d %d\n", ptrLayer->listIndex);
  printf ("\ttext %s %f %f %f\n", s, x, y, height);
#endif

  OpenGl_TextRender* textRender=OpenGl_TextRender::instance();

  FTGLLayerFontCurrent = textRender->FindFont( FTGLLayerFontName,
                                               FTGLLayerFontAspect,
                                               FTGLLayerFontHeight,
                                               FTGLLayerFontXScale,
                                               FTGLLayerFontYScale );

  objx1 = x,
  objy1 = y,
  objz1 = 0.0;

  /*
  * On traite les differents types d'affichage de texte
  */
  switch (layerFontType) {
  case 0 : /* Aspect_TODT_Normal */
#ifdef DEBUG
    printf ("---------------------\n");
    printf ("texte normal %f %f\n", objx1, objy1);
#endif

    textRender->RenderText(s , fontBase, 1, (float )objx1, (float )objy1, 0.f );

    break;
  case 1 : /* Aspect_TODT_SubTitle */
#ifdef OK
#ifdef DEBUG
    printf ("---------------------\n");
    printf ("texte subtitle %f %f\n", objx1, objy1);
#endif
    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);
#ifdef TRACE_MAT
    printf ("viewport :\n");
    printf ("\t");
    for (i = 0; i < 4; i++)
      printf ("%d ", viewport[i]);
    printf ("\n");
    printf ("modelMatrix :\n");
    for (i = 0; i < 4; i++) {
      printf ("\t");
      for (j = 0; j < 4; j++)
        printf ("%f ", modelMatrix[i*4+j]);
      printf ("\n");
    }
    printf ("projMatrix :\n");
    for (i = 0; i < 4; i++) {
      printf ("\t");
      for (j = 0; j < 4; j++)
        printf ("%f ", projMatrix[i*4+j]);
      printf ("\n");
    }
#endif
    /*
    * dimensionString("Methode cadre", &sWidth, &sAscent, &sDescent);
    */
    sWidth = 2.;
    sAscent = .2;
    sDescent = .2;

    objx2 = objx1; objy2 = objy1; objz2 = objz1;
    objx3 = objx1; objy3 = objy1; objz3 = objz1;
    objx4 = objx1; objy4 = objy1; objz4 = objz1;
    objx5 = objx1; objy5 = objy1; objz5 = objz1;

    status = gluProject (objx1, objy1, objz1,
                         modelMatrix, projMatrix, viewport,
                         &winx1, &winy1, &winz1);

    winx2 = winx1;
    winy2 = winy1-sDescent;
    winz2 = winz1+0.00001;
    status = gluUnProject (winx2, winy2, winz2,
                           modelMatrix, projMatrix, viewport,
                           &objx2, &objy2, &objz2);

    winx2 = winx1 + sWidth;
    winy2 = winy1-sDescent;
    winz2 = winz1+0.00001; /* il vaut mieux F+B / 1000000 ? */
    status = gluUnProject (winx2, winy2, winz2,
                           modelMatrix, projMatrix, viewport,
                           &objx3, &objy3, &objz3);

    winx2 = winx1 + sWidth;
    winy2 = winy1 + sAscent;
    winz2 = winz1+0.00001;
    status = gluUnProject (winx2, winy2, winz2,
                           modelMatrix, projMatrix, viewport,
                           &objx4, &objy4, &objz4);

    winx2 = winx1;
    winy2 = winy1+ sAscent;
    winz2 = winz1+0.00001;
    status = gluUnProject (winx2, winy2, winz2,
                           modelMatrix, projMatrix, viewport,
                           &objx5, &objy5, &objz5);

    glColor3f (layerFontRed, layerFontGreen, layerFontBlue);
    glBegin(GL_POLYGON);
    glVertex3d(objx2, objy2, objz2);
    glVertex3d(objx3, objy3, objz3);
    glVertex3d(objx4, objy4, objz4);
    glVertex3d(objx5, objy5, objz5);
    glEnd();
    glColor3f (layerRed, layerGreen, layerBlue);
#endif /* OK */


    textRender->RenderText(s, fontBase, 1, (float )objx1, (float )objy1, 0.f); 

    break;
  case 2 : /* Aspect_TODT_Dekale */
#ifdef OK
#ifdef DEBUG
    printf ("---------------------\n");
    printf ("texte dekale %f %f\n", objx1, objy1);
#endif
    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);
#ifdef DEBUG
    printf ("viewport :\n");
    printf ("\t");
    for (i = 0; i < 4; i++)
      printf ("%d ", viewport[i]);
    printf ("\n");
    printf ("modelMatrix :\n");
    for (i = 0; i < 4; i++) {
      printf ("\t");
      for (j = 0; j < 4; j++)
        printf ("%f ", modelMatrix[i*4+j]);
      printf ("\n");
    }
    printf ("projMatrix :\n");
    for (i = 0; i < 4; i++) {
      printf ("\t");
      for (j = 0; j < 4; j++)
        printf ("%f ", projMatrix[i*4+j]);
      printf ("\n");
    }
#endif
    status = gluProject (objx1, objy1, objz1,
      modelMatrix, projMatrix, viewport,
      &winx1, &winy1, &winz1);
#ifdef DEBUG
    printf ("status %s\n", (status == GL_FALSE ? "ko" : "ok"));
#endif
    winx2 = winx1+1;
    winy2 = winy1+1;
    winz2 = winz1+0.00001;
    status = gluUnProject (winx2, winy2, winz2,
      modelMatrix, projMatrix, viewport,
      &objx2, &objy2, &objz2);
#ifdef DEBUG
    printf ("status %s\n", (status == GL_FALSE ? "ko" : "ok"));
#endif
    glColor3f (layerFontRed, layerFontGreen, layerFontBlue);


    OpenGl_TextRender* textRender=OpenGl_TextRender::instance();
    textRender->RenderText(s, fontBase, 1, objx2, objy2, 0.f);


    winx2 = winx1-1;
    winy2 = winy1-1;
    winz2 = winz1+0.00001;
    status = gluUnProject (winx2, winy2, winz2,
      modelMatrix, projMatrix, viewport,
      &objx2, &objy2, &objz2);
#ifdef DEBUG
    printf ("status %s\n", (status == GL_FALSE ? "ko" : "ok"));
#endif

    textRender->RenderText(s, fontBase, 1, objx2, objy2, 0.f);


    winx2 = winx1-1;
    winy2 = winy1+1;
    winz2 = winz1+0.00001;
    status = gluUnProject (winx2, winy2, winz2,
      modelMatrix, projMatrix, viewport,
      &objx2, &objy2, &objz2);
#ifdef DEBUG
    printf ("status %s\n", (status == GL_FALSE ? "ko" : "ok"));
#endif


    textRender->RenderText(s, fontBase, 1, objx2, objy2, 0.f);

    winx2 = winx1+1;
    winy2 = winy1-1;
    winz2 = winz1+0.00001;
    status = gluUnProject (winx2, winy2, winz2,
      modelMatrix, projMatrix, viewport,
      &objx2, &objy2, &objz2);
#ifdef DEBUG
    printf ("status %s\n", (status == GL_FALSE ? "ko" : "ok"));
#endif

    textRender->RenderText(s, fontBase, 1, objx2, objy2, 0.f);

    glColor3f (layerRed, layerGreen, layerBlue);
#endif /* OK */

    textRender->RenderText(s, fontBase, 1, (float )objx1, (float )objy1, 0.f);

#ifdef DEBUG
    printf ("---------------------\n");
#endif
    break;
  case 3 : /* Aspect_TODT_Blend */
#ifdef OK
#ifndef WNT
#ifndef __sun
    if ( (PFNGLBLENDEQUATIONEXTPROC)glBlendEquationOp == (PFNGLBLENDEQUATIONEXTPROC)INVALID_EXT_FUNCTION_PTR )
    {
      if(QueryExtension("GL_EXT_blend_logic_op"))   
        glBlendEquationOp = (PFNGLBLENDEQUATIONEXTPROC)glXGetProcAddress((GLubyte*)"glBlendEquationOp");
      else
	glBlendEquationOp = NULL;
    }

    if ( glBlendEquationOp )
#endif //__sun
    {
      glEnable(GL_BLEND);
#ifndef __sun
      glBlendEquationOp(GL_LOGIC_OP);
#else
      glBlendEquation(GL_LOGIC_OP);
#endif //__sun
      glLogicOp(GL_XOR); 
      glColor3f (layerRed, layerGreen, layerBlue);
      glRasterPos2f (objx1, objy1);             
      glDisable (GL_BLEND);
    }
#else
    glColor3f (layerRed, layerGreen, layerBlue);

    textRender->RenderText(s, fontBase, 1, (float )objx1, (float )objy1, 0.f);


#endif //WNT

#else /* OK */
#ifdef DEBUG
    printf ("---------------------\n");
    printf ("texte blend %f %f\n", objx1, objy1);
#endif

    textRender->RenderText(s, fontBase, 1, (float )objx1, (float )objy1, 0.f);

#ifdef DEBUG
    printf ("---------------------\n");
#endif
#endif /* OK */
    break;
  }
}

void EXPORT
call_togl_textsize2d
(
 char *s,
 float height,
 float *width,
 float *ascent,
 float *descent
 )
{
  call_def_ptrLayer ptrLayer;
#ifndef WNT
  Tint dispWidth, dispHeight;
#endif


  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;


  if ( FTGLLayerFontHeight != height || layerFontFlag == IsModified || FTGLLayerFontCurrent == 0 ) {
    layerFontFlag = IsNotModified;
    FTGLLayerFontHeight = height;  
    OpenGl_TextRender*  textRender =  OpenGl_TextRender::instance();
    FTGLLayerFontCurrent = textRender -> FindFont(FTGLLayerFontName, FTGLLayerFontAspect, FTGLLayerFontHeight);
    textRender -> StringSize(s, &FTGLLayerFontWidth, &FTGLLayerFontAscent, &FTGLLayerFontDescent);
  }

  
  *width = (float) FTGLLayerFontWidth;
  *ascent = (float) FTGLLayerFontAscent;
  *descent = (float) FTGLLayerFontDescent;

}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_set_color
(
 float r,
 float g,
 float b
 )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_set_color %d\n", ptrLayer->listIndex);
  printf ("\tcolor %f %f %f\n", r, g, b);
#endif

  layerRed = r,
    layerGreen = g,
    layerBlue = b;
  glColor3f (r, g, b);
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_set_transparency
(
 float a
 )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_set_transparency %d\n", ptrLayer->listIndex);
  printf ("\ttransparency %f\n", a);
#endif

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f (layerRed, layerGreen, layerBlue, a);
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_unset_transparency
(
 )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_set_untransparency %d\n", ptrLayer->listIndex);
#endif

  glDisable (GL_BLEND);
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_set_line_attributes
(
 long type,
 float width
 )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_set_line_attributes %d\n", ptrLayer->listIndex);
  printf ("\ttype, width %d %f\n", type, width);
#endif
  if (layerType != type) {
    layerType = type;
    if (layerType == 0) { /* TOL_SOLID */
      glDisable (GL_LINE_STIPPLE);
    }
    else {
      if (layerType) {
        glCallList (linestyleBase+layerType);
        glEnable (GL_LINE_STIPPLE);
      }
    }
  }
  if (layerWidth != width) {
    layerWidth = width;
    glLineWidth ((GLfloat) layerWidth);
  }
}

/*----------------------------------------------------------------------*/
void EXPORT
call_togl_set_text_attributes
(
 Tchar* font,
 long type,
 float r,
 float g,
 float b
 )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;
#ifdef PRINT
  printf ("call_togl_set_text_attributes %d\n", ptrLayer->listIndex);
  printf ("\tfont %s type %ld\n", font, type);
  printf ("\tcolor %f %f %f\n", r, g, b);
#endif

  if ( strcmp( (char*)FTGLLayerFontName, (char*)font ) != 0 ) {
    FTGLLayerFontName[0] = '\0';
    strcat( (char*)FTGLLayerFontName, (char*)font );
    FTGLLayerFontAspect = OSD_FA_Regular;

    OpenGl_TextRender*  textRender =  OpenGl_TextRender::instance();
    FTGLLayerFontCurrent = textRender -> FindFont(FTGLLayerFontName, FTGLLayerFontAspect, FTGLLayerFontHeight);

    layerFontType = type;
    layerFontRed = r;
    layerFontGreen = g;
    layerFontBlue = b;         
    layerFontFlag = IsModified; 
  }
  else 
    return;                                   
}

void call_togl_set_text_attributes( Tchar* font,
                                    Tint height,
                                    OSD_FontAspect aspect,
                                    float r,
                                    float g,
                                    float b )
{
  call_def_ptrLayer ptrLayer;

  ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (ptrLayer == NULL) return;
  if (ptrLayer->listIndex == 0) return;

#ifdef PRINT
  printf ("call_togl_set_text_attributes %d\n", ptrLayer->listIndex);
  printf ("\tfont %s type \n", font);
  printf ("\tcolor %f %f %f\n", r, g, b);
#endif
  /*
  * On gere le type de font (ASCII !)
  */
  if ( strcmp( (char*)FTGLLayerFontName, (char*)font ) != 0 &&
       FTGLLayerFontAspect == aspect && 
       FTGLLayerFontHeight == height
      ) 
  {
    FTGLLayerFontName[0] = '\0';
    strcat( (char*)FTGLLayerFontName, (char*)font );
    FTGLLayerFontHeight = height;
    FTGLLayerFontAspect = aspect;
    layerFontFlag = IsModified;
    FTGLLayerFontXScale = FTGLLayerFontYScale = 1.f;
    OpenGl_TextRender*  textRender =  OpenGl_TextRender::instance();
    FTGLLayerFontCurrent = textRender -> FindFont(FTGLLayerFontName, FTGLLayerFontAspect, FTGLLayerFontHeight);
   
    layerFontRed = r;
    layerFontGreen = g;
    layerFontBlue = b;
  }

}

