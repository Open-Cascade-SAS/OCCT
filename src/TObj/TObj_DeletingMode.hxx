// File:        TObj_DeletingMode.hxx
// Created:     Mon Nov 22 13:00:11 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_DeletingMode_HeaderFile
#define TObj_DeletingMode_HeaderFile

typedef enum
{
  TObj_FreeOnly = 0,   //!< Delete objects only without dependence.
  TObj_KeepDepending,  //!< Remove object if depending one will be correct elsewhere.
  TObj_Forced          //!< Delete this object and all depenging object.
} TObj_DeletingMode;     

#endif

#ifdef _MSC_VER
#pragma once
#endif
