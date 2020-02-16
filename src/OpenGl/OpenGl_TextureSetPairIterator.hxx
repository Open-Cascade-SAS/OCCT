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

#ifndef _OpenGl_TextureSetPairIterator_Header
#define _OpenGl_TextureSetPairIterator_Header

#include <OpenGl_TextureSet.hxx>

//! Class for iterating pair of texture sets through each defined texture slot.
//! Note that iterator considers texture slots being in ascending order within OpenGl_TextureSet.
class OpenGl_TextureSetPairIterator
{
public:

  //! Constructor.
  OpenGl_TextureSetPairIterator (const Handle(OpenGl_TextureSet)& theSet1,
                                 const Handle(OpenGl_TextureSet)& theSet2)
  : myIter1 (theSet1),
    myIter2 (theSet2),
    myTexture1 (NULL),
    myTexture2 (NULL),
    myUnitLower (IntegerLast()),
    myUnitUpper (IntegerFirst()),
    myUnitCurrent (0)
  {
    if (!theSet1.IsNull()
     && !theSet1->IsEmpty())
    {
      myUnitLower = Min (myUnitLower, theSet1->FirstUnit());
      myUnitUpper = Max (myUnitUpper, theSet1->LastUnit());
    }
    if (!theSet2.IsNull()
     && !theSet2->IsEmpty())
    {
      myUnitLower = Min (myUnitLower, theSet2->FirstUnit());
      myUnitUpper = Max (myUnitUpper, theSet2->LastUnit());
    }
    myUnitCurrent = myUnitLower;
    myTexture1 = (myIter1.More() && myIter1.Unit() == myUnitCurrent)
               ? myIter1.ChangeValue().get()
               : NULL;
    myTexture2 = (myIter2.More() && myIter2.Unit() == myUnitCurrent)
               ? myIter2.ChangeValue().get()
               : NULL;
  }

  //! Return TRUE if there are more texture units to pass through.
  bool More() const { return myUnitCurrent <= myUnitUpper; }

  //! Return current texture unit.
  Graphic3d_TextureUnit Unit() const { return (Graphic3d_TextureUnit )myUnitCurrent; }

  //! Access texture from first texture set.
  const OpenGl_Texture* Texture1() const { return myTexture1; }

  //! Access texture from second texture set.
  const OpenGl_Texture* Texture2() const { return myTexture2; }

  //! Move iterator position to the next pair.
  void Next()
  {
    ++myUnitCurrent;
    myTexture1 = myTexture2 = NULL;
    for (; myIter1.More(); myIter1.Next())
    {
      if (myIter1.Unit() >= myUnitCurrent)
      {
        myTexture1 = myIter1.Unit() == myUnitCurrent ? myIter1.ChangeValue().get() : NULL;
        break;
      }
    }
    for (; myIter2.More(); myIter2.Next())
    {
      if (myIter2.Unit() >= myUnitCurrent)
      {
        myTexture2 = myIter2.Unit() == myUnitCurrent ? myIter2.ChangeValue().get() : NULL;
        break;
      }
    }
  }

private:

  OpenGl_TextureSet::Iterator myIter1;
  OpenGl_TextureSet::Iterator myIter2;
  OpenGl_Texture*  myTexture1;
  OpenGl_Texture*  myTexture2;
  Standard_Integer myUnitLower;
  Standard_Integer myUnitUpper;
  Standard_Integer myUnitCurrent;

};

#endif //_OpenGl_TextureSetPairIterator_Header
