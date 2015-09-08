// Created on: 1995-05-05
// Created by: Christophe MARION
// Copyright (c) 1995-1999 Matra Datavision
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

// Modified by cma, Tue Apr  1 11:39:48 1997
// Modified by cma, Tue Apr  1 11:40:30 1997

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <CSLib.hxx>
#include <CSLib_DerivativeStatus.hxx>
#include <CSLib_NormalStatus.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp.hxx>
#include <HLRAlgo_BiPoint.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRAlgo_ListIteratorOfListOfBPoint.hxx>
#include <HLRAlgo_PolyAlgo.hxx>
#include <HLRAlgo_PolyData.hxx>
#include <HLRAlgo_PolyInternalData.hxx>
#include <HLRAlgo_PolyShellData.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#define EMskOutLin1  ((Standard_Boolean)   1)
#define EMskOutLin2  ((Standard_Boolean)   2)
#define EMskOutLin3  ((Standard_Boolean)   4)
#define EMskGrALin1  ((Standard_Boolean)   8)
#define EMskGrALin2  ((Standard_Boolean)  16)
#define EMskGrALin3  ((Standard_Boolean)  32)
#define FMskBack     ((Standard_Boolean)  64)
#define FMskSide     ((Standard_Boolean) 128)
#define FMskHiding   ((Standard_Boolean) 256)
#define FMskFlat     ((Standard_Boolean) 512)
#define FMskOnOutL   ((Standard_Boolean)1024)
#define FMskOrBack   ((Standard_Boolean)2048)
#define FMskFrBack   ((Standard_Boolean)4096)

#define NMskVert ((Standard_Boolean) 1)
#define NMskOutL ((Standard_Boolean) 2)
#define NMskNorm ((Standard_Boolean) 4)
#define NMskFuck ((Standard_Boolean) 8)
#define NMskEdge ((Standard_Boolean)16)
#define NMskMove ((Standard_Boolean)32)

#define PntXTI1 ((Standard_Real*)Coordinates)[ 0]
#define PntYTI1 ((Standard_Real*)Coordinates)[ 1]
#define PntZTI1 ((Standard_Real*)Coordinates)[ 2]
#define PntXTI2 ((Standard_Real*)Coordinates)[ 3]
#define PntYTI2 ((Standard_Real*)Coordinates)[ 4]
#define PntZTI2 ((Standard_Real*)Coordinates)[ 5]
#define PntX1   ((Standard_Real*)Coordinates)[ 6]
#define PntY1   ((Standard_Real*)Coordinates)[ 7]
#define PntZ1   ((Standard_Real*)Coordinates)[ 8]
#define PntX2   ((Standard_Real*)Coordinates)[ 9]
#define PntY2   ((Standard_Real*)Coordinates)[10]
#define PntZ2   ((Standard_Real*)Coordinates)[11]

#define Pn2XTI1 ((Standard_Real*)Coordinate2)[ 0]
#define Pn2YTI1 ((Standard_Real*)Coordinate2)[ 1]
#define Pn2ZTI1 ((Standard_Real*)Coordinate2)[ 2]
#define Pn2XTI2 ((Standard_Real*)Coordinate2)[ 3]
#define Pn2YTI2 ((Standard_Real*)Coordinate2)[ 4]
#define Pn2ZTI2 ((Standard_Real*)Coordinate2)[ 5]
#define Pn2X1   ((Standard_Real*)Coordinate2)[ 6]
#define Pn2Y1   ((Standard_Real*)Coordinate2)[ 7]
#define Pn2Z1   ((Standard_Real*)Coordinate2)[ 8]
#define Pn2X2   ((Standard_Real*)Coordinate2)[ 9]
#define Pn2Y2   ((Standard_Real*)Coordinate2)[10]
#define Pn2Z2   ((Standard_Real*)Coordinate2)[11]

#define Tri1Node1 ((Standard_Integer*)Tri1Indices)[0]
#define Tri1Node2 ((Standard_Integer*)Tri1Indices)[1]
#define Tri1Node3 ((Standard_Integer*)Tri1Indices)[2]
#define Tri1Flags ((Standard_Boolean*)Tri1Indices)[3]

#define Tri2Node1 ((Standard_Integer*)Tri2Indices)[0]
#define Tri2Node2 ((Standard_Integer*)Tri2Indices)[1]
#define Tri2Node3 ((Standard_Integer*)Tri2Indices)[2]
#define Tri2Flags ((Standard_Boolean*)Tri2Indices)[3]

#define Tri3Node1 ((Standard_Integer*)Tri3Indices)[0]
#define Tri3Node2 ((Standard_Integer*)Tri3Indices)[1]
#define Tri3Node3 ((Standard_Integer*)Tri3Indices)[2]
#define Tri3Flags ((Standard_Boolean*)Tri3Indices)[3]

#define Seg1LstSg1 ((Standard_Integer*)Seg1Indices)[0]
#define Seg1LstSg2 ((Standard_Integer*)Seg1Indices)[1]
#define Seg1NxtSg1 ((Standard_Integer*)Seg1Indices)[2]
#define Seg1NxtSg2 ((Standard_Integer*)Seg1Indices)[3]
#define Seg1Conex1 ((Standard_Integer*)Seg1Indices)[4]
#define Seg1Conex2 ((Standard_Integer*)Seg1Indices)[5]

#define Seg2LstSg1 ((Standard_Integer*)Seg2Indices)[0]
#define Seg2LstSg2 ((Standard_Integer*)Seg2Indices)[1]
#define Seg2NxtSg1 ((Standard_Integer*)Seg2Indices)[2]
#define Seg2NxtSg2 ((Standard_Integer*)Seg2Indices)[3]
#define Seg2Conex1 ((Standard_Integer*)Seg2Indices)[4]
#define Seg2Conex2 ((Standard_Integer*)Seg2Indices)[5]

#define Nod1NdSg ((Standard_Integer*)Nod1Indices)[0]
#define Nod1Flag ((Standard_Boolean*)Nod1Indices)[1]
#define Nod1Edg1 ((Standard_Boolean*)Nod1Indices)[2]
#define Nod1Edg2 ((Standard_Boolean*)Nod1Indices)[3]

#define Nod1PntX ((Standard_Real*)Nod1RValues)[ 0]
#define Nod1PntY ((Standard_Real*)Nod1RValues)[ 1]
#define Nod1PntZ ((Standard_Real*)Nod1RValues)[ 2]
#define Nod1PntU ((Standard_Real*)Nod1RValues)[ 3]
#define Nod1PntV ((Standard_Real*)Nod1RValues)[ 4]
#define Nod1NrmX ((Standard_Real*)Nod1RValues)[ 5]
#define Nod1NrmY ((Standard_Real*)Nod1RValues)[ 6]
#define Nod1NrmZ ((Standard_Real*)Nod1RValues)[ 7]
#define Nod1PCu1 ((Standard_Real*)Nod1RValues)[ 8]
#define Nod1PCu2 ((Standard_Real*)Nod1RValues)[ 9]
#define Nod1Scal ((Standard_Real*)Nod1RValues)[10]

#define NodANdSg ((Standard_Integer*)NodAIndices)[0]
#define NodAFlag ((Standard_Boolean*)NodAIndices)[1]
#define NodAEdg1 ((Standard_Boolean*)NodAIndices)[2]
#define NodAEdg2 ((Standard_Boolean*)NodAIndices)[3]

#define NodAPntX ((Standard_Real*)NodARValues)[ 0]
#define NodAPntY ((Standard_Real*)NodARValues)[ 1]
#define NodAPntZ ((Standard_Real*)NodARValues)[ 2]
#define NodAPntU ((Standard_Real*)NodARValues)[ 3]
#define NodAPntV ((Standard_Real*)NodARValues)[ 4]
#define NodANrmX ((Standard_Real*)NodARValues)[ 5]
#define NodANrmY ((Standard_Real*)NodARValues)[ 6]
#define NodANrmZ ((Standard_Real*)NodARValues)[ 7]
#define NodAPCu1 ((Standard_Real*)NodARValues)[ 8]
#define NodAPCu2 ((Standard_Real*)NodARValues)[ 9]
#define NodAScal ((Standard_Real*)NodARValues)[10]

#define NodBNdSg ((Standard_Integer*)NodBIndices)[0]
#define NodBFlag ((Standard_Boolean*)NodBIndices)[1]
#define NodBEdg1 ((Standard_Boolean*)NodBIndices)[2]
#define NodBEdg2 ((Standard_Boolean*)NodBIndices)[3]

#define NodBPntX ((Standard_Real*)NodBRValues)[ 0]
#define NodBPntY ((Standard_Real*)NodBRValues)[ 1]
#define NodBPntZ ((Standard_Real*)NodBRValues)[ 2]
#define NodBPntU ((Standard_Real*)NodBRValues)[ 3]
#define NodBPntV ((Standard_Real*)NodBRValues)[ 4]
#define NodBNrmX ((Standard_Real*)NodBRValues)[ 5]
#define NodBNrmY ((Standard_Real*)NodBRValues)[ 6]
#define NodBNrmZ ((Standard_Real*)NodBRValues)[ 7]
#define NodBPCu1 ((Standard_Real*)NodBRValues)[ 8]
#define NodBPCu2 ((Standard_Real*)NodBRValues)[ 9]
#define NodBScal ((Standard_Real*)NodBRValues)[10]

#define Nod2NdSg ((Standard_Integer*)Nod2Indices)[0]
#define Nod2Flag ((Standard_Boolean*)Nod2Indices)[1]
#define Nod2Edg1 ((Standard_Boolean*)Nod2Indices)[2]
#define Nod2Edg2 ((Standard_Boolean*)Nod2Indices)[3]

#define Nod2PntX ((Standard_Real*)Nod2RValues)[ 0]
#define Nod2PntY ((Standard_Real*)Nod2RValues)[ 1]
#define Nod2PntZ ((Standard_Real*)Nod2RValues)[ 2]
#define Nod2PntU ((Standard_Real*)Nod2RValues)[ 3]
#define Nod2PntV ((Standard_Real*)Nod2RValues)[ 4]
#define Nod2NrmX ((Standard_Real*)Nod2RValues)[ 5]
#define Nod2NrmY ((Standard_Real*)Nod2RValues)[ 6]
#define Nod2NrmZ ((Standard_Real*)Nod2RValues)[ 7]
#define Nod2PCu1 ((Standard_Real*)Nod2RValues)[ 8]
#define Nod2PCu2 ((Standard_Real*)Nod2RValues)[ 9]
#define Nod2Scal ((Standard_Real*)Nod2RValues)[10]

#define Nod3NdSg ((Standard_Integer*)Nod3Indices)[0]
#define Nod3Flag ((Standard_Boolean*)Nod3Indices)[1]
#define Nod3Edg1 ((Standard_Boolean*)Nod3Indices)[2]
#define Nod3Edg2 ((Standard_Boolean*)Nod3Indices)[3]

#define Nod3PntX ((Standard_Real*)Nod3RValues)[ 0]
#define Nod3PntY ((Standard_Real*)Nod3RValues)[ 1]
#define Nod3PntZ ((Standard_Real*)Nod3RValues)[ 2]
#define Nod3PntU ((Standard_Real*)Nod3RValues)[ 3]
#define Nod3PntV ((Standard_Real*)Nod3RValues)[ 4]
#define Nod3NrmX ((Standard_Real*)Nod3RValues)[ 5]
#define Nod3NrmY ((Standard_Real*)Nod3RValues)[ 6]
#define Nod3NrmZ ((Standard_Real*)Nod3RValues)[ 7]
#define Nod3PCu1 ((Standard_Real*)Nod3RValues)[ 8]
#define Nod3PCu2 ((Standard_Real*)Nod3RValues)[ 9]
#define Nod3Scal ((Standard_Real*)Nod3RValues)[10]

#define Nod4NdSg ((Standard_Integer*)Nod4Indices)[0]
#define Nod4Flag ((Standard_Boolean*)Nod4Indices)[1]
#define Nod4Edg1 ((Standard_Boolean*)Nod4Indices)[2]
#define Nod4Edg2 ((Standard_Boolean*)Nod4Indices)[3]

#define Nod4PntX ((Standard_Real*)Nod4RValues)[ 0]
#define Nod4PntY ((Standard_Real*)Nod4RValues)[ 1]
#define Nod4PntZ ((Standard_Real*)Nod4RValues)[ 2]
#define Nod4PntU ((Standard_Real*)Nod4RValues)[ 3]
#define Nod4PntV ((Standard_Real*)Nod4RValues)[ 4]
#define Nod4NrmX ((Standard_Real*)Nod4RValues)[ 5]
#define Nod4NrmY ((Standard_Real*)Nod4RValues)[ 6]
#define Nod4NrmZ ((Standard_Real*)Nod4RValues)[ 7]
#define Nod4PCu1 ((Standard_Real*)Nod4RValues)[ 8]
#define Nod4PCu2 ((Standard_Real*)Nod4RValues)[ 9]
#define Nod4Scal ((Standard_Real*)Nod4RValues)[10]

#define Nod11NdSg ((Standard_Integer*)Nod11Indices)[0]
#define Nod11Flag ((Standard_Boolean*)Nod11Indices)[1]
#define Nod11Edg1 ((Standard_Boolean*)Nod11Indices)[2]
#define Nod11Edg2 ((Standard_Boolean*)Nod11Indices)[3]

#define Nod11PntX ((Standard_Real*)Nod11RValues)[ 0]
#define Nod11PntY ((Standard_Real*)Nod11RValues)[ 1]
#define Nod11PntZ ((Standard_Real*)Nod11RValues)[ 2]
#define Nod11PntU ((Standard_Real*)Nod11RValues)[ 3]
#define Nod11PntV ((Standard_Real*)Nod11RValues)[ 4]
#define Nod11NrmX ((Standard_Real*)Nod11RValues)[ 5]
#define Nod11NrmY ((Standard_Real*)Nod11RValues)[ 6]
#define Nod11NrmZ ((Standard_Real*)Nod11RValues)[ 7]
#define Nod11PCu1 ((Standard_Real*)Nod11RValues)[ 8]
#define Nod11PCu2 ((Standard_Real*)Nod11RValues)[ 9]
#define Nod11Scal ((Standard_Real*)Nod11RValues)[10]

#define Nod1ANdSg ((Standard_Integer*)Nod1AIndices)[0]
#define Nod1AFlag ((Standard_Boolean*)Nod1AIndices)[1]
#define Nod1AEdg1 ((Standard_Boolean*)Nod1AIndices)[2]
#define Nod1AEdg2 ((Standard_Boolean*)Nod1AIndices)[3]

#define Nod1APntX ((Standard_Real*)Nod1ARValues)[ 0]
#define Nod1APntY ((Standard_Real*)Nod1ARValues)[ 1]
#define Nod1APntZ ((Standard_Real*)Nod1ARValues)[ 2]
#define Nod1APntU ((Standard_Real*)Nod1ARValues)[ 3]
#define Nod1APntV ((Standard_Real*)Nod1ARValues)[ 4]
#define Nod1ANrmX ((Standard_Real*)Nod1ARValues)[ 5]
#define Nod1ANrmY ((Standard_Real*)Nod1ARValues)[ 6]
#define Nod1ANrmZ ((Standard_Real*)Nod1ARValues)[ 7]
#define Nod1APCu1 ((Standard_Real*)Nod1ARValues)[ 8]
#define Nod1APCu2 ((Standard_Real*)Nod1ARValues)[ 9]
#define Nod1AScal ((Standard_Real*)Nod1ARValues)[10]

#define Nod1BNdSg ((Standard_Integer*)Nod1BIndices)[0]
#define Nod1BFlag ((Standard_Boolean*)Nod1BIndices)[1]
#define Nod1BEdg1 ((Standard_Boolean*)Nod1BIndices)[2]
#define Nod1BEdg2 ((Standard_Boolean*)Nod1BIndices)[3]

#define Nod1BPntX ((Standard_Real*)Nod1BRValues)[ 0]
#define Nod1BPntY ((Standard_Real*)Nod1BRValues)[ 1]
#define Nod1BPntZ ((Standard_Real*)Nod1BRValues)[ 2]
#define Nod1BPntU ((Standard_Real*)Nod1BRValues)[ 3]
#define Nod1BPntV ((Standard_Real*)Nod1BRValues)[ 4]
#define Nod1BNrmX ((Standard_Real*)Nod1BRValues)[ 5]
#define Nod1BNrmY ((Standard_Real*)Nod1BRValues)[ 6]
#define Nod1BNrmZ ((Standard_Real*)Nod1BRValues)[ 7]
#define Nod1BPCu1 ((Standard_Real*)Nod1BRValues)[ 8]
#define Nod1BPCu2 ((Standard_Real*)Nod1BRValues)[ 9]
#define Nod1BScal ((Standard_Real*)Nod1BRValues)[10]

#define Nod12NdSg ((Standard_Integer*)Nod12Indices)[0]
#define Nod12Flag ((Standard_Boolean*)Nod12Indices)[1]
#define Nod12Edg1 ((Standard_Boolean*)Nod12Indices)[2]
#define Nod12Edg2 ((Standard_Boolean*)Nod12Indices)[3]

#define Nod12PntX ((Standard_Real*)Nod12RValues)[ 0]
#define Nod12PntY ((Standard_Real*)Nod12RValues)[ 1]
#define Nod12PntZ ((Standard_Real*)Nod12RValues)[ 2]
#define Nod12PntU ((Standard_Real*)Nod12RValues)[ 3]
#define Nod12PntV ((Standard_Real*)Nod12RValues)[ 4]
#define Nod12NrmX ((Standard_Real*)Nod12RValues)[ 5]
#define Nod12NrmY ((Standard_Real*)Nod12RValues)[ 6]
#define Nod12NrmZ ((Standard_Real*)Nod12RValues)[ 7]
#define Nod12PCu1 ((Standard_Real*)Nod12RValues)[ 8]
#define Nod12PCu2 ((Standard_Real*)Nod12RValues)[ 9]
#define Nod12Scal ((Standard_Real*)Nod12RValues)[10]

#define Nod13NdSg ((Standard_Integer*)Nod13Indices)[0]
#define Nod13Flag ((Standard_Boolean*)Nod13Indices)[1]
#define Nod13Edg1 ((Standard_Boolean*)Nod13Indices)[2]
#define Nod13Edg2 ((Standard_Boolean*)Nod13Indices)[3]

#define Nod13PntX ((Standard_Real*)Nod13RValues)[ 0]
#define Nod13PntY ((Standard_Real*)Nod13RValues)[ 1]
#define Nod13PntZ ((Standard_Real*)Nod13RValues)[ 2]
#define Nod13PntU ((Standard_Real*)Nod13RValues)[ 3]
#define Nod13PntV ((Standard_Real*)Nod13RValues)[ 4]
#define Nod13NrmX ((Standard_Real*)Nod13RValues)[ 5]
#define Nod13NrmY ((Standard_Real*)Nod13RValues)[ 6]
#define Nod13NrmZ ((Standard_Real*)Nod13RValues)[ 7]
#define Nod13PCu1 ((Standard_Real*)Nod13RValues)[ 8]
#define Nod13PCu2 ((Standard_Real*)Nod13RValues)[ 9]
#define Nod13Scal ((Standard_Real*)Nod13RValues)[10]

#define Nod14NdSg ((Standard_Integer*)Nod14Indices)[0]
#define Nod14Flag ((Standard_Boolean*)Nod14Indices)[1]
#define Nod14Edg1 ((Standard_Boolean*)Nod14Indices)[2]
#define Nod14Edg2 ((Standard_Boolean*)Nod14Indices)[3]

#define Nod14PntX ((Standard_Real*)Nod14RValues)[ 0]
#define Nod14PntY ((Standard_Real*)Nod14RValues)[ 1]
#define Nod14PntZ ((Standard_Real*)Nod14RValues)[ 2]
#define Nod14PntU ((Standard_Real*)Nod14RValues)[ 3]
#define Nod14PntV ((Standard_Real*)Nod14RValues)[ 4]
#define Nod14NrmX ((Standard_Real*)Nod14RValues)[ 5]
#define Nod14NrmY ((Standard_Real*)Nod14RValues)[ 6]
#define Nod14NrmZ ((Standard_Real*)Nod14RValues)[ 7]
#define Nod14PCu1 ((Standard_Real*)Nod14RValues)[ 8]
#define Nod14PCu2 ((Standard_Real*)Nod14RValues)[ 9]
#define Nod14Scal ((Standard_Real*)Nod14RValues)[10]

