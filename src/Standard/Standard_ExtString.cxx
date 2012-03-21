// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define OptJr 1

#include <Standard_ExtString.hxx>
#include <Standard_Type.hxx> 
#include <Standard_String.hxx>
#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif

//============================================================================
//==== ShallowDump : Writes a ExtString value.
//============================================================================
Standard_EXPORT void ShallowDump (const Standard_ExtString Value, Standard_OStream& s)
{  
   s << hex << Value << " Standard_ExtString " << endl; 
}

const Handle_Standard_Type& Standard_ExtString_Type_() 
{
  static Handle_Standard_Type _aType = 
    new Standard_Type("Standard_ExtString",sizeof(Standard_ExtString),0,NULL);
  
  return _aType;
}

Standard_Integer HashCode (const Standard_ExtString Value, 
			   const Standard_Integer Upper)
{
  Standard_Integer   aHashCode ;
  Standard_Integer   i = 0 ;

#if OptJr
  Standard_ExtString aValue ;

//  cout << "HashCode " << hex << Value << dec << endl ;

  if ((ptrdiff_t(Value) & 3 ) == 2) {
    aHashCode = Value[ 0 ] ;
#if defined(WNT) || defined(DECOSF1) || defined(LININTEL) || defined(__FreeBSD__)
    aHashCode = aHashCode << 16 ;
#endif
    aValue = &Value[1] ;
  }
  else {
    aHashCode = 0 ;
    aValue = Value ;
  }

  while ( ExtStringTestOfZero(((Standard_Integer *) aValue ) [ i ] ) ) {
//       cout << i << " " << hex << aHashCode << " " << ((Standard_Integer *) aValue )[i]
//            << dec << endl ;
       aHashCode = aHashCode ^ ((Standard_Integer *) aValue ) [ i++ ] ;
     }
  while ( aValue[ i << 1 ] != 0 && aValue[( i << 1 ) + 1 ] != 0 ) {
//       cout << i << " " << hex << aHashCode << " " << ((Standard_Integer *) aValue )[i]
//            << dec << endl ;
       aHashCode = aHashCode ^ ((Standard_Integer *) aValue ) [ i++ ] ;
  }
  if ( aValue[ i << 1 ] != 0 ) {
//	cout << i << " " << hex << aHashCode << dec << endl ;
    aHashCode = aHashCode ^ ((Standard_Integer *) aValue ) [ i ] ;
  }
  if ((ptrdiff_t(Value ) & 3) == 2) {
//	cout << hex << aHashCode << dec << endl ;
    aHashCode = (( aHashCode >> 16 ) & 0x0000ffff ) |
                (( aHashCode << 16 ) & 0xffff0000 ) ;
  }
//  cout << i << " " << hex << aHashCode << dec << endl ;

#else  
  char*              charPtr   = (char *)Value;
  Standard_Integer   pos       = 0,
                     count,
                     *tmphash,
                     isend     = 0;
  char               tabchar[20];

  aHashCode = 0 ;
  if (Value != NULL) {
     while(charPtr[i] != 0 || charPtr[i + 1] != 0) {

       for (count = 0,pos = i; count < sizeof(Standard_Integer); count += 2) {

	 if (charPtr[pos + count] == 0 && charPtr[pos + count + 1] == 0 || isend == 1) {
             isend = 1;
             tabchar[count]     = '\0';
	     tabchar[count + 1] = '\0';
         }
         else {
            tabchar[count] = charPtr[pos + count];
	    tabchar[count + 1] = charPtr[pos + count + 1];
         }
        i += 2;
       }
       tmphash = (Standard_Integer *)tabchar;   
       aHashCode = aHashCode ^ *tmphash;
     }
  }
#endif

  aHashCode = HashCode(aHashCode , Upper) ;
  return aHashCode ;
}
