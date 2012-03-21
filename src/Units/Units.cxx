// Created on: 1992-10-28
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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

// Modified     Wed Apr  2 14:10:05 1997 by Gerard GRAS
//              Add FirstQuantity access methods
// Modified     Mon Apr  7 16:52:40 1997 by Patrick BOSINCO
//              Add Dimensions access methods

#define PRO8619	//GG_160697
//		Convertir correctement les unites translatees

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <Units.hxx>
#include <Units_Measurement.hxx>
#include <Units_Quantity.hxx>
#include <Units_Dimensions.hxx>
#include <Units_Unit.hxx>
#include <Units_Lexicon.hxx>
#include <Units_UnitsLexicon.hxx>
#include <Units_UnitsDictionary.hxx>
#include <Units_UnitsSystem.hxx>
#include <Units_UnitSentence.hxx>
#include <Units_ShiftedToken.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Units_Operators.hxx>

static Handle(Units_Dimensions) nulldimensions;
static Handle(Units_UnitsLexicon) lexiconunits;
static Handle(Units_Lexicon) lexiconformula;
static Handle(Units_UnitsDictionary) unitsdictionary;
static Handle(Units_UnitsSystem) unitssystem;

static TCollection_AsciiString unitsfile;
static TCollection_AsciiString lexiconfile;
static TCollection_AsciiString lastunit;
static Handle(Units_Dimensions) lastdimension;
static Standard_Real lastvalue,lastmove;


//=======================================================================
//function : UnitsFile
//purpose  : 
//=======================================================================

void Units::UnitsFile(const Standard_CString afile) {
  unitsfile = TCollection_AsciiString(afile);
}


//=======================================================================
//function : LexiconFile
//purpose  : 
//=======================================================================

void Units::LexiconFile(const Standard_CString afile) {
  lexiconfile = TCollection_AsciiString(afile);
}


//=======================================================================
//function : DictionaryOfUnits
//purpose  : 
//=======================================================================

Handle(Units_UnitsDictionary) Units::DictionaryOfUnits(const Standard_Boolean amode)
{
  if(unitsdictionary.IsNull())
    {
//      cout<<"Allocation du dictionnaire"<<endl;
      unitsdictionary = new Units_UnitsDictionary();
//      cout<<"Creation du dictionnaire"<<endl;
      unitsdictionary->Creates(unitsfile.ToCString());
    }
  else if(amode)
    {
//      cout<<"Creation du dictionnaire"<<endl;
      unitsdictionary->Creates(unitsfile.ToCString());
    }
  else if(!unitsdictionary->UpToDate())
    {
//      cout<<"Creation du dictionnaire"<<endl;
      unitsdictionary->Creates(unitsfile.ToCString());
    }
  return unitsdictionary;
}


//=======================================================================
//function : Quantity
//purpose  : 
//=======================================================================

Handle(Units_Quantity) Units::Quantity(const Standard_CString aquantity)
{
  Standard_Integer index;
  Handle(Units_Quantity) quantity;
  Handle(Units_Quantity) nullquantity;
  Handle(Units_QuantitiesSequence) quantitiessequence;

  quantitiessequence = Units::DictionaryOfUnits()->Sequence();
  for(index=1;index<=quantitiessequence->Length();index++)
    {
      quantity = quantitiessequence->Value(index);
      if(quantity->Name() == aquantity) return quantity;
    }

  cout<<"Warning: BAD Quantity = Units::Quantity(quantity('" << aquantity << "'))" << endl;
  return nullquantity;
}


//=======================================================================
//function : Quantity
//purpose  :
//=======================================================================

static TCollection_AsciiString symbol_string,quantity_string;
Standard_CString Units::FirstQuantity(const Standard_CString aunit)
{
  Standard_Integer i,j,k;
  Handle(Units_Quantity) thequantity;
  Handle(Units_QuantitiesSequence) quantitiessequence;
  Handle(Units_UnitsSequence) unitssequence;
  Handle(Units_Unit) unit;
  Handle(TColStd_HSequenceOfHAsciiString) symbolssequence;
  TCollection_AsciiString symbol(aunit);

  if( symbol == symbol_string ) return quantity_string.ToCString();

  quantitiessequence = Units::DictionaryOfUnits()->Sequence();
  for(i=1;i<=quantitiessequence->Length();i++) {
    thequantity = quantitiessequence->Value(i);
    unitssequence = thequantity->Sequence();
    for(j=1;j<=unitssequence->Length();j++) {
      unit = unitssequence->Value(j);
      symbolssequence = unit->SymbolsSequence();
      for( k=1;k<=symbolssequence->Length();k++) {
	if( symbol == symbolssequence->Value(k)->String() ) {
          symbol_string = symbol;
	  quantity_string = thequantity->Name();
          return quantity_string.ToCString();
	}
      }
    }
  }

  cout<<"Warning: BAD Quantity = Units::Quantity(unit('" << symbol << "'))" << endl;
  return NULL;
}


//=======================================================================
//function : LexiconUnits
//purpose  : 
//=======================================================================

