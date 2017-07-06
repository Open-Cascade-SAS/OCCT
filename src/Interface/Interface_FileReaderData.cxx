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

//====================================================================
//#10 smh 22.12.99 Protection (case of unexisting directory entry in file)
//sln 21.01.2002 OCC133: Exception handling was added in method Interface_FileReaderData::BoundEntity
//====================================================================

#include <Interface_FileParameter.hxx>
#include <Interface_FileReaderData.hxx>
#include <Interface_ParamList.hxx>
#include <Interface_ParamSet.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_FileReaderData,Standard_Transient)

//  Stoque les Donnees issues d un Fichier (Conservees sous forme Litterale)
//  Chaque norme peut s en servir comme base (listes de parametres litteraux,
//  entites associees) et y ajoute ses donnees propres.
//  Travaille sous le controle de FileReaderTool
//  Optimisation : Champs pas possibles, car Param est const. Dommage
//  Donc, on suppose qu on lit un fichier a la fois (hypothese raisonnable)
//  On note en champ un numero de fichier, par rapport auquel on optimise
static Standard_Integer thefic = 0;
static Standard_Integer thenm0 = -1;
static Standard_Integer thenp0 = -1;


Interface_FileReaderData::Interface_FileReaderData (const Standard_Integer nbr,
						    const Standard_Integer npar)
     : therrload (0), thenumpar (0,nbr), theents (0,nbr)
{
  theparams = new Interface_ParamSet (npar);
  thenumpar.Init(0);
  thenm0 = -1;
  thenum0 = ++thefic;
}

    Standard_Integer Interface_FileReaderData::NbRecords () const
      {  return thenumpar.Upper();  }

    Standard_Integer Interface_FileReaderData::NbEntities () const
{
  Standard_Integer nb = 0; Standard_Integer num = 0;
  while ( (num = FindNextRecord(num)) > 0) nb ++;
  return nb;
}


