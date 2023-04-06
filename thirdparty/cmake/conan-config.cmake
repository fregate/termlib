set(PACKAGE_NAME conan)

include("${conan_DIR}/conan/conan.cmake")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${PROJECT_BINARY_DIR})

conan_cmake_configure(REQUIRES ${conan_FIND_COMPONENTS}
	GENERATORS cmake_find_package cmake)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
	BUILD missing
	REMOTE conancenter
	# INSTALL_FOLDER ${CMAKE_BINARY_DIR}
	SETTINGS ${settings})
