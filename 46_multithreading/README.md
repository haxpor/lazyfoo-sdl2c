# Changes from original

* modify thread to run in while loop instead one time execution while also monitoring shared variable `tshared_is_extra_thread_stop` to break the loop and return from it. Use space to chage value of this variable to `true` from main thread (which only main thread can modify such value).
* Use `LTimer` to help in simulate a loop in thread.
