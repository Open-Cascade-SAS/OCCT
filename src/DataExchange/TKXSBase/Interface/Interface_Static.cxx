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

#include <Interface_Static.hxx>

#include <OSD_Path.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

#include <cstdio>
IMPLEMENT_STANDARD_RTTIEXT(Interface_Static, Interface_TypedValue)

static char defmess[31];

//  Satisfies functions offered as standard ...

// svv #2
// static bool StaticPath(const occ::handle<TCollection_HAsciiString>& val)
//{
//   OSD_Path apath;
//   return apath.IsValid (TCollection_AsciiString(val->ToCString()));
// }

Interface_Static::Interface_Static(const char* const         family,
                                   const char* const         name,
                                   const Interface_ParamType type,
                                   const char* const         init)
    : Interface_TypedValue(name, type, init),
      thefamily(family),
      theupdate(true)
{
}

Interface_Static::Interface_Static(const char* const                    family,
                                   const char* const                    name,
                                   const occ::handle<Interface_Static>& other)
    : Interface_TypedValue(name, other->Type(), ""),
      thefamily(family),
      theupdate(true)
{
  switch (Type())
  {
    case Interface_ParamInteger: {
      int lim;
      if (other->IntegerLimit(true, lim))
        SetIntegerLimit(true, lim);
      if (other->IntegerLimit(false, lim))
        SetIntegerLimit(false, lim);
    }
    break;
    case Interface_ParamReal: {
      double lim;
      if (other->RealLimit(true, lim))
        SetRealLimit(true, lim);
      if (other->RealLimit(false, lim))
        SetRealLimit(false, lim);
      SetUnitDef(other->UnitDef());
    }
    break;
    case Interface_ParamEnum: {
      bool match;
      int  e0, e1, i;
      other->EnumDef(e0, e1, match);
      StartEnum(e0, match);
      //      if (e1 >= e0) theenums = new NCollection_HArray1<TCollection_AsciiString>(e0,e1);
      for (i = e0; i <= e1; i++)
        AddEnum(other->EnumVal(i));
    }
    break;
    case Interface_ParamIdent:
      SetObjectType(other->ObjectType());
      break;
    default:
      break;
  }

  if (other->IsSetValue())
    SetCStringValue(other->CStringValue());
}

//  ##   Print   ##

void Interface_Static::PrintStatic(Standard_OStream& S) const
{
  S << "--- Static Value : " << Name() << "  Family:" << Family();
  Print(S);
  if (!thewild.IsNull())
    S << " -- Attached to wild-card : " << thewild->Name() << std::endl;
  S << "--- Actual status : " << (theupdate ? "" : "original") << "  Value : ";

  if (thesatisf)
    S << " -- Specific Function for Satisfies : " << thesatisn.ToCString() << std::endl;
}

//  #########    COMPLEMENTS    ##########

const char* Interface_Static::Family() const
{
  return thefamily.ToCString();
}

occ::handle<Interface_Static> Interface_Static::Wild() const
{
  return thewild;
}

void Interface_Static::SetWild(const occ::handle<Interface_Static>& wild)
{
  thewild = wild;
}

//  #########   UPDATE    ##########

void Interface_Static::SetUptodate()
{
  theupdate = true;
}

bool Interface_Static::UpdatedStatus() const
{
  return theupdate;
}

//  #######################################################################
//  #########    STATICS DICTIONARY (static on Static)    ##########

bool Interface_Static::Init(const char* const         family,
                            const char* const         name,
                            const Interface_ParamType type,
                            const char* const         init)
{
  if (name[0] == '\0')
    return false;

  if (MoniTool_TypedValue::Stats().IsBound(name))
    return false;
  occ::handle<Interface_Static> item;
  if (type == Interface_ParamMisc)
  {
    occ::handle<Interface_Static> other = Interface_Static::Static(init);
    if (other.IsNull())
      return false;
    item = new Interface_Static(family, name, other);
  }
  else
    item = new Interface_Static(family, name, type, init);

  MoniTool_TypedValue::Stats().Bind(name, item);
  return true;
}

