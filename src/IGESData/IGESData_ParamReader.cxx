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

//pdn S4135 05.04.99 comment uninitialized Interface_Static::IVal("iges.convert.read");

#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESType.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <Interface_Check.hxx>
#include <Interface_EntityList.hxx>
#include <Interface_FileParameter.hxx>
#include <Interface_FileReaderData.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ParamList.hxx>
#include <Interface_Static.hxx>
#include <Message_Msg.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
// MGE 03/08/98
static Standard_Integer testconv = -1;  // cf parametre de session

//  ....              Gestion generale (etat, courant ...)              ....


//=======================================================================
//function : IGESData_ParamReader
//purpose  : 
//=======================================================================

IGESData_ParamReader::IGESData_ParamReader(const Handle(Interface_ParamList)& list,
                                           const Handle(Interface_Check)& ach,
                                           const Standard_Integer base,
                                           const Standard_Integer nbpar,
                                           const Standard_Integer /*num*/)
{
  Clear();
  theparams = list;  thecheck = ach;  thelast = Standard_True;
  thebase   = base;
  thenbpar  = (nbpar > 0 ? nbpar : list->Length());
  thenum    = 0;
  testconv  = -1;
}


//=======================================================================
//function : EntityNumber
//purpose  : 
//=======================================================================

