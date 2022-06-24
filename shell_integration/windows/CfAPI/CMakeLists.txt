add_library(${APPX_MANIFEST_SHELLEXT_TARGET_NAME} MODULE
    dllmain.cpp
    cfapishellintegrationclassfactory.cpp
    thumbnailprovider.cpp
    contextmenus.cpp
    customstateprovider.cpp
    CfApiShellIntegration.def
    customstateprovider.idl
)

add_custom_command(TARGET ${APPX_MANIFEST_SHELLEXT_TARGET_NAME} PRE_BUILD
    COMMAND midl /nologo /header customstateprovider.g.h "${CMAKE_CURRENT_SOURCE_DIR}/customstateprovider.idl"
    MAIN_DEPENDENCY foo.idl
)

target_link_libraries(${APPX_MANIFEST_SHELLEXT_TARGET_NAME} shlwapi)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /await")

target_compile_features(${APPX_MANIFEST_SHELLEXT_TARGET_NAME} PRIVATE cxx_std_17)


file(MAKE_DIRECTORY "${BIN_OUTPUT_DIRECTORY}/${APPX_MANIFEST_SHELLEXT_DLL_FOLDER}")

add_custom_command(TARGET ${APPX_MANIFEST_SHELLEXT_TARGET_NAME} POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:${APPX_MANIFEST_SHELLEXT_TARGET_NAME}>"
     "${BIN_OUTPUT_DIRECTORY}/shellext/$<TARGET_FILE_NAME:${APPX_MANIFEST_SHELLEXT_TARGET_NAME}>" 

  COMMENT "Copying ${APPX_MANIFEST_SHELLEXT_TARGET_NAME} to output directory")

install(TARGETS ${APPX_MANIFEST_SHELLEXT_TARGET_NAME} 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
)