//  ....            Gestion des Parametres attaches aux Records            ....

    void Interface_FileReaderData::InitParams (const Standard_Integer num)
{
  thenumpar.SetValue (num,theparams->NbParams());
}

    void Interface_FileReaderData::AddParam
  (const Standard_Integer /*num*/,
   const Standard_CString aval, const Interface_ParamType atype,
   const Standard_Integer nument)
{
  theparams->Append(aval,-1,atype,nument);
}

    void Interface_FileReaderData::AddParam
  (const Standard_Integer /*num*/,
   const TCollection_AsciiString& aval, const Interface_ParamType atype,
   const Standard_Integer nument)
{
  theparams->Append(aval.ToCString(),aval.Length(),atype,nument);
}

    void Interface_FileReaderData::AddParam
  (const Standard_Integer /*num*/,
   const Interface_FileParameter& FP)
{
  theparams->Append(FP);
}


    void Interface_FileReaderData::SetParam
  (const Standard_Integer num, const Standard_Integer nump,
   const Interface_FileParameter& FP)
{
    theparams->SetParam(thenumpar(num-1)+nump,FP);
}

    Standard_Integer Interface_FileReaderData::NbParams
  (const Standard_Integer num) const
{
  if (num > 1) return (thenumpar(num) - thenumpar(num-1));
  else if(num ==1) return thenumpar(num);
  else return theparams->NbParams();
}

    Handle(Interface_ParamList) Interface_FileReaderData::Params
  (const Standard_Integer num) const
{
  if (num == 0) return theparams->Params(0,0);  // complet
  else if(num ==1) return theparams->Params(0,thenumpar(1));
  else return theparams->Params ( thenumpar(num-1)+1, (thenumpar(num) - thenumpar(num-1)) );
}

    const Interface_FileParameter& Interface_FileReaderData::Param
  (const Standard_Integer num, const Standard_Integer nump) const
{
  if (thefic != thenum0) return theparams->Param(thenumpar(num-1)+nump);
  if (thenm0 != num) {  thenp0 = thenumpar(num-1);  thenm0 = num;  }
  return theparams->Param (thenp0+nump);
}

    Interface_FileParameter& Interface_FileReaderData::ChangeParam
  (const Standard_Integer num, const Standard_Integer nump)
{
  if (thefic != thenum0) return theparams->ChangeParam(thenumpar(num-1)+nump);
  if (thenm0 != num) {  thenp0 = thenumpar(num-1);  thenm0 = num;  }
  return theparams->ChangeParam (thenp0+nump);
}

    Interface_ParamType Interface_FileReaderData::ParamType
  (const Standard_Integer num, const Standard_Integer nump) const
      {  return Param(num,nump).ParamType();  }

    Standard_CString  Interface_FileReaderData::ParamCValue
  (const Standard_Integer num, const Standard_Integer nump) const
      {  return Param(num,nump).CValue();  }


    Standard_Boolean Interface_FileReaderData::IsParamDefined
  (const Standard_Integer num, const Standard_Integer nump) const
      {  return (Param(num,nump).ParamType() != Interface_ParamVoid);  }

    Standard_Integer Interface_FileReaderData::ParamNumber
  (const Standard_Integer num, const Standard_Integer nump) const
      {  return Param(num,nump).EntityNumber();  }

    const Handle(Standard_Transient)& Interface_FileReaderData::ParamEntity
  (const Standard_Integer num, const Standard_Integer nump) const
      {  return BoundEntity (Param(num,nump).EntityNumber());  }

    Interface_FileParameter& Interface_FileReaderData::ChangeParameter
  (const Standard_Integer numpar)
      {  return theparams->ChangeParam (numpar);  }

    void  Interface_FileReaderData::ParamPosition
  (const Standard_Integer numpar,
   Standard_Integer& num, Standard_Integer& nump) const
{
  Standard_Integer nbe = thenumpar.Upper();
  if (numpar <= 0) {  num = nump = 0;  return;  }
  for (Standard_Integer i = 1; i <= nbe; i ++) {
    if (thenumpar(i) > numpar)
      {  num = i;  nump = numpar - thenumpar(i) +1;  return;  }
  }
  num = nbe;  nump = numpar - thenumpar(nbe) +1;
}

    Standard_Integer Interface_FileReaderData::ParamFirstRank
  (const Standard_Integer num) const
      {  return thenumpar(num);  }

    void  Interface_FileReaderData::SetErrorLoad (const Standard_Boolean val)
      {  therrload = (val ? 1 : -1);  }

    Standard_Boolean  Interface_FileReaderData::IsErrorLoad () const
      {  return (therrload != 0);  }

    Standard_Boolean  Interface_FileReaderData::ResetErrorLoad ()
      {  Standard_Boolean res = (therrload > 0); therrload = 0;  return res;  }

//  ....        Gestion des Entites Associees aux Donnees du Fichier       ....


const Handle(Standard_Transient)& Interface_FileReaderData::BoundEntity
       (const Standard_Integer num) const
       //      {  return theents(num);  }
{
  if (num >= theents.Lower() && num <= theents.Upper()) {
    return theents(num);
  }
  else {
    static Handle(Standard_Transient) dummy;
    return dummy;
  }
}
/*  //static Handle(Standard_Transient) dummy;
  {
  //smh#10 Protection. If iges entity does not exist, return null pointer.
    try {
      OCC_CATCH_SIGNALS
      Handle(Standard_Transient) temp = theents.Value(num);
    }
  ////sln 21.01.2002 OCC133: Exception handling
 // catch (Standard_OutOfRange) {
 //   cout<<" Catch of sln"<<endl;

 //   return dummy;
 // }
    catch (Standard_Failure) {

    // some work-around, the best would be to modify CDL to
    // return "Handle(Standard_Transient)" not "const Handle(Standard_Transient)&"
      static Handle(Standard_Transient) dummy;
     // cout<<" Catch of smh"<<endl;
    return dummy;
    }
  }
   //cout<<" Normal"<<endl;
  if (theents.Value(num).IsImmutable()) cout << "IMMUTABLE:"<<num<<endl;
  return theents(num);
}
*/