Standard_Integer IGESData_ParamReader::EntityNumber () const
{
  return thenum;
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void IGESData_ParamReader::Clear ()
{
  thecurr = 1;
  thestage = IGESData_ReadOwn;
  pbrealint=pbrealform=0;
}


//=======================================================================
//function : CurrentNumber
//purpose  : 
//=======================================================================

Standard_Integer IGESData_ParamReader::CurrentNumber () const
{
  return thecurr;
}


//=======================================================================
//function : SetCurrentNumber
//purpose  : 
//=======================================================================

void IGESData_ParamReader::SetCurrentNumber (const Standard_Integer num)
{
  //if (num <= NbParams() + 1) thecurr = num;  // NbParams+1 : "fin d'objet"
  //else thecurr = 0;
  thecurr = num;
}


//=======================================================================
//function : Stage
//purpose  : 
//=======================================================================

IGESData_ReadStage IGESData_ParamReader::Stage () const
{
  return thestage;
}


//=======================================================================
//function : NextStage
//purpose  : 
//=======================================================================

void IGESData_ParamReader::NextStage ()
{
  if (thestage != IGESData_ReadEnd) thestage =
    (IGESData_ReadStage) (((long) thestage) + 1);
}


//=======================================================================
//function : EndAll
//purpose  : 
//=======================================================================

void IGESData_ParamReader::EndAll ()
{
  thestage = IGESData_ReadEnd;
}


//  ....                  Acces de base aux parametres                  ....


//=======================================================================
//function : NbParams
//purpose  : 
//=======================================================================

Standard_Integer IGESData_ParamReader::NbParams () const
{
  return (thenbpar - 1);
}


//=======================================================================
//function : ParamType
//purpose  : 
//=======================================================================

Interface_ParamType IGESData_ParamReader::ParamType
  (const Standard_Integer num) const
{
  return theparams->Value(num+thebase).ParamType();
}


//=======================================================================
//function : ParamValue
//purpose  : 
//=======================================================================

Standard_CString IGESData_ParamReader::ParamValue
  (const Standard_Integer num) const
{
  return theparams->Value(num+thebase).CValue();
}


//=======================================================================
//function : IsParamDefined
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::IsParamDefined
  (const Standard_Integer num) const
{
  if (num >= thenbpar) return Standard_False;
  return (theparams->Value(num+thebase).ParamType() != Interface_ParamVoid);
}


//=======================================================================
//function : IsParamEntity
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::IsParamEntity
  (const Standard_Integer num) const
{
  return (ParamNumber(num) != 0);
}


//=======================================================================
//function : ParamNumber
//purpose  : 
//=======================================================================

Standard_Integer IGESData_ParamReader::ParamNumber
  (const Standard_Integer num) const
{
  return theparams->Value(num+thebase).EntityNumber();
}


//=======================================================================
//function : ParamEntity
//purpose  : 
//=======================================================================

Handle(IGESData_IGESEntity) IGESData_ParamReader::ParamEntity
       (const Handle(IGESData_IGESReaderData)& IR, const Standard_Integer num)
{
  Standard_Integer n = ParamNumber(num);
  if (n == 0) thecheck->AddFail("IGES ParamReader : ParamEntity, bad param");
  return GetCasted(IGESData_IGESEntity,IR->BoundEntity(n));
}


//  ....                    Assistance a la lecture                    ....

//  Les fonctions Read* offrent les services suivants :
//  Gestion des erreurs : le Check est alimente, par Fail ou Corrected selon
//  Si Fail, retour de fonction False (peut etre utile), sinon True
//  En outre, un Status est gere (de type enum DataState)
//    (peut etre interroge suite a appel Read* si retour True/False trop court)
//
//  Gestion du pointeur courant (sur option, nais elle est mise par defaut) :
//  Les parametres sont designes via un ParmCursor, qui peut etre fabrique par
//  les methodes ad hoc  Current et CurrentList, et qui peut demander a avancer
//  le pointeur courant une fois la lecture faite
//  En outre, pour un HArray1, on peut preciser index de depart


//=======================================================================
//function : Current
//purpose  : 
//=======================================================================

IGESData_ParamCursor IGESData_ParamReader::Current () const
{
  return IGESData_ParamCursor(thecurr);
}


//=======================================================================
//function : CurrentList
//purpose  : 
//=======================================================================

IGESData_ParamCursor IGESData_ParamReader::CurrentList
  (const Standard_Integer nb, const Standard_Integer size) const
{
  return IGESData_ParamCursor(thecurr,nb,size);
}


// PrepareRead for MoniTool


//=======================================================================
//function : PrepareRead
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::PrepareRead
  (const IGESData_ParamCursor& PC, const Standard_Boolean several,
   const Standard_Integer size)
{
  theindex  = PC.Start();
  themaxind = PC.Limit();
  thenbitem = PC.Count();
  theitemsz = PC.ItemSize();
  theoffset = PC.Offset();
  thetermsz = PC.TermSize();
  if (!several && thenbitem > 1) {
   // AddFail (mess," : List not allowed","");
    return Standard_False;
  }
  if (size > 1) {
    if (thetermsz % size != 0) {
      return Standard_False;
    }
  }
  if (theindex <= 0 || (themaxind-1) > NbParams()) {
    return Standard_False;
  }
  if (PC.Advance()) 
    SetCurrentNumber (themaxind);  //themaxind : prochain index
  thelast = Standard_True;
  return Standard_True;
}


//=======================================================================
//function : PrepareRead
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::PrepareRead
  (const IGESData_ParamCursor& PC, const Standard_CString mess,
   const Standard_Boolean several, const Standard_Integer size)
{
  theindex  = PC.Start();
  themaxind = PC.Limit();
  thenbitem = PC.Count();
  theitemsz = PC.ItemSize();
  theoffset = PC.Offset();
  thetermsz = PC.TermSize();
  if (!several && thenbitem > 1) {
    AddFail (mess," : List not allowed","");
    return Standard_False;
  }
  if (size > 1) {
    if (thetermsz % size != 0) {
      AddFail (mess," : term size mismatch","");
      return Standard_False;
    }
  }
  if (theindex <= 0 || (themaxind-1) > NbParams()) {
    if (thenbitem == 1) AddFail (mess," : Parameter number out of range","");
    //else AddFail (mess," : too many values to read" ,"");
    else AddWarning (mess," : too many values to read" ,"");
    return Standard_False;
  }
  if (PC.Advance()) SetCurrentNumber (themaxind);  //themaxind : prochain index
  thelast = Standard_True;
  return Standard_True;
}


//  theindex donne le debut de la lecture; tjrs cale sur debut d item
//  thenbterm donne debut a lire dans l item
//  Ainsi, l indice vrai est   theindex + thenbterm
//  thenbterm avance par +nb. Quand il a depasse thetermsz, item suivant
//  theindex est lui-meme limite (critere d arret) a themaxind


//=======================================================================
//function : FirstRead
//purpose  : 
//=======================================================================

Standard_Integer IGESData_ParamReader::FirstRead (const Standard_Integer nb)
{
  theindex += theoffset;   // On se cale d office sur le debut du terme a lire
  Standard_Integer res = theindex;
  thenbterm = nb;
  if (thenbterm >= thetermsz) {
    theindex += theitemsz;
    thenbterm = 0;
  }
  return res;
}


//=======================================================================
//function : NextRead
//purpose  : 
//=======================================================================

Standard_Integer IGESData_ParamReader::NextRead (const Standard_Integer nb)
{
  Standard_Integer res = theindex;
  if (theindex >= themaxind) res = 0;
  thenbterm += nb;    // Par Item : en lire thetermsz, puis item suivant
  if (thenbterm >= thetermsz) {
    theindex += theitemsz;
    thenbterm = 0;
  }
  return res;
}


//=======================================================================
//function : DefinedElseSkip
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::DefinedElseSkip ()
{
  if (thecurr > NbParams())    return Standard_False;   // Skip en butee
  if (IsParamDefined(thecurr)) return Standard_True;    // Defined
  SetCurrentNumber (thecurr+1);                         // Skip
  return Standard_False;
}


// ReadInteger for MoniTool

//=======================================================================
//function : ReadInteger
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadInteger (const IGESData_ParamCursor& PC,
						    Standard_Integer& val)
{
  if (!PrepareRead(PC,Standard_False)) return Standard_False;
  const Interface_FileParameter& FP = theparams->Value(theindex+thebase);
  if (FP.ParamType() != Interface_ParamInteger) {
    if (FP.ParamType() == Interface_ParamVoid){
      val = 0;
      return Standard_True;
    }    // DEFAULT
    return Standard_False;
  }
  val = atoi(FP.CValue());
  return Standard_True;
}


//=======================================================================
//function : ReadInteger
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadInteger
  (const IGESData_ParamCursor& PC, const Standard_CString mess,
   Standard_Integer& val)
{
  if (!PrepareRead(PC,mess,Standard_False)) return Standard_False;
  const Interface_FileParameter& FP = theparams->Value(theindex+thebase);
  if (FP.ParamType() != Interface_ParamInteger) {
    if (FP.ParamType() == Interface_ParamVoid)
      { val = 0; return Standard_True; }    // DEFAULT
    AddFail (mess," : not given as an Integer","");
    return Standard_False;
  }
  val = atoi(FP.CValue());
  return Standard_True;
}


// ReadBoolean for MoniTool

//=======================================================================
//function : ReadBoolean
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadBoolean
  (const IGESData_ParamCursor& PC, const Message_Msg& amsg,
   Standard_Boolean& val, const Standard_Boolean exact)
{
  if (!PrepareRead(PC,Standard_False)) return Standard_False;
  const Interface_FileParameter& FP = theparams->Value(theindex+thebase);
  if (FP.ParamType() != Interface_ParamInteger) {
    if (FP.ParamType() == Interface_ParamVoid) {
      val = Standard_False;
      return Standard_True;
    }     // DEFAULT
    SendFail (amsg);
    return Standard_False;
  }
  
  //  Un Booleen, c est 0/1. Mais on peut tolerer d autres valeurs
  //  On peut toujours consulter LastReadStatus apres la lecture pour etre sur
  Standard_Integer flag = atoi (FP.CValue());
  if (flag != 0 && flag != 1) {
    if (exact) {
      SendFail (amsg);
      thelast = Standard_True;
      return Standard_False;
    }
    else {
      SendWarning (amsg);
    }
  }
  val = (flag > 0);
  return Standard_True;
}


//=======================================================================
//function : ReadBoolean
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadBoolean
  (const IGESData_ParamCursor& PC, const Standard_CString mess,
   Standard_Boolean& val, const Standard_Boolean exact)
{
  if (!PrepareRead(PC,mess,Standard_False)) return Standard_False;
  const Interface_FileParameter& FP = theparams->Value(theindex+thebase);
  if (FP.ParamType() != Interface_ParamInteger) {
    if (FP.ParamType() == Interface_ParamVoid)
      {
	val = Standard_False;
	return Standard_True;
      }    // DEFAULT
    AddFail (mess," : not an Integer (for Boolean)","");
    return Standard_False;
  }

  //  Un Booleen, c est 0/1. Mais on peut tolerer d autres valeurs
  //  On peut toujours consulter LastReadStatus apres la lecture pour etre sur
  Standard_Integer flag = atoi (FP.CValue());
  if (flag != 0 && flag != 1) {
    char ssem[100];
    sprintf(ssem," : Value is not 0/1, but %s",FP.CValue());
    if (exact) {
      AddFail (mess,ssem," : Value is not 0/1, but %s");
      thelast = Standard_True;
      return Standard_False;
    }
    else AddWarning (mess,ssem," : Value is not 0/1, but %s");
  }
  val = (flag > 0);
  return Standard_True;
}


// ReadReal for MoniTool

//=======================================================================
//function : ReadReal
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadReal (const IGESData_ParamCursor& PC,
						 Standard_Real& val)
{
  if (!PrepareRead(PC,Standard_False)) return Standard_False;
//  return ReadingReal (theindex,amsg,val);
  return ReadingReal (theindex,val);
}


//=======================================================================
//function : ReadReal
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadReal
  (const IGESData_ParamCursor& PC,const Standard_CString mess,
   Standard_Real& val)
{
  if (!PrepareRead(PC,mess,Standard_False)) return Standard_False;
  return ReadingReal (theindex,mess,val);
}


// ReadXY for MoniTool

//=======================================================================
//function : ReadXY
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadXY
  (const IGESData_ParamCursor& PC,Message_Msg& /*amsg*/, gp_XY& val)
{
  if (!PrepareRead(PC,Standard_False,2)) return Standard_False;
  Standard_Real X,Y = 0.;
  Standard_Boolean stat =
    (ReadingReal (theindex  ,X)  &&
     ReadingReal (theindex+1,Y)  );
  if (stat) val.SetCoord(X,Y);
  return stat;
}


//=======================================================================
//function : ReadXY
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadXY
  (const IGESData_ParamCursor& PC, const Standard_CString mess, gp_XY& val)
{
  if (!PrepareRead(PC,mess,Standard_False,2)) return Standard_False;
  Standard_Real X,Y = 0.;
  Standard_Boolean stat =
    (ReadingReal (theindex  ,mess,X)  &&
     ReadingReal (theindex+1,mess,Y)  );
  if (stat) val.SetCoord(X,Y);
  return stat;
}


// ReadXYZ for MoniTool

//=======================================================================
//function : ReadXYZ
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadXYZ
  (const IGESData_ParamCursor& PC,Message_Msg& /*amsg*/, gp_XYZ& val)
{
  if (!PrepareRead(PC,Standard_False,3)) return Standard_False;
  Standard_Real X,Y = 0.,Z = 0.;
  Standard_Boolean stat =
    (ReadingReal (theindex  ,X)  &&
     ReadingReal (theindex+1,Y)  &&
     ReadingReal (theindex+2,Z)  );
  if (stat) val.SetCoord(X,Y,Z);
  return Standard_True;
}


//=======================================================================
//function : ReadXYZ
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadXYZ
  (const IGESData_ParamCursor& PC, const Standard_CString mess, gp_XYZ& val)
{
  if (!PrepareRead(PC,mess,Standard_False,3)) return Standard_False;
  Standard_Real X,Y = 0.,Z = 0.;
  Standard_Boolean stat =
    (ReadingReal (theindex  ,mess,X)  &&
     ReadingReal (theindex+1,mess,Y)  &&
     ReadingReal (theindex+2,mess,Z)  );
  if (stat) val.SetCoord(X,Y,Z);
  return Standard_True;
}


// ReadText for MoniTool

//=======================================================================
//function : ReadText
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadText
  (const IGESData_ParamCursor& PC, const Message_Msg& amsg,
   Handle(TCollection_HAsciiString)& val)
{
  if (!PrepareRead(PC,Standard_False)) return Standard_False;
  const Interface_FileParameter& FP = theparams->Value(theindex+thebase);
  if (FP.ParamType() != Interface_ParamText) {
    if (FP.ParamType() == Interface_ParamVoid) {
      val = new TCollection_HAsciiString("");
      return Standard_True;
    }
    SendFail (amsg);
    return Standard_False;
  }
  Handle(TCollection_HAsciiString) tval = new TCollection_HAsciiString (FP.CValue());
  Standard_Integer lnt = tval->Length();
  Standard_Integer lnh = tval->Location(1,'H',1,lnt);
  if (lnh <= 1 || lnh >= lnt) {
    SendFail (amsg);
    return Standard_False;
  } else {
    Standard_Integer hol = atoi (tval->SubString(1,lnh-1)->ToCString());
    if (hol != (lnt-lnh)) SendWarning (amsg);
  }
  val = new TCollection_HAsciiString(tval->SubString(lnh+1,lnt)->ToCString());
  return Standard_True;
}


//=======================================================================
//function : ReadText
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadText
  (const IGESData_ParamCursor& PC, const Standard_CString mess,
   Handle(TCollection_HAsciiString)& val)
{
  if (!PrepareRead(PC,mess,Standard_False)) return Standard_False;
  const Interface_FileParameter& FP = theparams->Value(theindex+thebase);
  if (FP.ParamType() != Interface_ParamText) {
    if (FP.ParamType() == Interface_ParamVoid) {
      val = new TCollection_HAsciiString("");
      return Standard_True;
    }
    AddFail (mess," : not given as a Text","");
    return Standard_False;
  }
  Handle(TCollection_HAsciiString) tval = new TCollection_HAsciiString (FP.CValue());
  Standard_Integer lnt = tval->Length();
  Standard_Integer lnh = tval->Location(1,'H',1,lnt);
  if (lnh <= 1 || lnh >= lnt) {
    AddFail (mess," : not in Hollerith Form","");
    return Standard_False;
  }
  else {
    Standard_Integer hol = atoi (tval->SubString(1,lnh-1)->ToCString());
    if (hol != (lnt-lnh)) AddWarning (mess," : bad Hollerith count ","");
  }
  val = new TCollection_HAsciiString(tval->SubString(lnh+1,lnt)->ToCString());
  return Standard_True;
}


// ReadEntity for MoniTool

//=======================================================================
//function : ReadEntity
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadEntity (const Handle(IGESData_IGESReaderData)& IR, 
						   const IGESData_ParamCursor& PC,
						   IGESData_Status& aStatus, 
						   Handle(IGESData_IGESEntity)& val, 
						   const Standard_Boolean canbenul)
{
  aStatus = IGESData_EntityError;
  if (!PrepareRead(PC,Standard_False)) return Standard_False;
  Standard_Integer nval;
//  if (!ReadingEntityNumber(theindex,amsg,nval)) return Standard_False;
  if (!ReadingEntityNumber(theindex,nval)) return Standard_False;
  if (nval == 0) {
    val.Nullify();
    if (!canbenul) {
      aStatus = IGESData_ReferenceError;
     // Message_Msg Msg216 ("IGESP_216");
     // amsg.Arg(amsg.Value());
     // SendFail (amsg);

      thelast = Standard_True;
    }
    else
      aStatus = IGESData_EntityOK;
    return canbenul;
  }
  else val = GetCasted(IGESData_IGESEntity,IR->BoundEntity(nval));
  if (val.IsNull()) return canbenul;
  //    Cas du "Nul IGES"
  if (val->TypeNumber() == 0) {           // Null ou pas encore rempli ...
    if (IR->DirType(nval).Type() == 0) {  // le vrai critere (un peu cher)
      val.Nullify();
      if (!canbenul) {
	aStatus = IGESData_EntityError;
	// Message_Msg Msg217 ("IGES_217");
        //amsg.Arg(Msg217.Value());
        //SendFail (amsg);
        thelast = Standard_True;
      }
      else
        aStatus = IGESData_EntityOK;
      return canbenul;
    }
  }
  aStatus = IGESData_EntityOK;
  return Standard_True;
}


//=======================================================================
//function : ReadEntity
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadEntity
  (const Handle(IGESData_IGESReaderData)& IR, const IGESData_ParamCursor& PC,
   const Standard_CString mess, Handle(IGESData_IGESEntity)& val, const Standard_Boolean canbenul)
{
  if (!PrepareRead(PC,mess,Standard_False)) return Standard_False;
  Standard_Integer nval;
  if (!ReadingEntityNumber(theindex,mess,nval)) return Standard_False;
  if (nval == 0) {
    val.Nullify();
    if (!canbenul) {
      AddFail (mess," : Null Reference","");
      thelast = Standard_True;
    }
    return canbenul;
  }
  else val = GetCasted(IGESData_IGESEntity,IR->BoundEntity(nval));
  if (val.IsNull()) return canbenul;
  //    Cas du "Nul IGES"
  if (val->TypeNumber() == 0) {           // Null ou pas encore rempli ...
    if (IR->DirType(nval).Type() == 0) {  // le vrai critere (un peu cher)
      val.Nullify();
      if (!canbenul) {
	AddFail (mess," : IGES Null Entity","");
	thelast = Standard_True;
      }
      return canbenul;
    }
  }
  return Standard_True;
}


// ReadEntity for MoniTool

//=======================================================================
//function : ReadEntity
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadEntity (const Handle(IGESData_IGESReaderData)& IR,
						   const IGESData_ParamCursor& PC,
						   IGESData_Status& aStatus, 
						   const Handle(Standard_Type)& type,
						   Handle(IGESData_IGESEntity)& val,
						   const Standard_Boolean canbenul)
{
  Standard_Boolean res = ReadEntity (IR,PC,aStatus,val,canbenul);
  if (!res) {
    return res;
  }
  if (val.IsNull()) return res;
  if (!val->IsKind(type)) {
    aStatus = IGESData_TypeError;
    // Message_Msg Msg218 ("IGES_218");
    //amsg.Arg(Msg218.Value());
    //SendFail(amsg);
    thelast = Standard_True;
    val.Nullify();
    return Standard_False;
  }
  return Standard_True;
}


//=======================================================================
//function : ReadEntity
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadEntity
  (const Handle(IGESData_IGESReaderData)& IR,
   const IGESData_ParamCursor& PC, const Standard_CString mess,
   const Handle(Standard_Type)& type,
   Handle(IGESData_IGESEntity)& val, const Standard_Boolean canbenul)
{
  Standard_Boolean res = ReadEntity (IR,PC,mess,val,canbenul);
  if (!res) return res;
  if (val.IsNull()) return res;
  if (!val->IsKind(type)) {
    AddFail (mess," : Incorrect Type","");
    thelast = Standard_True;
    val.Nullify();
    return Standard_False;
  }
  return Standard_True;
}


// ReadInts for MoniTool

//=======================================================================
//function : ReadInts
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadInts
  (const IGESData_ParamCursor& PC, const Message_Msg& amsg ,
   Handle(TColStd_HArray1OfInteger)& val,  const Standard_Integer index)
{
  if (!PrepareRead(PC,Standard_True)) return Standard_False;
  if (thenbitem == 0) return Standard_True;    // vide : retour Null ...
  val = new TColStd_HArray1OfInteger (index,index+thenbitem*thetermsz-1);
  Standard_Integer ind = index;

  for (Standard_Integer i = FirstRead(); i > 0; i = NextRead()) {
    const Interface_FileParameter& FP = theparams->Value(i+thebase);
    if (FP.ParamType() == Interface_ParamInteger) {
      val->SetValue (ind, atoi(FP.CValue()));    ind ++;
    } else if (FP.ParamType() == Interface_ParamVoid) {
      val->SetValue (ind,0);    ind ++;    // DEFAULT : rien a dire
    } else {
      SendFail (amsg);
      return Standard_False;
    }
  }
  return Standard_True;
}
  

//=======================================================================
//function : ReadInts
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadInts
  (const IGESData_ParamCursor& PC, const Standard_CString mess,
   Handle(TColStd_HArray1OfInteger)& val,  const Standard_Integer index)
{
  if (!PrepareRead(PC,mess,Standard_True)) return Standard_False;
  if (thenbitem == 0) return Standard_True;    // vide : retour Null ...
  val = new TColStd_HArray1OfInteger (index,index+thenbitem*thetermsz-1);
  Standard_Integer ind = index;

  for (Standard_Integer i = FirstRead(); i > 0; i = NextRead()) {
    const Interface_FileParameter& FP = theparams->Value(i+thebase);
    if (FP.ParamType() == Interface_ParamInteger) {
      val->SetValue (ind, atoi(FP.CValue()));    ind ++;
    } else if (FP.ParamType() == Interface_ParamVoid) {
      val->SetValue (ind,0);    ind ++;    // DEFAULT : rien a dire
    } else {
      char ssem[100];
      sprintf(ssem," : not an Integer, rank %d",i);
      AddFail (mess,ssem," : not an Integer, rank %d");
      return Standard_False;
    }
  }
  return Standard_True;
}


// ReadReals for MoniTool

//=======================================================================
//function : ReadReals
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadReals
  (const IGESData_ParamCursor& PC,Message_Msg& /*amsg*/,
   Handle(TColStd_HArray1OfReal)& val,  const Standard_Integer index)
{
  if (!PrepareRead(PC,Standard_True)) return Standard_False;
  if (thenbitem == 0) return Standard_True;    // vide : retour Null ...
  val = new TColStd_HArray1OfReal (index,index+thenbitem*thetermsz-1);
  Standard_Integer ind = index;

  for (Standard_Integer i = FirstRead(); i > 0; i = NextRead()) {
    Standard_Real rval;
    if (!ReadingReal(i,rval)) return Standard_False;
    val->SetValue (ind, rval);    ind ++;
  }
  return Standard_True;
}


//=======================================================================
//function : ReadReals
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadReals
  (const IGESData_ParamCursor& PC, const Standard_CString mess,
   Handle(TColStd_HArray1OfReal)& val,  const Standard_Integer index)
{
  if (!PrepareRead(PC,mess,Standard_True)) return Standard_False;
  if (thenbitem == 0) return Standard_True;    // vide : retour Null ...
  val = new TColStd_HArray1OfReal (index,index+thenbitem*thetermsz-1);
  Standard_Integer ind = index;

  for (Standard_Integer i = FirstRead(); i > 0; i = NextRead()) {
    Standard_Real rval;
    if (!ReadingReal(i,mess,rval)) return Standard_False;
    val->SetValue (ind, rval);    ind ++;
  }
  return Standard_True;
}


// ReadTexts for MoniTool

//=======================================================================
//function : ReadTexts
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadTexts
  (const IGESData_ParamCursor& PC, const Message_Msg& amsg  ,
   Handle(Interface_HArray1OfHAsciiString)& val,  const Standard_Integer index)
{
  if (!PrepareRead(PC,Standard_True)) return Standard_False;
  if (thenbitem == 0) return Standard_True;    // vide : retour Null ...
  val = new Interface_HArray1OfHAsciiString(index,index+thenbitem*thetermsz-1);
  Standard_Integer ind = index;

  for (Standard_Integer i = FirstRead(); i > 0; i = NextRead()) {
    const Interface_FileParameter& FP = theparams->Value(i+thebase);
    if (FP.ParamType() != Interface_ParamText) {
      if (FP.ParamType() == Interface_ParamVoid) {
	val->SetValue (ind, new TCollection_HAsciiString(""));
	ind ++;
        //AddWarning (mess," : empty text","");  DEFAULT : rien a dire
	continue;
      }
      SendFail(amsg);
      return Standard_False;
    }
    Handle(TCollection_HAsciiString) tval = new TCollection_HAsciiString (FP.CValue());
    // IGESFile_Read a filtre
    Standard_Integer lnt = tval->Length();
    Standard_Integer lnh = tval->Location(1,'H',1,lnt);
    if (lnh <= 1 || lnh >= lnt) {
      SendFail (amsg);
      return Standard_False;
    } else {
      Standard_Integer hol = atoi (tval->SubString(1,lnh-1)->ToCString());
      if (hol != (lnt-lnh)) SendWarning(amsg);
    }
    val->SetValue (ind, new TCollection_HAsciiString
		   (tval->SubString(lnh+1,lnt)->ToCString()));
    ind++;
  }
  return Standard_True;
}


//=======================================================================
//function : ReadTexts
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadTexts
  (const IGESData_ParamCursor& PC, const Standard_CString mess,
   Handle(Interface_HArray1OfHAsciiString)& val,  const Standard_Integer index)
{
  if (!PrepareRead(PC,mess,Standard_True)) return Standard_False;
  if (thenbitem == 0) return Standard_True;    // vide : retour Null ...
  val = new Interface_HArray1OfHAsciiString(index,index+thenbitem*thetermsz-1);
  Standard_Integer ind = index;

  for (Standard_Integer i = FirstRead(); i > 0; i = NextRead()) {
    const Interface_FileParameter& FP = theparams->Value(i+thebase);
    if (FP.ParamType() != Interface_ParamText) {
      if (FP.ParamType() == Interface_ParamVoid) {
        val->SetValue (ind, new TCollection_HAsciiString(""));
        ind ++;
        //AddWarning (mess," : empty text","");  DEFAULT : rien a dire
        continue;
      }
      AddFail (mess," : not given as a Text","");
      return Standard_False;
    }
    Handle(TCollection_HAsciiString) tval = new TCollection_HAsciiString (FP.CValue());
    // IGESFile_Read a filtre
    Standard_Integer lnt = tval->Length();
    Standard_Integer lnh = tval->Location(1,'H',1,lnt);
    if (lnh <= 1 || lnh >= lnt) {
      AddFail (mess," : not in Hollerith Form","");
      return Standard_False;
    } else {
      Standard_Integer hol = atoi (tval->SubString(1,lnh-1)->ToCString());
      if (hol != (lnt-lnh)) AddWarning(mess," : bad Hollerith count ","");
    }
    val->SetValue (ind, new TCollection_HAsciiString
                   (tval->SubString(lnh+1,lnt)->ToCString()));
    ind++;
  }
  return Standard_True;
}


// ReadEnts for MoniTool

//=======================================================================
//function : ReadEnts
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadEnts
  (const Handle(IGESData_IGESReaderData)& IR,
   const IGESData_ParamCursor& PC, const Message_Msg& amsg  ,
   Handle(IGESData_HArray1OfIGESEntity)& val,  const Standard_Integer index)
{
  if (!PrepareRead(PC,Standard_True)) return Standard_False;
  if (thenbitem == 0) return Standard_True;    // vide : retour Null ...
  Standard_Integer indmax = index+thenbitem*thetermsz-1;
  val = new IGESData_HArray1OfIGESEntity (index , indmax);
  Standard_Integer ind = index;
  Standard_Integer nbneg = 0, nbnul = 0;

  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = FirstRead(); i > 0; i = NextRead()) {
    Standard_Integer nval;
    if (!ReadingEntityNumber(i,nval)) nval = 0;  //return Standard_False;
    if (nval < 0) nbneg ++;
    if (nval > 0) {
      DeclareAndCast(IGESData_IGESEntity,anent,IR->BoundEntity(nval));
      if (anent.IsNull()) nbnul ++;
      else if (IR->DirType(nval).Type() == 0) nbnul ++;
      else  {  val->SetValue (ind, anent);  ind ++;  }
    }
  }
  if      (ind == indmax+1) {  }                   // tableau complet
  else if (ind == index)  val.Nullify();         // tableau vide
  else {
    // Trous : ils ont ete elimines, mais le tableau est a retailler
    Handle(IGESData_HArray1OfIGESEntity) tab =
      new IGESData_HArray1OfIGESEntity (index , ind-1);
    for (i = index; i < ind; i ++) tab->SetValue (i,val->Value(i));
    val = tab;
  }
  if (nbnul > 0) {
    SendWarning (amsg);
  }
  return Standard_True;
}


