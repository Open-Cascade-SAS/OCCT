/***********************************************************************

FONCTION :
----------
File OpenGl_textdisplaystyle :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
30-11-98 : FMN ; Creation.

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
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_Memory.hxx>

/*----------------------------------------------------------------------*/
/*
* Fonctions statiques
*/

static  TStatus  TextDisplayTypeDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextDisplayTypeAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextDisplayTypePrint( TSM_ELEM_DATA, Tint, cmn_key* );

/*----------------------------------------------------------------------*/

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  TextDisplayTypeDisplay,
  TextDisplayTypeAdd,
  0,             /* Delete */
  TextDisplayTypePrint,
  0              /* Inquire */
};


/*----------------------------------------------------------------------*/

MtblPtr
TelTextDisplayTypeInitClass( TelType *el )
{
  *el = TelTextDisplayType;
  return MtdTbl;
}

/*----------------------------------------------------------------------*/

static  TStatus
TextDisplayTypeAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


/*----------------------------------------------------------------------*/

static  TStatus
TextDisplayTypeDisplay( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelTextDisplayType;
  key.data.ldata = d.ldata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
TextDisplayTypePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  return TSuccess;
}

/*----------------------------------------------------------------------*/

