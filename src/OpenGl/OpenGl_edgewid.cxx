
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  EdgeWidthDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  EdgeWidthAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  EdgeWidthDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  EdgeWidthPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  EdgeWidthInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  EdgeWidthDisplay,
  EdgeWidthAdd,
  EdgeWidthDelete,
  EdgeWidthPrint,
  EdgeWidthInquire
};


MtblPtr
TelEdgeWidthInitClass( TelType *el )
{
  *el = TelEdgeWidth;
  return MtdTbl;
}

static  TStatus
EdgeWidthAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  Tfloat *data = new Tfloat();

  //cmn_memreserve( data, 1, 0 );
  if( !data )
    return TFailure;

  *data = k[0]->data.fdata;

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}


static  TStatus
EdgeWidthDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY     key;

  key.id = TelEdgeWidth;
  key.data.fdata = *(Tfloat*)(data.pdata);
  TsmSetAttri( 1, &key );

  return TSuccess;
}


static  TStatus
EdgeWidthDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  //cmn_freemem( data.pdata );
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}


static  TStatus
EdgeWidthPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  fprintf( stdout, "TelEdgeWidth. Value = %g\n", *(Tfloat *)(data.pdata));
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
EdgeWidthInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint i;
  Tfloat *f = (Tfloat *)data.pdata;

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
        w->fdata = *f;
        break;
      }
    }
  }

  return TSuccess;
}
