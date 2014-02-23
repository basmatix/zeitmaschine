#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# file: test-pyZm.py
#
# Copyright (C) 2014 Frans Fuerst

from optparse import OptionParser
import sys
import logging

import pyZm

def yield_n(string, length):
    if len(string) <= length:
        return string + '.' * (length - len(string))
    else:
        return string[:length]

def list_all(model):

    print model.getItemCount()

    for i in model.getItems():
        print i[:6], yield_n(model.getCaption(i), 30)

def main():
    parser = OptionParser()
    parser.add_option("-r", "--root", dest="root",
                      help="set root folder", metavar="PATH")
    parser.add_option("-v", "--verbose", dest="verbose", default=False,
                      action="store_true", help="set verbosity on", metavar="verbosity")
    parser.add_option("-d", "--disable-hash-check", dest="no_check_hash", default=False,
                      action="store_true", help="disabling checking checksums on startup")
    (options, args) = parser.parse_args()
    
    print args

    model = pyZm.MindMatterModel()
    
    if options.root:
        #print options.root
        model.setLocalFolder(options.root)
    if options.no_check_hash:
        model.disableHashChecking()
    
    model.initialize()
    #model.setUsedUsername( "test-user" )
    #model.setUsedHostname( "test-machine" )
    #model.setTraceLevel(4)
    
    list_all(model)
    
    
if __name__ == "__main__":
    logging.basicConfig(
        format="%(asctime)s [test] %(levelname)s %(message)s",
        datefmt="%y%m%d-%H%M%S",
        level=logging.DEBUG)
    logging.addLevelName( logging.CRITICAL, "(CRITICAL)" )
    logging.addLevelName( logging.ERROR,    "(EE)" )
    logging.addLevelName( logging.WARNING,  "(WW)" )
    logging.addLevelName( logging.INFO,     "(II)" )
    logging.addLevelName( logging.DEBUG,    "(DD)" )
    logging.addLevelName( logging.NOTSET,   "(NA)" )

    main()
