// Created by: Sergey RUIN
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _TDocStd_CompoundDelta_HeaderFile
#define _TDocStd_CompoundDelta_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Delta.hxx>
class TDocStd_Document;


class TDocStd_CompoundDelta;
DEFINE_STANDARD_HANDLE(TDocStd_CompoundDelta, TDF_Delta)

//! A delta set is available at <aSourceTime>. If
//! applied, it restores the TDF_Data in the state it
//! was at <aTargetTime>.
class TDocStd_CompoundDelta : public TDF_Delta
{

public:

  
  //! Creates a compound delta.
  //! Validates <me> at <aBeginTime>. If applied, it
  //! restores the TDF_Data in the state it was at
  //! <anEndTime>. Reserved to TDF_Data.
  Standard_EXPORT TDocStd_CompoundDelta();


friend class TDocStd_Document;


  DEFINE_STANDARD_RTTIEXT(TDocStd_CompoundDelta,TDF_Delta)

protected:




private:




};







#endif // _TDocStd_CompoundDelta_HeaderFile
