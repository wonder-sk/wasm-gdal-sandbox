add_gdal_driver(TARGET ogr_Arrow
                SOURCES ogrfeatherdriver.cpp
                        ogrfeatherdataset.cpp
                        ogrfeatherlayer.cpp
                        ogrfeatherwriterdataset.cpp
                        ogrfeatherwriterlayer.cpp
                PLUGIN_CAPABLE
                STRONG_CXX_WFLAGS)
gdal_standard_includes(ogr_Arrow)
target_include_directories(ogr_Arrow PRIVATE $<TARGET_PROPERTY:ogrsf_generic,SOURCE_DIR>)
if(${Arrow_VERSION} VERSION_GREATER_EQUAL 10.0)
    if(ARROW_USE_STATIC_LIBRARIES)
        gdal_target_link_libraries(ogr_Arrow PRIVATE Arrow::arrow_static)
    else()
        gdal_target_link_libraries(ogr_Arrow PRIVATE Arrow::arrow_shared)
    endif()
    target_compile_features(ogr_Arrow PRIVATE cxx_std_17)
else()
    if(ARROW_USE_STATIC_LIBRARIES)
        gdal_target_link_libraries(ogr_Arrow PRIVATE arrow_static)
    else()
        gdal_target_link_libraries(ogr_Arrow PRIVATE arrow_shared)
    endif()
endif()