#define Nod21NdSg ((Standard_Integer*)Nod21Indices)[0]
#define Nod21Flag ((Standard_Boolean*)Nod21Indices)[1]
#define Nod21Edg1 ((Standard_Boolean*)Nod21Indices)[2]
#define Nod21Edg2 ((Standard_Boolean*)Nod21Indices)[3]

#define Nod21PntX ((Standard_Real*)Nod21RValues)[ 0]
#define Nod21PntY ((Standard_Real*)Nod21RValues)[ 1]
#define Nod21PntZ ((Standard_Real*)Nod21RValues)[ 2]
#define Nod21PntU ((Standard_Real*)Nod21RValues)[ 3]
#define Nod21PntV ((Standard_Real*)Nod21RValues)[ 4]
#define Nod21NrmX ((Standard_Real*)Nod21RValues)[ 5]
#define Nod21NrmY ((Standard_Real*)Nod21RValues)[ 6]
#define Nod21NrmZ ((Standard_Real*)Nod21RValues)[ 7]
#define Nod21PCu1 ((Standard_Real*)Nod21RValues)[ 8]
#define Nod21PCu2 ((Standard_Real*)Nod21RValues)[ 9]
#define Nod21Scal ((Standard_Real*)Nod21RValues)[10]

#define Nod2ANdSg ((Standard_Integer*)Nod2AIndices)[0]
#define Nod2AFlag ((Standard_Boolean*)Nod2AIndices)[1]
#define Nod2AEdg1 ((Standard_Boolean*)Nod2AIndices)[2]
#define Nod2AEdg2 ((Standard_Boolean*)Nod2AIndices)[3]

#define Nod2APntX ((Standard_Real*)Nod2ARValues)[ 0]
#define Nod2APntY ((Standard_Real*)Nod2ARValues)[ 1]
#define Nod2APntZ ((Standard_Real*)Nod2ARValues)[ 2]
#define Nod2APntU ((Standard_Real*)Nod2ARValues)[ 3]
#define Nod2APntV ((Standard_Real*)Nod2ARValues)[ 4]
#define Nod2ANrmX ((Standard_Real*)Nod2ARValues)[ 5]
#define Nod2ANrmY ((Standard_Real*)Nod2ARValues)[ 6]
#define Nod2ANrmZ ((Standard_Real*)Nod2ARValues)[ 7]
#define Nod2APCu1 ((Standard_Real*)Nod2ARValues)[ 8]
#define Nod2APCu2 ((Standard_Real*)Nod2ARValues)[ 9]
#define Nod2AScal ((Standard_Real*)Nod2ARValues)[10]

#define Nod2BNdSg ((Standard_Integer*)Nod2BIndices)[0]
#define Nod2BFlag ((Standard_Boolean*)Nod2BIndices)[1]
#define Nod2BEdg1 ((Standard_Boolean*)Nod2BIndices)[2]
#define Nod2BEdg2 ((Standard_Boolean*)Nod2BIndices)[3]

#define Nod2BPntX ((Standard_Real*)Nod2BRValues)[ 0]
#define Nod2BPntY ((Standard_Real*)Nod2BRValues)[ 1]
#define Nod2BPntZ ((Standard_Real*)Nod2BRValues)[ 2]
#define Nod2BPntU ((Standard_Real*)Nod2BRValues)[ 3]
#define Nod2BPntV ((Standard_Real*)Nod2BRValues)[ 4]
#define Nod2BNrmX ((Standard_Real*)Nod2BRValues)[ 5]
#define Nod2BNrmY ((Standard_Real*)Nod2BRValues)[ 6]
#define Nod2BNrmZ ((Standard_Real*)Nod2BRValues)[ 7]
#define Nod2BPCu1 ((Standard_Real*)Nod2BRValues)[ 8]
#define Nod2BPCu2 ((Standard_Real*)Nod2BRValues)[ 9]
#define Nod2BScal ((Standard_Real*)Nod2BRValues)[10]

#define Nod22NdSg ((Standard_Integer*)Nod22Indices)[0]
#define Nod22Flag ((Standard_Boolean*)Nod22Indices)[1]
#define Nod22Edg1 ((Standard_Boolean*)Nod22Indices)[2]
#define Nod22Edg2 ((Standard_Boolean*)Nod22Indices)[3]

#define Nod22PntX ((Standard_Real*)Nod22RValues)[ 0]
#define Nod22PntY ((Standard_Real*)Nod22RValues)[ 1]
#define Nod22PntZ ((Standard_Real*)Nod22RValues)[ 2]
#define Nod22PntU ((Standard_Real*)Nod22RValues)[ 3]
#define Nod22PntV ((Standard_Real*)Nod22RValues)[ 4]
#define Nod22NrmX ((Standard_Real*)Nod22RValues)[ 5]
#define Nod22NrmY ((Standard_Real*)Nod22RValues)[ 6]
#define Nod22NrmZ ((Standard_Real*)Nod22RValues)[ 7]
#define Nod22PCu1 ((Standard_Real*)Nod22RValues)[ 8]
#define Nod22PCu2 ((Standard_Real*)Nod22RValues)[ 9]
#define Nod22Scal ((Standard_Real*)Nod22RValues)[10]

#define Nod23NdSg ((Standard_Integer*)Nod23Indices)[0]
#define Nod23Flag ((Standard_Boolean*)Nod23Indices)[1]
#define Nod23Edg1 ((Standard_Boolean*)Nod23Indices)[2]
#define Nod23Edg2 ((Standard_Boolean*)Nod23Indices)[3]

#define Nod23PntX ((Standard_Real*)Nod23RValues)[ 0]
#define Nod23PntY ((Standard_Real*)Nod23RValues)[ 1]
#define Nod23PntZ ((Standard_Real*)Nod23RValues)[ 2]
#define Nod23PntU ((Standard_Real*)Nod23RValues)[ 3]
#define Nod23PntV ((Standard_Real*)Nod23RValues)[ 4]
#define Nod23NrmX ((Standard_Real*)Nod23RValues)[ 5]
#define Nod23NrmY ((Standard_Real*)Nod23RValues)[ 6]
#define Nod23NrmZ ((Standard_Real*)Nod23RValues)[ 7]
#define Nod23PCu1 ((Standard_Real*)Nod23RValues)[ 8]
#define Nod23PCu2 ((Standard_Real*)Nod23RValues)[ 9]
#define Nod23Scal ((Standard_Real*)Nod23RValues)[10]

#define Nod24NdSg ((Standard_Integer*)Nod24Indices)[0]
#define Nod24Flag ((Standard_Boolean*)Nod24Indices)[1]
#define Nod24Edg1 ((Standard_Boolean*)Nod24Indices)[2]
#define Nod24Edg2 ((Standard_Boolean*)Nod24Indices)[3]

#define Nod24PntX ((Standard_Real*)Nod24RValues)[ 0]
#define Nod24PntY ((Standard_Real*)Nod24RValues)[ 1]
#define Nod24PntZ ((Standard_Real*)Nod24RValues)[ 2]
#define Nod24PntU ((Standard_Real*)Nod24RValues)[ 3]
#define Nod24PntV ((Standard_Real*)Nod24RValues)[ 4]
#define Nod24NrmX ((Standard_Real*)Nod24RValues)[ 5]
#define Nod24NrmY ((Standard_Real*)Nod24RValues)[ 6]
#define Nod24NrmZ ((Standard_Real*)Nod24RValues)[ 7]
#define Nod24PCu1 ((Standard_Real*)Nod24RValues)[ 8]
#define Nod24PCu2 ((Standard_Real*)Nod24RValues)[ 9]
#define Nod24Scal ((Standard_Real*)Nod24RValues)[10]

#define ShapeIndex ((Standard_Integer*)IndexPtr)[0]
#define F1Index    ((Standard_Integer*)IndexPtr)[1]
#define F1Pt1Index ((Standard_Integer*)IndexPtr)[2]
#define F1Pt2Index ((Standard_Integer*)IndexPtr)[3]
#define F2Index    ((Standard_Integer*)IndexPtr)[4]
#define F2Pt1Index ((Standard_Integer*)IndexPtr)[5]
#define F2Pt2Index ((Standard_Integer*)IndexPtr)[6]
#define MinSeg     ((Standard_Integer*)IndexPtr)[7]
#define MaxSeg     ((Standard_Integer*)IndexPtr)[8]
#define SegFlags   ((Standard_Integer*)IndexPtr)[9]
#ifdef OCCT_DEBUG
static Standard_Integer DoTrace = Standard_False; 
static Standard_Integer DoError = Standard_False; 
#endif
//=======================================================================
//function : HLRBRep_PolyAlgo
//purpose  : 
//=======================================================================

HLRBRep_PolyAlgo::HLRBRep_PolyAlgo () :
myDebug     (Standard_False),
myAngle     (5 * M_PI / 180.),
myTolSta    (0.1),
myTolEnd    (0.9),
myTolAngular(0.001)
{
  myAlgo = new HLRAlgo_PolyAlgo();
}

//=======================================================================
//function : HLRBRep_PolyAlgo
//purpose  : 
//=======================================================================

HLRBRep_PolyAlgo::HLRBRep_PolyAlgo (const Handle(HLRBRep_PolyAlgo)& A)
{
  myDebug      = A->Debug();
  myAngle      = A->Angle();
  myTolAngular = A->TolAngular();
  myTolSta     = A->TolCoef();
  myTolEnd     = 1 - myTolSta;
  myAlgo       = A->Algo();
  myProj       = A->Projector();

  Standard_Integer n = A->NbShapes();

  for (Standard_Integer i = 1; i <= n; i++)
    Load(A->Shape(i));
}

//=======================================================================
//function : HLRBRep_PolyAlgo
//purpose  : 
//=======================================================================

HLRBRep_PolyAlgo::HLRBRep_PolyAlgo (const TopoDS_Shape& S) :
myDebug     (Standard_False),
myAngle     (5 * M_PI / 180.),
myTolSta    (0.1),
myTolEnd    (0.9),
myTolAngular(0.001)
{
  myShapes.Append(S);
  myAlgo = new HLRAlgo_PolyAlgo();
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape & HLRBRep_PolyAlgo::Shape (const Standard_Integer I)
{
  Standard_OutOfRange_Raise_if (I == 0 || I > myShapes.Length(),
			       "HLRBRep_PolyAlgo::Shape : unknown Shape");
  return myShapes(I);
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::Remove (const Standard_Integer I)
{
  Standard_OutOfRange_Raise_if (I == 0 || I > myShapes.Length(),
				"HLRBRep_PolyAlgo::Remove : unknown Shape");
  myShapes.Remove(I);
  myAlgo->Clear();
  myEMap.Clear();
  myFMap.Clear();
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer HLRBRep_PolyAlgo::Index (const TopoDS_Shape& S) const
{
  Standard_Integer n = myShapes.Length();

  for (Standard_Integer i = 1; i <= n; i++)
    if (myShapes(i) == S) return i;

  return 0;
}

//=======================================================================
//function : Algo
//purpose  : 
//=======================================================================

Handle(HLRAlgo_PolyAlgo) HLRBRep_PolyAlgo::Algo () const
{
  return myAlgo;
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::Update ()
{
  myAlgo->Clear();
  myEMap.Clear();
  myFMap.Clear();
  TopoDS_Shape Shape = MakeShape();

  if (!Shape.IsNull()) {
    TopExp_Explorer exshell;
    Standard_Boolean IsoledF,IsoledE;//,closed;
    TopLoc_Location L;
    TopTools_MapOfShape ShapeMap1,ShapeMap2;
    TopExp::MapShapes(Shape,TopAbs_EDGE,myEMap);
    TopExp::MapShapes(Shape,TopAbs_FACE,myFMap);
    Standard_Integer nbEdge = myEMap.Extent();
    Standard_Integer nbFace = myFMap.Extent();
    TColStd_Array1OfInteger   ES (0,nbEdge); // index of the Shell
    TColStd_Array1OfTransient PD (0,nbFace); // HLRAlgo_PolyData
    TColStd_Array1OfTransient PID(0,nbFace); // PolyInternalData
    Standard_Integer nbShell = InitShape(Shape,IsoledF,IsoledE);
    if (nbShell > 0) {
      TColStd_Array1OfTransient& Shell = myAlgo->PolyShell();
      Standard_Integer iShell = 0;
      
      for (exshell.Init(Shape, TopAbs_SHELL);
	   exshell.More(); 
	   exshell.Next())
	StoreShell(exshell.Current(),iShell,Shell,
		   Standard_False,Standard_False,
		   ES,PD,PID,ShapeMap1,ShapeMap2);
      if (IsoledF)
	StoreShell(Shape,iShell,Shell,IsoledF,Standard_False,
		   ES,PD,PID,ShapeMap1,ShapeMap2);
      if (IsoledE)
	StoreShell(Shape,iShell,Shell,Standard_False,IsoledE,
		   ES,PD,PID,ShapeMap1,ShapeMap2);
      myAlgo->Update();
    }
  }
}

//=======================================================================
//function : MakeShape
//purpose  : 
//=======================================================================

TopoDS_Shape HLRBRep_PolyAlgo::MakeShape () const
{
  Standard_Integer n = myShapes.Length();
  Standard_Boolean FirstTime = Standard_True;
  BRep_Builder B;
  TopoDS_Shape Shape;
  
  for (Standard_Integer i = 1; i <= n; i++) {
    if (FirstTime) {
      FirstTime = Standard_False;
      B.MakeCompound(TopoDS::Compound(Shape));
    }
    B.Add(Shape,myShapes(i));
  }
  return Shape;
}
  
//=======================================================================
//function : InitShape
//purpose  : 
//=======================================================================

Standard_Integer
HLRBRep_PolyAlgo::InitShape (const TopoDS_Shape& Shape,
			     Standard_Boolean& IsoledF,
			     Standard_Boolean& IsoledE)
{
  TopTools_MapOfShape ShapeMap0;
  Standard_Integer nbShell = 0;
  IsoledF = Standard_False;
  IsoledE = Standard_False;
  TopExp_Explorer exshell,exface,exedge;
  TopLoc_Location L;
  
  for (exshell.Init(Shape, TopAbs_SHELL);
       exshell.More(); 
       exshell.Next()) {
    Standard_Boolean withTrian = Standard_False;
    
    for (exface.Init(exshell.Current(), TopAbs_FACE);
	 exface.More(); 
	 exface.Next()) {
      const TopoDS_Face& F = TopoDS::Face(exface.Current());
      if (!BRep_Tool::Triangulation(F,L).IsNull()) {
	if (ShapeMap0.Add(F))
	  withTrian = Standard_True;
      }
    }
    if (withTrian) nbShell++;
  }
  
  for (exface.Init(Shape, TopAbs_FACE, TopAbs_SHELL);
       exface.More() && !IsoledF; 
       exface.Next()) {
    const TopoDS_Face& F = TopoDS::Face(exface.Current());
    if (!BRep_Tool::Triangulation(F,L).IsNull()) {
      if (ShapeMap0.Add(F))
	IsoledF = Standard_True;
    }
  }
  if (IsoledF) nbShell++;
  
  for (exedge.Init(Shape, TopAbs_EDGE, TopAbs_FACE);
       exedge.More() && !IsoledE; 
       exedge.Next())
    IsoledE = Standard_True;
  if (IsoledE) nbShell++;
  if (nbShell > 0)
    myAlgo->Init(new TColStd_HArray1OfTransient(1,nbShell));
  return nbShell;
}

//=======================================================================
//function : StoreShell
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::StoreShell (const TopoDS_Shape& Shape,
				   Standard_Integer& iShell,
				   TColStd_Array1OfTransient& Shell,
				   const Standard_Boolean IsoledF,
				   const Standard_Boolean IsoledE,
				   TColStd_Array1OfInteger& ES,
				   TColStd_Array1OfTransient& PD,
				   TColStd_Array1OfTransient& PID,
				   TopTools_MapOfShape& ShapeMap1,
				   TopTools_MapOfShape& ShapeMap2)
{
  TopLoc_Location L;
  TopExp_Explorer exface,exedge;
  Standard_Integer f = 0,i,j;
  Standard_Integer nbFaceShell = 0;
  Standard_Boolean reversed;
  Standard_Boolean closed    = Standard_False;
  const gp_Trsf& T  = myProj.Transformation();
  const gp_Trsf& TI = myProj.InvertedTransformation();
  const gp_XYZ& tloc = T.TranslationPart();
  TLoc[0] = tloc.X();
  TLoc[1] = tloc.Y();
  TLoc[2] = tloc.Z();
  const gp_Mat& tmat = T.VectorialPart();
  TMat[0][0] = tmat.Value(1,1);
  TMat[0][1] = tmat.Value(1,2);
  TMat[0][2] = tmat.Value(1,3);
  TMat[1][0] = tmat.Value(2,1);
  TMat[1][1] = tmat.Value(2,2);
  TMat[1][2] = tmat.Value(2,3);
  TMat[2][0] = tmat.Value(3,1);
  TMat[2][1] = tmat.Value(3,2);
  TMat[2][2] = tmat.Value(3,3);
  const gp_XYZ& tilo = TI.TranslationPart();
  TILo[0] = tilo.X();
  TILo[1] = tilo.Y();
  TILo[2] = tilo.Z();
  const gp_Mat& tima = TI.VectorialPart();
  TIMa[0][0] = tima.Value(1,1);
  TIMa[0][1] = tima.Value(1,2);
  TIMa[0][2] = tima.Value(1,3);
  TIMa[1][0] = tima.Value(2,1);
  TIMa[1][1] = tima.Value(2,2);
  TIMa[1][2] = tima.Value(2,3);
  TIMa[2][0] = tima.Value(3,1);
  TIMa[2][1] = tima.Value(3,2);
  TIMa[2][2] = tima.Value(3,3);
  if (!IsoledE) {
    if (!IsoledF) {
      closed = Shape.Closed();
      if (!closed) {
	TopTools_IndexedMapOfShape EM;
	TopExp::MapShapes(Shape,TopAbs_EDGE,EM);
	Standard_Integer ie;
	Standard_Integer nbEdge = EM.Extent ();
	Standard_Integer *flag = new Standard_Integer[nbEdge + 1];

	for (ie = 1; ie <= nbEdge; ie++)
	  flag[ie] = 0;
	
	for (exedge.Init(Shape, TopAbs_EDGE);
	     exedge.More(); 
	     exedge.Next()) {
	  const TopoDS_Edge& E = TopoDS::Edge(exedge.Current());
	  ie = EM.FindIndex(E);
	  TopAbs_Orientation orient = E.Orientation();
	  if (!BRep_Tool::Degenerated(E)) {
	    if      (orient == TopAbs_FORWARD ) flag[ie] += 1;
	    else if (orient == TopAbs_REVERSED) flag[ie] -= 1;
	  }
	}
	closed = Standard_True;
	
	for (ie = 1; ie <= nbEdge && closed; ie++)
	  closed = (flag[ie] == 0); 
	delete [] flag;
	flag = NULL;
      }
      
      exface.Init(Shape, TopAbs_FACE);
    }
    else
      exface.Init(Shape, TopAbs_FACE, TopAbs_SHELL);
    
    for (; exface.More(); exface.Next()) {
      const TopoDS_Face& F = TopoDS::Face(exface.Current());
      if (!BRep_Tool::Triangulation(F,L).IsNull()) {
	if (ShapeMap1.Add(F))
	  nbFaceShell++;
      }
    }
  }
  if (nbFaceShell > 0 || IsoledE) {
    iShell++;
    Shell(iShell) = new HLRAlgo_PolyShellData(nbFaceShell);
  }
  if (nbFaceShell > 0) {
    const Handle(HLRAlgo_PolyShellData)& psd =
      *(Handle(HLRAlgo_PolyShellData)*)&(Shell(iShell));
    Standard_Integer iFace = 0;
    if (!IsoledF) exface.Init(Shape, TopAbs_FACE);
    else          exface.Init(Shape, TopAbs_FACE, TopAbs_SHELL);
    TopTools_MapOfShape ShapeMapBis;
    
    for (; exface.More(); exface.Next()) {
      const TopoDS_Face& F = TopoDS::Face(exface.Current());  
      const Handle(Poly_Triangulation)& Tr = BRep_Tool::Triangulation(F,L);
      if (!Tr.IsNull()) {
	if (ShapeMap2.Add(F)) {
	  iFace++;
	  f = myFMap.FindIndex(F);
	  reversed = F.Orientation() == TopAbs_REVERSED;
	  gp_Trsf TT = L.Transformation();
	  TT.PreMultiply(T);
	  const gp_XYZ& ttlo = TT.TranslationPart();
	  TTLo[0] = ttlo.X();
	  TTLo[1] = ttlo.Y();
	  TTLo[2] = ttlo.Z();
	  const gp_Mat& ttma = TT.VectorialPart();
	  TTMa[0][0] = ttma.Value(1,1);
	  TTMa[0][1] = ttma.Value(1,2);
	  TTMa[0][2] = ttma.Value(1,3);
	  TTMa[1][0] = ttma.Value(2,1);
	  TTMa[1][1] = ttma.Value(2,2);
	  TTMa[1][2] = ttma.Value(2,3);
	  TTMa[2][0] = ttma.Value(3,1);
	  TTMa[2][1] = ttma.Value(3,2);
	  TTMa[2][2] = ttma.Value(3,3);
	  Poly_Array1OfTriangle & Tri = Tr->ChangeTriangles();
	  TColgp_Array1OfPnt    & Nod = Tr->ChangeNodes();
	  Standard_Integer nbN = Nod.Upper();
	  Standard_Integer nbT = Tri.Upper();
	  PD (f) = new HLRAlgo_PolyData();
	  psd->PolyData().ChangeValue(iFace) = PD(f);
	  PID(f) = new HLRAlgo_PolyInternalData(nbN,nbT);
	  Handle(HLRAlgo_PolyInternalData)& pid = 
	    *(Handle(HLRAlgo_PolyInternalData)*)&(PID(f));
	  Handle(Geom_Surface) S = BRep_Tool::Surface(F);
	  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
	    S = Handle(Geom_RectangularTrimmedSurface)::DownCast(S)->BasisSurface();
	  GeomAdaptor_Surface AS(S);
	  pid->Planar(AS.GetType() == GeomAbs_Plane);
	  Standard_Address TData = &pid->TData();
	  Standard_Address PISeg = &pid->PISeg();
	  Standard_Address PINod = &pid->PINod();
	  Poly_Triangle       * OT = &(Tri.ChangeValue(1));
	  HLRAlgo_TriangleData* NT =
	    &(((HLRAlgo_Array1OfTData*)TData)->ChangeValue(1));

	  for (i = 1; i <= nbT; i++) {
	    Standard_Address Tri2Indices = NT->Indices();
	    OT->Get(Tri2Node1,Tri2Node2,Tri2Node3);
	    Tri2Flags = 0;
	    if (reversed) {
	      j         = Tri2Node1;
	      Tri2Node1 = Tri2Node3;
	      Tri2Node3 = j;
	    }
	    OT++;
	    NT++;
	  }

	  gp_Pnt                          * ON = &(Nod.ChangeValue(1));
	  Handle(HLRAlgo_PolyInternalNode)* NN = 
	    &(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(1));

	  for (i = 1; i <= nbN; i++) {
	    const Standard_Address Nod1RValues = (*NN)->RValues();
	    const Standard_Address Nod1Indices = (*NN)->Indices();
	    Nod1NdSg = 0;
	    Nod1Flag = 0;
	    Nod1PntX = ON->X();
	    Nod1PntY = ON->Y();
	    Nod1PntZ = ON->Z();
	    TTMultiply(Nod1PntX,Nod1PntY,Nod1PntZ);
	    ON++;
	    NN++;
	  }
	  pid->UpdateLinks(TData,PISeg,PINod);
	  if (Tr->HasUVNodes()) {
	    myBSurf.Initialize(F,Standard_False);
	    TColgp_Array1OfPnt2d & UVN = Tr->ChangeUVNodes();
	    gp_Pnt2d* OUVN = &(UVN.ChangeValue(1));
	    NN             = &(((HLRAlgo_Array1OfPINod*)PINod)->
			       ChangeValue(1));
	    
	    for (i = 1; i <= nbN; i++) {
	      const Standard_Address Nod1Indices = (*NN)->Indices();
	      const Standard_Address Nod1RValues = (*NN)->RValues();
	      Nod1PntU = OUVN->X();
	      Nod1PntV = OUVN->Y();
	      if (Normal(i,Nod1Indices,Nod1RValues,
			 TData,PISeg,PINod,Standard_False))
		Nod1Flag |=  NMskNorm;
	      else {
		Nod1Flag &= ~NMskNorm;
		Nod1Scal = 0;
	      }
	      OUVN++;
	      NN++;
	    }
	  }
#ifdef OCCT_DEBUG
	  else if (DoError) {
	    cout << " HLRBRep_PolyAlgo::StoreShell : Face ";
	    cout << f << " non triangulated" << endl;
	  }
#endif
	  NT = &(((HLRAlgo_Array1OfTData*)TData)->ChangeValue(1));

	  for (i = 1; i <= nbT; i++) {
	    const Standard_Address Tri1Indices = NT->Indices();
	    const Handle(HLRAlgo_PolyInternalNode)* PN1 = 
	      &(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri1Node1));
	    const Handle(HLRAlgo_PolyInternalNode)* PN2 = 
	      &(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri1Node2));
	    const Handle(HLRAlgo_PolyInternalNode)* PN3 = 
	      &(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri1Node3));
	    const Standard_Address Nod1Indices = (*PN1)->Indices();
	    const Standard_Address Nod2Indices = (*PN2)->Indices();
	    const Standard_Address Nod3Indices = (*PN3)->Indices();
	    const Standard_Address Nod1RValues = (*PN1)->RValues();
	    const Standard_Address Nod2RValues = (*PN2)->RValues();
	    const Standard_Address Nod3RValues = (*PN3)->RValues();
	    OrientTriangle(i,Tri1Indices,
			   Nod1Indices,Nod1RValues,
			   Nod2Indices,Nod2RValues,
			   Nod3Indices,Nod3RValues);
	    NT++;
	  }
	}
      }
#ifdef OCCT_DEBUG
      else if (DoError) {
	cout << "HLRBRep_PolyAlgo::StoreShell : Face ";
	cout << f << " deja stockee" << endl;
      }
#endif
    }
    Standard_Integer nbFace = myFMap.Extent();
    HLRAlgo_ListOfBPoint& List = psd->Edges();
    TopTools_IndexedDataMapOfShapeListOfShape EF;
    TopExp::MapShapesAndAncestors(Shape,TopAbs_EDGE,TopAbs_FACE,EF);
    Handle(HLRAlgo_PolyInternalData)* pid = 
	(Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));
    
    for (f = 1; f <= nbFace; f++) {
      if (!(*pid).IsNull()) {

	for (exedge.Init(myFMap(f),TopAbs_EDGE);
	     exedge.More(); 
	     exedge.Next()) {
	  TopoDS_Edge E = TopoDS::Edge(exedge.Current());
	  if (ShapeMap1.Add(E)) {
	    Standard_Integer e = myEMap.FindIndex(E);
	    ES(e) = iShell;
	    Standard_Integer anIndexE = EF.FindIndex(E);
	    if (anIndexE > 0) {
	      TopTools_ListOfShape& LS = EF(anIndexE);
	      InitBiPointsWithConnexity(e,E,List,PID,LS,Standard_True);
	    }
	    else {
	      TopTools_ListOfShape LS;
	      InitBiPointsWithConnexity(e,E,List,PID,LS,Standard_False);
	    }
	  }
	}
      }
      pid++;
    }
    InsertOnOutLine(PID);
    CheckFrBackTriangles(List,PID);
    UpdateOutLines(List,PID);
    UpdateEdgesBiPoints(List,PID,closed);
    UpdatePolyData(PD,PID,closed);
    pid = (Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));
    
    for (f = 1; f <= nbFace; f++) {
      (*pid).Nullify();  
      pid++;
    }
  }
  else if (IsoledE) {
    const Handle(HLRAlgo_PolyShellData)& psd =
      *(Handle(HLRAlgo_PolyShellData)*)&(Shell(iShell));
    HLRAlgo_ListOfBPoint& List = psd->Edges();
    
    for (exedge.Init(Shape, TopAbs_EDGE, TopAbs_FACE);
	 exedge.More(); 
	 exedge.Next()) {
      TopoDS_Edge E = TopoDS::Edge(exedge.Current());
      if (ShapeMap1.Add(E)) {
	Standard_Integer e = myEMap.FindIndex(E);
	ES(e) = iShell;
	TopTools_ListOfShape LS;
	InitBiPointsWithConnexity(e,E,List,PID,LS,Standard_False);
      }
    }
  }
}

