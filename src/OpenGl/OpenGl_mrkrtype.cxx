
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  PolymarkerTypeDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolymarkerTypeAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolymarkerTypePrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolymarkerTypeInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  PolymarkerTypeDisplay,             /* PickTraverse */
  PolymarkerTypeDisplay,
  PolymarkerTypeAdd,
  0,             /* Delete */
  PolymarkerTypePrint,
  PolymarkerTypeInquire
};


MtblPtr
TelPolymarkerTypeInitClass( TelType *el )
{
  *el = TelPolymarkerType;
  return MtdTbl;
}

static  TStatus
PolymarkerTypeAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


static  TStatus
PolymarkerTypeDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY     key;

  key.id = TelPolymarkerType;
  key.data.ldata = data.ldata;
  TsmSetAttri( 1, &key );

  return TSuccess;
}


static  TStatus
PolymarkerTypePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  switch( data.ldata )
  {
  case TEL_PM_PLUS:
    fprintf( stdout, "TelPolymarkerType. Value = PLUS\n" );
    break;

  case TEL_PM_STAR:
    fprintf( stdout, "TelPolymarkerType. Value = STAR\n" );
    break;

  case TEL_PM_CROSS:
    fprintf( stdout, "TelPolymarkerType. Value = CROSS\n" );
    break;

  case TEL_PM_CIRC:
    fprintf( stdout, "TelPolymarkerType. Value = CIRC\n" );
    break;

  case TEL_PM_DOT:
    fprintf( stdout, "TelPolymarkerType. Value = DOT\n" );
    break;

  case TEL_PM_USERDEFINED:
    fprintf( stdout, "TelPolymarkerType. Value = USERDEFINED\n" );
    break;

  default:
    fprintf( stdout, "TelPolymarkerType. Value = %d (UNKNOWN)",
      data.ldata );
  }
  fprintf( stdout, "\n" );

  return TSuccess;
}

static TStatus
PolymarkerTypeInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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
