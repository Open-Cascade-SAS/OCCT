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

#include <RWHeaderSection_GeneralModule.ixx>

#include <Interface_Macros.hxx>

#include <Interface_GeneralLib.hxx>

#include <Interface_EntityIterator.hxx>

#include <HeaderSection.hxx>

#include <RWHeaderSection.hxx>

#include <Interface_HArray1OfHAsciiString.hxx>
#include <TCollection_HAsciiString.hxx>


#include <HeaderSection_FileName.hxx>
#include <HeaderSection_FileDescription.hxx>
#include <HeaderSection_FileSchema.hxx>

#include <StepData_UndefinedEntity.hxx>
#include <Interface_Macros.hxx>


RWHeaderSection_GeneralModule::RWHeaderSection_GeneralModule ()
	{ Interface_GeneralLib::SetGlobal(this, HeaderSection::Protocol()); }

void RWHeaderSection_GeneralModule::FillSharedCase(const Standard_Integer CN, const Handle(Standard_Transient)& ent, Interface_EntityIterator& iter) const
{
  if (CN != 4) return;

  DeclareAndCast(StepData_UndefinedEntity,undf,ent);
  undf->FillShared (iter);

}


void RWHeaderSection_GeneralModule::CheckCase(const Standard_Integer CN,
                                              const Handle(Standard_Transient)& ent,
                                              const Interface_ShareTool& shares,
                                              Handle(Interface_Check)& ach) const
{
}


void RWHeaderSection_GeneralModule::CopyCase(const Standard_Integer CN, const Handle(Standard_Transient)& entfrom, const Handle(Standard_Transient)& entto, Interface_CopyTool& TC) const
{
//   ajout manuel
  switch (CN) {

    case 1 : {
      Standard_Integer i;
      DeclareAndCast(HeaderSection_FileName,enfr,entfrom);
      DeclareAndCast(HeaderSection_FileName,ento,entto);
      Handle(TCollection_HAsciiString) name =
	new TCollection_HAsciiString (enfr->Name());
      Handle(TCollection_HAsciiString) time =
	new TCollection_HAsciiString (enfr->TimeStamp());
      Standard_Integer nba = enfr->NbAuthor();
      Handle(Interface_HArray1OfHAsciiString) auth =
	new Interface_HArray1OfHAsciiString (1,nba);
      for (i = 1; i <= nba; i ++) auth->SetValue
	(i, new TCollection_HAsciiString (enfr->AuthorValue(i)) );
      Standard_Integer nbo = enfr->NbOrganization();
      Handle(Interface_HArray1OfHAsciiString) orga =
	new Interface_HArray1OfHAsciiString (1,nbo);
      for (i = 1; i <= nbo; i ++) orga->SetValue
	(i, new TCollection_HAsciiString (enfr->OrganizationValue(i)) );
      Handle(TCollection_HAsciiString) prep =
	new TCollection_HAsciiString (enfr->PreprocessorVersion());
      Handle(TCollection_HAsciiString) orig =
	new TCollection_HAsciiString (enfr->OriginatingSystem());
      Handle(TCollection_HAsciiString) autr =
	new TCollection_HAsciiString (enfr->Authorisation());
      ento->Init (name,time,auth,orga,prep,orig,autr);
    }
    break;

    case 2 : {
      Standard_Integer i;
      DeclareAndCast(HeaderSection_FileDescription,enfr,entfrom);
      DeclareAndCast(HeaderSection_FileDescription,ento,entto);
      Standard_Integer nbd = enfr->NbDescription();
      Handle(Interface_HArray1OfHAsciiString) desc =
	new Interface_HArray1OfHAsciiString (1,nbd);
      for (i = 1; i <= nbd; i ++) desc->SetValue
	(i, new TCollection_HAsciiString (enfr->DescriptionValue(i)) );
      Handle(TCollection_HAsciiString) impl =
	new TCollection_HAsciiString (enfr->ImplementationLevel());
      ento->Init (desc,impl);
    }
    break;

    case 3 : {
      Standard_Integer i;
      DeclareAndCast(HeaderSection_FileSchema,enfr,entfrom);
      DeclareAndCast(HeaderSection_FileSchema,ento,entto);
      Standard_Integer nbs = enfr->NbSchemaIdentifiers();
      Handle(Interface_HArray1OfHAsciiString) sche =
	new Interface_HArray1OfHAsciiString (1,nbs);
      for (i = 1; i <= nbs; i ++) sche->SetValue
	(i, new TCollection_HAsciiString (enfr->SchemaIdentifiersValue(i)) );
      ento->Init (sche);
    }
    break;

    case 4 : {
      DeclareAndCast(StepData_UndefinedEntity,undfrom,entfrom);
      DeclareAndCast(StepData_UndefinedEntity,undto,entto);
      undto->GetFromAnother(undfrom,TC);  //  On pourrait rapatrier cela
    }
    break;

    default : break;
  }
}
	// --- Construction of empty class ---

Standard_Boolean RWHeaderSection_GeneralModule::NewVoid
	(const Standard_Integer CN, Handle(Standard_Transient)& ent) const
{
	if (CN == 0) return Standard_False;
	switch (CN) {
	  case 1 : 
	    ent = new HeaderSection_FileName;
	    break;
	  case 2 : 
	    ent = new HeaderSection_FileDescription;
	    break;
	  case 3 : 
	    ent = new HeaderSection_FileSchema;
	    break;
	  case 4 : 
	    ent = new StepData_UndefinedEntity;
	    break;
	  default : return Standard_False;
	}

return Standard_True;
}

