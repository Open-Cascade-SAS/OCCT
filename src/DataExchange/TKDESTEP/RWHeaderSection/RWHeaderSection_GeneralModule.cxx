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

#include <HeaderSection.hxx>
#include <HeaderSection_FileDescription.hxx>
#include <HeaderSection_FileName.hxx>
#include <HeaderSection_FileSchema.hxx>
#include <HeaderSection_Protocol.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <RWHeaderSection_GeneralModule.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepData_UndefinedEntity.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWHeaderSection_GeneralModule, StepData_GeneralModule)

RWHeaderSection_GeneralModule::RWHeaderSection_GeneralModule()
{
  Interface_GeneralLib::SetGlobal(this, HeaderSection::Protocol());
}

void RWHeaderSection_GeneralModule::FillSharedCase(const int                              CN,
                                                   const occ::handle<Standard_Transient>& ent,
                                                   Interface_EntityIterator& iter) const
{
  if (CN != 4)
    return;

  DeclareAndCast(StepData_UndefinedEntity, undf, ent);
  undf->FillShared(iter);
}

void RWHeaderSection_GeneralModule::CheckCase(const int,
                                              const occ::handle<Standard_Transient>&,
                                              const Interface_ShareTool&,
                                              occ::handle<Interface_Check>&) const
{
}

void RWHeaderSection_GeneralModule::CopyCase(const int                              CN,
                                             const occ::handle<Standard_Transient>& entfrom,
                                             const occ::handle<Standard_Transient>& entto,
                                             Interface_CopyTool&                    TC) const
{
  //   ajout manuel
  switch (CN)
  {

    case 1: {
      int i;
      DeclareAndCast(HeaderSection_FileName, enfr, entfrom);
      DeclareAndCast(HeaderSection_FileName, ento, entto);
      occ::handle<TCollection_HAsciiString> name = new TCollection_HAsciiString(enfr->Name());
      occ::handle<TCollection_HAsciiString> time = new TCollection_HAsciiString(enfr->TimeStamp());
      int                                   nba  = enfr->NbAuthor();
      occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> auth =
        new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nba);
      for (i = 1; i <= nba; i++)
        auth->SetValue(i, new TCollection_HAsciiString(enfr->AuthorValue(i)));
      int nbo = enfr->NbOrganization();
      occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> orga =
        new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nbo);
      for (i = 1; i <= nbo; i++)
        orga->SetValue(i, new TCollection_HAsciiString(enfr->OrganizationValue(i)));
      occ::handle<TCollection_HAsciiString> prep =
        new TCollection_HAsciiString(enfr->PreprocessorVersion());
      occ::handle<TCollection_HAsciiString> orig =
        new TCollection_HAsciiString(enfr->OriginatingSystem());
      occ::handle<TCollection_HAsciiString> autr =
        new TCollection_HAsciiString(enfr->Authorisation());
      ento->Init(name, time, auth, orga, prep, orig, autr);
    }
    break;

    case 2: {
      int i;
      DeclareAndCast(HeaderSection_FileDescription, enfr, entfrom);
      DeclareAndCast(HeaderSection_FileDescription, ento, entto);
      int nbd = enfr->NbDescription();
      occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> desc =
        new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nbd);
      for (i = 1; i <= nbd; i++)
        desc->SetValue(i, new TCollection_HAsciiString(enfr->DescriptionValue(i)));
      occ::handle<TCollection_HAsciiString> impl =
        new TCollection_HAsciiString(enfr->ImplementationLevel());
      ento->Init(desc, impl);
    }
    break;

    case 3: {
      int i;
      DeclareAndCast(HeaderSection_FileSchema, enfr, entfrom);
      DeclareAndCast(HeaderSection_FileSchema, ento, entto);
      int nbs = enfr->NbSchemaIdentifiers();
      occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> sche =
        new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nbs);
      for (i = 1; i <= nbs; i++)
        sche->SetValue(i, new TCollection_HAsciiString(enfr->SchemaIdentifiersValue(i)));
      ento->Init(sche);
    }
    break;

    case 4: {
      DeclareAndCast(StepData_UndefinedEntity, undfrom, entfrom);
      DeclareAndCast(StepData_UndefinedEntity, undto, entto);
      undto->GetFromAnother(undfrom, TC); //  On pourrait rapatrier cela
    }
    break;

    default:
      break;
  }
}

// --- Construction of empty class ---

bool RWHeaderSection_GeneralModule::NewVoid(const int                        CN,
                                            occ::handle<Standard_Transient>& ent) const
{
  if (CN == 0)
    return false;
  switch (CN)
  {
    case 1:
      ent = new HeaderSection_FileName;
      break;
    case 2:
      ent = new HeaderSection_FileDescription;
      break;
    case 3:
      ent = new HeaderSection_FileSchema;
      break;
    case 4:
      ent = new StepData_UndefinedEntity;
      break;
    default:
      return false;
  }

  return true;
}