//=======================================================================
//function : ReadEnts
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadEnts
  (const Handle(IGESData_IGESReaderData)& IR,
   const IGESData_ParamCursor& PC, const Standard_CString mess,
   Handle(IGESData_HArray1OfIGESEntity)& val,  const Standard_Integer index)
{
  if (!PrepareRead(PC,mess,Standard_True)) return Standard_False;
  if (thenbitem == 0) return Standard_True;    // vide : retour Null ...
  Standard_Integer indmax = index+thenbitem*thetermsz-1;
  val = new IGESData_HArray1OfIGESEntity (index , indmax);
  Standard_Integer ind = index;
  Standard_Integer nbneg = 0, nbnul = 0;

  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = FirstRead(); i > 0; i = NextRead()) {
    Standard_Integer nval;
    if (!ReadingEntityNumber(i,mess,nval)) nval = 0;  //return Standard_False;
    if (nval < 0) nbneg ++;
    if (nval > 0) {
      DeclareAndCast(IGESData_IGESEntity,anent,IR->BoundEntity(nval));
      if (anent.IsNull()) nbnul ++;
      else if (IR->DirType(nval).Type() == 0) nbnul ++;
      else  {  val->SetValue (ind, anent);  ind ++;  }
    }
  }
  if      (ind == indmax+1) {  }                   // tableau complet
  else if (ind == index)  val.Nullify();         // tableau vide
  else {
    // Trous : ils ont ete elimines, mais le tableau est a retailler
    Handle(IGESData_HArray1OfIGESEntity) tab =
      new IGESData_HArray1OfIGESEntity (index , ind-1);
    for (i = index; i < ind; i ++) tab->SetValue (i,val->Value(i));
    val = tab;
  }
  //  Messages ?
  char mest[80];
  if (nbneg > 0) {
    sprintf(mest,"Skipped Negative Pointer(s), count %d",nbneg);
    AddWarning (mest,"Skipped Negative Pointer(s), count %d");
  }
  if (nbnul > 0) {
    sprintf(mest,"Skipped Null Type Entity(ies), count %d",nbnul);
    AddWarning (mest,"Skipped Null Type Entity(ies), count %d");
  }
  return Standard_True;
}


