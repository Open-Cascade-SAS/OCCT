
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  CharacterSpacingDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CharacterSpacingAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CharacterSpacingDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CharacterSpacingPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CharacterSpacingInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  CharacterSpacingDisplay,             /* PickTraverse */
  CharacterSpacingDisplay,
  CharacterSpacingAdd,
  CharacterSpacingDelete,
  CharacterSpacingPrint,
  CharacterSpacingInquire
};


MtblPtr
TelCharacterSpacingInitClass( TelType *el )
{
  *el = TelCharacterSpacing;
  return MtdTbl;
}

static  TStatus
CharacterSpacingAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  Tfloat *data = new Tfloat();
  if( !data )
    return TFailure;

  *data = k[0]->data.fdata;

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}


static  TStatus
CharacterSpacingDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY     key;

  key.id = TelCharacterSpacing;
  key.data.fdata = *(Tfloat*)(data.pdata);
  TsmSetAttri( 1, &key );

  return TSuccess;
}


static  TStatus
CharacterSpacingDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}


static  TStatus
CharacterSpacingPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  fprintf( stdout, "TelCharacterSpacing. Value = %g\n", *(Tfloat *)(data.pdata));
  fprintf( stdout, "\n" );
  return TSuccess;
}


static TStatus
CharacterSpacingInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint    i;
  Tfloat *f = (Tfloat*)data.pdata;

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
