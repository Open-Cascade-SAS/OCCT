// Created on: 2014-09-30
// Created by: Denis BOGOLEPOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_Utils_H__
#define _OpenGl_Utils_H__

#include <OpenGl_Vec.hxx>
#include <NCollection_Vector.hxx>

//! Helper class that implements some functionality of GLU library.
namespace OpenGl_Utils
{

  //! Matrix type selector.
  template<class T>
  struct MatrixType {
    //
  };

  template<>
  struct MatrixType<Standard_Real> {
    typedef OpenGl_Mat4d Mat4;
  };

  template<>
  struct MatrixType<Standard_ShortReal> {
    typedef OpenGl_Mat4 Mat4;
  };

  //! Vector type selector.
  template<class T>
  struct VectorType {
    //
  };

  template<>
  struct VectorType<Standard_Real> {
    typedef OpenGl_Vec2d Vec2;
    typedef OpenGl_Vec3d Vec3;
    typedef OpenGl_Vec4d Vec4;
  };

  template<>
  struct VectorType<Standard_ShortReal> {
    typedef OpenGl_Vec2 Vec2;
    typedef OpenGl_Vec3 Vec3;
    typedef OpenGl_Vec4 Vec4;
  };

  //! Software implementation for OpenGL matrix stack.
  template<class T>
  class MatrixState
  {
  public:

    //! Constructs matrix state object.
    MatrixState()
    : myStack (8),
      myStackHead (-1)
    {
      //
    }

    //! Pushes current matrix into stack.
    void Push()
    {
      if (++myStackHead >= myStack.Size())
      {
        myStack.Append (myCurrent);
      }
      else
      {
        myStack.SetValue (myStackHead, myCurrent);
      }
    }

    //! Pops matrix from stack to current.
    void Pop()
    {
      Standard_ASSERT_RETURN (myStackHead != -1, "Matrix stack already empty when MatrixState.Pop() called.", );
      myCurrent = myStack.Value (myStackHead--);
    }

    //! @return current matrix.
    const typename MatrixType<T>::Mat4& Current()
    {
      return myCurrent;
    }

    //! Sets given matrix as current.
    void SetCurrent (const typename MatrixType<T>::Mat4& theNewCurrent)
    {
      myCurrent = theNewCurrent;
    }

    //! Sets given matrix as current.
    template <typename Other_t>
    void SetCurrent (const typename MatrixType<Other_t>::Mat4& theNewCurrent)
    {
      myCurrent.Convert (theNewCurrent);
    }

    //! Sets current matrix to identity.
    void SetIdentity()
    {
      myCurrent = typename MatrixType<T>::Mat4();
    }

  private:

    NCollection_Vector<typename MatrixType<T>::Mat4> myStack;     //!< Collection used to maintenance matrix stack
    typename MatrixType<T>::Mat4                     myCurrent;   //!< Current matrix
    Standard_Integer                                 myStackHead; //!< Index of stack head

  };

  //! Constructs a 3D orthographic projection matrix.
  template<class T>
  static void Ortho (typename MatrixType<T>::Mat4& theOut,
    const T theLeft, const T theRight, const T theBottom, const T theTop, const T theZNear, const T theZFar);

  //! Constructs a 2D orthographic projection matrix.
  template<class T>
  static void Ortho2D (typename MatrixType<T>::Mat4& theOut,
    const T theLeft, const T theRight, const T theBottom, const T theTop);

  //! Maps object coordinates to window coordinates.
  template<class T>
  static Standard_Boolean Project (const T                             theObjX,
                                   const T                             theObjY,
                                   const T                             theObjZ,
                                   const typename MatrixType<T>::Mat4& theModViewMat,
                                   const typename MatrixType<T>::Mat4& theProjectMat,
                                   const Standard_Integer              theViewport[4],
                                   T&                                  theWinX,
                                   T&                                  theWinY,
                                   T&                                  theWinZ);

  //! Maps window coordinates to object coordinates.
  template<class T>
  static Standard_Boolean UnProject (const T                             theWinX,
                                     const T                             theWinY,
                                     const T                             theWinZ,
                                     const typename MatrixType<T>::Mat4& theModViewMat,
                                     const typename MatrixType<T>::Mat4& theProjectMat,
                                     const Standard_Integer              theViewport[4],
                                     T&                                  theObjX,
                                     T&                                  theObjY,
                                     T&                                  theObjZ);

