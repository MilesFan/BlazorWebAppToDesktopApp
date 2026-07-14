# BlazorWebViewWrapper
Turn Blazor web app into desktop app.

## Usage
### 1
Install BlazorWebViewWrapper from nuget.org.

### 2
In the Program.cs of a Blazor web application, add the following to the top

```
using BlazorWebViewWrapper;
```
and change
```
var app = builder.Build();
```
to
```
var app = builder.BuildWithWebView("[the window title]");
```
### Done
That's it.

## FAQ
### Gettring rid of the console
It cann't be done directly.

Changing the application type from Console to Window will result in missing blazor.server.js in the final output. 

One approach is to use C++/CLR to invoke the DLL generated. I will set up another repo for that later.

## License
MIT