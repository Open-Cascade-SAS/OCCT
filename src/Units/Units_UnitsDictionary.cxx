// Created on: 1992-06-24
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


#define PRO13136	//GG_090498
//			Ne pas utiliser la fonction systeme atof() qui
//			depend de la localisation

#include <Units_UnitsDictionary.ixx>
#include <Units.hxx>

#include <Standard_Stream.hxx>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#if defined (HAVE_SYS_STAT_H) || defined(WNT)
# include <sys/stat.h>
#endif

#include <Units_Token.hxx>
#include <Units_TokensSequence.hxx>
#include <Units_UnitsSequence.hxx>
#include <Units_Unit.hxx>
#include <Units_ShiftedUnit.hxx>
#include <Units_Dimensions.hxx>
#include <Units_QuantitiesSequence.hxx>
#include <Units_Quantity.hxx>
#include <Units_MathSentence.hxx>
#include <Units_UnitSentence.hxx>
#include <Units_UnitsLexicon.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Units_Operators.hxx>
#include <OSD.hxx>

#ifdef PRO13136		//Temporaire en attendant OSD::CStringToReal()
static char DecimalPoint = '\0' ;

static void GetDecimalPoint() {
  Standard_Real f = 1.1 ;
  char str[5] ;

  sprintf(str,"%.1f",f) ;
//#ifdef DEB
//  printf("Local System is  %s\n",str) ;
//#endif
  DecimalPoint = str[1] ;
}
#endif
// Make the RealToCString reciprocal conversion.

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

