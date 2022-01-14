// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntPatch_Line_HeaderFile
#define _IntPatch_Line_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IntPatch_IType.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <IntSurf_Situation.hxx>
#include <Standard_Transient.hxx>


class IntPatch_Line;
DEFINE_STANDARD_HANDLE(IntPatch_Line, Standard_Transient)

//! Definition of an intersection line between two
//! surfaces.
//! A line may be either geometric : line, circle, ellipse,
//! parabola, hyperbola, as defined in the class GLine,
//! or analytic, as defined in the class ALine, or defined
//! by a set of points (coming from a walking algorithm) as
//! defined in the class WLine.
class IntPatch_Line : public Standard_Transient
{

public:

  
  //! To set the values returned by IsUIsoS1,....
  //! The default values are False.
    void SetValue (const Standard_Boolean Uiso1, const Standard_Boolean Viso1, const Standard_Boolean Uiso2, const Standard_Boolean Viso2);
  
  //! Returns the type of geometry 3d (Line, Circle, Parabola,
  //! Hyperbola, Ellipse, Analytic, Walking, Restriction)
    IntPatch_IType ArcType() const;
  
  //! Returns TRUE if the intersection is a line of tangency
  //! between the 2 patches.
    Standard_Boolean IsTangent() const;
  
  //! Returns the type of the transition of the line
  //! for the first surface. The transition is "constant"
  //! along the line.
  //! The transition is IN if the line is oriented in such
  //! a way that the system of vector (N1,N2,T) is right-handed,
  //! where N1 is the normal to the first surface at a point P,
  //! N2 is the normal to the second surface at a point P,
  //! T  is the tangent to the intersection line at P.
  //! If the system of vector is left-handed, the transition
  //! is OUT.
  //! When N1 and N2 are colinear all along the intersection
  //! line, the transition will be
  //! - TOUCH, if it is possible to use the 2nd derivatives
  //! to determine the position of one surafce compared
  //! to the other (see Situation)
  //! - UNDECIDED otherwise.
  //!
  //! If one of the transition is TOUCH or UNDECIDED, the other
  //! one has got the same value.
    IntSurf_TypeTrans TransitionOnS1() const;
  
  //! Returns the type of the transition of the line
  //! for the second surface. The transition is "constant"
  //! along the line.
    IntSurf_TypeTrans TransitionOnS2() const;
  
  //! Returns the situation (INSIDE/OUTSIDE/UNKNOWN) of
  //! the first patch compared to the second one, when
  //! TransitionOnS1 or TransitionOnS2 returns TOUCH.
  //! Otherwise, an exception is raised.
    IntSurf_Situation SituationS1() const;
  
  //! Returns the situation (INSIDE/OUTSIDE/UNKNOWN) of
  //! the second patch compared to the first one, when
  //! TransitionOnS1 or TransitionOnS2 returns TOUCH.
  //! Otherwise, an exception is raised.
    IntSurf_Situation SituationS2() const;
  
  //! Returns TRUE if the intersection is a U isoparametric curve
  //! on the first patch.
    Standard_Boolean IsUIsoOnS1() const;
  
  //! Returns TRUE if the intersection is a V isoparametric curve
  //! on the first patch.
    Standard_Boolean IsVIsoOnS1() const;
  
  //! Returns TRUE if the intersection is a U isoparametric curve
  //! on the second patch.
    Standard_Boolean IsUIsoOnS2() const;
  
  //! Returns TRUE if the intersection is a V isoparametric curve
  //! on the second patch.
    Standard_Boolean IsVIsoOnS2() const;




  DEFINE_STANDARD_RTTIEXT(IntPatch_Line,Standard_Transient)

protected:

  
  //! To initialize the fields, when the transitions
  //! are In or Out.
  Standard_EXPORT IntPatch_Line(const Standard_Boolean Tang, const IntSurf_TypeTrans Trans1, const IntSurf_TypeTrans Trans2);
  
  //! To initialize the fields, when the transitions
  //! are Touch.
  Standard_EXPORT IntPatch_Line(const Standard_Boolean Tang, const IntSurf_Situation Situ1, const IntSurf_Situation Situ2);
  
  //! To initialize the fields, when the transitions
  //! are Undecided.
  Standard_EXPORT IntPatch_Line(const Standard_Boolean Tang);

  IntPatch_IType typ;


private:


  Standard_Boolean tg;
  IntSurf_TypeTrans tS1;
  IntSurf_TypeTrans tS2;
  IntSurf_Situation sit1;
  IntSurf_Situation sit2;
  Standard_Boolean uS1;
  Standard_Boolean vS1;
  Standard_Boolean uS2;
  Standard_Boolean vS2;


};


#include <IntPatch_Line.lxx>





#endif // _IntPatch_Line_HeaderFile