//=======================================================================
//function : Normal
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_PolyAlgo::
Normal (const Standard_Integer iNode,
	const Standard_Address Nod1Indices,
	const Standard_Address Nod1RValues,
        Standard_Address& TData,
        Standard_Address& PISeg,
        Standard_Address& PINod,
        const Standard_Boolean orient) const
{
  gp_Vec D1U,D1V,D2U,D2V,D2UV;
  gp_Pnt P;
  gp_Dir Norma;
  Standard_Boolean OK;
  CSLib_DerivativeStatus Status;
  CSLib_NormalStatus NStat;
  myBSurf.D1(Nod1PntU,Nod1PntV,P,D1U,D1V);
  CSLib::Normal(D1U,D1V,Standard_Real(Precision::Angular()),
		Status,Norma);
  if (Status != CSLib_Done) {
    myBSurf.D2(Nod1PntU,Nod1PntV,P,D1U,D1V,D2U,D2V,D2UV);
    CSLib::Normal(D1U,D1V,D2U,D2V,D2UV,
		  Precision::Angular(),OK,NStat,Norma);
    if (!OK)
      return Standard_False;
  }
  Standard_Real EyeX =  0;
  Standard_Real EyeY =  0;
  Standard_Real EyeZ = -1;
  if (myProj.Perspective()) {
    EyeX = Nod1PntX;
    EyeY = Nod1PntY;
    EyeZ = Nod1PntZ - myProj.Focus();
    Standard_Real d = sqrt(EyeX * EyeX + EyeY * EyeY + EyeZ * EyeZ);
    if (d > 0) {
      EyeX /= d;
      EyeY /= d;
      EyeZ /= d;
    }
  }
  Nod1NrmX = Norma.X();
  Nod1NrmY = Norma.Y();
  Nod1NrmZ = Norma.Z();
//  TMultiply(Nod1NrmX,Nod1NrmY,Nod1NrmZ);
  TMultiply(Nod1NrmX,Nod1NrmY,Nod1NrmZ,myProj.Perspective()); //OCC349
  Standard_Real NormX,NormY,NormZ;
  
  if (AverageNormal(iNode,Nod1Indices,TData,PISeg,PINod,
		    NormX,NormY,NormZ)) {
    if (Nod1NrmX * NormX +
	Nod1NrmY * NormY +
	Nod1NrmZ * NormZ < 0) {
      Nod1NrmX = -Nod1NrmX;
      Nod1NrmY = -Nod1NrmY;
      Nod1NrmZ = -Nod1NrmZ;
    }
    Nod1Scal = (Nod1NrmX * EyeX +
		Nod1NrmY * EyeY +
		Nod1NrmZ * EyeZ);
  }
  else {
    Nod1Scal = 0;
    Nod1NrmX = 1;
    Nod1NrmY = 0;
    Nod1NrmZ = 0;
#ifdef OCCT_DEBUG
    if (DoError) {
      cout << "HLRBRep_PolyAlgo::Normal : AverageNormal error";
      cout << endl;
    }
#endif
  }
  if (Nod1Scal > 0) {
    if ( Nod1Scal < myTolAngular) {
      Nod1Scal  = 0;
      Nod1Flag |= NMskOutL;
    }
  }
  else {
    if (-Nod1Scal < myTolAngular) {
      Nod1Scal  = 0;
      Nod1Flag |= NMskOutL;
    }
  }
  if (orient) UpdateAroundNode(iNode,Nod1Indices,
			       TData,PISeg,PINod);
  return Standard_True;
}

//=======================================================================
//function : AverageNormal
//purpose  : 
//=======================================================================

Standard_Boolean
HLRBRep_PolyAlgo::AverageNormal(const Standard_Integer iNode,
				const Standard_Address Nod1Indices,
				Standard_Address& TData,
				Standard_Address& PISeg,
				Standard_Address& PINod,
				Standard_Real& X,
				Standard_Real& Y,
				Standard_Real& Z) const
{
  Standard_Boolean OK = Standard_False;
  Standard_Integer jNode = 0,kNode,iiii,iTri1,iTri2;
  X = 0;
  Y = 0;
  Z = 0;
  iiii = Nod1NdSg;

  while (iiii != 0 && !OK) {
    const Standard_Address Seg2Indices = 
      ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii).Indices();
    iTri1 = Seg2Conex1;
    iTri2 = Seg2Conex2;
    if ( iTri1 != 0) AddNormalOnTriangle
      (iTri1,iNode,jNode,TData,PINod,X,Y,Z,OK);
    if ( iTri2 != 0) AddNormalOnTriangle
      (iTri2,iNode,jNode,TData,PINod,X,Y,Z,OK);
    if (Seg2LstSg1 == iNode) iiii = Seg2NxtSg1;
    else                     iiii = Seg2NxtSg2;
  }

  if (jNode != 0) {
    const Standard_Address Nod2Indices =
      ((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(jNode)->Indices();
    iiii = Nod2NdSg;
    
    while (iiii != 0 && !OK) {
      const Standard_Address Seg2Indices = 
	((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii).Indices();
      iTri1 = Seg2Conex1;
      iTri2 = Seg2Conex2;
      if ( iTri1 != 0) AddNormalOnTriangle
	(iTri1,jNode,kNode,TData,PINod,X,Y,Z,OK);
      if ( iTri2 != 0) AddNormalOnTriangle
	(iTri2,jNode,kNode,TData,PINod,X,Y,Z,OK);
      if (Seg2LstSg1 == jNode) iiii = Seg2NxtSg1;
      else                     iiii = Seg2NxtSg2;
    }
  }
  Standard_Real d = sqrt (X * X + Y * Y + Z * Z);
  if (OK && d < 1.e-10) {
    OK = Standard_False;
#ifdef OCCT_DEBUG
    if (DoError) {
      cout << "HLRAlgo_PolyInternalData:: inverted normals on ";
      cout << "node " << iNode << endl;
    }
#endif
  }
  return OK;
}

//=======================================================================
//function : AddNormalOnTriangle
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
AddNormalOnTriangle(const Standard_Integer iTri,
		    const Standard_Integer iNode,
		    Standard_Integer& jNode,
		    Standard_Address& TData,
		    Standard_Address& PINod,
		    Standard_Real& X,
		    Standard_Real& Y,
		    Standard_Real& Z,
		    Standard_Boolean& OK) const
{
  Standard_Real dn,dnx,dny,dnz;
  Standard_Real d1,dx1,dy1,dz1;
  Standard_Real d2,dx2,dy2,dz2;
  Standard_Real d3,dx3,dy3,dz3;
  const Standard_Address Tri2Indices =
    ((HLRAlgo_Array1OfTData*)TData)->ChangeValue(iTri).Indices();
  const Standard_Address Nod1RValues =
    ((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri2Node1)->RValues();
  const Standard_Address Nod2RValues =
    ((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri2Node2)->RValues();
  const Standard_Address Nod3RValues =
    ((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri2Node3)->RValues();
  dx1 = Nod2PntX - Nod1PntX;
  dy1 = Nod2PntY - Nod1PntY;
  dz1 = Nod2PntZ - Nod1PntZ;
  d1 = sqrt(dx1 * dx1 + dy1 * dy1 + dz1 * dz1);
  if (d1 < 1.e-10) {
    if      (Tri2Node1 == iNode) jNode = Tri2Node2;
    else if (Tri2Node2 == iNode) jNode = Tri2Node1;
  }
  else {
    dx2 = Nod3PntX - Nod2PntX;
    dy2 = Nod3PntY - Nod2PntY;
    dz2 = Nod3PntZ - Nod2PntZ;
    d2 = sqrt(dx2 * dx2 + dy2 * dy2 + dz2 * dz2);
    if (d2 < 1.e-10) {
      if      (Tri2Node2 == iNode) jNode = Tri2Node3;
      else if (Tri2Node3 == iNode) jNode = Tri2Node2;
    }
    else {
      dx3 = Nod1PntX - Nod3PntX;
      dy3 = Nod1PntY - Nod3PntY;
      dz3 = Nod1PntZ - Nod3PntZ;
      d3 = sqrt(dx3 * dx3 + dy3 * dy3 + dz3 * dz3);
      if (d3 < 1.e-10) {
	if      (Tri2Node3 == iNode) jNode = Tri2Node1;
	else if (Tri2Node1 == iNode) jNode = Tri2Node3;
      }
      else {
	dn = 1 / (d1 * d2);
	dnx = (dy1 * dz2 - dy2 * dz1) * dn;
	dny = (dz1 * dx2 - dz2 * dx1) * dn;
	dnz = (dx1 * dy2 - dx2 * dy1) * dn;
	dn = sqrt(dnx * dnx + dny * dny + dnz * dnz);
	if (dn > 1.e-10) {
	  OK = Standard_True;
	  X += dnx;
	  Y += dny;
	  Z += dnz;
	}
      }
    }
  }
}

//=======================================================================
//function : InitBiPointsWithConnexity
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::
InitBiPointsWithConnexity (const Standard_Integer e,
			   TopoDS_Edge& E,
			   HLRAlgo_ListOfBPoint& List,
			   TColStd_Array1OfTransient& PID,
			   TopTools_ListOfShape& LS,
			   const Standard_Boolean connex)
{
  Standard_Integer iPol,nbPol,i1,i1p1,i1p2,i2,i2p1,i2p2;
  Standard_Real X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ;
  Standard_Real XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2;
  Standard_Real U1,U2 = 0.;
  Handle(Poly_PolygonOnTriangulation) HPol[2];
  TopLoc_Location L;
  myBCurv.Initialize(E);
  if (connex) {
    Standard_Integer nbConnex = LS.Extent();
    if      (nbConnex == 1) {
      TopTools_ListIteratorOfListOfShape itn(LS);
      const TopoDS_Face& F1 = TopoDS::Face(itn.Value());
      i1      = myFMap.FindIndex(F1);
      const Handle(Poly_Triangulation)& Tr1 = BRep_Tool::Triangulation(F1,L);
      HPol[0] = BRep_Tool::PolygonOnTriangulation(E,Tr1,L);
      const Handle(HLRAlgo_PolyInternalData)& pid1 = 
	*(Handle(HLRAlgo_PolyInternalData)*)&(PID(i1));
      if (!HPol[0].IsNull()) {
	myPC.Initialize(E,F1);
	const Handle(TColStd_HArray1OfReal)& par = HPol[0]->Parameters();
	const TColStd_Array1OfInteger&      Pol1 = HPol[0]->Nodes();
	nbPol = Pol1.Upper();
	Standard_Address TData1 = &pid1->TData();
	Standard_Address PISeg1 = &pid1->PISeg();
	Standard_Address PINod1 = &pid1->PINod();
	Standard_Address Nod11Indices,Nod12Indices;
	Standard_Address Nod11RValues,Nod12RValues;
	const Handle(HLRAlgo_PolyInternalNode)* pi1p1 =
	  &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(1    )));
	Nod11Indices = (*pi1p1)->Indices();
	Nod11RValues = (*pi1p1)->RValues();
	const Handle(HLRAlgo_PolyInternalNode)* pi1p2 =
	  &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(nbPol)));
	Nod12Indices = (*pi1p2)->Indices();
	Nod12RValues = (*pi1p2)->RValues();
	Nod11Flag |=  NMskVert;
	Nod12Flag |=  NMskVert;
	
	for (iPol = 1; iPol <= nbPol; iPol++) {
	  const Handle(HLRAlgo_PolyInternalNode)* pi1pA =
	    &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(iPol)));
	  Standard_Address Nod1AIndices = (*pi1pA)->Indices();
	  Standard_Address Nod1ARValues = (*pi1pA)->RValues();
	  if (Nod1AEdg1 == 0 || Nod1AEdg1 == (Standard_Boolean) e) {
	    Nod1AEdg1 = e;
	    Nod1APCu1 = par->Value(iPol);
	  }
	  else {
	    Nod1AEdg2 = e;
	    Nod1APCu2 = par->Value(iPol);
	  }
	}
	
	i1p2 = Pol1(1);
	Nod12Indices = Nod11Indices;
	Nod12RValues = Nod11RValues;
	XTI2 = X2 = Nod12PntX;
	YTI2 = Y2 = Nod12PntY;
	ZTI2 = Z2 = Nod12PntZ;
	if      (Nod12Edg1 ==  (Standard_Boolean) e) U2 = Nod12PCu1;
	else if (Nod12Edg2 ==  (Standard_Boolean) e) U2 = Nod12PCu2;
#ifdef OCCT_DEBUG
	else {
	  cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
	  cout << "Parameter error on Node " << i1p2 << endl;
	}
#endif
	Nod12Flag |= NMskEdge;
	TIMultiply(XTI2,YTI2,ZTI2);
	if (Pol1(1) == Pol1(nbPol) && myPC.IsPeriodic())
	  U2 = U2 - myPC.Period();
	
	if (nbPol == 2 && BRep_Tool::Degenerated(E)) {
	  CheckDegeneratedSegment(Nod11Indices,Nod11RValues,
				  Nod12Indices,Nod12RValues);
	  UpdateAroundNode(Pol1(1    ),Nod11Indices,TData1,PISeg1,PINod1);
	  UpdateAroundNode(Pol1(nbPol),Nod12Indices,TData1,PISeg1,PINod1);
	}
	else {

	  for (iPol = 2; iPol <= nbPol; iPol++) {
	    i1p1 = i1p2;
	    Nod11Indices = Nod12Indices;
	    Nod11RValues = Nod12RValues;
	    i1p2 = Pol1(iPol);
	    const Handle(HLRAlgo_PolyInternalNode)* pi1p2iPol =
	      &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(iPol)));
	    Nod12Indices = (*pi1p2iPol)->Indices();
	    Nod12RValues = (*pi1p2iPol)->RValues();
#ifdef OCCT_DEBUG
	    if (DoError) {
	      if (Nod11NrmX*Nod12NrmX +
		  Nod11NrmY*Nod12NrmY +
		  Nod11NrmZ*Nod12NrmZ < 0) {
		cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
		cout << "Too big angle between " << i1p1 << setw(6);
		cout << " and " << i1p2 << setw(6);
		cout << " in face " << i1 << endl;
	      }
	    }
#endif
	    X1   = X2;
	    Y1   = Y2;
	    Z1   = Z2;
	    XTI1 = XTI2;
	    YTI1 = YTI2;
	    ZTI1 = ZTI2;
	    U1   = U2;
	    XTI2 = X2 = Nod12PntX;
	    YTI2 = Y2 = Nod12PntY;
	    ZTI2 = Z2 = Nod12PntZ;
	    if      (Nod12Edg1 ==  (Standard_Boolean) e) U2 = Nod12PCu1;
	    else if (Nod12Edg2 ==  (Standard_Boolean) e) U2 = Nod12PCu2;
#ifdef OCCT_DEBUG
	    else {
	      cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
	      cout << "Parameter error on Node " << i1p2 << endl;
	    }
#endif
	    Nod12Flag |= NMskEdge;
	    TIMultiply(XTI2,YTI2,ZTI2);
	    Interpolation(List,
			  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			  XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			  e,U1,U2,
			  Nod11Indices,Nod11RValues,
			  Nod12Indices,Nod12RValues,
			  i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1);
	  }
	}
      }
