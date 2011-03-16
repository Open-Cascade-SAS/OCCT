switch (elem) {
  case TelNil:
    printf ("TelNil\n");
    break;
  case TelLabel:
    printf ("TelLabel %d\n", e[i].data);
    break;
  case TelAntiAliasing:
    printf ("TelAntiAliasing\n");
    break;
  case TelBlink:
    printf ("TelBlink\n");
    break;
  case TelExecuteStructure:
    printf ("TelExecuteStructure %d\n", e[i].data);
    break;
  case TelPolylineColour:
    printf ("TelPolylineColour\n");
    break;
  case TelPolylineType:
    printf ("TelPolylineType\n");
    break;
  case TelPolylineWidth:
    printf ("TelPolylineWidth\n");
    break;
  case TelInteriorStyle:
    printf ("TelInteriorStyle\n");
    break;
  case TelInteriorColour:
    printf ("TelInteriorColour\n");
    break;
  case TelBackInteriorColour:
    printf ("TelBackInteriorColour\n");
    break;
  case TelEdgeColour:
    printf ("TelEdgeColour\n");
    break;
  case TelEdgeType:
    printf ("TelEdgeType\n");
    break;
  case TelEdgeWidth:
    printf ("TelEdgeWidth\n");
    break;
  case TelEdgeFlag:
    printf ("TelEdgeFlag\n");
    break;
  case TelInteriorStyleIndex:
    printf ("TelInteriorStyleIndex\n");
    break;
  case TelFaceDistinguishingMode:
    printf ("TelFaceDistinguishingMode\n");
    break;
  case TelFaceCullingMode:
    printf ("TelFaceCullingMode\n");
    break;
  case TelInteriorReflectanceEquation:
    printf ("TelInteriorReflectanceEquation\n");
    break;
  case TelSurfaceAreaProperties:
    printf ("TelSurfaceAreaProperties\n");
    break;
  case TelBackInteriorReflectanceEquation:
    printf ("TelBackInteriorReflectanceEquation\n");
    break;
  case TelBackSurfaceAreaProperties:
    printf ("TelBackSurfaceAreaProperties\n");
    break;
  case TelPolymarkerColour:
    printf ("TelPolymarkerColour\n");
    break;
  case TelPolymarkerType:
    printf ("TelPolymarkerType\n");
    break;
  case TelPolymarkerSize:
    printf ("TelPolymarkerSize\n");
    break;
  case TelTextFont:
    printf ("TelTextFont\n");
    break;
  case TelTextHeight:
    printf ("TelTextHeight\n");
    break;
  case TelCharacterSpacing:
    printf ("TelCharacterSpacing\n");
    break;
  case TelCharacterExpansionFactor:
    printf ("TelCharacterExpansionFactor\n");
    break;
  case TelTextColour:
    printf ("TelTextColour\n");
    break;
  case TelDepthCueIndex:
    printf ("TelDepthCueIndex\n");
    break;
  case TelPickId:
    printf ("TelPickId\n");
    break;
  case TelPolygon:
    printf ("TelPolygon\n");
    break;
  case TelPolygonSet:
    printf ("TelPolygonSet\n");
    break;
  case TelPolygonHoles:
    printf ("TelPolygonHoles\n");
    break;
  case TelPolygonIndices:
    printf ("TelPolygonIndices\n");
    break;
  case TelQuadrangle:
    printf ("TelQuadrangle\n");
    break;
  case TelPolyline:
    printf ("TelPolyline\n");
    break;
  case TelMarker:
    printf ("TelMarker\n");
    break;
  case TelMarkerSet:
    printf ("TelMarkerSet\n");
    break;
  case TelText:
    printf ("TelText\n");
    break;
  case TelTriangleMesh:
    printf ("TelTriangleMesh\n");
    break;
  case TelAddNameset:
    printf ("TelAddNameset\n");
    break;
  case TelRemoveNameset:
    printf ("TelRemoveNameset\n");
    break;
  case TelInteriorShadingMethod:
    printf ("TelInteriorShadingMethod\n");
    break;
  case TelLocalTran3:
    printf ("TelLocalTran3\n");
    break;
  case TelHighlightIndex:
    printf ("TelHighlightIndex\n");
    break;
  case TelLightSrcState:
    printf ("TelLightSrcState\n");
    break;
  case TelClippingPlane:
    printf ("TelClippingPlane\n");
    break;
  case TelViewIndex:
    printf ("TelViewIndex\n");
    break;
  case TelApplicationData:
    printf ("TelApplicationData\n");
    break;
  case TelCurve:
    printf ("TelCurve\n");
    break;
  case TelTextureId:
    printf ("TelTextureId\n");
    break;
  case TelDoTextureMap:
    printf ("TelDoTextureMap\n");
    break;
  case TelLast:
    printf ("TelLast\n");
    break;
  default:
    printf ("default\n");
    break;
}
