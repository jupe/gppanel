# gpPanel

## Easy Chart panel for wxWidget
gpPanel is chart libary for wxWidget.
It inheritance from wxPanel and use modified [wxMathPlot](http://wxmathplot.sourceforge.net/) library 
at chart engine. Flexible to use and easy to implement new gpLayers from examples.

## Video
Because video explain more than thousand words, there it is:
[http://www.youtube.com/watch?v=wmz4ElnNCS4]

## News
 * 28.4.2013
   * Merged several improvements to master branch.
 * 15.7.2012
   * [New Forum](https://groups.google.com/forum/#!forum/gppanel)
 * 14.7.2012
   * more developers, so there will be more implementation in future ;)
 * 23.4.2012
   * Several updates for 
     * folder structures
     * header implementations to own files
     * new features to mathplot
 * 21.4.2012
   *Change license type to wxWindows Library License v3.1 License.
   *Major update for all part of code. a lot of code documentation added.
   *Change project version control to GIT.
     *If you want't to access it, you can send mail for me and I can add your account to commit list
 * 15.11.-09
  * wxMathPlot: 
   * bar layer ( vertical and horizontal )
    * default label (bar value, position: end of bar )
    * custom label for each of bars
 * 12.11.-09
  * Optimized mpFXY layer, when used big amount of data ( 2k .. 1M )
  * gpPanel add automatically popups for x&y axis, and main window to change chart type and axis scales. gpLayer can choice wich are enabled.
  * Update screenshots
 * 2.11.-09
  * new wxMathPlot layer: mpFXYCandleStick
 * 1.11.-09
  * Implementation is still in progress (but a lot of features is coming more..)
  * new wxMathPlot modifications:
   * Possible to change layers drawing order
   * Bounding X,Y (min/max) locks
   * mpPointLayer pop-up
   * gradient backcolour
 * 18.10.-09
  * Added new features to wxMathPlot library (look ex. screenshot 4)
  * Added my wxMathPlot library version to svn

if you're interested, please contact..


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

