#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# file: test-pyZm.py
#
# Copyright (C) 2014 Frans Fuerst

from optparse import OptionParser
import os
import sys
import logging
import colorama

import pyZm

def yield_n(string, length, padding=' '):
    if len(string) <= length:
        return string + padding * (length - len(string.decode('utf-8')))
    else:
        return string[:length]

def to_uids(args, model):
    uid_items = [model.base().completeUid(i)
                    for i in args]
    uid_items = [i for i in uid_items if len(i) == 16]
    if len(set(uid_items)) == len(args):
        return uid_items
    return None


def show_status(model):
    print("local changes:    %s" % ("yes" if model.base().hasLocalChanges() else "no"))
    print("remote changes:   %s" % ("yes" if model.base().hasRemoteChanges() else "no"))
    print("model consistent: %s" % ("yes" if model.base().isConsistent() else "no"))


def list_all(model):

    logging.debug("found %d items total", model.base().getItemCount())

#    for i in model.getItems():
#        print i[:6], yield_n(model.getCaption(i), 30)
    print     ("ID    TYPE   CAPTION" )
    print     ("----  -----  ----" )

    for i in model.base().query("interim_filter_tags"
                                " -gtd_inbox"
                                " -gtd_task"
                                " -gtd_done"
                                " -gtd_project"
                                " -gtd_item_done"):
        tags = model.base().getTags(i)
#        if model.base().isTag(i): continue
        print( "%s%s  [NONE] %s %s"  % (
                colorama.Fore.YELLOW,
                i[:4],
                yield_n(model.base().getCaption(i), 80, ' '),
                tags))

    for i in model.getInboxItems(False):
        tags = model.base().getTags(i)
        print( "%s%s  [INB]  %s %s" % (
                colorama.Fore.RED,
                i[:4],
                yield_n(model.base().getCaption(i), 80, ' '),
                tags))

    for i in model.getTaskItems(True, False):
        tags = model.base().getTags(i)
        print ( "%s%s  [TASK] %s %s" % (
                colorama.Fore.BLUE,
                i[:4],
                yield_n(model.base().getCaption(i), 80, '.'),
                tags))

    for i in model.getProjectItems(True, False):
        tags = model.base().getTags(i)
        print ( "%s%s  [PROJ] %s %s" % (
                colorama.Fore.GREEN,
                i[:4],
                yield_n(model.base().getCaption(i), 80, '.'),
                tags))

def list_matching(gtd_model, pattern):
    print     ("ID    CAPTION" )
    print     ("----  ----" )
    for i in gtd_model.find(pattern):
        print("%s  %s" %
                (i[:4], yield_n(gtd_model.base().getCaption(i), 80, ' ')))

def operate(gtd_model, args, auto_save):

    """ add
        add task
    """

    modifications_done = False
    show_list = True

    if not args == []:

        if args[0] == "add":
            text = " ".join(args[1:])
            print("create a generic item with caption '%s'" % text)
            new_item = gtd_model.createNewInboxItem(text)
            logging.info("created new item %s with caption '%s'", new_item, text )
            modifications_done = True

        if args[0] == "rm":
            while 'rm' in args: args.remove('rm')
            rm_uids = to_uids(args, gtd_model)
            if rm_uids:
                print("remove items permanently: %s" % rm_uids)
                for i in rm_uids:
                    gtd_model.base().eraseItem(i)
                modifications_done = True
            else:
                print("not all given items map to a unique existing one - abort")
                show_list = False

        if args[0] == "rm-tag":
            if len(args) is not 2:
                print "invalid number of arguments"
            else:
                gtd_model.base().eraseItem(args[1])
                modifications_done = True

        elif args[0] == "task":
            text = " ".join(args[1:])
            print("create task with caption '%s'" % text)
            new_item = gtd_model.createNewInboxItem(text)
            gtd_model.registerItemAsTask(new_item)
            logging.info("created new item %s with caption '%s'", new_item, text)
            modifications_done = True

        elif 'done' in args:
            while 'done' in args: args.remove('done')
            done_uids = to_uids(args, gtd_model)
            if done_uids:
                print("set items to done: %s" % done_uids)
                for i in done_uids:
                    gtd_model.setDone(i)
                modifications_done = True
            else:
                print("not all given items map to a unique existing one - abort")
                show_list = False

        elif args[0] == "sync-pull":
            logging.info("trigger sync")
            if gtd_model.base().sync_pull():
                modifications_done = True
            else:
                print ("did nothing")
                show_list = False

        elif args[0] == "sync-push":
            logging.info("trigger sync")
            if not gtd_model.base().sync_push():
                print ("did nothing")
                show_list = False

        elif args[0] == "snapshot":
            print gtd_model.base().getLoadedJournalFiles()
            if len(args) >= 2:
                if args[1] == 'save':
                    print "save"
                    gtd_model.base().createSnapshot()
                    pass
                elif args[1] == 'load':
                    print "load"
                    gtd_model.base().loadSnapshot()
                    pass

        elif args[0] == "search":
            show_list = False
            if len(args) >= 2:
                list_matching(gtd_model, args[1])
            else:
                print "missing pattern"

        else:
            print ("'%s' not a valid command" % args[0])
            show_list = False


    if show_list:
        list_all(gtd_model)
        show_status(gtd_model)

    if modifications_done:
        if auto_save:
            print("modifications done, save..")
            if gtd_model.base().isConsistent():
                gtd_model.base().saveLocal()
            else:
                print "inconsistent model! - abort saving"
        else:
            print("saving disabled on command line!")

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
    elif os.path.exists("./zm-local"):
        gtd_model.base().setLocalFolder(os.path.abspath("./zm-local"))

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

    operate(gtd_model, args, options.auto_save)
    
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