bool Interface_Static::Init(const char* const family,
                            const char* const name,
                            const char        type,
                            const char* const init)
{
  Interface_ParamType epyt;
  switch (type)
  {
    case 'e':
      epyt = Interface_ParamEnum;
      break;
    case 'i':
      epyt = Interface_ParamInteger;
      break;
    case 'o':
      epyt = Interface_ParamIdent;
      break;
    case 'p':
      epyt = Interface_ParamText;
      break;
    case 'r':
      epyt = Interface_ParamReal;
      break;
    case 't':
      epyt = Interface_ParamText;
      break;
    case '=':
      epyt = Interface_ParamMisc;
      break;
    case '&': {
      occ::handle<Interface_Static> unstat = Interface_Static::Static(name);
      if (unstat.IsNull())
        return false;
      //    Editions : init gives a small edition text, in 2 terms "cmd var" :
      //  imin <ival>  imax <ival>  rmin <rval>  rmax <rval>  unit <def>
      //  enum <from>  ematch <from>  eval <cval>
      int i, iblc = 0;
      for (i = 0; init[i] != '\0'; i++)
        if (init[i] == ' ')
          iblc = i + 1;
      //  Recognition of the sub-case and routing
      if (init[0] == 'i' && init[2] == 'i')
        unstat->SetIntegerLimit(false, atoi(&init[iblc]));
      else if (init[0] == 'i' && init[2] == 'a')
        unstat->SetIntegerLimit(true, atoi(&init[iblc]));
      else if (init[0] == 'r' && init[2] == 'i')
        unstat->SetRealLimit(false, Atof(&init[iblc]));
      else if (init[0] == 'r' && init[2] == 'a')
        unstat->SetRealLimit(true, Atof(&init[iblc]));
      else if (init[0] == 'u')
        unstat->SetUnitDef(&init[iblc]);
      else if (init[0] == 'e' && init[1] == 'm')
        unstat->StartEnum(atoi(&init[iblc]), true);
      else if (init[0] == 'e' && init[1] == 'n')
        unstat->StartEnum(atoi(&init[iblc]), false);
      else if (init[0] == 'e' && init[1] == 'v')
        unstat->AddEnum(&init[iblc]);
      else
        return false;
      return true;
    }
    default:
      return false;
  }
  if (!Interface_Static::Init(family, name, epyt, init))
    return false;
  if (type != 'p')
    return true;
  occ::handle<Interface_Static> stat = Interface_Static::Static(name);
  // NT  stat->SetSatisfies (StaticPath,"Path");
  if (!stat->Satisfies(stat->HStringValue()))
    stat->SetCStringValue("");
  return true;
}

occ::handle<Interface_Static> Interface_Static::Static(const char* const name)
{
  occ::handle<Standard_Transient> result;
  MoniTool_TypedValue::Stats().Find(name, result);
  return occ::down_cast<Interface_Static>(result);
}

bool Interface_Static::IsPresent(const char* const name)
{
  return MoniTool_TypedValue::Stats().IsBound(name);
}

const char* Interface_Static::CDef(const char* const name, const char* const part)
{
  if (!part || part[0] == '\0')
    return "";
  occ::handle<Interface_Static> stat = Interface_Static::Static(name);
  if (stat.IsNull())
    return "";
  if (part[0] == 'f' && part[1] == 'a')
    return stat->Family();
  if (part[0] == 'l' && part[1] == 'a')
    return stat->Label();
  if (part[0] == 's' && part[1] == 'a')
    return stat->SatisfiesName();
  if (part[0] == 't' && part[1] == 'y')
  {
    Interface_ParamType typ = stat->Type();
    if (typ == Interface_ParamInteger)
      return "integer";
    if (typ == Interface_ParamReal)
      return "real";
    if (typ == Interface_ParamText)
      return "text";
    if (typ == Interface_ParamEnum)
      return "enum";
    return "?type?";
  }
  if (part[0] == 'e')
  {
    int nume = 0;
    sscanf(part, "%30s %d", defmess, &nume);
    return stat->EnumVal(nume);
  }
  if (part[0] == 'i')
  {
    int ilim;
    if (!stat->IntegerLimit((part[2] == 'a'), ilim))
      return "";
    Sprintf(defmess, "%d", ilim);
    return defmess;
  }
  if (part[0] == 'r')
  {
    double rlim;
    if (!stat->RealLimit((part[2] == 'a'), rlim))
      return "";
    Sprintf(defmess, "%f", rlim);
    return defmess;
  }
  if (part[0] == 'u')
    return stat->UnitDef();
  return "";
}

int Interface_Static::IDef(const char* const name, const char* const part)
{
  if (!part || part[0] == '\0')
    return 0;
  occ::handle<Interface_Static> stat = Interface_Static::Static(name);
  if (stat.IsNull())
    return 0;
  if (part[0] == 'i')
  {
    int ilim;
    if (!stat->IntegerLimit((part[2] == 'a'), ilim))
      return 0;
    return ilim;
  }
  if (part[0] == 'e')
  {
    int  startcase, endcase;
    bool match;
    stat->EnumDef(startcase, endcase, match);
    if (part[1] == 's')
      return startcase;
    if (part[1] == 'c')
      return (endcase - startcase + 1);
    if (part[1] == 'm')
      return (match ? 1 : 0);
    if (part[1] == 'v')
    {
      char vale[51];
      sscanf(part, "%30s %50s", defmess, vale);
      return stat->EnumCase(vale);
    }
  }
  return 0;
}

