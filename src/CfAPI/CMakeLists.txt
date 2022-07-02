cmake_minimum_required(VERSION 3.16)

project("ShellExtServer")

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CustomStateProvider.g.h" MidlOutputPath)
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CustomStateProvider.tlb" MidlOutputPathTlb)
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CustomStateProvider.winmd" MidlOutputPathWinmd)
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Generated" GeneratedDirBackSlash)
file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} MidlOutputPathGeneral)

set(shellextServer "ShellExtServer")

add_custom_target(ShellextServerCustomStateProviderImpl
   DEPENDS ${MidlOutputPath}
)

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
   COMMAND midl /winrt /h nul /tlb ${MidlOutputPathTlb} /winmd ${MidlOutputPathWinmd} /metadata_dir "D:\\Windows Kits\\10\\References\\10.0.22621.0\\Windows.Foundation.FoundationContract\\4.0.0.0" /nomidl /reference "D:\\Windows Kits\\10\\References\\10.0.22621.0\\Windows.Foundation.FoundationContract\\4.0.0.0\\Windows.Foundation.FoundationContract.winmd" /reference "D:\\Windows Kits\\10\\References\\10.0.22621.0\\Windows.Storage.Provider.CloudFilesContract\\7.0.0.0\\Windows.Storage.Provider.CloudFilesContract.winmd" /I "d:/work/nextcloud/desktop/shell_integration/windows/CfAPI" CustomStateProvider.idl
   COMMAND cppwinrt -in ${MidlOutputPathWinmd} -comp ${MidlOutputPathGeneral} -pch pch.h -ref ${sdk_metadata_directory} -out ${GeneratedDirBackSlash} -verbose
   COMMENT "Generating CustomStateProvider.g.h from ${MidlOutputPath}"
)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

add_executable(${shellextServer} WIN32
    main.cpp
    thumbnailprovider.cpp
    contextmenus.cpp
    customstateprovider.cpp
    ShellServices.cpp
)

set_target_properties(${shellextServer} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIRECTORY}
)

add_dependencies(${shellextServer} ShellextServerCustomStateProviderImpl)

target_link_libraries(${shellextServer} Qt5::Core shlwapi RuntimeObject)

target_include_directories(${shellextServer} PRIVATE "D:\\Windows Kits\\10\\Include\\10.0.22621.0\\winrt")
target_include_directories(${shellextServer} PRIVATE ${GeneratedDirBackSlash})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /await")

target_compile_features(${shellextServer} PRIVATE cxx_std_17)

file(MAKE_DIRECTORY "${BIN_OUTPUT_DIRECTORY}/${APPX_MANIFEST_SHELLEXT_DLL_FOLDER}")

add_custom_command(TARGET ${shellextServer} POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:${shellextServer}>"
     "${BIN_OUTPUT_DIRECTORY}/shellext/$<TARGET_FILE_NAME:${shellextServer}>" 

  COMMENT "Copying ${shellextServer} to output directory")

install(TARGETS ${shellextServer} 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
)
