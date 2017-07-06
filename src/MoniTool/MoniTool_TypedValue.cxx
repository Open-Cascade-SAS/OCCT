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


#include <Message_Messenger.hxx>
#include <MoniTool_Element.hxx>
#include <MoniTool_TypedValue.hxx>
#include <OSD_Path.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(MoniTool_TypedValue,Standard_Transient)

// Not Used :
//static  char defmess[30];
static NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> thelibtv;
static NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> astats;

static NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& libtv()
{
  if (thelibtv.IsEmpty()) {
    Handle(MoniTool_TypedValue) tv;
    tv = new MoniTool_TypedValue("Integer",MoniTool_ValueInteger);
    thelibtv.Bind ("Integer",tv);
    tv = new MoniTool_TypedValue("Real",MoniTool_ValueReal);
    thelibtv.Bind ("Real",tv);
    tv = new MoniTool_TypedValue("Text",MoniTool_ValueText);
    thelibtv.Bind ("Text",tv);
    tv = new MoniTool_TypedValue("Transient",MoniTool_ValueIdent);
    thelibtv.Bind ("Transient",tv);
    tv = new MoniTool_TypedValue("Boolean",MoniTool_ValueEnum);
    tv->AddDef ("enum 0");    //    = 0 False  ,  > 0 True
    tv->AddDef ("eval False");
    tv->AddDef ("eval True");
    thelibtv.Bind ("Boolean",tv);
    tv = new MoniTool_TypedValue("Logical",MoniTool_ValueEnum);
    tv->AddDef ("enum -1");    //    < 0 False  ,  = 0 Unk  ,  > 0 True
    tv->AddDef ("eval False");
    tv->AddDef ("eval Unknown");
    tv->AddDef ("eval True");
    thelibtv.Bind ("Logical",tv);
  }
  return thelibtv;
}


