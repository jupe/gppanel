ADD_SOURCES(gppanel baseData.cpp gpLayer.cpp gpLineLayer.cpp
    gpPanel.cpp gpSeries.cpp gpSizer.cpp mathplot.cpp)

ADD_SUBDIRECTORY(math)
ADD_SUBDIRECTORY(menu)

ADD_LIBRARY(gpPanel SHARED ${gppanel_sources})
TARGET_LINK_LIBRARIES(gpPanel ${wxWidgets_LIBRARIES})

INSTALL(TARGETS gpPanel DESTINATION ${wxWidgets_LIBRARY_DIRS})
