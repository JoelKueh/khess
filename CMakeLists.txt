
# Create the project.
cmake_minimum_required(VERSION 3.30.8)
project(khess VERSION 0.0.1)

# Compile Flags
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# Add the chessboard library.
add_library(cblib
	src/cblib/magical.c
	src/cblib/normal.c
	src/cblib/gen.c
	src/cblib/cblib.c
	src/cblib/cbconst.c
        src/cblib/cbdbg.c
)
target_include_directories(cblib
	PRIVATE
		${PROJECT_SOURCE_DIR}/include/cblib
)

# Add the debug executable.
add_executable(debug
	src/debug/debug.c
        src/debug/perft.c
)
target_include_directories(debug
	PRIVATE
		${PROJECT_SOURCE_DIR}/include/cblib
		${PROJECT_SOURCE_DIR}/include/debug
)
target_link_libraries(debug
	cblib
)

# Create the engine executable.
add_executable(cibyl
	src/cibyl/cibyl.c
)
target_include_directories(cibyl
	PRIVATE
		${PROJECT_SOURCE_DIR}/include/cibyl
)

# Create the server executable.
add_executable(kcsrv
	src/kcsrv/kcsrv.c
)
target_include_directories(kcsrv
	PRIVATE
		${PROJECT_SOURCE_DIR}/include/kcsrv
)

# Create the chess executable.
add_executable(khess
	src/khess/khess.c
)
target_include_directories(khess
	PRIVATE
		${PROJECT_SOURCE_DIR}/include/khess
)
