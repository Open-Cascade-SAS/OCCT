// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


// Update JR 12-09-1997 :
//        - three methods of HashCoding of strings : we may keep the value
//          of the hashcode of the string itself. This value is used when
//          resizing of a Map or copying an item from a Map to another Map.
//        - three methods of HashCoding of strings converted to uppercase.

#define _Standard_CString_SourceFile

#define OptJr 1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Standard_CLocaleSentry.hxx>
#include <Standard_CString.hxx>
#include <Standard_Type.hxx>
#include <Standard_OStream.hxx>

#if OptJr
# if defined(WORDS_BIGENDIAN)
static const Standard_Integer static_MaskEndIntegerString[4] = { 0x00000000 ,
                                                                 0xff000000 ,
                                                                 0xffff0000 ,
                                                                 0xffffff00 } ;
# else
static const Standard_Integer static_MaskEndIntegerString[4] = { 0x00000000 ,
                                                                 0x000000ff ,
                                                                 0x0000ffff ,
                                                                 0x00ffffff } ;
# endif
#endif

#include <Standard_String.hxx>
#include <string.h>
#include <stdarg.h>

//============================================================================
//====
//============================================================================
const Handle_Standard_Type& Standard_CString_Type_()
{
  static Handle_Standard_Type _aType =
    new Standard_Type("Standard_CString",sizeof(Standard_CString),0,NULL);

  return _aType;
}

//============================================================================
//==== ShallowDump : Writes a CString value.
//============================================================================
Standard_EXPORT void ShallowDump (const Standard_CString Value, Standard_OStream& s)
{ s << Value << " Standard_CString " << "\n"; }

//============================================================================
//==== HashCode of a CString
//============================================================================
Standard_Integer HashCode (const Standard_CString Value,
                           const Standard_Integer Upper )
{
  Standard_Integer aLen ;
  //#if OptJr
  //STRINGLEN( Value , aLen ) ;
  //#else
  aLen = (Standard_Integer)strlen(Value);
  //#endif
  return HashCode ( HashCodes( Value , aLen ) , Upper ) ;
}

#if OptJr
# if defined(WORDS_BIGENDIAN)
  static Standard_Integer Standard_Mask_String_Left[4] =
                  { 0 , 0x00ffffff , 0x0000ffff , 0x000000ff } ;
  static Standard_Integer Standard_Mask_String_Right[4] =
                  { 0 , 0xff000000 , 0xffff0000 , 0xffffff00 } ;
# else
  static Standard_Integer Standard_Mask_String_Left[4] =
                  { 0 , 0xffffff00 , 0xffff0000 , 0xff000000 } ;
  static Standard_Integer Standard_Mask_String_Right[4] =
                  { 0 , 0x000000ff , 0x0000ffff , 0x00ffffff } ;
# endif
#endif

//============================================================================
//==== HashCode of a CString
//============================================================================
Standard_Integer HashCodes (const Standard_CString Value ,
                            const Standard_Integer Len )
{
 Standard_Integer  aHashCode = 0 ;
 Standard_Integer  i ;
#if !OptJr
 char             *charPtr   = (char *)Value;
 Standard_Integer  pos       = 0,
                   count,
                  *tmphash;
 char              tabchar[20];
#endif

 if (Value != NULL) {

#if !OptJr
   i = 0 ;
   while (i < Len) {
        for (count = 0,pos = i;count < sizeof(Standard_Integer); count++) {
           if (pos + count >= Len)  tabchar[count] = '\0';
           else tabchar[count] = charPtr[pos + count];
           i++;
	 }
        tmphash = (Standard_Integer *)tabchar;
        aHashCode = aHashCode ^ *tmphash;
      }
 }

#else
   Standard_Integer *value = (Standard_Integer *)(ptrdiff_t(Value) & ~0x3) ;
   Standard_Integer len = Len ;

   unsigned int aResidue = (unsigned int)(ptrdiff_t(Value) & 0x3);
   if (aResidue) {
     aHashCode = *value & Standard_Mask_String_Left[aResidue] ;
     value += 1 ;
     len -= (4 - aResidue);
   }

   for ( i = 1 ; i <= len >> 2 ; i++ ) {
      aHashCode = aHashCode ^ value[ i - 1 ] ;
    }
   aHashCode = aHashCode ^ ( value[ i - 1 ] &
               Standard_Mask_String_Right[ len & 3 ] ) ;

   if ( len != Len ) {
# if defined(WORDS_BIGENDIAN)
     aHashCode = aHashCode << 8*aResidue |
                 aHashCode >> 8*(4 - aResidue) ;
# else
     aHashCode = aHashCode << 8*(4 - aResidue) |
                 aHashCode >> 8*aResidue ;
# endif
   }

 }
