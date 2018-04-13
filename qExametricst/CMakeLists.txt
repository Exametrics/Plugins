cmake_minimum_required(VERSION 3.0)

option( INSTALL_QEXAMETRICSt_PLUGIN "Check to install qExametricst plugin" OFF )

if (INSTALL_QEXAMETRICSt_PLUGIN)

    project(QEXAMETRICSt_PLUGIN)
    	
    	
    	
        #target_link_libraries( ${PROJECT_NAME} QEXAMETRICST_LIB )
        include_directories( ${CloudCompare_SOURCE_DIR} )
        set(CloudCompare_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qCC)
        
        
	#we need ccOverlay classes
        file( GLOB CC_PLUGIN_CUSTOM_HEADER_LIST ${CloudCompare_SOURCE_DIR}/ccOverlayDialog*.h )
        file( GLOB CC_PLUGIN_CUSTOM_SOURCE_LIST ${CloudCompare_SOURCE_DIR}/ccOverlayDialog*.cpp )
        
        set( CC_PLUGIN_CUSTOM_UI_LIST ${CMAKE_CURRENT_SOURCE_DIR}/exametricsDialog.ui )
        include( ../../CMakePluginTpl.cmake)

endif()
