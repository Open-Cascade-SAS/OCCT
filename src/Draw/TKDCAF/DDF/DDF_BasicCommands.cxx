// Created by: DAUTRY Philippe & VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
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

// ---------------------

// Version: 0.0
// Version   Date            Purpose
//          0.0 Feb 10 1997  Creation

#include <DDF.hxx>

#include <TDF_ComparisonTool.hxx>

#include <DDF_Data.hxx>

#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Standard_NotImplemented.hxx>

#include <TCollection_AsciiString.hxx>

#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDF_DerivedAttribute.hxx>

//=======================================================================
// function : Children
// purpose  : Returns a list of sub-label entries.
//=======================================================================

static Standard_Integer DDF_Children(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2)
    return 1;

  Handle(TDF_Data)        DF;
  TCollection_AsciiString entry;

  if (!DDF::GetDF(a[1], DF))
    return 1;

  TDF_Label lab;
  if (n == 3)
    TDF_Tool::Label(DF, a[2], lab);

  if (lab.IsNull())
  {
    di << "0";
  }
  else
  {
    for (TDF_ChildIterator itr(lab); itr.More(); itr.Next())
    {
      TDF_Tool::Entry(itr.Value(), entry);
      // TCollection_AsciiString entry(itr.Value().Tag());
      di << entry.ToCString() << " ";
    }
  }
  return 0;
}

//=======================================================================
// function : Attributes
// purpose  : Returns a list of label attributes.
//=======================================================================

static Standard_Integer DDF_Attributes(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 3)
    return 1;

  Handle(TDF_Data) DF;

  if (!DDF::GetDF(a[1], DF))
    return 1;

  TDF_Label lab;
  TDF_Tool::Label(DF, a[2], lab);

  if (lab.IsNull())
    return 1;

  for (TDF_AttributeIterator itr(lab); itr.More(); itr.Next())
  {
    di << itr.Value()->DynamicType()->Name() << " ";
  }
  return 0;
}

//=======================================================================
// function : SetEmptyAttribute
// purpose  : Adds an empty attribute to the label by its dynamic type.
//=======================================================================

static Standard_Integer DDF_SetEmptyAttribute(Draw_Interpretor& di,
                                              Standard_Integer  n,
                                              const char**      a)
{
  if (n != 4)
    return 1;

  Handle(TDF_Data) DF;

  if (!DDF::GetDF(a[1], DF))
    return 1;

  TDF_Label lab;
  TDF_Tool::Label(DF, a[2], lab);

  if (lab.IsNull())
    return 1;

  Handle(TDF_Attribute) anAttrByType = TDF_DerivedAttribute::Attribute(a[3]);
  if (anAttrByType.IsNull())
  {
    di << "DDF: Not registered attribute type '" << a[3] << "'\n";
    return 1;
  }

  lab.AddAttribute(anAttrByType);

  return 0;
}

//=======================================================================
// function : ForgetAll
// purpose  : "ForgetAll dfname Label"
//=======================================================================

static Standard_Integer DDF_ForgetAll(Draw_Interpretor& /*di*/, Standard_Integer n, const char** a)
{
  if (n != 3)
    return 1;

  Handle(TDF_Data) DF;

  if (!DDF::GetDF(a[1], DF))
    return 1;

  TDF_Label label;
  TDF_Tool::Label(DF, a[2], label);
  if (label.IsNull())
    return 1;
  label.ForgetAllAttributes();
  // POP pour NT
  return 0;
}

//=======================================================================
// function : ForgetAttribute
// purpose  : "ForgetAtt dfname Label guid_or_type"
//=======================================================================

static Standard_Integer DDF_ForgetAttribute(Draw_Interpretor& di,
                                            Standard_Integer  n,
                                            const char**      a)
{
  if (n != 4)
    return 1;
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(a[1], DF))
    return 1;

  TDF_Label aLabel;
  TDF_Tool::Label(DF, a[2], aLabel);
  if (aLabel.IsNull())
    return 1;
  if (!Standard_GUID::CheckGUIDFormat(a[3]))
  {
    // check this may be derived attribute by its type
    Handle(TDF_Attribute) anAttrByType = TDF_DerivedAttribute::Attribute(a[3]);
    if (!anAttrByType.IsNull())
    {
      aLabel.ForgetAttribute(anAttrByType->ID());
      return 0;
    }
    di << "DDF: The format of GUID is invalid\n";
    return 1;
  }
  Standard_GUID guid(a[3]);
  aLabel.ForgetAttribute(guid);
  return 0;
}

