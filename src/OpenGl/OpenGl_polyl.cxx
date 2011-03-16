/***********************************************************************

FONCTION :
----------
File OpenGl_polyl :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
21-10-96 : FMN ; Suppression LMC_COLOR fait dans OpenGl_execstruct.c

************************************************************************/

#define G003    /* EUG 07-10-99 Degeneration mode support
*/

#define IMP190602       /* GG Avoid memory leaks after creating a polyline
//                      with vertex colors primitive.
//                      Thanks to Ivan Fontaine (SAMTECH) for this improvment
*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_telem_highlight.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_Memory.hxx>

#ifdef G003
extern int g_nDegenerateModel;
#endif  /* G003 */

static  TStatus  PolylineDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolylineAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolylineDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolylinePrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolylineInquire( TSM_ELEM_DATA, Tint, cmn_key* );

/*static  GLboolean         lighting_mode;*/

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  PolylineDisplay,
  PolylineDisplay,
  PolylineAdd,
  PolylineDelete,
  PolylinePrint,
  PolylineInquire
};

struct TEL_LINE_DATA
{
  Tint       num_lines;       /* Number of polylines */
  Tint       num_vertices;    /* Number of vertices in vertices array */
  Tint       *bounds;         /* Array of number of points in each polyline */
  tel_colour vcolours;        /* vertex colour values for each vertex */
  tel_point  vertices;        /* vertices array of length num_vertices */
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TEL_LINE_DATA* tel_line_data;

MtblPtr
TelPolylineInitClass( TelType* el )
{
  *el = TelPolyline;
  return MtdTbl;
}

static  TStatus
PolylineAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )

{
  Tint          i, j;
  tel_line_data p;

  for( i = 0; i < n; i++ )
  {
    if( k[i]->id == NUM_LINES_ID )
      break;
  }

  if( i == n )
    return TFailure;

  if( !(k[i]->data.ldata) )
    return TFailure;

  for( j = 0; j < n; j++ )
  {
    if( k[j]->id == BOUNDS_DATA_ID )
      break;
  }

  if( j == n )
    return TFailure;

  p = new TEL_LINE_DATA();
  p->num_lines = k[i]->data.ldata;
  p->bounds = new Tint[p->num_lines];
  memcpy( p->bounds, k[j]->data.pdata, p->num_lines*sizeof(Tint) );

  p->num_vertices = 0;
  for( i = 0; i < p->num_lines; i++ )
  {
    p->num_vertices += p->bounds[i];
  }

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case VERTEX_COLOUR_VALS_ID:
      p->vcolours = new TEL_COLOUR[p->num_vertices];
      memcpy( p->vcolours, k[i]->data.pdata, p->num_vertices*sizeof(TEL_COLOUR) );
      break;

    case VERTICES_ID:
      p->vertices = new TEL_POINT[p->num_vertices];
      memcpy( p->vertices, k[i]->data.pdata, p->num_vertices*sizeof(TEL_POINT) );
      break;
    }
  }

  ((tsm_elem_data)(d.pdata))->pdata = p;

  return TSuccess;
}


static  TStatus
PolylineDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_line_data   d;
  Tint            i, j, a;
  CMN_KEY         key;
  TEL_COLOUR      colour;

  d = (tel_line_data)data.pdata;

  /* OCC11904 -- Temporarily disable environment mapping */
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);

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
    key.id = TelPolylineColour;
    key.data.pdata = &colour;
    TsmGetAttri( 1, &key );
  }

  if( !d->vcolours )
  {
    glColor3fv( colour.rgb );
    for( i = 0, j = 0, a = 0; i < d->num_lines; i++ )
    {
      a = j + d->bounds[i];
#ifdef G003
      glBegin ( g_nDegenerateModel != 3 ? GL_LINE_STRIP : GL_POINTS );
#else
      glBegin(GL_LINE_STRIP);
#endif  /* G003 */
      for( ; j < a; j++ )
        glVertex3fv( d->vertices[j].xyz );
      glEnd();
    }
  }
  else
  {
    for( i = 0, j = 0, a = 0; i < d->num_lines; i++ )
    {
      a = j + d->bounds[i];
#ifdef G003
      glBegin ( g_nDegenerateModel != 3 ? GL_LINE_STRIP : GL_POINTS );
#else
      glBegin(GL_LINE_STRIP);
#endif  /* G003 */
      for( ; j < a; j++ )
      {
        glColor3fv( d->vcolours[j].rgb );
        glVertex3fv( d->vertices[j].xyz );
      }
      glEnd();
    }

  }

  glPopAttrib();
  return TSuccess;
}


