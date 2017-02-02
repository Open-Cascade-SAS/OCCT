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


#include <Units_Lexicon.hxx>
#include <Units_NoSuchType.hxx>
#include <Units_Operators.hxx>
#include <Units_Sentence.hxx>
#include <Units_ShiftedToken.hxx>
#include <Units_Token.hxx>
#include <Units_TokensSequence.hxx>

static Handle(Units_Token) CreateTokenForNumber(const Standard_CString str)
{
  TCollection_AsciiString tstr = str[0];
  Standard_Boolean IsPoint = Standard_False;
  Standard_Size len = strlen(str);
  for(Standard_Size in=1; in < len; in++) {
    if( str[in]=='0' || str[in]=='1' || str[in]=='2' || str[in]=='3' || 
        str[in]=='4' || str[in]=='5' || str[in]=='6' || str[in]=='7' || 
        str[in]=='8' || str[in]=='9' ) {
      tstr.AssignCat(str[in]);
    }
    else if( str[in]=='.' && !IsPoint ) {
      tstr.AssignCat(str[in]);
      IsPoint = Standard_True;
    }
    else
      break;
  }
  return new Units_Token(tstr.ToCString(), "0");
}


//=======================================================================
//function : Units_Sentence
//purpose  : 
//=======================================================================

Units_Sentence::Units_Sentence(const Handle(Units_Lexicon)& alexicon,
			       const Standard_CString astring)
{
  Standard_Integer index;
  Standard_Size i,limchain;
  Handle(Units_Token) token;
  Handle(Units_Token) referencetoken;
  
  thesequenceoftokens = new Units_TokensSequence();
  Handle(Units_TokensSequence) lstlexicon=alexicon->Sequence();
  if(lstlexicon.IsNull())
    throw Units_NoSuchType("BAD LEXICON descriptor");
  limchain=strlen(astring);
  i=0;

  TCollection_AsciiString tmpstr = astring;
  //Handle(Units_Token) tmptoken;
  TCollection_AsciiString PrevMean;
  TCollection_AsciiString PrevWord;
  while(i < limchain) {
    Standard_Boolean IsFound = Standard_False;
    TCollection_AsciiString LastWord = "";
    for(index=1; index<=lstlexicon->Length(); index++) {
      referencetoken = lstlexicon->Value(index);
      TCollection_AsciiString aword = referencetoken->Word();
      int num = tmpstr.Search(aword);
      if( num==1 && aword.Length()>=LastWord.Length() ) {
        token = referencetoken->Creates();
        LastWord = aword;
        IsFound = Standard_True;
      }
    }
    if(!IsFound) {
      // may be it is a number(degree)?
      LastWord = tmpstr.SubString(1,1);
      if(!LastWord.IsIntegerValue()) {
        // unknown expression - not create sentene
        thesequenceoftokens->Clear();
#ifdef OCCT_DEBUG
        cout<<"Warning: can not create correct sentence from string: "<<astring<<endl;
#endif
        return;
      }
      else {
        // create token for number
        token = CreateTokenForNumber(tmpstr.ToCString());
        LastWord = token->Word();
      }
    }
    if(i>0) {
      // make additional checking
      if( ( token->Mean()=="M" && ( PrevMean=="M" || PrevMean=="MU" || PrevMean=="0" ) ) ||
          ( token->Mean()=="U" && ( PrevMean=="U" || PrevMean=="0" )                   ) ||
          ( token->Mean()=="O" && ( PrevMean=="M" || PrevMean=="O" )                   ) ||
          ( ( token->Word()=="+" || token->Word()=="-" ) && PrevWord!="("              ) ||
          ( token->Mean()=="S" && ( PrevMean=="M" )                                    ) ||
          ( token->Mean()=="0" && ( PrevMean=="M" || PrevMean=="U" || PrevMean=="MU" ) ) )  {
        // incorrect situation
        thesequenceoftokens->Clear();
#ifdef OCCT_DEBUG
        cout<<"Warning: can not create correct sentence from string: "<<astring<<endl;
#endif
        return;
      }
    }
    thesequenceoftokens->Append(token);
    PrevMean = token->Mean();
    PrevWord = token->Word();
    i = i + LastWord.Length();
    tmpstr.Remove(1,LastWord.Length());
  }
}