//=======================================================================
// function : DDF_SetTagger
// purpose  : SetTagger (DF, entry)
//=======================================================================

static Standard_Integer DDF_SetTagger(Draw_Interpretor& di, Standard_Integer nb, const char** arg)
{
  if (nb == 3)
  {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1], DF))
      return 1;
    TDF_Label L;
    DDF::AddLabel(DF, arg[2], L);
    TDF_TagSource::Set(L);
    return 0;
  }
  di << "DDF_SetTagger : Error\n";
  return 1;
}

//=======================================================================
// function : DDF_NewTag
// purpose  : NewTag (DF,[father]
//=======================================================================

static Standard_Integer DDF_NewTag(Draw_Interpretor& di, Standard_Integer nb, const char** arg)
{
  if (nb == 3)
  {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1], DF))
      return 1;
    Handle(TDF_TagSource) A;
    if (!DDF::Find(DF, arg[2], TDF_TagSource::GetID(), A))
      return 1;
    di << A->NewTag();
    return 0;
  }
  di << "DDF_NewTag : Error\n";
  return 1;
}

//=======================================================================
// function : DDF_NewChild
// purpose  : NewChild(DF,[father])
//=======================================================================

static Standard_Integer DDF_NewChild(Draw_Interpretor& di, Standard_Integer nb, const char** arg)
{
  Handle(TDF_Data) DF;
  if (nb >= 2)
  {
    if (!DDF::GetDF(arg[1], DF))
      return 1;
    if (nb == 2)
    {
      TDF_Label free = TDF_TagSource::NewChild(DF->Root());
      di << free.Tag();
      return 0;
    }
    else if (nb == 3)
    {
      TDF_Label fatherlabel;
      if (!DDF::FindLabel(DF, arg[2], fatherlabel))
        return 1;
      TDF_Label free = TDF_TagSource::NewChild(fatherlabel);
      di << arg[2] << ":" << free.Tag();
      return 0;
    }
  }
  di << "DDF_NewChild : Error\n";
  return 1;
}

//=======================================================================
// function : Label (DF,freeentry)
//=======================================================================

static Standard_Integer DDF_Label(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n == 3)
  {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1], DF))
      return 1;
    TDF_Label L;
    if (!DDF::FindLabel(DF, a[2], L, Standard_False))
    {
      DDF::AddLabel(DF, a[2], L);
      // di << "Label : " << a[2] << " created\n";
    }
    // else di << "Label : " << a[2] << " retrieved\n";
    DDF::ReturnLabel(di, L);
    return 0;
  }
  di << "DDF_Label : Error\n";
  return 1;
}

//=================================================================================================

void DDF::BasicCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done)
    return;
  done = Standard_True;

  const char* g = "DF basic commands";

  // Label :

  theCommands.Add("SetTagger", "SetTagger (DF, entry)", __FILE__, DDF_SetTagger, g);

  theCommands.Add("NewTag", "NewTag (DF, tagger)", __FILE__, DDF_NewTag, g);

  theCommands.Add("NewChild", "NewChild (DF, [tagger])", __FILE__, DDF_NewChild, g);

  theCommands.Add("Children",
                  " Returns the list of label children: Children DF label",
                  __FILE__,
                  DDF_Children,
                  g);

  theCommands.Add("Attributes",
                  " Returns the list of label attributes: Attributes DF label",
                  __FILE__,
                  DDF_Attributes,
                  g);

  theCommands.Add(
    "SetEmptyAttribute",
    "Sets an empty attribute by its type (like TDataStd_Tick): SetEmptyAttribute DF label type",
    __FILE__,
    DDF_SetEmptyAttribute,
    g);

  theCommands.Add("ForgetAll",
                  "Forgets all attributes from the label: ForgetAll DF Label",
                  __FILE__,
                  DDF_ForgetAll,
                  g);

  theCommands.Add("ForgetAtt",
                  "Forgets the specified by guid attribute or type from the label: ForgetAtt DF "
                  "Label guid_or_type",
                  __FILE__,
                  DDF_ForgetAttribute,
                  g);

  theCommands.Add("Label", "Label DF entry", __FILE__, DDF_Label, g);
}
