# Changes from orginal

* `LTexture` doesn't need to have additional accessor methods as this is C, we provide direct accessibility via attributes defined in struct.
* Provide a chance for user to select whether to create which type of texture; either static or streaming (not support render target for now) instead of always creating in streaming. This is usually relating to performance, so it's better to keep creating streaming texture only as needed. See API in `LTexture.h` for more information.

