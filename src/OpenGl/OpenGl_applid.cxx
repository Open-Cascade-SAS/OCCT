
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  ApplicationDataAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ApplicationDataPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ApplicationDataInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
    0,             /* PickTraverse */
    0,             /* Display */
    ApplicationDataAdd,
    0,             /* Delete */
    ApplicationDataPrint,
    ApplicationDataInquire
};


MtblPtr
TelApplicationDataInitClass( TelType *el )
{
  *el = TelApplicationData;
  return MtdTbl;
}

static  TStatus
ApplicationDataAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->pdata = k[0]->data.pdata;

  return TSuccess;
}


static  TStatus
ApplicationDataPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  fprintf( stdout, "TelApplicationData. Value = %lx\n",(long ) data.pdata);
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
ApplicationDataInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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
        w->data = (Tchar*)data.pdata;
        break;
      }
    }
  }

  return TSuccess;
}
