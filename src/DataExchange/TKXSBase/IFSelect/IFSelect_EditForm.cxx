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

#include <IFSelect_EditForm.hxx>
#include <IFSelect_Editor.hxx>
#include <IFSelect_ListEditor.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_MSG.hxx>
#include <Interface_TypedValue.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_EditForm, Standard_Transient)

IFSelect_EditForm::IFSelect_EditForm(const occ::handle<IFSelect_Editor>& editor,
                                     const bool         readonly,
                                     const bool         undoable,
                                     const char*         label)
    : thecomplete(true),
      theloaded(false),
      thekeepst(false),
      thelabel(label),
      thenums(0, 1),
      theorigs(0, (undoable ? editor->NbValues() : 0)),
      themodifs(0, (readonly ? 0 : editor->NbValues())),
      thestatus(0, (readonly ? 0 : editor->NbValues())),
      theeditor(editor),
      thetouched(0)
{
}

IFSelect_EditForm::IFSelect_EditForm(const occ::handle<IFSelect_Editor>&   editor,
                                     const NCollection_Sequence<int>& nums,
                                     const bool           readonly,
                                     const bool           undoable,
                                     const char*           label)
    : thecomplete(false),
      theloaded(false),
      thekeepst(false),
      thelabel(label),
      thenums(0, nums.Length()),
      theorigs(0, (undoable ? nums.Length() : 0)),
      themodifs(0, (readonly ? 0 : nums.Length())),
      thestatus(0, (readonly ? 0 : nums.Length())),
      theeditor(editor),
      thetouched(0)
{
  int i, nb = nums.Length();
  for (i = 1; i <= nb; i++)
    thenums.SetValue(i, nums.Value(i));
}

bool& IFSelect_EditForm::EditKeepStatus()
{
  return thekeepst;
}

const char* IFSelect_EditForm::Label() const
{
  return thelabel.ToCString();
}

bool IFSelect_EditForm::IsLoaded() const
{
  return theloaded;
}

void IFSelect_EditForm::ClearData()
{
  theent.Nullify();
  themodel.Nullify();
  theloaded = false;
}

void IFSelect_EditForm::SetData(const occ::handle<Standard_Transient>&       ent,
                                const occ::handle<Interface_InterfaceModel>& model)
{
  theent   = ent;
  themodel = model;
}

void IFSelect_EditForm::SetEntity(const occ::handle<Standard_Transient>& ent)
{
  theent = ent;
}

void IFSelect_EditForm::SetModel(const occ::handle<Interface_InterfaceModel>& model)
{
  themodel = model;
}

occ::handle<Standard_Transient> IFSelect_EditForm::Entity() const
{
  return theent;
}

occ::handle<Interface_InterfaceModel> IFSelect_EditForm::Model() const
{
  return themodel;
}

occ::handle<IFSelect_Editor> IFSelect_EditForm::Editor() const
{
  return theeditor;
}

bool IFSelect_EditForm::IsComplete() const
{
  return thecomplete;
}

int IFSelect_EditForm::NbValues(const bool editable) const
{
  if (!editable || thecomplete)
    return theeditor->NbValues();
  return thenums.Upper();
}

int IFSelect_EditForm::NumberFromRank(const int rank) const
{
  if (thecomplete)
    return rank;
  if (rank < 1 || rank > thenums.Upper())
    return 0;
  return thenums.Value(rank);
}

int IFSelect_EditForm::RankFromNumber(const int num) const
{
  if (thecomplete)
    return num;
  int i, n = thenums.Upper();
  for (i = 1; i <= n; i++)
  {
    if (thenums.Value(i) == num)
      return i;
  }
  return 0;
}

int IFSelect_EditForm::NameNumber(const char* name) const
{
  int res = theeditor->NameNumber(name);
  if (thecomplete || res == 0)
    return res;
  //   Otherwise, search res in thenums
  int i, nb = thenums.Length();
  for (i = 1; i <= nb; i++)
  {
    if (res == thenums.Value(i))
      return res;
  }
  return -res;
}

int IFSelect_EditForm::NameRank(const char* name) const
{
  int res = theeditor->NameNumber(name);
  if (thecomplete || res == 0)
    return res;
  //   Otherwise, search res in thenums
  int i, nb = thenums.Length();
  for (i = 1; i <= nb; i++)
  {
    if (res == thenums.Value(i))
      return i;
  }
  return 0;
}

