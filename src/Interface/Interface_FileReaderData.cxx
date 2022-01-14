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
 //   std::cout<<" Catch of sln"<<std::endl;

 //   return dummy;
 // }
    catch (Standard_Failure) {

    // some work-around, the best would be to modify CDL to
    // return "Handle(Standard_Transient)" not "const Handle(Standard_Transient)&"
      static Handle(Standard_Transient) dummy;
     // std::cout<<" Catch of smh"<<std::endl;
    return dummy;
    }
  }
   //std::cout<<" Normal"<<std::endl;
  if (theents.Value(num).IsImmutable()) std::cout << "IMMUTABLE:"<<num<<std::endl;
  return theents(num);
}
*/

void Interface_FileReaderData::BindEntity
   (const Standard_Integer num, const Handle(Standard_Transient)& ent)
//      {  theents.SetValue(num,ent);  }
{
//  #ifdef OCCT_DEBUG
//    if (ent.IsImmutable())
//      std::cout << "Bind IMMUTABLE:"<<num<<std::endl;
//  #endif
  theents.SetValue(num,ent);
}

void Interface_FileReaderData::Destroy ()
{
}

Standard_Real Interface_FileReaderData::Fastof (const Standard_CString ligne)
{
  return Strtod (ligne, 0);
}
