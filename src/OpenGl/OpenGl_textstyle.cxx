/***********************************************************************

FONCTION :
----------
File OpenGl_textstyle :


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

static  TStatus  TextStyleDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextStyleAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextStylePrint( TSM_ELEM_DATA, Tint, cmn_key* );

/*----------------------------------------------------------------------*/

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  TextStyleDisplay,
  TextStyleAdd,
  0,             /* Delete */
  TextStylePrint,
  0              /* Inquire */
};

/*----------------------------------------------------------------------*/

MtblPtr
TelTextStyleInitClass( TelType *el )
{
  *el = TelTextStyle;
  return MtdTbl;
}

/*----------------------------------------------------------------------*/

static  TStatus
TextStyleAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
TextStyleDisplay( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelTextStyle;
  key.data.ldata = d.ldata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}


/*----------------------------------------------------------------------*/

static  TStatus
TextStylePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  return TSuccess;
}

/*----------------------------------------------------------------------*/
