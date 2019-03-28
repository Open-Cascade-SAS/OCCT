// Created by: DAUTRY Philippe
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

#ifndef _TDF_LabelMapHasher_HeaderFile
#define _TDF_LabelMapHasher_HeaderFile

#include <TDF_Label.hxx>

//! A label hasher for label maps.
class TDF_LabelMapHasher 
{
public:
  //! Computes a hash code for the given label, in the range [1, theUpperBound]
  //! @param theLabel the label which hash code is to be computed
  //! @param theUpperBound the upper bound of the range a computing hash code must be within
  //! @return a computed hash code, in the range [1, theUpperBound]
  static Standard_Integer HashCode(const TDF_Label& theLabel, const Standard_Integer theUpperBound)
  {
    return ::HashCode(theLabel.myLabelNode, theUpperBound);
  }

  //! Returns True  when the two  keys are the same. Two
  //! same  keys  must   have  the  same  hashcode,  the
  //! contrary is not necessary.
  static Standard_Boolean IsEqual(const TDF_Label& aLab1, const TDF_Label& aLab2)
  {
    return aLab1.IsEqual(aLab2);
  }
};

#endif // _TDF_LabelMapHasher_HeaderFile