//=======================================================================
//function : SetConstants
//purpose  : 
//=======================================================================

void Units_Sentence::SetConstants()
{
  Standard_Integer index;
  Standard_Real value;
  Handle(Units_Token) token;
  TCollection_AsciiString string;

  for(index=1;index<=thesequenceoftokens->Length();index++) {
    token = thesequenceoftokens->Value(index);
    if(token->Value()==0.) {
      string = token->Word();
      if(string.IsRealValue()) {
        value = string.RealValue();
        token->Mean("K");
        token->Value(value);
      }
    }
  }
}


//=======================================================================
//function : CalculateLocal
//purpose  : auxilary
//=======================================================================
static Handle(Units_Token) CalculateLocal(const Handle(Units_TokensSequence)& aSeq)
{
  //cout<<endl;
  //for(int index=1; index<=aSeq->Length(); index++) {
  //  Handle(Units_Token) tok = aSeq->Value(index);
  //  cout<<tok->Word()<<" ";
  //}
  //cout<<endl;
  Handle(Units_Token) tok1,tok2;
  Standard_Integer i,j;

  if(aSeq->Length()==1) {
    return aSeq->Value(1);
  }

  // case of unar sign
  if(aSeq->Length()==2) {
    if(aSeq->Value(1)->Word()=="+")
      aSeq->Remove(1);
    if(aSeq->Value(1)->Word()=="-") {
      tok2 = aSeq->Value(2);
      TCollection_AsciiString aword = "-";
      aword.AssignCat(tok2->Word());
      tok1 = new Units_Token(aword.ToCString(), tok2->Mean().ToCString(),
                             tok2->Value()*(-1.0), tok2->Dimensions());
      aSeq->Remove(1);
      aSeq->SetValue(1,tok1);
    }
    return aSeq->Value(1);
  }

  Standard_Boolean IsBracket = Standard_True;
  while(IsBracket) {
    for(i=1; i<=aSeq->Length(); i++) {
      if(aSeq->Value(i)->Word()=="(") {
        Handle(Units_TokensSequence) TmpSeq = new Units_TokensSequence;
        Standard_Integer NbBrackets = 1;
        for(j=i+1; j<=aSeq->Length(); j++) {
          if(aSeq->Value(j)->Word()==")")
            NbBrackets--;
          if(aSeq->Value(j)->Word()=="(")
            NbBrackets++;
          if(NbBrackets>0)
            TmpSeq->Append(aSeq->Value(j));
          else
            break;
        }
        tok1 = CalculateLocal(TmpSeq);
        aSeq->Remove(i+1,j);
        aSeq->SetValue(i,tok1);
        break;
      }
    }
    if(i>aSeq->Length()) {
      IsBracket = Standard_False;
      // there are not brackets => make calculations
      // step 1: operation '**'
      for(i=1; i<=aSeq->Length(); i++) {
        if(aSeq->Value(i)->Word()=="**") {
          tok1 = aSeq->Value(i-1);
          tok2 = aSeq->Value(i+1);
          tok1 = pow(tok1,tok2);
          aSeq->Remove(i);
          aSeq->Remove(i);
          aSeq->SetValue(i-1,tok1);
          i--;
        }
      }
      // step 2: operation '*', '.' and '/'
      for(i=1; i<=aSeq->Length(); i++) {
        if(aSeq->Value(i)->Mean()=="O") {
          tok1 = aSeq->Value(i-1);
          tok2 = aSeq->Value(i+1);
          if(aSeq->Value(i)->Word()=="/")
            tok1 = tok1 / tok2;
          else
            tok1 = tok1 * tok2;
          aSeq->Remove(i);
          aSeq->Remove(i);
          aSeq->SetValue(i-1,tok1);
          i--;
        }
      }
      // now aSeq have to include only one result token
    }
  }
  return tok1;
}


