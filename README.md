# lazyfoo-sdl2c
Source code studying Lazy Foo's SDL2 tutorial in C.

Each tutroial has code ported to C based on `-std=c99`.

# How to Build

Go to each sample directory, then execute `make` to compile and build each one.  
Execute `make clean` to clean built files.

Or

Just go to root directory, then execute `make` or `make clean` to build or clean respectively for all samples.

# Credits Author

This project is based on the work of [Lazyfoo](http://lazyfoo.net/tutorials/SDL/).
See _License_ for more information about rights.

# Note

* The C port is not 100% similar to the original. May be slightly different, or more customization for learning purpose. So watch out and look at the code!
* Use `template.c`, and `template.gitignore` files to kickstart each sample.
* `LTexture.c`, `LTexture.h`, `LButton.c`, `LButton.h`, `common.c`, `common.h` are gradually improved, refactored, and added more functionality on top from one sample to another. If you need a more updated version of it, look at the most recent sample. You might have to look into multiple of samples to gather all reuseable systems as certain samples don't require to use certain systems. Anyway, when this repository is done, I will separate all those reusable systems (`.c`, and `.h`) files out into separate place.
* Each individual sample is self-contained.
* Use `createsample.sh` to kickstart creating a new sample directory based on existing one. See comment in the source file for detail how to use.

# License

* As stated in http://lazyfoo.net/contact.php, the rights of this derived works (code and assets) still belong to Lazyfoo otherwise noted.
  [LGPL](https://www.gnu.org/copyleft/lesser.html), LazyFoo, original author this repository based on.

* The code that's not based on original work is belong to me (Wasin Thonkaew) as [MIT](https://github.com/haxpor/lazyfoo-sdl2c/blob/master/LICENSE).
* Other additional assets are belong to what is noted in README.md file of corresponding sample.

If you have any enquiry about modifying or extending the project, please contact LazyFoo [here](http://lazyfoo.net/contact.php).