#ifdef OCCT_DEBUG
      else if (DoError) {
	cout << "HLRBRep_PolyAlgo::InitBiPointsWithConnexity : Edge ";
	cout << e << " connex 1 sans PolygonOnTriangulation" << endl;
      }
#endif
    }
    else if (nbConnex == 2) {
      TopTools_ListIteratorOfListOfShape itn(LS);
      const TopoDS_Face& F1 = TopoDS::Face(itn.Value());
      i1      = myFMap.FindIndex(F1);
      const Handle(Poly_Triangulation)& Tr1 = BRep_Tool::Triangulation(F1,L);
      HPol[0] = BRep_Tool::PolygonOnTriangulation(E,Tr1,L);
      itn.Next();
      const TopoDS_Face& F2 = TopoDS::Face(itn.Value());
      i2      = myFMap.FindIndex(F2);
      if (i1 == i2) E.Reverse();
      const Handle(Poly_Triangulation)& Tr2 = BRep_Tool::Triangulation(F2,L);
      HPol[1] = BRep_Tool::PolygonOnTriangulation(E,Tr2,L);
      GeomAbs_Shape rg = BRep_Tool::Continuity(E,F1,F2);
      const Handle(HLRAlgo_PolyInternalData)& pid1 = 
	*(Handle(HLRAlgo_PolyInternalData)*)&(PID(i1));
      const Handle(HLRAlgo_PolyInternalData)& pid2 = 
	*(Handle(HLRAlgo_PolyInternalData)*)&(PID(i2));
      if (!HPol[0].IsNull() && !HPol[1].IsNull()) {
	myPC.Initialize(E,F1);
	const TColStd_Array1OfInteger&      Pol1 = HPol[0]->Nodes();
	const TColStd_Array1OfInteger&      Pol2 = HPol[1]->Nodes();
	const Handle(TColStd_HArray1OfReal)& par = HPol[0]->Parameters();
	Standard_Integer nbPol1 = Pol1.Upper();
	Standard_Address TData1 = &pid1->TData();
	Standard_Address PISeg1 = &pid1->PISeg();
	Standard_Address PINod1 = &pid1->PINod();
	Standard_Address TData2 = &pid2->TData();
	Standard_Address PISeg2 = &pid2->PISeg();
	Standard_Address PINod2 = &pid2->PINod();
	Standard_Address Nod11Indices,Nod11RValues;
	Standard_Address Nod12Indices,Nod12RValues;
	Standard_Address Nod21Indices,Nod21RValues;
	Standard_Address Nod22Indices,Nod22RValues;
	const Handle(HLRAlgo_PolyInternalNode)* pi1p1 =
	  &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(1    )));
	Nod11Indices = (*pi1p1)->Indices();
	Nod11RValues = (*pi1p1)->RValues();
	const Handle(HLRAlgo_PolyInternalNode)* pi1p2nbPol1 =
	  &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(nbPol1)));
	Nod12Indices = (*pi1p2nbPol1)->Indices();
	Nod12RValues = (*pi1p2nbPol1)->RValues();
	const Handle(HLRAlgo_PolyInternalNode)* pi2p1 =
	  &(((HLRAlgo_Array1OfPINod*)PINod2)->ChangeValue(Pol2(1    )));
	Nod21Indices = (*pi2p1)->Indices();
	Nod21RValues = (*pi2p1)->RValues();
	const Handle(HLRAlgo_PolyInternalNode)* pi2p2 =
	  &(((HLRAlgo_Array1OfPINod*)PINod2)->ChangeValue(Pol2(nbPol1)));
	Nod22Indices = (*pi2p2)->Indices();
	Nod22RValues = (*pi2p2)->RValues();
	Nod11Flag |=  NMskVert;
	Nod12Flag |=  NMskVert;
	Nod21Flag |=  NMskVert;
	Nod22Flag |=  NMskVert;
	
	for (iPol = 1; iPol <= nbPol1; iPol++) {
	  const Handle(HLRAlgo_PolyInternalNode)* pi1pA =
	    &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(iPol)));
	  Standard_Address Nod1AIndices = (*pi1pA)->Indices();
	  Standard_Address Nod1ARValues = (*pi1pA)->RValues();
	  const Handle(HLRAlgo_PolyInternalNode)* pi2pA =
	    &(((HLRAlgo_Array1OfPINod*)PINod2)->ChangeValue(Pol2(iPol)));
	  Standard_Address Nod2AIndices = (*pi2pA)->Indices();
	  Standard_Address Nod2ARValues = (*pi2pA)->RValues();
	  Standard_Real PCu = par->Value(iPol);
	  if (Nod1AEdg1 == 0 || Nod1AEdg1 ==  (Standard_Boolean) e) {
	    Nod1AEdg1 = e;
	    Nod1APCu1 = PCu;
	  }
	  else {
	    Nod1AEdg2 = e;
	    Nod1APCu2 = PCu;
	  }
	  if (Nod2AEdg1 == 0 || Nod2AEdg1 == (Standard_Boolean) e) {
	    Nod2AEdg1 = e;
	    Nod2APCu1 = PCu;
	  }
	  else {
	    Nod2AEdg2 = e;
	    Nod2APCu2 = PCu;
	  }
	}

	i1p2 = Pol1(1);
	Nod12Indices = Nod11Indices;
	Nod12RValues = Nod11RValues;
	i2p2 = Pol2(1);
	Nod22Indices = Nod21Indices;
	Nod22RValues = Nod21RValues;
	XTI2 = X2 = Nod12PntX;
	YTI2 = Y2 = Nod12PntY;
	ZTI2 = Z2 = Nod12PntZ;
	if      (Nod12Edg1 == (Standard_Boolean) e) U2 = Nod12PCu1;
	else if (Nod12Edg2 == (Standard_Boolean) e) U2 = Nod12PCu2;
#ifdef OCCT_DEBUG
	else {
	  cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
	  cout << "Parameter error on Node " << i1p2 << endl;
	}
#endif
	Nod12Flag |= NMskEdge;
	Nod22Flag |= NMskEdge;
	TIMultiply(XTI2,YTI2,ZTI2);
	if (Pol1(1) == Pol1(nbPol1) && myPC.IsPeriodic())
	  U2 = U2 - myPC.Period();
	
	if (nbPol1 == 2 && BRep_Tool::Degenerated(E)) {
	  CheckDegeneratedSegment(Nod11Indices,Nod11RValues,
				  Nod12Indices,Nod12RValues);
	  CheckDegeneratedSegment(Nod21Indices,Nod21RValues,
				  Nod22Indices,Nod22RValues);
	  UpdateAroundNode(Pol1(1    ),Nod11Indices,TData1,PISeg1,PINod1);
	  UpdateAroundNode(Pol1(nbPol1),Nod12Indices,TData1,PISeg1,PINod1);
	  UpdateAroundNode(Pol2(1    ),Nod21Indices,TData2,PISeg2,PINod2);
	  UpdateAroundNode(Pol2(nbPol1),Nod22Indices,TData2,PISeg2,PINod2);
	}
	else {

	  for (iPol = 2; iPol <= nbPol1; iPol++) {
	    i1p1 = i1p2;
	    Nod11Indices = Nod12Indices;
	    Nod11RValues = Nod12RValues;
	    i2p1 = i2p2;
	    Nod21Indices = Nod22Indices;
	    Nod21RValues = Nod22RValues;
	    i1p2 = Pol1(iPol);
	    const Handle(HLRAlgo_PolyInternalNode)* pi1p2iPol =
	      &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(Pol1(iPol)));
	    Nod12Indices = (*pi1p2iPol)->Indices();
	    Nod12RValues = (*pi1p2iPol)->RValues();
	    i2p2 = Pol2(iPol);
	    const Handle(HLRAlgo_PolyInternalNode)* pi2p2iPol =
	      &(((HLRAlgo_Array1OfPINod*)PINod2)->ChangeValue(Pol2(iPol)));
	    Nod22Indices = (*pi2p2iPol)->Indices();
	    Nod22RValues = (*pi2p2iPol)->RValues();
#ifdef OCCT_DEBUG
	    if (DoError) {
	      if (Nod11NrmX*Nod12NrmX +
		  Nod11NrmY*Nod12NrmY +
		  Nod11NrmZ*Nod12NrmZ < 0) {
		cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
		cout << "To big angle between " << i1p1 << setw(6);
		cout << " and " << i1p2 << setw(6);
		cout << " in face " << i1 << endl;
	      }
	      if (Nod21NrmX*Nod22NrmX +
		  Nod21NrmY*Nod22NrmY +
		  Nod21NrmZ*Nod22NrmZ < 0) {
		cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
		cout << "To big angle between " << i2p1 << setw(6);
		cout << " and " << i2p2 << setw(6);
		cout<< " in face " << i2 << endl;
	      }
	    }
#endif
	    X1   = X2;
	    Y1   = Y2;
	    Z1   = Z2;
	    XTI1 = XTI2;
	    YTI1 = YTI2;
	    ZTI1 = ZTI2;
	    U1   = U2;
	    XTI2 = X2 = Nod12PntX;
	    YTI2 = Y2 = Nod12PntY;
	    ZTI2 = Z2 = Nod12PntZ;
	    if      (Nod12Edg1 == (Standard_Boolean) e) U2 = Nod12PCu1;
	    else if (Nod12Edg2 == (Standard_Boolean) e) U2 = Nod12PCu2;
#ifdef OCCT_DEBUG
	    else {
	      cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
	      cout << "Parameter error on Node " << i1p2 << endl;
	    }
#endif
	    Nod12Flag |= NMskEdge;
	    Nod22Flag |= NMskEdge;
	    TIMultiply(XTI2,YTI2,ZTI2);
	    Interpolation(List,
			  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			  XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			  e,U1,U2,rg,
			  Nod11Indices,Nod11RValues,
			  Nod12Indices,Nod12RValues,
			  i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			  Nod21Indices,Nod21RValues,
			  Nod22Indices,Nod22RValues,
			  i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2);
	  }
	}
      }
#ifdef OCCT_DEBUG
      else if (DoError) {
	cout << "HLRBRep_PolyAlgo::InitBiPointsWithConnexity : Edge ";
	cout << e << " connect 2 without PolygonOnTriangulation" << endl;
      }
#endif
    }
  }
  else {  // no connexity
    const Handle(Poly_Polygon3D)& Polyg = BRep_Tool::Polygon3D(E,L);
    if (!Polyg.IsNull()) {
      const TColgp_Array1OfPnt& Pol = Polyg->Nodes();
      gp_Trsf TT       = L.Transformation();
      const gp_Trsf& T = myProj.Transformation();
      TT.PreMultiply(T);
      const gp_XYZ& ttlo = TT.TranslationPart();
      TTLo[0] = ttlo.X();
      TTLo[1] = ttlo.Y();
      TTLo[2] = ttlo.Z();
      const gp_Mat& ttma = TT.VectorialPart();
      TTMa[0][0] = ttma.Value(1,1);
      TTMa[0][1] = ttma.Value(1,2);
      TTMa[0][2] = ttma.Value(1,3);
      TTMa[1][0] = ttma.Value(2,1);
      TTMa[1][1] = ttma.Value(2,2);
      TTMa[1][2] = ttma.Value(2,3);
      TTMa[2][0] = ttma.Value(3,1);
      TTMa[2][1] = ttma.Value(3,2);
      TTMa[2][2] = ttma.Value(3,3);
      Standard_Integer nbPol1 = Pol.Upper();
      const gp_XYZ& P1 = Pol(1).XYZ();
      X2 = P1.X();
      Y2 = P1.Y();
      Z2 = P1.Z();
      TTMultiply(X2,Y2,Z2);
      XTI2 = X2;
      YTI2 = Y2;
      ZTI2 = Z2;
      TIMultiply(XTI2,YTI2,ZTI2);
      
      for (Standard_Integer jPol = 2; jPol <= nbPol1; jPol++) {
	X1   = X2;
	Y1   = Y2;
	Z1   = Z2;
	XTI1 = XTI2;
	YTI1 = YTI2;
	ZTI1 = ZTI2;
	const gp_XYZ& P2 = Pol(jPol).XYZ();
	X2 = P2.X();
	Y2 = P2.Y();
	Z2 = P2.Z();
	TTMultiply(X2,Y2,Z2);
	XTI2 = X2;
	YTI2 = Y2;
	ZTI2 = Z2;
	TIMultiply(XTI2,YTI2,ZTI2);
	List.Prepend(HLRAlgo_BiPoint
		     (XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		      X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,   e,
		      0));
      }
    }
#ifdef OCCT_DEBUG
    else if (DoError) {
      cout << "HLRBRep_PolyAlgo::InitBiPointsWithConnexity : Edge ";
      cout << e << " Isolated, without Polygone 3D" << endl;
    }
#endif
  }
}

//=======================================================================
//function : Interpolation
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
Interpolation (HLRAlgo_ListOfBPoint& List,
	       Standard_Real& X1,
	       Standard_Real& Y1,
	       Standard_Real& Z1,
	       Standard_Real& X2,
	       Standard_Real& Y2,
	       Standard_Real& Z2,
	       Standard_Real& XTI1,
	       Standard_Real& YTI1,
	       Standard_Real& ZTI1,
	       Standard_Real& XTI2,
	       Standard_Real& YTI2,
	       Standard_Real& ZTI2,
	       const Standard_Integer e,
	       Standard_Real& U1,
	       Standard_Real& U2,
	       Standard_Address& Nod11Indices,
	       Standard_Address& Nod11RValues,
	       Standard_Address& Nod12Indices,
	       Standard_Address& Nod12RValues,
	       const Standard_Integer i1p1,
	       const Standard_Integer i1p2,
	       const Standard_Integer i1,
	       const Handle(HLRAlgo_PolyInternalData)& pid1,
	       Standard_Address& TData1,
	       Standard_Address& PISeg1,
	       Standard_Address& PINod1) const
{
  Standard_Boolean insP3,mP3P1;
  Standard_Real X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3;
//  gp_Pnt P3,PT3;
  insP3 = Interpolation(U1,U2,Nod11RValues,Nod12RValues,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,mP3P1);
  MoveOrInsertPoint(List,
		    X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
		    XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		    e,U1,U2,
		    Nod11Indices,Nod11RValues,
		    Nod12Indices,Nod12RValues,
		    i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
		    X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,insP3,mP3P1,0);
}

//=======================================================================
//function : Interpolation
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
Interpolation (HLRAlgo_ListOfBPoint& List,
	       Standard_Real& X1,
	       Standard_Real& Y1,
	       Standard_Real& Z1,
	       Standard_Real& X2,
	       Standard_Real& Y2,
	       Standard_Real& Z2,
	       Standard_Real& XTI1,
	       Standard_Real& YTI1,
	       Standard_Real& ZTI1,
	       Standard_Real& XTI2,
	       Standard_Real& YTI2,
	       Standard_Real& ZTI2,
	       const Standard_Integer e,
	       Standard_Real& U1,
	       Standard_Real& U2,
	       const GeomAbs_Shape rg,
	       Standard_Address& Nod11Indices,
	       Standard_Address& Nod11RValues,
	       Standard_Address& Nod12Indices,
	       Standard_Address& Nod12RValues,
	       const Standard_Integer i1p1,
	       const Standard_Integer i1p2,
	       const Standard_Integer i1,
	       const Handle(HLRAlgo_PolyInternalData)& pid1,
	       Standard_Address& TData1,
	       Standard_Address& PISeg1,
	       Standard_Address& PINod1,
	       Standard_Address& Nod21Indices,
	       Standard_Address& Nod21RValues,
	       Standard_Address& Nod22Indices,
	       Standard_Address& Nod22RValues,
	       const Standard_Integer i2p1,
	       const Standard_Integer i2p2,
	       const Standard_Integer i2,
	       const Handle(HLRAlgo_PolyInternalData)& pid2,
	       Standard_Address& TData2,
	       Standard_Address& PISeg2,
	       Standard_Address& PINod2) const
{
  Standard_Boolean insP3,mP3P1,insP4,mP4P1;
  Standard_Real X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3;
  Standard_Real X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4;
//  gp_Pnt P3,PT3,P4,PT4;
  Standard_Boolean flag = 0;
  if (rg >= GeomAbs_G1) flag += 1;
  if (rg >= GeomAbs_G2) flag += 2;
  insP3 = Interpolation(U1,U2,Nod11RValues,Nod12RValues,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,mP3P1);
  insP4 = Interpolation(U1,U2,Nod21RValues,Nod22RValues,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,mP4P1);
  Standard_Boolean OK = insP3 || insP4;
  if (OK) {
    if      (!insP4)                               // p1 i1p3 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,insP3,mP3P1,flag);
    else if (!insP3)                               // p1 i2p4 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,insP4,mP4P1,flag);
    else if (Abs(coef4 - coef3) < myTolSta)       // p1 i1p3-i2p4 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,insP4,mP4P1,flag);
    else if (coef4 < coef3)                        // p1 i2p4 i1p3 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,insP4,mP4P1,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,insP3,mP3P1,flag);
    else                                           // p1 i1p3 i2p4 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,insP3,mP3P1,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,insP4,mP4P1,flag);
  }
  else                                             // p1 p2
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p1,i1p2,i2  ,i2p1,i2p2,flag));
}

//=======================================================================
//function : Interpolation
//purpose  : 
//=======================================================================

