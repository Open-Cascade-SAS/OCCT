// File:        NCollection_DefineHArray2.hxx
// Created:     Mon Apr 29 11:36:31 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>
//            Automatically created from NCollection_HArray2.hxx by GAWK
// Copyright:   Open Cascade 2002

#ifndef NCollection_DefineHArray2_HeaderFile
#define NCollection_DefineHArray2_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <NCollection_DefineArray2.hxx>
#include <MMgt_TShared.hxx>

//      Declaration of Array2 class managed by Handle

#define DEFINE_HARRAY2(HClassName, _Array2Type_)                               \
                                                                               \
class HClassName : public _Array2Type_,                                        \
                   public MMgt_TShared {                                       \
 public:                                                                       \
   inline                     HClassName  (const Standard_Integer theRowLower, \
                                           const Standard_Integer theRowUpper, \
                                           const Standard_Integer theColLower, \
                                           const Standard_Integer theColUpper);\
   inline                     HClassName  (const _Array2Type_&);               \
   inline const _Array2Type_& Array2      () const;                            \
   inline _Array2Type_&       ChangeArray2();                                  \
   DEFINE_STANDARD_RTTI (HClassName)                                           \
};                                                                             \
                                                                               \
DEFINE_STANDARD_HANDLE (HClassName, MMgt_TShared)                              \
                                                                               \
inline HClassName::HClassName (const Standard_Integer theRowLow,               \
                               const Standard_Integer theRowUpp,               \
                               const Standard_Integer theColLow,               \
                               const Standard_Integer theColUpp) :             \
       _Array2Type_ (theRowLow, theRowUpp, theColLow, theColUpp),              \
       MMgt_TShared() {}                                                       \
                                                                               \
inline HClassName::HClassName (const _Array2Type_& theOther) :                 \
       _Array2Type_(theOther),                                                 \
       MMgt_TShared() {}                                                       \
                                                                               \
inline const _Array2Type_& HClassName::Array2 () const                         \
{ return * (const _Array2Type_ *) this; }                                      \
                                                                               \
inline _Array2Type_& HClassName::ChangeArray2 ()                               \
{ return * (_Array2Type_ *) this; }                                            \

#define IMPLEMENT_HARRAY2(HClassName)                                          \
IMPLEMENT_STANDARD_HANDLE  (HClassName, MMgt_TShared)                          \
IMPLEMENT_STANDARD_RTTIEXT (HClassName, MMgt_TShared)

#endif