//  Fonctions Satisfies offertes en standard ...
/* Not Used

static Standard_Boolean StaticPath(const Handle(TCollection_HAsciiString)& val)
{
  OSD_Path apath;
  return apath.IsValid (TCollection_AsciiString(val->ToCString()));
}
*/

    MoniTool_TypedValue::MoniTool_TypedValue
  (const Standard_CString name,
   const MoniTool_ValueType type, const Standard_CString init)
    : thename (name) , thetype (type) ,
      thelims (0), themaxlen (0) , theintlow (0) , theintup (-1) ,
      theinterp (NULL) , thesatisf (NULL) , 
      theival (0),
      thehval (new TCollection_HAsciiString(""))
{
  if (type != MoniTool_ValueInteger && type != MoniTool_ValueReal &&
      type != MoniTool_ValueEnum    && type != MoniTool_ValueText &&
      type != MoniTool_ValueIdent)
    throw Standard_ConstructionError("MoniTool_TypedValue : Type not supported");
  if (init[0] != '\0')
    if (Satisfies(new TCollection_HAsciiString(init))) SetCStringValue (init);

}


    MoniTool_TypedValue::MoniTool_TypedValue
  (const Handle(MoniTool_TypedValue)& other)
    : thename  (other->Name()) , thedef (other->Definition()) ,
      thelabel (other->Label()) , thetype (other->ValueType()) ,
      theotyp  (other->ObjectType()) ,
      thelims (0) , themaxlen (other->MaxLength()) ,
      theintlow (0) , theintup (0) , therealow (0) , therealup (0) ,
      theunidef (other->UnitDef()) ,
      theival  (other->IntegerValue()) ,  thehval  (other->HStringValue()) ,
      theoval  (other->ObjectValue())
{
  NCollection_DataMap<TCollection_AsciiString, Standard_Integer> eadds;
  Standard_CString satisname;
  other->Internals (theinterp,thesatisf,satisname, eadds);
  thesatisn.AssignCat (satisname);

  if (other->IntegerLimit (Standard_False,theintlow)) thelims |= 1;
  if (other->IntegerLimit (Standard_True ,theintup )) thelims |= 2;
  if (other->RealLimit    (Standard_False,therealow)) thelims |= 1;
  if (other->RealLimit    (Standard_True ,therealup)) thelims |= 2;

  Standard_Integer startcase, endcase; Standard_Boolean match;
  if (other->EnumDef (startcase,endcase,match)) {
    theintlow = startcase;  theintup = endcase;
    if (match) thelims |= 4;
    if (theintup >= theintlow) theenums = new TColStd_HArray1OfAsciiString (theintlow,theintup);
    for (startcase = theintlow; startcase <= theintup; startcase ++) {
      theenums->SetValue (startcase,other->EnumVal (startcase));
    }
  }
//  dupliquer theeadds
  if (!eadds.IsEmpty()) {
    NCollection_DataMap<TCollection_AsciiString, Standard_Integer>::Iterator itad(eadds);
    for (; itad.More(); itad.Next()) theeadds.Bind (itad.Key(),itad.Value());
  }

//  on duplique la string
  if (!thehval.IsNull()) thehval = new TCollection_HAsciiString (other->CStringValue());
}


    void  MoniTool_TypedValue::Internals
  (MoniTool_ValueInterpret& interp, MoniTool_ValueSatisfies& satisf,
   Standard_CString& satisname,
    NCollection_DataMap<TCollection_AsciiString, Standard_Integer>& enums) const
{  interp = theinterp; satisf = thesatisf; satisname = thesatisn.ToCString();
   enums = theeadds;  }

    Standard_CString  MoniTool_TypedValue::Name   () const
      {  return thename.ToCString();    }

    MoniTool_ValueType  MoniTool_TypedValue::ValueType () const
      {  return thetype;  }

    TCollection_AsciiString  MoniTool_TypedValue::Definition () const
{
  if (thedef.Length() > 0) return thedef;
  TCollection_AsciiString def;
  char mess[50];
  switch (thetype) {
    case MoniTool_ValueInteger : {
      def.AssignCat("Integer");
      Standard_Integer ilim;
      if (IntegerLimit(Standard_False, ilim)) {
	Sprintf(mess,"  >= %d",ilim);
	def.AssignCat(mess);
      }
      if (IntegerLimit(Standard_True,  ilim)) {
	Sprintf(mess,"  <= %d",ilim);
	def.AssignCat(mess);
      }
    }
      break;
    case MoniTool_ValueReal    : {
      def.AssignCat("Real");
      Standard_Real rlim;
      if (RealLimit(Standard_False, rlim)) {
	Sprintf(mess,"  >= %f",rlim);
	def.AssignCat(mess);
      }
      if (RealLimit(Standard_True,  rlim)) {
	Sprintf(mess,"  <= %f",rlim);
	def.AssignCat(mess);
      }
      if (theunidef.Length() > 0)
	{  def.AssignCat("  Unit:");  def.AssignCat(UnitDef());  }
    }
      break;
    case MoniTool_ValueEnum    : {
      def.AssignCat("Enum");
      Standard_Integer startcase=0, endcase=0;
      Standard_Boolean match=0;
      EnumDef (startcase,endcase,match);
      Sprintf(mess," [%s%d-%d]",(match ? "in " : ""),startcase,endcase);
      def.AssignCat(mess);
      for (Standard_Integer i = startcase; i <= endcase; i ++) {
	Standard_CString enva = EnumVal(i);
	if (enva[0] == '?' || enva[0] == '\0') continue;
	Sprintf(mess," %d:%s",i,enva);
	def.AssignCat (mess);
      }
      if (!theeadds.IsEmpty()) {
        def.AssignCat(" , alpha: ");
        NCollection_DataMap<TCollection_AsciiString, Standard_Integer>::Iterator listadd(theeadds);
        for (; listadd.More(); listadd.Next()) {
          TCollection_AsciiString aName = listadd.Key();
          Standard_CString enva = aName.ToCString();
          if (enva[0] == '?') continue;
          Sprintf(mess,":%d ",listadd.Value());
          def.AssignCat (enva);
          def.AssignCat (mess);
        }
      }
    }
      break;
    case MoniTool_ValueIdent  : {
      def.AssignCat("Object(Entity)");
      if (!theotyp.IsNull()) {
	def.AssignCat(":");
	def.AssignCat(theotyp->Name());
      }
    }
      break;
    case MoniTool_ValueText    : {
      def.AssignCat("Text");
      if (themaxlen > 0) {
	Sprintf (mess," <= %d C.",themaxlen);
	def.AssignCat (mess);
      }
      break;
    }
    default :  def.AssignCat("(undefined)");  break;
  }
  return def;
}

    void  MoniTool_TypedValue::SetDefinition (const Standard_CString deftext)
      {  thedef.Clear();  thedef.AssignCat(deftext);  }