Standard_Boolean
HLRBRep_PolyAlgo::
Interpolation (const Standard_Real U1,
	       const Standard_Real U2,
	       const Standard_Address Nod1RValues,
	       const Standard_Address Nod2RValues,
	       Standard_Real& X3,
	       Standard_Real& Y3,
	       Standard_Real& Z3,
	       Standard_Real& XTI3,
	       Standard_Real& YTI3,
	       Standard_Real& ZTI3,
	       Standard_Real& coef3,
	       Standard_Real& U3,
	       Standard_Boolean& mP3P1) const
{
  if (NewNode(Nod1RValues,Nod2RValues,coef3,mP3P1)) {
    U3 = U1 + (U2 - U1) * coef3;
    const gp_Pnt& P3 = myBCurv.Value(U3);
    XTI3 = X3 = P3.X();
    YTI3 = Y3 = P3.Y();
    ZTI3 = Z3 = P3.Z();
    TMultiply(X3,Y3,Z3);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : MoveOrInsertPoint
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
MoveOrInsertPoint (HLRAlgo_ListOfBPoint& List,
		   Standard_Real& X1,
		   Standard_Real& Y1,
		   Standard_Real& Z1,
		   Standard_Real& X2,
		   Standard_Real& Y2,
		   Standard_Real& Z2,
		   Standard_Real& XTI1,
		   Standard_Real& YTI1,
		   Standard_Real& ZTI1,
		   Standard_Real& XTI2,
		   Standard_Real& YTI2,
		   Standard_Real& ZTI2,
		   const Standard_Integer e,
		   Standard_Real& U1,
		   Standard_Real& U2,
		   Standard_Address& Nod11Indices,
		   Standard_Address& Nod11RValues,
		   Standard_Address& Nod12Indices,
		   Standard_Address& Nod12RValues,
		   const Standard_Integer i1p1,
		   const Standard_Integer i1p2,
		   const Standard_Integer i1,
		   const Handle(HLRAlgo_PolyInternalData)& pid1,
		   Standard_Address& TData1,
		   Standard_Address& PISeg1,
		   Standard_Address& PINod1,
		   const Standard_Real X3,
		   const Standard_Real Y3,
		   const Standard_Real Z3,
		   const Standard_Real XTI3,
		   const Standard_Real YTI3,
		   const Standard_Real ZTI3,
		   const Standard_Real coef3,
		   const Standard_Real U3,
		   const Standard_Boolean insP3,
		   const Standard_Boolean mP3P1,
		   const Standard_Boolean flag) const
{
  Standard_Address TData2 = 0;
  Standard_Address PISeg2 = 0;
  Standard_Address PINod2 = 0;
  Standard_Boolean ins3 = insP3;
  if (ins3 && mP3P1) {                             // P1 ---> P3
    if (!(Nod11Flag & NMskVert) && coef3 < myTolSta) {
      ins3 = Standard_False;
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef3,X3,Y3,Z3,Standard_True,
		 TData1,PISeg1,PINod1);
      X1   = X3;
      Y1   = Y3;
      Z1   = Z3;
      XTI1 = XTI3;
      YTI1 = YTI3;
      ZTI1 = ZTI3;
      U1   = U3;
      Nod11PntX = X3;
      Nod11PntY = Y3;
      Nod11PntZ = Z3;
      if      (Nod11Edg1 == (Standard_Boolean) e) Nod11PCu1 = U3;
      else if (Nod11Edg2 == (Standard_Boolean) e) Nod11PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p1 << endl;
      }
#endif
      Nod11Scal  = 0;
      Nod11Flag |= NMskOutL;
      UpdateAroundNode(i1p1,Nod11Indices,TData1,PISeg1,PINod1);
      Standard_Address Coordinates = List.First().Coordinates();
      PntX2   = X3;
      PntY2   = Y3;
      PntZ2   = Z3;
      PntXTI2 = XTI3;
      PntYTI2 = YTI3;
      PntZTI2 = ZTI3;
    }
  }
  if (ins3 && !mP3P1) {                            // P2 ---> P3
    if (!(Nod12Flag & NMskVert) && coef3 > myTolEnd) {
      ins3 = Standard_False;
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef3,X3,Y3,Z3,Standard_False,
		 TData1,PISeg1,PINod1);
      X2   = X3;
      Y2   = Y3;
      Z2   = Z3;
      XTI2 = XTI3;
      YTI2 = YTI3;
      ZTI2 = ZTI3;
      U2   = U3;
      Nod12PntX = X3;
      Nod12PntY = Y3;
      Nod12PntZ = Z3;
      if      (Nod12Edg1 == (Standard_Boolean) e) Nod12PCu1 = U3;
      else if (Nod12Edg2 == (Standard_Boolean) e) Nod12PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p2 << endl;
      }
#endif
      Nod12Scal  = 0;
      Nod12Flag |= NMskOutL;
      UpdateAroundNode(i1p2,Nod12Indices,TData1,PISeg1,PINod1);
    }
  }
  if (ins3) {                                      // p1 i1p3 p2
    Standard_Integer i1p3 = pid1->AddNode
      (Nod11RValues,Nod12RValues,PINod1,PINod2,coef3,X3,Y3,Z3);
    const Handle(HLRAlgo_PolyInternalNode)* pi1p3 =
      &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(i1p3));
    const Standard_Address Nod13Indices = (*pi1p3)->Indices();
    const Standard_Address Nod13RValues = (*pi1p3)->RValues();
    Nod13Edg1  = e;
    Nod13PCu1  = U3;
    Nod13Scal  = 0;
    Nod13Flag |= NMskOutL;
    Nod13Flag |= NMskEdge;
    pid1->UpdateLinks(i1p1,i1p2,i1p3,
		      TData1,TData2,PISeg1,PISeg2,PINod1,PINod2);
    UpdateAroundNode(i1p3,Nod13Indices,TData1,PISeg1,PINod1);
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI3,YTI3,ZTI3,
		  X1  ,Y1  ,Z1  ,X3  ,Y3  ,Z3  ,   e,
		  i1  ,i1p1,i1p3,flag));
    List.Prepend(HLRAlgo_BiPoint
		 (XTI3,YTI3,ZTI3,XTI2,YTI2,ZTI2,
		  X3  ,Y3  ,Z3  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p3,i1p2,flag));
  }
  else                                             // p1 p2
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p1,i1p2,flag));
}

//=======================================================================
//function : MoveOrInsertPoint
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
MoveOrInsertPoint (HLRAlgo_ListOfBPoint& List,
		   Standard_Real& X1,
		   Standard_Real& Y1,
		   Standard_Real& Z1,
		   Standard_Real& X2,
		   Standard_Real& Y2,
		   Standard_Real& Z2,
		   Standard_Real& XTI1,
		   Standard_Real& YTI1,
		   Standard_Real& ZTI1,
		   Standard_Real& XTI2,
		   Standard_Real& YTI2,
		   Standard_Real& ZTI2,
		   const Standard_Integer e,
		   Standard_Real& U1,
		   Standard_Real& U2,
		   Standard_Address& Nod11Indices,
		   Standard_Address& Nod11RValues,
		   Standard_Address& Nod12Indices,
		   Standard_Address& Nod12RValues,
		   const Standard_Integer i1p1,
		   const Standard_Integer i1p2,
		   const Standard_Integer i1,
		   const Handle(HLRAlgo_PolyInternalData)& pid1,
		   Standard_Address& TData1,
		   Standard_Address& PISeg1,
		   Standard_Address& PINod1,
		   Standard_Address& Nod21Indices,
		   Standard_Address& Nod21RValues,
		   Standard_Address& Nod22Indices,
		   Standard_Address& Nod22RValues,
		   const Standard_Integer i2p1,
		   const Standard_Integer i2p2,
		   const Standard_Integer i2,
		   const Handle(HLRAlgo_PolyInternalData)& pid2,
		   Standard_Address& TData2,
		   Standard_Address& PISeg2,
		   Standard_Address& PINod2,
		   const Standard_Real X3,
		   const Standard_Real Y3,
		   const Standard_Real Z3,
		   const Standard_Real XTI3,
		   const Standard_Real YTI3,
		   const Standard_Real ZTI3,
		   const Standard_Real coef3,
		   const Standard_Real U3,
		   const Standard_Boolean insP3,
		   const Standard_Boolean mP3P1,
		   const Standard_Boolean flag) const
{
  Standard_Boolean ins3 = insP3;
  if (ins3 && mP3P1) {                             // P1 ---> P3
    if (!(Nod11Flag & NMskVert) && coef3 < myTolSta) {
      ins3 = Standard_False;
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef3,X3,Y3,Z3,Standard_True,
		 TData1,PISeg1,PINod1);
      ChangeNode(i2p1,i2p2,
		 Nod21Indices,Nod21RValues,
		 Nod22Indices,Nod22RValues,
		 coef3,X3,Y3,Z3,Standard_True,
		 TData2,PISeg2,PINod2);
      X1   = X3;
      Y1   = Y3;
      Z1   = Z3;
      XTI1 = XTI3;
      YTI1 = YTI3;
      ZTI1 = ZTI3;
      U1   = U3;
      Nod11PntX = X3;
      Nod11PntY = Y3;
      Nod11PntZ = Z3;
      if      (Nod11Edg1 == (Standard_Boolean) e) Nod11PCu1 = U3;
      else if (Nod11Edg2 == (Standard_Boolean) e) Nod11PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p1 << endl;
      }
#endif
      Nod11Scal  = 0;
      Nod11Flag |= NMskOutL;
      UpdateAroundNode(i1p1,Nod11Indices,TData1,PISeg1,PINod1);
      Nod21PntX  = X3;
      Nod21PntY  = Y3;
      Nod21PntZ  = Z3;
      if      (Nod21Edg1 == (Standard_Boolean) e) Nod21PCu1 = U3;
      else if (Nod21Edg2 == (Standard_Boolean) e) Nod21PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i2p1 << endl;
      }
#endif
      Nod21Scal  = 0;
      Nod21Flag |= NMskOutL;
      UpdateAroundNode(i2p1,Nod21Indices,TData2,PISeg2,PINod2);
      Standard_Address Coordinates = List.First().Coordinates();
      PntX2   = X3;
      PntY2   = Y3;
      PntZ2   = Z3;
      PntXTI2 = XTI3;
      PntYTI2 = YTI3;
      PntZTI2 = ZTI3;
    }
  }
  if (ins3 && !mP3P1) {                            // P2 ---> P3
    if (!(Nod12Flag & NMskVert) && coef3 > myTolEnd) {
      ins3 = Standard_False;
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef3,X3,Y3,Z3,Standard_False,
		 TData1,PISeg1,PINod1);
      ChangeNode(i2p1,i2p2,
		 Nod21Indices,Nod21RValues,
		 Nod22Indices,Nod22RValues,
		 coef3,X3,Y3,Z3,Standard_False,
		 TData2,PISeg2,PINod2);
      X2   = X3;
      Y2   = Y3;
      Z2   = Z3;
      XTI2 = XTI3;
      YTI2 = YTI3;
      ZTI2 = ZTI3;
      U2   = U3;
      Nod12PntX = X3;
      Nod12PntY = Y3;
      Nod12PntZ = Z3;
      if      (Nod12Edg1 == (Standard_Boolean) e) Nod12PCu1 = U3;
      else if (Nod12Edg2 == (Standard_Boolean) e) Nod12PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p2 << endl;
      }
#endif
      Nod12Scal  = 0;
      Nod12Flag |= NMskOutL;
      UpdateAroundNode(i1p2,Nod12Indices,TData1,PISeg1,PINod1);
      Nod22PntX  = X3;
      Nod22PntY  = Y3;
      Nod22PntZ  = Z3;
      if      (Nod22Edg1 == (Standard_Boolean) e) Nod22PCu1 = U3;
      else if (Nod22Edg2 == (Standard_Boolean) e) Nod22PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i2p2 << endl;
      }
#endif
      Nod22Scal = 0;
      Nod22Flag |=  NMskOutL;
      UpdateAroundNode(i2p2,Nod22Indices,TData2,PISeg2,PINod2);
    }
  }
  if (ins3) {                                      // p1 i1p3 p2
    Standard_Integer i1p3 = pid1->AddNode
      (Nod11RValues,Nod12RValues,PINod1,PINod2,coef3,X3,Y3,Z3);
    Standard_Integer i2p3 = pid2->AddNode
      (Nod21RValues,Nod22RValues,PINod2,PINod1,coef3,X3,Y3,Z3); 
    const Handle(HLRAlgo_PolyInternalNode)* pi1p3 =
      &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(i1p3));
    const Standard_Address Nod13Indices = (*pi1p3)->Indices();
    const Standard_Address Nod13RValues = (*pi1p3)->RValues();
    const Handle(HLRAlgo_PolyInternalNode)* pi2p3 =
      &(((HLRAlgo_Array1OfPINod*)PINod2)->ChangeValue(i2p3));
    const Standard_Address Nod23Indices = (*pi2p3)->Indices();
    const Standard_Address Nod23RValues = (*pi2p3)->RValues();
    Nod13Edg1  = e;
    Nod13PCu1  = U3;
    Nod13Scal  = 0;
    Nod13Flag |= NMskOutL;
    Nod13Flag |= NMskEdge;
    Nod23Edg1  = e;
    Nod23PCu1  = U3;
    Nod23Scal  = 0;
    Nod23Flag |= NMskOutL;
    Nod23Flag |= NMskEdge;
    pid1->UpdateLinks(i1p1,i1p2,i1p3,
		      TData1,TData2,PISeg1,PISeg2,PINod1,PINod2);
    pid2->UpdateLinks(i2p1,i2p2,i2p3,
		      TData2,TData1,PISeg2,PISeg1,PINod2,PINod1);
    UpdateAroundNode(i1p3,Nod13Indices,TData1,PISeg1,PINod1);
    UpdateAroundNode(i2p3,Nod23Indices,TData2,PISeg2,PINod2);
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI3,YTI3,ZTI3,
		  X1  ,Y1  ,Z1  ,X3  ,Y3  ,Z3  ,   e,
		  i1  ,i1p1,i1p3,i2  ,i2p1,i2p3,flag));
    List.Prepend(HLRAlgo_BiPoint
		 (XTI3,YTI3,ZTI3,XTI2,YTI2,ZTI2,
		  X3  ,Y3  ,Z3  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p3,i1p2,i2  ,i2p3,i2p2,flag));
  }
  else                                             // p1 p2
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p1,i1p2,i2  ,i2p1,i2p2,flag));
}

//=======================================================================
//function : MoveOrInsertPoint
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
MoveOrInsertPoint (HLRAlgo_ListOfBPoint& List,
		   Standard_Real& X1,
		   Standard_Real& Y1,
		   Standard_Real& Z1,
		   Standard_Real& X2,
		   Standard_Real& Y2,
		   Standard_Real& Z2,
		   Standard_Real& XTI1,
		   Standard_Real& YTI1,
		   Standard_Real& ZTI1,
		   Standard_Real& XTI2,
		   Standard_Real& YTI2,
		   Standard_Real& ZTI2,
		   const Standard_Integer e,
		   Standard_Real& U1,
		   Standard_Real& U2,
		   Standard_Address& Nod11Indices,
		   Standard_Address& Nod11RValues,
		   Standard_Address& Nod12Indices,
		   Standard_Address& Nod12RValues,
		   const Standard_Integer i1p1,
		   const Standard_Integer i1p2,
		   const Standard_Integer i1,
		   const Handle(HLRAlgo_PolyInternalData)& pid1,
		   Standard_Address& TData1,
		   Standard_Address& PISeg1,
		   Standard_Address& PINod1,
		   Standard_Address& Nod21Indices,
		   Standard_Address& Nod21RValues,
		   Standard_Address& Nod22Indices,
		   Standard_Address& Nod22RValues,
		   const Standard_Integer i2p1,
		   const Standard_Integer i2p2,
		   const Standard_Integer i2,
		   const Handle(HLRAlgo_PolyInternalData)& pid2,
		   Standard_Address& TData2,
		   Standard_Address& PISeg2,
		   Standard_Address& PINod2,
		   const Standard_Real X3,
		   const Standard_Real Y3,
		   const Standard_Real Z3,
		   const Standard_Real XTI3,
		   const Standard_Real YTI3,
		   const Standard_Real ZTI3,
		   const Standard_Real coef3,
		   const Standard_Real U3,
		   const Standard_Boolean insP3,
		   const Standard_Boolean mP3P1,
		   const Standard_Real X4,
		   const Standard_Real Y4,
		   const Standard_Real Z4,
		   const Standard_Real XTI4,
		   const Standard_Real YTI4,
		   const Standard_Real ZTI4,
		   const Standard_Real coef4,
		   const Standard_Real U4,
		   const Standard_Boolean insP4,
		   const Standard_Boolean mP4P1,
		   const Standard_Boolean flag) const
{
  Standard_Boolean ins3 = insP3;
  Standard_Boolean ins4 = insP4;
  if (ins3 && mP3P1) {                             // P1 ---> P3
    if (!(Nod11Flag & NMskVert) && coef3 < myTolSta) {
      ins3 = Standard_False;
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef3,X3,Y3,Z3,Standard_True,
		 TData1,PISeg1,PINod1);
      ChangeNode(i2p1,i2p2,
		 Nod21Indices,Nod21RValues,
		 Nod22Indices,Nod22RValues,
		 coef3,X3,Y3,Z3,Standard_True,
		 TData2,PISeg2,PINod2);
      X1   = X3;
      Y1   = Y3;
      Z1   = Z3;
      XTI1 = XTI3;
      YTI1 = YTI3;
      ZTI1 = ZTI3;
      U1   = U3;
      Nod11PntX = X3;
      Nod11PntY = Y3;
      Nod11PntZ = Z3;
      if      (Nod11Edg1 == (Standard_Boolean) e) Nod11PCu1 = U3;
      else if (Nod11Edg2 == (Standard_Boolean) e) Nod11PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p1 << endl;
      }
#endif
      Nod11Scal  = 0;
      Nod11Flag |= NMskOutL;
      UpdateAroundNode(i1p1,Nod11Indices,TData1,PISeg1,PINod1);
      Nod21PntX  = X3;
      Nod21PntY  = Y3;
      Nod21PntZ  = Z3;
      if      (Nod21Edg1 == (Standard_Boolean) e) Nod21PCu1 = U3;
      else if (Nod21Edg2 == (Standard_Boolean) e) Nod21PCu2 = U3;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i2p1 << endl;
      }
