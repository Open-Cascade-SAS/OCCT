
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_telem_highlight.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  CurveDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CurveAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CurveDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CurvePrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CurveInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  CurveDisplay,
  CurveDisplay,
  CurveAdd,
  CurveDelete,
  CurvePrint,
  CurveInquire
};

struct TEL_CURVE_DATA
{
  TelCurveType type;          /* curve type : Bezier, Cardinal, BSpline */
  Tint         num_points;
  TEL_POINT    vertices[4];   /* vertices array */
  IMPLEMENT_MEMORY_OPERATORS
};

typedef TEL_CURVE_DATA* tel_curve_data;

MtblPtr
TelCurveInitClass( TelType* el )
{
  *el = TelCurve;
  return MtdTbl;
}

static  TStatus
CurveAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
/* accepts the keys CURVE_TYPE_ID,
CURVE_VERTICES_ID */
{
  Tint i, j;
  tel_curve_data p;

  /* Recherche du type de la courbe. Presence obligatoire. */
  for( i = 0; i < n; i++ )
  {
    if( k[i]->id == CURVE_TYPE_ID )
      break;
  }

  /* Type de la courbe non fourni. */
  if( i == n )
    return TFailure;

  /* Recherche du nombre de points par courbe. Presence obligatoire. */
  for( j = 0; j < n; j++ )
  {
    if( k[j]->id == CURVE_NUM_POINTS_ID )
      break;
  }

  /* Nombre de points par courbe non fourni. */
  if( j == n )
    return TFailure;

  /* Nombre de points par courbe ridicule */
  if( k[j]->data.ldata < 2 )
    return TFailure;

  //cmn_memreserve( p, 1, 1 );
  p = new TEL_CURVE_DATA();
  p->type = (TelCurveType)k[i]->data.ldata;
  p->num_points = (TelCurveType)k[j]->data.ldata;

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case CURVE_VERTICES_ID:
      //cmn_memcpy<TEL_POINT>(p->vertices, k[i]->data.pdata, 4);
      memcpy(p->vertices, k[i]->data.pdata, 4*sizeof(TEL_POINT));
      break;
    }
  }

  ((tsm_elem_data)(d.pdata))->pdata = p;

  return TSuccess;
}


static  TStatus
CurveDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_curve_data d;
  CMN_KEY        key;
  TEL_COLOUR     colour;
  int            i, j;
  GLfloat          reseau[4][3];

  d = (tel_curve_data)data.pdata;

  /* Mode highlight en cours */
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

  /* Recopie des points du reseau : Pb prototypage en c ansi */
  for( i = 0; i < 4; i++ )
    for( j = 0; i < 3; i++ )
      reseau[i][j] = d->vertices[i].xyz[j];

  /* Determination du type de courbe */
  switch( d->type )
  {
  case TelBezierCurve:
    /* OGLXXX curvebasis: see man pages
    glMap1();glMap2();glMapGrid();
    */
    break;
  case TelCardinalCurve:
    /* OGLXXX curvebasis: see man pages
    glMap1();glMap2();glMapGrid();
    */
    break;
  case TelBSplineCurve:
    /* OGLXXX curvebasis: see man pages
    glMap1();glMap2();glMapGrid();
    */
    break;
  default:
    printf( "\n\tCurveDisplay : INVALID KEY" );
    break;
  }

  /* Determination du type de courbe */
  /* OGLXXX curveprecision:see man pages
  glMap1();glMap2();glMapGrid();
  */

  /* Couleur */
  glColor3fv( colour.rgb );
  /* OGLXXX replace u with domain coordinate
  glEvalCoord1f( u );
  */

  return TSuccess;
}


static  TStatus
CurveDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_curve_data p = (tel_curve_data)data.pdata;
  //cmn_freemem( p );
  delete p;

  return TSuccess;
}




static  TStatus
CurvePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint           i;
  tel_curve_data  p = (tel_curve_data)data.pdata;

  fprintf( stdout, "TelCurve. \n" );

  fprintf( stdout, "\n\t\tVertices : " );
  for( i = 0; i < 4; i++ )
    fprintf( stdout, "\n\t\t v[%d] = %g %g %g", i, p->vertices[i].xyz[0],
    p->vertices[i].xyz[1],
    p->vertices[i].xyz[2] );

  fprintf( stdout, "\n" );

  return TSuccess;
}

static TStatus
CurveInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint           i;
  tel_curve_data d;
  Tint           size_reqd=0;
  TStatus        status = TSuccess;

  d = (tel_curve_data)data.pdata;

  size_reqd  = sizeof( TEL_CURVE_DATA );

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
          w->pts3 = (tel_point) c->buf;
          //cmn_memcpy<TEL_POINT>( w->pts3, d->vertices, 4 );
          memcpy( w->pts3, d->vertices, 4*sizeof(TEL_POINT) );
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
