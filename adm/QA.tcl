global tcl_platform

if {[array get env QA_DUMP] != "" && $env(QA_DUMP) == "1"} {
    puts "QA commands have been loaded"
    set env(QADEFAULT) $env(CASROOT)/src/QAResources/QADrawAppliInit
    set env(QA_LD_LIBRARY_PATH) $env(CASROOT)/sun/lib/.
    set env(DRAWDEFAULTOLD) $env(DRAWDEFAULT)
    set env(DRAWDEFAULT) "$env(QADEFAULT)"
    set env(CSF_DrawPluginQADefaults) $env(CASROOT)/src/QAResources/.
}
