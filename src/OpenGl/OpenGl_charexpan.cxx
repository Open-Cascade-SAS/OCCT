
#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

static  TStatus  CharacterExpansionFactorDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CharacterExpansionFactorAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CharacterExpansionFactorDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CharacterExpansionFactorPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  CharacterExpansionFactorInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  CharacterExpansionFactorDisplay,             /* PickTraverse */
  CharacterExpansionFactorDisplay,
  CharacterExpansionFactorAdd,
  CharacterExpansionFactorDelete,
  CharacterExpansionFactorPrint,
  CharacterExpansionFactorInquire
};


MtblPtr
TelCharacterExpansionFactorInitClass( TelType *el )
{
  *el = TelCharacterExpansionFactor;
  return MtdTbl;
}

static  TStatus
CharacterExpansionFactorAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  Tfloat *data = new Tfloat();
  if( !data )
    return TFailure;

  *data = k[0]->data.fdata;

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}


static  TStatus
CharacterExpansionFactorDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY     key;

  key.id = TelCharacterExpansionFactor;
  key.data.fdata = *(Tfloat*)(data.pdata);
  TsmSetAttri( 1, &key );

  return TSuccess;
}


static  TStatus
CharacterExpansionFactorDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}


static  TStatus
CharacterExpansionFactorPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  fprintf( stdout, "TelCharacterExpansionFactor. Value = %g\n", *(Tfloat *)(data.pdata));
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
CharacterExpansionFactorInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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
