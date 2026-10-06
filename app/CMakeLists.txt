add_executable(radar_app main.cpp)

# Linkeamos nuestras dos librerias. Heredamos sus carpetas /include
target_link_libraries(radar_app PRIVATE 
    dsp_engine 
    sil_sim
)