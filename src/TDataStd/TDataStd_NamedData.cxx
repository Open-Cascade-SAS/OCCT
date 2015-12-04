// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfStringInteger.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringByte.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringReal.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringString.hxx>
#include <TDataStd_HDataMapOfStringByte.hxx>
#include <TDataStd_HDataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_HDataMapOfStringHArray1OfReal.hxx>
#include <TDataStd_HDataMapOfStringInteger.hxx>
#include <TDataStd_HDataMapOfStringReal.hxx>
#include <TDataStd_HDataMapOfStringString.hxx>
#include <TDataStd_NamedData.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_NamedData,TDF_Attribute)

#ifdef _WIN32
#define EXCEPTION ...
#else
#define EXCEPTION Standard_Failure
#endif
//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_NamedData::GetID() 
{ 
  static Standard_GUID TDataStd_NamedDataID ("F170FD21-CBAE-4e7d-A4B4-0560A4DA2D16");
  return TDataStd_NamedDataID; 
}

//=======================================================================
//function : TDataStd_NamedData
//purpose  : Empty Constructor
//=======================================================================
TDataStd_NamedData::TDataStd_NamedData() 
{

}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_NamedData) TDataStd_NamedData::Set(const TDF_Label& label) 
{
  Handle(TDataStd_NamedData) A;
  if (!label.FindAttribute (TDataStd_NamedData::GetID(), A)) 
  {
    A = new TDataStd_NamedData;
    label.AddAttribute(A);
  }
  return A;
}


//Category: Integers

//=======================================================================
//function : HasInteger
//purpose  : Returns true if the attribute contains this named integer.
//=======================================================================
Standard_Boolean TDataStd_NamedData::HasInteger(const TCollection_ExtendedString& theName) const
{
  if(!HasIntegers()) return Standard_False;
  return myIntegers->Map().IsBound(theName);
}

//=======================================================================
//function :  GetInteger
//purpose  : Returns the named integer. It returns 0 if there is no such 
//         : a named integer(use HasInteger()).
//=======================================================================
Standard_Integer TDataStd_NamedData::GetInteger(const TCollection_ExtendedString& theName)
{
  if(!HasIntegers()) {
    TColStd_DataMapOfStringInteger aMap;
    myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
  }
  return myIntegers->Map()(theName);
}

//=======================================================================
//function : SetInteger
//purpose  : Defines a named integer. If the integer already exists,
//         : it changes its value to <theInteger>.
//=======================================================================
void TDataStd_NamedData::SetInteger(const TCollection_ExtendedString& theName,
				    const Standard_Integer theInteger)
{
  if(!HasIntegers()) {
    TColStd_DataMapOfStringInteger aMap;
    myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
  }
  if (!myIntegers->Map().IsBound(theName) || myIntegers->Map().Find(theName) != theInteger)
  {
    Backup();
    if (myIntegers->Map().IsBound(theName))
      myIntegers->ChangeMap().ChangeFind(theName) = theInteger;
    else
      myIntegers->ChangeMap().Bind(theName, theInteger);
  }
}

//=======================================================================
//function : GetIntegersContainer
//purpose  : Returns the internal container of named integers.
//         : Use before HasIntegers()
//=======================================================================
const TColStd_DataMapOfStringInteger& TDataStd_NamedData::GetIntegersContainer()
{
  if(!HasIntegers()) {
    TColStd_DataMapOfStringInteger aMap;
    myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
  }
  return myIntegers->Map();
}

//=======================================================================
//function : ChangeIntegers
//purpose  : Replace the container content by new content of the <theIntegers>.
//=======================================================================
void TDataStd_NamedData::ChangeIntegers(const TColStd_DataMapOfStringInteger& theIntegers)
{
  if(!HasIntegers()) {
    TColStd_DataMapOfStringInteger aMap;
    myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
  };
  if (&myIntegers->Map() == &theIntegers) return;
  Backup();
  myIntegers->ChangeMap().Assign(theIntegers);  
}


//Category: Reals
//          =====    

//=======================================================================
//function : HasReal
//purpose  : Returns true if the attribute contains this named real.
//=======================================================================
Standard_Boolean TDataStd_NamedData::HasReal(const TCollection_ExtendedString& theName) const
{
  if(!HasReals()) return Standard_False;
  return myReals->Map().IsBound(theName);
}

