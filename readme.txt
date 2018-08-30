Usage:
Originally the DtRecord application, use to record DTU351 raw data to disk.

Modified to push any captured frames into a queue, and have a seperate thread
flush the queue. Not that it really needs a thread, but, still.

What did the changes do:
The changes would buffer LTN_LIST_ITEMS frames (4.7GB) in memory, then flush them
to disk as raw data when the DtCollector received a SIGUSR2.

The dtanalyzer app would take a raw data and look for some specific conditions I
was interested in chasing.


Compiling:

To compile with Visual Studio 2010/2012/2013/2015/2017:
- Download latest WinSDK installer: http://www.dektec.com/Products/SDK/WinSDK/Downloads/WinSDK.zip
- Run the WinSDK installer to install latest DTAPI libraries and drivers
- Open the project file and compile

To compile under linux:
- Copy DTAPI.h and DTAPI(64).o from the LinuxSDK to the IMPORT/DTAPI/ directory
- Run make, the executable will be generated in the current directory