// ReadEntList for MoniTool

//=======================================================================
//function : ReadEntList
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadEntList
  (const Handle(IGESData_IGESReaderData)& IR,
   const IGESData_ParamCursor& PC, Message_Msg& amsg,
   Interface_EntityList& val, const Standard_Boolean ord)
{

  if (!PrepareRead(PC,Standard_True)) return Standard_False;
  val.Clear();
  if (thenbitem == 0) return Standard_True;    // vide : retour Null ...
  for (Standard_Integer i = FirstRead(); i > 0; i = NextRead()) {
    Standard_Integer nval;
    if (!ReadingEntityNumber(i,nval)) return Standard_False;
    if (nval < 0) 
      {
        Message_Msg Msg219 ("XSTEP_219");
        amsg.Arg(Msg219.Value());
        SendWarning(amsg);
      }
    if (nval <= 0) continue;
    DeclareAndCast(IGESData_IGESEntity,anent,IR->BoundEntity(nval));
    if (anent.IsNull()) 
    { 
      Message_Msg Msg216 ("XSTEP_216");
      amsg.Arg(Msg216.Value());
      SendWarning(amsg);
    }   
    else if (IR->DirType(nval).Type() == 0)
    {
      Message_Msg Msg217 ("XSTEP_217");
      SendWarning(TCollection_AsciiString(Msg217.Value()).ToCString());
    }
    else if (ord) val.Append (anent);
    else          val.Add    (anent);
  }
  return Standard_True;
}


