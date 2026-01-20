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

#include <MoniTool_TypedValue.hxx>

#include <MoniTool_Element.hxx>
#include <OSD_Path.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(MoniTool_TypedValue, Standard_Transient)

// Not Used :
// static  char defmess[30];
static NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> thelibtv;
static NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> astats;

static NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>& libtv()
{
  if (thelibtv.IsEmpty())
  {
    occ::handle<MoniTool_TypedValue> tv;
    tv = new MoniTool_TypedValue("Integer", MoniTool_ValueInteger);
    thelibtv.Bind("Integer", tv);
    tv = new MoniTool_TypedValue("Real", MoniTool_ValueReal);
    thelibtv.Bind("Real", tv);
    tv = new MoniTool_TypedValue("Text", MoniTool_ValueText);
    thelibtv.Bind("Text", tv);
    tv = new MoniTool_TypedValue("Transient", MoniTool_ValueIdent);
    thelibtv.Bind("Transient", tv);
    tv = new MoniTool_TypedValue("Boolean", MoniTool_ValueEnum);
    tv->AddDef("enum 0"); //    = 0 False  ,  > 0 True
    tv->AddDef("eval False");
    tv->AddDef("eval True");
    thelibtv.Bind("Boolean", tv);
    tv = new MoniTool_TypedValue("Logical", MoniTool_ValueEnum);
    tv->AddDef("enum -1"); //    < 0 False  ,  = 0 Unk  ,  > 0 True
    tv->AddDef("eval False");
    tv->AddDef("eval Unknown");
    tv->AddDef("eval True");
    thelibtv.Bind("Logical", tv);
  }
  return thelibtv;
}

//  Fonctions Satisfies offertes en standard ...
/* Not Used

static bool StaticPath(const occ::handle<TCollection_HAsciiString>& val)
{
  OSD_Path apath;
  return apath.IsValid (TCollection_AsciiString(val->ToCString()));
}
*/

MoniTool_TypedValue::MoniTool_TypedValue(const char* const   name,
                                         const MoniTool_ValueType type,
                                         const char* const   init)
    : thename(name),
      thetype(type),
      thelims(0),
      themaxlen(0),
      theintlow(0),
      theintup(-1),
      therealow(0.0),
      therealup(0.0),
      theinterp(NULL),
      thesatisf(NULL),
      theival(0),
      thehval(new TCollection_HAsciiString(""))
{
  if (type != MoniTool_ValueInteger && type != MoniTool_ValueReal && type != MoniTool_ValueEnum
      && type != MoniTool_ValueText && type != MoniTool_ValueIdent)
    throw Standard_ConstructionError("MoniTool_TypedValue : Type not supported");
  if (init[0] != '\0')
    if (Satisfies(new TCollection_HAsciiString(init)))
      SetCStringValue(init);
}

MoniTool_TypedValue::MoniTool_TypedValue(const occ::handle<MoniTool_TypedValue>& other)
    : thename(other->Name()),
      thedef(other->Definition()),
      thelabel(other->Label()),
      thetype(other->ValueType()),
      theotyp(other->ObjectType()),
      thelims(0),
      themaxlen(other->MaxLength()),
      theintlow(0),
      theintup(0),
      therealow(0),
      therealup(0),
      theunidef(other->UnitDef()),
      theival(other->IntegerValue()),
      thehval(other->HStringValue()),
      theoval(other->ObjectValue())
{
  NCollection_DataMap<TCollection_AsciiString, int> eadds;
  const char*                                               satisname;
  other->Internals(theinterp, thesatisf, satisname, eadds);
  thesatisn.AssignCat(satisname);

  if (other->IntegerLimit(false, theintlow))
    thelims |= 1;
  if (other->IntegerLimit(true, theintup))
    thelims |= 2;
  if (other->RealLimit(false, therealow))
    thelims |= 1;
  if (other->RealLimit(true, therealup))
    thelims |= 2;

  int startcase, endcase;
  bool match;
  if (other->EnumDef(startcase, endcase, match))
  {
    theintlow = startcase;
    theintup  = endcase;
    if (match)
      thelims |= 4;
    if (theintup >= theintlow)
      theenums = new NCollection_HArray1<TCollection_AsciiString>(theintlow, theintup);
    for (startcase = theintlow; startcase <= theintup; startcase++)
    {
      theenums->SetValue(startcase, other->EnumVal(startcase));
    }
  }
  //  dupliquer theeadds
  if (!eadds.IsEmpty())
  {
    NCollection_DataMap<TCollection_AsciiString, int>::Iterator itad(eadds);
    for (; itad.More(); itad.Next())
      theeadds.Bind(itad.Key(), itad.Value());
  }

  //  we duplicate the string
  if (!thehval.IsNull())
    thehval = new TCollection_HAsciiString(other->CStringValue());
}

