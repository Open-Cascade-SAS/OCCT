// File:      OpenGl_textzoomable.cxx
// Created:   08.10.09
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

static  TStatus  TextFontAspectDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextFontAspectAdd( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  TextFontAspectDisplay,
  TextFontAspectAdd,
  0,             /* Delete */
  0,
  0              /* Inquire */
};

MtblPtr
TelTextFontAspectInitClass( TelType *el )
{
  *el = TelTextFontAspect;
  return MtdTbl;
}

static  TStatus
TextFontAspectDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelTextFontAspect;
  key.data.ldata = data.ldata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}

static  TStatus
TextFontAspectAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;
  return TSuccess;
}