//=======================================================================
//function : ReadEntList
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadEntList
  (const Handle(IGESData_IGESReaderData)& IR,
   const IGESData_ParamCursor& PC, const Standard_CString mess,
   Interface_EntityList& val, const Standard_Boolean ord)
{
  if (!PrepareRead(PC,mess,Standard_True)) return Standard_False;
  val.Clear();
  if (thenbitem == 0) return Standard_True;    // vide : retour Null ...
  for (Standard_Integer i = FirstRead(); i > 0; i = NextRead()) {
    Standard_Integer nval;
    if (!ReadingEntityNumber(i,mess,nval)) return Standard_False;
    if (nval < 0)  AddWarning(" Negative Pointer, skipped","");
    if (nval <= 0) continue;
    DeclareAndCast(IGESData_IGESEntity,anent,IR->BoundEntity(nval));
    if (anent.IsNull()) AddWarning(" Null Pointer, skipped","");
    else if (IR->DirType(nval).Type() == 0) AddWarning(" Pointer to IGES Null Entity, skipped","");
    else if (ord) val.Append (anent);
    else          val.Add    (anent);
  }
  return Standard_True;
}


// ReadingReal for MoniTool

//=======================================================================
//function : ReadingReal
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadingReal (const Standard_Integer num,
						    Standard_Real& val)
{
  const Interface_FileParameter& FP = theparams->Value(num+thebase);
  if(FP.ParamType() == Interface_ParamInteger) {
    if (!pbrealint) {
      if (testconv < 0) testconv = 0; //Interface_Static::IVal("iges.convert.read");
      if (testconv > 0) {
     //   char ssem[100];
        pbrealint = num;
     //   sprintf(ssem,": Integer converted to Real, 1st rank=%d",num);
     //   AddWarning (mess,ssem,"At least one Integer converted to Real, 1st rank=%d");
      }
    }
    Standard_Integer ival = atoi(FP.CValue());
    val = ival;
    return Standard_True; 
  }
  char text[50];
  Standard_CString orig = FP.CValue();
  Standard_Integer i , j = 0;
  for (i = 0; i < 50; i ++) {
    if (orig[i] ==  'D' || orig[i] == 'd')
      text[j++] = 'e';
    else
      text[j++] = orig[i];  
    if (orig[i] == '\0') break;
  }
  if (FP.ParamType() == Interface_ParamReal) 
    val = Atof(text);
  else if (FP.ParamType() == Interface_ParamEnum) {  // convention
    if (!pbrealform) {
      if (testconv < 0) testconv = 0; //Interface_Static::IVal("iges.convert.read");
      if (testconv > 0) {
       // char ssem[100];
        pbrealform = num;
      //  sprintf(ssem,"Real with no decimal point (added), 1st rank=%d",num);
      //  AddWarning (mess,ssem,"Real with no decimal point (added), 1st rank=%d");
      }
    }
    // Par convention (pas d enum explicite dans IGES), signifie
    // "reconnu comme flottant mais pas blanc-bleu" c-a-d sans point decimal
    // mais avec exposant (sinon ce serait un entier)
    // -> un message avertissement + on ajoute le point puis on convertit
    
    val = Atof(text);
  } else if (FP.ParamType() == Interface_ParamVoid) {
    val = 0.0;    // DEFAULT
  } else {
   // char ssem[100];
  //  sprintf(ssem,": not given as Real, rank %d",num);
  //  AddFail (mess,ssem,": not given as Real, rank %d");
  /*  TCollection_AsciiString mess = amsg.Value();
    if ((mess.Search("ter %d"))||(mess.Search("tre %d")))
       amsg.AddInteger(num); // Parameter index
  */ 
    return Standard_False;
  }
  return Standard_True;
}


