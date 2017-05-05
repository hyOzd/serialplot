# Based on: https://github.com/mhoeher/opentodolist

set(LINUXDEPLOYQT_URL "https://github.com/probonopd/linuxdeployqt/releases/download/1/linuxdeployqt-1-x86_64.AppImage")
set(LINUXDEPLOYQT_TOOL ${CMAKE_CURRENT_BINARY_DIR}/linuxdeployqt-1-x86_64.AppImage)

set(APPIMAGE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PROGRAM_NAME}-${VERSION_STRING})

add_custom_command(
    OUTPUT
        ${LINUXDEPLOYQT_TOOL}
    COMMAND
        wget ${LINUXDEPLOYQT_URL}
    COMMAND
        chmod a+x ${LINUXDEPLOYQT_TOOL})

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
            -always-overwrite -bundle-non-qt-libs -verbose=2
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR})
