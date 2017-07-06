// Created on: 1992-06-24
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Units_UnitsDictionary.hxx>

#include <OSD.hxx>
#include <OSD_OpenFile.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Units.hxx>
#include <Units_Dimensions.hxx>
#include <Units_MathSentence.hxx>
#include <Units_Operators.hxx>
#include <Units_QuantitiesSequence.hxx>
#include <Units_Quantity.hxx>
#include <Units_ShiftedUnit.hxx>
#include <Units_Token.hxx>
#include <Units_TokensSequence.hxx>
#include <Units_Unit.hxx>
#include <Units_UnitSentence.hxx>
#include <Units_UnitsLexicon.hxx>
#include <Units_UnitsSequence.hxx>

#include "../UnitsAPI/UnitsAPI_Units_dat.pxx"

#include <stdio.h>

IMPLEMENT_STANDARD_RTTIEXT(Units_UnitsDictionary,Standard_Transient)

//=======================================================================
//function : Units_UnitsDictionary
//purpose  : 
//=======================================================================
Units_UnitsDictionary::Units_UnitsDictionary()
{ }

//=======================================================================
//function : Creates
//purpose  : 
//=======================================================================

namespace
{

  //! Auxiliary method removing trailing spaces.
  static bool strrightadjust (char *str)
  {
    for (size_t len = strlen(str); len > 0 && IsSpace (str[len-1]); len--)
    {
      str[len-1] = '\0';
    }
    return str[0] != '\0';
  }

  //! Auxiliary method for iterating string line-by-line.
  static const char* readLine (TCollection_AsciiString& theLine,
                               const char* theString)
  {
    theLine.Clear();
    if (theString == NULL)
    {
      return NULL;
    }

    for (const char* aCharIter = theString;; ++aCharIter)
    {
      if (*aCharIter == '\0')
      {
        return NULL;
      }

      if (*aCharIter == '\n')
      {
        const Standard_Integer aLineLen = Standard_Integer(aCharIter - theString);
        if (aLineLen != 0)
        {
          theLine = TCollection_AsciiString (theString, aLineLen);
        }
        return aCharIter + 1;
      }
    }
  }

}

