
#file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CustomStateProvider.g.h" MidlOutputPath)

#add_custom_target(CustomStateProviderImpl
   # DEPENDS ${MidlOutputPath}
#)

#add_custom_command(OUTPUT ${MidlOutputPath}
    #COMMAND midl /winrt /h ${MidlOutputPath} /metadata_dir "D:\\Windows Kits\\10\\References\\10.0.22621.0\\Windows.Foundation.FoundationContract\\4.0.0.0" /nomidl /reference "D:\\Windows Kits\\10\\References\\10.0.22621.0\\Windows.Foundation.FoundationContract\\4.0.0.0\\Windows.Foundation.FoundationContract.winmd" /reference "D:\\Windows Kits\\10\\References\\10.0.22621.0\\Windows.Storage.Provider.CloudFilesContract\\7.0.0.0\\Windows.Storage.Provider.CloudFilesContract.winmd" /I "d:/work/nextcloud/desktop/shell_integration/windows/CfAPI" CustomStateProvider.idl
    #COMMENT "Generating CustomStateProvider.g.h from ${MidlOutputPath}"
#)

add_library(${APPX_MANIFEST_SHELLEXT_TARGET_NAME} MODULE
    dllmain.cpp
    cfapishellintegrationclassfactory.cpp
    thumbnailprovider.cpp
    contextmenus.cpp
    customstateprovider.cpp
    CfApiShellIntegration.def
)

#add_dependencies(${APPX_MANIFEST_SHELLEXT_TARGET_NAME} CustomStateProviderImpl)

target_link_libraries(${APPX_MANIFEST_SHELLEXT_TARGET_NAME} shlwapi)

target_include_directories(${APPX_MANIFEST_SHELLEXT_TARGET_NAME}
PRIVATE "D:\\Windows Kits\\10\\Include\\10.0.22621.0\\winrt D:\\Windows Kits\\10\\Include\\10.0.22621.0\\ucrt D:\\Windows Kits\\10\\Include\\10.0.22621.0\\um D:\\Windows Kits\\10\\Include\\10.0.22621.0\\shared")

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