//=======================================================================
//function : ReadingReal
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadingReal
  (const Standard_Integer num, const Standard_CString mess,
   Standard_Real& val)
{
  const Interface_FileParameter& FP = theparams->Value(num+thebase);
  if (FP.ParamType() == Interface_ParamInteger) {
    if (!pbrealint) {
      if (testconv < 0) testconv = 0; //Interface_Static::IVal("iges.convert.read");
      if (testconv > 0) {
	char ssem[100];
	pbrealint = num;
	sprintf(ssem,": Integer converted to Real, 1st rank=%d",num);
	AddWarning (mess,ssem,"At least one Integer converted to Real, 1st rank=%d");
      }
    }
    Standard_Integer ival = atoi(FP.CValue());
    val = ival;
    return Standard_True;
  }
  char text[50];
  Standard_CString orig = FP.CValue();
  Standard_Integer i , j = 0;
  for (i = 0; i < 50; i ++) {
    if (orig[i] ==  'D' || orig[i] == 'd')
      text[j++] = 'e';
    else
      text[j++] = orig[i];  
    if (orig[i] == '\0') break;
  }
  if (FP.ParamType() == Interface_ParamReal) 
    val = Atof(text);
  else if (FP.ParamType() == Interface_ParamEnum) {  // convention
    if (!pbrealform) {
      if (testconv < 0) testconv = 0; //Interface_Static::IVal("iges.convert.read");
      if (testconv > 0) {
	char ssem[100];
	pbrealform = num;
	sprintf(ssem,"Real with no decimal point (added), 1st rank=%d",num);
	AddWarning (mess,ssem,"Real with no decimal point (added), 1st rank=%d");
      }
    }
    // Par convention (pas d enum explicite dans IGES), signifie
    // "reconnu comme flottant mais pas blanc-bleu" c-a-d sans point decimal
    // mais avec exposant (sinon ce serait un entier)
    // -> un message avertissement + on ajoute le point puis on convertit
    
    val = Atof(text);
  } else if (FP.ParamType() == Interface_ParamVoid) {
    val = 0.0;    // DEFAULT
  } else {
    val = 0.0;    // DEFAULT
    char ssem[100];
    sprintf(ssem,": not given as Real, rank %d",num);
    AddFail (mess,ssem,": not given as Real, rank %d");
    return Standard_False;
  }
  return Standard_True;
}
 

