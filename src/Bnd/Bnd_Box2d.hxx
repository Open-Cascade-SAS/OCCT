// Created on: 1991-01-28
// Created by: Remi Lequette
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _Bnd_Box2d_HeaderFile
#define _Bnd_Box2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class gp_Pnt2d;
class gp_Dir2d;
class gp_Trsf2d;


//! Describes a bounding box in 2D space.
//! A bounding box is parallel to the axes of the coordinates
//! system. If it is finite, it is defined by the two intervals:
//! -   [ Xmin,Xmax ], and
//! -   [ Ymin,Ymax ].
//! A bounding box may be infinite (i.e. open) in one or more
//! directions. It is said to be:
//! -   OpenXmin if it is infinite on the negative side of the   "X Direction";
//! -   OpenXmax if it is infinite on the positive side of the   "X Direction";
//! -   OpenYmin if it is infinite on the negative side of the   "Y Direction";
//! -   OpenYmax if it is infinite on the positive side of the   "Y Direction";
//! -   WholeSpace if it is infinite in all four directions. In
//! this case, any point of the space is inside the box;
//! -   Void if it is empty. In this case, there is no point included in the box.
//! A bounding box is defined by four bounds (Xmin, Xmax, Ymin and Ymax) which
//! limit the bounding box if it is finite, six flags (OpenXmin, OpenXmax, OpenYmin,
//! OpenYmax, WholeSpace and Void) which describe the bounding box if it is infinite or empty, and
//! -   a gap, which is included on both sides in any direction when consulting the finite bounds of the box.
class Bnd_Box2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty 2D bounding box.
  //! The constructed box is qualified Void. Its gap is null.
    Bnd_Box2d();
  
  //! Sets this bounding box so that it covers the whole 2D
  //! space, i.e. it is infinite in all directions.
    void SetWhole();
  
  //! Sets this 2D bounding box so that it is empty. All points are outside a void box.
    void SetVoid();
  
  //! Sets this 2D bounding box so that it bounds
  //! the point P. This involves first setting this bounding box
  //! to be void and then adding the point PThe rectangle bounds   the  point <P>.
    void Set (const gp_Pnt2d& P);
  
  //! Sets this 2D bounding box so that it bounds
  //! the half-line defined by point P and direction D, i.e. all
  //! points M defined by M=P+u*D, where u is greater than
  //! or equal to 0, are inside the bounding area. This involves
  //! first setting this 2D box to be void and then adding the   half-line.
    void Set (const gp_Pnt2d& P, const gp_Dir2d& D);
  
  //! Enlarges this 2D bounding box, if required, so that it
  //! contains at least:
  //! -   interval [ aXmin,aXmax ] in the "X Direction",
  //! -   interval [ aYmin,aYmax ] in the "Y Direction"
  Standard_EXPORT void Update (const Standard_Real aXmin, const Standard_Real aYmin, const Standard_Real aXmax, const Standard_Real aYmax);
  
  //! Adds a point of coordinates (X,Y) to this bounding box.
  Standard_EXPORT void Update (const Standard_Real X, const Standard_Real Y);
  
  //! Returns the gap of this 2D bounding box.
    Standard_Real GetGap() const;
  
  //! Set the gap of this 2D bounding box to abs(Tol).
    void SetGap (const Standard_Real Tol);
  
  //! Enlarges     the  box  with    a  tolerance  value.
  //! This means that the minimum values of its X and Y
  //! intervals of definition, when they are finite, are reduced by
  //! the absolute value of Tol, while the maximum values are
  //! increased by the same amount.
    void Enlarge (const Standard_Real Tol);
  
  //! Returns the bounds of this 2D bounding box.
  //! The gap is included. If this bounding box is infinite (i.e. "open"), returned values
  //! may be equal to +/- Precision::Infinite().
  //! if IsVoid()
  Standard_EXPORT void Get (Standard_Real& aXmin, Standard_Real& aYmin, Standard_Real& aXmax, Standard_Real& aYmax) const;
  
  //! The Box will be infinitely long in the Xmin direction.
    void OpenXmin();
  
  //! The Box will be infinitely long in the Xmax direction.
    void OpenXmax();
  
  //! The Box will be infinitely long in the Ymin direction.
    void OpenYmin();
  
  //! The Box will be infinitely long in the Ymax direction.
    void OpenYmax();
  
  //! Returns true if this bounding box is open in the Xmin direction.
    Standard_Boolean IsOpenXmin() const;
  
  //! Returns true if this bounding box is open in the Xmax direction.
    Standard_Boolean IsOpenXmax() const;
  
  //! Returns true if this bounding box is open in the Ymin direction.
    Standard_Boolean IsOpenYmin() const;
  
  //! Returns true if this bounding box is open in the Ymax direction.
    Standard_Boolean IsOpenYmax() const;
  

  //! Returns true if this bounding box is infinite in all 4
  //! directions (Whole Space flag).
    Standard_Boolean IsWhole() const;
  

  //! Returns true if this 2D bounding box is empty (Void flag).
    Standard_Boolean IsVoid() const;
  
  //! Returns a bounding box which is the result of applying the
  //! transformation T to this bounding box.
  //! Warning
  //! Applying a geometric transformation (for example, a
  //! rotation) to a bounding box generally increases its
  //! dimensions. This is not optimal for algorithms which use it.
  Standard_EXPORT Bnd_Box2d Transformed (const gp_Trsf2d& T) const;
  
  //! Adds the 2d box <Other> to <me>.
  Standard_EXPORT void Add (const Bnd_Box2d& Other);
  
  //! Adds  the 2d pnt <P> to <me>.
    void Add (const gp_Pnt2d& P);
  
  //! Extends <me> from the Pnt <P> in the direction <D>.
    void Add (const gp_Pnt2d& P, const gp_Dir2d& D);
  
  //! Extends the Box  in the given Direction, i.e. adds
  //! a half-line. The box may become infinite in 1 or 2
  //! directions.
  Standard_EXPORT void Add (const gp_Dir2d& D);
  
  //! Returns True if the 2d pnt <P> is out <me>.
  Standard_EXPORT Standard_Boolean IsOut (const gp_Pnt2d& P) const;
  
  //! Returns True if <Box2d> is out <me>.
  Standard_EXPORT Standard_Boolean IsOut (const Bnd_Box2d& Other) const;
  
  //! Returns True if transformed <Box2d> is out <me>.
    Standard_Boolean IsOut (const Bnd_Box2d& Other, const gp_Trsf2d& T) const;
  
  //! Compares  a transformed  bounding with  a    transformed
  //! bounding. The default implementation is  to make a copy
  //! of <me> and <Other>, to transform them and to test.
    Standard_Boolean IsOut (const gp_Trsf2d& T1, const Bnd_Box2d& Other, const gp_Trsf2d& T2) const;
  
  Standard_EXPORT void Dump() const;
  
  //! Computes the squared diagonal of me.
    Standard_Real SquareExtent() const;




protected:





private:



  Standard_Real Xmin;
  Standard_Real Xmax;
  Standard_Real Ymin;
  Standard_Real Ymax;
  Standard_Real Gap;
  Standard_Integer Flags;


};


#include <Bnd_Box2d.lxx>





#endif // _Bnd_Box2d_HeaderFile
