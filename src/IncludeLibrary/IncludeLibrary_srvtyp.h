/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

/* srvtyp.h */ 
/* Types dans les commandes/fonctions serveurs/services applicatifs */

/*
; Type des arguments des commandes serveur et des fonctions services applicatifs

; Code hex.  Type                            4 octets (binaire)

;yxxx0000 : Skip n octets                   1nnnnnnn nnnnnnnn 00000000 00000000
;xxxx0000 : Undefined                       xxxxxxxx xxxxxxxx 00000000 00000000
;00010001 : Byte                            00000000 00000001 00000000 00000001
;00020001 : Integer*2                       00000000 00000010 00000000 00000001
;00040001 : Integer*4                       00000000 00000100 00000000 00000001
;00080001 : Integer*8                       00000000 00001000 00000000 00000001
;00010002 : Logical*1                       00000000 00000001 00000000 00000010
;00040002 : Logical*4                       00000000 00000100 00000000 00000010
;00040004 : Real*4                          00000000 00000100 00000000 00000100
;00080004 : Real*8                          00000000 00001000 00000000 00000100
;00100004 : Real*16                         00000000 00010000 00000000 00000100
;00040008 : Complex*4                       00000000 00000100 00000000 00001000
;00080008 : Complex*8                       00000000 00001000 00000000 00001000
;00100008 : Complex*16                      00000000 00010000 00000000 00001000
;xxxx0010 : Character                       xxxxxxxx xxxxxxxx 00000000 00010000
;xxxx0011 : Extended Character              xxxxxxxx xxxxxxxx 00000000 00010001

;------	Inutilises peut etre pris:
;00040020 : Entry point address             00000000 00000100 00000000 00100000
;00040040 : Etiquette/Instruction address   00000000 00000100 00000000 01000000
;00040080 : Data address                    00000000 00000100 00000000 10000000
;------	fin des Inutilises 

;00000100 : Adresse de datas a remplacer par la valeur dans ACSRV
;00000300 : Adresse de datas indexee a remplacer par la valeur dans ACSRV
;         : (On a l'adresse suivie de l'adresse de l'index)
;00000400 : Indicateur mode de passage IN   00000000 00000000 00000100 00000000
;00000800 : Indicateur mode de passage OUT  00000000 00000000 00001000 00000000
	     Ces indicateurs ne sont pas des types mais sont codes dans le meme
	     mot que le type.
;xxxx1000 : Structure de xxxx champs        xxxxxxxx xxxxxxxx 00010000 00000000
;xxxx2000 : Enumeration de xxxx arguments   xxxxxxxx xxxxxxxx 00100000 00000000
;xxxx4000 : Handle de xxxx mots             xxxxxxxx xxxxxxxx 01000000 00000000
;xxxx8000 : Tableau de xxxx elements        xxxxxxxx xxxxxxxx 10000000 00000000
;                    
*/

#define TYPSKB        0x80000000                /* Skip n octets */
#define TYPUND        0x00000000                /* Undefined */
#define TYPIT1        0x00010001		/* Byte */
#define TYPIT2        0x00020001		/* Integer*2 */
#define TYPIT4        0x00040001		/* Integer*4 */
#define TYPIT8        0x00080001		/* Integer*8 */
#define TYPLO1        0x00010002		/* Logical*1 */
#define TYPLO4        0x00040002		/* Logical*4 */
#define TYPRE4        0x00040004		/* Real*4 */
#define TYPRE8        0x00080004		/* Real*8 */
#define TYPR16        0x00100004		/* Real*16 */
#define TYPCO4        0x00040008		/* Complex*4 */
#define TYPCO8        0x00080008		/* Complex*8 */
#define TYPC16        0x00100008		/* Complex*16 */
#define TYPCHR        0x00000010		/* Character */	
#define TYPCHREXT     0x00000011		/* Character */	

#define TYPINT        0x00000001		/* Integer */
#define TYPLOG        0x00000002		/* Logical */
#define TYPREAL       0x00000004		/* Real */
#define TYPCOMP       0x00000008		/* Complex */

#define MSK_TYPE      0x0000f3ff		/* Masque isole le type  */
#define TYPALONE(X)   (unsigned int ) ((X) & MSK_TYPE)		/* Macro */
#define MSK_LGR       0x7fff0000                /* Masque isole la longueur */
#define LGRALONE(X)   (((X) & MSK_LGR) >> 16)   /* Macro */

#define MSK_TYPCHR    0x0000ffff		/* Masque pour TYPCHR */
#define MSK_LGCHR     0x7fff0000                /* Masque pour lg TYPCHR */
#define MSK_LGUND     0x7fff0000                /* Masque pour lg Undefined */
#define MSK_LGSTRUCT  0x7fff0000                /* Masque pour lg Structure */
#define MSK_LGHDLE    0x7fff0000                /* Masque pour lg Handle */

#define TYPENT        0x00040020	        /* Entry point address */
#define TYPETI        0x00040040	        /* Etiquette/Inst address */
#if defined (__osf__) || defined (DECOSF1)
#define TYPDAD        0x00080080		/* Data address */
#else
#define TYPDAD        0x00040080		/* Data address */
#endif
#define TYPADD        0x00000100		/* Adresse de data a remplacer 
					           par la valeur dans ACSRV */
#define TYPADI        0x00000300		/* Adresse de datas indexee */
	/* A remplacer par la valeur dans ACSRV.  */
	/*  (On a l'adresse suivie de l'adresse de l'index) */

#define TYPSTRUCT    0x00001000		/* Structure */
        /* 16 bits de poids forts  : Nombre de champs de la structure */
        /* Suivent les champs donnes par leur type et leur valeur */

#define TYPENUM      0x00002000		/* Enumeration */
        /* 16 bits de poids forts  : Nombre d'arguments la decrivant */
        /* Suivent les arguments donnes par leur type et leur valeur */

#define TYPHDLE      0x00004000		/* Handle */
        /* 16 bits de poids forts  : Nombre de mots de 32 bits du handle */

#define TYPARR        0x00008000		/* Tableau   */
        /* 16 bits de poids forts : nombre  d'elements du tableau       */
        /* Ce mot de type est suivi par un autre mot de type qui        */
        /* indique la longueur de l'element de tableau et son type      */    

/*  Le mode de passage de l'argument */
/*  Il est mis dans le type          */

#define MODIN         0x00000400
#define MODOUT        0x00000800
#define MODINOUT      0x00000c00
#define MSKMOD         (~MODINOUT)

#define MODALONE(X)   ((X) & (~MSKMOD))		/* Macro */

/* Le protocole 
*/

#define CCALLEVEL   1
