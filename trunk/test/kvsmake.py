#!/usr/bin/python

import sys
import os


#=============================================================================
#  Returns a list of target directories.
#=============================================================================
def GetDirectoryList( find_path ):
    list = []
    for root, dirs, files in os.walk( find_path ):

        for filename in files:

            file_path = os.path.join( root, filename )
            if os.path.splitext( filename )[1] == ".cpp":

                if not root in list:
                    list.append( root )

    return list

#=============================================================================
#  Executes kvsmake command.
#=============================================================================
def KVSMake( dir_list, option ):

    s = ";"
    if os.name == 'nt': s = "&"

    make_option = ''
    if option == 'build': make_option = "kvsmake" + s
    elif option == 'clean': make_option = "kvsmake clean" + s
    elif option == 'distclean': make_option = "kvsmake distclean" + s
    elif option == 'rebuild': make_option = "kvsmake rebuild" + s
    else:
        print( "Error: Unknown option '" + option +"'" )
        print( "Usage: python kvsmake.py [clean | distclean | rebuild]" )
        sys.exit()

    curdir = os.getcwd()
    for dir in dir_list:
        print( ">> " + dir )
        command = "cd " + dir + s
        command += "kvsmake -G" + s
        command += make_option
        command += "cd " + curdir
        os.system( command )

#=============================================================================
#  Main process.
#=============================================================================
if __name__=='__main__':
    dir_list = GetDirectoryList( '.' )

    argc = len( sys.argv )
    argv = sys.argv
    if   argc == 1: KVSMake( dir_list, 'build' )
    elif argc == 2: KVSMake( dir_list, argv[1] )