//  ##   Print   ##

    void  MoniTool_TypedValue::Print (const Handle(Message_Messenger)& S) const
{
  S<<"--- Typed Value : "<<Name();
  if (thelabel.Length() > 0) S<<"  Label : "<<Label();
  S<<endl<<"--- Type : "<<Definition()<<endl<<"--- Value : ";

  PrintValue (S);
  S<<endl;

  if (thesatisf) S<<" -- Specific Function for Satisfies : "<<thesatisn.ToCString()<<endl;
}


    void  MoniTool_TypedValue::PrintValue (const Handle(Message_Messenger)& S) const
{
  if (IsSetValue()) {
    if (thetype == MoniTool_ValueIdent)
      S<<" (type) "<<theoval->DynamicType()->Name();
    if (!thehval.IsNull())
      S<<(thetype == MoniTool_ValueIdent ? " : " : "")<<thehval->ToCString();

    if (HasInterpret()) {
      S<<"  (";
      Handle(TCollection_HAsciiString) str = Interpret (thehval,Standard_True);
      if (!str.IsNull() && str != thehval) S<<"Native:"<<str->ToCString();
      str = Interpret (thehval,Standard_False);
      if (!str.IsNull() && str != thehval) S<<"  Coded:"<<str->ToCString();
      S<<")";
    }
  }
  else S<<"(not set)";
}


//  #########    COMPLEMENTS    ##########

    Standard_Boolean  MoniTool_TypedValue::AddDef
  (const Standard_CString init)
{
//    Editions : init donne un petit texte d edition, en 2 termes "cmd var" :
  Standard_Integer i,iblc = 0;
  for (i = 0; init[i] != '\0'; i ++) if (init[i] == ' ') iblc = i+1;
  if (iblc == 0) return Standard_False;
//  Reconnaissance du sous-cas et aiguillage
  if      (init[0] == 'i' && init[2] == 'i')		// imin ival
    SetIntegerLimit (Standard_False,atoi(&init[iblc]));
  else if (init[0] == 'i' && init[2] == 'a')		// imax ival
    SetIntegerLimit (Standard_True ,atoi(&init[iblc]));
  else if (init[0] == 'r' && init[2] == 'i')		// rmin rval
    SetRealLimit (Standard_False,Atof(&init[iblc]));
  else if (init[0] == 'r' && init[2] == 'a')		// rmax rval
    SetRealLimit (Standard_True ,Atof(&init[iblc]));
  else if (init[0] == 'u')				// unit name
    SetUnitDef (&init[iblc]);
  else if (init[0] == 'e' && init[1] == 'm')		// ematch istart
    StartEnum (atoi(&init[iblc]),Standard_True);
  else if (init[0] == 'e' && init[1] == 'n')		// enum istart
    StartEnum (atoi(&init[iblc]),Standard_False);
  else if (init[0] == 'e' && init[1] == 'v')		// eval text
    AddEnum (&init[iblc]);
  else if (init[0] == 't' && init[1] == 'm')		// tmax length
    SetMaxLength (atoi(&init[iblc]));

  else return Standard_False;

  return Standard_True;
}


    void  MoniTool_TypedValue::SetLabel (const Standard_CString label)
      {  thelabel.Clear();  thelabel.AssignCat (label);  }

    Standard_CString  MoniTool_TypedValue::Label  () const
      {  return thelabel.ToCString();    }



    void  MoniTool_TypedValue::SetMaxLength
  (const Standard_Integer max)
      {  themaxlen = max;  if (max < 0) themaxlen = 0;  }

    Standard_Integer  MoniTool_TypedValue::MaxLength () const
      {  return themaxlen;  }

    void  MoniTool_TypedValue::SetIntegerLimit
  (const Standard_Boolean max, const Standard_Integer val)
{
  if (thetype != MoniTool_ValueInteger) throw Standard_ConstructionError("MoniTool_TypedValue : SetIntegerLimit, not an Integer");

  if (max)  {  thelims |= 2;  theintup  = val; }
  else      {  thelims |= 1;  theintlow = val; }
}

    Standard_Boolean  MoniTool_TypedValue::IntegerLimit
  (const Standard_Boolean max, Standard_Integer& val) const
{
  Standard_Boolean res = Standard_False;
  if (max) { res = (thelims & 2) != 0; val = (res ? theintup  : IntegerLast()); }
  else     { res = (thelims & 1) != 0; val = (res ? theintlow : IntegerFirst()); }
  return res;
}


    void  MoniTool_TypedValue::SetRealLimit
  (const Standard_Boolean max, const Standard_Real val)
{
  if (thetype != MoniTool_ValueReal) throw Standard_ConstructionError("MoniTool_TypedValue : SetRealLimit, not a Real");

  if (max)  {  thelims |= 2;  therealup = val; }
  else      {  thelims |= 1;  therealow = val; }
}

    Standard_Boolean  MoniTool_TypedValue::RealLimit
  (const Standard_Boolean max, Standard_Real& val) const
{
  Standard_Boolean res = Standard_False;
  if (max) { res = (thelims & 2) != 0; val = (res ? therealup : RealLast()); }
  else     { res = (thelims & 1) != 0; val = (res ? therealow : RealFirst()); }
  return res;
}

    void   MoniTool_TypedValue::SetUnitDef (const Standard_CString def)
      {  theunidef.Clear();  theunidef.AssignCat(def);  }

    Standard_CString  MoniTool_TypedValue::UnitDef () const
      {  return  theunidef.ToCString();  }

