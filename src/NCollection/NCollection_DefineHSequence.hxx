// File:      NCollection_DefineHSequence.hxx
// Created:   29.01.01 12:58:53
// Author:    Alexander GRIGORIEV
//            Automatically created from NCollection_HSequence.hxx by GAWK
// Copyright: Open Cascade 2002

#ifndef NCollection_DefineHSequence_HeaderFile
#define NCollection_DefineHSequence_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <NCollection_DefineSequence.hxx>
#include <MMgt_TShared.hxx>

//      Declaration of Sequence class managed by Handle

#define DEFINE_HSEQUENCE(HClassName, _SequenceType_)                           \
                                                                               \
class HClassName : public _SequenceType_,                                      \
                   public MMgt_TShared {                                       \
 public:                                                                       \
   inline                       HClassName ();                                 \
   inline                       HClassName (const _SequenceType_&);            \
   inline const _SequenceType_& Sequence        () const;                      \
   inline _SequenceType_&       ChangeSequence  ();                            \
   DEFINE_STANDARD_RTTI (HClassName)                                           \
};                                                                             \
                                                                               \
DEFINE_STANDARD_HANDLE (HClassName, MMgt_TShared)                              \
                                                                               \
inline HClassName::HClassName () :                                             \
       _SequenceType_(),                                                       \
       MMgt_TShared() {}                                                       \
                                                                               \
inline HClassName::HClassName (const _SequenceType_& anOther) :                \
       _SequenceType_(anOther),                                                \
       MMgt_TShared() {}                                                       \
                                                                               \
inline const _SequenceType_& HClassName::Sequence () const                     \
{ return * (const _SequenceType_ *) this; }                                    \
                                                                               \
inline _SequenceType_& HClassName::ChangeSequence ()                           \
{ return * (_SequenceType_ *) this; }                                          \

#define IMPLEMENT_HSEQUENCE(HClassName)                                        \
IMPLEMENT_STANDARD_HANDLE  (HClassName, MMgt_TShared)                          \
IMPLEMENT_STANDARD_RTTIEXT (HClassName, MMgt_TShared)

#endif
