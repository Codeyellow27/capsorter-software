cmake_minimum_required (VERSION 3.12)

project (Capsorter LANGUAGES C VERSION 1.0)

set(output_bin_dir "${CMAKE_SOURCE_DIR/bin}")
set(USBDRV_LIBRARY_DIR "${PROJECT_SOURCE_DIR}/lib/usbdrv")
set(LIBUSB_LIBRARY_DIR "${PROJECT_SOURCE_DIR}/lib/LibUSB/libusb-win32-bin-1.2.6.0")


#Add the gui and controller
add_subdirectory(Capsorter)

#Add the mcu
add_subdirectory(mcu)