//=======================================================================
//function : GetReal
//purpose  : Returns the named real. It returns 0 if there is no such 
//         : a named real (use HasReal()).
//=======================================================================
Standard_Real TDataStd_NamedData::GetReal(const TCollection_ExtendedString& theName)
{
  if(!HasReals()) {
    TDataStd_DataMapOfStringReal aMap;
    myReals = new TDataStd_HDataMapOfStringReal(aMap);
  }
  return myReals->Map()(theName);
}

//=======================================================================
//function : SetReal
//purpose  : Defines a named real. If the real already exists, 
//         : it changes its value to <theReal>.
//=======================================================================
void TDataStd_NamedData::SetReal(const TCollection_ExtendedString& theName,
				 const Standard_Real theReal)
{
  if(!HasReals()) {
    TDataStd_DataMapOfStringReal aMap;
    myReals = new TDataStd_HDataMapOfStringReal(aMap);
 }
  if (!myReals->Map().IsBound(theName) || myReals->Map().Find(theName) != theReal)
  {
    Backup();
    if (myReals->Map().IsBound(theName))
      myReals->ChangeMap().ChangeFind(theName) = theReal;
    else
      myReals->ChangeMap().Bind(theName, theReal);
  }
}

//=======================================================================
//function : GetRealsContainer
//purpose  : Returns the internal container of named reals.
//=======================================================================
const TDataStd_DataMapOfStringReal& TDataStd_NamedData::GetRealsContainer()
{
  if(!HasReals()) {
    TDataStd_DataMapOfStringReal aMap;
    myReals = new TDataStd_HDataMapOfStringReal(aMap);
  }  
  return myReals->Map();
}

//=======================================================================
//function : ChangeReals
//purpose  : Replace the container content by new content of the <theReals>.
//=======================================================================
void TDataStd_NamedData::ChangeReals(const TDataStd_DataMapOfStringReal& theReals)
{
  if(!HasReals()) {
    TDataStd_DataMapOfStringReal aMap;
    myReals = new TDataStd_HDataMapOfStringReal(aMap);
  }
  if (&myReals->Map() == &theReals) return;
  Backup();
  myReals->ChangeMap().Assign(theReals);
}


//Category: Strings
//          =======
//=======================================================================
//function : HasString
//purpose  : Returns true if the attribute contains this named string.
//=======================================================================
Standard_Boolean TDataStd_NamedData::HasString(const TCollection_ExtendedString& theName) const
{
  if(!HasStrings()) return Standard_False;
  return myStrings->Map().IsBound(theName);
}

//=======================================================================
//function : GetString
//purpose  : Returns the named string.It returns empty string if there is 
//         : string specified by the Name(use HasString()).
//=======================================================================
const TCollection_ExtendedString& TDataStd_NamedData::GetString(const TCollection_ExtendedString& theName)
{
  if(!HasStrings()) {
    TDataStd_DataMapOfStringString aMap;
    myStrings = new TDataStd_HDataMapOfStringString(aMap);
  }
  return myStrings->Map()(theName);
}

//=======================================================================
//function : SetString
//purpose  : Defines a named string. If the string already exists,
//         : it changes its value to <theString>.
//=======================================================================
void TDataStd_NamedData::SetString(const TCollection_ExtendedString& theName,
				   const TCollection_ExtendedString& theString)
{
  if(!HasStrings()) {
    TDataStd_DataMapOfStringString aMap;
    myStrings = new TDataStd_HDataMapOfStringString(aMap);
  }
  if (!myStrings->Map().IsBound(theName) || myStrings->Map().Find(theName) != theString)
  {
    Backup();
    if (myStrings->Map().IsBound(theName))
      myStrings->ChangeMap().ChangeFind(theName) = theString;
    else
      myStrings->ChangeMap().Bind(theName, theString);
  }
}

//=======================================================================
//function : GetStringsContainer
//purpose  : Returns the internal container of named strings.
//=======================================================================
const TDataStd_DataMapOfStringString& TDataStd_NamedData::GetStringsContainer()
{
  if(!HasStrings()) {
    TDataStd_DataMapOfStringString aMap;
    myStrings = new TDataStd_HDataMapOfStringString(aMap);
  }
  return myStrings->Map();
}

//=======================================================================
//function : ChangeStrings
//purpose  : Replace the container content by new content of the <theStrings>.
//=======================================================================
void TDataStd_NamedData::ChangeStrings(const TDataStd_DataMapOfStringString& theStrings)
{
  if(!HasStrings()) {
    TDataStd_DataMapOfStringString aMap;
    myStrings = new TDataStd_HDataMapOfStringString(aMap);
  }
  if (&myStrings->Map() == &theStrings) return;
  Backup();
  myStrings->ChangeMap().Assign(theStrings);
}


