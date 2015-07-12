// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef _BOPDS_PassKeyMapHasher_HeaderFile
#define _BOPDS_PassKeyMapHasher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class BOPDS_PassKey;



class BOPDS_PassKeyMapHasher 
{
public:

  DEFINE_STANDARD_ALLOC

  
    static Standard_Integer HashCode (const BOPDS_PassKey& aPKey, const Standard_Integer Upper);
  
    static Standard_Boolean IsEqual (const BOPDS_PassKey& aPKey1, const BOPDS_PassKey& aPKey2);




protected:





private:





};


#include <BOPDS_PassKeyMapHasher.lxx>





#endif // _BOPDS_PassKeyMapHasher_HeaderFile
