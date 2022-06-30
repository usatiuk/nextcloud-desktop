if (WIN32)
    add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_WIN10)

    add_library(nextcloudsync_vfs_cfapi SHARED
        cfapiwrapper.h
        cfapiwrapper.cpp
        hydrationjob.h
        hydrationjob.cpp
        vfs_cfapi.h
        vfs_cfapi.cpp
    )
	
	add_subdirectory(shellextensions)

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shellextensions/CustomStateProvider.g.h" MidlOutputPath)
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shellextensions/CustomStateProvider.tlb" MidlOutputPathTlb)
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shellextensions/CustomStateProvider.winmd" MidlOutputPathWinmd)
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shellextensions/Generated" GeneratedDirBackSlash)
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shellextensions" MidlOutputPathGeneral)

add_custom_target(CfApiCustomStateProviderImpl
   DEPENDS ${MidlOutputPath}
)

add_dependencies(nextcloudsync_vfs_cfapi CfApiCustomStateProviderImpl)

set(WindowsSDK_IncludePath "D:\\Windows Kits\\10\\Include\\10.0.22621.0")
set(um_sdk_directory "${WindowsSDK_IncludePath}\\um")
set(shared_sdk_directory "${WindowsSDK_IncludePath}\\shared")
set(winrt_sdk_directory "${WindowsSDK_IncludePath}\\winrt")

set(sdk_metadata_directory "D:\\Windows Kits\\10\\UnionMetadata\\10.0.22621.0")
set(WindowsSDK_MetadataPath "D:\\Windows Kits\\10\\References")
set(WindowsSDK_MetadataPathVersioned "${WindowsSDK_MetadataPath}\\10.0.22621.0")
set(WindowsSDK_MetadataFoundationPath "${WindowsSDK_MetadataPathVersioned}\\Windows.Foundation.FoundationContract\\4.0.0.0")
set(WindowsSDK_MetadataCloudFilesPath "${WindowsSDK_MetadataPathVersioned}\\Windows.Storage.Provider.CloudFilesContract\\7.0.0.0")

add_custom_command(OUTPUT ${MidlOutputPath}
   COMMAND midl /winrt /h nul /tlb ${MidlOutputPathTlb} /winmd ${MidlOutputPathWinmd} /metadata_dir "D:\\Windows Kits\\10\\References\\10.0.22621.0\\Windows.Foundation.FoundationContract\\4.0.0.0" /nomidl /reference "D:\\Windows Kits\\10\\References\\10.0.22621.0\\Windows.Foundation.FoundationContract\\4.0.0.0\\Windows.Foundation.FoundationContract.winmd" /reference "D:\\Windows Kits\\10\\References\\10.0.22621.0\\Windows.Storage.Provider.CloudFilesContract\\7.0.0.0\\Windows.Storage.Provider.CloudFilesContract.winmd" /I ${MidlOutputPathGeneral} CustomStateProvider.idl
   COMMAND cppwinrt -in ${MidlOutputPathWinmd} -comp ${MidlOutputPathGeneral} -pch pch.h -ref ${sdk_metadata_directory} -out ${GeneratedDirBackSlash} -verbose
   COMMENT "Generating CustomStateProvider.g.h from ${MidlOutputPath}"
)

    target_link_libraries(nextcloudsync_vfs_cfapi PRIVATE
        Nextcloud::sync
        cldapi
        shlwapi
    )

    set_target_properties(nextcloudsync_vfs_cfapi
      PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY
          ${BIN_OUTPUT_DIRECTORY}
        RUNTIME_OUTPUT_DIRECTORY
          ${BIN_OUTPUT_DIRECTORY}
        PREFIX
          ""
        AUTOMOC
          TRUE
        LIBRARY_OUTPUT_NAME
          ${APPLICATION_EXECUTABLE}sync_vfs_cfapi
        RUNTIME_OUTPUT_NAME
          ${APPLICATION_EXECUTABLE}sync_vfs_cfapi
    )

    target_compile_features(nextcloudsync_vfs_cfapi PRIVATE cxx_std_17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /await")

    target_include_directories("nextcloudsync_vfs_cfapi" BEFORE PUBLIC ${CMAKE_CURRENT_BINARY_DIR} INTERFACE ${CMAKE_BINARY_DIR})

    target_include_directories("nextcloudsync_vfs_cfapi" PRIVATE "D:\\Windows Kits\\10\\Include\\10.0.22621.0\\winrt")
    target_include_directories("nextcloudsync_vfs_cfapi" PRIVATE ${GeneratedDirBackSlash})

    set(vfs_installdir "${PLUGINDIR}")

    generate_export_header(nextcloudsync_vfs_cfapi
        BASE_NAME nextcloudsync_vfs_cfapi
        EXPORT_MACRO_NAME NEXTCLOUD_CFAPI_EXPORT
        EXPORT_FILE_NAME cfapiexport.h
    )

    install(TARGETS nextcloudsync_vfs_cfapi
      LIBRARY DESTINATION "${vfs_installdir}"
      RUNTIME DESTINATION "${vfs_installdir}"
    )
endif()
