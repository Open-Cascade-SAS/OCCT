// Created on: 1993-11-09
// Created by: Laurent BOURESCHE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _ChFi3d_HeaderFile
#define _ChFi3d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Boolean.hxx>
class BRepAdaptor_Surface;
class TopoDS_Edge;
class ChFi3d_Builder;
class ChFi3d_ChBuilder;
class ChFi3d_FilBuilder;
class ChFi3d_SearchSing;


//! creation of spatial fillets on a solid.
class ChFi3d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns  Reversed  in  Or1  and(or)  Or2  if
  //! the  concave edge  defined by the  interior of faces F1 and F2,
  //! in  the  neighbourhood of  their boundary E is of the edge opposite to  the
  //! normal  of their surface  support.  The  orientation of
  //! faces is  not  taken  into  consideration in  the calculation. The
  //! function  returns  0 if  the calculation fails (tangence),
  //! if  not, it  returns the  number of  choice of  the fillet
  //! or chamfer corresponding to  the orientations  calculated
  //! and  to  the tangent to  the  guide line read in  E.
  Standard_EXPORT static Standard_Integer ConcaveSide (const BRepAdaptor_Surface& S1, const BRepAdaptor_Surface& S2, const TopoDS_Edge& E, TopAbs_Orientation& Or1, TopAbs_Orientation& Or2);
  
  //! Same  as ConcaveSide, but the orientations are
  //! logically  deduced from  the result of  the call of
  //! ConcaveSide on  the  first pair of faces of  the fillet or
  //! chamnfer.
  Standard_EXPORT static Standard_Integer NextSide (TopAbs_Orientation& Or1, TopAbs_Orientation& Or2, const TopAbs_Orientation OrSave1, const TopAbs_Orientation OrSave2, const Standard_Integer ChoixSauv);
  
  //! Same  as  the  other NextSide, but the calculation is  done
  //! on an edge  only.
  Standard_EXPORT static void NextSide (TopAbs_Orientation& Or, const TopAbs_Orientation OrSave, const TopAbs_Orientation OrFace);
  
  //! Enables  to  determine while  processing  an  angle, if
  //! two fillets or chamfers constituting a face have
  //! identic or opposed  concave  edges.
  Standard_EXPORT static Standard_Boolean SameSide (const TopAbs_Orientation Or, const TopAbs_Orientation OrSave1, const TopAbs_Orientation OrSave2, const TopAbs_Orientation OrFace1, const TopAbs_Orientation OrFace2);




protected:





private:




friend class ChFi3d_Builder;
friend class ChFi3d_ChBuilder;
friend class ChFi3d_FilBuilder;
friend class ChFi3d_SearchSing;

};







#endif // _ChFi3d_HeaderFile
