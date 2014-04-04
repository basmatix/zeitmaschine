#!/bin/python

import logging
import os
import sys

def main():
    if not os.path.exists('compilation-report.txt'):
        log.error('run ../src/tooling/header_dependency_check/build-with-time-report.sh to create a time report')
        sys.exit(-1)

    lines = open('compilation-report.txt').readlines()
    lines = [l.strip('\n') for l in lines if 'TOTAL' in l or '.h.cpp' in l]

    time_per_file = []
    current_file = ''
    total_duration = 0
    for l in lines:
        log.debug( l )
        if '.h.cpp.o' in l:
            current_file = l[l.rfind('/')+1:-6]
            log.debug( current_file )
        if 'TOTAL' in l:
            duration =  float(l.split()[2]) 
            log.debug( "%s:\t'%f'" % ( current_file,duration ) )
            time_per_file.append(( current_file, duration ))
            total_duration += duration

    time_per_file = sorted(time_per_file, key=lambda x: x[1])

    for i in time_per_file: log.info(str(i))
    log.info("total: %f"%total_duration)

if __name__ == "__main__":
    logging.basicConfig(format='%(asctime)-15s [%(levelname)-7s] %(message)s')
    log = logging.getLogger()
    log.setLevel( logging.INFO )

    main()