void MoniTool_TypedValue::Internals(
  MoniTool_ValueInterpret&                                        interp,
  MoniTool_ValueSatisfies&                                        satisf,
  const char*&                                               satisname,
  NCollection_DataMap<TCollection_AsciiString, int>& enums) const
{
  interp    = theinterp;
  satisf    = thesatisf;
  satisname = thesatisn.ToCString();
  enums     = theeadds;
}

const char* MoniTool_TypedValue::Name() const
{
  return thename.ToCString();
}

MoniTool_ValueType MoniTool_TypedValue::ValueType() const
{
  return thetype;
}

TCollection_AsciiString MoniTool_TypedValue::Definition() const
{
  if (thedef.Length() > 0)
    return thedef;
  TCollection_AsciiString def;
  char                    mess[50];
  switch (thetype)
  {
    case MoniTool_ValueInteger: {
      def.AssignCat("Integer");
      int ilim;
      if (IntegerLimit(false, ilim))
      {
        Sprintf(mess, "  >= %d", ilim);
        def.AssignCat(mess);
      }
      if (IntegerLimit(true, ilim))
      {
        Sprintf(mess, "  <= %d", ilim);
        def.AssignCat(mess);
      }
    }
    break;
    case MoniTool_ValueReal: {
      def.AssignCat("Real");
      double rlim;
      if (RealLimit(false, rlim))
      {
        Sprintf(mess, "  >= %f", rlim);
        def.AssignCat(mess);
      }
      if (RealLimit(true, rlim))
      {
        Sprintf(mess, "  <= %f", rlim);
        def.AssignCat(mess);
      }
      if (theunidef.Length() > 0)
      {
        def.AssignCat("  Unit:");
        def.AssignCat(UnitDef());
      }
    }
    break;
    case MoniTool_ValueEnum: {
      def.AssignCat("Enum");
      int startcase = 0, endcase = 0;
      bool match = 0;
      EnumDef(startcase, endcase, match);
      Sprintf(mess, " [%s%d-%d]", (match ? "in " : ""), startcase, endcase);
      def.AssignCat(mess);
      for (int i = startcase; i <= endcase; i++)
      {
        const char* enva = EnumVal(i);
        if (enva[0] == '?' || enva[0] == '\0')
          continue;
        Sprintf(mess, " %d:%s", i, enva);
        def.AssignCat(mess);
      }
      if (!theeadds.IsEmpty())
      {
        def.AssignCat(" , alpha: ");
        NCollection_DataMap<TCollection_AsciiString, int>::Iterator listadd(theeadds);
        for (; listadd.More(); listadd.Next())
        {
          const TCollection_AsciiString& aName = listadd.Key();
          const char*               enva  = aName.ToCString();
          if (enva[0] == '?')
            continue;
          Sprintf(mess, ":%d ", listadd.Value());
          def.AssignCat(enva);
          def.AssignCat(mess);
        }
      }
    }
    break;
    case MoniTool_ValueIdent: {
      def.AssignCat("Object(Entity)");
      if (!theotyp.IsNull())
      {
        def.AssignCat(":");
        def.AssignCat(theotyp->Name());
      }
    }
    break;
    case MoniTool_ValueText: {
      def.AssignCat("Text");
      if (themaxlen > 0)
      {
        Sprintf(mess, " <= %d C.", themaxlen);
        def.AssignCat(mess);
      }
      break;
    }
    default:
      def.AssignCat("(undefined)");
      break;
  }
  return def;
}

void MoniTool_TypedValue::SetDefinition(const char* const deftext)
{
  thedef.Clear();
  thedef.AssignCat(deftext);
}

