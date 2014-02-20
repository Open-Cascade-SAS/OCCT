// Created by: CAL
// Copyright (c) 1996-1999 Matra Datavision
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

#define XTRACE

#define UNICOD          //GG_041196
//                      Conformite UNICODE

// Modified     22/12/97 : FMN ; Remplacement getenv par OSD_Environment
//        	27/12/98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)
// JR 02.01.100 : Implicit conversions
//-Design       Implementation des methodes de package

#include <Aspect.hxx>
#include <Aspect_Units.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#ifdef UNICOD
#include <Resource_Unicode.hxx>
#include <stdlib.h>
#endif

#include <TColStd_Array2OfReal.hxx>
#include <OSD_Environment.hxx>

# include <stdio.h>

// Valeurs en cms !
static Quantity_Length Widths[20] = {
   84.1,  59.4,  42.0,  29.7,  21.0,
  14.85, 101.6, 101.6, 86.36, 86.36,
  71.12, 71.12, 27.94, 27.94, 71.12,
  86.36, 55.88, 43.18, 27.94, 21.59
};

static Quantity_Length Heights[20] = {
   118.9,   84.1,  59.4,   42.0,  29.7,
    21.0, 363.22, 139.7, 447.04, 139.7,
  363.22, 111.76, 228.6,  57.15, 101.6,
  111.76,  86.36, 55.88,  43.18, 27.94
};

static Standard_CString Fnames[] = {
  "A0","A1","A2","A3","A4","A5","KL","KS","JL","JS","HL","HS","GL","GS","F","E","D","C","B","A"
};

#define LOPTIM
#ifndef LOPTIM
static TCollection_AsciiString astring;
static TCollection_ExtendedString estring;
#else 
static TCollection_AsciiString& _astring() {
    static TCollection_AsciiString astring;
return astring;
}
#define astring _astring()

static TCollection_ExtendedString& _estring() {
    static TCollection_ExtendedString estring;
return estring;
}
#define estring _estring()
#endif // LOPTIM

// Returns size in meters
Standard_CString Aspect::ValuesOfFOSP (
	const Aspect_FormatOfSheetPaper aFOSP,
	Quantity_Length& Width,
	Quantity_Length& Height) {

Standard_Integer Index = Standard_Integer (aFOSP);

	Width	= Quantity_Length (Widths[Index]) CENTIMETER;
	Height	= Quantity_Length (Heights[Index]) CENTIMETER;

	return Fnames[Index];
}

//=========================================================================
Standard_CString Aspect::ToCString(const TCollection_ExtendedString &aString) {
Standard_PCharacter pstring;

#ifdef UNICOD
  Standard_Integer max_string = 2*aString.Length() + 2;
  astring = TCollection_AsciiString(max_string,' ');
  pstring = (Standard_PCharacter)astring.ToCString();
  estring = aString;
  Resource_FormatType aType = Resource_Unicode::GetFormat();
  if( (aType == Resource_EUC) || (aType == Resource_SJIS) ) {
    Standard_Integer i;
    Standard_ExtCharacter c;
    for( i=1 ; i<=aString.Length() ; i++ ) {
      c = estring.Value(i); 
      if( (c >= 0) && (c <= Standard_ExtCharacter(0x0020)) ) { 
         c = Standard_ExtCharacter(0x3000); 
         estring.SetValue(i,c);
      } else if( (c > Standard_ExtCharacter(0x0020)) && 
			(c <= Standard_ExtCharacter(0x007F)) ) {
	c += Standard_ExtCharacter((unsigned short)0xFEE0);
	estring.SetValue(i,c);
      }
    }
  }

//  char *jf = getenv("CSF_JIS_Font");
//  if (((aType == Resource_EUC) || (aType == Resource_SJIS)) && (jf!=((char *)0))) {
  OSD_Environment aVariable("CSF_JIS_Font");
  TCollection_AsciiString jf(aVariable.Value());
  if (((aType == Resource_EUC) || (aType == Resource_SJIS)) && (jf.Length() != 0)) {
    Standard_PCharacter p;

    Resource_Unicode::ConvertUnicodeToEUC(estring, 
					  pstring, 
					  max_string);
    p=(Standard_PCharacter)pstring;
    while ((*p)!='\0') {
       *p &= 0x7F ;
       p++;
    }
  } 
  else if( !Resource_Unicode::ConvertUnicodeToFormat(estring,
						     pstring,
						     max_string) )
        printf(" Aspect::ToCString.BAD string length %d\n",max_string);
#else
  if( aString.IsAscii() ) {
    astring = aString; pstring = astring.ToCString();
  } else {
    pstring = (Standard_CString) aString.ToExtString();
  }
#endif

  return pstring;
}