//  ******  les enums  ******

    void  MoniTool_TypedValue::StartEnum
  (const Standard_Integer start, const Standard_Boolean match)
{
  if (thetype != MoniTool_ValueEnum) throw Standard_ConstructionError("MoniTool_TypedValue : StartEnum, Not an Enum");

  thelims |= 4;  if (!match) thelims -= 4;
  theintlow = start;  theintup = start -1;
}

    void  MoniTool_TypedValue::AddEnum
  (const Standard_CString v1, const Standard_CString v2,
   const Standard_CString v3, const Standard_CString v4,
   const Standard_CString v5, const Standard_CString v6,
   const Standard_CString v7, const Standard_CString v8,
   const Standard_CString v9, const Standard_CString v10)
{
  if (thetype != MoniTool_ValueEnum) throw Standard_ConstructionError("MoniTool_TypedValue : AddEnum, Not an Enum");
  if (theenums.IsNull()) theenums =
    new TColStd_HArray1OfAsciiString(theintlow,theintlow+10);
  else if (theenums->Upper() < theintup + 10) {
    Handle(TColStd_HArray1OfAsciiString) enums =
      new TColStd_HArray1OfAsciiString(theintlow,theintup+10);
    for (Standard_Integer i = theintlow; i <= theintup; i ++)
      enums->SetValue(i,theenums->Value(i));
    theenums = enums;
  }

  if (v1[0] != '\0') {
    theintup ++;  theenums->SetValue(theintup,TCollection_AsciiString(v1));
    theeadds.Bind (v1,theintup);
  }
  if (v2[0] != '\0') {
    theintup ++;  theenums->SetValue(theintup,TCollection_AsciiString(v2));
    theeadds.Bind (v2,theintup);
  }
  if (v3[0] != '\0') {
    theintup ++;  theenums->SetValue(theintup,TCollection_AsciiString(v3));
    theeadds.Bind (v3,theintup);
  }
  if (v4[0] != '\0') {
    theintup ++;  theenums->SetValue(theintup,TCollection_AsciiString(v4));
    theeadds.Bind (v4,theintup);
  }
  if (v5[0] != '\0') {
    theintup ++;  theenums->SetValue(theintup,TCollection_AsciiString(v5));
    theeadds.Bind (v5,theintup);
  }
  if (v6[0] != '\0') {
    theintup ++;  theenums->SetValue(theintup,TCollection_AsciiString(v6));
    theeadds.Bind (v6,theintup);
  }
  if (v7[0] != '\0') {
    theintup ++;  theenums->SetValue(theintup,TCollection_AsciiString(v7));
    theeadds.Bind (v7,theintup);
  }
  if (v8[0] != '\0') {
    theintup ++;  theenums->SetValue(theintup,TCollection_AsciiString(v8));
    theeadds.Bind (v8,theintup);
  }
  if (v9[0] != '\0') {
    theintup ++;  theenums->SetValue(theintup,TCollection_AsciiString(v9));
    theeadds.Bind (v9,theintup);
  }
  if (v10[0] != '\0') {
    theintup ++;  theenums->SetValue(theintup,TCollection_AsciiString(v10));
    theeadds.Bind (v10,theintup);
  }
}

    void  MoniTool_TypedValue::AddEnumValue
  (const Standard_CString val, const Standard_Integer num)
{
  if (thetype != MoniTool_ValueEnum) throw Standard_ConstructionError("MoniTool_TypedValue : AddEnum, Not an Enum");
  if (num < theintlow) throw Standard_ConstructionError("MoniTool_TypedValue : AddEnum, out of range");
  if (val[0] == '\0') return;
  if (theenums.IsNull()) theenums =
    new TColStd_HArray1OfAsciiString(theintlow,num+1);
  else if (theenums->Upper() < num) {
    Handle(TColStd_HArray1OfAsciiString) enums =
      new TColStd_HArray1OfAsciiString(theintlow,num+1);
    for (Standard_Integer i = theintlow; i <= theintup; i ++)
      enums->SetValue(i,theenums->Value(i));
    theenums = enums;
  }

  if (theintup < num) theintup = num;
  if (theenums->Value(num).Length() == 0)
    theenums->SetValue(num,TCollection_AsciiString(val));
//    On met AUSSI dans le dictionnaire
//  else {
    theeadds.Bind (val,num);
//  }
}

    Standard_Boolean  MoniTool_TypedValue::EnumDef
  (Standard_Integer& startcase, Standard_Integer& endcase,
   Standard_Boolean& match) const
{
  if (thetype != MoniTool_ValueEnum) return Standard_False;
  startcase = theintlow;  endcase = theintup;
  match = ((thelims & 4) != 0);
  return Standard_True;
}

    Standard_CString  MoniTool_TypedValue::EnumVal
  (const Standard_Integer num) const
{
  if (thetype != MoniTool_ValueEnum) return "";
  if (num < theintlow || num > theintup) return "";
  return theenums->Value(num).ToCString();
}

    Standard_Integer  MoniTool_TypedValue::EnumCase
  (const Standard_CString val) const
{
  if (thetype != MoniTool_ValueEnum) return (theintlow - 1);
  Standard_Integer i; // svv Jan 10 2000 : porting on DEC
  for (i = theintlow; i <= theintup; i ++)
    if (theenums->Value(i).IsEqual(val)) return i;
//  cas additionnel ?
  if (!theeadds.IsEmpty()) {
    if (theeadds.Find(val,i)) return i;
  }
//  entier possible
  //gka S4054
  for (i = 0; val[i] != '\0'; i ++)
    if (val[i] != ' ' && val[i] != '-' && (val[i] < '0' || val[i] > '9' )) return (theintlow -1);
  return atoi(val);
}

