# The plugin expects to not link against the Support and Core libraries,
# but expects them to exist in the process loading the plugin. This doesn't
# work with DLLs on Windows (where a shared library can't have undefined
# references), so just skip this example on Windows.
if (NOT WIN32 AND NOT CYGWIN)
  add_llvm_pass_plugin(SimpleMergeReturn
    SimpleMergeReturn.cpp
    DEPENDS
    intrinsics_gen
    BUILDTREE_ONLY
   )

  install(TARGETS ${name} RUNTIME DESTINATION "${SimpleMergeReturnPass}")
  set_target_properties(${name} PROPERTIES FOLDER "SimpleMergeReturnPass")
endif()
