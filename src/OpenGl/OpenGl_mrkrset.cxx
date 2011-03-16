/***********************************************************************

FONCTION :
----------
File OpenGl_mrkrset :


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
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_highlight.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  MarkerSetDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  MarkerSetAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  MarkerSetDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  MarkerSetPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  MarkerSetInquire( TSM_ELEM_DATA, Tint, cmn_key* );


extern GLuint GetListIndex(int);

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  MarkerSetDisplay,             /* PickTraverse */
  MarkerSetDisplay,
  MarkerSetAdd,
  MarkerSetDelete,
  MarkerSetPrint,
  MarkerSetInquire
};


MtblPtr
TelMarkerSetInitClass( TelType* el )
{
  *el = TelMarkerSet;
  return MtdTbl;
}

extern GLuint markerBase;
static  TStatus
MarkerSetAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  Tint  i, num;
  tel_point_data  data;
  tel_point       ptr;

  num = k[0]->id;
  data = new TEL_POINT_DATA();
  if( !data )
    return TFailure;

  data->data = new TEL_POINT[num];
  if( !data->data )
    return TFailure;

  for( i=0, ptr=(tel_point)(k[0]->data.pdata); i<num; i++ )
  {
    data->data[i] = ptr[i];
  }
  data->num = num;

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}


static  TStatus
MarkerSetDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_point_data  d;
  tel_point       ptr;
  Tint            i;
  CMN_KEY         key1, key2, key3;
  TEL_COLOUR      colour;
  Tchar           *str;

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
  d = (tel_point_data)data.pdata;
  ptr = d->data;

  if( key2.data.ldata == TEL_PM_DOT )
  {
    glPointSize( key3.data.fdata );
    glBegin( GL_POINTS );
    for( i=0; i<d->num; i++, ptr++ )
      glVertex3f( ptr->xyz[0],  ptr->xyz[1],  ptr->xyz[2] );
    glEnd();
  }
  else
  {
    GLint mode;
    glGetIntegerv( GL_RENDER_MODE, &mode );

    if( key2.data.ldata == TEL_PM_USERDEFINED )
    {       
      for( i=0; i<d->num; i++, ptr++ )
      {
        glRasterPos3f( ptr->xyz[0],  ptr->xyz[1],  ptr->xyz[2] );
        glCallList( GetListIndex( (int)key3.data.fdata ) );
        if( mode==GL_FEEDBACK )
        {
          glBegin( GL_POINTS );
          glVertex3f( ptr->xyz[0], ptr->xyz[1], ptr->xyz[2] );
          glEnd();
          /* it is necessary to indicate end of marker information */
        }
      }
    }
    else
    {
      str = TelGetStringForPM( key2.data.ldata, key3.data.fdata );
      glListBase(markerBase);
      for( i=0; i<d->num; i++, ptr++ )
      {
        glRasterPos3f( ptr->xyz[0],  ptr->xyz[1],  ptr->xyz[2] );
        glCallLists(strlen( (char*)str ), GL_UNSIGNED_BYTE, (GLubyte *) str );

        if( mode==GL_FEEDBACK )
        {
          glBegin( GL_POINTS );
          glVertex3f( ptr->xyz[0], ptr->xyz[1], ptr->xyz[2] );
          glEnd();
        }
      }
    }
  }

  return TSuccess;
}


static  TStatus
MarkerSetDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}




static  TStatus
MarkerSetPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint           i;
  tel_point_data p;

  p = (tel_point_data)data.pdata;

  fprintf( stdout, "TelMarkerSet. Number of points: %d\n", p->num );
  for( i = 0; i < p->num; i++ )
  {
    fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i, p->data[i].xyz[0],
      p->data[i].xyz[1], p->data[i].xyz[2] );
  }
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
MarkerSetInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint           i, j;
  tel_point_data d;
  Tint           size_reqd=0;
  TStatus        status = TSuccess;

  d = (tel_point_data)data.pdata;

  size_reqd  = d->num * sizeof( TEL_POINT );

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

          for( j = 0; j < d->num; j++ )
            w->pts3[j] = d->data[j];

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
