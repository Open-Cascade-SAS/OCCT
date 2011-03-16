
#ifndef _Standard_String_HeaderFile
# define _Standard_String_HeaderFile

# ifndef _Standard_TypeDef_HeaderFile
#  include <Standard_TypeDef.hxx>
# endif

#define INF(X,Y) (((X)<(Y))?(X):(Y))

# if OptJr

#define STRLEN(s,i) {(i) = 0;while((s)[(i)++] != '\0');(i)--;}
#define EXTSTRLEN(s,i) {(i) = 0;while((s)[(i)++] != 0);(i)--;}
#define STRCPY(s1,s2,i) {for(LoopIndex=0; LoopIndex<(i); LoopIndex++)(s1)[LoopIndex] = (s2)[LoopIndex];}
#define STRCAT(s1,i,s2,j) {for(LoopIndex=0; LoopIndex<(j); LoopIndex++) (s1)[(i)+LoopIndex] = (s2)[LoopIndex];}

//#   ifdef __Standard_DLL
#   ifdef _Standard_CString_SourceFile
__Standard_API const Standard_Integer *MaskEndIntegerString = static_MaskEndIntegerString ;
#   else
__Standard_APIEXTERN const Standard_Integer *MaskEndIntegerString ;
#   endif
//#   else
//Standard_IMPORT const Standard_Integer *MaskEndIntegerString ;
//#   endif

// JR 16-jul-1998
// Algorithme utilise pour le calcul des longueurs de strings
// Je suppose que les octets sont de l'ascii 7 bits.
// Si un des octets d'un mot est a zero et si on soustrait 0x01010101 a ce
//    mot, l'octet a zero change de signe.
// Reciproquement si (( Word - 0x01010101 ) & 0x80808080 ) != 0
//    alors Word a un octet a zero.

// Si on a des octets negatifs et si on applique le resultat ci-dessus a
// ( Word & 0x7f7f7f7f ), cela sera vrai sauf si un des octets vaut 0x80
// auquel cas on trouvera un octet a zero a tort.

// Conclusion : il suffit de controler la presence d'un octet a zero a partir
//              du debut du mot ou l'on croira en avoir trouve un pour traiter
//              correctement les octets qui valent 0x80.

// La meme chose est vraie pour les extendedstrings.

// D'autre part afin d'accelerer les traitements, on teste si les chaines
// sont alignees sur des mots de 32 bits ( AND de l'adresse avec 3 ) ou
// sont alignees sur des shorts de 16 bits ( AND de l'adresse avec 1 ).

inline Standard_Boolean CStringTestOfZero(const Standard_Integer aString )
{
 return ( ((( aString & 0x7f7f7f7f ) - \
            0x01010101 ) & 0x80808080 ) == 0 ) ;
}

inline Standard_Boolean HalfCStringTestOfZero(const Standard_ExtCharacter anExtCharacter )
{
 return ( ((( anExtCharacter & 0x7f7f ) - 0x0101 ) & 0x8080 ) == 0 ) ;
}

inline Standard_Boolean ExtStringTestOfZero(const Standard_Integer anExtString )
{
 return ( ((( anExtString & 0x7fff7fff ) - \
            0x00010001 ) & 0x80008000 ) == 0 ) ;
}

#define STRINGLEN( aString , LoopIndex ) { \
 if ((ptrdiff_t(aString) & 1) == 0) { \
   LoopIndex = 0 ; \
   if ((ptrdiff_t(aString) & 3) == 0) { \
     while (CStringTestOfZero(((Standard_Integer *)aString)[LoopIndex++])); \
     LoopIndex = ( LoopIndex << 2 ) - 4 ; \
   } \
   else { \
     while (HalfCStringTestOfZero(((Standard_ExtCharacter *)aString)[LoopIndex++])); \
     LoopIndex = ( LoopIndex << 1 ) - 2 ; \
   } \
   while (aString[LoopIndex++] != '\0'); \
   LoopIndex -= 1 ; \
 } \
 else \
   STRLEN( aString , LoopIndex ) ; \
}

#define EXTSTRINGLEN( anExtString , LoopIndex ) { \
 if ((ptrdiff_t(anExtString) & 3) == 0) { \
   LoopIndex = 0 ; \
   while (ExtStringTestOfZero(((Standard_Integer *)anExtString)[LoopIndex++]));\
   LoopIndex = ( LoopIndex << 1 ) - 2 ; \
   if ( anExtString[ LoopIndex ] != 0 ) \
     LoopIndex += 1 ; \
 } \
 else \
   EXTSTRLEN( anExtString , LoopIndex ) ; \
}