//Category: Bytes
//          =====    
//=======================================================================
//function : HasByte
//purpose  : Returns true if the attribute contains this named byte.
//=======================================================================
Standard_Boolean TDataStd_NamedData::HasByte(const TCollection_ExtendedString& theName) const
{
  if(!HasBytes()) return Standard_False;
  return myBytes->Map().IsBound(theName);
}

//=======================================================================
//function : GetByte
//purpose  : Returns the named byte. It returns 0 if there is no such
//         : a named byte (use HasByte()).
//=======================================================================
Standard_Byte TDataStd_NamedData::GetByte(const TCollection_ExtendedString& theName)
{
  if(!HasBytes()) {
    TDataStd_DataMapOfStringByte aMap;
    myBytes = new TDataStd_HDataMapOfStringByte(aMap);
  }
  return myBytes->Map()(theName);
}

//=======================================================================
//function : SetByte
//purpose  : Defines a named byte. If the byte already exists,
//         : it changes its value to <theByte>.
//=======================================================================
void TDataStd_NamedData::SetByte(const TCollection_ExtendedString& theName,
				 const Standard_Byte theByte)
{
  if(!HasBytes()) {
    TDataStd_DataMapOfStringByte aMap;
    myBytes = new TDataStd_HDataMapOfStringByte(aMap);
  }
  if (!myBytes->Map().IsBound(theName) || myBytes->Map().Find(theName) != theByte)
  {
    Backup();
    if (myBytes->Map().IsBound(theName))
      myBytes->ChangeMap().ChangeFind(theName) = theByte;
    else
      myBytes->ChangeMap().Bind(theName, theByte);
  }
}

//=======================================================================
//function : GetBytesContainer
//purpose  : Returns the internal container of named bytes. 
//=======================================================================
const TDataStd_DataMapOfStringByte& TDataStd_NamedData::GetBytesContainer()
{
  if(!HasBytes()) {
    TDataStd_DataMapOfStringByte aMap;
    myBytes = new TDataStd_HDataMapOfStringByte(aMap);
  }
  return myBytes->Map();
}

//=======================================================================
//function : ChangeBytes
//purpose  : Replace the container content by new content of the <theBytes>.
//=======================================================================
void TDataStd_NamedData::ChangeBytes(const TDataStd_DataMapOfStringByte& theBytes)
{
  if(!HasBytes()) {
    TDataStd_DataMapOfStringByte aMap;
    myBytes = new TDataStd_HDataMapOfStringByte(aMap);
  }
  if (&myBytes->Map() == &theBytes) return;
  Backup();
  myBytes->ChangeMap().Assign(theBytes);
}


//Category: Arrays of integers
//          ==================
//=======================================================================
//function : HasArrayOfIntegers
//purpose  : Returns true if the attribute contains this named array 
//         : of integer values.
//=======================================================================
Standard_Boolean TDataStd_NamedData::HasArrayOfIntegers
                        (const TCollection_ExtendedString& theName) const
{
  if(!HasArraysOfIntegers()) return Standard_False;
  return myArraysOfIntegers->Map().IsBound(theName);
}

//=======================================================================
//function : GetArrayOfIntegers
//purpose  : Returns the named array of integer values. It returns a NULL 
//         : Handle if there is no such a named array of integers
//=======================================================================
const Handle(TColStd_HArray1OfInteger)& TDataStd_NamedData::GetArrayOfIntegers
                        (const TCollection_ExtendedString& theName)
{
  if(!HasArraysOfIntegers()) {
    TDataStd_DataMapOfStringHArray1OfInteger aMap;
    myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
  }
  return myArraysOfIntegers->Map().Find(theName);
}

