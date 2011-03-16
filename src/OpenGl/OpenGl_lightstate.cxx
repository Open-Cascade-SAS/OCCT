/***********************************************************************

FONCTION :
----------
File OpenGl_qstrip :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
30-06-97 : FMN ; Appel de la toolkit OpenGl_LightBox.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 


#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_LightBox.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  LightSrcStateDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  LightSrcStateAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  LightSrcStateDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  LightSrcStatePrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  LightSrcStateInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  LightSrcStateDisplay,
  LightSrcStateAdd,
  LightSrcStateDelete,
  LightSrcStatePrint,
  LightSrcStateInquire
};

#define NO_TRACE_LIGHT 

MtblPtr
TelLightSrcStateInitClass( TelType* el )
{
  *el = TelLightSrcState;
  return MtdTbl;
}

static  TStatus
LightSrcStateAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  Tint  i, num, *ptr;
  tel_tint_data  data;

  num = k[0]->id;
  data = new TEL_TINT_DATA();
  if( !data )
    return TFailure;
  data->data = new int[num];
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
LightSrcStateDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_tint_data d = (tel_tint_data)data.pdata;

#ifdef TRACE_LIGHT
  printf("OpenGl_lightstate::LightSrcStateDisplay:UpdateLight(%d)\n", TglActiveWs);
#endif
  UpdateLight( TglActiveWs);
  return  TSuccess;
}

static  TStatus
LightSrcStateDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  delete [] data.pdata;
  return TSuccess;
}




static  TStatus
LightSrcStatePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint           i;
  tel_tint_data  p;

  p = (tel_tint_data)data.pdata;

  fprintf( stdout, "TelLightSrcState. Number: %d\n", p->num );
  for( i = 0; i < p->num; i++ )
  {
    fprintf( stdout, "\n\t\t v[%d] = %d", i, p->data[i] );
  }
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
LightSrcStateInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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

        if( c->size >= size_reqd )
        {
          w->light_source_state.on.number = d->num;
          w->light_source_state.off.number = 0;
          w->light_source_state.on.integers = (Tint *)(c->buf);
          w->light_source_state.off.integers = 0;

          memcpy( w->light_source_state.on.integers, d->data, d->num*sizeof(Tint) );
        }

        break;
      }
    }
  }
  return status;
}
