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

//#52 rln 23.12.98

#include <Interface_FloatWriter.hxx>

#include <stdio.h>
Interface_FloatWriter::Interface_FloatWriter (const Standard_Integer chars)
{
  SetDefaults(chars);
}

//  ....                Controle d Envoi des Flottants                ....

    void Interface_FloatWriter::SetFormat
  (const Standard_CString form, const Standard_Boolean reset)
{
  strcpy(themainform,form);
  if (!reset) return;
  therange1 = therange2 = 0.;    // second form : inhibee
  thezerosup = Standard_False;
}

    void Interface_FloatWriter::SetFormatForRange
  (const Standard_CString form,
   const Standard_Real R1, const Standard_Real R2)
{
  strcpy(therangeform,form);
  therange1 = R1;
  therange2 = R2;
}

    void Interface_FloatWriter::SetZeroSuppress (const Standard_Boolean mode)
      {  thezerosup = mode;  }

    void Interface_FloatWriter::SetDefaults (const Standard_Integer chars)
{
  if (chars <= 0) {
    strcpy(themainform  ,"%E");
    strcpy(therangeform ,"%f");
  } else {
    char pourcent = '%'; char point = '.';
    Sprintf(themainform,  "%c%d%c%dE",pourcent,chars+2,point,chars);
    Sprintf(therangeform, "%c%d%c%df",pourcent,chars+2,point,chars);
  }
  therange1 = 0.1; therange2 = 1000.;
  thezerosup = Standard_True;
}

    void Interface_FloatWriter::Options
  (Standard_Boolean& zerosup, Standard_Boolean& range,
   Standard_Real& R1, Standard_Real& R2) const
{
  zerosup = thezerosup;
  range = (therange2 >= therange1 && therange1 >= 0.);
  R1 = therange1;
  R2 = therange2;
}

    Standard_CString  Interface_FloatWriter::MainFormat () const
      {  const Standard_CString mainform  = Standard_CString(&themainform[0]);  return mainform;   }

    Standard_CString  Interface_FloatWriter::FormatForRange () const
      {  const Standard_CString rangeform = Standard_CString(&therangeform[0]); return rangeform;  }

//  ########################################################################

    Standard_Integer Interface_FloatWriter::Write
  (const Standard_Real val, const Standard_CString text) const
{
  const Standard_CString mainform  = Standard_CString(themainform);
  const Standard_CString rangeform = Standard_CString(therangeform);
  return Convert
    (val,text,thezerosup,therange1,therange2,mainform,rangeform);
}

//=======================================================================
//function : Convert
//purpose  : 
//=======================================================================
Standard_Integer Interface_FloatWriter::Convert (const Standard_Real val, 
						 const Standard_CString text,
						 const Standard_Boolean zsup, 
						 const Standard_Real R1, 
						 const Standard_Real R2,
						 const Standard_CString mainform, 
						 const Standard_CString rangeform)
{
//    Valeur flottante, expurgee de "0000" qui trainent et de "E+00"
  const Standard_Integer anMasSize = 5; // change 6 to 5: index 5 is not used below
  char lxp[anMasSize], *pText; 
  int i0 = 0, j0 = 0;

  for (Standard_Integer i = 0; i < anMasSize; ++i)
    lxp[i] = '\0';

  pText=(char *)text;
  //
  if ( (val >= R1 && val <  R2) || (val <= -R1 && val > -R2) ) 
    Sprintf(pText,rangeform,val);
  else 
    Sprintf(pText,mainform,val);
  
  if (zsup) 
  {
    for (int i = 0; i < 16; i ++) 
    {
      i0 = i;
      if (text[i] == 'e' || text[i] == 'E') 
      {
	      lxp[0] = 'E'; 
	      lxp[1] = text[i+1]; 
	      lxp[2] = text[i+2];
	      lxp[3] = text[i+3];  
	      lxp[4] = text[i+4];
	      
        if (lxp[1] == '+' && lxp[2] == '0' && lxp[3] == '0' &&  lxp[4] == '\0') 
	        lxp[0] = '\0';

	      pText[i] = '\0';
      }
      if (text[i] == '\0') break;
    }
    //#52 rln 23.12.98 converting 1e-07 throws exception
    for (int j = i0-1; j >= 0; j --) 
    {
      j0 = j;  

      if (text[j] != '0') 
	      break;

      pText[j] = '\0';
    }

    pText[j0+1] = lxp[0]; 
    pText[j0+2] = lxp[1]; 
    pText[j0+3] = lxp[2];
    pText[j0+4] = lxp[3]; 
    pText[j0+5] = lxp[4]; 
    pText[j0+6] = '\0';
  }
  return (Standard_Integer)strlen(text);
}