void IFSelect_EditForm::LoadDefault()
{
  theloaded  = true;
  thetouched = 0;
  int i, nb = theorigs.Upper();
  if (nb == 0)
    return;
  for (i = 1; i <= nb; i++)
  {
    int num = NumberFromRank(i);
    if (num == 0)
      continue;
    occ::handle<TCollection_HAsciiString> str = theeditor->StringValue(this, num);
    theorigs.SetValue(i, str);
  }
}

bool IFSelect_EditForm::LoadData(const occ::handle<Standard_Transient>&       ent,
                                             const occ::handle<Interface_InterfaceModel>& model)
{
  thetouched = 0;
  if (!theeditor->Load(this, ent, model))
    return false;
  SetData(ent, model);
  theloaded = true;
  return true;
}

bool IFSelect_EditForm::LoadEntity(const occ::handle<Standard_Transient>& ent)
{
  thetouched = 0;
  occ::handle<Interface_InterfaceModel> model;
  if (!theeditor->Load(this, ent, model))
    return false;
  SetEntity(ent);
  theloaded = true;
  return true;
}

bool IFSelect_EditForm::LoadModel(const occ::handle<Interface_InterfaceModel>& model)
{
  thetouched = 0;
  occ::handle<Standard_Transient> ent;
  if (!theeditor->Load(this, ent, model))
    return false;
  SetData(ent, model);
  theloaded = true;
  return true;
}

bool IFSelect_EditForm::LoadData()
{
  thetouched = 0;
  occ::handle<Interface_InterfaceModel> model;
  occ::handle<Standard_Transient>       ent;
  if (!theeditor->Load(this, ent, model))
    return false;
  theloaded = true;
  return true;
}

//  ########    VALUES    ########

occ::handle<IFSelect_ListEditor> IFSelect_EditForm::ListEditor(const int num) const
{
  int            n = RankFromNumber(num);
  occ::handle<IFSelect_ListEditor> led;
  if (n <= 0 || n > theorigs.Upper())
    return led;
  if (!theeditor->IsList(n))
    return led;
  led                                         = theeditor->ListEditor(num);
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> lis = theeditor->ListValue(this, num);
  led->LoadModel(themodel);
  led->LoadValues(lis);
  return led;
}

void IFSelect_EditForm::LoadValue(const int                  num,
                                  const occ::handle<TCollection_HAsciiString>& val)
{
  int n = RankFromNumber(num);
  if (n <= 0 || n > theorigs.Upper())
    return;
  theorigs.SetValue(n, val);
}

void IFSelect_EditForm::LoadList(const int                         num,
                                 const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& list)
{
  int n = RankFromNumber(num);
  if (n <= 0 || n > theorigs.Upper())
    return;
  theorigs.SetValue(n, list);
}

occ::handle<TCollection_HAsciiString> IFSelect_EditForm::OriginalValue(const int num) const
{
  int                 n = RankFromNumber(num);
  occ::handle<TCollection_HAsciiString> val;
  if (theorigs.Upper() == 0)
    return theeditor->StringValue(this, num);
  else
    return occ::down_cast<TCollection_HAsciiString>(theorigs.Value(n));
}

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> IFSelect_EditForm::OriginalList(
  const int num) const
{
  int                        n = RankFromNumber(num);
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> list;
  if (theorigs.Upper() == 0)
    return theeditor->ListValue(this, num);
  else
    return occ::down_cast<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>(theorigs.Value(n));
}

occ::handle<TCollection_HAsciiString> IFSelect_EditForm::EditedValue(const int num) const
{
  if (themodifs.Upper() == 0)
    return OriginalValue(num);
  if (!IsModified(num))
    return OriginalValue(num);
  int n = RankFromNumber(num);
  return occ::down_cast<TCollection_HAsciiString>(themodifs.Value(n));
}

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> IFSelect_EditForm::EditedList(
  const int num) const
{
  if (themodifs.Upper() == 0)
    return OriginalList(num);
  if (!IsModified(num))
    return OriginalList(num);
  int n = RankFromNumber(num);
  return occ::down_cast<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>(themodifs.Value(n));
}

bool IFSelect_EditForm::IsModified(const int num) const
{
  if (thestatus.Upper() == 0)
    return false;
  int n = RankFromNumber(num);
  return (thestatus.Value(n) != 0);
}

bool IFSelect_EditForm::IsTouched(const int num) const
{
  if (thestatus.Upper() == 0)
    return false;
  int n = RankFromNumber(num);
  return (thestatus.Value(n) == 2);
}

