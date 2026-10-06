add_library(sil_sim STATIC)

target_sources(sil_sim PRIVATE src/environment.cpp)

target_include_directories(sil_sim PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(sil_sim PUBLIC dsp_engine)