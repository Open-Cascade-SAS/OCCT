// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PCollection_HExtendedString.ixx>
#include <PCollection_HAsciiString.hxx>
#include <Standard_ExtString.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#if defined(HAVE_STRING_H)
# include <string.h>
#endif
#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_LIBC_H)
# include <libc.h>
#endif

//------------------------------------------------------------------------
//  Create from an ExtendedString of TCollection
//------------------------------------------------------------------------
PCollection_HExtendedString::PCollection_HExtendedString
                  (const TCollection_ExtendedString& S):Data(S.Length())
{
   for( Standard_Integer i = 1; i <= Data.Length() ; i++) 
                            Data.SetValue(i-1, S.Value(i)) ;   
}

//------------------------------------------------------------------------
//  Create from a ExtCharacter
//------------------------------------------------------------------------
PCollection_HExtendedString::PCollection_HExtendedString
            (const Standard_ExtCharacter C): Data(1)
{
   Data.SetValue(0, C);
}

//------------------------------------------------------------------------
//  Create from a range of an HExtendedString of PCollection
//------------------------------------------------------------------------
PCollection_HExtendedString::PCollection_HExtendedString 
  (const Handle(PCollection_HExtendedString)& S, 
   const Standard_Integer FromIndex, const Standard_Integer ToIndex) : Data(ToIndex-FromIndex+1)
{
   for( Standard_Integer i = 0 , k = FromIndex; i < Data.Length() ; i++, k++) 
                        Data.SetValue(i, S->Value(k));
}

//-----------------------------------------------------------------------
// Create : from a CString
//-----------------------------------------------------------------------
PCollection_HExtendedString::PCollection_HExtendedString(const Standard_CString S)
                                               : Data((Standard_Integer) strlen(S))
{
   for( Standard_Integer i = 0 ; i < Data.Length() ; i++)  {
         Standard_ExtCharacter val = ToExtCharacter(S[i]);
         Data.SetValue(i, val) ;
   }
}

//------------------------------------------------------------------------
//  Create from an HAsciiString from PCollection
//------------------------------------------------------------------------
PCollection_HExtendedString::PCollection_HExtendedString 
          (const Handle(PCollection_HAsciiString)& S) : Data(S->Length())
 {
   for( Standard_Integer i = 1; i <= Data.Length() ; i++)  {
//     convert the character i of S
       Standard_ExtCharacter val = ToExtCharacter(S->Value(i)) ;
       Data.SetValue(i-1,val );   
   }   
}

//------------------------------------------------------------------------
//  Append
//------------------------------------------------------------------------
void PCollection_HExtendedString::Append 
                   (const Handle(PCollection_HExtendedString)& S)
{
   InsertAfter(Length(),S);
}

//------------------------------------------------------------------------
//  Center
//------------------------------------------------------------------------
void PCollection_HExtendedString::Center 
           (const Standard_Integer Width, const Standard_ExtCharacter Filler)
{
   if (Width < 0) Standard_NegativeValue::Raise();
   Standard_Integer size1 = Length();
   if(Width > size1) {
      Standard_Integer size2 = size1 + ((Width - size1)/2);
      LeftJustify(size2,Filler);
      RightJustify(Width,Filler);
   }
}

//------------------------------------------------------------------------
//  ChangeAll
//------------------------------------------------------------------------
void PCollection_HExtendedString::ChangeAll 
     (const Standard_ExtCharacter C, const Standard_ExtCharacter NewC)
{
   for( Standard_Integer i = 0 ; i < Data.Length(); i++) {
      if(Data(i) == C) Data.SetValue(i, NewC);
   }
}

//------------------------------------------------------------------------
//  Clear
//------------------------------------------------------------------------
void PCollection_HExtendedString::Clear ()
{
   Data.Resize(0);
}

//------------------------------------------------------------------------
//  Convert
//------------------------------------------------------------------------
TCollection_ExtendedString PCollection_HExtendedString::Convert() const
{
   Standard_Integer L = Length();
   TCollection_ExtendedString TString (L,' ');
   for (Standard_Integer i = 1 ; i <= L ; i++) {
      TString.SetValue(i,Value(i));
   }
   return TString;
}

//------------------------------------------------------------------------
//  FirstLocationInSet
//------------------------------------------------------------------------
Standard_Integer PCollection_HExtendedString::FirstLocationInSet 
              (const Handle(PCollection_HExtendedString)& Set, 
               const Standard_Integer FromIndex, 
               const Standard_Integer ToIndex) const
{
   if (Length() == 0 || Set->Length() == 0) return 0;
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                     Standard_OutOfRange::Raise();
   for(Standard_Integer i = FromIndex-1 ; i <= ToIndex-1; i++)
     for(Standard_Integer j = 1; j <= Set->Length(); j++) 
       if(Data(i) == Set->Value(j)) return (i+1);
   return 0;
}

