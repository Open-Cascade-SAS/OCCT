// File:       TObj_SequenceOfIterator.hxx
// Created:    Thu Nov 23 12:27:24 2004
// Author:     Pavel TELKOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_SequenceOfIterator_HeaderFile
#define TObj_SequenceOfIterator_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_DefineSequence.hxx>

#include <TObj_ObjectIterator.hxx>

DEFINE_BASECOLLECTION (TObj_CollectionOfIterator, Handle(TObj_ObjectIterator))
DEFINE_SEQUENCE (TObj_SequenceOfIterator, TObj_CollectionOfIterator,
                 Handle(TObj_ObjectIterator))

#endif

#ifdef _MSC_VER
#pragma once
#endif
