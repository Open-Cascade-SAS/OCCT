// File:        NCollection_DefineHSet.hxx
// Created:     Mon Apr 29 12:34:55 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>

#ifndef NCollection_DefineHSet_HeaderFile
#define NCollection_DefineHSet_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <NCollection_DefineSet.hxx>
#include <MMgt_TShared.hxx>

//      Declaration of Set class managed by Handle

#define DEFINE_HSET(HClassName, _SetType_)                                     \
                                                                               \
class HClassName : public _SetType_,                                           \
                   public MMgt_TShared {                                       \
 public:                                                                       \
   inline                  HClassName ();                                      \
   inline                  HClassName (const _SetType_& anOther);              \
   inline const _SetType_& Set        () const;                                \
   inline _SetType_&       ChangeSet  ();                                      \
   DEFINE_STANDARD_RTTI (HClassName)                                           \
};                                                                             \
                                                                               \
DEFINE_STANDARD_HANDLE (HClassName, MMgt_TShared)                              \
                                                                               \
inline HClassName::HClassName () :                                             \
       _SetType_(),                                                            \
       MMgt_TShared() {}                                                       \
                                                                               \
inline HClassName::HClassName (const _SetType_& anOther) :                     \
       _SetType_(anOther),                                                     \
       MMgt_TShared() {}                                                       \
                                                                               \
inline const _SetType_& HClassName::Set () const                               \
{ return * (const _SetType_ *) this; }                                         \
                                                                               \
inline _SetType_& HClassName::ChangeSet ()                                     \
{ return * (_SetType_ *) this; }                                               \

#define IMPLEMENT_HSET(HClassName)                                             \
IMPLEMENT_STANDARD_HANDLE  (HClassName, MMgt_TShared)                          \
IMPLEMENT_STANDARD_RTTIEXT (HClassName, MMgt_TShared)

#endif