//  ##   Print   ##

void MoniTool_TypedValue::Print(Standard_OStream& S) const
{
  S << "--- Typed Value : " << Name();
  if (thelabel.Length() > 0)
    S << "  Label : " << Label();
  S << std::endl << "--- Type : " << Definition() << std::endl << "--- Value : ";

  PrintValue(S);
  S << std::endl;

  if (thesatisf)
    S << " -- Specific Function for Satisfies : " << thesatisn.ToCString() << std::endl;
}

void MoniTool_TypedValue::PrintValue(Standard_OStream& S) const
{
  if (IsSetValue())
  {
    if (thetype == MoniTool_ValueIdent)
      S << " (type) " << theoval->DynamicType()->Name();
    if (!thehval.IsNull())
      S << (thetype == MoniTool_ValueIdent ? " : " : "") << thehval->ToCString();

    if (HasInterpret())
    {
      S << "  (";
      occ::handle<TCollection_HAsciiString> str = Interpret(thehval, true);
      if (!str.IsNull() && str != thehval)
        S << "Native:" << str->ToCString();
      str = Interpret(thehval, false);
      if (!str.IsNull() && str != thehval)
        S << "  Coded:" << str->ToCString();
      S << ")";
    }
  }
  else
    S << "(not set)";
}

//  #########    COMPLEMENTS    ##########

bool MoniTool_TypedValue::AddDef(const char* const init)
{
  //    Editions : init gives a small edition text, in 2 terms "cmd var" :
  int i, iblc = 0;
  for (i = 0; init[i] != '\0'; i++)
    if (init[i] == ' ')
      iblc = i + 1;
  if (iblc == 0)
    return false;
  //  Recognition of sub-case and routing
  if (init[0] == 'i' && init[2] == 'i') // imin ival
    SetIntegerLimit(false, atoi(&init[iblc]));
  else if (init[0] == 'i' && init[2] == 'a') // imax ival
    SetIntegerLimit(true, atoi(&init[iblc]));
  else if (init[0] == 'r' && init[2] == 'i') // rmin rval
    SetRealLimit(false, Atof(&init[iblc]));
  else if (init[0] == 'r' && init[2] == 'a') // rmax rval
    SetRealLimit(true, Atof(&init[iblc]));
  else if (init[0] == 'u') // unit name
    SetUnitDef(&init[iblc]);
  else if (init[0] == 'e' && init[1] == 'm') // ematch istart
    StartEnum(atoi(&init[iblc]), true);
  else if (init[0] == 'e' && init[1] == 'n') // enum istart
    StartEnum(atoi(&init[iblc]), false);
  else if (init[0] == 'e' && init[1] == 'v') // eval text
    AddEnum(&init[iblc]);
  else if (init[0] == 't' && init[1] == 'm') // tmax length
    SetMaxLength(atoi(&init[iblc]));

  else
    return false;

  return true;
}

void MoniTool_TypedValue::SetLabel(const char* const label)
{
  thelabel.Clear();
  thelabel.AssignCat(label);
}

const char* MoniTool_TypedValue::Label() const
{
  return thelabel.ToCString();
}

void MoniTool_TypedValue::SetMaxLength(const int max)
{
  themaxlen = max;
  if (max < 0)
    themaxlen = 0;
}

int MoniTool_TypedValue::MaxLength() const
{
  return themaxlen;
}

void MoniTool_TypedValue::SetIntegerLimit(const bool max, const int val)
{
  if (thetype != MoniTool_ValueInteger)
    throw Standard_ConstructionError("MoniTool_TypedValue : SetIntegerLimit, not an Integer");

  if (max)
  {
    thelims |= 2;
    theintup = val;
  }
  else
  {
    thelims |= 1;
    theintlow = val;
  }
}

bool MoniTool_TypedValue::IntegerLimit(const bool max,
                                                   int&      val) const
{
  bool res = false;
  if (max)
  {
    res = (thelims & 2) != 0;
    val = (res ? theintup : IntegerLast());
  }
  else
  {
    res = (thelims & 1) != 0;
    val = (res ? theintlow : IntegerFirst());
  }
  return res;
}

