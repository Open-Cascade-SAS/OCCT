// Created on: 2006-05-25
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#include <VrmlData_Scene.hxx>
#include <VrmlData_Coordinate.hxx>
#include <VrmlData_Color.hxx>
#include <VrmlData_Normal.hxx>
#include <VrmlData_TextureCoordinate.hxx>
#include <VrmlData_InBuffer.hxx>
#include <VrmlData_Box.hxx>
#include <VrmlData_Cone.hxx>
#include <VrmlData_Cylinder.hxx>
#include <VrmlData_Sphere.hxx>
#include <VrmlData_UnknownNode.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrim_Cone.hxx>
#include <BRepPrim_Cylinder.hxx>
#include <BRepPrim_Sphere.hxx>
#include <BRepPrim_Builder.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_ErrorHandler.hxx>
#include <VrmlData_ArrayVec3d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlData_Geometry,VrmlData_Node)

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning (disable:4996)
#endif

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const gp_XYZ& VrmlData_ArrayVec3d::Value (const Standard_Size i) const
{
  static gp_XYZ anOrigin (0., 0., 0.);
  return i < myLength ? myArray[i] : anOrigin;
}

//=======================================================================
//function : AllocateValues
//purpose  : 
//=======================================================================

Standard_Boolean VrmlData_ArrayVec3d::AllocateValues
                                (const Standard_Size theLength)
{
  myArray = reinterpret_cast <const gp_XYZ *>
    (Scene().Allocator()->Allocate (theLength*sizeof(gp_XYZ)));
  myLength = theLength;
  return (myArray != 0L);
}

//=======================================================================
//function : VrmlData_Box::TShape
//purpose  : 
//=======================================================================

const Handle(TopoDS_TShape)& VrmlData_Box::TShape ()
{
  if (myIsModified) {
    try {
      const TopoDS_Shell aShell =
        BRepPrimAPI_MakeBox (gp_Pnt (-0.5 * mySize),
                             mySize.X(), mySize.Y(), mySize.Z());
      SetTShape (aShell.TShape());
      myIsModified = Standard_False;
    } catch (Standard_Failure) {
      myTShape.Nullify();
    }
  }
  return myTShape;
}

//=======================================================================
//function : VrmlData_Box::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_Box::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_Box) aResult =
    Handle(VrmlData_Box)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_Box (theOther.IsNull() ? Scene() : theOther->Scene(),
                                Name());
  aResult->SetSize(mySize);
  return aResult;
}