// aStringOut is an AsciiString and is word aligned
// aStringIn is a CString and may be not word aligned
#define CSTRINGCOPY( aStringOut , aStringIn , aStringLen ) { \
 Standard_Integer LoopIndex = (Standard_Integer)(ptrdiff_t(aStringIn) & 3) ; \
 if ( ( LoopIndex & 1 ) == 0 ) { \
   if ( LoopIndex == 0 ) { \
     for (; LoopIndex <= ( aStringLen >> 2 ) ; LoopIndex++ ) { \
        ((Standard_Integer *) aStringOut )[ LoopIndex ] = \
                   ((Standard_Integer *) aStringIn )[ LoopIndex ] ; \
        } \
   } \
   else { \
     for ( LoopIndex = 0 ; LoopIndex <= ( aStringLen >> 1) ; LoopIndex++ ) { \
        ((Standard_ExtCharacter *) aStringOut )[ LoopIndex ] = \
                      ((Standard_ExtCharacter *) aStringIn )[ LoopIndex ] ; \
        } \
   } \
 } \
 else \
   STRCPY( aStringOut , aStringIn , aStringLen + 1 ) ; \
}

// aStringOut is an AsciiString and is word aligned
// aStringIn is an AsciiString and is word aligned
#define ASCIISTRINGCOPY( aStringOut , aStringIn , aStringLen ) { \
 Standard_Integer LoopIndex ; \
 LoopIndex = 0 ; \
 for(; LoopIndex <= ( aStringLen >> 2 ) ; LoopIndex++ ) { \
    ((Standard_Integer *) aStringOut )[ LoopIndex ] = \
                    ((Standard_Integer *) aStringIn )[ LoopIndex ] ; \
    } \
}

#define STRINGCAT( aStringOut , aStringOutLen , aStringIn , aStringInLen ) { \
 Standard_Integer LoopIndex ; \
 if ((ptrdiff_t(&aStringOut[ aStringOutLen ]) & 1) == 0 && \
     (ptrdiff_t(aStringIn) & 1 ) == 0 ) { \
   LoopIndex = 0 ; \
   if ((ptrdiff_t(&aStringOut[ aStringOutLen ]) & 3 ) == 0 && \
       (ptrdiff_t(aStringIn) & 3 ) == 0 ) { \
     for (; LoopIndex <= ( aStringInLen >> 2 ) ; LoopIndex++ ) { \
        ((Standard_Integer *) aStringOut )[ ( aStringOutLen >> 2 ) + \
         LoopIndex ] = ((Standard_Integer *) aStringIn )[ LoopIndex ] ; \
        } \
   } \
   else { \
     for (; LoopIndex <= ( aStringInLen >> 1 ) ; LoopIndex++ ) { \
        ((Standard_ExtCharacter *) aStringOut )[ ( aStringOutLen >> 1 ) + \
         LoopIndex ] = ((Standard_ExtCharacter *) aStringIn )[ LoopIndex ] ; \
        } \
   } \
 } \
 else \
   STRCPY( &aStringOut[ aStringOutLen ] , aStringIn , aStringInLen + 1 ) ; \
}

// aString is an AsciiString and is word aligned
// anOtherString is aCString and may be not word aligned
// aStringLen is the length of aString and anOtherString
#define CSTRINGEQUAL( aString , anOtherString , aStringLen , KEqual ) { \
 KEqual = Standard_True ; \
 Standard_Integer LoopIndex = Standard_Integer(ptrdiff_t(anOtherString) & 3); \
 if ( ( LoopIndex & 1 ) == 0 ) { \
   if ( LoopIndex == 0 ) { \
     for (; LoopIndex < ( aStringLen >> 2 ) ; LoopIndex++ ) { \
        if (((Standard_Integer *) aString )[ LoopIndex ] != \
            ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
          KEqual = Standard_False ; \
          break ; \
          } \
        } \
     if ( KEqual == Standard_True && \
          (((Standard_Integer *) aString )[ LoopIndex ] & \
           MaskEndIntegerString[ aStringLen & 3 ]) != \
          (((Standard_Integer *) anOtherString )[ LoopIndex ] & \
           MaskEndIntegerString[ aStringLen & 3 ] ) ) \
       KEqual = Standard_False ; \
   } \
   else { \
     for ( LoopIndex = 0 ; LoopIndex < (( aStringLen + 1) >> 1) ; LoopIndex++ ) { \
        if (((Standard_ExtCharacter *) aString )[ LoopIndex ] != \
            ((Standard_ExtCharacter *) anOtherString )[ LoopIndex ] ) { \
          KEqual = Standard_False ; \
          break ; \
          } \
        } \
   } \
 } \
 else { \
   for ( LoopIndex = 0 ; LoopIndex < aStringLen ; LoopIndex++ ) { \
      if ( aString [ LoopIndex ] != anOtherString [ LoopIndex ] ) { \
        KEqual = Standard_False ; \
        break ; \
      } \
   } \
 } \
}