//=======================================================================
//function : Evaluate
//purpose  : 
//=======================================================================

Handle(Units_Token) Units_Sentence::Evaluate()
{
  Handle(Units_Token) rtoken,ktoken;
  if(thesequenceoftokens->Length()==0)
    return rtoken;

/* old variant
  Standard_Integer index;
  static char *ooper[6] = { "+", "-", "*", ".", "/", "**" };
  Standard_Integer numterm,i,j,k,maxlevel,curlevel,level[255];
  Handle(Units_Token) currenttoken;
  TCollection_AsciiString string;
  TCollection_AsciiString mean;
  TCollection_AsciiString oper;
  
  numterm=curlevel=i=0;
  for(index=1;index<=thesequenceoftokens->Length();index++) {
    string = thesequenceoftokens->Value(index)->Word();

    if( string=="(" ) {
      level[numterm]=curlevel;
      curlevel++;
    }
    else if( string==")" ) {
      curlevel--;
      level[numterm]=curlevel;
    }
    else {
      level[numterm]=curlevel;
    }
    numterm++;
  }

  //if(IPrint==1) {
  //  cout<<endl;
  //  for(index=1; index<=thesequenceoftokens->Length(); index++) {
  //    Handle(Units_Token) tok = thesequenceoftokens->Value(index);
  //    cout<<tok->Word()<<" ";
  //  }
  //  cout<<endl;
  //  for(index=1; index<=thesequenceoftokens->Length(); index++) {
  //    cout<<level[index-1]<<" ";
  //  }
  //  cout<<endl;
  //}

  if(curlevel) {
    cout<<" Incorrect number of brackets"<<endl;
    return new Units_Token();
  }

  for(k=0; k<6; k++) {
    for(index=1,i=0; index<=thesequenceoftokens->Length(); index++,i++)	{
      if( thesequenceoftokens->Value(index)->Word() == ooper[k] ) {
        j=i+1;
        curlevel=level[j];
        while(j < numterm && level[j] >= curlevel)
          level[j++]++;
        j=i-1;
        curlevel=level[j];
        while(j >= 0 && level[j] >= curlevel)
          level[j--]++;
      }
    }
  }

  Handle(Units_Dimensions) aDim;

  numterm--;
  while(numterm>0) {

    maxlevel=0;
    for(i=0; i<=numterm; i++)if(level[i] > maxlevel)
      maxlevel=level[i];

    i=0;
    while(level[i] != maxlevel)
      i++;

    oper.Clear();

    k=i;
    while( k<=numterm && level[k]==maxlevel ) {
      ktoken=thesequenceoftokens->Value(k+1);
      mean=ktoken->Mean();
      if(mean=="O") {
        oper = ktoken->Word();
      }
      else if(mean!="S") {
        if     (oper == "/")  rtoken = rtoken / ktoken;
        else if(oper == "+")  rtoken = rtoken + ktoken;
        else if(oper == "-")  rtoken = rtoken - ktoken;
        else if(oper == "*")  rtoken = rtoken * ktoken;
        else if(oper == "**") rtoken = pow(rtoken,ktoken);
        else if(oper == ".")  rtoken = rtoken * ktoken;
        else                  rtoken = ktoken->Creates();
      }
      k++;
    }
    thesequenceoftokens->SetValue(i+1,rtoken);
    level[i]=maxlevel-1;

    if( (i+1) != k ) {
      thesequenceoftokens->Remove(i+2,k);
      j=i;
      while(k <= numterm)
        level[++j]=level[k++];
      numterm=j;
    }
  }

  rtoken = thesequenceoftokens->Value(1)->Creates();

  if( rtoken->Value()==0. ) {
    cout << "*BAD token value from unit '" << rtoken->Word() << "'" << endl;
    rtoken->Value(1.);
  }
*/ // end old variant

  // variant skl 15.09.2005
  rtoken = CalculateLocal(thesequenceoftokens);

  //rtoken->Dump(0,1);

  return rtoken;
}
