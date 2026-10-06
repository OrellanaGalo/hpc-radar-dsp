cmake_minimum_required(VERSION 3.20)
project(HPCRadarDSP VERSION 1.0.0 LANGUAGES CXX)

# Pido C++20 (obligatorio)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Flags para compilar.
if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Mas adelante aca vamos a buscar FFTW y Threads

# Incluimos subdirectorios
add_subdirectory(dsp_engine)
add_subdirectory(sil_sim)
add_subdirectory(app)