void Units_UnitsDictionary::Creates(const Standard_CString afilename)
{
  Standard_Boolean ismove,emptyline;
  Standard_Integer charnumber,unitscomputed;
  Standard_Integer fr,i,j,k;
  char MM[11],LL[11],TT[11],II[11],tt[11],NN[11],JJ[11],PP[11],SS[11];
  Standard_Real M=0,L=0,T=0,I=0,t=0,N=0,J=0,P=0,S=0;
  char unite[52];
  char symbol[28],unit2[28];
  char line[256],convert[256];
  //char name[81];
  //char name[80]; //skl
  char name[41]; // skl for OCC13438
#if 0
  char *Unite = unite ;
  char *Symbol = symbol ;
  char *Unit2 = unit2 ;
  char *Line = line ;
  char *Convert = convert ;
  char *Name = name ;
#endif
  Standard_Real matrix[50][50], coeff=0, move=0;
  Handle(Units_Token) token;
  Handle(Units_UnitsSequence) theunitssequence;
  Handle(Units_Unit) unit;
  Handle(Units_ShiftedUnit) shiftedunit;
  Handle(Units_Dimensions) dimensions;
  Handle(Units_Quantity) quantity;
  
  struct stat buf;

  unite[51] = '\0' ;
  symbol[27] = '\0' ;
  unit2[27] = '\0' ;
  line[255] = '\0' ;
  convert[255] = '\0' ;
  //name[80]  = '\0' ;
  name[40]  = '\0' ; // skl for OCC13438

  ifstream file(afilename, ios::in);

  if(!file) {
    cout<<"unable to open "<<afilename<<" for input"<<endl;
    return;
  }
  
  thefilename = new TCollection_HAsciiString(afilename);

  if(!stat(afilename,&buf)) thetime = (Standard_Integer)buf.st_ctime;

  thequantitiessequence = new Units_QuantitiesSequence();
  
  Standard_Integer numberofunits = 0;
  
  for(;;) {
    file.getline(line,255);
    if (!file)
      break;
    fr = strlen(line);
    if(fr <= 1)
      continue;
    //if( !file || line[0] == '.') { //skl
    if(line[0]=='.') {
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
	  
      //if(!file) break; //skl
      file.getline(line,255);
      file.getline(line,255);
      fr = strlen(line);
	  
#if 0 // skl for OCC13438
      //for(i=0; i<80; i++)name[i] = 0;
      for(i=0; i<41; i++) name[i] = 0;
      M = L = T = I = t = N = J = P = S = 0.;
      //fr = sscanf(line,"%80c%d%d%d%d%d%d%d%d%d",
      fr = sscanf(line,"%40c%f%f%f%f%f%f%f%f%f",
                  &name,&M,&L,&T,&I,&t,&N,&J,&P,&S);
      cout << "UnitsDictionarysscanf(%40c)Name" << Name << endl ;
#else
      for(i=0; i<11; i++)MM[i] = 0;
      for(i=0; i<11; i++)LL[i] = 0;
      for(i=0; i<11; i++)TT[i] = 0;
      for(i=0; i<11; i++)II[i] = 0;
      for(i=0; i<11; i++)tt[i] = 0;
      for(i=0; i<11; i++)NN[i] = 0;
      for(i=0; i<11; i++)JJ[i] = 0;
      for(i=0; i<11; i++)PP[i] = 0;
      for(i=0; i<11; i++)SS[i] = 0;

      fr = sscanf(line,"%40c%10c%10c%10c%10c%10c%10c%10c%10c%10c",
		  name,MM,LL,TT,II,tt,NN,JJ,PP,SS);
      OSD::CStringToReal(MM, M);
      OSD::CStringToReal(LL, L);
      OSD::CStringToReal(TT, T);
      OSD::CStringToReal(II, I);
      OSD::CStringToReal(tt, t);
      OSD::CStringToReal(NN, N);
      OSD::CStringToReal(JJ, J);
      OSD::CStringToReal(PP, P);
      OSD::CStringToReal(SS, S);
      //for ( i = 0 ; i < 80 ; i++ ) {
      //  fr = sscanf(&line[i],"%c",&name[i]);
      //}
# if 0
      cout << "UnitsDictionarysscanf(%c  )Name" << Name << endl ;
# endif
      //fr = sscanf(&line[80],"%d%d%d%d%d%d%d%d%d",
      //            &M,&L,&T,&I,&t,&N,&J,&P,&S);
#endif

#ifdef DEB
      /*cout << " Name of Dimension : " << name << endl ;
	  cout << MM << " " << LL << " " << TT << " " 
		   << II << " " << tt << " " << NN << " " 
		   << JJ << " " << PP << " " << SS << endl;
	  cout << M << " " << L << " " << T << " " 
		   << I << " " << t << " " << N << " " 
		   << J << " " << P << " " << S << endl;*/
#endif

      i = 39;
      while( i >= 0 && ( name[i] == ' ' || !name[i]))
        name[i--] = 0;
      dimensions = new Units_Dimensions(M,L,T,I,t,N,J,P,S);
      
      numberofunits = 0;
      theunitssequence = new Units_UnitsSequence();
      quantity = new Units_Quantity(name,dimensions,theunitssequence);
      thequantitiessequence->Append(quantity);
	  
      for(i=0; i<50; i++) {
        for(j=0; j<50; j++)
          matrix[i][j] = 0.;
      }
      file.getline(line,255);
    }

    else { // line[0] != '.'
#if 0
      for(i=0; i<51; i++)
        unite  [i] = 0;
      for(i=0; i<27; i++)
        symbol [i] = 0;
      for(i=0; i<27; i++)
        convert[i] = 0;
      for(i=0; i<27; i++)
        unit2  [i] = 0;
      fr = sscanf(line,"%51c%27c%27c%27c",&unite,&symbol,&convert,&unit2);
      cout << "UnitsDictionarysscanf(%51c%27c%27c%27c)" << endl
           << "Unite" << Unite << endl << "Symbol" << Symbol << endl
           << "Convert" << Convert << endl 
           << "Unit2" << Unit2 << endl ;
#else
      int len = strlen( line ) ;
      for ( i=0 ; i<51 ; i++ ) {
        if ( i<len )
          fr = sscanf(&line[i],"%c",&unite[i]);
        else
          unite[i] = 0 ;
      }
      for ( i=0 ; i<27 ; i++ ) {
        if ( 51+i < len )
          fr = sscanf(&line[51+i],"%c",&symbol[i]);
        else
          symbol[i] = 0 ;
      }
      for ( i=0 ; i<27 ; i++ ) {
        if ( 78+i < len )
          fr = sscanf(&line[78+i],"%c",&convert[i]);
        else
          convert[i] = 0 ;
      }
      for ( i=0 ; i<27 ; i++ ) {
        if ( 105+i < len )
          fr = sscanf(&line[105+i],"%c",&unit2[i]);
        else
          unit2[i] = 0 ;
      }
# if 0
      cout << "UnitsDictionarysscanf(%c              )" << endl
           << "Unite" << Unite << endl << "Symbol" << Symbol << endl
           << "Convert" << Convert << endl 
           << "Unit2" << Unit2 << endl ;
# endif
#endif

      if(fr == -1) continue;

      emptyline = Standard_True;

      i = 50;
      while(i >= 0 && (unite  [i] == ' ' || !unite  [i]))
        unite  [i--] = 0;
      if(i >= 0)
        emptyline = Standard_False;

      i = 26;
      while(i >= 0 && (symbol [i] == ' ' || !symbol [i]))
        symbol [i--] = 0;
      if(i >= 0)
        emptyline = Standard_False;

      i = 26;
      while(i >= 0 && (convert[i] == ' ' || !convert[i]))
        convert[i--] = 0;
      if(i >= 0)
        emptyline = Standard_False;

      i = 26;
      while(i >= 0 && (unit2  [i] == ' ' || !unit2  [i]))
        unit2  [i--] = 0;
      if(i >= 0)
        emptyline = Standard_False;

      if(emptyline) continue;
	  
      if(convert[0] == '[') {
        coeff = 1.;
        i = strlen(convert);
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
        i = strlen(convert);
        convert[i-1] = 0;
        Units_MathSentence mathsentence(&convert[charnumber+1]);
        if(ismove)
          move  = (mathsentence.Evaluate())->Value();
        else
          coeff = (mathsentence.Evaluate())->Value();
      }
      else if(convert[0]) {
#ifdef PRO13136
        if(ismove) {
          OSD::CStringToReal(&convert[charnumber], move);
        }
        else
          OSD::CStringToReal(convert, coeff);
#else
        if(ismove) {
          move = atof(&convert[charnumber]);
        }
        else
          coeff = atof(convert);
#endif
      }
      else {
        coeff = 1.;
      }

      if(ismove) {
        if(move) {
          Standard_Integer last = theunitssequence->Length();
          unit = theunitssequence->Value(last);
          shiftedunit = *(Handle_Units_ShiftedUnit*)&unit;
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
  file.close();
/*
  Handle(Units_TokensSequence) tmpSeq = Units::LexiconUnits(Standard_False)->Sequence();
  for(int ii=1; ii<=tmpSeq->Length(); ii++) {
    token = tmpSeq->Value(ii);
    cout<<"i="<<ii<<"  token:  "<<token->Word().ToCString()<<"   "
      <<token->Mean().ToCString()<<"  "<<token->Value()<<endl;
  }
  cout<<endl;
*/
}


//=======================================================================
//function : UpToDate
//purpose  : 
//=======================================================================

Standard_Boolean Units_UnitsDictionary::UpToDate() const
{
  struct stat buf;
  TCollection_AsciiString string = thefilename->String();
  if(!stat(string.ToCString(),&buf)) {
    if(thetime == buf.st_ctime) return Standard_True;
  }

  return Standard_False;
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
        cout<<" Pas d'unite active pour "<<aquantity<<endl;
        return "";
      }
    }
  }

  cout<<" La grandeur physique "<<aquantity<<" n'existe pas."<<endl;
  return "";
}