//=======================================================================
//function : VrmlData_Box::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Box::Read (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "size"))
      aStatus = Scene().ReadXYZ (theBuffer, mySize,
                                 Standard_True, Standard_True);
    if (OK(aStatus))
      aStatus = readBrace (theBuffer);
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_Box::Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Box::Write (const char * thePrefix) const
{
  static char header[] = "Box {";
  VrmlData_ErrorStatus aStatus;
  if (OK (aStatus, Scene().WriteLine (thePrefix, header, GlobalIndent())))
  {
    char buf[128];
    Sprintf (buf, "size %.12g %.12g %.12g", mySize.X(), mySize.Y(), mySize.Z());
    Scene().WriteLine (buf);
    aStatus = WriteClosing();
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_Cone::TShape
//purpose  : 
//=======================================================================

const Handle(TopoDS_TShape)& VrmlData_Cone::TShape ()
{
  if (myIsModified && (myHasBottom || myHasSide)) {
    try {
      gp_Ax2 aLocalAxis (gp_Pnt (0., -0.5 * myHeight, 0.),
                         gp_Dir (0., 1., 0.));
      BRepPrim_Cone aBuilder (aLocalAxis, myBottomRadius, 0., myHeight);
      if (!myHasBottom)
        myTShape = aBuilder.LateralFace().TShape();
      else if (!myHasSide) 
        myTShape = aBuilder.BottomFace().TShape();
      else
        myTShape = aBuilder.Shell().TShape();
      myIsModified = Standard_False;
    } catch (Standard_Failure) {
      myTShape.Nullify();
    }
  }
  return myTShape;
}

//=======================================================================
//function : VrmlData_Cone::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_Cone::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_Cone) aResult =
    Handle(VrmlData_Cone)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_Cone (theOther.IsNull() ? Scene(): theOther->Scene(),
                                Name());

  aResult->SetBottomRadius (myBottomRadius);
  aResult->SetHeight (myHeight);
  aResult->SetFaces (myHasBottom, myHasSide);
  return aResult;
}

//=======================================================================
//function : VrmlData_Cone::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Cone::Read (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  Standard_Boolean hasSide(Standard_True), hasBottom(Standard_True);

  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "bottomRadius"))
      aStatus = Scene().ReadReal (theBuffer, myBottomRadius,
                                  Standard_True, Standard_True);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "height"))
      aStatus = Scene().ReadReal (theBuffer, myHeight,
                                  Standard_True, Standard_True);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "side")) {
      if (OK(aStatus, ReadBoolean (theBuffer, hasSide)))
        myHasSide = hasSide;
    } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "bottom")) {
      if (OK(aStatus, ReadBoolean (theBuffer, hasBottom)))
        myHasBottom = hasBottom;
    } else
      break;

    if (!OK(aStatus))
      break;
  }
  // Read the terminating (closing) brace
  if (OK(aStatus))
    aStatus = readBrace (theBuffer);
  return aStatus;
}

//=======================================================================
//function : VrmlData_Cone::Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Cone::Write (const char * thePrefix) const
{
  static char header[] = "Cone {";
  VrmlData_ErrorStatus aStatus;
  if (OK (aStatus, Scene().WriteLine (thePrefix, header, GlobalIndent())))
  {
    char buf[128];
    if ((myBottomRadius - 1.)*(myBottomRadius - 1.) > Precision::Confusion()) {
      Sprintf (buf, "bottomRadius %.12g", myBottomRadius);
      aStatus = Scene().WriteLine (buf);
    }
    if (OK(aStatus) &&
        (myHeight - 2.)*(myHeight - 2.) > Precision::Confusion()) {
      Sprintf (buf, "height       %.12g", myHeight);
      aStatus = Scene().WriteLine (buf);
    }
    if (OK(aStatus) && myHasBottom == Standard_False)
      aStatus = Scene().WriteLine ("bottom   FALSE");
    if (OK(aStatus) && myHasSide == Standard_False)
      aStatus = Scene().WriteLine ("side     FALSE");

    aStatus = WriteClosing();
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_Cone::IsDefault
//purpose  : 
//=======================================================================

// Standard_Boolean VrmlData_Cone::IsDefault () const
// {
//   return
//     (myHasBottom && myHasSide &&
//      ((myBottomRadius - 1.)*(myBottomRadius-1.) < Precision::Confusion()) &&
//      ((myHeight - 2.)*(myHeight - 2.) < Precision::Confusion()));
// }

//=======================================================================
//function : VrmlData_Cylinder::TShape
//purpose  : 
//=======================================================================

const Handle(TopoDS_TShape)& VrmlData_Cylinder::TShape ()
{
  if (myIsModified && (myHasBottom || myHasSide || myHasTop)) {
    try {
      gp_Ax2 aLocalAxis (gp_Pnt (0., -0.5 * myHeight, 0.),
                         gp_Dir (0., 1., 0.));
      BRepPrim_Cylinder aBuilder (aLocalAxis, myRadius, myHeight);
      BRepPrim_Builder aShapeBuilder;
      TopoDS_Shell aShell;
      aShapeBuilder.MakeShell(aShell);
      if (myHasSide)
        aShapeBuilder.AddShellFace (aShell, aBuilder.LateralFace());
      if (myHasTop)
        aShapeBuilder.AddShellFace (aShell, aBuilder.TopFace());
      if (myHasBottom)
        aShapeBuilder.AddShellFace (aShell, aBuilder.BottomFace());
      myTShape = aShell.TShape();
      myIsModified = Standard_False;
    } catch (Standard_Failure) {
      myTShape.Nullify();
    }
  }
  return myTShape;
}

//=======================================================================
//function : VrmlData_Cylinder::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_Cylinder::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_Cylinder) aResult =
    Handle(VrmlData_Cylinder)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_Cylinder(theOther.IsNull()?Scene():theOther->Scene(),
                                    Name());
  aResult->SetRadius (myRadius);
  aResult->SetHeight (myHeight);
  aResult->SetFaces  (myHasBottom, myHasSide, myHasTop);
  return aResult;
}

//=======================================================================
//function : VrmlData_Cylinder::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Cylinder::Read (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  Standard_Boolean hasSide(Standard_True), hasBottom(Standard_True);
  Standard_Boolean hasTop (Standard_True);

  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "radius"))
      aStatus = Scene().ReadReal (theBuffer, myRadius,
                                  Standard_True, Standard_True);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "height"))
      aStatus = Scene().ReadReal (theBuffer, myHeight,
                                  Standard_True, Standard_True);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "top")) {
      if (OK(aStatus, ReadBoolean (theBuffer, hasTop)))
        myHasTop = hasTop;
    } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "side")) {
      if (OK(aStatus, ReadBoolean (theBuffer, hasSide)))
        myHasSide = hasSide;
    } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "bottom")) {
      if (OK(aStatus, ReadBoolean (theBuffer, hasBottom)))
        myHasBottom = hasBottom;
    } else
      break;

    if (!OK(aStatus))
      break;
  }

  // Read the terminating (closing) brace
  if (OK(aStatus))
    aStatus = readBrace (theBuffer);
  return aStatus;
}