Handle(Units_Lexicon) Units::LexiconUnits(const Standard_Boolean amode)
{
  if(lexiconunits.IsNull()) {
//      cout<<"Allocation du lexique d'unites"<<endl;
    lexiconunits = new Units_UnitsLexicon();
//      cout<<"Creation du lexique d'unites"<<endl;
    lexiconunits->Creates(lexiconfile.ToCString(),unitsfile.ToCString(),amode);
  }
  else if(!lexiconunits->UpToDate()) {
//      cout<<"Creation du lexique d'unites"<<endl;
    lexiconunits->Creates(lexiconfile.ToCString(),unitsfile.ToCString(),amode);
  }
  return lexiconunits;
}


//=======================================================================
//function : LexiconFormula
//purpose  : 
//=======================================================================

Handle(Units_Lexicon) Units::LexiconFormula()
{
  if(lexiconformula.IsNull()) {
//      cout<<"Allocation du lexique d'expression"<<endl;
    lexiconformula = new Units_Lexicon();
//      cout<<"Creation du lexique d'expression"<<endl;
    lexiconformula->Creates(lexiconfile.ToCString());
  }
  else if(!lexiconformula->UpToDate()) {
//      cout<<"Creation du lexique d'expression"<<endl;
    lexiconformula->Creates(lexiconfile.ToCString());
  }
  return lexiconformula;
}


//=======================================================================
//function : NullDimensions
//purpose  : 
//=======================================================================

Handle(Units_Dimensions) Units::NullDimensions()
{
  if(nulldimensions.IsNull()) nulldimensions = new Units_Dimensions(0.,0.,0.,0.,0.,0.,0.,0.,0.);
  return nulldimensions;
}


//=======================================================================
//function : Convert
//purpose  : 
//=======================================================================

Standard_Real Units::Convert(const Standard_Real avalue,
			     const Standard_CString afirstunit,
			     const Standard_CString asecondunit)
{
  Units_Measurement measurement(avalue,afirstunit);
  measurement.Convert(asecondunit);
  return measurement.Measurement();
}


//=======================================================================
//function : ToSI
//purpose  :
//=======================================================================

Standard_Real Units::ToSI(const Standard_Real aData,
                          const Standard_CString aUnit){

  Handle(Units_Dimensions) aDimBid;
  return Units::ToSI(aData,aUnit,aDimBid);
}


//=======================================================================
//function : ToSI
//purpose  :
//=======================================================================

Standard_Real Units::ToSI(const Standard_Real aData,
                          const Standard_CString aUnit,
                          Handle(Units_Dimensions) &dim)
{
  if(lastunit != aUnit ) {

    lastunit = TCollection_AsciiString(aUnit);
    Units_UnitSentence unitsentence(aUnit);
    if(!unitsentence.IsDone()) {
      cout<<"can not convert - incorrect unit => return 0.0"<<endl;
      return 0.0;
    }
    Handle(Units_Token) token = unitsentence.Evaluate();
    lastvalue = token->Value();
    lastmove = 0.;
    if( token->IsKind(STANDARD_TYPE(Units_ShiftedToken)) ) {
      Handle(Units_ShiftedToken) stoken =
        Handle(Units_ShiftedToken)::DownCast(token) ;
      lastmove = stoken->Move();
    }
    lastdimension = token->Dimensions();
  }
  dim = lastdimension;
#ifdef PRO8619
  return (aData + lastmove) * lastvalue;
#else
  return aData * lastvalue;
#endif
}


//=======================================================================
//function : FromSI
//purpose  :
//=======================================================================

Standard_Real Units::FromSI(const Standard_Real aData,
                            const Standard_CString aUnit){
  Handle(Units_Dimensions) aDimBid;
  return Units::FromSI(aData,aUnit,aDimBid);
}


//=======================================================================
//function : FromSI
//purpose  :
//=======================================================================

Standard_Real Units::FromSI(const Standard_Real aData,
                            const Standard_CString aUnit,
                            Handle(Units_Dimensions) &dim)
{
  if(lastunit != aUnit) {
    lastunit = TCollection_AsciiString(aUnit);
    Units_UnitSentence unitsentence(aUnit);
    if(!unitsentence.IsDone()) {
      cout<<"Warning: can not convert - incorrect unit => return 0.0"<<endl;
      return 0.0;
    }
    Handle(Units_Token) token = unitsentence.Evaluate();
    lastvalue = token->Value();
    lastmove = 0.;
    if( token->IsKind(STANDARD_TYPE(Units_ShiftedToken)) ) {
      Handle(Units_ShiftedToken) stoken =
        Handle(Units_ShiftedToken)::DownCast(token) ;
      lastmove = stoken->Move();
    }
    lastdimension = token->Dimensions();
  }
  dim = lastdimension;
#ifdef PRO8619
  return (aData / lastvalue) - lastmove;
#else
  return aData / lastvalue;
#endif
}


//=======================================================================
//function : Dimensions
//purpose  :
//=======================================================================

Handle(Units_Dimensions) Units::Dimensions(const Standard_CString aType)
{
 if (aType) {
   Handle(Units_UnitsDictionary) dico=Units::DictionaryOfUnits(Standard_False);

   Handle(Units_QuantitiesSequence) qSeq = dico->Sequence();
   Handle(Units_Quantity) q;
   for (Standard_Integer i=1; i <= qSeq->Length(); i++) {
     if (qSeq->Value(i) == aType) {
       return qSeq->Value(i)->Dimensions();
     }
   }
   Standard_NoSuchObject::Raise("Units::Dimensions");
 }
 return Units_Dimensions::ALess();
}
