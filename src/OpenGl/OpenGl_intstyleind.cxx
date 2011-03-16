
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

static  TStatus  InteriorStyleIndexDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  InteriorStyleIndexAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  InteriorStyleIndexPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  InteriorStyleIndexInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  InteriorStyleIndexDisplay,
  InteriorStyleIndexAdd,
  0,             /* Delete */
  InteriorStyleIndexPrint,
  InteriorStyleIndexInquire
};


MtblPtr
TelInteriorStyleIndexInitClass( TelType *el )
{
  *el = TelInteriorStyleIndex;
  return MtdTbl;
}

static  TStatus
InteriorStyleIndexAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


static  TStatus
InteriorStyleIndexDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY     key;

  key.id = TelInteriorStyleIndex;
  key.data.ldata = data.ldata;
  TsmSetAttri( 1, &key );

  return TSuccess;
}


static  TStatus
InteriorStyleIndexPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  switch( data.ldata )
  {
  case TEL_HS_SOLID:
    fprintf( stdout, "TelInteriorStyleIndex. Value = SOLID\n" );
    break;

  case TEL_HS_CROSS:
    fprintf( stdout, "TelInteriorStyleIndex. Value = CROSS\n" );
    break;

  case TEL_HS_CROSS_SPARSE:
    fprintf( stdout, "TelInteriorStyleIndex. Value = CROSS_SPARSE\n");
    break;

  case TEL_HS_GRID:
    fprintf( stdout, "TelInteriorStyleIndex. Value = GRID\n" );
    break;

  case TEL_HS_GRID_SPARSE:
    fprintf( stdout, "TelInteriorStyleIndex. Value = GRID_SPARSE\n" );
    break;

  case TEL_HS_DIAG_45:
    fprintf( stdout, "TelInteriorStyleIndex. Value = DIAG_45\n" );
    break;

  case TEL_HS_DIAG_135:
    fprintf( stdout, "TelInteriorStyleIndex. Value = DIAG_135\n" );
    break;

  case TEL_HS_HORIZONTAL:
    fprintf( stdout, "TelInteriorStyleIndex. Value = HORIZONTAL\n" );
    break;

  case TEL_HS_VERTICAL:
    fprintf( stdout, "TelInteriorStyleIndex. Value = VERTICAL\n" );
    break;

  case TEL_HS_DIAG_45_SPARSE:
    fprintf(stdout,"TelInteriorStyleIndex. Value = DIAG_45_SPARSE\n");
    break;

  case TEL_HS_DIAG_135_SPARSE:
    fprintf( stdout, "TelInteriorStyleIndex. Value = DIAG_135_SPARSE\n" );
    break;

  case TEL_HS_HORIZONTAL_SPARSE:
    fprintf( stdout,"TelInteriorStyleIndex. Value = HORIZONTAL_SPARSE\n");
    break;

  case TEL_HS_VERTICAL_SPARSE:
    fprintf( stdout, "TelInteriorStyleIndex. Value = VERTICAL_SPARSE\n" );
    break;

  }
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
InteriorStyleIndexInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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
