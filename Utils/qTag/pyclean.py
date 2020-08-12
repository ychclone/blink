#!/usr/bin/python

# History
# Version   Date        Creator     Remarks
# 1.0       14 Feb 08   YCH        Initial version

import glob
import os

import string
import getopt, sys

removeMaskOptionList = ['makefile', 'debug', 'release'] 

removeMaskListDict = dict()
                      
removeMaskListDict['release'] = [
"release/*.ii",
"release/*.o",
"release/moc*.cpp",
"release/*.exe",
"release/qrc*.cpp",
"ui_*.h"
]

removeMaskListDict['debug'] = [
"debug\*.ii",
"debug\*.o",
"debug\moc*.cpp",
"debug\*.exe",
"debug\qrc*.cpp",
"ui_*.h"         
]

removeMaskListDict['makefile'] = [
"*.o",
"Makefile*.*",
"Makefile*",
"*.Debug",
"*.Release",
]

def usage():
    print "pyclean"
    print "Usage [--all] [--makefile] [--debug] [--release]"
    print ""

def main():

    bClean = dict()
    for removeMaskOption in removeMaskOptionList: 
        bClean[removeMaskOption] = False # False initially

    shortOptionStr = ""
    longOptionList = removeMaskOptionList + ["all"] 

    try:
        optArgList, argList = getopt.getopt(sys.argv[1:], shortOptionStr, longOptionList)
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)

    if len(optArgList) == 0:
        usage()
        sys.exit(2)

    for opt, arg in optArgList:
        bValidOpt = False 
        for removeMaskOption in removeMaskOptionList:
            if opt == "--" + removeMaskOption:
                bClean[removeMaskOption] = True
                bValidOpt = True
        if opt == "--" + "all":
            for removeMaskOption in removeMaskOptionList:
                bClean[removeMaskOption] = True
            bValidOpt = True
            
        if bValidOpt == False:
            assert False, "unhandled option"

    for removeMaskOption in removeMaskOptionList:
        if bClean[removeMaskOption] == True:
            print removeMaskOption + ":"
            print
            currentRemoveMaskList = removeMaskListDict[removeMaskOption]

            for removeMask in currentRemoveMaskList:
                globList = glob.glob(removeMask)
                for globItem in globList:
                    print "removing " + globItem + "..."
                    os.remove(globItem) 
                    

if __name__ == "__main__":
    main()




