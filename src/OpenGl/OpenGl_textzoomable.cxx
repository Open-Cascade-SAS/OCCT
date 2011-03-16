// File:      OpenGl_textzoomable.cxx
// Created:   09.09.09
// Author:    Paul Supryatkin
// Copyright: Open CASCADE 2009
                                             
#include <stddef.h>
#include <stdio.h>

#include <OpenGl_tgl_all.hxx>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_Memory.hxx>

static  TStatus  TextZoomableDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextZoomableAdd( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  TextZoomableDisplay,
  TextZoomableAdd,
  0,             /* Delete */
  0,
  0              /* Inquire */
};

MtblPtr
TelTextZoomableInitClass( TelType *el )
{
  *el = TelTextZoomable;
  return MtdTbl;
}

static  TStatus
TextZoomableDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelTextZoomable;
  key.data.ldata = data.ldata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}

static  TStatus
TextZoomableAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;
  return TSuccess;
}