//  ******  object/entity  ******

    void  MoniTool_TypedValue::SetObjectType
  (const Handle(Standard_Type)& typ)
{
  if (thetype != MoniTool_ValueIdent) throw Standard_ConstructionError("MoniTool_TypedValue : AddEnum, Not an Entity/Object");
  theotyp = typ;
}

    Handle(Standard_Type)  MoniTool_TypedValue::ObjectType () const
{
  if (!theotyp.IsNull()) return theotyp;
  return STANDARD_TYPE(Standard_Transient);
}

//  ******   Specific Interpret/Satisfy   ******

    void  MoniTool_TypedValue::SetInterpret
  (const MoniTool_ValueInterpret func)
      {  theinterp = func;  }

    Standard_Boolean  MoniTool_TypedValue::HasInterpret () const
{
  if (theinterp) return Standard_True;
  if (thetype == MoniTool_ValueEnum) return Standard_True;
  return Standard_False;
}

    void  MoniTool_TypedValue::SetSatisfies
  (const MoniTool_ValueSatisfies func, const Standard_CString name)
{
  thesatisn.Clear();
  thesatisf = func;
  if (thesatisf) thesatisn.AssignCat (name);
}

    Standard_CString MoniTool_TypedValue::SatisfiesName () const
      {  return thesatisn.ToCString();  }