void MoniTool_TypedValue::SetRealLimit(const bool max, const double val)
{
  if (thetype != MoniTool_ValueReal)
    throw Standard_ConstructionError("MoniTool_TypedValue : SetRealLimit, not a Real");

  if (max)
  {
    thelims |= 2;
    therealup = val;
  }
  else
  {
    thelims |= 1;
    therealow = val;
  }
}

bool MoniTool_TypedValue::RealLimit(const bool max,
                                                double&         val) const
{
  bool res = false;
  if (max)
  {
    res = (thelims & 2) != 0;
    val = (res ? therealup : RealLast());
  }
  else
  {
    res = (thelims & 1) != 0;
    val = (res ? therealow : RealFirst());
  }
  return res;
}

void MoniTool_TypedValue::SetUnitDef(const char* const def)
{
  theunidef.Clear();
  theunidef.AssignCat(def);
}

const char* MoniTool_TypedValue::UnitDef() const
{
  return theunidef.ToCString();
}

//  ******  the enums  ******

void MoniTool_TypedValue::StartEnum(const int start, const bool match)
{
  if (thetype != MoniTool_ValueEnum)
    throw Standard_ConstructionError("MoniTool_TypedValue : StartEnum, Not an Enum");

  thelims |= 4;
  if (!match)
    thelims -= 4;
  theintlow = start;
  theintup  = start - 1;
}

void MoniTool_TypedValue::AddEnum(const char* const v1,
                                  const char* const v2,
                                  const char* const v3,
                                  const char* const v4,
                                  const char* const v5,
                                  const char* const v6,
                                  const char* const v7,
                                  const char* const v8,
                                  const char* const v9,
                                  const char* const v10)
{
  if (thetype != MoniTool_ValueEnum)
    throw Standard_ConstructionError("MoniTool_TypedValue : AddEnum, Not an Enum");
  if (theenums.IsNull())
    theenums = new NCollection_HArray1<TCollection_AsciiString>(theintlow, theintlow + 10);
  else if (theenums->Upper() < theintup + 10)
  {
    occ::handle<NCollection_HArray1<TCollection_AsciiString>> enums =
      new NCollection_HArray1<TCollection_AsciiString>(theintlow, theintup + 10);
    for (int i = theintlow; i <= theintup; i++)
      enums->SetValue(i, theenums->Value(i));
    theenums = enums;
  }

  if (v1[0] != '\0')
  {
    theintup++;
    theenums->SetValue(theintup, TCollection_AsciiString(v1));
    theeadds.Bind(v1, theintup);
  }
  if (v2[0] != '\0')
  {
    theintup++;
    theenums->SetValue(theintup, TCollection_AsciiString(v2));
    theeadds.Bind(v2, theintup);
  }
  if (v3[0] != '\0')
  {
    theintup++;
    theenums->SetValue(theintup, TCollection_AsciiString(v3));
    theeadds.Bind(v3, theintup);
  }
  if (v4[0] != '\0')
  {
    theintup++;
    theenums->SetValue(theintup, TCollection_AsciiString(v4));
    theeadds.Bind(v4, theintup);
  }
  if (v5[0] != '\0')
  {
    theintup++;
    theenums->SetValue(theintup, TCollection_AsciiString(v5));
    theeadds.Bind(v5, theintup);
  }
  if (v6[0] != '\0')
  {
    theintup++;
    theenums->SetValue(theintup, TCollection_AsciiString(v6));
    theeadds.Bind(v6, theintup);
  }
  if (v7[0] != '\0')
  {
    theintup++;
    theenums->SetValue(theintup, TCollection_AsciiString(v7));
    theeadds.Bind(v7, theintup);
  }
  if (v8[0] != '\0')
  {
    theintup++;
    theenums->SetValue(theintup, TCollection_AsciiString(v8));
    theeadds.Bind(v8, theintup);
  }
  if (v9[0] != '\0')
  {
    theintup++;
    theenums->SetValue(theintup, TCollection_AsciiString(v9));
    theeadds.Bind(v9, theintup);
  }
  if (v10[0] != '\0')
  {
    theintup++;
    theenums->SetValue(theintup, TCollection_AsciiString(v10));
    theeadds.Bind(v10, theintup);
  }
}

