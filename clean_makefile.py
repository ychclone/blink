import glob
import os

removeMaskList = [
"*.o",
"Makefile*.*",
"Makefile*",
"*.Debug",
"*.Release",
]

for removeMask in removeMaskList:
     globList = glob.glob(removeMask)
     for globItem in globList:
        os.remove(globItem)

    
