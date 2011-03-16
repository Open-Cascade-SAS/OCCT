
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  DepthCueIndexAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  DepthCueIndexDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  DepthCueIndexPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  DepthCueIndexInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  DepthCueIndexDisplay,             /* PickTraverse */
    DepthCueIndexDisplay,
    DepthCueIndexAdd,
    0,             /* Delete */
    DepthCueIndexPrint,
    DepthCueIndexInquire
};


MtblPtr
TelDepthCueIndexInitClass( TelType *el )
{
  *el = TelDepthCueIndex;
  return MtdTbl;
}

static  TStatus
DepthCueIndexAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


static  TStatus
DepthCueIndexDisplay( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelDepthCueIndex;
  key.data.ldata = d.ldata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}


static  TStatus
DepthCueIndexPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  fprintf( stdout, "TelDepthCueIndex. Value = %d\n", data.ldata);
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
DepthCueIndexInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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
