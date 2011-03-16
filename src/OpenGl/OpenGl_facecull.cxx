
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  FaceCullingModeDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  FaceCullingModeAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  FaceCullingModePrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  FaceCullingModeInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  FaceCullingModeDisplay,             /* PickTraverse */
  FaceCullingModeDisplay,
  FaceCullingModeAdd,
  0,             /* Delete */
  FaceCullingModePrint,
  FaceCullingModeInquire
};


MtblPtr
TelFaceCullingModeInitClass( TelType *el )
{
  *el = TelFaceCullingMode;
  return MtdTbl;
}

static  TStatus
FaceCullingModeAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


static  TStatus
FaceCullingModeDisplay( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelFaceCullingMode;
  key.data.ldata = d.ldata;
  TsmSetAttri( 1, &key );

  return TSuccess;
}


static  TStatus
FaceCullingModePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  switch( data.ldata )
  {
  case TelCullNone:
    fprintf( stdout, "TelFaceCullingMode. Value = CULL_NONE\n" );
    break;

  case TelCullFront:
    fprintf( stdout, "TelFaceCullingMode. Value = CULL_FRONT\n" );
    break;

  case TelCullBack:
    fprintf( stdout, "TelFaceCullingMode. Value = CULL_BACK\n" );
    break;
  }
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
FaceCullingModeInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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
        /* w->face_processing_mode.cull = data.ldata; */
        w->idata = data.ldata;
        break;
      }
    }
  }

  return TSuccess;
}