void MoniTool_TypedValue::AddEnumValue(const char* const val, const int num)
{
  if (thetype != MoniTool_ValueEnum)
    throw Standard_ConstructionError("MoniTool_TypedValue : AddEnum, Not an Enum");
  if (num < theintlow)
    throw Standard_ConstructionError("MoniTool_TypedValue : AddEnum, out of range");
  if (val[0] == '\0')
    return;
  if (theenums.IsNull())
    theenums = new NCollection_HArray1<TCollection_AsciiString>(theintlow, num + 1);
  else if (theenums->Upper() < num)
  {
    occ::handle<NCollection_HArray1<TCollection_AsciiString>> enums =
      new NCollection_HArray1<TCollection_AsciiString>(theintlow, num + 1);
    for (int i = theintlow; i <= theintup; i++)
      enums->SetValue(i, theenums->Value(i));
    theenums = enums;
  }

  if (theintup < num)
    theintup = num;
  if (theenums->Value(num).Length() == 0)
  {
    theenums->SetValue(num, TCollection_AsciiString(val));
  }
  //    We ALSO put in the dictionary
  //  else {
  theeadds.Bind(val, num);
  //  }
}

bool MoniTool_TypedValue::EnumDef(int& startcase,
                                              int& endcase,
                                              bool& match) const
{
  if (thetype != MoniTool_ValueEnum)
    return false;
  startcase = theintlow;
  endcase   = theintup;
  match     = ((thelims & 4) != 0);
  return true;
}

const char* MoniTool_TypedValue::EnumVal(const int num) const
{
  if (thetype != MoniTool_ValueEnum)
    return "";
  if (num < theintlow || num > theintup)
    return "";
  return theenums->Value(num).ToCString();
}

int MoniTool_TypedValue::EnumCase(const char* const val) const
{
  if (thetype != MoniTool_ValueEnum)
    return (theintlow - 1);
  int i; // svv Jan 10 2000 : porting on DEC
  for (i = theintlow; i <= theintup; i++)
    if (theenums->Value(i).IsEqual(val))
      return i;
  //  cas additionnel ?
  if (!theeadds.IsEmpty())
  {
    if (theeadds.Find(val, i))
      return i;
  }
  //  entier possible
  // gka S4054
  for (i = 0; val[i] != '\0'; i++)
    if (val[i] != ' ' && val[i] != '-' && (val[i] < '0' || val[i] > '9'))
      return (theintlow - 1);
  return atoi(val);
}

//  ******  object/entity  ******

void MoniTool_TypedValue::SetObjectType(const occ::handle<Standard_Type>& typ)
{
  if (thetype != MoniTool_ValueIdent)
    throw Standard_ConstructionError("MoniTool_TypedValue : AddEnum, Not an Entity/Object");
  theotyp = typ;
}

occ::handle<Standard_Type> MoniTool_TypedValue::ObjectType() const
{
  if (!theotyp.IsNull())
    return theotyp;
  return STANDARD_TYPE(Standard_Transient);
}

//  ******   Specific Interpret/Satisfy   ******

void MoniTool_TypedValue::SetInterpret(const MoniTool_ValueInterpret func)
{
  theinterp = func;
}

bool MoniTool_TypedValue::HasInterpret() const
{
  if (theinterp)
    return true;
  if (thetype == MoniTool_ValueEnum)
    return true;
  return false;
}

void MoniTool_TypedValue::SetSatisfies(const MoniTool_ValueSatisfies func,
                                       const char* const        name)
{
  thesatisn.Clear();
  thesatisf = func;
  if (thesatisf)
    thesatisn.AssignCat(name);
}

const char* MoniTool_TypedValue::SatisfiesName() const
{
  return thesatisn.ToCString();
}

//  ###########    STATIC VALUE    ############

bool MoniTool_TypedValue::IsSetValue() const
{
  if (thetype == MoniTool_ValueIdent)
    return (!theoval.IsNull());
  if (thehval->Length() > 0)
    return true;
  if (!theoval.IsNull())
    return true;
  return false;
}

const char* MoniTool_TypedValue::CStringValue() const
{
  if (thehval.IsNull())
    return "";
  return thehval->ToCString();
}

