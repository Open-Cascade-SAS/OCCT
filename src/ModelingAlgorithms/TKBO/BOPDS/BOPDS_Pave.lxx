// Created by: Peter KURNEV
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

//=================================================================================================

inline BOPDS_Pave::BOPDS_Pave()
{
  myIndex     = -1;
  myParameter = 99.;
}

//=================================================================================================

inline void BOPDS_Pave::SetIndex(const int theIndex)
{
  myIndex = theIndex;
}

//=================================================================================================

inline int BOPDS_Pave::Index() const
{
  return myIndex;
}

//=================================================================================================

inline void BOPDS_Pave::SetParameter(const double theParameter)
{
  myParameter = theParameter;
}

//=================================================================================================

inline double BOPDS_Pave::Parameter() const
{
  return myParameter;
}

//=================================================================================================

inline void BOPDS_Pave::Contents(int& theIndex, double& theParameter) const
{
  theIndex     = myIndex;
  theParameter = myParameter;
}

//=================================================================================================

inline bool BOPDS_Pave::IsLess(const BOPDS_Pave& theOther) const
{
  return myParameter < theOther.myParameter;
}

//=================================================================================================

inline bool BOPDS_Pave::IsEqual(const BOPDS_Pave& theOther) const
{
  bool bFlag;
  //
  bFlag = ((myIndex == theOther.myIndex) && (myParameter == theOther.myParameter));
  return bFlag;
}
