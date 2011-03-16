
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  LocalTran3Display( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  LocalTran3Add( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  LocalTran3Delete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  LocalTran3Print( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  LocalTran3Inquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  LocalTran3Display,             /* PickTraverse */
  LocalTran3Display,
  LocalTran3Add,
  LocalTran3Delete,
  LocalTran3Print,
  LocalTran3Inquire
};


MtblPtr
TelLocalTran3InitClass( TelType *el )
{
  *el = TelLocalTran3;
  return MtdTbl;
}

static  TStatus
LocalTran3Add( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  tel_matrix3_data data = new TEL_MATRIX3_DATA();

  //cmn_memreserve( data, 1, 0 );

  if( !data )
    return TFailure;

  data->mode = (TComposeType)(k[0]->id);
  matcpy( data->mat, k[0]->data.pdata );

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}


static  TStatus
LocalTran3Display( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_matrix3_data d;
  CMN_KEY          key;

  d = (tel_matrix3_data)data.pdata;

  key.id = TelLocalTran3;
  key.data.pdata = d;
  TsmSetAttri( 1, &key );

  return TSuccess;
}


static  TStatus
LocalTran3Delete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  //cmn_freemem( data.pdata );
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}




static  TStatus
LocalTran3Print( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_matrix3_data  d;
  Tint i;

  d = (tel_matrix3_data)data.pdata;

  fprintf( stdout, "TelLocalTran3. \n" );
  for( i=0; i<4; i++ )
    fprintf( stdout, "\t\t%12.7f  %12.7f  %12.7f  %12.7f\n", d->mat[i][0],
    d->mat[i][1], d->mat[i][2], d->mat[i][3] );
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
LocalTran3Inquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint              i;
  tel_matrix3_data d;

  d = (tel_matrix3_data)data.pdata;

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case INQ_GET_SIZE_ID:
      {
        k[i]->data.ldata = 0;
        break;
      }

    case INQ_GET_CONTENT_ID:
      {
        TEL_INQ_CONTENT *c;
        Teldata         *w;

        c = (tel_inq_content)k[i]->data.pdata;
        c->act_size = 0;
        w = c->data;

        w->local_xform_3.compose_type = d->mode;
        //cmn_memcpy( w->local_xform_3.mat3, d->mat, sizeof( Tmatrix3), 1 );
        memcpy( w->local_xform_3.mat3, d->mat, sizeof( Tmatrix3) );
      }
    }
  }
  return TSuccess;
}
