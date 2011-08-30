/***********************************************************************

FONCTION :
----------
File OpenGl_text :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
05-03-96 : FMN ; Suppression code inutile:
01-04-96 : CAL ; Integration MINSK portage WNT
29-04-96 : FMN ; correction warning de compilation.
21-10-96 : FMN ; Suppression LMC_COLOR fait dans OpenGl_execstruct.c
30-11-98 : FMN ; S4069 : Textes toujours visibles
02.14.100 : JR : Warnings on WNT truncations from double to float
21.06.03 : SAN : Suppress text display while in animation mode (TEXT_DEGENER)
22.01.04 : SAN : Implement texture mapped fonts on WNT only (OCC2934)

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#ifndef WNT
#include <X11/Xlib.h>
#endif
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_highlight.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_Extension.hxx>
#include <OpenGl_Memory.hxx>

#include <OpenGl_TextRender.hxx>

struct TEL_TEXT_DATA
{
  TEL_POINT      attach_pt;
  Tint           length;
  const wchar_t *sdata;
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TEL_TEXT_DATA* tel_text_data;

/*----------------------------------------------------------------------*/
/*
* Fonctions statiques
*/

static  TStatus  TextDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextInquire( TSM_ELEM_DATA, Tint, cmn_key* );

#if defined(__sun) || defined (__osf__) || defined (__hp) || defined (__sgi) 
#else
PFNGLBLENDEQUATIONEXTPROC glBlendEquationOp = (PFNGLBLENDEQUATIONEXTPROC) INVALID_EXT_FUNCTION_PTR;
#endif
/*----------------------------------------------------------------------*/
/*
* Constantes 
*/

#if defined(__osf__) || defined (__sun) || defined (__sgi) || defined(__hp9000s700) || defined(WNT)
#undef GL_EXT_blend_logic_op
#endif

/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/

/*static GLboolean lighting_mode;*/

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  TextDisplay,
  TextDisplay,
  TextAdd,
  TextDelete,
  TextPrint,
  TextInquire
};

extern GLuint fontBase;
extern GLuint printerFontBase;

#define TEXT_DEGENER

#ifdef TEXT_DEGENER
extern int    g_nDegenerateModel;
extern float  g_fSkipRatio;
#endif

/*----------------------------------------------------------------------*/

MtblPtr
TelTextInitClass( TelType* el )
{
  *el = TelText;
  return MtdTbl;
}

/*----------------------------------------------------------------------*/

static  TStatus
TextAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  Tint           i;
  TEL_POINT      def_pt = {{ ( float )0.0, ( float )0.0, ( float )0.0 }};
  tel_point      pt = &def_pt;
  tel_text_data  data;
  Techar         *str= 0;

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case TEXT_ATTACH_PT_ID:
      pt = (tel_point)k[i]->data.pdata;
      break;
    case TEXT_STRING_ID:
      str = (Techar*)k[i]->data.pdata;
      break;
    }
  }

  data = new TEL_TEXT_DATA();
  if( !data )
    return TFailure;

  //szv: instead of strlen + 1
  i = 0; while (str[i++]);

  wchar_t *wstr = new wchar_t[i];
  if( !wstr )
    return TFailure;

  data->attach_pt = *pt;
  data->length    = i;
  //szv: instead of memcpy
  i = 0; while (wstr[i++] = (wchar_t)(*str++));
  data->sdata = wstr;

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
TextDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_text_data   d;
  CMN_KEY         key, key1;
  TEL_COLOUR      colour,  colours;
  int             style,  display_type;
  GLboolean       flag_zbuffer = GL_FALSE;
  GLboolean       blend_state = GL_FALSE; 
  GLint           sWidth, sAscent, sDescent;
  GLdouble        modelMatrix[16], projMatrix[16];
  GLint           viewport[4];
  GLdouble        objrefX, objrefY, objrefZ;
  GLdouble        objX, objY, objZ;
  GLdouble        obj1X, obj1Y, obj1Z;
  GLdouble        obj2X, obj2Y, obj2Z;
  GLdouble        obj3X, obj3Y, obj3Z;
  GLdouble        winx1, winy1, winz1;
  GLdouble        winx, winy, winz;
  GLint           status;

