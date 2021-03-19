var OccViewerModule =
{
  print: (function() {
    var anElement = document.getElementById('output');
    return function(theText) { anElement.innerHTML += theText + "<br>"; };
  })(),
  printErr: function(theText) {
    //var anElement = document.getElementById('output');
    //anElement.innerHTML += theText + "<br>";
    console.warn(theText);
  },
  canvas: (function() {
    var aCanvas = document.getElementById('occViewerCanvas');
    var aGlCtx =                   aCanvas.getContext ('webgl2', { alpha: false, depth: true, antialias: false, preserveDrawingBuffer: true } );
    if (aGlCtx == null) { aGlCtx = aCanvas.getContext ('webgl',  { alpha: false, depth: true, antialias: false, preserveDrawingBuffer: true } ); }
    return aCanvas;
  })(),

  onRuntimeInitialized: function() {
    //console.log(" @@ onRuntimeInitialized()" + Object.getOwnPropertyNames(OccViewerModule));
  }
};

const OccViewerModuleInitialized = createOccViewerModule(OccViewerModule);
OccViewerModuleInitialized.then(function(Module) {
  //OccViewerModule.setCubemapBackground ("cubemap.jpg");
  OccViewerModule.openFromUrl ("ball", "samples/Ball.brep");
});
