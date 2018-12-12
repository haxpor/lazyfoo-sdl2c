# Changes from original

* Use arbitrary set size for rendertarget texture then render on screen. This is better to customize size of render target than referencing to window's dimensions all over the places.
* Clear bg color to black for rendertarget (but not seen due to other stuff drawn over) to differentiate it from other content drew on main renderer.