occ::handle<TCollection_HAsciiString> MoniTool_TypedValue::HStringValue() const
{
  return thehval;
}

occ::handle<TCollection_HAsciiString> MoniTool_TypedValue::Interpret(
  const occ::handle<TCollection_HAsciiString>& hval,
  const bool                  native) const
{
  occ::handle<TCollection_HAsciiString> inter = hval;
  if (hval.IsNull())
    return hval;
  if (theinterp)
    return theinterp(this, hval, native);
  if (thetype == MoniTool_ValueEnum)
  {
    //  We accept both forms : Enum preferably, otherwise Integer
    int startcase, endcase;
    bool match;
    EnumDef(startcase, endcase, match);
    int encas = EnumCase(hval->ToCString());
    if (encas < startcase)
      return hval; // loupe
    if (native)
      inter = new TCollection_HAsciiString(EnumVal(encas));
    else
      inter = new TCollection_HAsciiString(encas);
  }
  return inter;
}

bool MoniTool_TypedValue::Satisfies(const occ::handle<TCollection_HAsciiString>& val) const
{
  if (val.IsNull())
    return false;
  if (thesatisf)
    if (!thesatisf(val))
      return false;
  if (val->Length() == 0)
    return true;
  switch (thetype)
  {
    case MoniTool_ValueInteger: {
      if (!val->IsIntegerValue())
        return false;
      int ival, ilim;
      ival = atoi(val->ToCString());
      if (IntegerLimit(false, ilim))
        if (ilim > ival)
          return false;
      if (IntegerLimit(true, ilim))
        if (ilim < ival)
          return false;
      return true;
    }
    case MoniTool_ValueReal: {
      if (!val->IsRealValue())
        return false;
      double rval, rlim;
      rval = val->RealValue();
      if (RealLimit(false, rlim))
        if (rlim > rval)
          return false;
      if (RealLimit(true, rlim))
        if (rlim < rval)
          return false;
      return true;
    }
    case MoniTool_ValueEnum: {
      //  We accept both forms : Enum preferably, otherwise Integer
      int startcase, endcase; // unused ival;
      bool match;
      EnumDef(startcase, endcase, match);
      if (!match)
        return true;
      if (EnumCase(val->ToCString()) >= startcase)
        return true;
      //  Here, we accept an integer in the range
      ////      if (val->IsIntegerValue()) ival = atoi (val->ToCString());

      // PTV 16.09.2000 The if is comment, cause this check is never been done (You can see the
      // logic)
      //      if (ival >= startcase && ival <= endcase) return true;
      return false;
    }
    case MoniTool_ValueText: {
      if (themaxlen > 0 && val->Length() > themaxlen)
        return false;
      break;
    }
    default:
      break;
  }
  return true;
}

void MoniTool_TypedValue::ClearValue()
{
  thehval.Nullify();
  theoval.Nullify();
  theival = 0;
}

bool MoniTool_TypedValue::SetCStringValue(const char* const val)
{
  occ::handle<TCollection_HAsciiString> hval = new TCollection_HAsciiString(val);
  if (hval->IsSameString(thehval))
    return true;
  if (!Satisfies(hval))
    return false;
  if (thetype == MoniTool_ValueInteger)
  {
    thehval->Clear();
    theival = atoi(val);
    thehval->AssignCat(val);
  }
  else if (thetype == MoniTool_ValueEnum)
  {
    int ival = EnumCase(val);
    const char* cval = EnumVal(ival);
    if (!cval || cval[0] == '\0')
      return false;
    theival = ival;
    thehval->Clear();
    thehval->AssignCat(cval);
  }
  else
  {
    thehval->Clear();
    thehval->AssignCat(val);
    return true;
  }
  return true;
}

bool MoniTool_TypedValue::SetHStringValue(const occ::handle<TCollection_HAsciiString>& hval)
{
  if (hval.IsNull())
    return false;
  if (!Satisfies(hval))
    return false;
  thehval = hval;
  if (thetype == MoniTool_ValueInteger)
    theival = atoi(hval->ToCString());
  else if (thetype == MoniTool_ValueEnum)
    theival = EnumCase(hval->ToCString());
  //  else return true;
  return true;
}