void Interface_FileReaderData::BindEntity
   (const Standard_Integer num, const Handle(Standard_Transient)& ent)
//      {  theents.SetValue(num,ent);  }
{
//  #ifdef OCCT_DEBUG
//    if (ent.IsImmutable())
//      cout << "Bind IMMUTABLE:"<<num<<endl;
//  #endif
  theents.SetValue(num,ent);
}

void Interface_FileReaderData::Destroy ()
{
}


#define MAXCHIF 80

static const Standard_Real vtab[] =
{ 1.,     2.,     3.,     4., 5., 6., 7.,     8., 9.,
  10.,    20.,    30.,    40., 50., 60., 70.,    80., 90.,
  100.,   200.,   300.,   400., 500., 600., 700.,   800., 900.,
  1000.,  2000.,  3000.,  4000., 5000., 6000., 7000.,  8000., 9000.,
  10000., 20000., 30000., 40000., 50000., 60000., 70000., 80000., 90000.,
  1.E+05, 2.E+05, 3.E+05, 4.E+05, 5.E+05, 6.E+05, 7.E+05, 8.E+05, 9.E+05,
  1.E+06, 2.E+06, 3.E+06, 4.E+06, 5.E+06, 6.E+06, 7.E+06, 8.E+06, 9.E+06,
  1.E+07, 2.E+07, 3.E+07, 4.E+07, 5.E+07, 6.E+07, 7.E+07, 8.E+07, 9.E+07,
  1.E+08, 2.E+08, 3.E+08, 4.E+08, 5.E+08, 6.E+08, 7.E+08, 8.E+08, 9.E+08,
  1.E+09, 2.E+09, 3.E+09, 4.E+09, 5.E+09, 6.E+09, 7.E+09, 8.E+09, 9.E+09,
  1.E+10, 2.E+10, 3.E+10, 4.E+10, 5.E+10, 6.E+10, 7.E+10, 8.E+10, 9.E+10,
  1.E+11, 2.E+11, 3.E+11, 4.E+11, 5.E+11, 6.E+11, 7.E+11, 8.E+11, 9.E+11,
  1.E+12, 2.E+12, 3.E+12, 4.E+12, 5.E+12, 6.E+12, 7.E+12, 8.E+12, 9.E+12,
  1.E+13, 2.E+13, 3.E+13, 4.E+13, 5.E+13, 6.E+13, 7.E+13, 8.E+13, 9.E+13,
  1.E+14, 2.E+14, 3.E+14, 4.E+14, 5.E+14, 6.E+14, 7.E+14, 8.E+14, 9.E+14,
  1.E+15, 2.E+15, 3.E+15, 4.E+15, 5.E+15, 6.E+15, 7.E+15, 8.E+15, 9.E+15,
  1.E+16, 2.E+16, 3.E+16, 4.E+16, 5.E+16, 6.E+16, 7.E+16, 8.E+16, 9.E+16,
  1.E+17, 2.E+17, 3.E+17, 4.E+17, 5.E+17, 6.E+17, 7.E+17, 8.E+17, 9.E+17,
  1.E+18, 2.E+18, 3.E+18, 4.E+18, 5.E+18, 6.E+18, 7.E+18, 8.E+18, 9.E+18,
  1.E+19, 2.E+19, 3.E+19, 4.E+19, 5.E+19, 6.E+19, 7.E+19, 8.E+19, 9.E+19,
  1.E+20, 2.E+20, 3.E+20, 4.E+20, 5.E+20, 6.E+20, 7.E+20, 8.E+20, 9.E+20,
  1.E+21, 2.E+21, 3.E+21, 4.E+21, 5.E+21, 6.E+21, 7.E+21, 8.E+21, 9.E+21,
  1.E+22, 2.E+22, 3.E+22, 4.E+22, 5.E+22, 6.E+22, 7.E+22, 8.E+22, 9.E+22,
  1.E+23, 2.E+23, 3.E+23, 4.E+23, 5.E+23, 6.E+23, 7.E+23, 8.E+23, 9.E+23,
  1.E+24, 2.E+24, 3.E+24, 4.E+24, 5.E+24, 6.E+24, 7.E+24, 8.E+24, 9.E+24,
  1.E+25, 2.E+25, 3.E+25, 4.E+25, 5.E+25, 6.E+25, 7.E+25, 8.E+25, 9.E+25,
  1.E+26, 2.E+26, 3.E+26, 4.E+26, 5.E+26, 6.E+26, 7.E+26, 8.E+26, 9.E+26,
  1.E+27, 2.E+27, 3.E+27, 4.E+27, 5.E+27, 6.E+27, 7.E+27, 8.E+27, 9.E+27,
  1.E+28, 2.E+28, 3.E+28, 4.E+28, 5.E+28, 6.E+28, 7.E+28, 8.E+28, 9.E+28,
  1.E+29, 2.E+29, 3.E+29, 4.E+29, 5.E+29, 6.E+29, 7.E+29, 8.E+29, 9.E+29,
  1.E+30, 2.E+30, 3.E+30, 4.E+30, 5.E+30, 6.E+30, 7.E+30, 8.E+30, 9.E+30,
  1.E+31, 2.E+31, 3.E+31, 4.E+31, 5.E+31, 6.E+31, 7.E+31, 8.E+31, 9.E+31,
  1.E+32, 2.E+32, 3.E+32, 4.E+32, 5.E+32, 6.E+32, 7.E+32, 8.E+32, 9.E+32,
  1.E+33, 2.E+33, 3.E+33, 4.E+33, 5.E+33, 6.E+33, 7.E+33, 8.E+33, 9.E+33,
  1.E+34, 2.E+34, 3.E+34, 4.E+34, 5.E+34, 6.E+34, 7.E+34, 8.E+34, 9.E+34,
  1.E+35, 2.E+35, 3.E+35, 4.E+35, 5.E+35, 6.E+35, 7.E+35, 8.E+35, 9.E+35,
  1.E+36, 2.E+36, 3.E+36, 4.E+36, 5.E+36, 6.E+36, 7.E+36, 8.E+36, 9.E+36,
  1.E+37, 2.E+37, 3.E+37, 4.E+37, 5.E+37, 6.E+37, 7.E+37, 8.E+37, 9.E+37,
  1.E+38, 2.E+38, 3.E+38, 4.E+38, 5.E+38, 6.E+38, 7.E+38, 8.E+38, 9.E+38,
  1.E+39, 2.E+39, 3.E+39, 4.E+39, 5.E+39, 6.E+39, 7.E+39, 8.E+39, 9.E+39,
  1.E+40, 2.E+40, 3.E+40, 4.E+40, 5.E+40, 6.E+40, 7.E+40, 8.E+40, 9.E+40,
  1.E+41, 2.E+41, 3.E+41, 4.E+41, 5.E+41, 6.E+41, 7.E+41, 8.E+41, 9.E+41,
  1.E+42, 2.E+42, 3.E+42, 4.E+42, 5.E+42, 6.E+42, 7.E+42, 8.E+42, 9.E+42,
  1.E+43, 2.E+43, 3.E+43, 4.E+43, 5.E+43, 6.E+43, 7.E+43, 8.E+43, 9.E+43,
  1.E+44, 2.E+44, 3.E+44, 4.E+44, 5.E+44, 6.E+44, 7.E+44, 8.E+44, 9.E+44,
  1.E+45, 2.E+45, 3.E+45, 4.E+45, 5.E+45, 6.E+45, 7.E+45, 8.E+45, 9.E+45,
  1.E+46, 2.E+46, 3.E+46, 4.E+46, 5.E+46, 6.E+46, 7.E+46, 8.E+46, 9.E+46,
  1.E+47, 2.E+47, 3.E+47, 4.E+47, 5.E+47, 6.E+47, 7.E+47, 8.E+47, 9.E+47,
  1.E+48, 2.E+48, 3.E+48, 4.E+48, 5.E+48, 6.E+48, 7.E+48, 8.E+48, 9.E+48,
  1.E+49, 2.E+49, 3.E+49, 4.E+49, 5.E+49, 6.E+49, 7.E+49, 8.E+49, 9.E+49,
  1.E+50, 2.E+50, 3.E+50, 4.E+50, 5.E+50, 6.E+50, 7.E+50, 8.E+50, 9.E+50,
  1.E+51, 2.E+51, 3.E+51, 4.E+51, 5.E+51, 6.E+51, 7.E+51, 8.E+51, 9.E+51,
  1.E+52, 2.E+52, 3.E+52, 4.E+52, 5.E+52, 6.E+52, 7.E+52, 8.E+52, 9.E+52,
  1.E+53, 2.E+53, 3.E+53, 4.E+53, 5.E+53, 6.E+53, 7.E+53, 8.E+53, 9.E+53,
  1.E+54, 2.E+54, 3.E+54, 4.E+54, 5.E+54, 6.E+54, 7.E+54, 8.E+54, 9.E+54,
  1.E+55, 2.E+55, 3.E+55, 4.E+55, 5.E+55, 6.E+55, 7.E+55, 8.E+55, 9.E+55,
  1.E+56, 2.E+56, 3.E+56, 4.E+56, 5.E+56, 6.E+56, 7.E+56, 8.E+56, 9.E+56,
  1.E+57, 2.E+57, 3.E+57, 4.E+57, 5.E+57, 6.E+57, 7.E+57, 8.E+57, 9.E+57,
  1.E+58, 2.E+58, 3.E+58, 4.E+58, 5.E+58, 6.E+58, 7.E+58, 8.E+58, 9.E+58,
  1.E+59, 2.E+59, 3.E+59, 4.E+59, 5.E+59, 6.E+59, 7.E+59, 8.E+59, 9.E+59,
  1.E+60, 2.E+60, 3.E+60, 4.E+60, 5.E+60, 6.E+60, 7.E+60, 8.E+60, 9.E+60,
  1.E+61, 2.E+61, 3.E+61, 4.E+61, 5.E+61, 6.E+61, 7.E+61, 8.E+61, 9.E+61,
  1.E+62, 2.E+62, 3.E+62, 4.E+62, 5.E+62, 6.E+62, 7.E+62, 8.E+62, 9.E+62,
  1.E+63, 2.E+63, 3.E+63, 4.E+63, 5.E+63, 6.E+63, 7.E+63, 8.E+63, 9.E+63,
  1.E+64, 2.E+64, 3.E+64, 4.E+64, 5.E+64, 6.E+64, 7.E+64, 8.E+64, 9.E+64,
  1.E+65, 2.E+65, 3.E+65, 4.E+65, 5.E+65, 6.E+65, 7.E+65, 8.E+65, 9.E+65,
  1.E+66, 2.E+66, 3.E+66, 4.E+66, 5.E+66, 6.E+66, 7.E+66, 8.E+66, 9.E+66,
  1.E+67, 2.E+67, 3.E+67, 4.E+67, 5.E+67, 6.E+67, 7.E+67, 8.E+67, 9.E+67,
  1.E+68, 2.E+68, 3.E+68, 4.E+68, 5.E+68, 6.E+68, 7.E+68, 8.E+68, 9.E+68,
  1.E+69, 2.E+69, 3.E+69, 4.E+69, 5.E+69, 6.E+69, 7.E+69, 8.E+69, 9.E+69,
  1.E+70, 2.E+70, 3.E+70, 4.E+70, 5.E+70, 6.E+70, 7.E+70, 8.E+70, 9.E+70,
  1.E+71, 2.E+71, 3.E+71, 4.E+71, 5.E+71, 6.E+71, 7.E+71, 8.E+71, 9.E+71,
  1.E+72, 2.E+72, 3.E+72, 4.E+72, 5.E+72, 6.E+72, 7.E+72, 8.E+72, 9.E+72,
  1.E+73, 2.E+73, 3.E+73, 4.E+73, 5.E+73, 6.E+73, 7.E+73, 8.E+73, 9.E+73,
  1.E+74, 2.E+74, 3.E+74, 4.E+74, 5.E+74, 6.E+74, 7.E+74, 8.E+74, 9.E+74,
  1.E+75, 2.E+75, 3.E+75, 4.E+75, 5.E+75, 6.E+75, 7.E+75, 8.E+75, 9.E+75,
  1.E+76, 2.E+76, 3.E+76, 4.E+76, 5.E+76, 6.E+76, 7.E+76, 8.E+76, 9.E+76,
  1.E+77, 2.E+77, 3.E+77, 4.E+77, 5.E+77, 6.E+77, 7.E+77, 8.E+77, 9.E+77,
  1.E+78, 2.E+78, 3.E+78, 4.E+78, 5.E+78, 6.E+78, 7.E+78, 8.E+78, 9.E+78,
  1.E+79, 2.E+79, 3.E+79, 4.E+79, 5.E+79, 6.E+79, 7.E+79, 8.E+79, 9.E+79,
  1.E+80, 2.E+80, 3.E+80, 4.E+80, 5.E+80, 6.E+80, 7.E+80, 8.E+80, 9.E+80,
  1.E+81, 2.E+81, 3.E+81, 4.E+81, 5.E+81, 6.E+81, 7.E+81, 8.E+81, 9.E+81,
  1.E+82, 2.E+82, 3.E+82, 4.E+82, 5.E+82, 6.E+82, 7.E+82, 8.E+82, 9.E+82,
  1.E+83, 2.E+83, 3.E+83, 4.E+83, 5.E+83, 6.E+83, 7.E+83, 8.E+83, 9.E+83,
  1.E+84, 2.E+84, 3.E+84, 4.E+84, 5.E+84, 6.E+84, 7.E+84, 8.E+84, 9.E+84,
  1.E+85, 2.E+85, 3.E+85, 4.E+85, 5.E+85, 6.E+85, 7.E+85, 8.E+85, 9.E+85,
  1.E+86, 2.E+86, 3.E+86, 4.E+86, 5.E+86, 6.E+86, 7.E+86, 8.E+86, 9.E+86,
  1.E+87, 2.E+87, 3.E+87, 4.E+87, 5.E+87, 6.E+87, 7.E+87, 8.E+87, 9.E+87,
  1.E+88, 2.E+88, 3.E+88, 4.E+88, 5.E+88, 6.E+88, 7.E+88, 8.E+88, 9.E+88,
  1.E+89, 2.E+89, 3.E+89, 4.E+89, 5.E+89, 6.E+89, 7.E+89, 8.E+89, 9.E+89,
  1.E+90, 2.E+90, 3.E+90, 4.E+90, 5.E+90, 6.E+90, 7.E+90, 8.E+90, 9.E+90,
  1.E+91, 2.E+91, 3.E+91, 4.E+91, 5.E+91, 6.E+91, 7.E+91, 8.E+91, 9.E+91,
  1.E+92, 2.E+92, 3.E+92, 4.E+92, 5.E+92, 6.E+92, 7.E+92, 8.E+92, 9.E+92,
  1.E+93, 2.E+93, 3.E+93, 4.E+93, 5.E+93, 6.E+93, 7.E+93, 8.E+93, 9.E+93,
  1.E+94, 2.E+94, 3.E+94, 4.E+94, 5.E+94, 6.E+94, 7.E+94, 8.E+94, 9.E+94,
  1.E+95, 2.E+95, 3.E+95, 4.E+95, 5.E+95, 6.E+95, 7.E+95, 8.E+95, 9.E+95,
  1.E+96, 2.E+96, 3.E+96, 4.E+96, 5.E+96, 6.E+96, 7.E+96, 8.E+96, 9.E+96,
  1.E+97, 2.E+97, 3.E+97, 4.E+97, 5.E+97, 6.E+97, 7.E+97, 8.E+97, 9.E+97,
  1.E+98, 2.E+98, 3.E+98, 4.E+98, 5.E+98, 6.E+98, 7.E+98, 8.E+98, 9.E+98,
  1.E+99, 2.E+99, 3.E+99, 4.E+99, 5.E+99, 6.E+99, 7.E+99, 8.E+99, 9.E+99 };


    Standard_Real Interface_FileReaderData::Fastof (const Standard_CString ligne)
{
  Standard_Real val = 0., valdiv;
  int i, jj, j, k, je, jx;
  int chiffre[MAXCHIF];  int point, prem, moins, exp, grexp;
  j = jj = je = jx = 0;
  moins = exp = grexp = 0;
  prem = MAXCHIF + 1;  point = -1;  //   ligne[MAXCHIF+1] = '\0';  fin forcee

/*  Analyse de la ligne (reputee representer un Flottant)  */
  for (i = 0; ; i ++) // critere d arret : cf '\0'
  {
    const char& car = ligne[i];
/* critere d arret : ne pas l oublier, mais eviter de freiner le traitement
   DONC, une fois en fin, plus une fois pour 1er passage
    if (car == '\0')
      {  if (point < 0) {  point = i; jx = point - prem; };  break;  }   */

    if (prem > MAXCHIF)
    {
      if (car == '0') continue;
      if (car == '.') {  point = i;  continue;  }
      if (car == '+') continue;
      if (car == '-') {  moins = 1;  continue;  }
      if (car == '\0')
      {
        if (point < 0)
        {
          point = i;
          jx = point - prem;
        }
        break;
      }
      if (car < 33)
      {
        continue;
      }
      prem = i;
      if (point >= 0)
      {
        jx = point - prem + 1;
      }
    }
    if (car > 48 && car <= 57)
    {
      chiffre[j] = car - 48;
      jj = ++j; // j++ puis jj = j
      continue;
    }
    if (car == '0') {  chiffre[j] = 0;  j ++;  continue;  }
    if (car == '.') {  point = i;  jx = point - prem;  continue;  }
    if ((car & 94) == 68) { // prend  : e E d D
      je = i;     exp = atoi(&ligne[i+1]);     jx += exp;
      if (exp < -100 || exp > 100)
      {
        grexp = 1;
      }
      break;
    }
    if (car == '\0')
    {
      if (point < 0)
      {
        point = i;
        jx = point - prem;
      }
      break;
    }
  }

/*  Interpretation  : on decale le Point Decimal selon l Exposant
    Puis on considere par rapport a <jj>, <prem> :
    chiffre[i, pour i <  point - prem] : partie entiere
    chiffre[i, pour i >= point - prem] : partie decimale
*/

//  jx = (point < prem ? point - prem + 1 : point - prem) + exp;  deja calcule
//        Puissance de 10 premiere decimale <0>
//  limite : le PLUS PETIT SIGNIFIANT doit etre >= 1e-99
  je = jx*9;
  if (jx > jj) {
    valdiv = 1.;
    if (jx >= 100 || grexp) return Atof(ligne);  // issue de secours
  } else {
    k = jj*9;
    if (jj >= 100 || grexp || (k-je >= 900)) return Atof(ligne);  // issue de secours
    valdiv = vtab[k-je];
    je = k;
  }

  je --;
//  val = 0.;    mis a la creation
  for (i = 0; i < jj; i ++) {
    je -= 9;
    const int& chif = chiffre[i];
    if (chif != 0) val += vtab[chif+je];
  }
  if (valdiv != 1.) val = val/valdiv;
  if (moins) val = -val;
  return val;
}
