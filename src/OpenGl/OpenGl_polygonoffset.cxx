/*
Created : 22-03-04 : SAN : OCC4895 High-level interface for controlling polygon offsets
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
#include <OpenGl_telem_inquire.hxx>

static  TStatus  PolygonOffsetDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolygonOffsetAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolygonOffsetDelete( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  PolygonOffsetPrint( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  PolygonOffsetDisplay,
  PolygonOffsetAdd,
  PolygonOffsetDelete,
  PolygonOffsetPrint,
  0              /* Inquire */
};


MtblPtr
TelPolygonOffsetInitClass( TelType *el )
{
  *el = TelPolygonOffset;
  return MtdTbl;
}

static  TStatus
PolygonOffsetAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  tel_poffset_param data = new TEL_POFFSET_PARAM();

  if( !data )
    return TFailure;

  *data = *(tel_poffset_param)(k[0]->data.pdata);

  ((tsm_elem_data)(d.pdata))->pdata = data;

  return TSuccess;
}


static  TStatus
PolygonOffsetDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_poffset_param d;
  CMN_KEY           key;

  d = (tel_poffset_param)data.pdata;

  key.id = TelPolygonOffset;
  key.data.pdata = d;
  TsmSetAttri( 1, &key );

  return TSuccess;
}


static  TStatus
PolygonOffsetDelete( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  if (data.pdata)
    delete data.pdata;
  return TSuccess;
}


static  TStatus
PolygonOffsetPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  tel_poffset_param p;
  p = (tel_poffset_param)data.pdata;

  fprintf( stdout, "TelPolygonOffset. Mode = %x, Factor = %f, Units = %f\n", 
    p->mode, p->factor, p->units);
  fprintf( stdout, "\n" );

  return TSuccess;
}

