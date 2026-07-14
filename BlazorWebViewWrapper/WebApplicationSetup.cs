using Photino.NET;
using System.Reflection;

namespace BlazorWebViewWrapper
{
    public static class WebApplicationSetup
    {
        private static string LOADING_PAGE_HTML = @"<!DOCTYPE html>
<html lang=""zh-CN"">
<head>
    <meta charset=""UTF-8"">
    <meta name=""viewport"" content=""width=device-width, initial-scale=1.0"">
    <title>Loading</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        html, body {
            width: 100%;
            height: 100%;
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #282828;
        }
        .loading {
            width: 50px;
            height: 50px;
            border: 5px solid #e0e0e0;
            border-top-color: #282828;
            border-radius: 50%;
            animation: spin 1s linear infinite;
        }
        @keyframes spin {
            to {
                transform: rotate(360deg);
            }
        }
    </style>
</head>
<body>
    <div class=""loading""></div>
<script>
window.chrome.webview.postMessage(""WebViewInitialized"");
</script
</body>
</html>
";
        public static WebApplication BuildWithWebView(this WebApplicationBuilder builder, string WindowTitle)
        {
            lock (builder)
            {
                builder.Services.AddSingleton<WebApp>();
                var app = builder.Build();
                AutoResetEvent event_webview_ready = new AutoResetEvent(false);

                Thread thread = new Thread(async () =>
                {
                    try
                    {
                        var webapp = app.Services.GetService<WebApp>();
                        PhotinoWindow window = new PhotinoWindow();
                        if (webapp is not null)
                        {
                            webapp.Window = window;
                        }
                        window.LogVerbosity = 0;
                        window.SetTitle(WindowTitle);
                        window.StartString = LOADING_PAGE_HTML;
                        window.SetDevToolsEnabled(true);
                        EventHandler<string> myEventHandler = default!;

                        myEventHandler = (sender, webMessage) =>
                        {
                            if (sender is PhotinoWindow _window)
                            {
                                if (webMessage == "WebViewInitialized")
                                {
                                    _window.WebMessageReceived -= myEventHandler;
                                    event_webview_ready.Set();
                                }
                            }
                        };
                        window.WebMessageReceived += myEventHandler;
                        window.WaitForClose();
                        await app.StopAsync();
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex.Message);
                        Console.WriteLine(ex.InnerException?.Message);
                        throw;
                    }

                });
                //#pragma warning disable CA1416 // Only When Windows
                if (OperatingSystem.IsWindows())
                    thread.SetApartmentState(ApartmentState.STA);
                //#pragma warning restore CA1416 // Only When Windows

                thread.Start();
                var nameCallingAssembly = Assembly.GetCallingAssembly().GetName().Name ?? string.Empty;
                app.Environment.ApplicationName = nameCallingAssembly;
                app.Lifetime.ApplicationStarted.Register(() =>
                {
                    event_webview_ready.WaitOne();
                    if (app.Services.GetService<WebApp>() is WebApp webApp)
                    {
                        webApp.Window?.Load(new Uri(app.Urls.First()));
                    }
                });
                return app;
            }
        }
    }
}