//=======================================================================
//function : VrmlData_Cylinder::Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Cylinder::Write (const char * thePrefix) const
{
  static char header[] = "Cylinder {";
  VrmlData_ErrorStatus aStatus;
  if (OK (aStatus, Scene().WriteLine (thePrefix, header, GlobalIndent())))
  {
    char buf[128];
    if ((myRadius - 1.)*(myRadius - 1.) > Precision::Confusion()) {
      Sprintf (buf, "radius   %.12g", myRadius);
      aStatus = Scene().WriteLine (buf);
    }
    if (OK(aStatus) &&
        (myHeight - 2.)*(myHeight - 2.) > Precision::Confusion()) {
      Sprintf (buf, "height   %.12g", myHeight);
      aStatus = Scene().WriteLine (buf);
    }
    if (OK(aStatus) && myHasBottom == Standard_False)
      aStatus = Scene().WriteLine ("bottom   FALSE");
    if (OK(aStatus) && myHasSide == Standard_False)
      aStatus = Scene().WriteLine ("side     FALSE");
    if (OK(aStatus) && myHasTop == Standard_False)
      aStatus = Scene().WriteLine ("top      FALSE");

    aStatus = WriteClosing();
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_Cylinder::IsDefault
//purpose  : 
//=======================================================================

// Standard_Boolean VrmlData_Cylinder::IsDefault () const
// {
//   return
//     (myHasBottom && myHasSide && myHasTop &&
//      ((myRadius - 1.)*(myRadius - 1.) < Precision::Confusion()) &&
//      ((myHeight - 2.)*(myHeight - 2.) < Precision::Confusion()));
// }

//=======================================================================
//function : VrmlData_Sphere::TShape
//purpose  : 
//=======================================================================

const Handle(TopoDS_TShape)& VrmlData_Sphere::TShape ()
{
  if (myIsModified) {
    try {
      myTShape = BRepPrim_Sphere(myRadius).Shell().TShape();
      myIsModified = Standard_False;
    } catch (Standard_Failure) {
      myTShape.Nullify();
    }
  }
  return myTShape;
}

//=======================================================================
//function : VrmlData_Sphere::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_Sphere::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_Sphere) aResult =
    Handle(VrmlData_Sphere)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_Sphere(theOther.IsNull()? Scene() :theOther->Scene(),
                                  Name());
  aResult->SetRadius (myRadius);
  return aResult;
}

//=======================================================================
//function : VrmlData_Sphere::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Sphere::Read (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
    if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "radius"))
      aStatus = Scene().ReadReal (theBuffer, myRadius,
                                  Standard_True, Standard_True);
    else
      break;

  // Read the terminating (closing) brace
  if (OK(aStatus))
    aStatus = readBrace (theBuffer);
  return aStatus;
}

