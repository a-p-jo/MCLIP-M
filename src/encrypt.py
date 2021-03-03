#!/usr/bin/env python3

import os
import sys

if len(sys.argv) >= 3 :
    choice = sys.argv[1]
    filename = sys.argv[2]

    if len(sys.argv)>=4 :
        LAUCH_AFTER_DEC = eval(sys.argv[3])
    else :
        LAUCH_AFTER_DEC = False

    #Change Below Commands Based on your Setup
    EXE_CMD = "./gen"
    ENC_CMD = "gpg -c " + filename
    DEC_CMD = "gpg " + filename
    DEL_CMD = "rm " + filename

    if choice == 'e':
        os.system(ENC_CMD)
        confirm = input("Delete " + filename + " ? (Y/N) : ")
        if confirm == 'y' or cofirm == 'Y' :
            os.system(DEL_CMD)
        print("Completed.")

    elif choice == 'd':
        os.system(DEC_CMD)
        confirm = input("Delete " + filename + " ? (Y/N) : ")
        if confirm == 'y' or confirm == 'Y' :
            os.system(DEL_CMD)
        print("Completed.")
        if LAUCH_AFTER_DEC :
            os.system(EXE_CMD)
    else :
        print("Incorrect Option \"",sys.argv[2],"\".")
else : 
    print("Insufficient Arguments.")