Standard_Boolean Aspect::Inverse (const TColStd_Array2OfReal& aMat, TColStd_Array2OfReal& Result) {

Standard_Integer lr, ur, lc, uc;
Standard_Integer i, j, k, l;

Standard_Boolean BResult = Standard_False;

	lr = aMat.LowerRow ();
	ur = aMat.UpperRow ();
	lc = aMat.LowerCol ();
	uc = aMat.UpperCol ();

	if ( (ur - lr + 1 != 4) || (uc - lc + 1 != 4) ) {
#ifdef TRACE
		cout << "Inverse () : aMat not a 4x4 matrix" << endl << flush;
		cout << "Inverse returns False => Error\n" << flush;
#endif
		return BResult;
	}

	lr = Result.LowerRow ();
	ur = Result.UpperRow ();
	lc = Result.LowerCol ();
	uc = Result.UpperCol ();

	if ( (ur - lr + 1 != 4) || (uc - lc + 1 != 4) ) {
#ifdef TRACE
		cout << "Inverse () : Result not a 4x4 matrix" << endl << flush;
		cout << "Inverse returns False => Error\n" << flush;
#endif
		return BResult;
	}

Standard_Integer ip1;

float a [5][9];
float amax, p;
int tperm [5][3];
int perm = 0;

	for (i=1; i<=4; i++)
	    for (j=1; j<=4; j++) {
//		a [i][j] = aMat (lr + i - 1, lc + j - 1);
		a [i][j] = (float ) aMat (lr + i - 1, lc + j - 1);
		if (i == j)	Result (lr + i - 1, lc + j - 1) = 1.;
		else		Result (lr + i - 1, lc + j - 1) = 0.;
	    }
	for (i=1; i<=4; i++)
	    for (j=5; j<=8; j++)
		if (i == j-4)	a [i][j] = 1.;
		else		a [i][j] = 0.;

#ifdef TRACE
	cout << "Matrice :\n";
	for (i=1; i<=4; i++) {
	    for (j=1; j<=4; j++)
		cout << a [i][j] << " ";
	    cout << endl;
	}
	cout << flush;
#endif

	for (i=1; i<=4; i++) {
	    ip1 = i + 1;
	    if (ip1 <= 4) {
		/* Recherche pivot maximal */
		l = i;
//		amax = fabs (a [i][i]);
		amax = (float ) fabs (a [i][i]);
		for (k=ip1; k<=4; k++) {
		    if (amax < fabs (a [k][i])) {
			l = k;
//			amax = fabs (a [k][i]);
			amax = (float ) fabs (a [k][i]);
		    } /* if (amax < abs (a [k][i])) */
		}
		/* Permutation eventuelle */
		if (l != i) {
#ifdef TRACE
		cout << "permutation des lignes "
		     << l << " et " << i << "\n" << flush;
#endif
		    perm ++;
		    tperm [perm][1] = l;
		    tperm [perm][2] = i;
		    for (j=1; j<=4; j++) {
			p = a [i][j];
			a [i][j] = a [l][j];
			a [l][j] = p;
		    }
		} /* if (l != i) */
	    } /* if (ip1 <= 4) */
	    p = a [i][i];
	    if (p == 0.) {
#ifdef TRACE
		cout << "p == 0.\n" << flush;
		cout << "Inverse returns False => Error\n" << flush;
#endif
		return BResult;
	    }
	    /* Substitution */
	    for (j=1; j<=4; j++)
		a [i][j] = a [i][j]/p;
	    for (k=1; k<=4; k++)
		if (k != i)
		    for (j=1; j<=4; j++)
			if (j != i) a [k][j] = a [k][j] - a [k][i]*a [i][j];
	    for (k=1; k<=4; k++)
		a [k][i] = - a [k][i]/p;
	    a [i][i] = - a [i][i];
	} /* for (i=1; i<=4; i++) */

	/* Remise en ordre */
	for (k=perm; k>0; k--) {
#ifdef TRACE
	    cout << "permutation des colonnes "
		 << tperm[k][1] << " et " << tperm[k][2] << "\n" << flush;
#endif
	    for (i=1; i<=4; i++) {
		p = a [i][tperm[k][1]];
		a [i][tperm[k][1]] = a [i][tperm[k][2]];
		a [i][tperm[k][2]] = p;
	    }
	}

	for (i=1; i<=4; i++)
	    for (j=1; j<=4; j++)
		Result (lr + i - 1, lc + j - 1) = a [i][j];

#ifdef TRACE
	cout << "Inverse :\n";
	for (i=lr; i<=ur; i++) {
	    for (j=lc; j<=uc; j++)
		cout << Result (i, j) << " ";
	    cout << endl;
	}
	cout << "Inverse returns True => Ok\n" << flush;
#endif

	return Standard_True;
}
