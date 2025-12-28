// Created on: 1992-10-28
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
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Units_Operators.hxx>

#include <stdlib.h>

static occ::handle<Units_Dimensions>      nulldimensions;
static occ::handle<Units_UnitsLexicon>    lexiconunits;
static occ::handle<Units_Lexicon>         lexiconformula;
static occ::handle<Units_UnitsDictionary> unitsdictionary;
static occ::handle<Units_UnitsSystem>     unitssystem;

static TCollection_AsciiString       unitsfile;
static TCollection_AsciiString       lexiconfile;
static TCollection_AsciiString       lastunit;
static occ::handle<Units_Dimensions> lastdimension;
static double                        lastvalue, lastmove;

//=================================================================================================

void Units::UnitsFile(const char* afile)
{
  unitsfile = TCollection_AsciiString(afile);
}

//=================================================================================================

void Units::LexiconFile(const char* afile)
{
  lexiconfile = TCollection_AsciiString(afile);
}

//=================================================================================================

occ::handle<Units_UnitsDictionary> Units::DictionaryOfUnits(const bool amode)
{
  if (unitsdictionary.IsNull())
  {
    //      std::cout<<"Allocation du dictionnaire"<<std::endl;
    unitsdictionary = new Units_UnitsDictionary();
    //      std::cout<<"Creation du dictionnaire"<<std::endl;
    unitsdictionary->Creates();
  }
  else if (amode)
  {
    //      std::cout<<"Creation du dictionnaire"<<std::endl;
    unitsdictionary->Creates();
  }
  return unitsdictionary;
}

//=================================================================================================

occ::handle<Units_Quantity> Units::Quantity(const char* aquantity)
{
  int                                                             index;
  occ::handle<Units_Quantity>                                     quantity;
  occ::handle<Units_Quantity>                                     nullquantity;
  occ::handle<NCollection_HSequence<occ::handle<Units_Quantity>>> quantitiessequence;

  quantitiessequence = Units::DictionaryOfUnits()->Sequence();
  for (index = 1; index <= quantitiessequence->Length(); index++)
  {
    quantity = quantitiessequence->Value(index);
    if (quantity->Name() == aquantity)
      return quantity;
  }

#ifdef OCCT_DEBUG
  std::cout << "Warning: BAD Quantity = Units::Quantity(quantity('" << aquantity << "'))"
            << std::endl;
#endif
  return nullquantity;
}

//=================================================================================================

static TCollection_AsciiString symbol_string, quantity_string;

const char* Units::FirstQuantity(const char* aunit)
{
  int                                                                       i, j, k;
  occ::handle<Units_Quantity>                                               thequantity;
  occ::handle<NCollection_HSequence<occ::handle<Units_Quantity>>>           quantitiessequence;
  occ::handle<NCollection_HSequence<occ::handle<Units_Unit>>>               unitssequence;
  occ::handle<Units_Unit>                                                   unit;
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> symbolssequence;
  TCollection_AsciiString                                                   symbol(aunit);

  if (symbol == symbol_string)
    return quantity_string.ToCString();

  quantitiessequence = Units::DictionaryOfUnits()->Sequence();
  for (i = 1; i <= quantitiessequence->Length(); i++)
  {
    thequantity   = quantitiessequence->Value(i);
    unitssequence = thequantity->Sequence();
    for (j = 1; j <= unitssequence->Length(); j++)
    {
      unit            = unitssequence->Value(j);
      symbolssequence = unit->SymbolsSequence();
      for (k = 1; k <= symbolssequence->Length(); k++)
      {
        if (symbol == symbolssequence->Value(k)->String())
        {
          symbol_string   = symbol;
          quantity_string = thequantity->Name();
          return quantity_string.ToCString();
        }
      }
    }
  }

#ifdef OCCT_DEBUG
  std::cout << "Warning: BAD Quantity = Units::Quantity(unit('" << symbol << "'))" << std::endl;
#endif
  return NULL;
}

//=================================================================================================

occ::handle<Units_Lexicon> Units::LexiconUnits(const bool amode)
{
  if (lexiconunits.IsNull())
  {
    //      std::cout<<"Allocation du lexique d'unites"<<std::endl;
    lexiconunits = new Units_UnitsLexicon();
    //      std::cout<<"Creation du lexique d'unites"<<std::endl;
    lexiconunits->Creates(amode);
  }
  return lexiconunits;
}