  //! Constructs a 4x4 rotation matrix.
  template<class T>
  static void ConstructRotate (typename MatrixType<T>::Mat4& theOut,
                               T                             theA,
                               T                             theX,
                               T                             theY,
                               T                             theZ);

  //! Constructs a 4x4 rotation matrix.
  template<class T>
  static void Rotate (typename MatrixType<T>::Mat4& theOut,
                      T                             theA,
                      T                             theX,
                      T                             theY,
                      T                             theZ);

  //! Constructs a 4x4 scaling matrix.
  template<class T>
  static void Scale (typename MatrixType<T>::Mat4& theOut,
                     T                             theX,
                     T                             theY,
                     T                             theZ);

  //! Constructs a 4x4 translation matrix.
  template<class T>
  static void Translate (typename MatrixType<T>::Mat4& theOut,
                         T                             theX,
                         T                             theY,
                         T                             theZ);

}

// =======================================================================
// function : Rotate
// purpose  : Constructs a 4x4 rotation matrix
// =======================================================================
template<class T>
void OpenGl_Utils::Rotate (typename MatrixType<T>::Mat4& theOut,
                           T                             theA,
                           T                             theX,
                           T                             theY,
                           T                             theZ)
{
  typename MatrixType<T>::Mat4 aMat;
  ConstructRotate (aMat, theA, theX, theY, theZ);
  theOut = theOut * aMat;
}

// =======================================================================
// function : Translate
// purpose  : Constructs a 4x4 translation matrix
// =======================================================================
template<class T>
void OpenGl_Utils::Translate (typename MatrixType<T>::Mat4& theOut,
                              T                             theX,
                              T                             theY,
                              T                             theZ)
{
  theOut.ChangeValue (0, 3) = theOut.GetValue (0, 0) * theX +
                              theOut.GetValue (0, 1) * theY +
                              theOut.GetValue (0, 2) * theZ +
                              theOut.GetValue (0, 3);

  theOut.ChangeValue (1, 3) = theOut.GetValue (1, 0) * theX +
                              theOut.GetValue (1, 1) * theY +
                              theOut.GetValue (1, 2) * theZ +
                              theOut.GetValue (1, 3);

  theOut.ChangeValue (2, 3) = theOut.GetValue (2, 0) * theX +
                              theOut.GetValue (2, 1) * theY +
                              theOut.GetValue (2, 2) * theZ +
                              theOut.GetValue (2, 3);

  theOut.ChangeValue (3, 3) = theOut.GetValue (3, 0) * theX +
                              theOut.GetValue (3, 1) * theY +
                              theOut.GetValue (3, 2) * theZ +
                              theOut.GetValue (3, 3);
}

// =======================================================================
// function : Scale
// purpose  : Constructs a 4x4 scaling matrix
// =======================================================================
template<class T>
void OpenGl_Utils::Scale (typename MatrixType<T>::Mat4& theOut,
                          T                             theX,
                          T                             theY,
                          T                             theZ)
{
  theOut.ChangeValue (0, 0) *= theX;
  theOut.ChangeValue (1, 0) *= theX;
  theOut.ChangeValue (2, 0) *= theX;
  theOut.ChangeValue (3, 0) *= theX;

  theOut.ChangeValue (0, 1) *= theY;
  theOut.ChangeValue (1, 1) *= theY;
  theOut.ChangeValue (2, 1) *= theY;
  theOut.ChangeValue (3, 1) *= theY;

  theOut.ChangeValue (0, 2) *= theZ;
  theOut.ChangeValue (1, 2) *= theZ;
  theOut.ChangeValue (2, 2) *= theZ;
  theOut.ChangeValue (3, 2) *= theZ;
}