//=======================================================================
//function : VrmlData_Sphere::Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Sphere::Write (const char * thePrefix) const
{
  static char header[] = "Sphere {";
  VrmlData_ErrorStatus aStatus;
  if (OK (aStatus, Scene().WriteLine (thePrefix, header, GlobalIndent())))
  {
    char buf[128];
    Sprintf (buf, "radius   %.12g", myRadius);
    Scene().WriteLine (buf);
    aStatus = WriteClosing();
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_Sphere::IsDefault
//purpose  : 
//=======================================================================

// Standard_Boolean VrmlData_Sphere::IsDefault () const
// {
//   return ((myRadius - 1.)*(myRadius - 1.) < Precision::Confusion())
// }

//=======================================================================
//function : VrmlData_TextureCoordinate::AllocateValues
//purpose  : 
//=======================================================================

Standard_Boolean VrmlData_TextureCoordinate::AllocateValues
                                (const Standard_Size theLength)
{
  myPoints = reinterpret_cast <const gp_XY *>
    (Scene().Allocator()->Allocate (theLength*sizeof(gp_XY)));
  myLength = theLength;
  return (myPoints != 0L);
}

//=======================================================================
//function : VrmlData_TextureCoordinate::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_TextureCoordinate::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_TextureCoordinate) aResult =
    Handle(VrmlData_TextureCoordinate)::DownCast
    (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_TextureCoordinate
      (theOther.IsNull() ? Scene() : theOther->Scene(), Name());
  if (&aResult->Scene() == &Scene())
    aResult->SetPoints (myLength, myPoints);
  else {
    aResult->AllocateValues (myLength);
    for (Standard_Size i = 0; i < myLength; i++)
      const_cast <gp_XY&> (aResult->myPoints[i]) = myPoints[i];
  }
  return aResult;
}

//=======================================================================
//function : VrmlData_TextureCoordinate::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_TextureCoordinate::Read
                                        (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  NCollection_Vector<gp_XY> vecValues;
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    // Match the name with the current word in the stream
    if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "point"))
      // Read the body of the data node (comma-separated list of duplets)
      if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
        if (theBuffer.LinePtr[0] != '[')  // opening bracket
          aStatus = VrmlData_VrmlFormatError;
        else {
          theBuffer.LinePtr++;
          for(;;) {
            gp_XY anXY;
            if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
              break;
            // closing bracket, in case that it follows a comma
            if (theBuffer.LinePtr[0] == ']') {
              theBuffer.LinePtr++;
              break;
            }
            if (!OK(aStatus, Scene().ReadXY(theBuffer, anXY,
                                            Standard_False, Standard_False)))
              break;
            vecValues.Append(anXY);
            if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
              break;
            if (theBuffer.LinePtr[0] == ',') {
              theBuffer.LinePtr++;
            } else if (theBuffer.LinePtr[0] == ']') { // closing bracket
              theBuffer.LinePtr++;
              break;
            }
          }
        }
      }
    if (OK(aStatus) && OK(aStatus, readBrace (theBuffer))) {
      myLength = vecValues.Length();
      if (myLength > 0) {
        gp_XY * aPoints = reinterpret_cast <gp_XY *>
          (Scene().Allocator()->Allocate (myLength * sizeof(gp_XY)));
        myPoints = aPoints;
        for (Standard_Integer i = 0; i < Standard_Integer(myLength); i++)
          aPoints[i] = vecValues(i);
      }
    }
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_ArrayVec3d::Clone
//purpose  : 
//=======================================================================

