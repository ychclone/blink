import glob
import os

removeMaskList = [
"debug\*.ii",
"debug\*.o",
"debug\moc*.cpp",
"debug\*.exe",
"debug\qrc*.cpp",
"ui_*.h"         
]

for removeMask in removeMaskList:
     globList = glob.glob(removeMask)
     for globItem in globList:
        os.remove(globItem)


