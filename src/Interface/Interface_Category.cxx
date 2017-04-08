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


#include <Interface_Category.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Mutex.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <NCollection_Vector.hxx>

static int THE_Interface_Category_init = 0;
static Standard_CString unspec = "unspecified";

static Standard_Mutex gMapTypesMutex;
static volatile Standard_Boolean gMapTypesInit = Standard_False;

static NCollection_Vector<TCollection_AsciiString>& theCats()
{
  static NCollection_Vector<TCollection_AsciiString> aCat;
  return aCat;
}

Standard_Integer Interface_Category::CatNum
  (const Handle(Standard_Transient)& theEnt,
   const Interface_ShareTool& theShares)
{
  if (theEnt.IsNull()) return 0;
  Standard_Integer CN;
  Handle(Interface_GeneralModule) aModule;
  if (!myGTool->Select (theEnt,aModule,CN)) return 0;
  return aModule->CategoryNumber (CN,theEnt,theShares);
}

void Interface_Category::Compute
  (const Handle(Interface_InterfaceModel)& theModel,
   const Interface_ShareTool& theShares)
{
  ClearNums();
  if (theModel.IsNull()) return;
  Standard_Integer CN, i, nb = theModel->NbEntities();
  myGTool->Reservate (nb);
  if (nb == 0) return;
  myNum = new TColStd_HArray1OfInteger (1,nb);  myNum->Init(0);
  for (i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) anEnt = theModel->Value(i);
    if (anEnt.IsNull()) continue;
    Handle(Interface_GeneralModule) aModule;
    if (!myGTool->Select (anEnt,aModule,CN)) continue;
    myNum->SetValue (i,aModule->CategoryNumber (CN,anEnt,theShares));
  }
}

Standard_Integer Interface_Category::Num (const Standard_Integer theNumEnt) const
{
  if (myNum.IsNull()) return 0;
  if (theNumEnt < 1 || theNumEnt > myNum->Length()) return 0;
  return myNum->Value(theNumEnt);
}

// List of Categories

Standard_Integer Interface_Category::AddCategory (const Standard_CString theName)
{
  Standard_Integer aNum = Interface_Category::Number (theName);
  if (aNum > 0) return aNum;
  theCats().Append (TCollection_AsciiString(theName));
  return theCats().Length()+1;
}

Standard_Integer Interface_Category::NbCategories()
{
  return theCats().Length();
}

Standard_CString Interface_Category::Name (const Standard_Integer theNum)
{
  if (theNum < 0) return "";
  if (theNum < theCats().Lower() || theNum > theCats().Upper()) return unspec;
  return theCats().ChangeValue(theNum).ToCString();
}

Standard_Integer Interface_Category::Number (const Standard_CString theName)
{
  Standard_Integer i;
  for (i = theCats().Lower(); i <= theCats().Upper(); i ++) {
    if (theCats().ChangeValue(i).IsEqual(theName)) return i;
  }
  return 0;
}

void Interface_Category::Init ()
{
  // On first call, initialize static map
  if ( !gMapTypesInit )
  {
    gMapTypesMutex.Lock();
    if ( !gMapTypesInit )
    {
      if (THE_Interface_Category_init)
      {
        return;
      }

      THE_Interface_Category_init = 1;
      Interface_Category::AddCategory ("Shape");
      Interface_Category::AddCategory ("Drawing");
      Interface_Category::AddCategory ("Structure");
      Interface_Category::AddCategory ("Description");
      Interface_Category::AddCategory ("Auxiliary");
      Interface_Category::AddCategory ("Professional");
      Interface_Category::AddCategory ("FEA");
      Interface_Category::AddCategory ("Kinematics");
      Interface_Category::AddCategory ("Piping");

      gMapTypesInit = Standard_True;
    }
    gMapTypesMutex.Unlock();
  }
}
