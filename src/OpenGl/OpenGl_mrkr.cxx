/***********************************************************************

FONCTION :
----------
File OpenGl_mrkr :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT
08-07-96 : BGN ; (PRO4768) suppression du cas particulier TEL_PM_DOT.
21-10-96 : FMN ; Suppression LMC_COLOR fait dans OpenGl_execstruct.c
19-10-96 : CAL ; Restauration du cas particulier TEL_PM_DOT mais
avec la correction sur la mise a jour du pointSize.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 


#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_telem_highlight.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

extern GLuint GetListIndex(int);


static  TStatus  MarkerDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  MarkerAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  MarkerDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  MarkerPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  MarkerInquire( TSM_ELEM_DATA, Tint, cmn_key* );

/*static  GLboolean         lighting_mode;*/

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  MarkerDisplay,             /* PickTraverse */
    MarkerDisplay,
    MarkerAdd,
    MarkerDelete,
    MarkerPrint,
    MarkerInquire
};


MtblPtr
TelMarkerInitClass( TelType *el )
{
  *el = TelMarker;
  return MtdTbl;
}

static  TStatus
MarkerAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  tel_point data = new TEL_POINT;

  if( !data )
    return TFailure;

  *data = *(tel_point)(k[0]->data.pdata);

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}

extern GLuint markerBase;
static  TStatus
MarkerDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_point  p;
  CMN_KEY    key1, key2, key3;
  TEL_COLOUR colour;
  Tchar      *str;

  key1.id         = TelPolymarkerColour;
  key1.data.pdata = &colour;

  key2.id = TelPolymarkerType;
  key3.id = TelPolymarkerSize;

  TsmGetAttri( 3, &key1, &key2, &key3 );

  if( k[0]->id == TOn )
  {                          /* Use highlight colours */
    TEL_HIGHLIGHT  hrep;

    key1.id = TelHighlightIndex;
    TsmGetAttri( 1, &key1 );
    if( TelGetHighlightRep( TglActiveWs, key1.data.ldata, &hrep )
      == TSuccess )
      colour = hrep.col;
    else
    {
      TelGetHighlightRep( TglActiveWs, 0, &hrep );
      colour = hrep.col;
    }
  }

  glColor3fv( colour.rgb );
  p = (tel_point)data.pdata;
  if( key2.data.ldata == TEL_PM_DOT )
  {
    glPointSize( key3.data.fdata );
    glBegin( GL_POINTS );
    glVertex3fv( p->xyz );
    glEnd();
  }
  else
  {
    GLint mode;
    glGetIntegerv( GL_RENDER_MODE, &mode );
    if( key2.data.ldata == TEL_PM_USERDEFINED )
    {       
      glRasterPos3f( (GLfloat)(p->xyz[0]),  
        (GLfloat)(p->xyz[1]),
        (GLfloat)(p->xyz[2]) );
      glCallList( GetListIndex( (int)key3.data.fdata ) );       
    }
    else
    {
      str = TelGetStringForPM( key2.data.ldata, key3.data.fdata );
      glRasterPos3f( (GLfloat)(p->xyz[0]),  (GLfloat)(p->xyz[1]),
        (GLfloat)(p->xyz[2]) );
      glListBase(markerBase);
      glCallLists(strlen( (char*)str ), GL_UNSIGNED_BYTE, (GLubyte *) str );
    }

    if( mode==GL_FEEDBACK )
    {
      glBegin( GL_POINTS );
      glVertex3f( p->xyz[0], p->xyz[1], p->xyz[2] );
      glEnd();
      /* it is necessary to indicate end of marker information*/
    }
  }

  return TSuccess;
}


static  TStatus
MarkerDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}




static  TStatus
MarkerPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_point p;

  p = (tel_point)data.pdata;

  fprintf( stdout, "TelMarker. Value = %g, %g, %g\n",
    p->xyz[0], p->xyz[1], p->xyz[2] );
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
MarkerInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint          i;
  tel_point     d;
  Tint          size_reqd=0;
  TStatus       status = TSuccess;

  d = (tel_point)data.pdata;

  size_reqd = sizeof( TEL_POINT );

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
          w->pts3 = (tel_point)(c->buf);
          *(w->pts3) = *d;
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
