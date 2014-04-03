#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# file: test-pyZm.py
#
# Copyright (C) 2014 Frans Fuerst

from optparse import OptionParser
import sys
import logging
import colorama

import pyZm

def yield_n(string, length, padding=' '):
    if len(string) <= length:
        return string + padding * (length - len(string.decode('utf-8')))
    else:
        return string[:length]

def list_all(model):

    logging.debug("found %d items total", model.base().getItemCount())

#    for i in model.getItems():
#        print i[:6], yield_n(model.getCaption(i), 30)
    print     ("ID    TYPE   CAPTION" )
    print     ("----  -----  ----" )

    for i in model.getInboxItems(False):
        print(colorama.Fore.RED   + "%s  [INB]  %s" %
                (i[:4], yield_n(model.base().getCaption(i), 80, ' ')))

    for i in model.getTaskItems(True, False):
        print (colorama.Fore.BLUE + "%s  [TASK] %s" %
                (i[:4], yield_n(model.base().getCaption(i), 80, '.')))

    for i in model.getProjectItems(True, False):
        print ("%s  [PROJ] %s" %
                (i[:4], yield_n(model.base().getCaption(i), 80, '.')))


def operate(gtd_model, args):

    """ add
        add task
    """

    modifications_done = False

    if not args == []:

        if args[0] == "add":
            text = " ".join(args[1:])
            new_item = gtd_model.createNewItem(text)
            logging.info("created new item %s with caption '%s'", new_item, text )
            modifications_done = True

        if args[0] == "task":
            text = " ".join(args[1:])
            new_item = gtd_model.createNewInboxItem(text)
            logging.info("created new item %s with caption '%s'", new_item, text)
            modifications_done = True

        if args[0] == "sync":
            logging.info("trigger sync")
            gtd_model.base().sync()
            modifications_done = True

        if args[0] == "snapshot":
            print gtd_model.base().getLoadedJournalFiles()
            if len(args) < 2:
                pass
            elif args[1] == 'save':
                print "save"
                gtd_model.base().createSnapshot()
                pass
            elif args[1] == 'load':
                print "load"
                gtd_model.base().loadSnapshot()
                pass

    list_all(gtd_model)

    if modifications_done:
        gtd_model.base().saveLocal()


def main():

    parser = OptionParser()

    parser.add_option("-r", "--root", dest="root",
                      help="set root folder", metavar="PATH")

    parser.add_option("-v", "--verbose", dest="verbose",
                      default=False, action="store_true",
                      help="set verbosity on", metavar="verbosity")

    parser.add_option("-d", "--disable-hash-check", dest="no_check_hash",
                      default=False, action="store_true",
                      help="disable checksum check on load")

    parser.add_option("-n", "--no-auto-save", dest="auto_save",
                      default=True, action="store_false",
                      help="disabling checking checksums on startup")

    (options, args) = parser.parse_args()

    model_basic = pyZm.MindMatterModel.create()
    gtd_model = pyZm.zmGtdModel(model_basic)

    if options.root:
        #print options.root
        gtd_model.base().setLocalFolder(options.root)
    if options.no_check_hash:
        gtd_model.base().disableHashChecking()
    if options.verbose:
        gtd_model.base().setTraceLevel(4)
        logging.getLogger().setLevel(logging.DEBUG)

    if args is not []:
        logging.debug("positional args: %s", args)

    gtd_model.initialize()

    if gtd_model.empty():
        print("there are no loaded items. if that feels strange try to sync.")

    logging.debug("use root folder '%s'",model_basic.getLocalFolder())

    operate(gtd_model, args)
    
if __name__ == "__main__":
    colorama.init(autoreset=True)
    logging.basicConfig(
        format="%(asctime)s %(levelname)s %(message)s",
        datefmt="%y%m%d-%H%M%S",
        level=logging.WARNING)
    logging.addLevelName( logging.CRITICAL, "(CRITICAL)" )
    logging.addLevelName( logging.ERROR,    "(EE)" )
    logging.addLevelName( logging.WARNING,  "(WW)" )
    logging.addLevelName( logging.INFO,     "(II)" )
    logging.addLevelName( logging.DEBUG,    "(DD)" )
    logging.addLevelName( logging.NOTSET,   "(NA)" )

    main()
