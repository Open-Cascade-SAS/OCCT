                                             
/*OCC7456 abd 14.12.2004 Text alingnment attributes  */

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_tgl_all.hxx>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_Memory.hxx>

static  TStatus  TextAlignDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextAlignAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextAlignPrint( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  TextAlignDisplay,
  TextAlignAdd,
  0,             /* Delete */
  TextAlignPrint,
  0              /* Inquire */
};

MtblPtr
TelTextAlignInitClass( TelType *el )
{
  *el = TelTextAlign;
  return MtdTbl;
}

static  TStatus
TextAlignDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelTextAlign;
  key.data.pdata = data.pdata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}

static  TStatus
TextAlignAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  tel_align_data data = new TEL_ALIGN_DATA();
  if( !data )
    return TFailure;

  *data = *(tel_align_data)(k[0]->data.pdata);

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}


static  TStatus
TextAlignPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  fprintf( stdout, "TelTextHAlign. Value = %d\n", data.ldata);
  fprintf( stdout, "\n" );

  return TSuccess;
}
