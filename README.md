# gpPanel

## Easy Chart panel for wxWidget
gpPanel is chart libary for wxWidget.
It inheritance from wxPanel and use modified [wxMathPlot](http://wxmathplot.sourceforge.net/) library 
at chart engine. Flexible to use and easy to implement new gpLayers from examples.

## Video
Because video explain more than thousand words, there it is:
[http://www.youtube.com/watch?v=wmz4ElnNCS4]

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


## Example graph frame features:
  * menu items for each of gpLayers:
   * Show/Hide

## Code examples
```
..wxWindow source...


//init gpPanel
gpPanel* graphPanel = new gpPanel(this,ID_CUSTOM1,wxDefaultPosition,wxSize(240,336));

//create new histogram layer
histogramLayer  *histogram = new histogramLayer(_("Histogram"), _("x-label"), _("y-label"));

//Push data to gpLayer
for(int i=0;i<12;i++)
   histogram->DataPush( i, (rand()+1)%1024);

//add gpLayer to gpPanel
graphPanel->AddLayer( histogram );

...
```

### ..Maybe more images to here...