//=======================================================================
//function : SetArrayOfIntegers
//purpose  : Defines a named array of integer values.
//         : If the array already exists, it changes its value to <theArrayOfIntegers>.
//=======================================================================
void TDataStd_NamedData::SetArrayOfIntegers(const TCollection_ExtendedString& theName,
					    const Handle(TColStd_HArray1OfInteger)& theArrayOfIntegers)
{
  if(!HasArraysOfIntegers()) {
    TDataStd_DataMapOfStringHArray1OfInteger aMap;
    myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
  }

  Backup();
  // Deep copy of the array
  Handle(TColStd_HArray1OfInteger) arr;
  if (!theArrayOfIntegers.IsNull())
  {
    Standard_Integer lower = theArrayOfIntegers->Lower(), i = lower, upper = theArrayOfIntegers->Upper();
    arr = new TColStd_HArray1OfInteger(lower, upper);
    for (; i <= upper; i++)
    {
      arr->SetValue(i, theArrayOfIntegers->Value(i));
    }
  }

  if (myArraysOfIntegers->Map().IsBound(theName))
    myArraysOfIntegers->ChangeMap().ChangeFind(theName) = arr;
  else
    myArraysOfIntegers->ChangeMap().Bind(theName, arr);
}

//=======================================================================
//function : GetArraysOfIntegersContainer
//purpose  : Returns the internal container of named arrays of integer values.
//=======================================================================
const TDataStd_DataMapOfStringHArray1OfInteger& TDataStd_NamedData::
                                     GetArraysOfIntegersContainer()
{
  if(!HasArraysOfIntegers()) {
    TDataStd_DataMapOfStringHArray1OfInteger aMap;
    myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
  }
  return myArraysOfIntegers->Map();
}

//=======================================================================
//function : ChangeArraysOfIntegers
//purpose  : Replace the container content by new content of the <theIntegers>.
//=======================================================================
void TDataStd_NamedData::ChangeArraysOfIntegers
            (const TDataStd_DataMapOfStringHArray1OfInteger& theIntegers)
{
  if(!HasArraysOfIntegers()) {
    TDataStd_DataMapOfStringHArray1OfInteger aMap;
    myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
  }
  if (&myArraysOfIntegers->Map() == &theIntegers) return;
  Backup();
  myArraysOfIntegers->ChangeMap().Assign(theIntegers);
}


//Category: Arrays of reals
//          ===============
//=======================================================================
//function : HasArrayOfReals
//purpose  : Returns true if the attribute contains this named array of 
//         : real values.
//=======================================================================
Standard_Boolean TDataStd_NamedData::HasArrayOfReals
                        (const TCollection_ExtendedString& theName) const
{
  if(!HasArraysOfReals()) return Standard_False;
  return myArraysOfReals->Map().IsBound(theName);
}

//=======================================================================
//function : GetArrayOfReals
//purpose  : Returns the named array of real values. It returns a NULL 
//         : Handle if there is no such a named array of reals.
//=======================================================================
const Handle(TColStd_HArray1OfReal)& TDataStd_NamedData::GetArrayOfReals
                        (const TCollection_ExtendedString& theName) 
{
  if(!HasArraysOfReals()) {
    TDataStd_DataMapOfStringHArray1OfReal aMap;
    myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
  }
  return myArraysOfReals->Map().Find(theName);
}

//=======================================================================
//function : SetArrayOfReals
//purpose  : Defines a named array of real values.
//         : If the array already exists, it changes its value to <theArrayOfReals>.
//=======================================================================
void TDataStd_NamedData::SetArrayOfReals(const TCollection_ExtendedString& theName,
					 const Handle(TColStd_HArray1OfReal)& theArrayOfReals)
{
  if(!HasArraysOfReals()) {
    TDataStd_DataMapOfStringHArray1OfReal aMap;
    myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
  }
  Backup();

  // Deep copy of the array
  Handle(TColStd_HArray1OfReal) arr;
  if (!theArrayOfReals.IsNull())
  {
    Standard_Integer lower = theArrayOfReals->Lower(), i = lower, upper = theArrayOfReals->Upper();
    arr = new TColStd_HArray1OfReal(lower, upper);
    for (; i <= upper; i++)
    {
      arr->SetValue(i, theArrayOfReals->Value(i));
    }
  }

  if (myArraysOfReals->Map().IsBound(theName))
    myArraysOfReals->ChangeMap().ChangeFind(theName) = arr;
  else
    myArraysOfReals->ChangeMap().Bind(theName, arr);
}

//=======================================================================
//function : GetArraysOfRealsContainer
//purpose  : Returns the internal container of named arrays of real values.
//=======================================================================
const TDataStd_DataMapOfStringHArray1OfReal& TDataStd_NamedData::
                                        GetArraysOfRealsContainer()
{
  if(!HasArraysOfReals()) {
    TDataStd_DataMapOfStringHArray1OfReal aMap;
    myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
  }
  return myArraysOfReals->Map();
}