#endif

 return aHashCode ;
}


# if defined(WORDS_BIGENDIAN)
 static Standard_Integer Standard_Mask_Upper_Lower[5] =
                 { 0 , 0xdf000000 , 0xdfdf0000 , 0xdfdfdf00 , 0xdfdfdfdf } ;
#else
 static Standard_Integer Standard_Mask_Upper_Lower[5] =
                 { 0 , 0xdf , 0xdfdf , 0xdfdfdf , 0xdfdfdfdf } ;
#endif

//============================================================================
//==== HashCode of a CString with discard of bit 5 (UpperCase<-->LowerCase)
//     Efficient for Types and MethodNames (without copy of characters)
//     Valid if we have only alphanumeric characters and "_" (unicity)
//     Valid if the Strings address is aligned for Integers
//============================================================================
Standard_Integer HASHCODES (const Standard_CString Value ,
                            const Standard_Integer Len )
{
 Standard_Integer aHashCode = 0 ;
 Standard_Integer i = 0 ;

 if (Value != NULL) {
#ifdef ALIGNMENT_BUG
   for ( i = 1 ; i <= Len >> 2 ; i++ ) {
      aHashCode = aHashCode ^
                  ( ((Standard_Integer *) Value ) [ i - 1 ] &
                    Standard_Mask_Upper_Lower[4] ) ;
    }
   aHashCode = aHashCode ^
               ( ((Standard_Integer *) Value ) [ i - 1 ] &
                 Standard_Mask_Upper_Lower[ Len & 3 ] ) ;
#else
   Standard_Integer itmp = 0 ;
   for ( i = 0 ; i <= Len-4 ; i+=4 ) {
      memcpy(&itmp,(const void *)&Value[i],4);
      aHashCode=aHashCode^(itmp&Standard_Mask_Upper_Lower[4]);
   }
   if (Len&3) {
      memcpy(&itmp,(const void *)&Value[i],Len&3);
      aHashCode=aHashCode^(itmp&Standard_Mask_Upper_Lower[Len&3]);
   }
#endif

 }
 return aHashCode ;
}

//============================================================================
// IsEqual : Returns Standard_True if two CString have the same value
// Comparison is done with discard of bit 5 (UpperCase<-->LowerCase)
// Efficient for Types and MethodNames (without copy of characters)
// Valid if we have only alphanumeric characters and "_" (unicity)
// Valid if the Strings address are aligned for Integers