// =======================================================================
// function : ConstructRotate
// purpose  : Constructs a 4x4 rotation matrix
// =======================================================================
template<class T>
void OpenGl_Utils::ConstructRotate (typename MatrixType<T>::Mat4& theOut, T theA, T theX, T theY, T theZ)
{
  const T aSin = std::sin (theA * static_cast<T> (M_PI / 180.0));
  const T aCos = std::cos (theA * static_cast<T> (M_PI / 180.0));

  const Standard_Boolean isOnlyX = (theX != static_cast<T> (0.0))
                                && (theY == static_cast<T> (0.0))
                                && (theZ == static_cast<T> (0.0));

  const Standard_Boolean isOnlyY = (theX == static_cast<T> (0.0))
                                && (theY != static_cast<T> (0.0))
                                && (theZ == static_cast<T> (0.0));

  const Standard_Boolean isOnlyZ = (theX == static_cast<T> (0.0))
                                && (theY == static_cast<T> (0.0))
                                && (theZ != static_cast<T> (0.0));

  if (isOnlyX) // Rotation only around X
  {
    theOut.SetValue (1, 1, aCos);
    theOut.SetValue (2, 2, aCos);

    if (theX < static_cast<T> (0.0))
    {
      theOut.SetValue (1, 2,  aSin);
      theOut.SetValue (2, 1, -aSin);
    }
    else
    {
      theOut.SetValue (1, 2, -aSin);
      theOut.SetValue (2, 1,  aSin);
    }

    return;
  }
  else if (isOnlyY) // Rotation only around Y
  {
    theOut.SetValue (0, 0, aCos);
    theOut.SetValue (2, 2, aCos);

    if (theY < static_cast<T> (0.0))
    {
      theOut.SetValue (0, 2, -aSin);
      theOut.SetValue (2, 0,  aSin);
    }
    else
    {
      theOut.SetValue (0, 2,  aSin);
      theOut.SetValue (2, 0, -aSin);
    }

    return;
  }
  else if (isOnlyZ) // Rotation only around Z
  {
    theOut.SetValue (0, 0, aCos);
    theOut.SetValue (1, 1, aCos);

    if (theZ < static_cast<T> (0.0))
    {
      theOut.SetValue (0, 1,  aSin);
      theOut.SetValue (1, 0, -aSin);
    }
    else
    {
      theOut.SetValue (0, 1, -aSin);
      theOut.SetValue (1, 0,  aSin);
    }

    return;
  }

  T aNorm = std::sqrt (theX * theX + theY * theY + theZ * theZ);

  if (aNorm <= static_cast<T> (1.0e-4))
  {
    return; // negligible rotation
  }

  aNorm = static_cast<T> (1.0) / aNorm;

  theX *= aNorm;
  theY *= aNorm;
  theZ *= aNorm;

  const T aXX = theX * theX;
  const T aYY = theY * theY;
  const T aZZ = theZ * theZ;
  const T aXY = theX * theY;
  const T aYZ = theY * theZ;
  const T aZX = theZ * theX;
  const T aSinX = theX * aSin;
  const T aSinY = theY * aSin;
  const T aSinZ = theZ * aSin;

  const T aOneMinusCos = static_cast<T> (1.0) - aCos;

  theOut.SetValue (0, 0, aOneMinusCos * aXX + aCos);
  theOut.SetValue (0, 1, aOneMinusCos * aXY - aSinZ);
  theOut.SetValue (0, 2, aOneMinusCos * aZX + aSinY);

  theOut.SetValue (1, 0, aOneMinusCos * aXY + aSinZ);
  theOut.SetValue (1, 1, aOneMinusCos * aYY + aCos);
  theOut.SetValue (1, 2, aOneMinusCos * aYZ - aSinX);

  theOut.SetValue (2, 0, aOneMinusCos * aZX - aSinY);
  theOut.SetValue (2, 1, aOneMinusCos * aYZ + aSinX);
  theOut.SetValue (2, 2, aOneMinusCos * aZZ + aCos);
}

// =======================================================================
// function : Ortho
// purpose  : Constructs a 3D orthographic projection matrix
// =======================================================================
template<class T>
void OpenGl_Utils::Ortho (typename MatrixType<T>::Mat4& theOut,
  const T theLeft, const T theRight, const T theBottom, const T theTop, const T theZNear, const T theZFar)
{
  theOut.InitIdentity();

  T* aData = theOut.ChangeData();

  const T anInvDx = static_cast<T> (1.0) / (theRight - theLeft);
  const T anInvDy = static_cast<T> (1.0) / (theTop - theBottom);
  const T anInvDz = static_cast<T> (1.0) / (theZFar - theZNear);

  aData[0]  = static_cast<T> ( 2.0) * anInvDx;
  aData[5]  = static_cast<T> ( 2.0) * anInvDy;
  aData[10] = static_cast<T> (-2.0) * anInvDz;

  aData[12] = -(theRight + theLeft) * anInvDx;
  aData[13] = -(theTop + theBottom) * anInvDy;
  aData[14] = -(theZFar + theZNear) * anInvDz;
}

