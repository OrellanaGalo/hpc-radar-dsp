# Creamos una libreria estatica para el motor dsp
add_library(dsp_engine STATIC)

# Se aniaden los archivos fuente del motor DSP
target_sources(dsp_engine PRIVATE src/matched_filter.cpp)
target_sources(dsp_engine PRIVATE src/io.cpp)

# Quien linkee contra 'dsp_engine' automaticamente va a tener en su include path esta carpeta.
target_include_directories(dsp_engine PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Mas adelante aca vamos a linkear contra FFTW de forma privada.
target_link_libraries(dsp_engine PRIVATE fftw3f)