// Handle(VrmlData_Node) VrmlData_ArrayVec3d::Clone
//                                 (const Handle(VrmlData_Node)& theOther) const
// {
//   VrmlData_Node::Clone (theOther);
//   const Handle(VrmlData_ArrayVec3d) anArrayNode =
//     Handle(VrmlData_ArrayVec3d)::DownCast (theOther);
//   if (anArrayNode.IsNull() == Standard_False)
//     anArrayNode->SetValues (myLength, myArray);
//   return theOther;
// }

//=======================================================================
//function : VrmlData_ArrayVec3d::ReadArray
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_ArrayVec3d::ReadArray
                                        (VrmlData_InBuffer&     theBuffer,
                                         const char *           theName,
                                         const Standard_Boolean isScale)
{
  VrmlData_ErrorStatus aStatus;
  NCollection_Vector<gp_XYZ> vecValues;
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    // Match the name with the current word in the stream
    if (theName) {
      const Standard_Size aNameLen = strlen(theName);
      if (strncmp (theBuffer.LinePtr, theName, aNameLen))
        aStatus = VrmlData_VrmlFormatError;
      else
        theBuffer.LinePtr += aNameLen;
    } else {
      // Skip the word in the input
      while (theBuffer.LinePtr[0] != ' ' &&
             theBuffer.LinePtr[0] != ',' &&
             theBuffer.LinePtr[0] != '\t' &&
             theBuffer.LinePtr[0] != '\n' &&
             theBuffer.LinePtr[0] != '\r' &&
             theBuffer.LinePtr[0] != '\0')
        theBuffer.LinePtr++;
    }
    // Read the body of the data node (list of triplets)
    if (OK(aStatus) && OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
      if (theBuffer.LinePtr[0] != '[')  // opening bracket
        aStatus = VrmlData_VrmlFormatError;
      else {
        theBuffer.LinePtr++;
        for(;;) {
          gp_XYZ anXYZ;
          if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
            break;
          // closing bracket, in case that it follows a comma
          if (theBuffer.LinePtr[0] == ']') {
            theBuffer.LinePtr++;
            break;
          }
          // Read three numbers (XYZ value)
          if (!OK(aStatus, Scene().ReadXYZ(theBuffer, anXYZ,
                                           isScale, Standard_False)))
            break;
          vecValues.Append(anXYZ);
          if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
            break;
          if (theBuffer.LinePtr[0] == ']') {// closing bracket
            theBuffer.LinePtr++;
            break;
          }
        }
      }
    }
    if (OK(aStatus) && OK(aStatus, readBrace (theBuffer))) {
      myLength = vecValues.Length();
      if (myLength > 0) {
        gp_XYZ * anArray = reinterpret_cast <gp_XYZ *>
          (Scene().Allocator()->Allocate (myLength * sizeof(gp_XYZ)));
        myArray = anArray;
        for (Standard_Integer i = 0; i < Standard_Integer(myLength); i++)
          anArray[i] = vecValues(i);
      }
    }
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_ArrayVec3d::WriteArray
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_ArrayVec3d::WriteArray
                                        (const char *           theName,
                                         const Standard_Boolean isScale) const
{
  VrmlData_ErrorStatus aStatus (VrmlData_StatusOK);
  if (myLength > 0) {
    aStatus = Scene().WriteLine (theName, "[", 2*GlobalIndent());
    if (OK(aStatus)) {
      for (Standard_Size i = 0; i < myLength-1; i++)
        if (!OK (aStatus, Scene().WriteXYZ (myArray[i], isScale, ",")))
          break;
      if (OK(aStatus))
        aStatus = Scene().WriteXYZ (myArray[myLength-1], isScale);
    }
    if (aStatus == VrmlData_StatusOK)
      aStatus = Scene().WriteLine ("]", 0L, -2*GlobalIndent());
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_ArrayVec3d::IsDefault
//purpose  : 
//=======================================================================

Standard_Boolean VrmlData_ArrayVec3d::IsDefault () const
{
  return myLength == 0;
}

//=======================================================================
//function : VrmlData_Coodinate::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_Coordinate::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_Coordinate) aResult =
    Handle(VrmlData_Coordinate)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_Coordinate
      (theOther.IsNull() ? Scene() : theOther->Scene(), Name());
  if (&aResult->Scene() == &Scene())
    aResult->SetValues (Length(), Values());
  else {
    aResult->AllocateValues (Length());
    for (Standard_Size i = 0; i < Length(); i++)
      const_cast <gp_XYZ&> (aResult->Values()[i]) = Values()[i];
  }
  return aResult;
}

//=======================================================================
//function : VrmlData_Coordinate::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Coordinate::Read (VrmlData_InBuffer& theBuffer)
{
  return VrmlData_ArrayVec3d::ReadArray (theBuffer, "point", Standard_True);
}

//=======================================================================
//function : VrmlData_Coordinate::Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Coordinate::Write (const char * thePrefix) const
{
  static char header[] = "Coordinate {";
  VrmlData_ErrorStatus aStatus;
  if (OK (aStatus, Scene().WriteLine (thePrefix, header, GlobalIndent())))
  {
    WriteArray ("point", Standard_True);
    aStatus = WriteClosing();
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_Color::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_Color::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_Color) aResult =
    Handle(VrmlData_Color)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_Color(theOther.IsNull()? Scene() : theOther->Scene(),
                                 Name());
  if (&aResult->Scene() == &Scene())
    aResult->SetValues (Length(), Values());
  else {
    aResult->AllocateValues (Length());
    for (Standard_Size i = 0; i < Length(); i++)
      const_cast <gp_XYZ&> (aResult->Values()[i]) = Values()[i];
  }
  return aResult;
}

//=======================================================================
//function : VrmlData_Color::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Color::Read (VrmlData_InBuffer& theBuffer)
{
  return ReadArray (theBuffer, "color", Standard_False);
}

//=======================================================================
//function : VrmlData_Color::Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Color::Write (const char * thePrefix) const
{
  static char header[] = "Color {";
  VrmlData_ErrorStatus aStatus;
  if (OK (aStatus, Scene().WriteLine (thePrefix, header, GlobalIndent())))
  {
    WriteArray ("color", Standard_False);
    aStatus = WriteClosing();
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_Normal::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_Normal::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_Normal) aResult =
    Handle(VrmlData_Normal)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult= new VrmlData_Normal(theOther.IsNull()? Scene() : theOther->Scene(),
                                 Name());
  if (&aResult->Scene() == &Scene())
    aResult->SetValues (Length(), Values());
  else {
    aResult->AllocateValues (Length());
    for (Standard_Size i = 0; i < Length(); i++)
      const_cast <gp_XYZ&> (aResult->Values()[i]) = Values()[i];
  }
  return aResult;
}

//=======================================================================
//function : VrmlData_Normal::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Normal::Read (VrmlData_InBuffer& theBuffer)
{
  return VrmlData_ArrayVec3d::ReadArray (theBuffer, "vector", Standard_False);
}

//=======================================================================
//function : VrmlData_Normal::Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Normal::Write (const char * thePrefix) const
{
  static char header[] = "Normal {";
  VrmlData_ErrorStatus aStatus;
  if (OK (aStatus, Scene().WriteLine (thePrefix, header, GlobalIndent())))
  {
    WriteArray ("vector", Standard_False);
    aStatus = WriteClosing();
  }
  return aStatus;
}
