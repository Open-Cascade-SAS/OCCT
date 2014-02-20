// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PCollection_HAsciiString.ixx>
#include <PCollection_HExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
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
# include <stdlib.h>
#endif
#if defined(HAVE_LIBC_H)
# include <libc.h>
#endif

//------------------------------------
//  Conversion functions and variables
//------------------------------------

#define MAXLENGTH 80
static char cnvbuf[MAXLENGTH];
static Standard_Integer cnvint;
static Standard_Real cnvreal;

//-----------------------------------------------------------------------
// intstr
//-----------------------------------------------------------------------
static int intstr(Standard_Integer V,Standard_CString F)
{
  sprintf(cnvbuf,F,V);
  return (int)strlen(cnvbuf);
}

//-----------------------------------------------------------------------
// realstr
//----------------------------------------------------------------------
static int realstr(Standard_Real V, Standard_CString F)
{
  sprintf(cnvbuf,F,V);
  return (int)strlen(cnvbuf);
}
  
//-----------------------------------------------------------------------
// Create : from a CString
//-----------------------------------------------------------------------
PCollection_HAsciiString::PCollection_HAsciiString(const Standard_CString S)
                                               : Data((Standard_Integer) strlen(S))
{
   for( Standard_Integer i = 0 ; i < Data.Length() ; i++) 
                            Data.SetValue(i, S[i]) ;
}

//------------------------------------------------------------------------
//  Create from an AsciiString of TCollection
//------------------------------------------------------------------------
PCollection_HAsciiString::PCollection_HAsciiString
                  (const TCollection_AsciiString& S):Data(S.Length())
{
   for( Standard_Integer i = 1; i <= Data.Length() ; i++) 
                            Data.SetValue(i-1, S.Value(i)) ;   
}

//------------------------------------------------------------------------
//  Create from a Character
//------------------------------------------------------------------------
PCollection_HAsciiString::PCollection_HAsciiString(const Standard_Character C)
                                               : Data(1)
{
   Data.SetValue(0, C);
}

//------------------------------------------------------------------------
//  Create from a range of an HAsciiString of PCollection
//------------------------------------------------------------------------
PCollection_HAsciiString::PCollection_HAsciiString 
  (const Handle(PCollection_HAsciiString)& S, 
   const Standard_Integer FromIndex, const Standard_Integer ToIndex) : Data(ToIndex-FromIndex+1)
{
   for( Standard_Integer i = 0 , k = FromIndex; i < Data.Length() ; i++, k++) 
                        Data.SetValue(i, S->Value(k));
}

//------------------------------------------------------------------------
//  Create from an HExtendedString from PCollection
//------------------------------------------------------------------------
PCollection_HAsciiString::PCollection_HAsciiString 
          (const Handle(PCollection_HExtendedString)& S) : Data(S->Length())
{
   if (!S->IsAscii()) Standard_OutOfRange::Raise();
   for( Standard_Integer i = 1; i <= Data.Length() ; i++)  {
//     convert the character i of S 
       Standard_Character val = ToCharacter(S->Value(i)) ;
       Data.SetValue(i-1,val ) ;   
   }   
}

//------------------------------------------------------------------------
//  Create from a Real by converting it
//------------------------------------------------------------------------
PCollection_HAsciiString::PCollection_HAsciiString 
                (const Standard_Real R , const Standard_CString F) : Data(realstr(R,F))
{
   for( Standard_Integer i =0 ; i < Data.Length() ; i++) 
                            Data.SetValue(i,cnvbuf[i] );
}

//------------------------------------------------------------------------
//  Create from an Integer by converting it
//------------------------------------------------------------------------
PCollection_HAsciiString::PCollection_HAsciiString 
        (const Standard_Integer I, const Standard_CString F) : Data(intstr(I,F))
{
   for( Standard_Integer i = 0 ; i < Data.Length() ; i++) 
                            Data.SetValue(i,cnvbuf[i]) ;
}


//------------------------------------------------------------------------
//  Append
//------------------------------------------------------------------------
void PCollection_HAsciiString::Append 
                   (const Handle(PCollection_HAsciiString)& S)
{
   InsertAfter(Length(),S);
}

//------------------------------------------------------------------------
//  Capitalize
//------------------------------------------------------------------------
void PCollection_HAsciiString::Capitalize ()
{
   for (Standard_Integer i = 0 ; i < Length() ; i++) {
      if( i == 0) Data.SetValue(i, UpperCase(Data(i)) );
      else Data.SetValue(i, LowerCase(Data(i)) );
   }
}