// aString is an AsciiString and is word aligned
// anOtherString is aCString and may be not word aligned
// aStringLen is the length of aString.
// The length of anOtherString is unknown
#define LCSTRINGEQUAL( aString , aStringLen , anOtherString , KEqual ) { \
 KEqual = Standard_True ; \
 Standard_Integer LoopIndex = Standard_Integer(ptrdiff_t(anOtherString) & 3); \
 if ( ( LoopIndex & 1 ) == 0 ) { \
   if ( LoopIndex == 0 ) { \
     for (; LoopIndex < ( aStringLen >> 2 ) ; LoopIndex++ ) { \
        if (((Standard_Integer *) aString )[ LoopIndex ] != \
            ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
          KEqual = Standard_False ; \
          break ; \
        } \
     } \
     LoopIndex = ( LoopIndex << 2) ; \
   } \
   else { \
     for ( LoopIndex = 0 ; LoopIndex < (( aStringLen + 1) >> 1) ; LoopIndex++ ) { \
        if (((Standard_ExtCharacter *) aString )[ LoopIndex ] != \
            ((Standard_ExtCharacter *) anOtherString )[ LoopIndex ] ) { \
          KEqual = Standard_False ; \
          break ; \
        } \
     } \
     LoopIndex = ( LoopIndex << 1) ; \
   } \
   if ( KEqual ) { \
     for (; LoopIndex <= aStringLen ; LoopIndex++ ) { \
          if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) { \
            KEqual = Standard_False ; \
            break ; \
          } \
     } \
   } \
 } \
 else { \
   for ( LoopIndex = 0 ; LoopIndex <= aStringLen ; LoopIndex++ ) { \
      if ( aString [ LoopIndex ] != anOtherString [ LoopIndex ] ) { \
        KEqual = Standard_False ; \
        break ; \
      } \
   } \
 } \
}

// aString is an AsciiString and is word aligned
// anOtherString is an AsciiString and is word aligned
#define ASCIISTRINGEQUAL( aString , anOtherString , aStringLen , KEqual ) { \
 Standard_Integer LoopIndex ; \
 KEqual = Standard_True ; \
 LoopIndex = 0 ; \
 for(; LoopIndex < ( aStringLen >> 2 ) ; LoopIndex++ ) { \
    if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
         ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
      KEqual = Standard_False ; \
      break ; \
      } \
    } \
 if ( KEqual == Standard_True && \
      (((Standard_Integer *) aString )[ LoopIndex ] & \
       MaskEndIntegerString[ aStringLen & 3 ]) != \
      (((Standard_Integer *) anOtherString )[ LoopIndex ] & \
       MaskEndIntegerString[ aStringLen & 3 ] ) ) \
   KEqual = Standard_False ; \
}

