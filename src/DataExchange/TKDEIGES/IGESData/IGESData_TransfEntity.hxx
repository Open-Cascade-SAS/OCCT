// Created on: 1992-04-07
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IGESData_TransfEntity_HeaderFile
#define _IGESData_TransfEntity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESEntity.hxx>
class gp_GTrsf;

class IGESData_TransfEntity;
DEFINE_STANDARD_HANDLE(IGESData_TransfEntity, IGESData_IGESEntity)

//! defines required type for Transf in directory part
//! an effective Transf entity must inherits it
class IGESData_TransfEntity : public IGESData_IGESEntity
{

public:
  //! gives value of the transformation, as a GTrsf
  //! To be defined by an effective class of Transformation Entity
  //! Warning : Must take in account Composition : if a TransfEntity has in
  //! its Directory Part, a Transf, this means that it is Compound,
  //! Value must return the global result
  Standard_EXPORT virtual gp_GTrsf Value() const = 0;

  DEFINE_STANDARD_RTTIEXT(IGESData_TransfEntity, IGESData_IGESEntity)

protected:
private:
};

#endif // _IGESData_TransfEntity_HeaderFile