//  ###########    VALEUR DU STATIC    ############

    Standard_Boolean MoniTool_TypedValue::IsSetValue () const
{
  if (thetype == MoniTool_ValueIdent) return (!theoval.IsNull());
  if (thehval->Length() > 0) return Standard_True;
  if (!theoval.IsNull()) return Standard_True;
  return Standard_False;
}

    Standard_CString MoniTool_TypedValue::CStringValue () const
      {  if (thehval.IsNull()) return "";  return thehval->ToCString();  }

    Handle(TCollection_HAsciiString) MoniTool_TypedValue::HStringValue () const
      {  return thehval;  }

    Handle(TCollection_HAsciiString)  MoniTool_TypedValue::Interpret
  (const Handle(TCollection_HAsciiString)& hval,
   const Standard_Boolean native) const
{
  Handle(TCollection_HAsciiString) inter = hval;
  if (hval.IsNull()) return hval;
  if (theinterp) return theinterp (this,hval,native);
  if (thetype == MoniTool_ValueEnum) {
//  On admet les deux formes : Enum de preference, sinon Integer
    Standard_Integer startcase, endcase; Standard_Boolean match;
    EnumDef (startcase,endcase,match);
    Standard_Integer encas = EnumCase (hval->ToCString());
    if (encas < startcase) return hval;    // loupe
    if (native) inter = new TCollection_HAsciiString (EnumVal(encas));
    else inter = new TCollection_HAsciiString (encas);
  }
  return inter;
}

    Standard_Boolean  MoniTool_TypedValue::Satisfies
  (const Handle(TCollection_HAsciiString)& val) const
{
  if (val.IsNull()) return Standard_False;
  if (thesatisf)
    if (!thesatisf (val) ) return Standard_False;
  if (val->Length() == 0) return Standard_True;
  switch (thetype) {
    case MoniTool_ValueInteger : {
      if (!val->IsIntegerValue()) return Standard_False;
      Standard_Integer ival, ilim;  ival = atoi(val->ToCString());
      if (IntegerLimit(Standard_False, ilim))
	if (ilim > ival) return Standard_False;
      if (IntegerLimit(Standard_True,  ilim))
	if (ilim < ival) return Standard_False;
      return Standard_True;
    }
    case MoniTool_ValueReal    : {
      if (!val->IsRealValue()) return Standard_False;
      Standard_Real rval, rlim;  rval = val->RealValue();
      if (RealLimit(Standard_False, rlim))
	if (rlim > rval) return Standard_False;
      if (RealLimit(Standard_True,  rlim))
	if (rlim < rval) return Standard_False;
      return Standard_True;
    }
    case MoniTool_ValueEnum    : {
//  On admet les deux formes : Enum de preference, sinon Integer
      Standard_Integer startcase, endcase;// unused ival;
      Standard_Boolean match;
      EnumDef (startcase,endcase,match);
      if (!match) return Standard_True;
      if (EnumCase (val->ToCString()) >= startcase) return Standard_True;
//  Ici, on admet un entier dans la fourchette
////      if (val->IsIntegerValue()) ival = atoi (val->ToCString());

// PTV 16.09.2000 The if is comment, cause this check is never been done (You can see the logic)
//      if (ival >= startcase && ival <= endcase) return Standard_True;
      return Standard_False;
    }
    case MoniTool_ValueText    : {
      if (themaxlen > 0 && val->Length() > themaxlen) return Standard_False;
      break;
    }
    default : break;
  }
  return Standard_True;
}

    void  MoniTool_TypedValue::ClearValue ()
{
  thehval.Nullify();
  theoval.Nullify();
  theival = 0;
}

    Standard_Boolean  MoniTool_TypedValue::SetCStringValue
  (const Standard_CString val)
{
  Handle(TCollection_HAsciiString) hval = new TCollection_HAsciiString(val);
  if (hval->IsSameString (thehval)) return Standard_True;
  if (!Satisfies(hval))  return Standard_False;
  if      (thetype == MoniTool_ValueInteger) {
    thehval->Clear();
    theival = atoi(val);
    thehval->AssignCat(val);
  }
  else if (thetype == MoniTool_ValueEnum) {
    Standard_Integer ival = EnumCase(val);
    Standard_CString cval = EnumVal(ival);
    if (!cval || cval[0] == '\0') return Standard_False;
    theival = ival;
    thehval->Clear();
    thehval->AssignCat(cval);
  } else {
    thehval->Clear();
    thehval->AssignCat(val);
    return Standard_True;
  }
  return Standard_True;
}

    Standard_Boolean  MoniTool_TypedValue::SetHStringValue
  (const Handle(TCollection_HAsciiString)& hval)
{
  if (hval.IsNull())     return Standard_False;
  if (!Satisfies(hval))  return Standard_False;
  thehval = hval;
  if      (thetype == MoniTool_ValueInteger) theival = atoi(hval->ToCString());
  else if (thetype == MoniTool_ValueEnum)    theival = EnumCase(hval->ToCString());
//  else return Standard_True;
  return Standard_True;
}

    Standard_Integer  MoniTool_TypedValue::IntegerValue () const
      {  return theival;  }

    Standard_Boolean  MoniTool_TypedValue::SetIntegerValue
  (const Standard_Integer ival)
{
  Handle(TCollection_HAsciiString) hval = new TCollection_HAsciiString(ival);
  if (hval->IsSameString (thehval)) return Standard_True;
  if (!Satisfies(hval))  return Standard_False;
  thehval->Clear();
  if (thetype == MoniTool_ValueEnum) thehval->AssignCat (EnumVal(ival));
  else thehval->AssignCat (hval->ToCString());
  theival = ival;
  return Standard_True;
}

    Standard_Real     MoniTool_TypedValue::RealValue () const
{
  if (thehval->Length() == 0)  return 0.0;
  if (!thehval->IsRealValue()) return 0.0;
  return thehval->RealValue();
}

    Standard_Boolean  MoniTool_TypedValue::SetRealValue (const Standard_Real rval)
{
  Handle(TCollection_HAsciiString) hval = new TCollection_HAsciiString(rval);
  if (hval->IsSameString (thehval)) return Standard_True;
  if (!Satisfies(hval))  return Standard_False;
  thehval->Clear();  thehval->AssignCat (hval->ToCString());
  return Standard_True;
}

    Handle(Standard_Transient)  MoniTool_TypedValue::ObjectValue () const
      {  return theoval;  }

    void  MoniTool_TypedValue::GetObjectValue (Handle(Standard_Transient)& val) const
      {  val = theoval;  }

    Standard_Boolean  MoniTool_TypedValue::SetObjectValue
  (const Handle(Standard_Transient)& obj)
{
  if (thetype != MoniTool_ValueIdent) return Standard_False;
  if (obj.IsNull()) { theoval.Nullify();  return Standard_True; }
  if (!theotyp.IsNull())
    if (!obj->IsKind(theotyp)) return Standard_False;
  theoval = obj;
  return Standard_True;
}


    Standard_CString  MoniTool_TypedValue::ObjectTypeName () const
{
  if (theoval.IsNull()) return "";
  Handle(MoniTool_Element) elm = Handle(MoniTool_Element)::DownCast(theoval);
  if (!elm.IsNull()) return elm->ValueTypeName();
  return theoval->DynamicType()->Name();
}


