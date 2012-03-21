// Created on: 2002-10-30
// Created by: Michael SAZONOV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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


#include <BinMDataStd.ixx>
#include <BinMDataStd_IntegerDriver.hxx>
#include <BinMDataStd_RealDriver.hxx>
#include <BinMDataStd_IntegerArrayDriver.hxx>
#include <BinMDataStd_RealArrayDriver.hxx>
#include <BinMDataStd_NameDriver.hxx>
#include <BinMDataStd_CommentDriver.hxx>
#include <BinMDataStd_ExpressionDriver.hxx>
#include <BinMDataStd_RelationDriver.hxx>
#include <BinMDataStd_TreeNodeDriver.hxx>
#include <BinMDataStd_UAttributeDriver.hxx>
#include <BinMDataStd_VariableDriver.hxx>
#include <BinMDataStd_DirectoryDriver.hxx>
#include <BinMDataStd_NoteBookDriver.hxx>
#include <BinMDataStd_ExtStringArrayDriver.hxx>
#include <BinMDataStd_TickDriver.hxx>
#include <BinMDataStd_IntegerListDriver.hxx>
#include <BinMDataStd_RealListDriver.hxx>
#include <BinMDataStd_ExtStringListDriver.hxx>
#include <BinMDataStd_BooleanListDriver.hxx>
#include <BinMDataStd_ReferenceListDriver.hxx>
#include <BinMDataStd_BooleanArrayDriver.hxx>
#include <BinMDataStd_ReferenceArrayDriver.hxx>
#include <BinMDataStd_ByteArrayDriver.hxx>
#include <BinMDataStd_NamedDataDriver.hxx>
#include <BinMDataStd_AsciiStringDriver.hxx>
#include <BinMDataStd_IntPackedMapDriver.hxx>

static Standard_Integer myDocumentVersion = -1;
//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================

void BinMDataStd::AddDrivers (const Handle(BinMDF_ADriverTable)& theDriverTable,
                              const Handle(CDM_MessageDriver)&   theMsgDriver)
{
  theDriverTable->AddDriver (new BinMDataStd_CommentDriver     (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_ExpressionDriver  (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_IntegerArrayDriver(theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_IntegerDriver     (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_NameDriver        (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_RealArrayDriver   (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_RealDriver        (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_RelationDriver    (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_TreeNodeDriver    (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_UAttributeDriver  (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_VariableDriver    (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_DirectoryDriver   (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_NoteBookDriver    (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_ExtStringArrayDriver(theMsgDriver) );

  theDriverTable->AddDriver (new BinMDataStd_TickDriver          (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_IntegerListDriver   (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_RealListDriver      (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_ExtStringListDriver (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_BooleanListDriver   (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_ReferenceListDriver (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_BooleanArrayDriver  (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_ReferenceArrayDriver(theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_ByteArrayDriver     (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_NamedDataDriver     (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_AsciiStringDriver   (theMsgDriver) );
  theDriverTable->AddDriver (new BinMDataStd_IntPackedMapDriver  (theMsgDriver) );
}

//=======================================================================
//function : SetDocumentVersion
//purpose  : Sets current document version
//=======================================================================
void BinMDataStd::SetDocumentVersion(const Standard_Integer theVersion)
{
  myDocumentVersion = theVersion;
}
//=======================================================================
//function : DocumentVersion
//purpose  : Retrieved document version
//=======================================================================
Standard_Integer BinMDataStd::DocumentVersion()
{
  return myDocumentVersion;
}