//  ##########  CURRENT VALUE  ###########

bool Interface_Static::IsSet(const char* const name, const bool proper)
{
  occ::handle<Interface_Static> item = Interface_Static::Static(name);
  if (item.IsNull())
    return false;
  if (item->IsSetValue())
    return true;
  if (proper)
    return false;
  item = item->Wild();
  return item->IsSetValue();
}

const char* Interface_Static::CVal(const char* const name)
{
  occ::handle<Interface_Static> item = Interface_Static::Static(name);
  if (item.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: Interface_Static::CVal: incorrect parameter " << name << std::endl;
#endif
    return "";
  }
  return item->CStringValue();
}

int Interface_Static::IVal(const char* const name)
{
  occ::handle<Interface_Static> item = Interface_Static::Static(name);
  if (item.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: Interface_Static::IVal: incorrect parameter " << name << std::endl;
#endif
    return 0;
  }
  return item->IntegerValue();
}

double Interface_Static::RVal(const char* const name)
{
  occ::handle<Interface_Static> item = Interface_Static::Static(name);
  if (item.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: Interface_Static::RVal: incorrect parameter " << name << std::endl;
#endif
    return 0.0;
  }
  return item->RealValue();
}

bool Interface_Static::SetCVal(const char* const name, const char* const val)
{
  occ::handle<Interface_Static> item = Interface_Static::Static(name);
  if (item.IsNull())
    return false;
  return item->SetCStringValue(val);
}

bool Interface_Static::SetIVal(const char* const name, const int val)
{
  occ::handle<Interface_Static> item = Interface_Static::Static(name);
  if (item.IsNull())
    return false;
  if (!item->SetIntegerValue(val))
    return false;
  return true;
}

bool Interface_Static::SetRVal(const char* const name, const double val)
{
  occ::handle<Interface_Static> item = Interface_Static::Static(name);
  if (item.IsNull())
    return false;
  return item->SetRealValue(val);
}

//    UPDATE

bool Interface_Static::Update(const char* const name)
{
  occ::handle<Interface_Static> item = Interface_Static::Static(name);
  if (item.IsNull())
    return false;
  item->SetUptodate();
  return true;
}

bool Interface_Static::IsUpdated(const char* const name)
{
  occ::handle<Interface_Static> item = Interface_Static::Static(name);
  if (item.IsNull())
    return false;
  return item->UpdatedStatus();
}

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> Interface_Static::Items(
  const int         mode,
  const char* const criter)
{
  int modup = (mode / 100); // 0 any, 1 non-update, 2 update
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> list =
    new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>::Iterator iter(
    MoniTool_TypedValue::Stats());
  for (; iter.More(); iter.Next())
  {
    occ::handle<Interface_Static> item = occ::down_cast<Interface_Static>(iter.Value());
    if (item.IsNull())
      continue;
    bool ok = true;
    if (criter[0] == '$' && criter[1] == '\0')
    {
      if ((item->Family())[0] != '$')
        ok = false;
    }
    else if (criter[0] != '\0')
    {
      if (strcmp(criter, item->Family()))
        continue;
      ok = true;
    }
    else
    { // all ... except family with $
      if (item->Family()[0] == '$')
        continue;
    }
    if (ok && (modup == 1))
      ok = !item->UpdatedStatus();
    if (ok && (modup == 2))
      ok = item->UpdatedStatus();

    if (ok)
      list->Append(new TCollection_HAsciiString(iter.Key()));
  }
  return list;
}

//=======================================================================
// function : FillMap
// purpose  : Fills given string-to-string map with all static data
//=======================================================================
void Interface_Static::FillMap(
  NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>& theMap)
{
  theMap.Clear();

  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>& aMap =
    MoniTool_TypedValue::Stats();

  for (NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>::Iterator anIt(
         aMap);
       anIt.More();
       anIt.Next())
  {
    occ::handle<Interface_Static> aValue = occ::down_cast<Interface_Static>(anIt.Value());
    if (aValue.IsNull())
    {
      continue;
    }
    if (aValue->HStringValue().IsNull())
    {
      continue;
    }

    theMap.Bind(anIt.Key(), aValue->HStringValue()->String());
  }
}
