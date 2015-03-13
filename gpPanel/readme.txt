  __  ___ ___  _   _  _  ___  _   
 / _|| o \ o \/ \ | \| || __|| |  
( |_n|  _/  _/ o || \\ || _| | |_ 
 \__/|_| |_| |_n_||_|\_||___||___|
 Author: Jussi Vatjus-Anttila
 (@)  jussiva@gmail.com
 (C)  Jussi Vatjus-Anttila 
      2009

 License: wxWindows Library License v3.1
 
gpPanel is wxWidget library, which purpose is to handle several charts, axis 
and chart mathematical operations. It's developed for my diplom thesis, which 
subject was of an ASIC circuit testing. The library can handle large volumes of
data. Up to several millions of points and the mathematical / statistical data.
Anyway, gpPanel main purpose is to easier use of wxMathPlot, when a needed several 
graphics and several mathematical operations for data.
	 
Library contains two parts, gpPanel and gpLayers. gpPanel derived from mpWindow -class, 
so it's the	basic class where all gpLayers are. gpPanel contains handle gpLayer visibility 
and other basic stuff. gpLayer contains all needed information how graphic is drawed and how 
layer data is handled (x/y-axis + data mathematical functions). One gpLayer contains normally 
at least one mpLayer, but it can handle also several of mpLayers. 


wxMathPlot is modification from original wxMathPlot library:
 new mpLayer:s
 -mpPoint       layer
 -mpXYBar       layer
 -mpFX2Y        layer
 -mpCandleStick layer
 
Usage:
------------------------------
Compile and link them:
-gpPanel.cpp
-baseLayers/gpLayer.cpp
 (all others are in headers)



Example:
------------------------------
#include "lineLayer.h"

gpPanel* graphPanel = new gpPanel( this, wxNewId(), wxDefaultPosition,wxSize(240,336) );

//create new line layer
lineLayer *line = new lineLayer(_("my line"), _("x-label"), _("y-label"));

//Push data to gpLayer
for(int i=0;i<12;i++)
   line->DataPush( i, (rand()+1)%1024);

//add gpLayer to gpPanel
graphPanel->AddLayer( line );

//Finally, updated graphics data
line->Refresh();