//=================================================================================================

occ::handle<Units_Lexicon> Units::LexiconFormula()
{
  if (lexiconformula.IsNull())
  {
    //      std::cout<<"Allocation du lexique d'expression"<<std::endl;
    lexiconformula = new Units_Lexicon();
    //      std::cout<<"Creation du lexique d'expression"<<std::endl;
    lexiconformula->Creates();
  }
  return lexiconformula;
}

//=================================================================================================

occ::handle<Units_Dimensions> Units::NullDimensions()
{
  if (nulldimensions.IsNull())
    nulldimensions = new Units_Dimensions(0., 0., 0., 0., 0., 0., 0., 0., 0.);
  return nulldimensions;
}

//=================================================================================================

double Units::Convert(const double avalue, const char* afirstunit, const char* asecondunit)
{
  Units_Measurement measurement(avalue, afirstunit);
  measurement.Convert(asecondunit);
  return measurement.Measurement();
}

//=================================================================================================

double Units::ToSI(const double aData, const char* aUnit)
{

  occ::handle<Units_Dimensions> aDimBid;
  return Units::ToSI(aData, aUnit, aDimBid);
}

//=================================================================================================

double Units::ToSI(const double aData, const char* aUnit, occ::handle<Units_Dimensions>& dim)
{
  if (lastunit != aUnit)
  {
    Units_UnitSentence unitsentence(aUnit);
    if (!unitsentence.IsDone())
    {
#ifdef OCCT_DEBUG
      std::cout << "can not convert - incorrect unit => return 0.0" << std::endl;
#endif
      return 0.0;
    }
    occ::handle<Units_Token> token = unitsentence.Evaluate();
    lastvalue                      = token->Value();
    lastmove                       = 0.;
    if (token->IsKind(STANDARD_TYPE(Units_ShiftedToken)))
    {
      occ::handle<Units_ShiftedToken> stoken = occ::down_cast<Units_ShiftedToken>(token);
      lastmove                               = stoken->Move();
    }
    lastunit      = TCollection_AsciiString(aUnit);
    lastdimension = token->Dimensions();
  }
  dim = lastdimension;
  return (aData + lastmove) * lastvalue;
}

//=================================================================================================

double Units::FromSI(const double aData, const char* aUnit)
{
  occ::handle<Units_Dimensions> aDimBid;
  return Units::FromSI(aData, aUnit, aDimBid);
}

//=================================================================================================

double Units::FromSI(const double aData, const char* aUnit, occ::handle<Units_Dimensions>& dim)
{
  if (lastunit != aUnit)
  {
    Units_UnitSentence unitsentence(aUnit);
    if (!unitsentence.IsDone())
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: can not convert - incorrect unit => return 0.0" << std::endl;
#endif
      return 0.0;
    }
    occ::handle<Units_Token> token = unitsentence.Evaluate();
    lastvalue                      = token->Value();
    lastmove                       = 0.;
    if (token->IsKind(STANDARD_TYPE(Units_ShiftedToken)))
    {
      occ::handle<Units_ShiftedToken> stoken = occ::down_cast<Units_ShiftedToken>(token);
      lastmove                               = stoken->Move();
    }
    lastunit      = TCollection_AsciiString(aUnit);
    lastdimension = token->Dimensions();
  }
  dim = lastdimension;
  return (aData / lastvalue) - lastmove;
}

//=================================================================================================

occ::handle<Units_Dimensions> Units::Dimensions(const char* aType)
{
  if (aType)
  {
    occ::handle<Units_UnitsDictionary> dico = Units::DictionaryOfUnits(false);

    occ::handle<NCollection_HSequence<occ::handle<Units_Quantity>>> qSeq = dico->Sequence();
    occ::handle<Units_Quantity>                                     q;
    for (int i = 1; i <= qSeq->Length(); i++)
    {
      if (qSeq->Value(i) == aType)
      {
        return qSeq->Value(i)->Dimensions();
      }
    }
    throw Standard_NoSuchObject("Units::Dimensions");
  }
  return Units_Dimensions::ALess();
}