//------------------------------------------------------------------------
//  FirstLocationNotInset
//------------------------------------------------------------------------
Standard_Integer PCollection_HExtendedString::FirstLocationNotInSet 
      (const Handle(PCollection_HExtendedString)& Set, const Standard_Integer FromIndex, 
       const Standard_Integer ToIndex) const
{
   if (Length() == 0 || Set->Length() == 0) return 0;
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                     Standard_OutOfRange::Raise();
   Standard_Boolean find;
   for(Standard_Integer i = FromIndex-1 ; i <= ToIndex-1; i++) {
     find = Standard_False;
     for(Standard_Integer j = 1; j <= Set->Length(); j++) { 
       if (Data(i) == Set->Value(j)) find = Standard_True;
     }
     if (!find)  return (i+1);
   }
   return 0;
}

//------------------------------------------------------------------------
//  InsertAfter
//------------------------------------------------------------------------
void PCollection_HExtendedString::InsertAfter 
           (const Standard_Integer Index, const Handle(PCollection_HExtendedString)& S)
{
   Standard_Integer i ;
   Standard_Integer size1 = Length();
   Standard_Integer size2 = S->Length();
#ifndef NOBOUNDCHECK
   if (Index < 0 || Index > size1) Standard_OutOfRange::Raise();
#endif
   Data.Resize(size1+size2);
   for( i = size1-1 ; i >= Index; i--) Data.SetValue(size2+i,Data(i));
   for( i = 1 ; i <= size2; i++) Data.SetValue(Index+i-1,S->Value(i));
}

//------------------------------------------------------------------------
//  InsertBefore
//------------------------------------------------------------------------
void PCollection_HExtendedString::InsertBefore 
           (const Standard_Integer Index, const Handle(PCollection_HExtendedString)& S)
{
   Standard_Integer i ;
   Standard_Integer size1 = Length();
   Standard_Integer size2 = S->Length();
#ifndef NOBOUNDCHECK
   if (Index < 0 || Index > size1) Standard_OutOfRange::Raise();
#endif
   Data.Resize(size1+size2);
   for( i = size1-1 ; i >= Index-1 ; i--) 
                                        Data.SetValue(size2+i,Data(i));
  for( i = 1 ; i <= size2; i++) Data.SetValue(Index+i-2, S->Value(i));
}

//------------------------------------------------------------------------
//  IsAscii
//------------------------------------------------------------------------
Standard_Boolean PCollection_HExtendedString::IsAscii() const
{
   for( Standard_Integer i = 0; i < Data.Length() ; i++)  {
             if (!IsAnAscii(Data(i))) return Standard_False;
   }
   return Standard_True;
}

//------------------------------------------------------------------------
//  IsDifferent
//------------------------------------------------------------------------
Standard_Boolean PCollection_HExtendedString::IsDifferent 
                        (const Handle(PCollection_HExtendedString)& S) const
{
   Standard_Integer size = Length();
   if( size != S->Length()) return Standard_True;
   Standard_Integer i = 1 ;
   Standard_Boolean different = Standard_False;
   while (i <= size && !different) {
      if (Data(i-1) != S->Value(i)) different = Standard_True;
      i++;
   }
   return different;
}

