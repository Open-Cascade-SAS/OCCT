// Created by: Peter KURNEV
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

#ifndef _BOPTools_AlgoTools3D_HeaderFile
#define _BOPTools_AlgoTools3D_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class TopoDS_Edge;
class TopoDS_Face;
class gp_Dir;
class Geom_Surface;
class Geom2d_Curve;
class gp_Pnt;
class IntTools_Context;
class gp_Pnt2d;
class TopoDS_Shape;



//! The class contains handy static functions
//! dealing with the topology
//! This is the copy of BOPTools_AlgoTools3D.cdl file
class BOPTools_AlgoTools3D 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Makes the edge <theESplit> seam edge for the face <theFace> basing on the surface properties (U and V periods)
  Standard_EXPORT static Standard_Boolean DoSplitSEAMOnFace (const TopoDS_Edge& theESplit,
                                                             const TopoDS_Face& theFace);

  //! Makes the split edge <theESplit> seam edge for the face <theFace> basing on the positions
  //! of 2d curves of the original edge <theEOrigin>.
  Standard_EXPORT static Standard_Boolean DoSplitSEAMOnFace (const TopoDS_Edge& theEOrigin,
                                                             const TopoDS_Edge& theESplit,
                                                             const TopoDS_Face& theFace);

  //! Computes normal to the face <aF> for the point on the edge <aE>
  //! at parameter <aT>.<br>
  //! <theContext> - storage for caching the geometrical tools
  Standard_EXPORT static void GetNormalToFaceOnEdge (const TopoDS_Edge& aE,
                                                     const TopoDS_Face& aF,
                                                     const Standard_Real aT,
                                                     gp_Dir& aD,
                                                     const Handle(IntTools_Context)& theContext = Handle(IntTools_Context)());
  

  //! Computes normal to the face <aF> for the point on the edge <aE>
  //! at arbitrary intermediate parameter.<br>
  //! <theContext> - storage for caching the geometrical tools
  Standard_EXPORT static void GetNormalToFaceOnEdge (const TopoDS_Edge& aE,
                                                     const TopoDS_Face& aF,
                                                     gp_Dir& aD,
                                                     const Handle(IntTools_Context)& theContext = Handle(IntTools_Context)());
  

  //! Returns 1  if scalar product aNF1* aNF2>0.<br>
  //! Returns 0  if directions aNF1 aNF2 coincide<br>
  //! Returns -1 if scalar product aNF1* aNF2<0.
  Standard_EXPORT static Standard_Integer SenseFlag (const gp_Dir& aNF1,
                                                     const gp_Dir& aNF2);

  //! Compute normal <aD> to surface <aS> in point (U,V)
  //! Returns TRUE if directions aD1U, aD1V coincide
  Standard_EXPORT static Standard_Boolean GetNormalToSurface (const Handle(Geom_Surface)& aS,
                                                              const Standard_Real U,
                                                              const Standard_Real V,
                                                              gp_Dir& aD);

  //! Computes normal to the face <aF> for the 3D-point that
  //! belongs to the edge <aE> at parameter <aT>.<br>
  //! Output:<br>
  //! aPx  -  the 3D-point where the normal computed<br>
  //! aD   -  the normal;<br>
  //! Warning:<br>
  //! The normal is computed not exactly in the point on the
  //! edge, but in point that is near to the edge towards to
  //! the face material (so, we'll have approx. normal);<br>
  //! The point is computed using PointNearEdge function,
  //! with the shifting value BOPTools_AlgoTools3D::MinStepIn2d(),
  //! from the edge, but if this value is too big,
  //! the point will be computed using Hatcher (PointInFace function).<br>
  //! Returns TRUE in case of success.
  Standard_EXPORT static Standard_Boolean GetApproxNormalToFaceOnEdge (const TopoDS_Edge& aE,
                                                                       const TopoDS_Face& aF,
                                                                       const Standard_Real aT,
                                                                       gp_Pnt& aPx,
                                                                       gp_Dir& aD,
                                                                       const Handle(IntTools_Context)& theContext);
  
  //! Computes normal to the face <aF> for the 3D-point that
  //! belongs to the edge <aE> at parameter <aT>.<br>
  //! Output:<br>
  //! aPx  -  the 3D-point where the normal computed<br>
  //! aD   -  the normal;<br>
  //! Warning:<br>
  //! The normal is computed not exactly in the point on the
  //! edge, but in point that is near to the edge towards to
  //! the face material (so, we'll have approx. normal);<br>
  //! The point is computed using PointNearEdge function
  //! with the shifting value <aDt2D> from the edge;<br>
  //! No checks on this value will be done.<br>
  //! Returns TRUE in case of success.
  Standard_EXPORT static Standard_Boolean GetApproxNormalToFaceOnEdge (const TopoDS_Edge& theE,
                                                                       const TopoDS_Face& theF,
                                                                       const Standard_Real aT,
                                                                       gp_Pnt& aP,
                                                                       gp_Dir& aDNF,
                                                                       const Standard_Real aDt2D);
  
  //! Computes normal to the face <aF> for the 3D-point that
  //! belongs to the edge <aE> at parameter <aT>.<br>
  //! Output:<br>
  //! aPx  -  the 3D-point where the normal computed<br>
  //! aD   -  the normal;<br>
  //! Warning:<br>
  //! The normal is computed not exactly in the point on the
  //! edge, but in point that is near to the edge towards to
  //! the face material (so, we'll have approx. normal);<br>
  //! The point is computed using PointNearEdge function
  //! with the shifting value <aDt2D> from the edge,
  //! but if this value is too big the point will be 
  //! computed using Hatcher (PointInFace function).<br>
  //! Returns TRUE in case of success.
  Standard_EXPORT static Standard_Boolean GetApproxNormalToFaceOnEdge (const TopoDS_Edge& theE,
                                                                       const TopoDS_Face& theF,
                                                                       const Standard_Real aT,
                                                                       const Standard_Real aDt2D,
                                                                       gp_Pnt& aP,
                                                                       gp_Dir& aDNF,
                                                                       const Handle(IntTools_Context)& theContext);

  //! Compute the point <aPx>,  (<aP2D>)  that is near to
  //! the edge <aE>   at parameter <aT>  towards to the
  //! material of the face <aF>. The value of shifting in
  //! 2D is <aDt2D><br>
  //! If the value of shifting is too big the point 
  //! will be computed using Hatcher (PointInFace function).<br>
  //! Returns error status:<br>
  //! 0 - in case of success;<br>
  //! 1 - <aE> does not have 2d curve on the face <aF>;<br>
  //! 2 - the computed point is out of the face.
  Standard_EXPORT static Standard_Integer PointNearEdge (const TopoDS_Edge& aE,
                                                         const TopoDS_Face& aF,
                                                         const Standard_Real aT,
                                                         const Standard_Real aDt2D,
                                                         gp_Pnt2d& aP2D,
                                                         gp_Pnt& aPx,
                                                         const Handle(IntTools_Context)& theContext);

  //! Compute the point <aPx>,  (<aP2D>)  that is near to
  //! the edge <aE>   at parameter <aT>  towards to the
  //! material of the face <aF>. The value of shifting in
  //! 2D is <aDt2D>. No checks on this value will be done.<br>
  //! Returns error status:<br>
  //! 0 - in case of success;<br>
  //! 1 - <aE> does not have 2d curve on the face <aF>.
  Standard_EXPORT static Standard_Integer PointNearEdge (const TopoDS_Edge& aE,
                                                         const TopoDS_Face& aF,
                                                         const Standard_Real aT,
                                                         const Standard_Real aDt2D,
                                                         gp_Pnt2d& aP2D,
                                                         gp_Pnt& aPx);

  //! Computes the point <aPx>,  (<aP2D>)  that is near to
  //! the edge <aE>   at parameter <aT>  towards to the
  //! material of the face <aF>. The value of shifting in
  //! 2D is  dt2D=BOPTools_AlgoTools3D::MinStepIn2d()<br>
  //! If the value of shifting is too big the point will be computed
  //! using Hatcher (PointInFace function).<br>
  //! Returns error status:<br>
  //! 0 - in case of success;<br>
  //! 1 - <aE> does not have 2d curve on the face <aF>;<br>
  //! 2 - the computed point is out of the face.
  Standard_EXPORT static Standard_Integer PointNearEdge (const TopoDS_Edge& aE,
                                                         const TopoDS_Face& aF,
                                                         const Standard_Real aT,
                                                         gp_Pnt2d& aP2D,
                                                         gp_Pnt& aPx,
                                                         const Handle(IntTools_Context)& theContext);
  

  //! Compute the point <aPx>,  (<aP2D>)  that is near to
  //! the edge <aE>   at arbitrary  parameter  towards to the
  //! material of the face <aF>. The value of shifting in
  //! 2D is  dt2D=BOPTools_AlgoTools3D::MinStepIn2d().<br>
  //! If the value of shifting is too big the point will be computed
  //! using Hatcher (PointInFace function).<br>
  //! Returns error status:<br>
  //! 0 - in case of success;<br>
  //! 1 - <aE> does not have 2d curve on the face <aF>;<br>
  //! 2 - the computed point is out of the face.
  Standard_EXPORT static Standard_Integer PointNearEdge (const TopoDS_Edge& aE,
                                                         const TopoDS_Face& aF,
                                                         gp_Pnt2d& aP2D,
                                                         gp_Pnt& aPx,
                                                         const Handle(IntTools_Context)& theContext);
  

  //! Returns simple step value that is used in 2D-computations
  //! = 1.e-5
  Standard_EXPORT static Standard_Real MinStepIn2d();
  

  //! Returns TRUE if the shape <aS> does not contain
  //! geometry information  (e.g. empty compound)
  Standard_EXPORT static Standard_Boolean IsEmptyShape (const TopoDS_Shape& aS);
  

  //! Get the edge <aER> from the face <aF> that is the same as
  //! the edge <aE>
  Standard_EXPORT static void OrientEdgeOnFace (const TopoDS_Edge& aE, const TopoDS_Face& aF, TopoDS_Edge& aER);
  
  //! Computes arbitrary point <theP> inside the face <theF>.<br>
  //! <theP2D> -  2D  representation of <theP>
  //! on the surface of <theF><br>
  //! Returns 0 in case of success.
  Standard_EXPORT static Standard_Integer PointInFace (const TopoDS_Face& theF, 
                                                       gp_Pnt& theP, 
                                                       gp_Pnt2d& theP2D, 
                                                       const Handle(IntTools_Context)& theContext);

  //! Computes a point <theP> inside the face <theF> 
  //! using starting point taken by the parameter <theT> 
  //! from the 2d curve of the edge <theE> on the face <theF>
  //! in the direction perpendicular to the tangent vector
  //! of the 2d curve of the edge.<br>
  //! The point will be distanced on <theDt2D> from the 2d curve.
  //! <theP2D> -  2D  representation of <theP>
  //! on the surface of <theF><br>
  //! Returns 0 in case of success.
  Standard_EXPORT static Standard_Integer PointInFace (const TopoDS_Face& theF, 
                                                       const TopoDS_Edge& theE,
                                                       const Standard_Real theT,
                                                       const Standard_Real theDt2D,
                                                       gp_Pnt& theP, 
                                                       gp_Pnt2d& theP2D, 
                                                       const Handle(IntTools_Context)& theContext);

  //! Computes a point <theP> inside the face <theF> 
  //! using the line <theL> so that 2D point
  //! <theP2D>, 2D representation of <theP>
  //! on the surface of <theF>, lies on that line.<br>
  //! Returns 0 in case of success.
  Standard_EXPORT static Standard_Integer PointInFace (const TopoDS_Face& theF, 
                                                       const Handle(Geom2d_Curve)& theL,
                                                       gp_Pnt& theP, 
                                                       gp_Pnt2d& theP2D, 
                                                       const Handle(IntTools_Context)& theContext,
                                                       const Standard_Real theDt2D = 0.0);


protected:

private:

};

#endif // _BOPTools_AlgoTools3D_HeaderFile
