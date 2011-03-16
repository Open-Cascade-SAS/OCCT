
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_telem_filters.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_Memory.hxx>

static  TStatus  RemoveNamesetDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  RemoveNamesetAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  RemoveNamesetDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  RemoveNamesetPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  RemoveNamesetInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  RemoveNamesetDisplay,             /* PickTraverse */
  RemoveNamesetDisplay,
  RemoveNamesetAdd,
  RemoveNamesetDelete,
  RemoveNamesetPrint,
  RemoveNamesetInquire
};


MtblPtr
TelRemoveNamesetInitClass( TelType* el )
{
  *el = TelRemoveNameset;
  return MtdTbl;
}

static  TStatus
RemoveNamesetAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  Tint  i, num, *ptr;
  tel_tint_data  data;

  num = k[0]->id;
  data = new TEL_TINT_DATA();
  if( !data )
    return TFailure;
  data->data = new Tint[num];
  if( !data->data )
    return TFailure;

  for( i=0, ptr=(Tint*)(k[0]->data.pdata); i<num; i++ )
  {
    data->data[i] = ptr[i];
  }
  data->num = num;

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}


static  TStatus
RemoveNamesetDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_tint_data d = (tel_tint_data)data.pdata;
  TglNamesetRemove( d->num, d->data );

  return  TSuccess;
}

static  TStatus
RemoveNamesetDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}




static  TStatus
RemoveNamesetPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint           i;
  tel_tint_data  p;

  p = (tel_tint_data)data.pdata;

  fprintf( stdout, "TelRemoveNameset. Number: %d\n", p->num );
  for( i = 0; i < p->num; i++ )
  {
    fprintf( stdout, "\n\t\t v[%d] = %d", i, p->data[i] );
  }
  fprintf( stdout, "\n" );

  return TSuccess;
}

static TStatus
RemoveNamesetInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint          i;
  tel_tint_data d;
  Tint          size_reqd=0;
  TStatus       status = TSuccess;

  d = (tel_tint_data)data.pdata;

  size_reqd = d->num * sizeof( Tint );

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

        w->name_set.number = d->num;
        if( c->size >= size_reqd )
        {
          w->name_set.integers = (Tint *)(c->buf);
          memcpy(w->name_set.integers, d->data, d->num*sizeof(Tint));
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