//============================================================================
Standard_Boolean ISSIMILAR(const Standard_CString One ,
                           const Standard_Integer LenOne ,
                           const Standard_CString Two )
{
  Standard_Integer i ;

#ifdef ALIGNMENT_BUG
 for ( i = 1 ; i <= LenOne >> 2 ; i++ ) {
    if ( (((Standard_Integer *) One ) [ i - 1 ] &
         Standard_Mask_Upper_Lower[ 4 ] ) !=
         (((Standard_Integer *) Two ) [ i - 1 ] &
         Standard_Mask_Upper_Lower[ 4 ] ) )
      return Standard_False ;
  }
 if ( (((Standard_Integer *) One ) [ i - 1 ] &
      Standard_Mask_Upper_Lower[ LenOne & 3 ] ) !=
      (((Standard_Integer *) Two ) [ i - 1 ] &
      Standard_Mask_Upper_Lower[ LenOne & 3 ] ) )
   return Standard_False  ;
#else
  Standard_Integer  iOne, iTwo ;
  for ( i = 0; i <= LenOne-4; i+=4 ) {
    memcpy(&iOne,(const void *)&One[i],4);
    memcpy(&iTwo,(const void *)&Two[i],4);
    if ((iOne&Standard_Mask_Upper_Lower[4] ) !=
	(iTwo&Standard_Mask_Upper_Lower[4]))
      return Standard_False;
  }
  if(LenOne&3) {
    memcpy(&iOne,(const void *)&One[i],4);
    memcpy(&iTwo,(const void *)&Two[i],4);
    if ( (iOne&Standard_Mask_Upper_Lower[LenOne&3]) !=
	 (iTwo&Standard_Mask_Upper_Lower[LenOne&3]))
      return Standard_False;
  }
#endif
  return Standard_True ;
}

#ifdef __APPLE__
  // There are a lot of *_l functions availalbe on Mac OS X - we use them
  #define SAVE_TL()
#elif defined(_WIN32)
  // MSVCRT has equivalents with slightly different syntax
  #define SAVE_TL()
  #define strtod_l(thePtr, theNextPtr, theLocale)                _strtod_l(thePtr, theNextPtr, theLocale)
  #define vprintf_l(theLocale, theFormat, theArgPtr)             _vprintf_l(theFormat, theLocale, theArgPtr)
  #define vsprintf_l(theBuffer, theLocale, theFormat, theArgPtr) _vsprintf_l(theBuffer, theFormat, theLocale, theArgPtr)
  #define vfprintf_l(theFile,   theLocale, theFormat, theArgPtr) _vfprintf_l(theFile,   theFormat, theLocale, theArgPtr)
#else
  // glibc provides only limited xlocale implementation:
  // strtod_l/strtol_l/strtoll_l functions with explicitly specified locale
  // and newlocale/uselocale/freelocale to switch locale within current thread only.
  // So we switch to C locale temporarily
  #define SAVE_TL() Standard_CLocaleSentry aLocaleSentry;
  #ifndef HAVE_XLOCALE_H
    #error System does not support xlocale. Import/export could be broken if C locale did not specified by application.
    #define strtod_l(thePtr, theNextPtr, theLocale)              strtod(thePtr, theNextPtr)
  #endif
  #define vprintf_l(theLocale, theFormat, theArgPtr)             vprintf(theFormat, theArgPtr)
  #define vsprintf_l(theBuffer, theLocale, theFormat, theArgPtr) vsprintf(theBuffer, theFormat, theArgPtr)
  #define vfprintf_l(theFile,   theLocale, theFormat, theArgPtr) vfprintf(theFile,   theFormat, theArgPtr)
#endif

double Strtod (const char* theStr, char** theNextPtr)
{
  return strtod_l (theStr, theNextPtr, Standard_CLocaleSentry::GetCLocale());
}

double Atof (const char* theStr)
{
  return Strtod (theStr, NULL);
}

int Printf  (const Standard_CString theFormat, ...)
{
  SAVE_TL();
  va_list argp;
  va_start(argp, theFormat);
  int result = vprintf_l (Standard_CLocaleSentry::GetCLocale(), theFormat, argp);
  va_end(argp);
  return result;
}

int Fprintf (FILE* theFile, const char* theFormat, ...)
{
  SAVE_TL();
  va_list argp;
  va_start(argp, theFormat);
  int result = vfprintf_l(theFile, Standard_CLocaleSentry::GetCLocale(), theFormat, argp);
  va_end(argp);
  return result;
}

int Sprintf (char* theBuffer, const char* theFormat, ...)
{
  SAVE_TL();
  va_list argp;
  va_start(argp, theFormat);
  int result = vsprintf_l(theBuffer, Standard_CLocaleSentry::GetCLocale(), theFormat, argp);
  va_end(argp);
  return result;
}