#endif
      Nod21Scal  = 0;
      Nod21Flag |= NMskOutL;
      UpdateAroundNode(i2p1,Nod21Indices,TData2,PISeg2,PINod2);
      Standard_Address Coordinates = List.First().Coordinates();
      PntX2   = X3;
      PntY2   = Y3;
      PntZ2   = Z3;
      PntXTI2 = XTI3;
      PntYTI2 = YTI3;
      PntZTI2 = ZTI3;
    }
  }
  if (ins4 && !mP4P1) {                            // P2 ---> P4
    if (!(Nod12Flag & NMskVert) && coef4 > myTolEnd) {
      ins4 = Standard_False;
      ChangeNode(i2p1,i2p2,
		 Nod21Indices,Nod21RValues,
		 Nod22Indices,Nod22RValues,
		 coef4,X4,Y4,Z4,Standard_False,
		 TData2,PISeg2,PINod2);
      ChangeNode(i1p1,i1p2,
		 Nod11Indices,Nod11RValues,
		 Nod12Indices,Nod12RValues,
		 coef4,X4,Y4,Z4,Standard_False,
		 TData1,PISeg1,PINod1);
      X2   = X4;
      Y2   = Y4;
      Z2   = Z4;
      XTI2 = XTI4;
      YTI2 = YTI4;
      ZTI2 = ZTI4;
      U2   = U4;
      Nod12PntX = X4;
      Nod12PntY = Y4;
      Nod12PntZ = Z4;
      if      (Nod12Edg1 == (Standard_Boolean) e) Nod12PCu1 = U4;
      else if (Nod12Edg2 == (Standard_Boolean) e) Nod12PCu2 = U4;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i1p2 << endl;
      }
#endif
      Nod12Scal  = 0;
      Nod12Flag |= NMskOutL;
      UpdateAroundNode(i1p2,Nod12Indices,TData1,PISeg1,PINod1);
      Nod22PntX  = X4;
      Nod22PntY  = Y4;
      Nod22PntZ  = Z4;
      if      (Nod22Edg1 == (Standard_Boolean) e) Nod22PCu1 = U4;
      else if (Nod22Edg2 == (Standard_Boolean) e) Nod22PCu2 = U4;
#ifdef OCCT_DEBUG
      else {
	cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
	cout << "Parameter error on Node " << i2p2 << endl;
      }
#endif
      Nod22Scal  = 0;
      Nod22Flag |= NMskOutL;
      UpdateAroundNode(i2p2,Nod22Indices,TData2,PISeg2,PINod2);
    }
  }
  if (ins3 || ins4) {
    if      (!ins4)                                // p1 i1p3 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			X3,Y3,Z3,XTI3,YTI3,ZTI3,coef3,U3,insP3,mP3P1,flag);
    else if (!ins3)                                // p1 i2p4 p2
      MoveOrInsertPoint(List,
			X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
			XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
			e,U1,U2,
			Nod21Indices,Nod21RValues,
			Nod22Indices,Nod22RValues,
			i2p1,i2p2,i2,pid2,TData2,PISeg2,PINod2,
			Nod11Indices,Nod11RValues,
			Nod12Indices,Nod12RValues,
			i1p1,i1p2,i1,pid1,TData1,PISeg1,PINod1,
			X4,Y4,Z4,XTI4,YTI4,ZTI4,coef4,U4,insP4,mP4P1,flag);
    else {                                         // p1 i1p3 i2p4 p2
      Standard_Integer i1p3 = pid1->AddNode
	(Nod11RValues,Nod12RValues,PINod1,PINod2,coef3,X3,Y3,Z3);
      Standard_Integer i2p3 = pid2->AddNode
	(Nod21RValues,Nod22RValues,PINod2,PINod1,coef3,X3,Y3,Z3);
      Standard_Integer i1p4 = pid1->AddNode
	(Nod11RValues,Nod12RValues,PINod1,PINod2,coef4,X4,Y4,Z4);
      Standard_Integer i2p4 = pid2->AddNode
	(Nod21RValues,Nod22RValues,PINod2,PINod1,coef4,X4,Y4,Z4);
      const Handle(HLRAlgo_PolyInternalNode)* pi1p3 =
	&(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(i1p3));
      const Standard_Address Nod13Indices = (*pi1p3)->Indices();
      const Standard_Address Nod13RValues = (*pi1p3)->RValues();
      const Handle(HLRAlgo_PolyInternalNode)* pi1p4 =
	&(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(i1p4));
      const Standard_Address Nod14Indices = (*pi1p4)->Indices();
      const Standard_Address Nod14RValues = (*pi1p4)->RValues();
      const Handle(HLRAlgo_PolyInternalNode)* pi2p3 =
	&(((HLRAlgo_Array1OfPINod*)PINod2)->ChangeValue(i2p3));
      const Standard_Address Nod23Indices = (*pi2p3)->Indices();
      const Standard_Address Nod23RValues = (*pi2p3)->RValues();
      const Handle(HLRAlgo_PolyInternalNode)* pi2p4 = 
	&(((HLRAlgo_Array1OfPINod*)PINod2)->ChangeValue(i2p4));
      const Standard_Address Nod24Indices = (*pi2p4)->Indices();
      const Standard_Address Nod24RValues = (*pi2p4)->RValues();
      Nod13Edg1  = e;
      Nod13PCu1  = U3;
      Nod13Scal  = 0;
      Nod13Flag |= NMskOutL;
      Nod13Flag |= NMskEdge;
      Nod23Edg1  = e;
      Nod23PCu1  = U3;
      Nod23Scal  = 0;
      Nod23Flag |= NMskOutL;
      Nod23Flag |= NMskEdge;
      Nod14Edg1  = e;
      Nod14PCu1  = U4;
      Nod14Scal  = 0;
      Nod14Flag |= NMskOutL;
      Nod14Flag |= NMskEdge;
      Nod24Edg1  = e;
      Nod24PCu1  = U4;
      Nod24Scal  = 0;
      Nod24Flag |= NMskOutL;
      Nod24Flag |= NMskEdge;
      pid1->UpdateLinks(i1p1,i1p2,i1p3,
			TData1,TData2,PISeg1,PISeg2,PINod1,PINod2);
      pid2->UpdateLinks(i2p1,i2p2,i2p3,
			TData2,TData1,PISeg2,PISeg1,PINod2,PINod1);
      pid2->UpdateLinks(i2p3,i2p2,i2p4,
			TData2,TData1,PISeg2,PISeg1,PINod2,PINod1);
      pid1->UpdateLinks(i1p3,i1p2,i1p4,
			TData1,TData2,PISeg1,PISeg2,PINod1,PINod2);
      UpdateAroundNode(i1p3,Nod13Indices,TData1,PISeg1,PINod1);
      UpdateAroundNode(i2p3,Nod23Indices,TData2,PISeg2,PINod2);
      UpdateAroundNode(i1p4,Nod14Indices,TData1,PISeg1,PINod1);
      UpdateAroundNode(i2p4,Nod24Indices,TData2,PISeg2,PINod2);
      List.Prepend(HLRAlgo_BiPoint
		   (XTI1,YTI1,ZTI1,XTI3,YTI3,ZTI3,
		    X1  ,Y1  ,Z1  ,X3  ,Y3  ,Z3  ,   e,
		    i1  ,i1p1,i1p3,i2  ,i2p1,i2p3,flag));
      List.Prepend(HLRAlgo_BiPoint
		   (XTI3,YTI3,ZTI3,XTI4,YTI4,ZTI4,
		    X3  ,Y3  ,Z3  ,X4  ,Y4  ,Z4  ,   e,
		    i1  ,i1p3,i1p4,i2  ,i2p3,i2p4,flag));
      List.Prepend(HLRAlgo_BiPoint
		   (XTI4,YTI4,ZTI4,XTI2,YTI2,ZTI2,
		    X4  ,Y4  ,Z4  ,X2  ,Y2  ,Z2  ,   e,
		    i1  ,i1p4,i1p2,i2  ,i2p4,i2p2,flag));
    }
  }
  else                                             // p1 p2
    List.Prepend(HLRAlgo_BiPoint
		 (XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
		  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,   e,
		  i1  ,i1p1,i1p2,i2  ,i2p1,i2p2,flag));
}

//=======================================================================
//function : InsertOnOutLine
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::InsertOnOutLine (TColStd_Array1OfTransient& PID)
{
  Standard_Address TData2 = 0;
  Standard_Address PISeg2 = 0;
  Standard_Address PINod2 = 0;
  Standard_Address Seg1Indices;
  Standard_Address Nod1Indices,Nod1RValues;
  Standard_Address Nod2Indices,Nod2RValues;
  Handle(HLRAlgo_PolyInternalData)* pid = 
    (Handle(HLRAlgo_PolyInternalData)*) (&(PID.ChangeValue(1)));

  TopLoc_Location L;
  Standard_Boolean insP3,mP3P1,IntOutL;
  Standard_Integer f,ip1,ip2,ip3;//, i;
  Standard_Real U3,V3,coef3,X3 = 0.,Y3 = 0.,Z3 = 0.;

  const gp_Trsf& T  = myProj.Transformation();
  
  Standard_Integer nbFace = myFMap.Extent();
  for (f = 1; f <= nbFace; f++) {

    if (!((*pid).IsNull())) {
      IntOutL = Standard_False;
      Standard_Address TData1= &((*pid)->TData());
      Standard_Address PISeg1= &((*pid)->PISeg());
      Standard_Address PINod1= &((*pid)->PINod());
      TopoDS_Shape LocalShape = myFMap(f);
      const TopoDS_Face& F = TopoDS::Face(LocalShape);
      myBSurf.Initialize(F,Standard_False);
      myGSurf = BRep_Tool::Surface(F,L);
      gp_Trsf TT = L.Transformation();
      TT.PreMultiply(T);
      const gp_XYZ& ttlo = TT.TranslationPart();
      TTLo[0] = ttlo.X();
      TTLo[1] = ttlo.Y();
      TTLo[2] = ttlo.Z();
      const gp_Mat& ttma = TT.VectorialPart();
      TTMa[0][0] = ttma.Value(1,1);
      TTMa[0][1] = ttma.Value(1,2);
      TTMa[0][2] = ttma.Value(1,3);
      TTMa[1][0] = ttma.Value(2,1);
      TTMa[1][1] = ttma.Value(2,2);
      TTMa[1][2] = ttma.Value(2,3);
      TTMa[2][0] = ttma.Value(3,1);
      TTMa[2][1] = ttma.Value(3,2);
      TTMa[2][2] = ttma.Value(3,3);

#ifdef OCCT_DEBUG
      if (DoTrace) {
	cout << " InsertOnOutLine : NbTData " << (*pid)->NbTData() << endl;
	cout << " InsertOnOutLine : NbPISeg " << (*pid)->NbPISeg() << endl;
	cout << " InsertOnOutLine : NbPINod " << (*pid)->NbPINod() << endl;
      }
#endif

      Standard_Integer iseg,nbS;
      nbS = (*pid)->NbPISeg();
      for (iseg = 1; iseg <= nbS; iseg++) {
	Seg1Indices =
	  ((HLRAlgo_Array1OfPISeg*)PISeg1)->ChangeValue(iseg).Indices();
//	Standard_Boolean Cutted = Standard_False;
	if (Seg1Conex1 != 0 && Seg1Conex2 != 0) {
	  ip1 = Seg1LstSg1;
	  ip2 = Seg1LstSg2;
	  const Handle(HLRAlgo_PolyInternalNode)* pip1 =
	    &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(ip1));
	  Nod1Indices = (*pip1)->Indices();
	  Nod1RValues = (*pip1)->RValues();
	  const Handle(HLRAlgo_PolyInternalNode)* pip2 =
	    &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(ip2));
	  Nod2Indices = (*pip2)->Indices();
	  Nod2RValues = (*pip2)->RValues();
	  if (Nod1Flag & NMskOutL && Nod2Flag & NMskOutL)
	    IntOutL = Standard_True;
	  else if ((Nod1Scal >=  myTolAngular &&
		    Nod2Scal <= -myTolAngular) ||
		   (Nod2Scal >=  myTolAngular &&
		    Nod1Scal <= -myTolAngular)) {
	    IntOutL = Standard_True;
	    insP3 = NewNode(Nod1RValues,Nod2RValues,coef3,mP3P1);
	    if (insP3) {
	      UVNode(Nod1RValues,Nod2RValues,coef3,U3,V3);
	      const gp_Pnt& PT3 = myGSurf->Value(U3,V3);
	      X3 = PT3.X();
	      Y3 = PT3.Y();
	      Z3 = PT3.Z();
	      TTMultiply(X3,Y3,Z3);
	    }
	    
	    if (insP3 && mP3P1) {                        // P1 ---> P3
	      if ((Nod1Flag & NMskEdge) == 0 && coef3 < myTolSta) {
		insP3 = Standard_False;
		ChangeNode(ip1,ip2,
			   Nod1Indices,Nod1RValues,
			   Nod2Indices,Nod2RValues,
			   coef3,X3,Y3,Z3,Standard_True,
			   TData1,PISeg1,PINod1);
		Nod1Scal  = 0;
		Nod1Flag |= NMskOutL;
	      }
	    }
	    if (insP3 && !mP3P1) {                       // P2 ---> P3
	      if ((Nod2Flag & NMskEdge) == 0 && coef3 > myTolEnd) {
		insP3 = Standard_False;
		ChangeNode(ip1,ip2,
			   Nod1Indices,Nod1RValues,
			   Nod2Indices,Nod2RValues,
			   coef3,X3,Y3,Z3,Standard_False,
			   TData1,PISeg1,PINod1);
		Nod2Scal  = 0;
		Nod2Flag |= NMskOutL;
	      }
	    }
	    if (insP3) {                                 // p1 ip3 p2
	      ip3 = (*pid)->AddNode(Nod1RValues,Nod2RValues,PINod1,PINod2,
				    coef3,X3,Y3,Z3);
	      const Handle(HLRAlgo_PolyInternalNode)* pip3 =
		(&((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(ip3));
	      const Standard_Address Nod3Indices = (*pip3)->Indices();
	      const Standard_Address Nod3RValues = (*pip3)->RValues();
	      (*pid)->UpdateLinks(ip1,ip2,ip3,
				  TData1,TData2,PISeg1,PISeg2,PINod1,PINod2);
	      UpdateAroundNode(ip3,Nod3Indices,TData1,PISeg1,PINod1);
	      Nod3Scal  = 0;
	      Nod3Flag |= NMskOutL;
	    }
	  }
	}
      }
      if (IntOutL)
	(*pid)->IntOutL(Standard_True);

      nbS = (*pid)->NbPISeg();

#ifdef OCCT_DEBUG
      if (DoTrace) {
	cout << " InsertOnOutLine : NbTData " << (*pid)->NbTData() << endl;
	cout << " InsertOnOutLine : NbPISeg " << (*pid)->NbPISeg() << endl;
	cout << " InsertOnOutLine : NbPINod " << (*pid)->NbPINod() << endl;
      }
#endif
    }
    pid++;
  }
}

//=======================================================================
//function : CheckFrBackTriangles
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::CheckFrBackTriangles (HLRAlgo_ListOfBPoint& List,
					TColStd_Array1OfTransient& PID)
{
  Standard_Integer f,i,nbN,nbT,nbFace;
  Standard_Real X1 =0.,Y1 =0.,X2 =0.,Y2 =0.,X3 =0.,Y3 =0.;
  Standard_Real D1,D2,D3;
  Standard_Real dd,dX,dY,nX,nY;
  Standard_Boolean FrBackInList;
  Standard_Address TData ,PISeg ,PINod ;
/*  Standard_Address IndexPtr = NULL; 
  const Handle(HLRAlgo_PolyInternalData)& pid1 =
    *(Handle(HLRAlgo_PolyInternalData)*)&(PID(F1Index));
  Standard_Address TData1 = &pid1->TData(),
  PISeg1 = &pid1->PISeg(),
  PINod1 = &pid1->PINod();

  const Handle(HLRAlgo_PolyInternalData)& pid2 =
    *(Handle(HLRAlgo_PolyInternalData)*)&(PID(F2Index));
  Standard_Address TData2 = &pid2->TData(),
  PISeg2 = &pid2->PISeg(),
  PINod2 = &pid2->PISeg();*/
  Standard_Address TData1 = NULL,PISeg1 = NULL,PINod1 = NULL;
  Standard_Address TData2 = NULL,PISeg2 = NULL,PINod2 = NULL;
  Standard_Address Nod11Indices,Nod12Indices,Nod13Indices;
  Standard_Address Nod11RValues,Nod12RValues,Nod13RValues;
  Standard_Address Tri1Indices;

  Handle(HLRAlgo_PolyInternalData)* pid;

  nbFace = myFMap.Extent();
  Standard_Boolean Modif = Standard_True;
  Standard_Integer iLoop = 0;
  
  while (Modif && iLoop < 4) {
    iLoop++;
    Modif        = Standard_False;
    FrBackInList = Standard_False;
    pid = (Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));
    
    for (f = 1; f <= nbFace; f++) {
      if (!(*pid).IsNull()) {
	nbT   =  (*pid)->NbTData();
	TData = &(*pid)->TData();
	PISeg = &(*pid)->PISeg();
	PINod = &(*pid)->PINod();
	HLRAlgo_TriangleData* tdata =
	  &(((HLRAlgo_Array1OfTData*)TData)->ChangeValue(1));
	
	for (i = 1; i <= nbT; i++) {
	  Tri1Indices = tdata->Indices();
	  if ((Tri1Flags & FMskSide) == 0 &&
	      (Tri1Flags & FMskFrBack)) {
#ifdef OCCT_DEBUG
	    if (DoTrace)
	      cout << " face : " << f << " , triangle " << i << endl;
#endif
	    Modif        = Standard_True;
	    const Handle(HLRAlgo_PolyInternalNode)* pi1p1 =
	      &(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri1Node1));
	    Nod11Indices = (*pi1p1)->Indices();
	    Nod11RValues = (*pi1p1)->RValues();
	    const Handle(HLRAlgo_PolyInternalNode)* pi1p2 =
	      &(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri1Node2));
	    Nod12Indices = (*pi1p2)->Indices();
	    Nod12RValues = (*pi1p2)->RValues();
	    const Handle(HLRAlgo_PolyInternalNode)* pi1p3 =
	      &(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri1Node3));
	    Nod13Indices = (*pi1p3)->Indices();
	    Nod13RValues = (*pi1p3)->RValues();
	    D1 = 0.; D2 = 0.; D3 = 0.;
	    if (((Nod11Flag & NMskEdge) == 0 || iLoop > 1) &&
		((Nod11Flag & NMskOutL) == 0 || iLoop > 1) &&
		((Nod11Flag & NMskVert) == 0)) {
	      dX = Nod13PntX - Nod12PntX; dY = Nod13PntY - Nod12PntY;
	      D1 = dX * dX + dY * dY;
	      D1 = sqrt(D1);
	      nX = - dY / D1; nY =   dX / D1;
	      dX = Nod11PntX - Nod12PntX; dY = Nod11PntY - Nod12PntY;
	      dd = - (dX * nX + dY * nY);
	      if (dd < 0) dd -= D1 * 0.01;
	      else        dd += D1 * 0.01;
	      X1 = nX * dd; Y1 = nY * dd;
	    }
	    if (((Nod12Flag & NMskEdge) == 0 || iLoop > 1) &&
		((Nod12Flag & NMskOutL) == 0 || iLoop > 1) &&
		((Nod12Flag & NMskVert) == 0)) {
	      dX = Nod11PntX - Nod13PntX; dY = Nod11PntY - Nod13PntY;
	      D2 = dX * dX + dY * dY;
	      D2 = sqrt(D2);
	      nX = - dY / D2; nY =   dX / D2;
	      dX = Nod12PntX - Nod13PntX; dY = Nod12PntY - Nod13PntY;
	      dd = - (dX * nX + dY * nY);
	      if (dd < 0) dd -= D2 * 0.01;
	      else        dd += D2 * 0.01;
	      X2 = nX * dd; Y2 = nY * dd;
	    }
	    if (((Nod13Flag & NMskEdge) == 0 || iLoop > 1) &&
		((Nod13Flag & NMskOutL) == 0 || iLoop > 1) &&
		((Nod13Flag & NMskVert) == 0)) {
	      dX = Nod12PntX - Nod11PntX; dY = Nod12PntY - Nod11PntY;
	      D3 = dX * dX + dY * dY;
	      D3 = sqrt(D3);
	      nX = - dY / D3; nY =   dX / D3;
	      dX = Nod13PntX - Nod11PntX; dY = Nod13PntY - Nod11PntY;
	      dd = - (dX * nX + dY * nY);
	      if (dd < 0) dd -= D3 * 0.01;
	      else        dd += D3 * 0.01;
	      X3 = nX * dd; Y3 = nY * dd;
	    }
	    if      (D1 > D2 && D1 > D3) {
	      Nod11PntX += X1; Nod11PntY += Y1;
	      Nod11Flag |= NMskMove;
	      UpdateAroundNode(Tri1Node1,Nod11Indices,TData,PISeg,PINod);
	      FrBackInList = Standard_True;
#ifdef OCCT_DEBUG
	      if (DoTrace) {
		cout << Tri1Node1 << " modifies  : DX,DY ";
		cout << X1 << " , " << Y1 << endl;
	      }
#endif
	    }
	    else if (D2 > D3 && D2 > D1) {
	      Nod12PntX += X2; Nod12PntY += Y2;
	      Nod12Flag |= NMskMove;
	      UpdateAroundNode(Tri1Node2,Nod12Indices,TData,PISeg,PINod);
	      FrBackInList = Standard_True;
#ifdef OCCT_DEBUG
	      if (DoTrace) {
		cout << Tri1Node2 << " modifies  : DX,DY ";
		cout << X2 << " , " << Y2 << endl;
	      }
#endif
	    }
	    else if (D3 > D1 && D3 > D2) {
	      Nod13PntX += X3; Nod13PntY += Y3;
	      Nod13Flag |= NMskMove;
	      UpdateAroundNode(Tri1Node3,Nod13Indices,TData,PISeg,PINod);
	      FrBackInList = Standard_True;
#ifdef OCCT_DEBUG
	      if (DoTrace) {
		cout << Tri1Node3 << " modifies  : DX,DY ";
		cout << X3 << " , " << Y3 << endl;
	      }
#endif
	    }
#ifdef OCCT_DEBUG
	    else if (DoTrace)
	      cout << "modification error" << endl;
#endif
	  }
	  tdata++;
	}
      }
      pid++;
    }
    if (FrBackInList) {
      Standard_Address IndexPtr,Coordinates;
      HLRAlgo_ListIteratorOfListOfBPoint it;
      
      for (it.Initialize(List); it.More(); it.Next()) {      
	HLRAlgo_BiPoint& BP = it.Value();
	IndexPtr = BP.Indices();
	if (F1Index != 0) {
	  const Handle(HLRAlgo_PolyInternalData)& pid1 =
	    *(Handle(HLRAlgo_PolyInternalData)*)&(PID(F1Index));
	  TData1 = &pid1->TData();
	  PISeg1 = &pid1->PISeg();
	  PINod1 = &pid1->PINod();
	}
	if (F2Index != 0) {
	  if (F1Index == F2Index) {
	    TData2 = TData1;
	    PISeg2 = PISeg1;
	    PINod2 = PINod1;
	  }
	  else {
	    const Handle(HLRAlgo_PolyInternalData)& pid2 =
	      *(Handle(HLRAlgo_PolyInternalData)*)&(PID(F2Index));
	    TData2 = &pid2->TData();
	    PISeg2 = &pid2->PISeg();
	    PINod2 = &pid2->PINod();
	  }
	}
	if (F1Index != 0) {
	  Nod11Indices = (((HLRAlgo_Array1OfPINod*)PINod1)->
			  ChangeValue(F1Pt1Index))->Indices();
	  if (Nod11Flag & NMskMove) {
#ifdef OCCT_DEBUG
	    if (DoTrace)
	      cout << F1Pt1Index << " modifies 11" << endl;
#endif
	    Nod11RValues = (((HLRAlgo_Array1OfPINod*)PINod1)->
			    ChangeValue(F1Pt1Index))->RValues();
	    Coordinates = BP.Coordinates();
	    PntXTI1 = PntX1 = Nod11PntX;
	    PntYTI1 = PntY1 = Nod11PntY;
	    PntZTI1 = PntZ1 = Nod11PntZ;
	    TIMultiply(PntXTI1,PntYTI1,PntZTI1);
	    if (F2Index != 0) {
	      Nod12Indices = (((HLRAlgo_Array1OfPINod*)PINod2)->
			      ChangeValue(F2Pt1Index))->Indices();
	      Nod12RValues = (((HLRAlgo_Array1OfPINod*)PINod2)->
			      ChangeValue(F2Pt1Index))->RValues();
	      Nod12PntX    = Nod11PntX;
	      Nod12PntY    = Nod11PntY;
	      UpdateAroundNode(F2Pt1Index,Nod12Indices,
			       TData2,PISeg2,PINod2);
	    }
	  }
	  Nod11Indices = (((HLRAlgo_Array1OfPINod*)PINod1)->
			  ChangeValue(F1Pt2Index))->Indices();
	  if (Nod11Flag & NMskMove) {
#ifdef OCCT_DEBUG
	    if (DoTrace)
	      cout << F1Pt2Index << " modifies 12" << endl;
#endif
	    Nod11RValues = (((HLRAlgo_Array1OfPINod*)PINod1)->
			    ChangeValue(F1Pt2Index))->RValues();
	    Coordinates = BP.Coordinates();
	    PntXTI2 = PntX2 = Nod11PntX;
	    PntYTI2 = PntY2 = Nod11PntY;
	    PntZTI2 = PntZ2 = Nod11PntZ;
	    TIMultiply(PntXTI2,PntYTI2,PntZTI2);
	    if (F2Index != 0) {
	      Nod12Indices = (((HLRAlgo_Array1OfPINod*)PINod2)->
			      ChangeValue(F2Pt2Index))->Indices();
	      Nod12RValues = (((HLRAlgo_Array1OfPINod*)PINod2)->
			      ChangeValue(F2Pt2Index))->RValues();
	      Nod12PntX    = Nod11PntX;
	      Nod12PntY    = Nod11PntY;
	      UpdateAroundNode(F2Pt2Index,Nod12Indices,
			       TData2,PISeg2,PINod2);
	    }
	  }
	}
	if (F2Index != 0) {
	  const Handle(HLRAlgo_PolyInternalData)& pid2 =
	    *(Handle(HLRAlgo_PolyInternalData)*)&(PID(F2Index));
	  PINod2 = &pid2->PINod();
	  Nod11Indices = (((HLRAlgo_Array1OfPINod*)PINod2)->
			  ChangeValue(F2Pt1Index))->Indices();
	  if (Nod11Flag & NMskMove) {
#ifdef OCCT_DEBUG
	    if (DoTrace)
	      cout << F2Pt1Index << " modifies 21" << endl;
#endif
	    Nod11RValues = (((HLRAlgo_Array1OfPINod*)PINod2)->
			    ChangeValue(F2Pt1Index))->RValues();
	    Coordinates = BP.Coordinates();
	    PntXTI1 = PntX1 = Nod11PntX;
	    PntYTI1 = PntY1 = Nod11PntY;
	    PntZTI1 = PntZ1 = Nod11PntZ;
	    TIMultiply(PntXTI1,PntYTI1,PntZTI1);
	    if (F1Index != 0) {
	      Nod12Indices = (((HLRAlgo_Array1OfPINod*)PINod1)->
			      ChangeValue(F1Pt1Index))->Indices();
	      Nod12RValues = (((HLRAlgo_Array1OfPINod*)PINod1)->
			      ChangeValue(F1Pt1Index))->RValues();
	      Nod12PntX    = Nod11PntX;
	      Nod12PntY    = Nod11PntY;
	      UpdateAroundNode(F1Pt1Index,Nod12Indices,
			       TData1,PISeg1,PINod1);
	    }
	  }
	  Nod11Indices = (((HLRAlgo_Array1OfPINod*)PINod2)->
			  ChangeValue(F2Pt2Index))->Indices();
	  if (Nod11Flag & NMskMove) {
#ifdef OCCT_DEBUG
	    if (DoTrace)
	      cout << F2Pt2Index << " modifies 22" << endl;
#endif
	    Nod11RValues = (((HLRAlgo_Array1OfPINod*)PINod2)->
			    ChangeValue(F2Pt2Index))->RValues();
	    Coordinates = BP.Coordinates();
	    PntXTI2 = PntX2 = Nod11PntX;
	    PntYTI2 = PntY2 = Nod11PntY;
	    PntZTI2 = PntZ2 = Nod11PntZ;
	    TIMultiply(PntXTI2,PntYTI2,PntZTI2);
	    if (F1Index != 0) {
	      Nod12Indices = (((HLRAlgo_Array1OfPINod*)PINod1)->
			      ChangeValue(F1Pt2Index))->Indices();
	      Nod12RValues = (((HLRAlgo_Array1OfPINod*)PINod1)->
			      ChangeValue(F1Pt2Index))->RValues();
	      Nod12PntX    = Nod11PntX;
	      Nod12PntY    = Nod11PntY;
	      UpdateAroundNode(F1Pt2Index,Nod12Indices,
			       TData1,PISeg1,PINod1);
	    }
	  }
	}
      }
      pid = (Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));
      
      for (f = 1; f <= nbFace; f++) {
	if (!(*pid).IsNull()) {
	  nbN   =  (*pid)->NbPINod();
	  PINod = &(*pid)->PINod();
	  Handle(HLRAlgo_PolyInternalNode)* NN = 
	    &(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(1));

	  for (i = 1; i <= nbN; i++) {
	    Nod11Indices = (*NN)->Indices();
	    Nod11Flag   &= ~NMskMove;
	    NN++;
	  }
	}
	pid++;
      }
    }
  }
}

