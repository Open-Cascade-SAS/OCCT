# This is a project template file defining an OCCT Module.
# This project should be included with predefined OCC_MODULE_NAME variable.
TEMPLATE = subdirs
exists(custom.auto.pri) { include(custom.auto.pri) }
exists(custom.pri)      { include(custom.pri) }

# Iterate over Toolkits within current Module and generate sub-project per Toolkit
aModuleList = $$cat(../MODULES, lines)
for (aModuleIter, aModuleList) {
  #aTkList = $$list($$aModuleIter) - this doesn't work, qmake bug?
  aModule = $$first($$list($$aModuleIter))
  equals (aModule, $$OCC_MODULE_NAME) {
    for (aToolKit, $$list($$aModuleIter)) {
      toSkipToolkit = 0
      equals (aToolKit, $$OCC_MODULE_NAME)      { toSkipToolkit = 1 }
      !HAVE_VTK:equals (aToolKit, "TKIVtk")     { toSkipToolkit = 1 }
      !HAVE_VTK:equals (aToolKit, "TKIVtkDraw") { toSkipToolkit = 1 }
      #!HAVE_OPENGL: equals (aToolKit, "TKOpenGl")       { toSkipToolkit = 1 }
      #!HAVE_OPENGL: equals (aToolKit, "TKOpenGlTest")   { toSkipToolkit = 1 }
      !HAVE_GLES2:   equals (aToolKit, "TKOpenGles")     { toSkipToolkit = 1 }
      !HAVE_GLES2:   equals (aToolKit, "TKOpenGlesTest") { toSkipToolkit = 1 }
      !win32:   equals (aToolKit, "TKD3DHost")  { toSkipToolkit = 1 }
      !win32:   equals (aToolKit, "TKD3DHostTest") { toSkipToolkit = 1 }
      equals (toSkipToolkit, 0) {

        #warning(aToolKit($$OCC_MODULE_NAME)=$$aToolKit)
        eval(occtkgen_$${aToolKit}.input  = $$_PRO_FILE_PWD_/../OccToolkit.pro.in)
        eval(occtkgen_$${aToolKit}.output = $$_PRO_FILE_PWD_/$${aToolKit}/$${aToolKit}.pro)
        eval(occtkgen_$${aToolKit}.config = verbatim)
        eval(QMAKE_SUBSTITUTES += occtkgen_$${aToolKit})
        SUBDIRS += $${aToolKit}

        aModExtList = $$cat(../../src/$${aToolKit}/EXTERNLIB, lines)
        aTkDepends = $${aToolKit}.depends
        for (aModExtIter, aModExtList) {
          contains ($$list($$aModuleIter), $$aModExtIter) { eval ($${aTkDepends} += $$aModExtIter) }
        }
        #warning($$aToolKit depends on: $$reverse($${aTkDepends}))
      }
    }
  }
}
