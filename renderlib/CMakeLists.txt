set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glad/include")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/version.h"
)

add_library(renderlib "${CMAKE_CURRENT_SOURCE_DIR}/renderlib.cpp")
set_property(TARGET renderlib PROPERTY AUTOMOC ON)

target_include_directories(renderlib PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
	${GLM_INCLUDE_DIRS}
	${GLAD_DIR}
)
target_sources(renderlib PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/AppScene.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/AppScene.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/CCamera.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/command.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/command.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/FileReader.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/FileReader.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/FileReaderCCP4.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/FileReaderCCP4.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/FileReaderCzi.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/FileReaderCzi.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/FileReaderTIFF.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/FileReaderTIFF.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/FileReaderZarr.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/FileReaderZarr.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Flags.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Flags.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Framebuffer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Framebuffer.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Fuse.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Fuse.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/GradientData.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/GradientData.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Histogram.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Histogram.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/IFileReader.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/ImageXYZC.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ImageXYZC.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/ImageXyzcGpu.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ImageXyzcGpu.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/IRenderWindow.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/IRenderWindow.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Logging.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Logging.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/RenderGL.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/RenderGL.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/RenderGLPT.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/RenderGLPT.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/RenderSettings.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/RenderSettings.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Status.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Status.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/threading.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/threading.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Timeline.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Timeline.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Timing.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Timing.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/version.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/VolumeDimensions.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/VolumeDimensions.h"
)
add_subdirectory(gl)
add_subdirectory(glad/src)
add_subdirectory(glsl)
add_subdirectory(pugixml)

# libCZI dependency for renderlib
# add_compile_definitions(_LIBCZISTATICLIB)
set(LIBCZI_DO_NOT_SET_MSVC_RUNTIME_LIBRARY ON)
set(LIBCZI_BUILD_UNITTESTS OFF)
set(LIBCZI_BUILD_DYNLIB ON)
set(LIBCZI_BUILD_CZICMD OFF)
add_subdirectory(libCZI)

# end libczi dependency

# zstd dependency for tensorstore / riegeli
FetchContent_Declare(
	zstd
	URL "https://github.com/facebook/zstd/archive/v1.5.5.tar.gz"
	URL_HASH SHA256=98e9c3d949d1b924e28e01eccb7deed865eefebf25c2f21c702e5cd5b63b85e1
)

# Additional FetchContent_Declare calls as needed...
FetchContent_MakeAvailable(zstd)

# tensorstore dependency for renderlib
set(TENSORSTORE_USE_SYSTEM_TIFF ON)
FetchContent_Declare(
	tensorstore
	URL "https://github.com/google/tensorstore/archive/refs/tags/v0.1.36.tar.gz"
	URL_HASH SHA256=5857582B1B68E5A3F470D2EE8E9A7FA1AD6E6C7E0867C7C69E5A523AE00BC002
)

# Additional FetchContent_Declare calls as needed...
FetchContent_MakeAvailable(tensorstore)

# end tensorstore dependency
target_link_libraries(renderlib
	Qt::Widgets Qt::Core Qt::Gui Qt::Network Qt::OpenGL Qt::OpenGLWidgets Qt::WebSockets Qt::Xml
	${CMAKE_DL_LIBS}
	${TIFF_LIBRARIES}
	libCZI

	tensorstore::tensorstore tensorstore::all_drivers

	${OPENGL_egl_LIBRARY}
	spdlog::spdlog_header_only
)

IF(WIN32)
	target_link_libraries(renderlib glm::glm)
ENDIF(WIN32)
