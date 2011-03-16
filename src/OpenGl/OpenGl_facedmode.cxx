
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  FaceDistinguishingModeAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  FaceDistinguishingModeDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  FaceDistinguishingModePrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  FaceDistinguishingModeInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  FaceDistinguishingModeDisplay,
  FaceDistinguishingModeAdd,
  0,             /* Delete */
  FaceDistinguishingModePrint,
  FaceDistinguishingModeInquire,
};


MtblPtr
TelFaceDistinguishingModeInitClass( TelType *el )
{
  *el = TelFaceDistinguishingMode;
  return MtdTbl;
}

static  TStatus
FaceDistinguishingModeAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


static  TStatus
FaceDistinguishingModeDisplay( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelFaceDistinguishingMode;
  key.data.ldata = d.ldata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}


static  TStatus
FaceDistinguishingModePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  data.ldata == TOn ?
    fprintf( stdout, "TelFaceDistinguishingMode. Value = ON\n" ) :
  fprintf( stdout, "TelFaceDistinguishingMode. Value = OFF\n" );
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
FaceDistinguishingModeInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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
        /* w->face_processing_mode.distinguish = data.ldata; */
        w->idata = data.ldata;
        break;
      }
    }
  }

  return TSuccess;
}
