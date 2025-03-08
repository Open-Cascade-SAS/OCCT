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

#include <XCAFDoc_DimTol.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_DimTol, TDF_Attribute)

//=================================================================================================

XCAFDoc_DimTol::XCAFDoc_DimTol() {}

//=================================================================================================

const Standard_GUID& XCAFDoc_DimTol::GetID()
{
  static Standard_GUID DGTID("58ed092d-44de-11d8-8776-001083004c77");
  // static Standard_GUID ID("efd212e9-6dfd-11d4-b9c8-0060b0ee281b");
  return DGTID;
  // return ID;
}

//=================================================================================================

Handle(XCAFDoc_DimTol) XCAFDoc_DimTol::Set(const TDF_Label&                        label,
                                           const Standard_Integer                  kind,
                                           const Handle(TColStd_HArray1OfReal)&    aVal,
                                           const Handle(TCollection_HAsciiString)& aName,
                                           const Handle(TCollection_HAsciiString)& aDescription)
{
  Handle(XCAFDoc_DimTol) A;
  if (!label.FindAttribute(XCAFDoc_DimTol::GetID(), A))
  {
    A = new XCAFDoc_DimTol();
    label.AddAttribute(A);
  }
  A->Set(kind, aVal, aName, aDescription);
  return A;
}

//=================================================================================================

void XCAFDoc_DimTol::Set(const Standard_Integer                  kind,
                         const Handle(TColStd_HArray1OfReal)&    aVal,
                         const Handle(TCollection_HAsciiString)& aName,
                         const Handle(TCollection_HAsciiString)& aDescription)
{
  Backup();
  myKind        = kind;
  myVal         = aVal;
  myName        = aName;
  myDescription = aDescription;
}

//=================================================================================================

Standard_Integer XCAFDoc_DimTol::GetKind() const
{
  return myKind;
}

//=================================================================================================

Handle(TColStd_HArray1OfReal) XCAFDoc_DimTol::GetVal() const
{
  return myVal;
}

//=================================================================================================

Handle(TCollection_HAsciiString) XCAFDoc_DimTol::GetName() const
{
  return myName;
}

//=================================================================================================

Handle(TCollection_HAsciiString) XCAFDoc_DimTol::GetDescription() const
{
  return myDescription;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_DimTol::ID() const
{
  return GetID();
}

//=================================================================================================

void XCAFDoc_DimTol::Restore(const Handle(TDF_Attribute)& With)
{
  myKind        = Handle(XCAFDoc_DimTol)::DownCast(With)->GetKind();
  myVal         = Handle(XCAFDoc_DimTol)::DownCast(With)->GetVal();
  myName        = Handle(XCAFDoc_DimTol)::DownCast(With)->GetName();
  myDescription = Handle(XCAFDoc_DimTol)::DownCast(With)->GetDescription();
}

//=================================================================================================

Handle(TDF_Attribute) XCAFDoc_DimTol::NewEmpty() const
{
  return new XCAFDoc_DimTol();
}

//=================================================================================================

void XCAFDoc_DimTol::Paste(const Handle(TDF_Attribute)& Into,
                           const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(XCAFDoc_DimTol)::DownCast(Into)->Set(myKind, myVal, myName, myDescription);
}

//=================================================================================================

void XCAFDoc_DimTol::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myKind)

  for (TColStd_Array1OfReal::Iterator aValIt(myVal->Array1()); aValIt.More(); aValIt.Next())
  {
    const Standard_Real& aValue = aValIt.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aValue)
  }

  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myName.get())
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myDescription.get())
}
