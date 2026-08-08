// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BinDrivers.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Owner.hxx>

#include <gtest/gtest.h>

// bugs/caf/bug159: closing an OCAF document clears its owner link and releases
// the application's reference to the document.
TEST(TDocStd_Application_Test, CafBug_159_OwnerClearedOnClose)
{
  const occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  BinDrivers::DefineFormat(anApplication);

  occ::handle<TDocStd_Document> aDocument;
  anApplication->NewDocument("BinOcaf", aDocument);
  ASSERT_FALSE(aDocument.IsNull());

  const int aReferenceCountBeforeClose = aDocument->GetRefCount();
  occ::handle<TDocStd_Owner> anOwner;
  ASSERT_TRUE(aDocument->Main().Root().FindAttribute(TDocStd_Owner::GetID(), anOwner));
  occ::handle<TDocStd_Document> anOwnedDocument = anOwner->GetDocument();
  ASSERT_FALSE(anOwnedDocument.IsNull());
  EXPECT_EQ(anOwnedDocument.get(), aDocument.get());
  anOwnedDocument.Nullify();

  EXPECT_NO_THROW(anApplication->Close(aDocument));
  EXPECT_TRUE(anOwner->GetDocument().IsNull());
  EXPECT_GE(aReferenceCountBeforeClose - aDocument->GetRefCount(), 1);
}
