# cppcallback

During my search about a good possibility to generate callbacks inside c++, I've found this tutorial http://www.tutok.sk/fastgl/callback.html which is quite perfect.
I've improved the provided solution with variadic templates to have a more flexible interface.

So you can now just include the header and create very flexible callbacks with NGenerateCallback and NCallback.

NCallback<RetType, Params...> is the main callback template and the first param to the template is always the return type of the function / method the callback should refer to.

Have a look into example.cc to understand the basic usage.
