// File:      TObj_SequenceOfObject.hxx
// Created:   22.11.04 13:31:14
// Author:    Pavel TELKOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_SequenceOfObject_HeaderFile
#define TObj_SequenceOfObject_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_DefineSequence.hxx>
#include <NCollection_DefineHSequence.hxx>

#include <TObj_Common.hxx>
#include <Handle_TObj_Object.hxx>

DEFINE_BASECOLLECTION (TObj_CollectionOfObject,Handle_TObj_Object)

DEFINE_SEQUENCE (TObj_SequenceOfObject,
                 TObj_CollectionOfObject,Handle(TObj_Object))

DEFINE_HSEQUENCE (TObj_HSequenceOfObject,
                  TObj_SequenceOfObject)

#endif
