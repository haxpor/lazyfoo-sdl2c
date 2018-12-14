# Note

* Migrated to use VAO.  
  
  Specifically trying to get OpenGL 3.1 won't work on macOS; at least 10.14 on my machine. It always get me 4.1. The point which is noted also at tutorial's web page at the bottom is that 3.2+ needs to use VAO (vertex array object). Thus if follow along with tutorial without using VAO, then it won't work on macOS. You need to migrate to VAO as we did here.
  Read more [here](https://www.khronos.org/opengl/wiki/OpenGL_Context), excerpted below.
  > "Platform Issue (MacOSX): When MacOSX 10.7 introduced support for OpenGL beyond 2.1, they also introduced the core/compatibility dichotomy. However, they did not introduce support for the compatibility profile itself. Instead, MacOSX gives you a choice: core profile for versions 3.2 or higher, or just version 2.1. There is no way to get access to features after 2.1 and still access the Fixed Function Pipeline."
