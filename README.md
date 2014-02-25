Solder:Time DeskClock
=====================

This is the source tree for the [Spikenzie Lab's Solder:Time DeskClock](http://www.spikenzielabs.com/Catalog/index.php?main_page=product_info&cPath=44&products_id=842).
The [official sources](http://www.spikenzielabs.com/Downloadables/STDESKCLOCK/STDC_SketchV1.zip)
are an Arduino sketch that has quite a bit of flicker (due to a
debugging change?) in the main loop.  These sources fix that, as
well as some other cleanup.

[trmm.net/SolderTime Desk Clock](http://trmm.net/SolderTime_Desk_Clock)

Other changes to be made:

* Move all font and character drawing code into separate routine.
* Build a state machine that implements the MODE/SET/timeout transitions
* Fix the timer interrupt to be more consistent; busy loops now to avoid errors
* Improve startup speed