bool IFSelect_EditForm::Modify(const int                  num,
                                           const occ::handle<TCollection_HAsciiString>& newval,
                                           const bool                  enforce)
{
  //  Peut-on editer
  thetouched = 0;
  if (themodifs.Upper() == 0)
    return false;
  int tnum = RankFromNumber(num);
  if (tnum == 0)
    return false;
  IFSelect_EditValue acc = theeditor->EditMode(num);
  if (newval.IsNull() && acc != IFSelect_Optional)
    return false;
  if (!enforce && (acc == IFSelect_EditProtected || acc == IFSelect_EditComputed))
    return false;

  //  Satisfies ?
  occ::handle<Interface_TypedValue> typval = theeditor->TypedValue(num);
  if (!typval->Satisfies(newval))
    return false;
  Interface_ParamType pty = typval->Type();
  if (pty == Interface_ParamIdent && !newval.IsNull())
  {
    if (themodel.IsNull())
      return false;
    if (themodel->NextNumberForLabel(newval->ToCString(), 0, false) <= 0)
      return false;
  }

  //  Update ?
  if (!theeditor->Update(this, num, newval, enforce))
    return false;

  thestatus.SetValue(tnum, 1);
  themodifs.SetValue(tnum, newval);
  return true;
}

bool IFSelect_EditForm::ModifyList(const int             num,
                                               const occ::handle<IFSelect_ListEditor>& edited,
                                               const bool             enforce)
{
  //  Faut-il prendre
  if (edited.IsNull())
    return false;
  if (!edited->IsTouched())
    return false;
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> newlist = edited->EditedValues();

  //  Peut-on editer
  thetouched = 0;
  if (themodifs.Upper() == 0)
    return false;
  int tnum = RankFromNumber(num);
  if (tnum == 0)
    return false;
  IFSelect_EditValue acc = theeditor->EditMode(num);
  if (acc == IFSelect_EditRead || acc == IFSelect_EditDynamic)
    return false;
  if (newlist.IsNull() && acc != IFSelect_Optional)
    return false;
  if (!enforce && (acc == IFSelect_EditProtected || acc == IFSelect_EditComputed))
    return false;

  //  Update ?
  if (!theeditor->UpdateList(this, num, newlist, enforce))
    return false;

  thestatus.SetValue(tnum, 1);
  themodifs.SetValue(tnum, newlist);
  return true;
}

bool IFSelect_EditForm::ModifyListValue(
  const int                         num,
  const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& list,
  const bool                         enforce)
{
  occ::handle<IFSelect_ListEditor> led = ListEditor(num);
  if (led.IsNull())
    return false;
  if (!led->LoadEdited(list))
    return false;
  return ModifyList(num, led, enforce);
}

bool IFSelect_EditForm::Touch(const int                  num,
                                          const occ::handle<TCollection_HAsciiString>& newval)
{
  if (themodifs.Upper() == 0)
    return false;
  int tnum = RankFromNumber(num);
  if (tnum == 0)
    return false;

  thestatus.SetValue(tnum, 2);
  themodifs.SetValue(tnum, newval);
  thetouched++;
  return true;
}

bool IFSelect_EditForm::TouchList(
  const int                         num,
  const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& newlist)
{
  if (themodifs.Upper() == 0)
    return false;
  int tnum = RankFromNumber(num);
  if (tnum == 0)
    return false;

  thestatus.SetValue(tnum, 2);
  themodifs.SetValue(tnum, newlist);
  thetouched++;
  return true;
}

void IFSelect_EditForm::ClearEdit(const int num)
{
  int i, nb = thestatus.Upper();
  if (num == 0)
  {
    for (i = 1; i <= nb; i++)
      thestatus.SetValue(i, 0);
  }
  else
  {
    int tnum = RankFromNumber(num);
    if (tnum > 0 && num <= nb)
      thestatus.SetValue(tnum, 0);
  }
}

void IFSelect_EditForm::PrintDefs(Standard_OStream& S) const
{
  int iv, nbv = NbValues(true);
  S << "***** EditForm,  Label : " << Label() << std::endl;
  if (IsComplete())
    S << "Complete, " << nbv << " Values" << std::endl;
  else
  {
    S << "Extraction on " << nbv << " Values : (extracted<-editor)" << std::endl;
    for (iv = 1; iv <= nbv; iv++)
      S << "  " << iv << "<-" << NumberFromRank(iv);
    S << std::endl;
  }
  S << "*****" << std::endl;
}

static void PrintList(const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& list,
                      Standard_OStream&                              S,
                      const bool                         alsolist)
{
  if (list.IsNull())
  {
    S << "(NULL LIST)" << std::endl;
    return;
  }

  int i, nb = list->Length();
  S << "(List : " << nb << " Items)" << std::endl;
  if (!alsolist)
    return;

  for (i = 1; i <= nb; i++)
  {
    occ::handle<TCollection_HAsciiString> str = list->Value(i);
    S << "  [" << i << "]	" << (str.IsNull() ? "(NULL)" : str->ToCString()) << std::endl;
  }
}