// =======================================================================
// function : Ortho2D
// purpose  : Constructs a 2D orthographic projection matrix
// =======================================================================
template<class T>
void OpenGl_Utils::Ortho2D (typename MatrixType<T>::Mat4& theOut,
  const T theLeft, const T theRight, const T theBottom, const T theTop)
{
  Ortho (theOut, theLeft, theRight, theBottom, theTop, static_cast<T> (-1.0), static_cast<T> (1.0));
}

// =======================================================================
// function : Project
// purpose  : Maps object coordinates to window coordinates
// =======================================================================
template<class T>
static Standard_Boolean OpenGl_Utils::Project (const T                             theObjX,
                                               const T                             theObjY,
                                               const T                             theObjZ,
                                               const typename MatrixType<T>::Mat4& theModViewMat,
                                               const typename MatrixType<T>::Mat4& theProjectMat,
                                               const Standard_Integer              theViewport[4],
                                               T&                                  theWinX,
                                               T&                                  theWinY,
                                               T&                                  theWinZ)
{
  typename VectorType<T>::Vec4 anIn (theObjX, theObjY, theObjZ, static_cast<T> (1.0));

  typename VectorType<T>::Vec4 anOut = theProjectMat * (theModViewMat * anIn);

  if (anOut.w() == static_cast<T> (0.0))
  {
    return Standard_False;
  }

  anOut.w() = static_cast<T> (1.0) / anOut.w();

  anOut.x() *= anOut.w();
  anOut.y() *= anOut.w();
  anOut.z() *= anOut.w();

  // Map x, y and z to range 0-1
  anOut.x() = anOut.x() * static_cast<T> (0.5) + static_cast<T> (0.5);
  anOut.y() = anOut.y() * static_cast<T> (0.5) + static_cast<T> (0.5);
  anOut.z() = anOut.z() * static_cast<T> (0.5) + static_cast<T> (0.5);

  // Map x,y to viewport
  anOut.x() = anOut.x() * theViewport[2] + theViewport[0];
  anOut.y() = anOut.y() * theViewport[3] + theViewport[1];

  theWinX = anOut.x();
  theWinY = anOut.y();
  theWinZ = anOut.z();

  return Standard_True;
}

// =======================================================================
// function : UnProject
// purpose  : Maps window coordinates to object coordinates
// =======================================================================
template<class T>
static Standard_Boolean OpenGl_Utils::UnProject (const T                             theWinX,
                                                 const T                             theWinY,
                                                 const T                             theWinZ,
                                                 const typename MatrixType<T>::Mat4& theModViewMat,
                                                 const typename MatrixType<T>::Mat4& theProjectMat,
                                                 const Standard_Integer              theViewport[4],
                                                 T&                                  theObjX,
                                                 T&                                  theObjY,
                                                 T&                                  theObjZ)
{
  typename MatrixType<T>::Mat4 anUnviewMat;

  if (!(theProjectMat * theModViewMat).Inverted (anUnviewMat))
  {
    return Standard_False;
  }

  typename VectorType<T>::Vec4 anIn (theWinX, theWinY, theWinZ, static_cast<T> (1.0));

  // Map x and y from window coordinates
  anIn.x() = (anIn.x() - theViewport[0]) / theViewport[2];
  anIn.y() = (anIn.y() - theViewport[1]) / theViewport[3];

  // Map to range -1 to 1
  anIn.x() = anIn.x() * static_cast<T> (2.0) - static_cast<T> (1.0);
  anIn.y() = anIn.y() * static_cast<T> (2.0) - static_cast<T> (1.0);
  anIn.z() = anIn.z() * static_cast<T> (2.0) - static_cast<T> (1.0);

  typename VectorType<T>::Vec4 anOut = anUnviewMat * anIn;

  if (anOut.w() == static_cast<T> (0.0))
  {
    return Standard_False;
  }

  anOut.w() = static_cast<T> (1.0) / anOut.w();

  anOut.x() *= anOut.w();
  anOut.y() *= anOut.w();
  anOut.z() *= anOut.w();

  theObjX = anOut.x();
  theObjY = anOut.y();
  theObjZ = anOut.z();

  return Standard_True;
}

#endif // _OpenGl_Utils_H__
