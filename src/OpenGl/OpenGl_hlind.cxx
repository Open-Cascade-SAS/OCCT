
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  HighlightIndexDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  HighlightIndexAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  HighlightIndexPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  HighlightIndexInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  HighlightIndexDisplay,
  HighlightIndexAdd,
  0,             /* Delete */
  HighlightIndexPrint,
  HighlightIndexInquire
};


MtblPtr
TelHighlightIndexInitClass( TelType *el )
{
  *el = TelHighlightIndex;
  return MtdTbl;
}

static  TStatus
HighlightIndexAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


static  TStatus
HighlightIndexDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY   kk;

  kk.id = TelHighlightIndex;
  kk.data.ldata = data.ldata;
  TsmSetAttri( 1, &kk );
  return  TSuccess;
}


static  TStatus
HighlightIndexPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  fprintf( stdout, "TelHighlightIndex. Value = %d\n", data.ldata);
  fprintf( stdout, "\n" );

  return TSuccess;
}

static TStatus
HighlightIndexInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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

        c =(tel_inq_content)k[i]->data.pdata;
        w = c->data;
        c->act_size = 0;
        w->idata = data.ldata;
        break;
      }
    }
  }

  return TSuccess;
}
