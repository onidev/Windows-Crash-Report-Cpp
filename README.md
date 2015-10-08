# Windows-Crash-Report-Cpp
Allows you to get new windows crash logs automatically and send version/offset to an external database

When a user have a crash, you need to get the offset crash for find the bug and get your support.

For this, there are multiple solutions.
The user clic on the details of the crash window and copy the offset and gives you.

![alt tag](https://cloud.githubusercontent.com/assets/10281739/10380884/f1a88c40-6e16-11e5-8f6b-bb3275ea03da.png)

![alt tag](https://cloud.githubusercontent.com/assets/10281739/10380889/f706f528-6e16-11e5-8dfa-7002ba57333c.png)

If the user closes the window, it's always possible to get the crash log.
He need to go to the 'Event Viewer' with widows+R => eventvwr
and find the wanted log in the list, open it, get the log and gives you.

![alt tag](https://cloud.githubusercontent.com/assets/10281739/10380887/f6f04f62-6e16-11e5-9fd5-039f1fefb8b0.png)

This two solutions are manual and annoying for users and developpers.


An alternative solution consists of doing this automaticaly, and retrieve the crash report on your server.
You need a server with sql support.

1. Create a sql table:
![alt tag](https://cloud.githubusercontent.com/assets/10281739/10381038/1260d87e-6e18-11e5-838d-33e34e5d3211.png)

2. Add the php script on your server.

3. Use the evlog lib (in the lib folder) and send new changes to your php script:
od::Http::callUrl("http://mysite.com/crash_report.php?version=" + version.asString() + "&error=" + to_string(error), 3000);


It remains for you to just find crash on your program from the offset.
For this, if you use gcc, you can generate a .map file with all functions/methods offsets: -Wl,-Map,file.map
Think of generate all map files of your releases.