int MoniTool_TypedValue::IntegerValue() const
{
  return theival;
}

bool MoniTool_TypedValue::SetIntegerValue(const int ival)
{
  occ::handle<TCollection_HAsciiString> hval = new TCollection_HAsciiString(ival);
  if (hval->IsSameString(thehval))
    return true;
  if (!Satisfies(hval))
    return false;
  thehval->Clear();
  if (thetype == MoniTool_ValueEnum)
    thehval->AssignCat(EnumVal(ival));
  else
    thehval->AssignCat(hval->ToCString());
  theival = ival;
  return true;
}

double MoniTool_TypedValue::RealValue() const
{
  if (thehval->Length() == 0)
    return 0.0;
  if (!thehval->IsRealValue())
    return 0.0;
  return thehval->RealValue();
}

bool MoniTool_TypedValue::SetRealValue(const double rval)
{
  occ::handle<TCollection_HAsciiString> hval = new TCollection_HAsciiString(rval);
  if (hval->IsSameString(thehval))
    return true;
  if (!Satisfies(hval))
    return false;
  thehval->Clear();
  thehval->AssignCat(hval->ToCString());
  return true;
}

occ::handle<Standard_Transient> MoniTool_TypedValue::ObjectValue() const
{
  return theoval;
}

void MoniTool_TypedValue::GetObjectValue(occ::handle<Standard_Transient>& val) const
{
  val = theoval;
}

bool MoniTool_TypedValue::SetObjectValue(const occ::handle<Standard_Transient>& obj)
{
  if (thetype != MoniTool_ValueIdent)
    return false;
  if (obj.IsNull())
  {
    theoval.Nullify();
    return true;
  }
  if (!theotyp.IsNull())
    if (!obj->IsKind(theotyp))
      return false;
  theoval = obj;
  return true;
}

const char* MoniTool_TypedValue::ObjectTypeName() const
{
  if (theoval.IsNull())
    return "";
  occ::handle<MoniTool_Element> elm = occ::down_cast<MoniTool_Element>(theoval);
  if (!elm.IsNull())
    return elm->ValueTypeName();
  return theoval->DynamicType()->Name();
}

//    ########        LIBRARY        ########

bool MoniTool_TypedValue::AddLib(const occ::handle<MoniTool_TypedValue>& tv,
                                             const char* const             defin)
{
  if (tv.IsNull())
    return false;
  if (defin[0] != '\0')
    tv->SetDefinition(defin);
  //  else if (tv->Definition() == '\0') return false;
  libtv().Bind(tv->Name(), tv);
  return true;
}

occ::handle<MoniTool_TypedValue> MoniTool_TypedValue::Lib(const char* const defin)
{
  occ::handle<MoniTool_TypedValue> val;
  occ::handle<Standard_Transient>  aTVal;
  if (libtv().Find(defin, aTVal))
    val = occ::down_cast<MoniTool_TypedValue>(aTVal);
  else
    val.Nullify();
  return val;
}

occ::handle<MoniTool_TypedValue> MoniTool_TypedValue::FromLib(const char* const defin)
{
  occ::handle<MoniTool_TypedValue> val = MoniTool_TypedValue::Lib(defin);
  if (!val.IsNull())
    val = new MoniTool_TypedValue(val);
  return val;
}

occ::handle<NCollection_HSequence<TCollection_AsciiString>> MoniTool_TypedValue::LibList()
{
  occ::handle<NCollection_HSequence<TCollection_AsciiString>> list = new NCollection_HSequence<TCollection_AsciiString>();
  if (libtv().IsEmpty())
    return list;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>::Iterator it(libtv());
  for (; it.More(); it.Next())
  {
    list->Append(it.Key());
  }
  return list;
}

NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>& MoniTool_TypedValue::
  Stats()
{
  return astats;
}

occ::handle<MoniTool_TypedValue> MoniTool_TypedValue::StaticValue(const char* const name)
{
  occ::handle<MoniTool_TypedValue> result;
  occ::handle<Standard_Transient>  aTResult;
  if (Stats().Find(name, aTResult))
    result = occ::down_cast<MoniTool_TypedValue>(aTResult);
  else
    result.Nullify();
  return result;
}
