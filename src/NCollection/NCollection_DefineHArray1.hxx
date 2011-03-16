// File:        NCollection_DefineHArray1.hxx
// Created:     Mon Apr 29 11:36:31 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>
//            Automatically created from NCollection_HArray1.hxx by GAWK
// Copyright:   Open Cascade 2002

#ifndef NCollection_DefineHArray1_HeaderFile
#define NCollection_DefineHArray1_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <NCollection_DefineArray1.hxx>
#include <MMgt_TShared.hxx>

//      Declaration of Array1 class managed by Handle

#define DEFINE_HARRAY1(HClassName, _Array1Type_)                               \
                                                                               \
class HClassName : public _Array1Type_,                                        \
                   public MMgt_TShared {                                       \
 public:                                                                       \
   inline                     HClassName    (const Standard_Integer theLower,  \
                                             const Standard_Integer theUpper); \
   inline                     HClassName    (const _Array1Type_&);             \
   inline const _Array1Type_& Array1        () const;                          \
   inline _Array1Type_&       ChangeArray1  ();                                \
   DEFINE_STANDARD_RTTI (HClassName)                                           \
};                                                                             \
                                                                               \
DEFINE_STANDARD_HANDLE (HClassName, MMgt_TShared)                              \
                                                                               \
inline HClassName::HClassName (const Standard_Integer theLower,                \
                               const Standard_Integer theUpper) :              \
       _Array1Type_ (theLower,theUpper),                                       \
       MMgt_TShared() {}                                                       \
                                                                               \
inline HClassName::HClassName (const _Array1Type_& theOther) :                 \
       _Array1Type_(theOther),                                                 \
       MMgt_TShared() {}                                                       \
                                                                               \
inline const _Array1Type_& HClassName::Array1 () const                         \
{ return * (const _Array1Type_ *) this; }                                      \
                                                                               \
inline _Array1Type_& HClassName::ChangeArray1 ()                               \
{ return * (_Array1Type_ *) this; }                                            \

#define IMPLEMENT_HARRAY1(HClassName)                                          \
IMPLEMENT_STANDARD_HANDLE  (HClassName, MMgt_TShared)                          \
IMPLEMENT_STANDARD_RTTIEXT (HClassName, MMgt_TShared)

#endif
