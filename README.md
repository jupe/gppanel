# gpPanel

## Easy Chart panel for wxWidget
gpPanel is chart libary for wxWidget.
It inheritance from wxPanel and use modified [wxMathPlot](http://wxmathplot.sourceforge.net/) library 
at chart engine. Flexible to use and easy to implement new gpLayers from examples.

## Video
Because video explain more than thousand words, there it is:
[![gpPanel](http://img.youtube.com/vi/wmz4ElnNCS4/0.jpg)](http://www.youtube.com/watch?v=wmz4ElnNCS4)

## summary
  * Inheritance to wxPanel
  * chart engine is modified [wxMathPlot](http://wxmathplot.sourceforge.net/) 
   * Added:
    * point layer
     * Find nearest chart coordinates from mouse
     * wxCommandEvent to main application when mouse moved (when point layer visibled)
     * various point shapes <br>( mpCIRCLE / mpSQUARE / mpELLIPSE / mpCHECKMARK)<br>shape size is modifable
     * point position can set by main application
     * If modifications enabled, point position follows the chart
     * Point Name shown/hide
    * mpFXYCandleStick
    * mpXYBar & mpFYXBar -bar chart layer
     * customizable LSB, defaulty LSB = 1
     * labels (bar value / custom)
    * note layer with multiline support, also in msWindows
    * Mouse zoom only X-axis (middle mouse button zoom always in x-axis)
    * Possible to change layers draw order
    * Zooming boundarys
    * Markers for chart curves
    * ..more is coming...
  * gpLayer contains a number of (wxMathPlot)mpLayers
  * Integrated menu support
  * Integrated Chart tips
  * Integrated math library for:
   * FFT
   * graph means:
    * Arithmetic, Geometric, Harmonic, Quadratic, Midrange, Standard deviation, Average absolute deviation mean, Average absolute deviation median, Median, min / max. <br>Example<br>
    {{{ double aritMean = graphLayer->getArithmeticMean(); }}}
  * multiple charts to one gpPanel
   * every gpLayer features:
    * hide/show by gpPanel
    * Popup menus for:
     * X and Y -axis
      * Scale select ( also custom formula)
     * chart
      * File
       * Export image / cvs ascii-table
       * print, print preview
      * Chart
       * Default 
       * DNL
       * INL
       * Accumulation
       * FFT
       * Custom ( Y(X)= ..)
      * Edit
       * Lock
       * Grid on/off
       * Corner markers on/off
       * Enable tooltips
       * Center, Zoomin/out, 
       * Lock aspect

  * Easy to use for everyone (If you know how use wxPanel, you know how to use gpPanel ;).
  * Customizable

## Installation 

Using Cmake on Linux/BSD:
 1. mkdir cmake
 2. cd cmake
 3. ccmake .. `
   * set variables, Eg. CMAKE_PREFIX to where you want gpPanel to be installed
 4. make
 5. sudo make install

Using Cmake on windows to create Visual Studio Solution:
 1. mkdir cmake
 2. cd cmake
 3. ccmake .. -G "Visual Studio 10 Win64"
   * set variables, Eg. CMAKE_PREFIX to where you want gpPanel to be installed
   * other generators are availible other than  VS10, you can run cmake --help to see a list of them
 4. open the sln in visual studio
 5. build as you would with VS

## Example graph frame features:
  * menu items for each of gpLayers:
   * Show/Hide

## Code examples
```
..wxWindow source...


//init gpPanel
gpPanel* graphPanel = new gpPanel(parentWindow,wxNewId(),wxDefaultPosition,wxSize(240,336));

//create new line layer
gpLineLayer* lineLayer = new gpLineLayer(_("Line"), _("x-label"), _("y-label"));

// Create a data series
gpSeries* series1 = lineLayer->AddSeriesLayer("Random");

//Push data to gpLayer
for(int i=0;i<12;i++)
	series1->DataPush( i, (rand()+1)%1024);

//add gpLayer to gpPanel
graphPanel->AddLayer( lineLayer, 0 );
lineLayer->RefreshChart();
graphPanel->Fit(lineLayer);

...
```

### ..Maybe more images to here...