// ReadingEntityNumber for MoniTool

//=======================================================================
//function : ReadingEntityNumber
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadingEntityNumber (const Standard_Integer num, 
							    Standard_Integer& val)
{
  const Interface_FileParameter& FP = theparams->Value(num+thebase);
  val = ParamNumber(num);
  if (val == 0) {
    Standard_Boolean nulref = Standard_False;
    if (FP.ParamType() == Interface_ParamInteger)
        nulref = (atoi(FP.CValue()) == 0);
    else if (FP.ParamType() == Interface_ParamVoid) nulref = Standard_True;
    if (!nulref) {
   //   AddFail (mess," : cannot refer to an Entity","");
      thelast = Standard_True;
      return Standard_False;
    }
  }
  return Standard_True;
}


//=======================================================================
//function : ReadingEntityNumber
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::ReadingEntityNumber
  (const Standard_Integer num, const Standard_CString mess,
   Standard_Integer& val)
{
  const Interface_FileParameter& FP = theparams->Value(num+thebase);
  val = ParamNumber(num);
  if (val == 0) {
    Standard_Boolean nulref = Standard_False;
    if (FP.ParamType() == Interface_ParamInteger)
	nulref = (atoi(FP.CValue()) == 0);
    else if (FP.ParamType() == Interface_ParamVoid) nulref = Standard_True;
    if (!nulref) {
      AddFail (mess," : cannot refer to an Entity","");
      thelast = Standard_True;
      return Standard_False;
    }
  }
  return Standard_True;
}


