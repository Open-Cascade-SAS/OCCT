// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef _Sample2D_Image_HeaderFile
#define _Sample2D_Image_HeaderFile
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

#include <OSD_File.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Face.hxx>

//! AIS shape for sample 2D image
class Sample2D_Image : public AIS_Shape
{
  DEFINE_STANDARD_RTTI_INLINE(Sample2D_Image, AIS_Shape)
public:

  //! Constructor.
  Standard_EXPORT Sample2D_Image (const TCollection_AsciiString& theFileName,
                                  const Standard_Real theX = 0.0,
                                  const Standard_Real theY = 0.0,
                                  const Standard_Real theScale = 1.0);

  //! Return image coordinates.
  void GetCoord (Standard_Real& theX, Standard_Real& theY) const
  {
    theX = myX;
    theY = myY;
  }

  //! Return image coordinates.
  void SetCoord (const Standard_Real theX, const Standard_Real theY)
  {
    myX = theX;
    myY = theY;
  }

  //! Return image scale factor.
  Standard_Real GetScale() const { return myScale; }

  //! Set image scale factor.
  void SetScale(const Standard_Real theNewScale) { myScale = theNewScale; }

  //! Assign new interactive context to the object.
  Standard_EXPORT virtual void SetContext (const Handle(AIS_InteractiveContext)& theContext) Standard_OVERRIDE;

private:

  void MakeShape();

protected:

  TopoDS_Face myFace;
  TCollection_AsciiString myFilename;
  Standard_Real myX;
  Standard_Real myY;
  Standard_Real myScale;

};

#endif
