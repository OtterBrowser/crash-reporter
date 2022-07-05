cmake_minimum_required(VERSION 3.1.0)

project(crash-reporter)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 11)

find_package(Qt5 5.6.0 REQUIRED COMPONENTS Core Gui Network Widgets)

set(crashreporter_src
	src/main.cpp
	src/CrashReporterDialog.cpp
)

qt5_wrap_ui(crashreporter_ui
	src/CrashReporterDialog.ui
)

add_executable(crash-reporter
	${crashreporter_src}
	${crashreporter_ui}
)

target_link_libraries(crash-reporter Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets)

install(TARGETS crash-reporter DESTINATION bin/)