//------------------------------------------------------------------------
//  IsEmpty
//------------------------------------------------------------------------
Standard_Boolean PCollection_HExtendedString::IsEmpty () const
{
   return (Data.Length() == 0);
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean PCollection_HExtendedString::IsLess(
        const Handle(PCollection_HExtendedString)& other) const
{
   Standard_Integer mysize = Data.Length();
   Standard_Integer size = other->Length();
   Standard_Integer i = 0;
   Standard_Integer j = 1;
   while (i < mysize && j <= size) {
      if (Data(i) < other->Value(j)) return (Standard_True);
      if (Data(i) > other->Value(j)) return (Standard_False);
      i++;
      j++;
   }
   if (i == mysize && j <= size) return (Standard_True);
   return (Standard_False);
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean PCollection_HExtendedString::IsGreater
    (const Handle(PCollection_HExtendedString)& other) const
{
   Standard_Integer mysize = Data.Length();
   Standard_Integer size = other->Length();
   Standard_Integer i = 0;
   Standard_Integer j = 1;
   while (i < mysize && j <= size) {
      if (Data(i) < other->Value(j)) return (Standard_False);
      if (Data(i) > other->Value(j)) return (Standard_True);
      i++;
      j++;
   }
   if (j == size && j < mysize) return (Standard_True);
   return (Standard_False);
}

//------------------------------------------------------------------------
//  IsSameString
//------------------------------------------------------------------------
Standard_Boolean PCollection_HExtendedString::IsSameString 
                        (const Handle(PCollection_HExtendedString)& S) const
{
   Standard_Integer size1 = Length();
   if( size1 != S->Length()) return Standard_False;
   for( Standard_Integer i = 1 ; i <= size1; i++) {
     if(Data(i-1) != S->Value(i)) return Standard_False;
   }
   return Standard_True;
}

//------------------------------------------------------------------------
//  LeftAdjust
//------------------------------------------------------------------------
void PCollection_HExtendedString::LeftAdjust ()
{
   Standard_Integer i ;
   
   if (!IsAscii()) Standard_OutOfRange::Raise();
   for ( i = 1 ; i <= Length() ; i ++) {
      if (!IsSpace((Standard_Character)Value(i))) break;
   }
   if( i > 1 ) Remove(1,i-1);
}

//------------------------------------------------------------------------
//  LeftJustify
//------------------------------------------------------------------------
void PCollection_HExtendedString::LeftJustify 
     (const Standard_Integer Width, const Standard_ExtCharacter Filler)
{
   if (Width < 0) Standard_NegativeValue::Raise();
   Standard_Integer size1 = Length();
   if(Width > size1) {
     Data.Resize(Width);
     for(Standard_Integer i = size1; i < Width ; i++) Data.SetValue(i, Filler);
   }
}

//------------------------------------------------------------------------
//  Length
//------------------------------------------------------------------------
Standard_Integer PCollection_HExtendedString::Length () const
{
   return Data.Length();
}

//------------------------------------------------------------------------
//  Location
//------------------------------------------------------------------------
Standard_Integer PCollection_HExtendedString::Location 
   (const Standard_Integer N, const Standard_ExtCharacter C, 
    const Standard_Integer FromIndex, const Standard_Integer ToIndex) const
{
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                      Standard_OutOfRange::Raise();
   for(Standard_Integer i = FromIndex-1, icount = 0; i <= ToIndex-1; i++) 
              if(Data(i) == C) {
	        icount++;
	        if ( icount == N ) return (i+1);
	      }
   return 0 ;
}

//------------------------------------------------------------------------
//  Location
//------------------------------------------------------------------------
Standard_Integer PCollection_HExtendedString::Location 
        (const Handle(PCollection_HExtendedString)& S, const Standard_Integer FromIndex, 
         const Standard_Integer ToIndex) const
{
   if (Length() == 0 || S->Length() == 0) return 0;
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                      Standard_OutOfRange::Raise();
   for(Standard_Integer i = FromIndex-1, k = 1, l = FromIndex-2; i < ToIndex; i++){
     if(Data(i) == S->Value(k)) {
       k++;
       if ( k > S->Length()) return l + 2;
     }
     else {
       k = 1;
       l = i;
     }
  }
  return 0;
}

//------------------------------------------------------------------------
//  Prepend
//------------------------------------------------------------------------
void PCollection_HExtendedString::Prepend 
                    (const Handle(PCollection_HExtendedString)& S)
{
   InsertAfter(0,S);
}


//------------------------------------------------------------------------
//  Print
//------------------------------------------------------------------------
void PCollection_HExtendedString::Print (Standard_OStream& S) const
{
   Standard_Integer len = Data.Length() ;
   for(Standard_Integer i = 0; i < len ; i++) {
       S.width(4);
       S.fill('0');
       S << hex << Data(i);
   }
}

//------------------------------------------------------------------------
//  Remove
//------------------------------------------------------------------------
void PCollection_HExtendedString::Remove (const Standard_Integer Index)
{
   if (Index < 0 || Index > Length()) Standard_OutOfRange::Raise();
   Remove(Index,Index);
}

//------------------------------------------------------------------------
//  Remove
//------------------------------------------------------------------------
void PCollection_HExtendedString::Remove 
    (const Standard_Integer FromIndex, const Standard_Integer ToIndex)
{
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                     Standard_OutOfRange::Raise();
   Standard_Integer size1 = Length();
   for( Standard_Integer i = ToIndex, j = FromIndex-1; i < size1 ; i++, j++)
     Data.SetValue(j,Data(i));
   Data.Resize(size1-(ToIndex-FromIndex+1));
}

//------------------------------------------------------------------------
//  RemoveAll
//------------------------------------------------------------------------
void PCollection_HExtendedString::RemoveAll (const Standard_ExtCharacter C)
{
   Standard_Integer i ;
   Standard_Integer j ;
   Standard_Integer size1 = Length();
   for( i = 0, j = 0; i < size1 ; i++) {
       if (Data(i) == C) continue;
       Data.SetValue(j++, Data(i));
   }
    Data.Resize(j);
}

//------------------------------------------------------------------------
//  RightAdjust
//------------------------------------------------------------------------
void PCollection_HExtendedString::RightAdjust ()
{
   Standard_Integer i ;
   if (! IsAscii()) Standard_OutOfRange::Raise();
   for ( i = Length() ; i >= 1 ; i --) {
      if (!IsSpace((Standard_Character)Value(i))) break;
   }
   if( i < Length() ) Remove(i+1,Length());
}

//------------------------------------------------------------------------
//  RightJustify
//------------------------------------------------------------------------
void PCollection_HExtendedString::RightJustify 
           (const Standard_Integer Width, const Standard_ExtCharacter Filler)
{
   Standard_Integer i ;
   Standard_Integer k ;
   if (Width < 0) Standard_NegativeValue::Raise();
   Standard_Integer size1 = Length();
   if(Width > size1) {
      Data.Resize(Width);
      for ( i = size1-1, k = Width-1 ; i >= 0 ; i--, k--) 
               Data.SetValue(k, Data(i));
      for(; k >= 0 ; k--) Data.SetValue(k, Filler);
   }
}

//------------------------------------------------------------------------
//  SetValue
//------------------------------------------------------------------------
void PCollection_HExtendedString::SetValue 
                (const Standard_Integer Index, const Standard_ExtCharacter C)
{
   if (Index < 0 || Index > Length()) Standard_OutOfRange::Raise();
   Data(Index-1) = C;
}

//------------------------------------------------------------------------
//  SetValue
//------------------------------------------------------------------------
void PCollection_HExtendedString::SetValue 
           (const Standard_Integer Index, const Handle(PCollection_HExtendedString)& S)
{

   Standard_Integer size1 = Length();
   Standard_Integer size2 = S->Length();
   Standard_Integer size3 = size2 + Index - 1;
#ifndef NOBOUNDCHECK
   if (Index < 0 || Index > size1) Standard_OutOfRange::Raise();
#endif
   if(size1 != size3) Data.Resize(size3);
   for( Standard_Integer i = 1 ; i <= size2; i++) Data.SetValue(Index+i-2, S->Value(i));
}

//------------------------------------------------------------------------
//  Split
//------------------------------------------------------------------------
Handle(PCollection_HExtendedString) PCollection_HExtendedString::Split 
             (const Standard_Integer Index)
{
   if (Index < 0 || Index > Length()) Standard_OutOfRange::Raise();
   Handle(PCollection_HExtendedString) S2;
   if (Index != Length()) { 
      S2 = SubString(Index+1,Length());
      Data.Resize(Index);
   }
   else {
#ifndef OBJS
      Handle(PCollection_HAsciiString) s = new PCollection_HAsciiString("");
      S2 = new PCollection_HExtendedString(s);
#else
      Handle(PCollection_HAsciiString) s = new (os_segment::of(this)) PCollection_HAsciiString("");
      S2 = new (os_segment::of(this)) PCollection_HExtendedString(s);
#endif
   }
   return S2;
}

//------------------------------------------------------------------------
//  SubString
//------------------------------------------------------------------------
Handle(PCollection_HExtendedString) PCollection_HExtendedString::SubString 
             (const Standard_Integer FromIndex, const Standard_Integer ToIndex) const
{
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                      Standard_OutOfRange::Raise();
   Handle(PCollection_HExtendedString) S1;
   Handle(PCollection_HExtendedString) S2;
   S2 = this;
#ifndef OBJS
   S1 = new PCollection_HExtendedString(S2,FromIndex,ToIndex);
#else
   S1 = new (os_segment::of(this)) PCollection_HExtendedString(S2,FromIndex,ToIndex);
#endif
   return S1;
}

//------------------------------------------------------------------------
//  UsefullLength
//------------------------------------------------------------------------
Standard_Integer PCollection_HExtendedString::UsefullLength () const
{
   Standard_Integer i ;
  if (! IsAscii()) Standard_OutOfRange::Raise();
  for( i = Length() ; i >= 1 ; i--) 
      if (IsGraphic((Standard_Character)Value(i))) break;
   return (i);
}

//------------------------------------------------------------------------
//  value
//------------------------------------------------------------------------
Standard_ExtCharacter PCollection_HExtendedString::Value 
                               (const Standard_Integer Index) const
{
   if (Index < 0 || Index > Length()) Standard_OutOfRange::Raise();
   return Data(Index-1);
}

//------------------------------------------------------------------------
//  ShallowDump
//------------------------------------------------------------------------
void PCollection_HExtendedString::ShallowDump(Standard_OStream& S) const
{
   S << "begin class HExtendedString " << endl;
   ::ShallowDump(Data, S);
   S << "end class HExtendedString" << endl;
}


//------------------------------------------------------------------------
//  Assign
//------------------------------------------------------------------------
void PCollection_HExtendedString::Assign
              (const DBC_VArrayOfExtCharacter& TheField)

{
   Data = TheField;
}