//=======================================================================
//function : ChangeArraysOfReals
//purpose  : Replace the container content by new content of the <theReals>.
//=======================================================================
void TDataStd_NamedData::ChangeArraysOfReals
                   (const TDataStd_DataMapOfStringHArray1OfReal& theReals)
{
  if(!HasArraysOfReals()) {
    TDataStd_DataMapOfStringHArray1OfReal aMap;
    myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
  }
  if (&myArraysOfReals->Map() == &theReals) return;
  Backup();
  myArraysOfReals->ChangeMap().Assign(theReals);
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_NamedData::ID () const 
{ 
  return GetID(); 
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataStd_NamedData::NewEmpty () const
{  
  return new TDataStd_NamedData(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_NamedData::Restore(const Handle(TDF_Attribute)& With) 
{
  
  Handle(TDataStd_NamedData) ND = Handle(TDataStd_NamedData)::DownCast(With);
  if(ND.IsNull()) return;
  // Integers
  if (!ND->GetIntegersContainer().IsEmpty())
  {
    if(!HasIntegers()) {
      TColStd_DataMapOfStringInteger aMap;
      myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
    }
    myIntegers->ChangeMap().Assign(ND->GetIntegersContainer());
  }

  // Reals
  if (!ND->GetRealsContainer().IsEmpty())
  {
    if(!HasReals()) {
      TDataStd_DataMapOfStringReal aMap;
      myReals = new TDataStd_HDataMapOfStringReal(aMap);
    }
    myReals->ChangeMap().Assign(ND->GetRealsContainer());

  }

  // Strings
  if (!ND->GetStringsContainer().IsEmpty())
  {
    if(!HasStrings()) {
      TDataStd_DataMapOfStringString aMap;
      myStrings = new TDataStd_HDataMapOfStringString(aMap);
    } 
    myStrings->ChangeMap().Assign(ND->GetStringsContainer());

  }

  // Bytes
  if (!ND->GetBytesContainer().IsEmpty())
  {
    if(!HasBytes()) {
      TDataStd_DataMapOfStringByte aMap;
      myBytes = new TDataStd_HDataMapOfStringByte(aMap);
    }
    myBytes->ChangeMap().Assign(ND->GetBytesContainer());

  }

  // Arrays of integers
  if (!ND->GetArraysOfIntegersContainer().IsEmpty())
  {
    if(!HasArraysOfIntegers()) {
      TDataStd_DataMapOfStringHArray1OfInteger aMap;
      myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
    }
    TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger itr(ND->GetArraysOfIntegersContainer());
    for (; itr.More(); itr.Next())
    {
      // Deep copy of the arrays
      const Handle(TColStd_HArray1OfInteger)& ints = itr.Value();
      Handle(TColStd_HArray1OfInteger) copied_ints;
      if (!ints.IsNull())
      {
	Standard_Integer lower = ints->Lower(), i = lower, upper = ints->Upper();
	copied_ints = new TColStd_HArray1OfInteger(lower, upper);
	for (; i <= upper; i++)
	{
	  copied_ints->SetValue(i, ints->Value(i));
	}
      }
      myArraysOfIntegers->ChangeMap().Bind(itr.Key(), copied_ints);
    }
  }

  // Arrays of realss
  if (!ND->GetArraysOfRealsContainer().IsEmpty())
  {
    if(!HasArraysOfReals()) {
      TDataStd_DataMapOfStringHArray1OfReal aMap;
      myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
    }
    TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal itr(ND->GetArraysOfRealsContainer());
    for (; itr.More(); itr.Next())
    {
      // Deep copy of the arrays
      const Handle(TColStd_HArray1OfReal)& dbls = itr.Value();
      Handle(TColStd_HArray1OfReal) copied_dbls;
      if (!dbls.IsNull())
      {
	Standard_Integer lower = dbls->Lower(), i = lower, upper = dbls->Upper();
	copied_dbls = new TColStd_HArray1OfReal(lower, upper);
	for (; i <= upper; i++)
	{
	  copied_dbls->SetValue(i, dbls->Value(i));
	}
      }
      myArraysOfReals->ChangeMap().Bind(itr.Key(), copied_dbls);
    }
  }
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_NamedData::Paste (const Handle(TDF_Attribute)& Into,
				  const Handle(TDF_RelocationTable)& ) const
{
  Handle(TDataStd_NamedData) ND = Handle(TDataStd_NamedData)::DownCast(Into);
  if (ND.IsNull()) return;

  // Integers
  if (HasIntegers() && !myIntegers->Map().IsEmpty())
  {
    if(!ND->HasIntegers()) {
      TColStd_DataMapOfStringInteger aMap;
      ND->myIntegers = new TDataStd_HDataMapOfStringInteger(aMap);
    };
    ND->myIntegers->ChangeMap().Assign(myIntegers->Map());
  }

  // Reals
  if (HasReals() && !myReals->Map().IsEmpty())
  {
    if(!ND->HasReals()) {
      TDataStd_DataMapOfStringReal aMap;
      ND->myReals = new TDataStd_HDataMapOfStringReal(aMap);
    }; 
    ND->myReals->ChangeMap().Assign(myReals->Map());

  }

  // Strings
  if (HasStrings() && !myStrings->Map().IsEmpty())
  {
    if(!ND->HasStrings()) {
      TDataStd_DataMapOfStringString aMap;
      ND->myStrings = new TDataStd_HDataMapOfStringString(aMap);
    }; 
    ND->myStrings->ChangeMap().Assign(myStrings->Map());
  }

  // Bytes
  if (HasBytes() && !myBytes->Map().IsEmpty())
  {
    if(!ND->HasBytes()) {
      TDataStd_DataMapOfStringByte aMap;
      ND->myBytes = new TDataStd_HDataMapOfStringByte(aMap);
    }; 
    ND->myBytes->ChangeMap().Assign(myBytes->Map());
  }

  // Arrays of integers
  if (HasArraysOfIntegers() && !myArraysOfIntegers->Map().IsEmpty())
  {
    if(!ND->HasArraysOfIntegers()) {
      TDataStd_DataMapOfStringHArray1OfInteger aMap;
      ND->myArraysOfIntegers = new TDataStd_HDataMapOfStringHArray1OfInteger(aMap);
    }

    TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger itr(myArraysOfIntegers->Map());
    for (; itr.More(); itr.Next())
    {
      const Handle(TColStd_HArray1OfInteger)& ints = itr.Value();
      Handle(TColStd_HArray1OfInteger) copied_ints;
      if (!ints.IsNull())
      {
	Standard_Integer lower = ints->Lower(), i = lower, upper = ints->Upper();
	copied_ints = new TColStd_HArray1OfInteger(lower, upper);
	for (; i <= upper; i++)
	{
	  copied_ints->SetValue(i, ints->Value(i));
	}
      }
      ND->myArraysOfIntegers->ChangeMap().Bind(itr.Key(), copied_ints);
    }
  }

  // Arrays of reals
  if (HasArraysOfReals() && !myArraysOfReals->Map().IsEmpty())
  {
    if(!ND->HasArraysOfReals()) {
      TDataStd_DataMapOfStringHArray1OfReal aMap;
      ND->myArraysOfReals = new TDataStd_HDataMapOfStringHArray1OfReal(aMap);
    }
    TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal itr(myArraysOfReals->Map());
    for (; itr.More(); itr.Next())
    {
      const Handle(TColStd_HArray1OfReal)& dbls = itr.Value();
      Handle(TColStd_HArray1OfReal) copied_dbls;
      if (!dbls.IsNull())
      {
	Standard_Integer lower = dbls->Lower(), i = lower, upper = dbls->Upper();
	copied_dbls = new TColStd_HArray1OfReal(lower, upper);
	for (; i <= upper; i++)
	{
	  copied_dbls->SetValue(i, dbls->Value(i));
	}
      }
      ND->myArraysOfReals->ChangeMap().Bind(itr.Key(), copied_dbls);
    }
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_NamedData::Dump (Standard_OStream& anOS) const
{  
  anOS << "NamedData: ";
  anOS << "\tIntegers = " << (HasIntegers() ? myIntegers->Map().Extent() : 0);
  anOS << "\tReals = " << (HasReals() ? myReals->Map().Extent() : 0);
  anOS << "\tStrings = " << (HasStrings() ? myStrings->Map().Extent() : 0);
  anOS << "\tBytes = " << (HasBytes() ? myBytes->Map().Extent() : 0);
  anOS << "\tArraysOfIntegers = " << (HasArraysOfIntegers() ? myArraysOfIntegers->Map().Extent() : 0);
  anOS << "\tArraysOfReals = " << (HasArraysOfReals() ? myArraysOfReals->Map().Extent() : 0);
  return anOS;
}
