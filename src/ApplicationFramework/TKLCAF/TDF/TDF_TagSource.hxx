// Created on: 1997-08-04
// Created by: VAUTHIER Jean-Claude
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

#ifndef _TDF_TagSource_HeaderFile
#define _TDF_TagSource_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TDF_Attribute.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_RelocationTable;

class TDF_TagSource;
DEFINE_STANDARD_HANDLE(TDF_TagSource, TDF_Attribute)

//! This attribute manage   a tag provider   to create
//! child labels of a given one.
class TDF_TagSource : public TDF_Attribute
{

public:
  //! class methods
  //! =============
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Find, or create, a  TagSource attribute. the TagSource
  //! attribute is returned.
  Standard_EXPORT static Handle(TDF_TagSource) Set(const TDF_Label& label);

  //! Find (or create) a  tagSource attribute located at <L>
  //! and make a new child label.
  //! TagSource methods
  //! =================
  Standard_EXPORT static TDF_Label NewChild(const TDF_Label& L);

  Standard_EXPORT TDF_TagSource();

  Standard_EXPORT Standard_Integer NewTag();

  Standard_EXPORT TDF_Label NewChild();

  Standard_EXPORT Standard_Integer Get() const;

  //! TDF_Attribute methods
  //! =====================
  Standard_EXPORT void Set(const Standard_Integer T);

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& with) Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       Into,
                             const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TDF_TagSource, TDF_Attribute)

protected:
private:
  Standard_Integer myTag;
};

#endif // _TDF_TagSource_HeaderFile
