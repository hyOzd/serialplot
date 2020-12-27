# Based on: https://github.com/mhoeher/opentodolist
#
# Note: we extract linuxdeployqt appimage so that it can run in docker, that's
# because fuse doesn't work in docker.

set(LINUXDEPLOYQT_URL "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage")
set(LINUXDEPLOYQT_APPIMAGE ${CMAKE_CURRENT_BINARY_DIR}/linuxdeployqt-continuous-x86_64.AppImage)
set(LINUXDEPLOYQT_TOOL ${CMAKE_CURRENT_BINARY_DIR}/squashfs-root/AppRun)

set(APPIMAGE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PROGRAM_NAME}-${VERSION_STRING}-${CMAKE_HOST_SYSTEM_PROCESSOR})

add_custom_command(
    OUTPUT
        ${LINUXDEPLOYQT_TOOL}
    COMMAND
        wget ${LINUXDEPLOYQT_URL}
    COMMAND
        chmod a+x ${LINUXDEPLOYQT_APPIMAGE}
    COMMAND
        ${LINUXDEPLOYQT_APPIMAGE} --appimage-extract)

add_custom_target(
    appimage

    DEPENDS ${LINUXDEPLOYQT_TOOL}

    COMMAND
        ${CMAKE_COMMAND} -E remove_directory ${APPIMAGE_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${APPIMAGE_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROGRAM_NAME}> ${APPIMAGE_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E copy ${DESKTOP_FILE} ${APPIMAGE_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E copy ${ICON_FILE} ${APPIMAGE_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E env PATH=${QT_INSTALL_PREFIX}/bin:$ENV{PATH} ${LINUXDEPLOYQT_TOOL}
            ${APPIMAGE_DIR}/${PROGRAM_NAME} -appimage
            -always-overwrite -bundle-non-qt-libs -unsupported-allow-new-glibc -verbose=2
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR})
