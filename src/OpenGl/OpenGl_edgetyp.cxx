
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  EdgeTypeDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  EdgeTypeAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  EdgeTypePrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  EdgeTypeInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  EdgeTypeDisplay,
  EdgeTypeAdd,
  0,             /* Delete */
  EdgeTypePrint,
  EdgeTypeInquire
};


MtblPtr
TelEdgeTypeInitClass( TelType *el )
{
  *el = TelEdgeType;
  return MtdTbl;
}

static  TStatus
EdgeTypeAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


static  TStatus
EdgeTypeDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  /* Yet to be coded */
  return TSuccess;
}


static  TStatus
EdgeTypePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  switch( data.ldata )
  {
  case TEL_LS_SOLID:
    fprintf( stdout, "TelEdgeType. Value = SOLID\n" );
    break;

  case TEL_LS_DOT:
    fprintf( stdout, "TelEdgeType. Value = DOT\n" );
    break;

  case TEL_LS_DASH_DOT:
    fprintf( stdout, "TelEdgeType. Value = DASH_DOT\n" );
    break;

  case TEL_LS_DASH:
    fprintf( stdout, "TelEdgeType. Value = DASH\n" );
    break;

  case TEL_LS_DOUBLE_DOT_DASH:
    fprintf( stdout, "TelEdgeType. Value = DOUBLE_DOT_DASH\n" );
    break;
  }
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
EdgeTypeInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint i;

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case INQ_GET_SIZE_ID:
      {
        k[i]->data.ldata = sizeof( Tint );
        break;
      }
    case INQ_GET_CONTENT_ID:
      {
        TEL_INQ_CONTENT *c;
        Teldata         *w;

        c = (tel_inq_content)k[i]->data.pdata;
        w = c->data;
        c->act_size = 0;
        w->idata = data.ldata;
        break;
      }
    }
  }

  return TSuccess;
}
