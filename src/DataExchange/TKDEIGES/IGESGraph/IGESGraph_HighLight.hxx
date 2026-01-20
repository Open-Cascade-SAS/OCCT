// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESGraph_HighLight_HeaderFile
#define _IGESGraph_HighLight_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>

//! defines IGESHighLight, Type <406> Form <20>
//! in package IGESGraph
//!
//! Attaches information that an entity is to be
//! displayed in some system dependent manner
class IGESGraph_HighLight : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGraph_HighLight();

  //! This method is used to set the fields of the class
  //! HighLight
  //! - nbProps          : Number of property values (NP = 1)
  //! - aHighLightStatus : HighLight Flag
  Standard_EXPORT void Init(const int nbProps,
                            const int aHighLightStatus);

  //! returns the number of property values in <me>
  Standard_EXPORT int NbPropertyValues() const;

  //! returns 0 if <me> is not highlighted(default),
  //! 1 if <me> is highlighted
  Standard_EXPORT int HighLightStatus() const;

  //! returns True if entity is highlighted
  Standard_EXPORT bool IsHighLighted() const;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_HighLight, IGESData_IGESEntity)

private:
  int theNbPropertyValues;
  int theHighLight;
};

#endif // _IGESGraph_HighLight_HeaderFile
