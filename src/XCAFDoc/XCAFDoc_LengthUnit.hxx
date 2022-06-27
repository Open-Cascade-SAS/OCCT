// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_LengthUnit_HeaderFile
#define _XCAFDoc_LengthUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TDF_DerivedAttribute.hxx>

#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class TDF_Label;
class TDF_RelocationTable;


class XCAFDoc_LengthUnit;
DEFINE_STANDARD_HANDLE(XCAFDoc_LengthUnit, TDF_Attribute)

//! Used to define a Length Unit attribute containing a length unit info
class XCAFDoc_LengthUnit : public TDF_Attribute
{

public:

  //! Returns the GUID of the attribute.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds or creates a LengthUnit attribute
  //! @param theUnitName - name of the unit: mm, m, cm, km, micron, in, min, nin, ft, stat.mile
  //! @param theUnitValue - length scale factor to meter
  //! The LengthUnit attribute is returned.
  Standard_EXPORT static Handle(XCAFDoc_LengthUnit) Set(const TDF_Label& theLabel,
                                                         const TCollection_AsciiString& theUnitName,
                                                         const Standard_Real theUnitValue);

  //! Finds or creates a LengthUnit attribute
  //! @param theUnitValue - length scale factor to meter
  //! The LengthUnit attribute is returned.
  Standard_EXPORT static Handle(XCAFDoc_LengthUnit) Set(const TDF_Label& theLabel,
                                                         const Standard_Real theUnitValue);

  //! Finds, or creates, a LengthUnit attribute with explicit user defined GUID
  //! @param theUnitName - name of the unit: mm, m, cm, km, micron, in, min, nin, ft, stat.mile
  //! @param theUnitValue - length scale factor to meter
  //! The LengthUnit attribute is returned
  Standard_EXPORT static Handle(XCAFDoc_LengthUnit) Set(const TDF_Label& theLabel,
                                                         const Standard_GUID& theGUID,
                                                         const TCollection_AsciiString& theUnitName,
                                                         const Standard_Real theUnitValue);

  //! Creates a LengthUnit attribute
  //! @param theUnitName - name of the unit: mm, m, cm, km, micron, in, min, nin, ft, stat.mile
  //! @param theUnitValue - length scale factor to meter
  Standard_EXPORT void Set(const TCollection_AsciiString& theUnitName,
                           const Standard_Real theUnitValue);
  
  //! Length unit description (could be arbitrary text)
  const TCollection_AsciiString& GetUnitName() const { return myUnitName; }

  //! Returns length unit scale factor to meter
  Standard_Real GetUnitValue() const { return myUnitScaleValue; }

  Standard_EXPORT Standard_Boolean IsEmpty() const { return myUnitName.IsEmpty(); }

  Standard_EXPORT XCAFDoc_LengthUnit();

  Standard_EXPORT virtual const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Restore(const Handle(TDF_Attribute)& theWith) Standard_OVERRIDE;

  Standard_EXPORT virtual void Paste(const Handle(TDF_Attribute)& theInto, const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;


  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_LengthUnit, TDF_Attribute)

private:

  Standard_Real myUnitScaleValue;
  TCollection_AsciiString myUnitName;
};

#endif // _XCAFDoc_LengthUnit_HeaderFile