#ifdef TEXT_DEGENER
  if ( g_nDegenerateModel > 0 && g_fSkipRatio == 1. )
    return TSuccess;
#endif

  d = (tel_text_data)data.pdata;

  if( k[0]->id == TOn )
  {                           /* Use highlight colours */
    TEL_HIGHLIGHT  hrep;

    key.id = TelHighlightIndex;
    TsmGetAttri( 1, &key );
    if( TelGetHighlightRep( TglActiveWs, key.data.ldata, &hrep )
      == TSuccess )
      colour = hrep.col;
    else
    {
      TelGetHighlightRep( TglActiveWs, 0, &hrep );
      colour = hrep.col;
    }
  }
  else
  {
    key.id = TelTextColour;
    key.data.pdata = &colour;
    TsmGetAttri( 1, &key );
  }

  key.id = TelTextStyle;
  key1.id = TelTextDisplayType;
  TsmGetAttri( 2, &key, &key1 );
  style = key.data.ldata;
  display_type = key1.data.ldata;

  /* style annotation */
  if (style == ASPECT_TOST_ANNOTATION)
  {
    flag_zbuffer = glIsEnabled(GL_DEPTH_TEST);
    if (flag_zbuffer) glDisable(GL_DEPTH_TEST);
  }

  OpenGl_TextRender* textRender=OpenGl_TextRender::instance();

  /* display type of text */
  if (display_type != ASPECT_TODT_NORMAL)
  {
    key.id = TelTextColourSubTitle;
    key.data.pdata = &colours;
    TsmGetAttri( 1, &key );

    /* Optimisation: il faudrait ne faire le Get qu'une fois par Redraw */
    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);

    switch (display_type) 
    {
    case ASPECT_TODT_BLEND:            

#if defined(GL_EXT_blend_logic_op)   
      if ( (PFNGLBLENDEQUATIONEXTPROC)glBlendEquationOp == (PFNGLBLENDEQUATIONEXTPROC)INVALID_EXT_FUNCTION_PTR )
      {
        if(QueryExtension("GL_EXT_blend_logic_op")) {  
#ifdef WNT
          glBlendEquationOp = (PFNGLBLENDEQUATIONEXTPROC)wglGetProcAddress("glBlendEquationOp");
#else
          glBlendEquationOp = (PFNGLBLENDEQUATIONEXTPROC)glXGetProcAddress((GLubyte*)"glBlendEquationOp");
#endif  
        } else
          glBlendEquationOp = NULL;
      }

      if ( glBlendEquationOp )
      {
        blend_state = glIsEnabled(GL_BLEND);
        if (!blend_state) glEnable(GL_BLEND);
        glBlendEquationOp(GL_LOGIC_OP);
        glLogicOp(GL_XOR); 
      }
#else

      blend_state = glIsEnabled(GL_BLEND);
      if (!blend_state) glEnable(GL_BLEND);
      glEnable(GL_COLOR_LOGIC_OP);
      glLogicOp(GL_XOR); 

#endif
      break;
    case ASPECT_TODT_SUBTITLE:
      textRender->StringSize(d->sdata, &sWidth, &sAscent, &sDescent);
      objrefX = (float)d->attach_pt.xyz[0];   
      objrefY = (float)d->attach_pt.xyz[1];   
      objrefZ = (float)d->attach_pt.xyz[2];
      status = gluProject (objrefX, objrefY, objrefZ, modelMatrix, projMatrix, viewport,
        &winx1, &winy1, &winz1);

      winx = winx1;
      winy = winy1-sDescent;
      winz = winz1+0.00001;     
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &objX, &objY, &objZ);

      winx = winx1 + sWidth;
      winy = winy1-sDescent;
      winz = winz1+0.00001; /* il vaut mieux F+B / 1000000 ? */     
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &obj1X, &obj1Y, &obj1Z);

      winx = winx1 + sWidth;
      winy = winy1 + sAscent;
      winz = winz1+0.00001;     
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &obj2X, &obj2Y, &obj2Z);

      winx = winx1;
      winy = winy1+ sAscent;
      winz = winz1+0.00001;   
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &obj3X, &obj3Y, &obj3Z);

      glColor3fv( colours.rgb );
      glBegin(GL_POLYGON);
      glVertex3d(objX, objY, objZ);
      glVertex3d(obj1X, obj1Y, obj1Z);
      glVertex3d(obj2X, obj2Y, obj2Z);
      glVertex3d(obj3X, obj3Y, obj3Z);
      glEnd();
      break;

    case ASPECT_TODT_DEKALE:
      objrefX = (float)d->attach_pt.xyz[0];   
      objrefY = (float)d->attach_pt.xyz[1];   
      objrefZ = (float)d->attach_pt.xyz[2];
      status = gluProject (objrefX, objrefY, objrefZ, modelMatrix, projMatrix, viewport,
        &winx1, &winy1, &winz1);

      winx = winx1+1;
      winy = winy1+1;
      winz = winz1+0.00001;     
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &objX, &objY, &objZ);

      glColor3fv( colours.rgb );
      textRender->RenderText( d->sdata, fontBase, 0, (float)objX, (float)objY,(float)objZ );
      winx = winx1-1;
      winy = winy1-1;
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &objX, &objY, &objZ);

      textRender->RenderText( d->sdata, fontBase, 0, (float)objX, (float)objY,(float)objZ );
      winx = winx1-1;
      winy = winy1+1;
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &objX, &objY, &objZ); 

      textRender->RenderText( d->sdata, fontBase, 0, (float)objX, (float)objY,(float)objZ );
      winx = winx1+1;
      winy = winy1-1;
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &objX, &objY, &objZ);
      textRender->RenderText( d->sdata, fontBase, 0, (float)objX, (float)objY,(float)objZ );
      break;
    }
  }

  glColor3fv( colour.rgb );
  textRender->RenderText( d->sdata, fontBase, 0, (float)d->attach_pt.xyz[0], (float)d->attach_pt.xyz[1],(float)d->attach_pt.xyz[2] );
  /* maj attributs */   
  if (flag_zbuffer) glEnable(GL_DEPTH_TEST); 
  if (display_type == ASPECT_TODT_BLEND) 
  {
#if defined(GL_EXT_blend_logic_op) 
    if ((!blend_state) && (QueryExtension("GL_EXT_blend_logic_op"))) glDisable(GL_BLEND);
#else
    if (!blend_state) glDisable(GL_BLEND);
    glDisable(GL_COLOR_LOGIC_OP);
#endif
  }

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
TextDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_text_data d = (tel_text_data)data.pdata;
  if (d)
  {
    delete[] d->sdata;
    delete d;
  }
  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
TextPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_text_data p;

  p = (tel_text_data)data.pdata;

  fprintf( stdout, "TelText.\n" );
  fprintf( stdout, "\t\tString : %S\n", p->sdata );
  fprintf( stdout, "\t\tAttach Point : %f %f %f\n", p->attach_pt.xyz[0],
    p->attach_pt.xyz[1],
    p->attach_pt.xyz[2] );
  fprintf( stdout, "\n" );

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static TStatus
TextInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint          i;
  tel_text_data d;
  Tint          size_reqd=0;
  TStatus       status = TSuccess;

  d = (tel_text_data)data.pdata;

  size_reqd = sizeof(Techar)*d->length;

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case INQ_GET_SIZE_ID:
      {
        k[i]->data.ldata = size_reqd;
        break;
      }

    case INQ_GET_CONTENT_ID:
      {
        TEL_INQ_CONTENT *c;
        Teldata         *w;

        c = (tel_inq_content)k[i]->data.pdata;
        c->act_size = size_reqd;
        w = c->data;

        if( c->size >= size_reqd )
        {
          w->atext3.string = (Techar*)c->buf;
          w->atext3.ref_pt = d->attach_pt;
          w->atext3.anno.xyz[0] = ( float )0.0;
          w->atext3.anno.xyz[1] = ( float )0.0;
          w->atext3.anno.xyz[2] = ( float )0.0;
          //szv: instead of strcpy
          Techar *ptr1 = w->atext3.string;
          const wchar_t *ptr2 = d->sdata;
          while (*ptr1++ = (Techar)(*ptr2++));
          status = TSuccess;
        }
        else
          status = TFailure;
        break;
      }
    }
  }
  return status;
}

/*----------------------------------------------------------------------*/
