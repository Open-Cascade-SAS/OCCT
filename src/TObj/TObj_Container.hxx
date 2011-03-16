// File:      TObj_Container.hxx
// Created:   22.11.04 13:31:14
// Author:    Pavel TELKOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_Container_HeaderFile
#define TObj_Container_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_DefineDataMap.hxx>
#include <NCollection_DefineSequence.hxx>
#include <NCollection_DefineHSequence.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <TDF_Label.hxx>

#include <TObj_Common.hxx>
#include <TObj_SequenceOfObject.hxx>

DEFINE_BASECOLLECTION (TObj_CollectionOfLabel, TDF_Label)

DEFINE_BASECOLLECTION(TObj_CollectionOfPointer, Standard_Address)

DEFINE_BASECOLLECTION (TObj_CollectionOfHSeqOfObject,
                       Handle(TObj_HSequenceOfObject))

DEFINE_DATAMAP (TObj_DataMapOfNameLabel, TObj_CollectionOfLabel,
                Handle(TCollection_HExtendedString), TDF_Label)


DEFINE_DATAMAP (TObj_DataMapOfObjectHSequenceOcafObjects,
                TObj_CollectionOfHSeqOfObject,
                Handle_TObj_Object,Handle_TObj_HSequenceOfObject)

DEFINE_DATAMAP(TObj_DataMapOfStringPointer,TObj_CollectionOfPointer,
               TCollection_AsciiString,Standard_Address)
#endif