//=======================================================================
//function : FindEdgeOnTriangle
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
FindEdgeOnTriangle (const Standard_Address Tri1Indices,
		    const Standard_Integer ip1,
		    const Standard_Integer ip2,
		    Standard_Integer& jtrouv,
		    Standard_Boolean& isDirect) const
{
  Standard_Integer n1 = Tri1Node1;
  Standard_Integer n2 = Tri1Node2;
  Standard_Integer n3 = Tri1Node3;
  if      (ip1 == n1 && ip2 == n2) {
    jtrouv = 0;
    isDirect = Standard_True;
    return;
  }
  else if (ip2 == n1 && ip1 == n2) {
    jtrouv = 0;
    isDirect = Standard_False;
    return;
  }
  else if (ip1 == n2 && ip2 == n3) {
    jtrouv = 1;
    isDirect = Standard_True;
    return;
  }
  else if (ip2 == n2 && ip1 == n3) {
    jtrouv = 1;
    isDirect = Standard_False;
    return;
  }
  else if (ip1 == n3 && ip2 == n1) {
    jtrouv = 2;
    isDirect = Standard_True;
    return;
  }
  else if (ip2 == n3 && ip1 == n1) {
    jtrouv = 2;
    isDirect = Standard_False;
    return;
  }
}

//=======================================================================
//function : ChangeNode
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::ChangeNode (const Standard_Integer ip1,
				   const Standard_Integer ip2,
				   const Standard_Address Nod1Indices,
				   const Standard_Address Nod1RValues,
				   const Standard_Address Nod2Indices,
				   const Standard_Address Nod2RValues,
				   const Standard_Real coef1,
				   const Standard_Real X3,
				   const Standard_Real Y3,
				   const Standard_Real Z3,
				   const Standard_Boolean first,
				   Standard_Address& TData,
				   Standard_Address& PISeg,
				   Standard_Address& PINod) const
{
  Standard_Real coef2 = 1 - coef1;
  if (first) {
    Nod1PntX = X3;
    Nod1PntY = Y3;
    Nod1PntZ = Z3;
    Nod1PntU = Nod1PntU * coef2 + Nod2PntU * coef1;
    Nod1PntV = Nod1PntV * coef2 + Nod2PntV * coef1; 
    Nod1Scal = Nod1Scal * coef2 + Nod2Scal * coef1;
    Standard_Real x = Nod1NrmX * coef2 + Nod2NrmX * coef1;
    Standard_Real y = Nod1NrmY * coef2 + Nod2NrmY * coef1;
    Standard_Real z = Nod1NrmZ * coef2 + Nod2NrmZ * coef1;
    Standard_Real D = sqrt (x * x + y * y + z * z);
    if (D > 0) {
      Nod1NrmX = x / D;
      Nod1NrmY = y / D;
      Nod1NrmZ = z / D;
    }
    else {
      Nod1NrmX = 1;
      Nod1NrmY = 0;
      Nod1NrmZ = 0;
#ifdef OCCT_DEBUG
      if (DoError) {
	cout << "HLRBRep_PolyAlgo::ChangeNode between " << ip1;
	cout << " and " << ip2 << endl;
      }
#endif
    }
    UpdateAroundNode(ip1,Nod1Indices,TData,PISeg,PINod);
  }
  else {
    Nod2PntX = X3;
    Nod2PntY = Y3;
    Nod2PntZ = Z3;
    Nod2PntU = Nod1PntU * coef2 + Nod2PntU * coef1;
    Nod2PntV = Nod1PntV * coef2 + Nod2PntV * coef1; 
    Nod2Scal = Nod1Scal * coef2 + Nod2Scal * coef1;
    Standard_Real x = Nod1NrmX * coef2 + Nod2NrmX * coef1;
    Standard_Real y = Nod1NrmY * coef2 + Nod2NrmY * coef1;
    Standard_Real z = Nod1NrmZ * coef2 + Nod2NrmZ * coef1;
    Standard_Real D = sqrt (x * x + y * y + z * z);
    if (D > 0) {
      D = 1 / D;
      Nod2NrmX = x * D;
      Nod2NrmY = y * D;
      Nod2NrmZ = z * D;
    }
    else {
      Nod2NrmX = 1;
      Nod2NrmY = 0;
      Nod2NrmZ = 0;
#ifdef OCCT_DEBUG
      if (DoError) {
	cout << "HLRBRep_PolyAlgo::ChangeNode between " << ip2;
	cout << " and " << ip1 << endl;
      }
#endif
    }
    UpdateAroundNode(ip2,Nod2Indices,TData,PISeg,PINod);
  }
}

//=======================================================================
//function : UpdateAroundNode
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::
UpdateAroundNode (const Standard_Integer iNode,
		  const Standard_Address Nod1Indices,
		  const Standard_Address TData,
		  const Standard_Address PISeg,
		  const Standard_Address PINod) const
{
  Standard_Integer iiii,iTri1,iTri2;
  iiii = Nod1NdSg;
  
  while (iiii != 0) {
    const Standard_Address Seg1Indices =
      ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii).Indices();
    iTri1 = Seg1Conex1;
    iTri2 = Seg1Conex2;
    if ( iTri1 != 0) {
      const Standard_Address Tri1Indices =
	((HLRAlgo_Array1OfTData*)TData)->ChangeValue(iTri1).Indices();
      const Handle(HLRAlgo_PolyInternalNode)* PN1 = 
	&(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri1Node1));
      const Handle(HLRAlgo_PolyInternalNode)* PN2 = 
	&(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri1Node2));
      const Handle(HLRAlgo_PolyInternalNode)* PN3 = 
	&(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri1Node3));
      const Standard_Address aNod1Indices = (*PN1)->Indices();
      const Standard_Address aNod2Indices = (*PN2)->Indices();
      const Standard_Address aNod3Indices = (*PN3)->Indices();
      const Standard_Address aNod1RValues = (*PN1)->RValues();
      const Standard_Address aNod2RValues = (*PN2)->RValues();
      const Standard_Address aNod3RValues = (*PN3)->RValues();
      OrientTriangle(iTri1,Tri1Indices,
		     aNod1Indices,aNod1RValues,
		     aNod2Indices,aNod2RValues,
		     aNod3Indices,aNod3RValues);
    }
    if ( iTri2 != 0) {
      const Standard_Address Tri2Indices =
	((HLRAlgo_Array1OfTData*)TData)->ChangeValue(iTri2).Indices();
      const Handle(HLRAlgo_PolyInternalNode)* PN1 = 
	&(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri2Node1));
      const Handle(HLRAlgo_PolyInternalNode)* PN2 = 
	&(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri2Node2));
      const Handle(HLRAlgo_PolyInternalNode)* PN3 = 
	&(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri2Node3));
      const Standard_Address aNod1Indices = (*PN1)->Indices();
      const Standard_Address aNod2Indices = (*PN2)->Indices();
      const Standard_Address aNod3Indices = (*PN3)->Indices();
      const Standard_Address aNod1RValues = (*PN1)->RValues();
      const Standard_Address aNod2RValues = (*PN2)->RValues();
      const Standard_Address aNod3RValues = (*PN3)->RValues();
      OrientTriangle(iTri2,Tri2Indices,
		     aNod1Indices,aNod1RValues,
		     aNod2Indices,aNod2RValues,
		     aNod3Indices,aNod3RValues);
    }
    if (Seg1LstSg1 == iNode) iiii = Seg1NxtSg1;
    else                     iiii = Seg1NxtSg2;
  }
}

//=======================================================================
//function : OrientTriangle
//purpose  : 
//=======================================================================

void 
#ifdef OCCT_DEBUG
HLRBRep_PolyAlgo::OrientTriangle(const Standard_Integer iTri,
#else
HLRBRep_PolyAlgo::OrientTriangle(const Standard_Integer,
#endif
				 const Standard_Address Tri1Indices,
				 const Standard_Address Nod1Indices,
				 const Standard_Address Nod1RValues,
				 const Standard_Address Nod2Indices,
				 const Standard_Address Nod2RValues,
				 const Standard_Address Nod3Indices,
				 const Standard_Address Nod3RValues) const
{
  Standard_Boolean o1 = Nod1Flag & NMskOutL;
  Standard_Boolean o2 = Nod2Flag & NMskOutL;
  Standard_Boolean o3 = Nod3Flag & NMskOutL;
  Tri1Flags &= ~FMskFlat;
  Tri1Flags &= ~FMskOnOutL;
  if (o1 && o2 && o3) {
    Tri1Flags |=  FMskSide;
    Tri1Flags &= ~FMskBack;
    Tri1Flags |=  FMskOnOutL;
#ifdef OCCT_DEBUG
    if (DoTrace) {
      cout << "HLRBRep_PolyAlgo::OrientTriangle : OnOutL";
      cout << " triangle " << iTri << endl;
    }
#endif
  }
  else {
    Standard_Real s1 = Nod1Scal;
    Standard_Real s2 = Nod2Scal;
    Standard_Real s3 = Nod3Scal;
    Standard_Real as1 = s1;
    Standard_Real as2 = s2;
    Standard_Real as3 = s3;
    if (s1 < 0) as1 = -s1;
    if (s2 < 0) as2 = -s2;
    if (s3 < 0) as3 = -s3;
    Standard_Real  s = 0;
    Standard_Real as = 0;
    if (!o1            ) {s = s1; as = as1;}
    if (!o2 && as < as2) {s = s2; as = as2;}
    if (!o3 && as < as3) {s = s3; as = as3;}
    if (s > 0) {
      Tri1Flags &= ~FMskSide;
      Tri1Flags |=  FMskBack;
    }
    else {
      Tri1Flags &= ~FMskSide;
      Tri1Flags &= ~FMskBack;
    }
    Standard_Real dx12 = Nod2PntX - Nod1PntX;
    Standard_Real dy12 = Nod2PntY - Nod1PntY;
    Standard_Real dz12 = Nod2PntZ - Nod1PntZ;
    Standard_Real  d12 = sqrt(dx12 * dx12 + dy12 * dy12 + dz12 * dz12);
    if (d12 <= 1.e-10) {
#ifdef OCCT_DEBUG
      if (DoTrace) {
	cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
	cout << " triangle " << iTri << endl;
      }
#endif
      Tri1Flags |=  FMskFlat;
      Tri1Flags |=  FMskSide;
      Tri1Flags &= ~FMskBack;
    }
    else {
      Standard_Real dx23 = Nod3PntX - Nod2PntX;
      Standard_Real dy23 = Nod3PntY - Nod2PntY;
      Standard_Real dz23 = Nod3PntZ - Nod2PntZ;
      Standard_Real  d23 = sqrt(dx23 * dx23 + dy23 * dy23 + dz23 * dz23);
      if (d23 < 1.e-10) {
#ifdef OCCT_DEBUG
	if (DoTrace) {
	  cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
	  cout << " triangle " << iTri << endl;
	}
#endif
	Tri1Flags |=  FMskFlat;
	Tri1Flags |=  FMskSide;
	Tri1Flags &= ~FMskBack;
      }
      else {
	Standard_Real  dx31  = Nod1PntX - Nod3PntX;
	Standard_Real  dy31  = Nod1PntY - Nod3PntY;
	Standard_Real  dz31  = Nod1PntZ - Nod3PntZ;
	Standard_Real   d31  = sqrt(dx31 * dx31 + dy31 * dy31 + dz31 * dz31);
	if (d31 < 1.e-10) {
#ifdef OCCT_DEBUG
	  if (DoTrace) {
	    cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
	    cout << " triangle " << iTri << endl;
	  }
#endif
	  Tri1Flags |=  FMskFlat;
	  Tri1Flags |=  FMskSide;
	  Tri1Flags &= ~FMskBack;
	}
	else {
	  dx12 /= d12;
	  dy12 /= d12;
	  dz12 /= d12;
	  dx23 /= d23;
	  dy23 /= d23;
	  dz23 /= d23;
          Standard_Real  dx = dy12 * dz23 - dz12 * dy23;
          Standard_Real  dy = dz12 * dx23 - dx12 * dz23;
          Standard_Real  dz = dx12 * dy23 - dy12 * dx23;
	  Standard_Real  d  = sqrt(dx * dx + dy * dy + dz * dz);
	  if (d < 1.e-5) {
#ifdef OCCT_DEBUG
	    if (DoTrace) {
	      cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
	      cout << " triangle " << iTri << endl;
	    }
#endif
	    Tri1Flags |=  FMskFlat;
	    Tri1Flags |=  FMskSide;
	    Tri1Flags &= ~FMskBack;
	  }
	  else {
	    Standard_Real o;
	    if (myProj.Perspective()) {
	      dx /= d;
	      dy /= d;
	      dz /= d;
	      o = (  dz * myProj.Focus()
		   - dx * Nod1PntX
		   - dy * Nod1PntY
		   - dz * Nod1PntZ);
	    }
	    else
	      o = dz / d;
	    if (o < 0) {
	      Tri1Flags |=  FMskOrBack;
	      o = -o;
	    }
	    else
	      Tri1Flags &= ~FMskOrBack;
	    if (o < 1.e-10) {
	      Tri1Flags |=  FMskSide;
	      Tri1Flags &= ~FMskBack;
	    }
	  }
	}
      }
    }
  }
  if ((!(Tri1Flags & FMskBack) &&  (Tri1Flags & FMskOrBack)) ||
      ( (Tri1Flags & FMskBack) && !(Tri1Flags & FMskOrBack)))
    Tri1Flags |=  FMskFrBack;
  else 
    Tri1Flags &= ~FMskFrBack;
}

//=======================================================================
//function : Triangles
//purpose  : 
//=======================================================================

Standard_Boolean
HLRBRep_PolyAlgo::Triangles(const Standard_Integer ip1,
			    const Standard_Integer ip2,
			    const Standard_Address Nod1Indices,
			    Standard_Address& PISeg,
			    Standard_Integer& iTri1,
			    Standard_Integer& iTri2) const
{
  Standard_Address Seg1Indices;
  Standard_Integer iiii = Nod1NdSg;
  
  while (iiii != 0) {
    Seg1Indices =
      ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii).Indices();
    if (Seg1LstSg1 == ip1) {
      if (Seg1LstSg2 == ip2) {
	iTri1 = Seg1Conex1;
	iTri2 = Seg1Conex2;
	return Standard_True;
      }
      else iiii = Seg1NxtSg1;
    }
    else {
      if (Seg1LstSg1 == ip2) {
	iTri1 = Seg1Conex1;
	iTri2 = Seg1Conex2;
	return Standard_True;
      }
      else iiii = Seg1NxtSg2;
    }
  }
  iTri1 = 0;
  iTri2 = 0;
#ifdef OCCT_DEBUG
  if (DoError) {
    cout << "HLRBRep_PolyAlgo::Triangles : error";
    cout << " between " << ip1 << " and " << ip2 << endl;
  }
#endif
  return Standard_False;
}

