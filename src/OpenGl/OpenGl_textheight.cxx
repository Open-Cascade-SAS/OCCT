/***********************************************************************

FONCTION :
----------
File OpenGl_textheight :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
20-02-96 : FMN ; Suppression code inutile:
01-04-96 : CAL ; Integration MINSK portage WNT
26-04-96 : FMN ; Correction warning de compilation.
20-11-97 : CAL ; RererereMerge avec le source de portage.
17-12-96 : FMN ; Warning de compilation.

************************************************************************/

#define BUC60821        //GG 12/02/01 allow tu use the nearest font size.

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
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_Memory.hxx>


/*----------------------------------------------------------------------*/
/*
* Fonctions statiques
*/

static  TStatus  TextHeightDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextHeightAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextHeightDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextHeightPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextHeightInquire( TSM_ELEM_DATA, Tint, cmn_key* );

/*----------------------------------------------------------------------*/
static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  TextHeightDisplay,             /* PickTraverse */
  TextHeightDisplay,
  TextHeightAdd,
  TextHeightDelete,
  TextHeightPrint,
  TextHeightInquire
};


/*----------------------------------------------------------------------*/
MtblPtr
TelTextHeightInitClass( TelType *el )
{
  *el = TelTextHeight;
  return MtdTbl;
}

/*----------------------------------------------------------------------*/
static  TStatus
TextHeightAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  Tfloat *data = new Tfloat();
  if( !data )
    return TFailure;

#ifdef BUC60821
  *data = k[0]->data.fdata;  
#else /*BUC60821*/
#ifndef WNT
  {
    Tint d_wdth, d_hght;
    call_tox_getscreen( &d_wdth, &d_hght );
    *data = k[0]->data.fdata * d_hght;   
  }
#else
  *data = k[0]->data.fdata;
#endif  /* WNT */
#endif  /* BUC60821 */

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}


/*----------------------------------------------------------------------*/
static  TStatus
TextHeightDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY     key;

  key.id = TelTextHeight;
  key.data.fdata = *(Tfloat*)(data.pdata);     
  TsmSetAttri( 1, &key );

  return TSuccess;
}


/*----------------------------------------------------------------------*/
static  TStatus
TextHeightDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}


/*----------------------------------------------------------------------*/
static  TStatus
TextHeightPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  fprintf( stdout, "TelTextHeight. Value = %g\n", *(Tfloat *)(data.pdata));
  fprintf( stdout, "\n" );

  return TSuccess;
}


/*----------------------------------------------------------------------*/
static TStatus
TextHeightInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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

        c = (TEL_INQ_CONTENT *)k[i]->data.pdata;
        w = c->data;
        c->act_size = 0;
        w->fdata = *f; 
        break;
      }
    }
  }

  return TSuccess;
}
/*----------------------------------------------------------------------*/
