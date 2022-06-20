project (PackageWithIdentity)

set(APPX_MANIFEST_IDENTITY_VERSION ${MIRALL_VERSION_FOUR_COMPONENTS})
set(APPX_MANIFEST_IDENTITY_NAME "${APPLICATION_EXECUTABLE}.app")
set(APPX_MANIFEST_PUBLISHER "CN=${APPLICATION_VENDOR}")
set(APPX_MANIFEST_PACKAGE_NAME "${APPLICATION_SHORTNAME}")
set(APPX_MANIFEST_APPLICATION_ID "${APPLICATION_REV_DOMAIN}.${APPLICATION_EXECUTABLE}")
set(APPX_MANIFEST_APPLICAION_EXECUTABLE "${APPLICATION_EXECUTABLE}.exe")
set(APPX_MANIFEST_DISPLAY_NAME "${APPLICATION_NAME}")
set(APPX_MANIFEST_PUBLISHER_DISPLAY_NAME ${APPLICATION_VENDOR})


message("Running package-with-identity Cmake")

configure_file(AppxManifest.xml.in ${CMAKE_CURRENT_BINARY_DIR}/AppxManifest.xml)

execute_process(COMMAND "MakeAppx.exe pack /d D:/work/nextcloud/desktop/out/build/x64-Debug/package-with-identity /p D:/work/nextcloud/desktop/out/build/x64-Debug/package-with-identity/identiypackage.msix /nv"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  RESULT_VARIABLE make_msix_result
  OUTPUT_VARIABLE make_msix_ver)

message("Building MSIX:"${make_msix_result})