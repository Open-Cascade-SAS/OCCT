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


#include <TCollection_AsciiString.hxx>
#include <Units.hxx>
#include <Units_Operators.hxx>
#include <Units_QuantitiesSequence.hxx>
#include <Units_Quantity.hxx>
#include <Units_Token.hxx>
#include <Units_TokensSequence.hxx>
#include <Units_Unit.hxx>
#include <Units_UnitsDictionary.hxx>
#include <Units_UnitSentence.hxx>
#include <Units_UnitsSequence.hxx>

//=======================================================================
//function : Units_UnitSentence
//purpose  : 
//=======================================================================
Units_UnitSentence::Units_UnitSentence (const Standard_CString astring)
 : Units_Sentence(Units::LexiconUnits(), astring)
{ 
  Analyse();
  SetConstants();
  SetUnits(Units::DictionaryOfUnits()->Sequence());

}


//=======================================================================
//function : Units_UnitSentence
//purpose  : 
//=======================================================================

Units_UnitSentence::Units_UnitSentence
  (const Standard_CString astring,
   const Handle(Units_QuantitiesSequence)& aquantitiessequence)
 : Units_Sentence(Units::LexiconUnits(Standard_False),astring)
{ 
  Analyse();
  SetConstants();
  SetUnits(aquantitiessequence);
}


//=======================================================================
//function : Analyse
//purpose  : 
//=======================================================================

void Units_UnitSentence::Analyse()
{
  if(Sequence()->Length()==0)
    return;

  Standard_Integer index;
  TCollection_AsciiString s;
  Handle(Units_Token) token;
  Handle(Units_Token) previoustoken;
  Handle(Units_Token) currenttoken;
  Handle(Units_Token) nexttoken;
  Handle(Units_Token) lasttoken;
  Handle(Units_TokensSequence) sequence = Sequence();

  currenttoken = sequence->Value(1);

  //cout<<endl;
  //for(int ind=1; ind<=sequence->Length(); ind++) {
  //  Handle(Units_Token) tok = sequence->Value(ind);
  //  cout<<tok->Word()<<" ";
  //}
  //cout<<endl;
  //for(ind=1; ind<=sequence->Length(); ind++) {
  //  Handle(Units_Token) tok = sequence->Value(ind);
  //  cout<<tok->Mean()<<" ";
  //}
  //cout<<endl;

  for(index=2;index<=sequence->Length();index++) {
    previoustoken = currenttoken;
    currenttoken = sequence->Value(index);
    s = currenttoken->Mean();
    if( s=="MU" || s=="U" ) {
      if( previoustoken->Mean()=="M" || previoustoken->Mean()=="MU" ) {
        previoustoken->Mean("M");
        currenttoken->Mean("U");
        currenttoken->Value(0.);
      }
    }
  }

  for(index=1;index<=sequence->Length();index++) {
    currenttoken = sequence->Value(index);
    s=currenttoken->Mean();
    if(s=="MU") {
      currenttoken->Mean("U");
      currenttoken->Value(0.);
    }
  }

  currenttoken = sequence->Value(1);
  index = 2;

  while(index<=sequence->Length()) {
    previoustoken = currenttoken;
    currenttoken = sequence->Value(index);
    if( previoustoken->Mean()=="M" && currenttoken->Mean()=="U") {
      token=new Units_Token("(", "S");
      sequence->InsertBefore(index-1,token);
      token = new Units_Token("*", "O");
      sequence->InsertBefore(++index,token);
      token = new Units_Token(")", "S");
      sequence->InsertAfter(++index,token);
      index++;
    }
    index++;
  }

  index = 1;
  while(index <= sequence->Length()) {
    currenttoken = sequence->Value(index);
    if(currenttoken->Mean()=="P") {
      if( currenttoken->Word()=="sq." || currenttoken->Word()=="cu." ) {
        sequence->Exchange(index,index+1);
        index++;
      }
      token = new Units_Token("**", "O");
      sequence->InsertBefore(index,token);
      index++;
    }
    index++;
  }

  currenttoken = sequence->Value(1);
  index = 2;

  while(index<=sequence->Length()) {
    previoustoken = currenttoken;
    currenttoken=sequence->Value(index);
    if(currenttoken->Word()=="(") {
      if( !( previoustoken->Mean()=="O" || previoustoken->Word()=="(" ) ) {
        token=new Units_Token("*", "O");
        sequence->InsertBefore(index,token);
        index++;
      }
    }
    else if(previoustoken->Word()==")")	{
      if( !( currenttoken->Mean()=="O" || currenttoken->Word()==")" ) ) {
        token = new Units_Token("*", "O");
        sequence->InsertBefore(index,token);
        index++;
      }
    }
    index++;
  }

}


//=======================================================================
//function : SetUnits
//purpose  : 
//=======================================================================

void Units_UnitSentence::SetUnits
  (const Handle(Units_QuantitiesSequence)& aquantitiessequence)
{
  Standard_Integer index,jindex,kindex;
  Standard_Boolean istheend=0;
  Handle(Units_Quantity) quantity;
  Handle(Units_TokensSequence) sequenceoftokens;
  Handle(Units_Token) currenttoken;
  Handle(Units_UnitsSequence) unitssequence;
  Handle(Units_Unit) unit;
  TCollection_AsciiString symbol;

  Handle(Units_QuantitiesSequence) quantitiessequence = aquantitiessequence;

  for(index=1; index<=quantitiessequence->Length(); index++) {
    quantity = quantitiessequence->Value(index);
    unitssequence=quantity->Sequence();
    for(jindex=1; jindex<=unitssequence->Length(); jindex++) {
      unit = unitssequence->Value(jindex);
      sequenceoftokens = Sequence();
      istheend = Standard_True;
      for(kindex=1; kindex<=sequenceoftokens->Length(); kindex++) {
        currenttoken = sequenceoftokens->Value(kindex);
        if(currenttoken->Mean()=="U") {
          if(currenttoken->Value()==0.) {
            symbol = currenttoken->Word();
            if(unit == symbol.ToCString())
              sequenceoftokens->SetValue(kindex,unit->Token());
            else
              istheend = Standard_False;
          }
        }
      }
      if(istheend)
        break;
    }
    if(istheend)
      break;
  }
}
