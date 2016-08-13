SET(gppanel_headers baseData.h Fourier.h Fourier.inline.h gpFormula.h
    gpLayer.h gpLineLayer.h gpMath.h gpMenu.h gpMenuGroup.h gpPanel.h
    gpPopup.h gpPopupGroup.h gpSeries.h gpSizer.h mathplot.h symbols.h)

INSTALL(FILES ${gppanel_headers} DESTINATION
    "${CMAKE_INSTALL_INCLUDEDIR}/gpPanel/")
