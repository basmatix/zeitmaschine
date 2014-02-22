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

def list_all():

    model = pyZm.MindMatterModel()
    #model.setLocalFolder( "./test-localfolder" )
    #model.setUsedUsername( "test-user" )
    #model.setUsedHostname( "test-machine" )
    model.disableHashChecking()
    #model.setTraceLevel(4)
    model.initialize()
    print model.getItemCount()

def main():
    parser = OptionParser()
    parser.add_option("-v", "--verbose", dest="verbose", default=False,
                      action="store_true", help="set verbosity on", metavar="verbosity")
    (options, args) = parser.parse_args()
    list_all()
    
    
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
