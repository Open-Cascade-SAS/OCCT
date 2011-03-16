
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  BackSurfaceAreaPropertiesDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  BackSurfaceAreaPropertiesAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  BackSurfaceAreaPropertiesDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  BackSurfaceAreaPropertiesPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  BackSurfaceAreaPropertiesInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  BackSurfaceAreaPropertiesDisplay,
  BackSurfaceAreaPropertiesAdd,
  BackSurfaceAreaPropertiesDelete,
  BackSurfaceAreaPropertiesPrint,
  BackSurfaceAreaPropertiesInquire
};


MtblPtr
TelBackSurfaceAreaPropertiesInitClass( TelType *el )
{
  *el = TelBackSurfaceAreaProperties;
  return MtdTbl;
}

static  TStatus
BackSurfaceAreaPropertiesAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  tel_surf_prop data = new TEL_SURF_PROP();
  if( !data )
    return TFailure;

  *data = *(tel_surf_prop)(k[0]->data.pdata);

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}


static  TStatus
BackSurfaceAreaPropertiesDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY         key;

  key.id = TelBackSurfaceAreaProperties;
  key.data.pdata = data.pdata;
  TsmSetAttri( 1, &key );

  return TSuccess;
}


static  TStatus
BackSurfaceAreaPropertiesDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}




static  TStatus
BackSurfaceAreaPropertiesPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_surf_prop p = (tel_surf_prop)data.pdata;

  fprintf( stdout, "TelBackSurfaceAreaProperties\n" );
  fprintf( stdout, "\tAmbient Coeff: %f\n", p->amb );
  fprintf( stdout, "\tDiffuse Coeff: %f\n", p->diff );
  fprintf( stdout, "\tSpecular Coeff: %f\n", p->spec );
  fprintf( stdout, "\tTransparency Coeff: %f\n", p->trans );
  fprintf( stdout, "\tShininess: %f\n", p->shine );
  fprintf( stdout, "\tSpecular colour: R = %f, G = %f, B = %f\n",
    p->speccol.rgb[0], p->speccol.rgb[1], p->speccol.rgb[2] );
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
BackSurfaceAreaPropertiesInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint i;
  tel_surf_prop p = (tel_surf_prop)data.pdata;

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
        w->surf_prop = *p;
        break;
      }
    }
  }

  return TSuccess;
}
