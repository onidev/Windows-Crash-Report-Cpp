# Windows-Crash-Report-Cpp
This library automatically grabs newly occurred Windows crash logs and sends the version/offset to an external online database for you to review.

When a crash occurs on the user side, it is crucial for you to know the crash offset value in order to find the bug and provide support.

There are several ways of achieving this. For instance, the user can click on the crash windows details button and copy the offset value for you.

![alt tag](https://cloud.githubusercontent.com/assets/10281739/10380884/f1a88c40-6e16-11e5-8f6b-bb3275ea03da.png)

![alt tag](https://cloud.githubusercontent.com/assets/10281739/10380889/f706f528-6e16-11e5-8dfa-7002ba57333c.png)

In the case where the user closes the crash window, it is still possible to grab the crash log. The standard procedure is for the user to open the "Event Viewer" (Windows+R > eventvwr), look for the specific log in the list, open it and copy the offset value for you.

![alt tag](https://cloud.githubusercontent.com/assets/10281739/10380887/f6f04f62-6e16-11e5-9fd5-039f1fefb8b0.png)

These two approaches involve manual control and are annoying for both users and developers.

Our alternative approach consists of performing the crash lookup automatically and will save the crash report on your server. You'll need a server with SQL support.

1. Create a sql table:
![alt tag](https://cloud.githubusercontent.com/assets/10281739/10381038/1260d87e-6e18-11e5-838d-33e34e5d3211.png)

2.Upload the PHP script on your server.

3. Use the evlog lib (in the lib folder) and adapt your PHP script accordingly: 
```C++
od::Http::callUrl("http://mysite.com/crash_report.php?version=" + err.version.asString() + "&error=" + to_string(err.offset), 3000);
```

All that's left for you is to look up the received crash offset value and find the origin of the bug in your application. To achieve this, using gcc, you can generate a .map file containing all functions/methods offsets: -Wl,-Map,file.map

It is good practice to generate map files for each of your releases.