// aString is an AsciiString and is word aligned
// anOtherString is aCString and may be not word aligned
#define CSTRINGLESS( aString , aStringLen , anOtherString , \
                     anOtherStringLen , MinLen , KLess ) { \
 Standard_Integer LoopIndex ; \
 KLess = Standard_True ; \
 LoopIndex = ptrdiff_t(anOtherString) & 3 ; \
 if ( ( LoopIndex & 1 ) == 0 && MinLen > 3 ) { \
   if ( LoopIndex == 0 ) { \
     for (; LoopIndex < ( MinLen >> 2 ) ; LoopIndex++ ) { \
        if (((Standard_Integer *) aString )[ LoopIndex ] != \
            ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 2 ; \
   } \
   else { \
     for ( LoopIndex = 0 ; LoopIndex < ( MinLen >> 1 ) ; LoopIndex++ ) { \
        if (((Standard_ExtCharacter *) aString )[ LoopIndex ] != \
            ((Standard_ExtCharacter *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 1 ; \
   } \
 } \
 else \
   LoopIndex = 0 ; \
 for (; LoopIndex < MinLen ; LoopIndex++ ) { \
    if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
      break ; \
    } \
 if ( LoopIndex != MinLen ) { \
   if ( aString[ LoopIndex ] > anOtherString[ LoopIndex ] ) \
     KLess = Standard_False ; \
 } \
 else if ( aStringLen >= anOtherStringLen ) \
   KLess = Standard_False ; \
}

// aString is an AsciiString and is word aligned
// anOtherString is aCString and may be not word aligned
// aStringLen is the length of aString
// The length of anOtherString is unknown
#define LCSTRINGLESS( aString , aStringLen , anOtherString , KLess ) { \
 KLess = Standard_True ; \
 Standard_Integer LoopIndex = Standard_Integer(ptrdiff_t(anOtherString) & 3); \
 if ( ( LoopIndex & 1 ) == 0 && aStringLen > 3 ) { \
   if ( LoopIndex == 0 ) { \
     for (; LoopIndex < ( aStringLen >> 2 ) ; LoopIndex++ ) { \
        if (((Standard_Integer *) aString )[ LoopIndex ] != \
            ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 2 ; \
   } \
   else { \
     for ( LoopIndex = 0 ; LoopIndex < ( aStringLen >> 1 ) ; LoopIndex++ ) { \
        if (((Standard_ExtCharacter *) aString )[ LoopIndex ] != \
            ((Standard_ExtCharacter *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 1 ; \
   } \
 } \
 else \
   LoopIndex = 0 ; \
 for (; LoopIndex < aStringLen ; LoopIndex++ ) { \
    if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
      break ; \
    } \
 if ( LoopIndex != aStringLen ) { \
   if ( aString[ LoopIndex ] > anOtherString[ LoopIndex ] ) \
     KLess = Standard_False ; \
 } \
 else if ( anOtherString[ LoopIndex ] == '\0' ) \
   KLess = Standard_False ; \
}

// aString is an AsciiString and is word aligned
// anOtherString is an AsciiString and is word aligned
#define ASCIISTRINGLESS( aString , aStringLen , anOtherString , \
                         anOtherStringLen , MinLen , KLess ) { \
 Standard_Integer LoopIndex ; \
 KLess = Standard_True ; \
 LoopIndex = 0 ; \
 if ( MinLen > 3 ) { \
   for(; LoopIndex < ( aStringLen >> 2 ) ; LoopIndex++ ) { \
      if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
           ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
        LoopIndex++ ; \
        break ; \
        } \
      } \
   LoopIndex = ( LoopIndex - 1 ) << 2 ; \
 } \
 for (; LoopIndex < MinLen ; LoopIndex++ ) { \
    if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
      break ; \
    } \
 if ( LoopIndex != MinLen ) { \
   if ( aString[ LoopIndex ] > anOtherString[ LoopIndex ] ) \
     KLess = Standard_False ; \
 } \
 else if ( aStringLen >= anOtherStringLen ) \
   KLess = Standard_False ; \
}

// aString is an AsciiString and is word aligned
// anOtherString is aCString and may be not word aligned
#define CSTRINGGREATER( aString , aStringLen , anOtherString , \
                        anOtherStringLen , MinLen , KGreater ) { \
 KGreater = Standard_True ; \
 Standard_Integer LoopIndex = Standard_Integer(ptrdiff_t(anOtherString) & 3); \
 if ( ( LoopIndex & 1 ) == 0 && MinLen > 3 ) { \
   if ( LoopIndex == 0 ) { \
     for (; LoopIndex < ( MinLen >> 2 ) ; LoopIndex++ ) { \
        if (((Standard_Integer *) aString )[ LoopIndex ] != \
            ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 2 ; \
   } \
   else { \
     for ( LoopIndex = 0 ; LoopIndex < ( MinLen >> 1 ) ; LoopIndex++ ) { \
        if (((Standard_ExtCharacter *) aString )[ LoopIndex ] != \
            ((Standard_ExtCharacter *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 1 ; \
   } \
 } \
 else \
   LoopIndex = 0 ; \
 for (; LoopIndex < MinLen ; LoopIndex++ ) { \
    if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
      break ; \
    } \
 if ( LoopIndex != MinLen ) { \
   if ( aString[ LoopIndex ] < anOtherString[ LoopIndex ] ) \
     KGreater = Standard_False ; \
 } \
 else if ( aStringLen <= anOtherStringLen ) \
   KGreater = Standard_False ; \
}

// aString is an AsciiString and is word aligned
// anOtherString is aCString and may be not word aligned
// aStringLen is the length of aString
// The length of anOtherString is unknown
#define LCSTRINGGREATER( aString , aStringLen , anOtherString , KGreater ) { \
 Standard_Integer LoopIndex ; \
 KGreater = Standard_True ; \
 LoopIndex = (Standard_Integer)(ptrdiff_t(anOtherString) & 3) ; \
 if ( ( LoopIndex & 1 ) == 0 && aStringLen > 3 ) { \
   if ( LoopIndex == 0 ) { \
     for (; LoopIndex < ( aStringLen >> 2 ) ; LoopIndex++ ) { \
        if (((Standard_Integer *) aString )[ LoopIndex ] != \
            ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 2 ; \
   } \
   else { \
     for ( LoopIndex = 0 ; LoopIndex < ( aStringLen >> 1 ) ; LoopIndex++ ) { \
        if (((Standard_ExtCharacter *) aString )[ LoopIndex ] != \
            ((Standard_ExtCharacter *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 1 ; \
   } \
 } \
 else \
   LoopIndex = 0 ; \
 for (; LoopIndex < aStringLen ; LoopIndex++ ) { \
    if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
      break ; \
    } \
 if ( LoopIndex != aStringLen ) { \
   if ( aString[ LoopIndex ] < anOtherString[ LoopIndex ] ) \
     KGreater = Standard_False ; \
 } \
 else \
   KGreater = Standard_False ; \
}

// aString is an AsciiString and is word aligned
// anOtherString is an AsciiString and is word aligned
#define ASCIISTRINGGREATER( aString , aStringLen , anOtherString , \
                            anOtherStringLen , MinLen , KGreater ) { \
 Standard_Integer LoopIndex ; \
 KGreater = Standard_True ; \
 LoopIndex = 0 ; \
 if ( MinLen > 3 ) { \
   for(; LoopIndex < ( aStringLen >> 2 ) ; LoopIndex++ ) { \
      if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
           ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
        LoopIndex++ ; \
        break ; \
        } \
      } \
   LoopIndex = ( LoopIndex - 1 ) << 2 ; \
 } \
 for (; LoopIndex < MinLen ; LoopIndex++ ) { \
    if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
      break ; \
    } \
 if ( LoopIndex != MinLen ) { \
   if ( aString[ LoopIndex ] < anOtherString[ LoopIndex ] ) \
     KGreater = Standard_False ; \
 } \
 else if ( aStringLen <= anOtherStringLen ) \
   KGreater = Standard_False ; \
}

// anExtStringOut is an ExtendedString and is word aligned
// anExtStringIn is an ExtString and may be not word aligned
#define EXTSTRINGCOPY( anExtStringOut , anExtStringIn , anExtStringLen ) { \
 Standard_Integer LoopIndex ; \
 LoopIndex = (Standard_Integer)(ptrdiff_t(anExtStringIn) & 3) ; \
 if ( LoopIndex == 0 ) { \
   for (; LoopIndex <= ( anExtStringLen >> 1 ) ; LoopIndex++ ) { \
      ((Standard_Integer *) anExtStringOut )[ LoopIndex ] = \
                   ((Standard_Integer *) anExtStringIn )[ LoopIndex ] ; \
      } \
 } \
 else \
   STRCPY( anExtStringOut , anExtStringIn , anExtStringLen + 1 ) ; \
}

// anExtStringOut is an ExtendedString and is word aligned
// anExtStringIn is an ExtendedString and is word aligned
// We copy the ending zero and possibly the ExtCharacter folowing
#define EXTENDEDSTRINGCOPY( anExtStringOut , anExtStringIn , anExtStringLen ) {\
 Standard_Integer LoopIndex ; \
 for (LoopIndex = 0 ; LoopIndex <= ( anExtStringLen >> 1 ) ; LoopIndex++ ) { \
    ((Standard_Integer *) anExtStringOut )[ LoopIndex ] = \
                ((Standard_Integer *) anExtStringIn )[ LoopIndex ] ; \
 } \
}

// anExtStringOut is an ExtendedString and is word aligned
// anExtStringIn is an ExtendedString and is word aligned
// We copy the ending zero and possibly the ExtCharacter folowing
#define EXTENDEDSTRINGCAT( anExtStringOut , anExtStringOutLen , anExtStringIn , anExtStringInLen ) {\
 Standard_Integer LoopIndex ; \
 if ( ( anExtStringOutLen & 1 ) == 0 ) { \
   for (LoopIndex = 0 ; LoopIndex <= ( anExtStringInLen >> 1 ) ; LoopIndex++ ) { \
      ((Standard_Integer *) anExtStringOut )[ ( anExtStringOutLen >> 1 ) + LoopIndex ] = \
                  ((Standard_Integer *) anExtStringIn )[ LoopIndex ] ; \
      } \
 } \
 else { \
   STRCAT( anExtStringOut , anExtStringOutLen , anExtStringIn , anExtStringInLen + 1 ) ; \
 } \
}

// aString is an ExtendedString and is word aligned
// anOtherString is an ExtString and may be not word aligned
// We compare the last two ExtCharacters or the last ExtCharacter and the
// ending zero if it's word aligned
// aStringLen is the length of aString and anOtherString
#define EXTSTRINGEQUAL( aString , anOtherString , aStringLen , KEqual ) { \
 Standard_Integer LoopIndex ; \
 KEqual = Standard_True ; \
 LoopIndex = 0 ; \
 if ((ptrdiff_t(anOtherString) & 3 ) == 0 ) { \
   for(; LoopIndex < (( aStringLen + 1) >> 1 ) ; LoopIndex++ ) { \
      if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
           ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
        KEqual = Standard_False ; \
        break ; \
        } \
      } \
 } \
 else { \
   for(; LoopIndex < aStringLen ; LoopIndex++ ) { \
      if ( aString [ LoopIndex ] != anOtherString [ LoopIndex ] ) { \
        KEqual = Standard_False ; \
        break ; \
        } \
      } \
 } \
}

// aString is an ExtendedString and is word aligned
// anOtherString is an ExtString and may be not word aligned
// We compare the last two ExtCharacters or the last ExtCharacter and the
// ending zero if it's word aligned
// aStringLen is the length of aString.
// The length of anOtherString is unknown
#define LEXTSTRINGEQUAL( aString , aStringLen , anOtherString , KEqual ) { \
 Standard_Integer LoopIndex ; \
 KEqual = Standard_True ; \
 LoopIndex = 0 ; \
 if ((ptrdiff_t(anOtherString) & 3 ) == 0 ) { \
   for(; LoopIndex < (( aStringLen + 1) >> 1 ) ; LoopIndex++ ) { \
      if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
           ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
        KEqual = Standard_False ; \
        break ; \
      } \
   } \
   if ( KEqual && aString [ aStringLen ] != anOtherString [ aStringLen ] ) \
     KEqual = Standard_False ; \
 } \
 else { \
   for(; LoopIndex <= aStringLen ; LoopIndex++ ) { \
      if ( aString [ LoopIndex ] != anOtherString [ LoopIndex ] ) { \
        KEqual = Standard_False ; \
        break ; \
      } \
   } \
 } \
}

// aString is an ExtendedString and is word aligned
// anOtherString is an ExtendedString and is word aligned
// We compare the last two ExtCharacters or the last ExtCharacter and the
// ending zero
#define EXTENDEDSTRINGEQUAL( aString , anOtherString , aStringLen , KEqual ) { \
 Standard_Integer LoopIndex ; \
 KEqual = Standard_True ; \
 LoopIndex = 0 ; \
 for(; LoopIndex < (( aStringLen + 1) >> 1 ) ; LoopIndex++ ) { \
    if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
         ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
      KEqual = Standard_False ; \
      break ; \
      } \
    } \
}

// aString is an ExtendedString and is word aligned
// anOtherString is an ExtendedString and may be not word aligned
#define EXTSTRINGLESS( aString , aStringLen , anOtherString , \
                       anOtherStringLen , MinLen , KLess ) { \
 Standard_Integer LoopIndex ; \
 KLess = Standard_True ; \
 LoopIndex = 0 ; \
 if ((ptrdiff_t(anOtherString) & 3 ) == 0 ) { \
   if ( MinLen > 1 ) { \
     for(; LoopIndex < ( aStringLen >> 1 ) ; LoopIndex++ ) { \
        if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
             ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 1 ; \
   } \
   for (; LoopIndex < MinLen ; LoopIndex++ ) { \
      if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
        break ; \
      } \
   if ( LoopIndex != MinLen ) { \
     if ( aString[ LoopIndex ] > anOtherString[ LoopIndex ] ) \
       KLess = Standard_False ; \
   } \
   else if ( aStringLen >= anOtherStringLen ) \
     KLess = Standard_False ; \
 } \
 else { \
   if ( MinLen > 1 ) { \
     for(; LoopIndex < aStringLen ; LoopIndex++ ) { \
        if ( aString [ LoopIndex ] != anOtherString [ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = LoopIndex - 1 ; \
   } \
   for (; LoopIndex < MinLen ; LoopIndex++ ) { \
      if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
        break ; \
      } \
   if ( LoopIndex != MinLen ) { \
     if ( aString[ LoopIndex ] > anOtherString[ LoopIndex ] ) \
       KLess = Standard_False ; \
   } \
   else if ( aStringLen >= anOtherStringLen ) \
     KLess = Standard_False ; \
 } \
}

// aString is an ExtendedString and is word aligned
// anOtherString is an ExtendedString and may be not word aligned
// aStringLen is the length of aString
// The length of anOtherString is unknown
#define LEXTSTRINGLESS( aString , aStringLen , anOtherString , KLess ) { \
 Standard_Integer LoopIndex ; \
 KLess = Standard_True ; \
 LoopIndex = 0 ; \
 if ((ptrdiff_t(anOtherString) & 3 ) == 0 ) { \
   if ( aStringLen > 1 ) { \
     for(; LoopIndex < ( aStringLen >> 1 ) ; LoopIndex++ ) { \
        if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
             ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 1 ; \
   } \
   for (; LoopIndex < aStringLen ; LoopIndex++ ) { \
      if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
        break ; \
      } \
   if ( LoopIndex != aStringLen ) { \
     if ( aString[ LoopIndex ] > anOtherString[ LoopIndex ] ) \
       KLess = Standard_False ; \
   } \
   else if ( anOtherString[ LoopIndex ] == 0 ) \
     KLess = Standard_False ; \
 } \
 else { \
   if ( aStringLen > 1 ) { \
     for(; LoopIndex < aStringLen ; LoopIndex++ ) { \
        if ( aString [ LoopIndex ] != anOtherString [ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = LoopIndex - 1 ; \
   } \
   for (; LoopIndex < aStringLen ; LoopIndex++ ) { \
      if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
        break ; \
      } \
   if ( LoopIndex != aStringLen ) { \
     if ( aString[ LoopIndex ] > anOtherString[ LoopIndex ] ) \
       KLess = Standard_False ; \
   } \
   else if ( anOtherString[ LoopIndex ] == 0 ) \
     KLess = Standard_False ; \
 } \
}

// aString is an ExtendedString and is word aligned
// anOtherString is an ExtendedString and is word aligned
#define EXTENDEDSTRINGLESS( aString , aStringLen , anOtherString , \
                            anOtherStringLen , MinLen , KLess ) { \
 Standard_Integer LoopIndex ; \
 KLess = Standard_True ; \
 LoopIndex = 0 ; \
 if ( MinLen > 1 ) { \
   for(; LoopIndex < ( aStringLen >> 1 ) ; LoopIndex++ ) { \
      if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
           ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
        LoopIndex++ ; \
        break ; \
        } \
      } \
   LoopIndex = ( LoopIndex - 1 ) << 1 ; \
 } \
 for (; LoopIndex < MinLen ; LoopIndex++ ) { \
    if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
      break ; \
    } \
 if ( LoopIndex != MinLen ) { \
   if ( aString[ LoopIndex ] > anOtherString[ LoopIndex ] ) \
     KLess = Standard_False ; \
 } \
 else if ( aStringLen >= anOtherStringLen ) \
   KLess = Standard_False ; \
}

// aString is an ExtendedString and is word aligned
// anOtherString is an ExtendedString and may be not word aligned
#define EXTSTRINGGREATER( aString , aStringLen , anOtherString , \
                          anOtherStringLen , MinLen , KGreater ) { \
 Standard_Integer LoopIndex ; \
 KGreater = Standard_True ; \
 LoopIndex = 0 ; \
 if ((ptrdiff_t(anOtherString) & 3 ) == 0 ) { \
   if ( MinLen > 1 ) { \
     for(; LoopIndex < ( aStringLen >> 1 ) ; LoopIndex++ ) { \
        if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
             ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 1 ; \
   } \
   for (; LoopIndex < MinLen ; LoopIndex++ ) { \
      if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
        break ; \
      } \
   if ( LoopIndex != MinLen ) { \
     if ( aString[ LoopIndex ] < anOtherString[ LoopIndex ] ) \
       KGreater = Standard_False ; \
   } \
   else if ( aStringLen <= anOtherStringLen ) \
     KGreater = Standard_False ; \
 } \
 else { \
   if ( MinLen > 1 ) { \
     for(; LoopIndex < aStringLen ; LoopIndex++ ) { \
        if ( aString [ LoopIndex ] !=  anOtherString [ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = LoopIndex - 1 ; \
   } \
   for (; LoopIndex < MinLen ; LoopIndex++ ) { \
      if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
        break ; \
      } \
   if ( LoopIndex != MinLen ) { \
     if ( aString[ LoopIndex ] < anOtherString[ LoopIndex ] ) \
       KGreater = Standard_False ; \
   } \
   else if ( aStringLen <= anOtherStringLen ) \
     KGreater = Standard_False ; \
 } \
}

// aString is an ExtendedString and is word aligned
// anOtherString is an ExtendedString and may be not word aligned
// aStringLen is the length of aString
// The length of anOtherString is unknown
#define LEXTSTRINGGREATER( aString , aStringLen , anOtherString , KGreater ) { \
 Standard_Integer LoopIndex ; \
 KGreater = Standard_True ; \
 LoopIndex = 0 ; \
 if ((ptrdiff_t(anOtherString) & 3 ) == 0 ) { \
   if ( aStringLen > 1 ) { \
     for(; LoopIndex < ( aStringLen >> 1 ) ; LoopIndex++ ) { \
        if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
             ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = ( LoopIndex - 1 ) << 1 ; \
   } \
   for (; LoopIndex < aStringLen ; LoopIndex++ ) { \
      if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
        break ; \
      } \
   if ( LoopIndex != aStringLen ) { \
     if ( aString[ LoopIndex ] < anOtherString[ LoopIndex ] ) \
       KGreater = Standard_False ; \
   } \
   else \
     KGreater = Standard_False ; \
 } \
 else { \
   if ( aStringLen > 1 ) { \
     for(; LoopIndex < aStringLen ; LoopIndex++ ) { \
        if ( aString [ LoopIndex ] !=  anOtherString [ LoopIndex ] ) { \
          LoopIndex++ ; \
          break ; \
          } \
        } \
     LoopIndex = LoopIndex - 1 ; \
   } \
   for (; LoopIndex < aStringLen ; LoopIndex++ ) { \
      if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
        break ; \
      } \
   if ( LoopIndex != aStringLen ) { \
     if ( aString[ LoopIndex ] < anOtherString[ LoopIndex ] ) \
       KGreater = Standard_False ; \
   } \
   else \
     KGreater = Standard_False ; \
 } \
}

// aString is an ExtendedString and is word aligned
// anOtherString is an ExtendedString and is word aligned
#define EXTENDEDSTRINGGREATER( aString , aStringLen , anOtherString , \
                               anOtherStringLen , MinLen , KGreater ) { \
 Standard_Integer LoopIndex ; \
 KGreater = Standard_True ; \
 LoopIndex = 0 ; \
 if ( MinLen > 1 ) { \
   for(; LoopIndex < ( aStringLen >> 1 ) ; LoopIndex++ ) { \
      if ( ((Standard_Integer *) aString )[ LoopIndex ] != \
           ((Standard_Integer *) anOtherString )[ LoopIndex ] ) { \
        LoopIndex++ ; \
        break ; \
        } \
      } \
   LoopIndex = ( LoopIndex - 1 ) << 1 ; \
 } \
 for (; LoopIndex < MinLen ; LoopIndex++ ) { \
    if ( aString[ LoopIndex ] != anOtherString[ LoopIndex ] ) \
      break ; \
    } \
 if ( LoopIndex != MinLen ) { \
   if ( aString[ LoopIndex ] < anOtherString[ LoopIndex ] ) \
     KGreater = Standard_False ; \
 } \
 else if ( aStringLen <= anOtherStringLen ) \
   KGreater = Standard_False ; \
}

#  else

#define STRLEN(s,i) {(i) = 0;while((s)[(i)++] != '\0');(i)--;}
#define EXTSTRLEN(s,i) {(i) = 0;while((s)[(i)++] != 0);(i)--;}
#define STRCPY(s1,s2,i) {for(int j=0; j<(i); j++)(s1)[j] = (s2)[j];}
#define STRCAT(s1,i,s2,j) {for(int k=0; k<(j); k++) (s1)[(i)+k] = (s2)[k];}

# endif

#endif