//------------------------------------------------------------------------
//  Center
//------------------------------------------------------------------------
void PCollection_HAsciiString::Center 
           (const Standard_Integer Width, const Standard_Character Filler)
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
void PCollection_HAsciiString::ChangeAll 
     (const Standard_Character C, const Standard_Character NewC, const Standard_Boolean CaseSensitive)
{
   for( Standard_Integer i = 0 ; i < Data.Length(); i++) {
      if (CaseSensitive) {
          if(Data(i) == C) Data.SetValue(i, NewC);
      }
      else {
          if(UpperCase(Data(i)) == UpperCase(C)) Data.SetValue(i, NewC);
      }
   }
}

//------------------------------------------------------------------------
//  Clear
//------------------------------------------------------------------------
void PCollection_HAsciiString::Clear ()
{
   Data.Resize(0);
}

//------------------------------------------------------------------------
//  Convert
//------------------------------------------------------------------------
TCollection_AsciiString PCollection_HAsciiString::Convert() const
{
   Standard_Integer L = Length();
   TCollection_AsciiString TString (L,' ');
   for (Standard_Integer i = 1 ; i <= L ; i++) {
      TString.SetValue(i,Value(i));
   }
   return TString;
}

//------------------------------------------------------------------------
//  FirstLocationInSet
//------------------------------------------------------------------------
Standard_Integer PCollection_HAsciiString::FirstLocationInSet 
              (const Handle(PCollection_HAsciiString)& Set, 
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
Standard_Integer PCollection_HAsciiString::FirstLocationNotInSet 
      (const Handle(PCollection_HAsciiString)& Set, const Standard_Integer FromIndex, 
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
void PCollection_HAsciiString::InsertAfter 
           (const Standard_Integer Index, const Handle(PCollection_HAsciiString)& S)
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
void PCollection_HAsciiString::InsertBefore 
           (const Standard_Integer Index, const Handle(PCollection_HAsciiString)& S)
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
//  IntegerValue
//------------------------------------------------------------------------
Standard_Integer PCollection_HAsciiString::IntegerValue () const
{
   if (!IsIntegerValue()) Standard_NumericError::Raise();
   return cnvint;
}

//------------------------------------------------------------------------
//  IsDifferent
//------------------------------------------------------------------------
Standard_Boolean PCollection_HAsciiString::IsDifferent 
                            (const Handle(PCollection_HAsciiString)& S) const
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
Standard_Boolean PCollection_HAsciiString::IsEmpty () const
{
   return (Data.Length() == 0);
}

//------------------------------------------------------------------------
//  IsGreater
//------------------------------------------------------------------------
Standard_Boolean PCollection_HAsciiString::IsGreater
                            (const Handle(PCollection_HAsciiString)& S) const
{
   TCollection_AsciiString TMe = Convert();
   TCollection_AsciiString TS  = S->Convert();
   return TMe.IsGreater(TS);
}

//------------------------------------------------------------------------
//  IsIntegervalue
//------------------------------------------------------------------------
Standard_Boolean PCollection_HAsciiString::IsIntegerValue () const
{
   Standard_Integer i ;
#if defined(__osf__) || defined(DECOSF1)
#ifdef OBJS
 #pragma pointer_size (save)
 #pragma pointer_size (long)
   char *ptr;
 #pragma pointer_size (restore)
#else
   char *ptr;
#endif
#else
   char *ptr;
#endif
#ifndef NOBOUNDCHECK
   if ( Data.Length() > MAXLENGTH ) return Standard_False;
#endif
   Handle(TCollection_HAsciiString) astring;
   astring  = new TCollection_HAsciiString (this->Convert());
   astring->LeftAdjust();
   astring->RightAdjust(); 
   for(i = 0; i < astring->Length(); i++) 
                            cnvbuf[i] = astring->Value(i+1);
   cnvbuf[i] = 0;
   cnvint = strtol(cnvbuf,&ptr,10);
   if (ptr < cnvbuf+astring->Length()) return Standard_False;
   else                                return Standard_True;
}

//------------------------------------------------------------------------
//  Isless
//------------------------------------------------------------------------
Standard_Boolean PCollection_HAsciiString::IsLess 
                            (const Handle(PCollection_HAsciiString)& S) const
{
   TCollection_AsciiString TMe = Convert();
   TCollection_AsciiString TS  = S->Convert();
   return TMe.IsLess(TS);
}

//------------------------------------------------------------------------
//  IsRealValue
//------------------------------------------------------------------------
Standard_Boolean PCollection_HAsciiString::IsRealValue () const
{
   Standard_Integer i ;
#if defined(__osf__) || defined(DECOSF1)
#ifdef OBJS
 #pragma pointer_size (save)
 #pragma pointer_size (long)
   char *ptr;
 #pragma pointer_size (restore)
#else
   char *ptr;
#endif
#else
   char *ptr;
#endif
#ifndef NOBOUNDCHECK
   if ( Data.Length() > MAXLENGTH ) return Standard_False;
#endif
   Handle(TCollection_HAsciiString) astring;
   astring  = new TCollection_HAsciiString (this->Convert());
   astring->LeftAdjust();
   astring->RightAdjust(); 
   for(i = 0; i < astring->Length(); i++)
                            cnvbuf[i] = astring->Value(i+1);
   cnvbuf[i] = 0;
   cnvreal = Strtod(cnvbuf,&ptr);
   if (ptr < cnvbuf+astring->Length()) return Standard_False;
   else                                return Standard_True;
}

//------------------------------------------------------------------------
//  IsSameString
//------------------------------------------------------------------------
Standard_Boolean PCollection_HAsciiString::IsSameString 
                        (const Handle(PCollection_HAsciiString)& S) const
{
   Standard_Integer i ;
   Standard_Integer size1 = Length();
   if( size1 != S->Length()) return Standard_False;
   for( i = 1 ; i <= size1; i++) {
     if(Data(i-1) != S->Value(i)) return Standard_False;
   }
   return Standard_True;
}

//------------------------------------------------------------------------
//  IsSameString
//------------------------------------------------------------------------
Standard_Boolean PCollection_HAsciiString::IsSameString 
  (const Handle(PCollection_HAsciiString)& S, const Standard_Boolean CaseSensitive) const
{
   Standard_Integer size1 = Length();
   if( size1 != S->Length()) return Standard_False;
   for( Standard_Integer i = 1 ; i <= size1; i++) {
     if(CaseSensitive){
       if(Data(i-1) != S->Value(i)) return Standard_False;
     }
     else {
       if(UpperCase(Data(i-1)) != UpperCase(S->Value(i))) return Standard_False;
     }
   }
   return Standard_True;
}

//------------------------------------------------------------------------
//  LeftAdjust
//------------------------------------------------------------------------
void PCollection_HAsciiString::LeftAdjust ()
{
   Standard_Integer i ;
   for(i = 0 ; i < Data.Length() ; i ++) if(!IsSpace(Data(i))) break;
   if( i > 0 ) Remove(1,i);
}

//------------------------------------------------------------------------
//  LeftJustify
//------------------------------------------------------------------------
void PCollection_HAsciiString::LeftJustify 
     (const Standard_Integer Width, const Standard_Character Filler)
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
Standard_Integer PCollection_HAsciiString::Length () const
{
   return Data.Length();
}

//------------------------------------------------------------------------
//  Location
//------------------------------------------------------------------------
Standard_Integer PCollection_HAsciiString::Location 
   (const Standard_Integer N, const Standard_Character C, 
    const Standard_Integer FromIndex, const Standard_Integer ToIndex) const
{
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                      Standard_OutOfRange::Raise();
   for(Standard_Integer i = FromIndex-1, aCount = 0; i <= ToIndex-1; i++)
              if(Data(i) == C) {
	        aCount++;
	        if ( aCount == N ) return (i+1);
	      }
   return 0 ;
}

//------------------------------------------------------------------------
//  Location
//------------------------------------------------------------------------
Standard_Integer PCollection_HAsciiString::Location 
        (const Handle(PCollection_HAsciiString)& S, const Standard_Integer FromIndex, 
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
//  Lowercase
//------------------------------------------------------------------------
void PCollection_HAsciiString::Lowercase ()
{
   for( Standard_Integer i = 0 ; i < Data.Length() ; i++) {
     Data.SetValue(i, LowerCase(Data(i)));
   }
}

//------------------------------------------------------------------------
//  Prepend
//------------------------------------------------------------------------
void PCollection_HAsciiString::Prepend 
                    (const Handle(PCollection_HAsciiString)& S)
{
   InsertAfter(0,S);
}


//------------------------------------------------------------------------
//  Print
//------------------------------------------------------------------------
void PCollection_HAsciiString::Print (Standard_OStream& S) const
{
   Standard_Integer len = Data.Length() ;
   for(Standard_Integer i = 0; i < len ; i++) {
       S << Data(i);
   }
}

//------------------------------------------------------------------------
//  RealValue
//------------------------------------------------------------------------
Standard_Real PCollection_HAsciiString::RealValue () const
{
   if(!IsRealValue()) Standard_NumericError::Raise();
   return cnvreal;
}

//------------------------------------------------------------------------
//  Remove
//------------------------------------------------------------------------
void PCollection_HAsciiString::Remove (const Standard_Integer Index)
{
   if (Index < 0 || Index > Length()) Standard_OutOfRange::Raise();
   Remove(Index,Index);
}

//------------------------------------------------------------------------
//  Remove
//------------------------------------------------------------------------
void PCollection_HAsciiString::Remove 
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
void PCollection_HAsciiString::RemoveAll 
    (const Standard_Character C, const Standard_Boolean CaseSensitive)
{
   Standard_Integer i ;
   Standard_Integer j ;
   Standard_Integer size1 = Length();
   for( i = 0, j = 0; i < size1 ; i++){
     if(CaseSensitive){
       if(Data(i) == C) continue;
     }
     else {
       if(UpperCase(Data(i)) == UpperCase(C))  continue;
     }
     Data.SetValue(j++, Data(i));
    }
    Data.Resize(j);
}

//------------------------------------------------------------------------
//  RightAdjust
//------------------------------------------------------------------------
void PCollection_HAsciiString::RightAdjust ()
{
   Standard_Integer i ;
   for( i = Data.Length()-1 ; i >= 0 ; i--) if(!IsSpace(Data(i))) break;
   if( i < Data.Length()-1 ) Remove(i+2,Data.Length());
}

//------------------------------------------------------------------------
//  RightJustify
//------------------------------------------------------------------------
void PCollection_HAsciiString::RightJustify 
           (const Standard_Integer Width, const Standard_Character Filler)
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
void PCollection_HAsciiString::SetValue 
           (const Standard_Integer Index, const Handle(PCollection_HAsciiString)& S)
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
//  SetValue
//------------------------------------------------------------------------
void PCollection_HAsciiString::SetValue 
                (const Standard_Integer Index, const Standard_Character C)
{
   if (Index < 0 || Index > Length()) Standard_OutOfRange::Raise();
   Data(Index-1) = C;
}

//------------------------------------------------------------------------
//  Split
//------------------------------------------------------------------------
Handle(PCollection_HAsciiString) PCollection_HAsciiString::Split 
             (const Standard_Integer Index)
{
   if (Index < 0 || Index > Length()) Standard_OutOfRange::Raise();
   Handle(PCollection_HAsciiString) S2;
   if (Index != Length()) { 
      S2 = SubString(Index+1,Length());
      Data.Resize(Index);
   }
   else {
#ifndef OBJS
      S2 = new PCollection_HAsciiString("");
#else
      S2 = new (os_segment::of(this)) PCollection_HAsciiString("");
#endif
   }
   return S2;
}

//------------------------------------------------------------------------
//  SubString
//------------------------------------------------------------------------
Handle(PCollection_HAsciiString) PCollection_HAsciiString::SubString 
             (const Standard_Integer FromIndex, const Standard_Integer ToIndex) const
{
   if (ToIndex > Length() || FromIndex <= 0 || FromIndex > ToIndex ) 
                      Standard_OutOfRange::Raise();
   Handle(PCollection_HAsciiString) S1;
   Handle(PCollection_HAsciiString) S2;
   S2 = this;
#ifndef OBJS
   S1 = new PCollection_HAsciiString(S2,FromIndex,ToIndex);
#else
   S1 = new (os_segment::of(this)) PCollection_HAsciiString(S2,FromIndex,ToIndex);

#endif
   return S1;
}

//------------------------------------------------------------------------
//  Token
//------------------------------------------------------------------------
Handle(PCollection_HAsciiString) PCollection_HAsciiString::Token 
                                   (const Standard_CString separators , 
                                    const Standard_Integer whichone) const
{
   TCollection_AsciiString TMe = Convert();
   Handle(PCollection_HAsciiString) 
#ifndef OBJS
       TheToken = new PCollection_HAsciiString(TMe.Token(separators,whichone));
#else
       TheToken = new (os_segment::of(this)) PCollection_HAsciiString(TMe.Token(separators,whichone));
#endif
   return TheToken;
}

//------------------------------------------------------------------------
//  Uppercase
//------------------------------------------------------------------------
void PCollection_HAsciiString::Uppercase ()
{
   for( Standard_Integer i = 0 ; i < Data.Length() ; i++) 
       Data.SetValue(i, UpperCase(Data(i)));
}

//------------------------------------------------------------------------
//  UsefullLength
//------------------------------------------------------------------------
Standard_Integer PCollection_HAsciiString::UsefullLength () const
{
   Standard_Integer i ;
   for( i = Data.Length() -1 ; i >= 0 ; i--) 
      if(IsGraphic(Data(i))) break;
   return (i+1) ;
}

//------------------------------------------------------------------------
//  value
//------------------------------------------------------------------------
Standard_Character PCollection_HAsciiString::Value (const Standard_Integer Index) const
{
   if (Index < 0 || Index > Length()) Standard_OutOfRange::Raise();
   return Data(Index-1);
}



//------------------------------------------------------------------------
//  Assign
//------------------------------------------------------------------------
void PCollection_HAsciiString::Assign(const DBC_VArrayOfCharacter& TheField)

{
   Data = TheField;
}

//------------------------------------------------------------------------
//  ShallowDump
//------------------------------------------------------------------------
void PCollection_HAsciiString::ShallowDump(Standard_OStream& S) const
{
   S << "begin class HAsciiString " << endl;
   ::ShallowDump(Data, S);
   S << "end class HAsciiString" << endl;
}
