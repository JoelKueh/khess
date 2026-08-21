
# Create the project.
cmake_minimum_required(VERSION 3.30.8)
project(khess VERSION 0.0.1)

# Required C version
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Compile Flags
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_compile_options(-mavx2)

set(BOARD_SOURCES
	src/cb/const.c
	src/cb/debug.c
	src/cb/gen.c
	src/cb/magical.c
	src/cb/normal.c
	src/cb/eval.c
	src/cb/cb.c
)

# Create the engine executable.
add_executable(cibyl
	${BOARD_SOURCES}
	src/cibyl.c
	src/uci.c
	src/engine.c
	src/search.c
	src/perft.c
)
target_include_directories(cibyl
	PRIVATE
		${PROJECT_SOURCE_DIR}/inc
)
target_compile_definitions(cibyl PRIVATE _POSIX_C_SOURCE=202405L)

# Create the cibyl test executable.
add_executable(cibyl_test
	${BOARD_SOURCES}
	src/uci.c
	src/engine.c
	src/search.c
	src/perft.c
	src/cb/test.c
	src/test.c
)
target_include_directories(cibyl_test
	PRIVATE
		${PROJECT_SOURCE_DIR}/inc
)
target_compile_definitions(cibyl_test
    PRIVATE
        _POSIX_C_SOURCE=202405L
        _DOCTEST_CPLUSPLUS=202302L
)

# Create the cibyl genmagics executable.
add_executable(cibyl_genmagics
    ${BOARD_SOURCES}
    src/genmagics.c
)
target_include_directories(cibyl_genmagics
	PRIVATE
		${PROJECT_SOURCE_DIR}/inc
)
target_compile_definitions(cibyl_genmagics
    PRIVATE
        _POSIX_C_SOURCE=202405L
        _DOCTEST_CPLUSPLUS=202302L
)
