/***********************************************************************

FONCTION :
----------
File OpenGl_textfont :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
20-02-96 : FMN ; Suppression code inutile:
01-04-96 : CAL ; Integration MINSK portage WNT

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_Memory.hxx>

/*----------------------------------------------------------------------*/
static  TStatus  TextFontDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextFontAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextFontPrint( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  TextFontDisplay,             /* PickTraverse */
  TextFontDisplay,
  TextFontAdd,
  0,             /* Delete */
  TextFontPrint,
  0              /* Inquire */
};


MtblPtr
TelTextFontInitClass( TelType *el )
{
  *el = TelTextFont;
  return MtdTbl;
}

static  TStatus
TextFontAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->pdata = k[0]->data.pdata;

  return TSuccess;
}


static  TStatus
TextFontDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY     key;

  key.id = TelTextFont;
  key.data.pdata = data.pdata;
  TsmSetAttri( 1, &key );

  return TSuccess;
}


static  TStatus
TextFontPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  fprintf( stdout, "TelTextFont. %s\n", (char *) data.pdata );
  fprintf( stdout, "\n" );

  return TSuccess;
}