//=======================================================================
//function : NewNode
//purpose  : 
//=======================================================================

Standard_Boolean
HLRBRep_PolyAlgo::
NewNode (const Standard_Address Nod1RValues,
	 const Standard_Address Nod2RValues,
	 Standard_Real& coef1,
	 Standard_Boolean& moveP1) const
{
  Standard_Real TolAng = myTolAngular * 0.5;
  if ((Nod1Scal >= TolAng && Nod2Scal <= -TolAng) ||
      (Nod2Scal >= TolAng && Nod1Scal <= -TolAng)) {
    coef1 = Nod1Scal / ( Nod2Scal - Nod1Scal );
    if (coef1 < 0) coef1 = - coef1;
    moveP1 = coef1 < 0.5;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : UVNode
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::UVNode (const Standard_Address Nod1RValues,
			  const Standard_Address Nod2RValues,
			  const Standard_Real coef1,
			  Standard_Real& U3,
			  Standard_Real& V3) const
{
  Standard_Real coef2 = 1 - coef1;
  U3 = Nod1PntU * coef2 + Nod2PntU * coef1;
  V3 = Nod1PntV * coef2 + Nod2PntV * coef1;
}

//=======================================================================
//function : CheckDegeneratedSegment
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::
CheckDegeneratedSegment(const Standard_Address Nod1Indices,
			const Standard_Address Nod1RValues,
			const Standard_Address Nod2Indices,
			const Standard_Address Nod2RValues) const
{
  Nod1Flag |=  NMskFuck;
  Nod2Flag |=  NMskFuck;
  if ((Nod1Scal >= myTolAngular && Nod2Scal <= -myTolAngular) ||
      (Nod2Scal >= myTolAngular && Nod1Scal <= -myTolAngular)) {
    Nod1Scal  = 0.;
    Nod1Flag |= NMskOutL;
    Nod2Scal  = 0.;
    Nod2Flag |= NMskOutL;
  }
}

//=======================================================================
//function : UpdateOutLines
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::UpdateOutLines (HLRAlgo_ListOfBPoint& List,
				  TColStd_Array1OfTransient& PID)
{
  Standard_Integer f;
  Standard_Integer nbFace = myFMap.Extent();
  Standard_Real X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ;
  Standard_Real XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2;
    
  Handle(HLRAlgo_PolyInternalData)* pid = 
    (Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));

  for (f = 1; f <= nbFace; f++) { 
    if (!(*pid).IsNull()) {
      if ((*pid)->IntOutL()) {
	Standard_Address TData = &((*pid)->TData());
	Standard_Address PISeg = &((*pid)->PISeg());
	Standard_Address PINod = &((*pid)->PINod());
	Standard_Integer i,j,it1,it2,tn1,tn2,tn3,pd,pf;
	Standard_Address Seg2Indices,Tri1Indices,Tri2Indices;
	Standard_Boolean outl;
	Standard_Integer nbS = (*pid)->NbPISeg();
	HLRAlgo_PolyInternalSegment* psg =
	  &(((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(1));
	
	for (i = 1; i <= nbS; i++) {
	  Seg2Indices = psg->Indices();
	  it1 = Seg2Conex1;
	  it2 = Seg2Conex2;
	  if (it1 != 0 && it2 != 0 && it1 != it2) {  // debile but sure !
	    Tri1Indices = ((HLRAlgo_Array1OfTData*)TData)->
	      ChangeValue(it1).Indices();
	    Tri2Indices = ((HLRAlgo_Array1OfTData*)TData)->
	      ChangeValue(it2).Indices();
	    if      (!(Tri1Flags & FMskSide) && !(Tri2Flags & FMskSide))
	      outl =  (Tri1Flags & FMskBack) !=  (Tri2Flags & FMskBack);
	    else if ( (Tri1Flags & FMskSide) &&  (Tri2Flags & FMskSide))
	      outl = Standard_False;
	    else if (  Tri1Flags & FMskSide)
	      outl = !(Tri1Flags & FMskFlat) && !(Tri2Flags & FMskBack);
	    else
	      outl = !(Tri2Flags & FMskFlat) && !(Tri1Flags & FMskBack);
	    
	    if (outl) {
	      pd = Seg2LstSg1;
	      pf = Seg2LstSg2;
	      tn1 = Tri1Node1;
	      tn2 = Tri1Node2;
	      tn3 = Tri1Node3;
	      if (!(Tri1Flags & FMskSide) && (Tri1Flags & FMskOrBack)) {
		j   = tn1;
		tn1 = tn3;
		tn3 = j;
	      }
	      if      ((tn1 == pd && tn2 == pf) || (tn1 == pf && tn2 == pd))
		Tri1Flags |=  EMskOutLin1;
	      else if ((tn2 == pd && tn3 == pf) || (tn2 == pf && tn3 == pd))
		Tri1Flags |=  EMskOutLin2;
	      else if ((tn3 == pd && tn1 == pf) || (tn3 == pf && tn1 == pd))
		Tri1Flags |=  EMskOutLin3;
#ifdef OCCT_DEBUG
	      else if (DoError) {
		cout << "HLRAlgo_PolyInternalData::UpdateOutLines";
		cout << " : segment not found" << endl;
	      }
#endif
	      tn1 = Tri2Node1;
	      tn2 = Tri2Node2;
	      tn3 = Tri2Node3;
	      if (!(Tri2Flags & FMskSide) && (Tri2Flags & FMskOrBack)) {
		j   = tn1;
		tn1 = tn3;
		tn3 = j;
	      }
	      if      ((tn1 == pd && tn2 == pf) || (tn1 == pf && tn2 == pd))
		Tri2Flags |=  EMskOutLin1;
	      else if ((tn2 == pd && tn3 == pf) || (tn2 == pf && tn3 == pd))
		Tri2Flags |=  EMskOutLin2;
	      else if ((tn3 == pd && tn1 == pf) || (tn3 == pf && tn1 == pd))
		Tri2Flags |=  EMskOutLin3;
#ifdef OCCT_DEBUG
	      else if (DoError) {
		cout << "HLRAlgo_PolyInternalData::UpdateOutLines";
		cout << " : segment not found" << endl;
	      }
#endif
	      Standard_Address Nod1RValues =
		((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(pd)->RValues();
	      Standard_Address Nod2RValues =
		((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(pf)->RValues();
	      XTI1 = X1 = Nod1PntX;
	      YTI1 = Y1 = Nod1PntY;
	      ZTI1 = Z1 = Nod1PntZ;
	      XTI2 = X2 = Nod2PntX;
	      YTI2 = Y2 = Nod2PntY;
	      ZTI2 = Z2 = Nod2PntZ;
	      TIMultiply(XTI1,YTI1,ZTI1);
	      TIMultiply(XTI2,YTI2,ZTI2);
	      List.Append(HLRAlgo_BiPoint(XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2,
					  X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2  ,
					  f,f,pd,pf,f,pd,pf,12));
	    }
	  }
	  psg++;
	}
      }
    }
    pid++;
  }
}

//=======================================================================
//function : UpdateEdgesBiPoints
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::
UpdateEdgesBiPoints (HLRAlgo_ListOfBPoint& List,
		     const TColStd_Array1OfTransient& PID,
		     const Standard_Boolean closed)
{
  Standard_Integer itri1,itri2,tbid;
  HLRAlgo_ListIteratorOfListOfBPoint it;
  
  for (it.Initialize(List); it.More(); it.Next()) {      
    HLRAlgo_BiPoint& BP = it.Value();
//    Standard_Integer i[5];
    Standard_Address IndexPtr = BP.Indices();
    if (F1Index != 0 && F2Index != 0) {
      const Handle(HLRAlgo_PolyInternalData)& pid1 =
	*(Handle(HLRAlgo_PolyInternalData)*)&(PID(F1Index));
      const Handle(HLRAlgo_PolyInternalData)& pid2 =
	*(Handle(HLRAlgo_PolyInternalData)*)&(PID(F2Index));
      Standard_Address PISeg1 = &pid1->PISeg();
      Standard_Address PISeg2 = &pid2->PISeg();
      Standard_Address Nod11Indices = 
	pid1->PINod().ChangeValue(F1Pt1Index)->Indices();
      Standard_Address Nod21Indices = 
	pid2->PINod().ChangeValue(F2Pt1Index)->Indices();
      Triangles(F1Pt1Index,F1Pt2Index,Nod11Indices,PISeg1,itri1,tbid);
      Triangles(F2Pt1Index,F2Pt2Index,Nod21Indices,PISeg2,itri2,tbid);

      if (itri1 != 0 && itri2 != 0) {
	if (F1Index != F2Index || itri1 != itri2) {
	  Standard_Address TData1 = &pid1->TData();
	  Standard_Address TData2 = &pid2->TData();
	  Standard_Address Tri1Indices =
	    ((HLRAlgo_Array1OfTData*)TData1)->ChangeValue(itri1).Indices();
	  Standard_Address Tri2Indices =
	    ((HLRAlgo_Array1OfTData*)TData2)->ChangeValue(itri2).Indices();
	  if (closed) {
	    if (((Tri1Flags & FMskBack) && (Tri2Flags & FMskBack)) ||
		((Tri1Flags & FMskSide) && (Tri2Flags & FMskSide)) ||
		((Tri1Flags & FMskBack) && (Tri2Flags & FMskSide)) ||
		((Tri1Flags & FMskSide) && (Tri2Flags & FMskBack)))
	      BP.Hidden(Standard_True);
	  }
	  Standard_Boolean outl;
	  if      (!(Tri1Flags & FMskSide) && !(Tri2Flags & FMskSide))
	    outl =  (Tri1Flags & FMskBack) !=  (Tri2Flags & FMskBack);
	  else if ( (Tri1Flags & FMskSide) &&  (Tri2Flags & FMskSide))
	    outl = Standard_False;
	  else if ( (Tri1Flags & FMskSide))
	    outl = !(Tri1Flags & FMskFlat) && !(Tri2Flags & FMskBack);
	  else
	    outl = !(Tri2Flags & FMskFlat) && !(Tri1Flags & FMskBack);
	  BP.OutLine(outl);
	}
      }
#ifdef OCCT_DEBUG
      else if (DoError) {
	cout << "HLRBRep_PolyAlgo::UpdateEdgesBiPoints : error ";
	cout << " between " << F1Index << setw(6);
	cout << " and " << F2Index << endl;
      }
#endif
    }
  }
}

//=======================================================================
//function : UpdatePolyData
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::UpdatePolyData (TColStd_Array1OfTransient& PD,
				  TColStd_Array1OfTransient& PID,
				  const Standard_Boolean closed)
{
  Standard_Integer f,i;//,n[3];
  Handle(TColgp_HArray1OfXYZ)    HNodes;
  Handle(HLRAlgo_HArray1OfTData) HTData;
  Handle(HLRAlgo_HArray1OfPHDat) HPHDat;
  Standard_Integer nbFace = myFMap.Extent();
  Handle(HLRAlgo_PolyInternalData)* pid = 
    (Handle(HLRAlgo_PolyInternalData)*)&(PID.ChangeValue(1));
  Handle(HLRAlgo_PolyData)* pd =
    (Handle(HLRAlgo_PolyData)*)&(PD.ChangeValue(1));

  for (f = 1; f <= nbFace; f++) {
    if (!(*pid).IsNull()) {
      Standard_Integer nbN = (*pid)->NbPINod();
      Standard_Integer nbT = (*pid)->NbTData();
      HNodes = new TColgp_HArray1OfXYZ   (1,nbN);
      HTData = new HLRAlgo_HArray1OfTData(1,nbT);
      TColgp_Array1OfXYZ&    Nodes = HNodes->ChangeArray1();
      HLRAlgo_Array1OfTData& Trian = HTData->ChangeArray1();
      Standard_Address TData = &(*pid)->TData();
      Standard_Address PINod = &(*pid)->PINod();
      Standard_Integer nbHide = 0;
      Handle(HLRAlgo_PolyInternalNode)* ON = 
	&(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(1));
      gp_XYZ                          * NN = &(Nodes.ChangeValue(1));
      
      for (i = 1; i <= nbN; i++) {
	const Standard_Address Nod1RValues = (*ON)->RValues();
	NN->SetCoord(Nod1PntX,Nod1PntY,Nod1PntZ);
	ON++;
	NN++;
      }
      
      HLRAlgo_TriangleData* OT =
	&(((HLRAlgo_Array1OfTData*)TData)->ChangeValue(1));
      HLRAlgo_TriangleData* NT = &(Trian.ChangeValue(1));
      Standard_Address Tri1Indices,Tri2Indices;
      
      for (i = 1; i <= nbT; i++) {
	Tri1Indices = OT->Indices();
	Tri2Indices = NT->Indices();
	if (!(Tri1Flags & FMskSide)) {
#ifdef OCCT_DEBUG
	  if ((Tri1Flags & FMskFrBack) && DoTrace) {
	    cout << "HLRBRep_PolyAlgo::ReverseBackTriangle :";
	    cout << " face " << f << setw(6);
	    cout << " triangle " << i << endl;
	  }
#endif
	  if (Tri1Flags & FMskOrBack) {
	    Standard_Integer j = Tri1Node1;
	    Tri1Node1          = Tri1Node3;
	    Tri1Node3          = j;
	    Tri1Flags |=  FMskBack;
	  }
	  else
	    Tri1Flags &= ~FMskBack;
	    //Tri1Flags |= FMskBack;//OCC349
	}
	Tri2Node1 = Tri1Node1;
	Tri2Node2 = Tri1Node2;
	Tri2Node3 = Tri1Node3;
	Tri2Flags = Tri1Flags;
	if (!(Tri2Flags & FMskSide) &&
	    (!(Tri2Flags & FMskBack) || !closed)) {
	  Tri2Flags |=  FMskHiding;
	  nbHide++;
	}
	else
	  Tri2Flags &= ~FMskHiding;
	OT++;
	NT++;
      }
      if (nbHide > 0) HPHDat = new HLRAlgo_HArray1OfPHDat(1,nbHide);
      else            HPHDat.Nullify();
      (*pd)->HNodes(HNodes);
      (*pd)->HTData(HTData);
      (*pd)->HPHDat(HPHDat);
      (*pd)->FaceIndex(f);
    }
    pid++;
    pd++;
  }
}

//=======================================================================
//function : TMultiply
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::TMultiply (Standard_Real& X,
			     Standard_Real& Y,
			     Standard_Real& Z,
			     const Standard_Boolean VPO) const
{
  Standard_Real Xt = TMat[0][0]*X + TMat[0][1]*Y + TMat[0][2]*Z + (VPO ? 0 : TLoc[0]);//OCC349
  Standard_Real Yt = TMat[1][0]*X + TMat[1][1]*Y + TMat[1][2]*Z + (VPO ? 0 : TLoc[1]);//OCC349
  Z                = TMat[2][0]*X + TMat[2][1]*Y + TMat[2][2]*Z + (VPO ? 0 : TLoc[2]);//OCC349
  X                = Xt;
  Y                = Yt;
}

//=======================================================================
//function : TTMultiply
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::TTMultiply (Standard_Real& X,
			      Standard_Real& Y,
			      Standard_Real& Z,
			      const Standard_Boolean VPO) const
{
  Standard_Real Xt = TTMa[0][0]*X + TTMa[0][1]*Y + TTMa[0][2]*Z + (VPO ? 0 : TTLo[0]);//OCC349
  Standard_Real Yt = TTMa[1][0]*X + TTMa[1][1]*Y + TTMa[1][2]*Z + (VPO ? 0 : TTLo[1]);//OCC349
  Z                = TTMa[2][0]*X + TTMa[2][1]*Y + TTMa[2][2]*Z + (VPO ? 0 : TTLo[2]);//OCC349
  X                = Xt;
  Y                = Yt;
}

//=======================================================================
//function : TIMultiply
//purpose  : 
//=======================================================================

void
HLRBRep_PolyAlgo::TIMultiply (Standard_Real& X,
			      Standard_Real& Y,
			      Standard_Real& Z,
			      const Standard_Boolean VPO) const
{
  Standard_Real Xt = TIMa[0][0]*X + TIMa[0][1]*Y + TIMa[0][2]*Z + (VPO ? 0 : TILo[0]);//OCC349
  Standard_Real Yt = TIMa[1][0]*X + TIMa[1][1]*Y + TIMa[1][2]*Z + (VPO ? 0 : TILo[1]);//OCC349
  Z                = TIMa[2][0]*X + TIMa[2][1]*Y + TIMa[2][2]*Z + (VPO ? 0 : TILo[2]);//OCC349
  X                = Xt;
  Y                = Yt;
}

//=======================================================================
//function : Hide
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::Hide (Standard_Address& Coordinates,
			     HLRAlgo_EdgeStatus& status,
			     TopoDS_Shape& S,
			     Standard_Boolean& reg1,
			     Standard_Boolean& regn,
			     Standard_Boolean& outl,
			     Standard_Boolean& intl)
{
  Standard_Integer index;
  myAlgo->Hide(Coordinates,status,index,reg1,regn,outl,intl);
  if (intl) S = myFMap(index);
  else      S = myEMap(index);
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

void HLRBRep_PolyAlgo::Show (Standard_Address& Coordinates,
			     TopoDS_Shape& S,
			     Standard_Boolean& reg1,
			     Standard_Boolean& regn,
			     Standard_Boolean& outl,
			     Standard_Boolean& intl)
{
  Standard_Integer index;
  myAlgo->Show(Coordinates,index,reg1,regn,outl,intl);
  if (intl) S = myFMap(index);
  else      S = myEMap(index);
}

//=======================================================================
//function : OutLinedShape
//purpose  : 
//=======================================================================

TopoDS_Shape
HLRBRep_PolyAlgo::OutLinedShape (const TopoDS_Shape& S) const
{
  TopoDS_Shape Result;

  if (!S.IsNull()) {
    BRep_Builder B;
    B.MakeCompound(TopoDS::Compound(Result));
    B.Add(Result,S);

    TopTools_MapOfShape Map;
    TopExp_Explorer ex;
    for (ex.Init(S,TopAbs_EDGE); ex.More(); ex.Next())
      Map.Add(ex.Current());
    for (ex.Init(S,TopAbs_FACE); ex.More(); ex.Next())
      Map.Add(ex.Current());

    Standard_Integer nbFace = myFMap.Extent();
    if (nbFace > 0) {
      TopTools_Array1OfShape NewF(1,nbFace);
      TColStd_Array1OfTransient& Shell = myAlgo->PolyShell();
      Standard_Integer nbShell = Shell.Upper();
      HLRAlgo_ListIteratorOfListOfBPoint it;
      
      for (Standard_Integer iShell = 1; iShell <= nbShell; iShell++) {
	HLRAlgo_ListOfBPoint& List =
	  (*(Handle(HLRAlgo_PolyShellData)*)&(Shell(iShell)))->Edges();
	
	for (it.Initialize(List); it.More(); it.Next()) {
	  HLRAlgo_BiPoint& BP = it.Value();
	  if (BP.IntLine()) {
	    Standard_Address IndexPtr = BP.Indices();
	    if (Map.Contains(myFMap(ShapeIndex))) {
	      Standard_Address Coordinates = BP.Coordinates();
	      B.Add(Result,BRepLib_MakeEdge
		    (gp_Pnt(PntXTI1,PntYTI1,PntZTI1),
		     gp_Pnt(PntXTI2,PntYTI2,PntZTI2)));
	    }
	  }
	}
      }
    }    
  }
  return Result;
}

