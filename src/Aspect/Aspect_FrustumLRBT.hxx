// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _Aspect_FrustumLRBT_HeaderFile
#define _Aspect_FrustumLRBT_HeaderFile

//! Structure defining frustum boundaries.
template<typename Elem_t>
struct Aspect_FrustumLRBT
{
  Elem_t Left;
  Elem_t Right;
  Elem_t Bottom;
  Elem_t Top;

  //! Empty constructor.
  Aspect_FrustumLRBT() : Left (0), Right (0), Bottom (0), Top (0) {}

  //! Copy/cast constructor.
  template<typename Other_t>
  explicit Aspect_FrustumLRBT (const Aspect_FrustumLRBT<Other_t>& theOther)
  : Left  (static_cast<Elem_t> (theOther.Left)),
    Right (static_cast<Elem_t> (theOther.Right)),
    Bottom(static_cast<Elem_t> (theOther.Bottom)),
    Top   (static_cast<Elem_t> (theOther.Top)) {}

  //! Apply multiply factor.
  void Multiply (Elem_t theScale)
  {
    Left   *= theScale;
    Right  *= theScale;
    Bottom *= theScale;
    Top    *= theScale;
  }

  //! Return multiplied frustum.
  Aspect_FrustumLRBT<Elem_t> Multiplied (Elem_t theScale)
  {
    Aspect_FrustumLRBT<Elem_t> aCopy (*this);
    aCopy.Multiply (theScale);
    return aCopy;
  }
};

#endif // _Aspect_FrustumLRBT_HeaderFile