//    ########        LIBRARY        ########

    Standard_Boolean  MoniTool_TypedValue::AddLib
  (const Handle(MoniTool_TypedValue)& tv, const Standard_CString defin)
{
  if (tv.IsNull()) return Standard_False;
  if (defin[0] != '\0') tv->SetDefinition(defin);
//  else if (tv->Definition() == '\0') return Standard_False;
  libtv().Bind(tv->Name(),tv);
  return Standard_True;
}

    Handle(MoniTool_TypedValue)  MoniTool_TypedValue::Lib
  (const Standard_CString defin)
{
  Handle(MoniTool_TypedValue) val;
  Handle(Standard_Transient) aTVal;
  if (libtv().Find(defin, aTVal))
    val = Handle(MoniTool_TypedValue)::DownCast(aTVal);
  else
    val.Nullify();
  return val;
}

    Handle(MoniTool_TypedValue)  MoniTool_TypedValue::FromLib
  (const Standard_CString defin)
{
  Handle(MoniTool_TypedValue) val = MoniTool_TypedValue::Lib(defin);
  if (!val.IsNull()) val = new MoniTool_TypedValue (val);
  return val;
}

    Handle(TColStd_HSequenceOfAsciiString)  MoniTool_TypedValue::LibList ()
{
  Handle(TColStd_HSequenceOfAsciiString) list = new TColStd_HSequenceOfAsciiString();
  if (libtv().IsEmpty()) return list;
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>::Iterator it(libtv());
  for (; it.More();it.Next()) {
    list->Append (it.Key());
  }
  return list;
}

NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& MoniTool_TypedValue::Stats ()
{
  return astats;
}

    Handle(MoniTool_TypedValue)  MoniTool_TypedValue::StaticValue
  (const Standard_CString name)
{
  Handle(MoniTool_TypedValue) result;
  Handle(Standard_Transient) aTResult;
  if (Stats().Find(name, aTResult))
    result = Handle(MoniTool_TypedValue)::DownCast(aTResult);
  else
    result.Nullify();
  return result;
}