//=======================================================================
//function : SendFail
//purpose  : 
//=======================================================================

void IGESData_ParamReader::SendFail (const Message_Msg& amsg)
{
  thecheck->SendFail (amsg);
  thelast = Standard_False;
}


//=======================================================================
//function : SendWarning
//purpose  : 
//=======================================================================

void IGESData_ParamReader::SendWarning (const Message_Msg& amsg)
{
  thecheck->SendWarning (amsg);
  thelast = Standard_False;
}



//  ....              Gestion courante du statut de lecture              ....


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void IGESData_ParamReader::AddFail(const Standard_CString idm,
                                   const Handle(TCollection_HAsciiString)& afail,
                                   const Handle(TCollection_HAsciiString)& bfail)
{
  afail->Insert (1,idm);
  if (bfail != afail) bfail->Insert (1,idm);
  thecheck->AddFail (afail,bfail);
  thelast = Standard_False;
}


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void IGESData_ParamReader::AddFail(const Standard_CString idm,
                                   const Standard_CString afail,
                                   const Standard_CString bfail)
{
  Handle(TCollection_HAsciiString) af = new TCollection_HAsciiString(afail);
  Handle(TCollection_HAsciiString) bf = af;
  if (bfail[0] != '\0') bf = new TCollection_HAsciiString(bfail);
  AddFail (idm, af,bf);
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void IGESData_ParamReader::AddWarning(const Standard_CString idm,
                                      const Handle(TCollection_HAsciiString)& aw,
                                      const Handle(TCollection_HAsciiString)& bw)
{
  aw->Insert (1,idm);
  if (bw != aw) bw->Insert (1,idm);
  thecheck->AddWarning (aw,bw);
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void IGESData_ParamReader::AddWarning(const Standard_CString idm,
                                      const Standard_CString awarn,
                                      const Standard_CString bwarn)
{
  Handle(TCollection_HAsciiString) aw = new TCollection_HAsciiString(awarn);
  Handle(TCollection_HAsciiString) bw = aw;
  if (bwarn[0] != '\0') bw = new TCollection_HAsciiString(bwarn);
  AddWarning (idm, aw,bw);
}


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void IGESData_ParamReader::AddFail(const Standard_CString afail,
                                   const Standard_CString bfail)
{
  thelast = Standard_False;
  thecheck->AddFail(afail,bfail);
}


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void IGESData_ParamReader::AddFail(const Handle(TCollection_HAsciiString)& afail,
                                   const Handle(TCollection_HAsciiString)& bfail)
{
  thelast = Standard_False;
  thecheck->AddFail(afail,bfail);
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void IGESData_ParamReader::AddWarning(const Standard_CString amess,
                                      const Standard_CString bmess)
{
  thecheck->AddWarning(amess,bmess);
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void IGESData_ParamReader::AddWarning(const Handle(TCollection_HAsciiString)& amess,
                                      const Handle(TCollection_HAsciiString)& bmess)
{
  thecheck->AddWarning(amess,bmess);
}


//=======================================================================
//function : Mend
//purpose  : 
//=======================================================================

void IGESData_ParamReader::Mend (const Standard_CString pref)
{
  thecheck->Mend (pref);
  thelast = Standard_True;
}


//=======================================================================
//function : HasFailed
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::HasFailed () const
{
  return !thelast;
}  //thecheck.HasFailed();


//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

const Handle(Interface_Check)& IGESData_ParamReader::Check () const
{
  return thecheck;
}


//=======================================================================
//function : CCheck
//purpose  : 
//=======================================================================

Handle(Interface_Check)& IGESData_ParamReader::CCheck ()
{
  return thecheck;
}


//=======================================================================
//function : IsCheckEmpty
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_ParamReader::IsCheckEmpty () const
{
  return (!thecheck->HasFailed() && !thecheck->HasWarnings());
}