static  TStatus
PolylineDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_line_data p = (tel_line_data)data.pdata;
  if( p->bounds )
    delete[] p->bounds;
  if( p->vertices )
    delete[] p->vertices;
#ifdef IMP190602
  if( p->vcolours )
    delete[] p->vcolours;
#endif
  if (data.pdata)
    delete data.pdata;

  return TSuccess;
}




static  TStatus
PolylinePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint           i;
  tel_line_data  p = (tel_line_data)data.pdata;

  fprintf( stdout, "TelPolyline. Number of Lines: %d\n", p->num_lines );

  if( p->vcolours )
    fprintf( stdout, "\t\tVertex Flag : COLOUR\n" );
  else
    fprintf( stdout, "\t\tVertex Flag : NONE\n" );

  if( p->bounds )
  {
    fprintf( stdout, "\t\tLengths array :\n" );
    for( i = 0; i < p->num_lines; i++ )
      fprintf( stdout, "\t\tL[%d] %d \n", i, p->bounds[i] );
  }

  if( p->vertices )
  {
    fprintf( stdout, "\n\t\tVertices : " );
    for( i = 0; i < p->num_vertices; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->vertices[i].xyz[0],
      p->vertices[i].xyz[1],
      p->vertices[i].xyz[2] );    }

  fprintf( stdout, "\n" );

  if( p->vcolours )
  {
    fprintf( stdout, "\n\t\tVertex Colours : " );
    for( i = 0; i < p->num_vertices; i++ )
      fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i,
      p->vcolours[i].rgb[0],
      p->vcolours[i].rgb[1],
      p->vcolours[i].rgb[2] );    }
  else
    fprintf( stdout, "\n\t\tVertex Colours not specified\n" );

  fprintf( stdout, "\n" );

  return TSuccess;
}

static TStatus
PolylineInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint          i;
  tel_line_data d;
  Tint          size_reqd=0;
  TStatus       status = TSuccess;

  d = (tel_line_data)data.pdata;

  size_reqd  = d->num_lines * sizeof( Tint );
  size_reqd += ( d->num_vertices * sizeof( TEL_POINT ) );
  if( d->vcolours )
  {
    size_reqd += ( d->num_vertices * sizeof( TEL_COLOUR ) );
  }

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
        w->lineset3data.num_bounds = d->num_lines;

        if( d->vcolours )
          w->lineset3data.vrtflag = TEL_VTFLAG_COLOUR;
        else
          w->lineset3data.vrtflag = TEL_VTFLAG_NONE;

        if( c->size >= size_reqd )
        {
          w->lineset3data.bounds = (Tint *)(c->buf);
          memcpy( w->lineset3data.bounds, d->bounds, d->num_lines*sizeof(Tint) );

          w->lineset3data.points =
            (tel_point)(c->buf + d->num_lines * sizeof( Tint ) );

          memcpy( w->lineset3data.points, d->vertices, d->num_vertices*sizeof(TEL_POINT) );
          if( d->vcolours )
          {
            w->lineset3data.colours = (tel_colour)( c->buf +
              d->num_lines * sizeof( Tint ) +
              d->num_vertices * sizeof( TEL_POINT ));
            memcpy( w->lineset3data.colours, d->vcolours, d->num_vertices*sizeof(TEL_COLOUR) );
          }
          else
            w->lineset3data.colours = 0;

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
