import glob
import os

removeMaskList = [
"release/*.ii",
"release/*.o",
"release/moc*.cpp",
"release/*.exe",
"release/qrc*.cpp",
"ui_*.h"
]

for removeMask in removeMaskList:
     globList = glob.glob(removeMask)
     for globItem in globList:
        os.remove(globItem)


