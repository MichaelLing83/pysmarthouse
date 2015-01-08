#!/bin/env python3
'''
Database table structure:
    id text, t text, v real, ts timestamp

    Example:
        "Kitchen", "Temperature", 25.67, "2014-12-10 17:25:53.956548"
'''
import sqlite3
import datetime
import os.path
import logging

PATH_TO_DB = "/home/pi/git/pysmarthouse/WebInterface/data/pysmarthouse.db"
ALL_IDS = ("KitchenWindow", "FrontDoorStep", "SunRoom", "GuestTiolet", "MainToilet", "Bedroom_1", "Bedroom_2", "Bedroom_3")
ALL_TYPES = ("Temperature", "Relay")

class PSMError(Exception):
    pass

def check(p, t, r):
    '''
    Check a variable if they satisfy given condition.
        p: parameter to check
        t: parameter has to be of this type
        r: parameter has to be in this range
    E.g.
        check(5, int, (5,6))
        check(5, int, range(4,9))
    '''
    if not isinstance(t, type): raise PSMError("{0} is not a type!".format(t))
    if not isinstance(p, t): raise PSMError("{0} is not of type {1}!".format(p, t))
    if isinstance(r, range) or isinstance(r, tuple) or isinstance(r, list):
        if not p in r: raise PSMError("{0} is not in {1}!".format(p, r))
    elif r == None:
        # skip checking values
        pass
    else:
        if not p == r: raise PSMError("{0} != {1}".format(p, r))

class DB:
    @staticmethod
    def gen_new_db():
        global PATH_TO_DB
        try:
            logging.info("Trying to create DB at {}".format(PATH_TO_DB))
            conn = sqlite3.connect(PATH_TO_DB, detect_types=sqlite3.PARSE_DECLTYPES|sqlite3.PARSE_COLNAMES)
            logging.info("DB created at {}".format(PATH_TO_DB))
        except sqlite3.OperationalError as e:
            # path not found, possibly it's not running on the right Raspberry Pi
            # use PWD instead
            logging.warning("Path could not be found: {}".format(PATH_TO_DB))
            PATH_TO_DB = "./pysmarthouse.db"
            logging.warning("Trying to create DB at {} instead.".format(PATH_TO_DB))
            conn = sqlite3.connect(PATH_TO_DB, detect_types=sqlite3.PARSE_DECLTYPES|sqlite3.PARSE_COLNAMES)
            logging.warning("DB created at {}".format(PATH_TO_DB))
        c = conn.cursor()
        c.execute("drop table if exists pysmarthouse")
        logging.info("Table pysmarthouse is dropped should it exist.")
        c.execute("create table pysmarthouse (id text, t text, v real, ts timestamp)")
        logging.info("Table pysmarthouse created with (id text, t text, v real, ts timestamp).")
    def __init__(self):
        logging.info("Initializing DB instance...")
        if not os.path.isfile(PATH_TO_DB): DB.gen_new_db()
        self.conn = sqlite3.connect(PATH_TO_DB, detect_types=sqlite3.PARSE_DECLTYPES|sqlite3.PARSE_COLNAMES)
        self.c = self.conn.cursor()
        logging.info("Initializing DB instance done.")
    def insert(self, id, t, v, ts=datetime.datetime.now()):
        # check parameter
        check(id, str, ALL_IDS)
        check(t, str, ALL_TYPES)
        check(v, float, None)
        check(ts, datetime.datetime, None)
        # insert into database
        self.c.execute("insert into pysmarthouse(id,t,v,ts) values (?,?,?,?)", (id, t, v, ts))
        self.conn.commit()
        logging.info("Record with id={}, t={}, v={}, ts={} is inserted.".format(id, t, v, ts))
    def get_records(self, half_delta=datetime.timedelta(hours=12), ts=datetime.datetime.now()):
        '''
        Return all records among |now()-ts| < half_delta
        Result is sorted by timestamp.
        '''
        logging.debug("half_delta={}, ts={}".format(half_delta, ts))
        check(half_delta, datetime.timedelta, None)
        check(ts, datetime.datetime, None)
        result = list()
        self.c.execute("select id, t, v, ts from pysmarthouse order by ts")
        record = self.c.fetchone()
        while record:
            if abs(record[-1] - ts) < half_delta: result.append(record)
            record = self.c.fetchone()
        logging.debug("Found records: {}".format(result))
        return tuple(result)

if __name__ == '__main__':
    PATH_TO_DB = "./pysmarthouse.db"
    db = DB()
    for i in range(10):
        db.insert(ALL_IDS[i % len(ALL_IDS)], ALL_TYPES[i % len(ALL_TYPES)], float(i*2))
    for id, t, v, ts in db.get_records():
        print("{:16s}{:16s}{:8.2f}\t{:10s}".format(id, t, v, str(ts)))
