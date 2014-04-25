#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# file: test-pyZm.py
#
# Copyright (C) 2014 Frans Fuerst

""" this script is both - a CLI for zeitmaschine and a reference for
    other implementations
"""

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
    block = '\xe2\x96\x88'
    block = '\xe2\x96\xa0'
    print("local [%s%s%s] - remote [%s%s%s] - consistent [%s%s%s] - items [%d]" % (
        colorama.Fore.YELLOW if model.base().hasLocalChanges() else colorama.Fore.GREEN,
        block, colorama.Fore.RESET,
        colorama.Fore.YELLOW if model.base().hasRemoteChanges() else colorama.Fore.GREEN,
        block, colorama.Fore.RESET,
        colorama.Fore.GREEN if model.base().isConsistent() else colorama.Fore.RED,
        block, colorama.Fore.RESET,
        model.base().getItemCount()))
    #BLACK', 'BLUE', 'CYAN', 'GREEN', 'MAGENTA', 'RED', 'RESET', 'WHITE', 'YELLOW',

def show_local_diff(model):
    print( "local changes:" )
    for c in model.base().diffLocal():
        print ( "  %s" % c )

def show_remote_diff(model):
    print( "remote changes:" )
    for c in model.base().diffRemote():
        print ( "  %s" % c )

def list_all(model):

    logging.debug("found %d items total", model.base().getItemCount())

    spacer = ' '
    show_tags = False

#    for i in model.getItems():
#        print i[:6], yield_n(model.getCaption(i), 30)
    print     ("ID    TYPE   CAPTION" )
    print     ("----  -----  ----" )

    for i in model.base().query("interim_filter_tags"
                                " -gtd_inbox"
                                " -gtd_task"
                                " -gtd_done"
                                " -gtd_project"
                                " -gtd_item_done"
                                " -knowledge"):
        tags = model.base().getTags(i)
        if model.base().isTag(i): continue
        print( "%s%s  [NONE] %s %s"  % (
                colorama.Fore.YELLOW,
                i[:4],
                yield_n(model.base().getCaption(i), 80, spacer),
                tags if show_tags else "|"))

    for i in model.getInboxItems(False):
        tags = model.base().getTags(i)
        print( "%s%s  [INB]  %s %s" % (
                colorama.Fore.RED,
                i[:4],
                yield_n(model.base().getCaption(i), 80, spacer),
                tags if show_tags else "|"))

    for i in model.getTaskItems(True, False):
        tags = model.base().getTags(i)
        print ( "%s%s  [TASK] %s %s" % (
                colorama.Fore.BLUE,
                i[:4],
                yield_n(model.base().getCaption(i), 80, spacer),
                tags if show_tags else "|"))

    for i in model.getProjectItems(True, False):
        tags = model.base().getTags(i)
        print ( "%s%s  [PROJ] %s %s" % (
                colorama.Fore.GREEN,
                i[:4],
                yield_n(model.base().getCaption(i), 80, spacer),
                tags if show_tags else "|"))

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

        elif args[0] == "rm":
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

        elif args[0] == "rm-tag":
            if len(args) is not 2:
                print "invalid number of arguments"
            else:
                gtd_model.base().eraseItem(args[1])
                modifications_done = True

        elif args[0] == "task":
            text = " ".join(args[1:])
            print("create task with caption '%s'" % text)
            new_item = gtd_model.createNewInboxItem(text)
            gtd_model.castToProject(new_item)
            logging.info("created new item %s with caption '%s'", new_item, text)
            modifications_done = True

        elif args[0] == "done":
            done_uids = to_uids(args[1:], gtd_model)
            if done_uids:
                print("set items to done: %s" % done_uids)
                for i in done_uids:
                    gtd_model.setDone(i)
                modifications_done = True
            else:
                print("not all given items map to a unique existing one - abort")
                show_list = False

        elif args[0] == "dismiss":
            dismiss_uids = to_uids(args[1:], gtd_model)
            if dismiss_uids:
                print("dismiss items: %s" % dismiss_uids)
                for i in dismiss_uids:
                    gtd_model.setDismissed(i)
                modifications_done = True
            else:
                print("not all given items map to a unique existing one - abort")
                show_list = False

        elif args[0] == "rename":
            show_list = False
            if len(args) < 3:
                print('too few args')
            else:
                uid = gtd_model.base().completeUid(args[1])
                caption = " ".join(args[2:])
                print("rename '%s' to '%s'" % (uid, caption))
                gtd_model.base().setCaption(uid, caption)
                modifications_done = True

        elif args[0] == "cast:project":
            uids = to_uids(args[1:], gtd_model)
            if uids:
                print("cast to project: %s" % uids)
                for i in uids:
                    gtd_model.castToProject(i)
                modifications_done = True
            else:
                print("not all given items map to a unique existing one - abort")
                show_list = False

        elif args[0] == "cast:note":
            uids = to_uids(args[1:], gtd_model)
            if uids:
                print("cast to note: %s" % uids)
                for i in uids:
                    gtd_model.castToTaggedItem(i, "knowledge")
                modifications_done = True
            else:
                print("not all given items map to a unique existing one - abort")
                show_list = False

        elif args[0] == "info":
            info_uids = to_uids(args[1:], gtd_model)
            if info_uids:
                for i in info_uids:
                    caption = gtd_model.base().getCaption(i)
                    print("%s:" % (i,))
                    print("'%s'(%d)" % (caption, len(caption)))
                    print("tags: %s " % gtd_model.base().getTags(i))
            else:
                print("not all given items map to a unique existing one - abort")
            show_list = False

        elif args[0] in ("sync-pull", "pull"):
            logging.info("trigger sync")
            if gtd_model.base().sync_pull():
                modifications_done = True
            else:
                print ("did nothing")
                show_list = False

        elif args[0] in ("sync-push", "push"):
            show_list = False
            print("push changes to sync folder - you have to distribute them yourself!")
            if gtd_model.base().sync_push():
                modifications_done = True
            else:
                print ("did nothing")

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

        elif args[0] in ("search", "find"):
            show_list = False
            if len(args) >= 2:
                list_matching(gtd_model, args[1])
            else:
                print "missing pattern"

        elif args[0] == "diff":
            show_list = False
            show_local_diff(gtd_model)
            show_remote_diff(gtd_model)

        elif args[0] == "diff-local":
            show_list = False
            show_local_diff(gtd_model)

        elif args[0] == "diff-remote":
            show_list = False
            show_remote_diff(gtd_model)

        elif args[0] == "status":
            show_status(gtd_model)
            show_list = False
            show_local_diff(gtd_model)
            show_remote_diff(gtd_model)

        elif args[0] == 'help':
            print("add")
            print("task")
            print("done")
            print("dismiss")
            print("rename")
            print("cast:task")
            print("cast:project")
            print("cast:note")
            print("info")
            print("sync-pull|pull")
            print("sync-push|push")
            print("snapshot")
            print("search|find")
            print("diff")
            print("diff-local")
            print("diff-remote")
            print("status")
            print("rm")
            print("rm-tag")
            show_list = False

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