void Units_UnitsDictionary::Creates()
{
  Standard_Boolean ismove;
  Standard_Integer i, j, k, charnumber, unitscomputed;
  Standard_Real matrix[50][50], coeff=0, move=0;
  Handle(Units_Token) token;
  Handle(Units_UnitsSequence) theunitssequence;
  Handle(Units_Unit) unit;
  Handle(Units_ShiftedUnit) shiftedunit;
  Handle(Units_Quantity) quantity;

  thequantitiessequence = new Units_QuantitiesSequence();

  // read file line by line
  Standard_Integer numberofunits = 0;
  TCollection_AsciiString aLine;
  for (const char* aLineIter = readLine (aLine, UnitsAPI_Units_dat); aLineIter != NULL; aLineIter = readLine (aLine, aLineIter))
  {
    // trim trailing spaces
    aLine.RightAdjust();
    if (aLine.IsEmpty())
    {
      continue;
    }

    // lines starting with dot separate sections of the file
    if (aLine.Value (1) == '.')
    {
      // if some units are collected in previous section, store them
      if(numberofunits) {
        unitscomputed = 0;
        for(i=0; i<=numberofunits; i++)
          matrix[i][i] = 1.;
        for(i=0; i<=numberofunits; i++)	{
          if(matrix[i][0])
            unitscomputed++;
        }
        while(unitscomputed != numberofunits+1)	{
          for(j=1; j<=numberofunits; j++) {
            if(!matrix[j][0]) {
              for(i=1; i<j; i++) {
                if(matrix[j][i] && matrix[i][0]) {
                  matrix[j][0] = matrix[i][0]*matrix[j][i];
                  unitscomputed++;
                  if(unitscomputed == numberofunits+1)
                    break;
                }
              }
              for(k=j+1; k<=numberofunits; k++) {
                if(matrix[k][j] && matrix[k][0]) {
                  matrix[j][0] = matrix[k][0]/matrix[k][j];
                  unitscomputed++;
                  if(unitscomputed == numberofunits+1)
                    break;
                }
              }
            }
            if(unitscomputed == numberofunits+1)
              break;
          }
        }
        for(i=1;i<=theunitssequence->Length();i++) {
          unit = theunitssequence->Value(i);
          unit->Value(matrix[i][0]);
        }
      }
	  
      // skip help string and read header
      aLineIter = readLine (aLine, aLineIter);
      aLineIter = readLine (aLine, aLineIter);

      // header consists of dimension name (40 symbols) and factors
      // for basic SI dimensions (mass, length, time, ...)
      char name[41];
      char MM[11], LL[11], TT[11], II[11], tt[11], NN[11], JJ[11], PP[11], SS[11];
      memset(name,0x00,sizeof(name));
      memset(MM,0x00,sizeof(MM));
      memset(LL,0x00,sizeof(LL));
      memset(TT,0x00,sizeof(TT));
      memset(II,0x00,sizeof(II));
      memset(tt,0x00,sizeof(tt));
      memset(NN,0x00,sizeof(NN));
      memset(JJ,0x00,sizeof(JJ));
      memset(PP,0x00,sizeof(PP));
      memset(SS,0x00,sizeof(SS));

      sscanf (aLine.ToCString(), "%40c%10c%10c%10c%10c%10c%10c%10c%10c%10c",
		    name, MM, LL, TT, II, tt, NN, JJ, PP, SS);
      strrightadjust (name);

      Standard_Real M=0., L=0., T=0., I=0., t=0., N=0., J=0., P=0., S=0.;
      OSD::CStringToReal(MM, M);
      OSD::CStringToReal(LL, L);
      OSD::CStringToReal(TT, T);
      OSD::CStringToReal(II, I);
      OSD::CStringToReal(tt, t);
      OSD::CStringToReal(NN, N);
      OSD::CStringToReal(JJ, J);
      OSD::CStringToReal(PP, P);
      OSD::CStringToReal(SS, S);

      Handle(Units_Dimensions) dimensions = 
        new Units_Dimensions (M, L, T, I, t, N, J, P, S);

      numberofunits = 0;
      theunitssequence = new Units_UnitsSequence();
      quantity = new Units_Quantity(name,dimensions,theunitssequence);
      thequantitiessequence->Append(quantity);

      // clean matrix of units
      for(i=0; i<50; i++) {
        for(j=0; j<50; j++)
          matrix[i][j] = 0.;
      }

      // skip next line (dotted)
      aLineIter = readLine (aLine, aLineIter);
    }
    else
    {
      // normal line defining a unit should contain:
      // - unit name (51 symbol)
      // - unit notation (27 symbols)
      // - factor (27 symbols)
      // - base unit (27 symbols)
      char unite[52], symbol[28], convert[28], unit2[28];
      memset(unite,  0x00,sizeof(unite));
      memset(symbol, 0x00,sizeof(symbol));
      memset(convert,0x00,sizeof(convert));
      memset(unit2,  0x00,sizeof(unit2));

      sscanf (aLine.ToCString(), "%51c%27c%27c%27c", unite, symbol, convert, unit2);

      strrightadjust (unite);
      strrightadjust (symbol);
      strrightadjust (convert);
      strrightadjust (unit2);
      if (! unite[0] && ! symbol[0] && ! convert[0] && ! unit2[0])
        continue; // empty line
	  
      if(convert[0] == '[') {
        coeff = 1.;
        i = (Standard_Integer) strlen(convert);
        convert[i-1] = 0;
        ismove = Standard_True;
        charnumber = 1;
        if(unite[0]) {
          numberofunits++;
          shiftedunit = new Units_ShiftedUnit(unite);
          shiftedunit->Quantity(quantity);
          theunitssequence->Append(shiftedunit);
        }
      }
      else {
        ismove = Standard_False;
        charnumber = 0;
        if(unite[0]) {
          numberofunits++;
          unit = new Units_Unit(unite);
          unit->Quantity(quantity);
          theunitssequence->Append(unit);
        }
      }
      
      if(symbol[0]) {
        Units::LexiconUnits(Standard_False)->AddToken(symbol,"U",0.);
        Standard_Integer last = theunitssequence->Length();
        theunitssequence->Value(last)->Symbol(symbol);
      }
	  
      if(convert[charnumber] == '(') {
        i = (Standard_Integer) strlen(convert);
        convert[i-1] = 0;
        Units_MathSentence mathsentence(&convert[charnumber+1]);
        if(ismove)
          move  = (mathsentence.Evaluate())->Value();
        else
          coeff = (mathsentence.Evaluate())->Value();
      }
      else if(convert[0]) {
        if(ismove) {
          OSD::CStringToReal(&convert[charnumber], move);
        }
        else
          OSD::CStringToReal(convert, coeff);
      }
      else {
        coeff = 1.;
      }

      if(ismove) {
        if(move) {
          Standard_Integer last = theunitssequence->Length();
          unit = theunitssequence->Value(last);
          shiftedunit = Handle(Units_ShiftedUnit)::DownCast (unit);
          shiftedunit->Move(move);
        }
      }

      if(unit2[0]) {
        j = 0;
        for(j=1;j<=theunitssequence->Length();j++)
          if(theunitssequence->Value(j) == unit2)break;

        if(j < numberofunits) {
          matrix[numberofunits][j] = coeff;
        }
        else {
          Units_UnitSentence unitsentence(unit2,thequantitiessequence);
          matrix[numberofunits][0] = coeff*(unitsentence.Evaluate())->Value();
        }
      }
      else {
        if(numberofunits == 1) {
          matrix[1][0] = coeff;
          unit = theunitssequence->Value(numberofunits);
          unit->Value(coeff);
        }
      }
    }
  }
}


//=======================================================================
//function : ActiveUnit
//purpose  : 
//=======================================================================

TCollection_AsciiString Units_UnitsDictionary::ActiveUnit(const Standard_CString aquantity) const
{
  Standard_Integer index1;
  Handle(Units_Unit) unit;
  Handle(Units_UnitsSequence) unitssequence;
  Handle(Units_Quantity) quantity;

  for(index1=1;index1<=thequantitiessequence->Length();index1++) {
    quantity = thequantitiessequence->Value(index1);
    if(quantity == aquantity) {
      unitssequence = quantity->Sequence();
      if(unitssequence->Length())
        return unitssequence->Value(1)->SymbolsSequence()->Value(1)->String();
      else {
#ifdef OCCT_DEBUG
        cout<<" Pas d'unite active pour "<<aquantity<<endl;
#endif
        return "";
      }
    }
  }

  cout<<" La grandeur physique "<<aquantity<<" n'existe pas."<<endl;
  return "";
}
