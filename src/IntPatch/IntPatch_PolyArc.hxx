// Created on: 1993-01-27
// Created by: Isabelle GRIGNON
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

#ifndef _IntPatch_PolyArc_HeaderFile
#define _IntPatch_PolyArc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <IntPatch_Polygo.hxx>
#include <Standard_Integer.hxx>
class Bnd_Box2d;
class gp_Pnt2d;



class IntPatch_PolyArc  : public IntPatch_Polygo
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates the polygon of the arc A on the surface S.
  //! The arc is limited by the parameters Pfirst and Plast.
  //! None of these parameters can be infinite.
  Standard_EXPORT IntPatch_PolyArc(const Handle(Adaptor2d_Curve2d)& A, const Standard_Integer NbSample, const Standard_Real Pfirst, const Standard_Real Plast, const Bnd_Box2d& BoxOtherPolygon);
  
  Standard_EXPORT virtual Standard_Boolean Closed() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbPoints() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Pnt2d Point (const Standard_Integer Index) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real Parameter (const Standard_Integer Index) const;
  
  Standard_EXPORT void SetOffset (const Standard_Real OffsetX, const Standard_Real OffsetY);




protected:





private:



  TColgp_Array1OfPnt2d brise;
  TColStd_Array1OfReal param;
  Standard_Real offsetx;
  Standard_Real offsety;
  Standard_Boolean ferme;


};







#endif // _IntPatch_PolyArc_HeaderFile
