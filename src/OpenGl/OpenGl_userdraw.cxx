/*
// File:  OpenGl_userdraw.c
// Created: Thu Mar  3 12:34:02 2005
// Author:  Sergey ZERTCHANINOV
//    <sergey.zertchaninov@opencascade.com>
// Copyright:  Open CASCADE 2005
*/

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_callback.hxx>


static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0, /* UserdrawPick, */
  0, /* UserdrawDisplay, */
  0, /* UserdrawAdd, */
  0, /* UserdrawDelete, */
  0, /* UserdrawPrint, */
  0, /* UserdrawInquire */
};

MtblPtr TelUserdrawInitClass( TelType* el )
{
  *el = TelUserdraw;
  return MtdTbl;
}

MtblPtr GetCallbackTable ()
{
  return MtdTbl;
}