void IFSelect_EditForm::PrintValues(Standard_OStream&      S,
                                    const int what,
                                    const bool names,
                                    const bool alsolist) const
{
  int iv, nbv = NbValues(true);
  S << "****************************************************" << std::endl;
  S << "*****  " << Label() << Interface_MSG::Blanks(Label(), 40) << "*****" << std::endl;
  S << "*****                                          *****" << std::endl;
  if (!theloaded)
    S << "*****         Values are NOT loaded            *****" << std::endl;

  else
  {
    //  Data on which we worked
    if (themodel.IsNull())
    {
      if (theent.IsNull())
        S << "*****  No loaded data";
      else
        S << "*****  No loaded Model. Loaded object : type " << theent->DynamicType()->Name();
    }
    else
    {
      if (theent.IsNull())
        S << "*****  No loaded entity";
      else
      {
        S << "*****  Loaded entity : ";
        themodel->PrintLabel(theent, S);
      }
    }
  }
  S << std::endl
    << "****************************************************" << std::endl
    << std::endl;

  //  Display of values
  bool nams   = names;
  int maxnam = theeditor->MaxNameLength(names ? 0 : -1);
  if (maxnam == 0)
  {
    maxnam = theeditor->MaxNameLength(0);
    nams   = true;
  }
  int nbmod = 0;
  if (what != 0)
    S << "Mod N0 Name               Value" << std::endl;
  else
    S << " N0 Name               Value" << std::endl;

  for (iv = 1; iv <= nbv; iv++)
  {
    int jv   = NumberFromRank(iv);
    const char* name = theeditor->Name(jv, !nams);

    //     Original or Final
    if (what != 0)
    {
      occ::handle<TCollection_HAsciiString> str;
      if (IsModified(jv))
        S << "* ";
      else
        S << "  ";
      S << Interface_MSG::Blanks(iv, 3) << iv << " " << name << Interface_MSG::Blanks(name, maxnam)
        << "  ";

      if (theeditor->IsList(jv))
      {
        occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> list;
        if (what < 0)
          list = OriginalList(jv);
        if (what > 0)
          list = EditedList(jv);
        PrintList(list, S, alsolist);
        continue;
      }

      if (what < 0)
        str = OriginalValue(jv);
      if (what > 0)
        str = EditedValue(jv);

      S << (str.IsNull() ? "(NULL)" : str->ToCString()) << std::endl;

      //    Modified only
    }
    else
    {
      if (!IsModified(jv))
        continue;
      nbmod++;
      if (theeditor->IsList(jv))
      {
        occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> list = OriginalList(jv);
        S << Interface_MSG::Blanks(iv, 3) << iv << " " << name
          << Interface_MSG::Blanks(name, maxnam) << " ORIG:";
        PrintList(list, S, alsolist);

        list = EditedList(jv);
        S << Interface_MSG::Blanks("", maxnam + 5) << "MOD :";
        PrintList(list, S, alsolist);

        continue;
      }

      occ::handle<TCollection_HAsciiString> str = OriginalValue(jv);
      S << Interface_MSG::Blanks(iv, 3) << iv << " " << name << Interface_MSG::Blanks(name, maxnam)
        << " ORIG:" << (str.IsNull() ? "(NULL)" : str->ToCString()) << std::endl;
      str = EditedValue(jv);
      S << Interface_MSG::Blanks("", maxnam + 4)
        << " MOD :" << (str.IsNull() ? "(NULL)" : str->ToCString()) << std::endl;
    }
  }
  if (what == 0)
    S << "On " << nbv << " Values, " << nbmod << " Modified" << std::endl;
}

bool IFSelect_EditForm::Apply()
{
  bool stat = ApplyData(theent, themodel);
  if (stat && !thekeepst)
    ClearEdit();
  return stat;
}

bool IFSelect_EditForm::Recognize() const
{
  return theeditor->Recognize(this);
}

bool IFSelect_EditForm::ApplyData(const occ::handle<Standard_Transient>&       ent,
                                              const occ::handle<Interface_InterfaceModel>& model)
{
  return theeditor->Apply(this, ent, model);
}

bool IFSelect_EditForm::Undo()
{
  if (thestatus.Upper() == 0 || theorigs.Upper() == 0)
    return false;
  int i, nb = thestatus.Upper();
  for (i = 1; i <= nb; i++)
  {
    if (thestatus.Value(i) != 0)
      themodifs.SetValue(i, theorigs.Value(i));
  }
  return Apply();
}
