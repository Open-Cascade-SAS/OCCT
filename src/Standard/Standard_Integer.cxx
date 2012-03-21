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


#include <stdlib.h>
#include <Standard_Integer.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#ifndef _Standard_Stream_HeaderFile
#include <Standard_Stream.hxx>
#endif
#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif

// ------------------------------------------------------------------
// CharToInt : Converts a character in an integer value
// ------------------------------------------------------------------
const Handle_Standard_Type& Standard_Integer_Type_() 
{
  static Handle_Standard_Type _aType = new 
    Standard_Type("Standard_Integer",sizeof(Standard_Integer),0,NULL);
  
  return _aType;
}

Standard_Integer CharToInt(const Standard_Character me) 
{ 
  if (!IsDigit(me)) {
    Standard_ConstructionError::Raise();
  }
    
  Standard_Character S[2];
  S[0] = me;
  S[1] = 0;
  return atoi(S);
}

// ------------------------------------------------------------------
// CharToInt : Converts a string in an integer value
// ------------------------------------------------------------------

Standard_Integer CharToInt(const Standard_CString me) 
{ 
  const Standard_Size Len = strlen(me);
  for (Standard_Size I = 0; I < Len; I++)
    if (!IsDigit(me[I])) {
	Standard_ConstructionError::Raise();
    }
  return atoi(me);
}

// ------------------------------------------------------------------
// ShallowCopy : Copy of an integer
// ------------------------------------------------------------------

Standard_Integer ShallowCopy (const Standard_Integer me) 
{ return me; }

// ------------------------------------------------------------------
// ShallowDump : Writes an integer value
// ------------------------------------------------------------------
Standard_EXPORT void ShallowDump (const Standard_Integer Value, Standard_OStream& s)
{ s << Value << " Standard_Integer" << "\n"; }

// ------------------------------------------------------------------
// NextPrime : Compute the first prime number greater or equal than an integer
// ------------------------------------------------------------------

#define VALUESNBR 4

long NextPrime (const long me ) 
{

 struct svalue {int signiaib ;
                int nbr ;} ;

 struct svalue values[VALUESNBR] ;
 long ia ;
 long maxia ;
 long ib[4] ;
 int n[4] ;
// int signiaib[4] = { -1 , +1 , +1 , -1 } ;
 int signiaib[4];
 signiaib[0] = -1;
 signiaib[1] = 1;
 signiaib[2] = 1;
 signiaib[3] = -1;
 long remain ;

 int nbvalues ;
 int loop ;
 int nindd ;
 long minn ;
 long maxvn ;
 long premret = 0 ;

   if (me < 0 || me >
#if defined (__alpha) || defined(DECOSF1)
                      127149130704178201
#else
                      2147483647
#endif
                               ){
      Standard_RangeError::
       Raise("Try to apply NextPrime method with negative, null or too large value.");
   }

   if ( me <= 7 ) {
     if ( me <= 1 )
       return 1 ;
     else if ( me <= 2 )
       return 2 ;
     else if ( me <= 3 )
       return 3 ;
     else if ( me <= 5 )
       return 5 ;
     else if ( me <= 7 )
       return 7 ;
   }

   minn = ( me - 1 ) / 6 ;              // n minimum
   while ( 6*minn+1 < me ) {
        minn += 1 ;
      }

   maxia = long( sqrt((double ) me ) / 6 + 1 ) ;

   maxvn = minn + VALUESNBR ;

   nbvalues = 0 ;
   for ( nindd = 0 ; nindd < VALUESNBR ; nindd++ ) {
      if ( 6*(nindd+minn)-1 < me ) {
        values[nindd].nbr = 1 ;
        values[nindd].signiaib = -1 ;
        nbvalues += 1 ;
      }
      else {
        values[nindd].nbr = 0 ;
        values[nindd].signiaib = 0 ;
      }
    }

   for ( ia = 1 ; ia <= maxia ; ia++ ) {
      if ( nbvalues == VALUESNBR*2 ) {
        break ;
      }
      remain = -VALUESNBR ;
      ib[0] = ( minn + ia - remain ) / (6*ia - 1) ;
      n[0] = int ( 6*ia*ib[0] - ia - ib[0] - minn ) ;
      ib[1] = ( minn - ia - remain ) / (6*ia - 1) ;
      n[1] = int ( 6*ia*ib[1] + ia - ib[1] - minn ) ;
      ib[2] = ( minn + ia - remain ) / (6*ia + 1) ;
      n[2] = int ( 6*ia*ib[2] - ia + ib[2] - minn ) ;
      ib[3] = ( minn - ia - remain ) / (6*ia + 1) ;
      n[3] = int ( 6*ia*ib[3] + ia + ib[3] - minn ) ;
      for ( loop = 0 ; loop < 4 ; loop++ ) {
         if ( n[loop] >= 0 && n[loop] < VALUESNBR ) {
           if ( ( values[n[loop]].nbr == 0 ) ||
                ( values[n[loop]].signiaib == signiaib[loop] ) ) {
             values[n[loop]].signiaib = -signiaib[loop] ;
             values[n[loop]].nbr += 1 ;
             if ( values[n[loop]].nbr <= 2 )
               nbvalues += 1 ;
	   }
	 }
       }
    }
   for ( nindd = 0 ; nindd < VALUESNBR ; nindd++ ) {
     if ( values[nindd].nbr == 0 ) {
       if ( me <= 6*(nindd+minn)-1 ) {
          premret = 6*(nindd+minn)-1 ;
          break ;
	}
       else if ( me <= 6*(nindd+minn)+1 ) {
         premret = 6*(nindd+minn)+1 ;
         break ;
       }
     }
     else if ( values[nindd].nbr == 1 ) {
        if ( values[nindd].signiaib > 0 ) {
          if ( me <= 6*(nindd+minn)-1 ) {
            premret = 6*(nindd+minn)-1 ;
            break ;
	  }
	}
        else {
          if ( me <= 6*(nindd+minn)+1 ) {
            premret = 6*(nindd+minn)+1 ;
            break ;
	  }
	}
      }
   }

   if ( premret != 0 ) {
     return premret ;
   }

 return NextPrime ( 6*(maxvn-1)+2) ;

}
static const Standard_Integer Primes[] = {
    101,  1009,  2003,  3001,  4001,  5003,  6007,  7001,  8009,  9001,
  10007,        12007,        14009,        16007,        18013, 
  20011,               23003,               26003,               29009,
                       33013,                      37003,              
         41011,                             46021,                     
         51001,                                    57037,              
                                     65003, 
  100019};  // catch the biggest

const Standard_Integer NbPrimes = 26; // does not include the biggest

Standard_Integer NextPrimeForMap(const Standard_Integer N)
{
  Standard_Integer i;
  for (i = 0; i < NbPrimes; i++) 
    if (Primes[i] > N) break;
  return Primes[i];
}
