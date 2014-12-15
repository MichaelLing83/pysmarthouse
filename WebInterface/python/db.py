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

PATH_TO_DB = "/home/pi/git/pysmarthouse/WebInterface/data/pysmarthouse.db"
ALL_IDS = ("Kitchen", "FrontYard", "SunRoom", "GuestTiolet", "MainToilet", "Bedroom_1", "Bedroom_2", "Bedroom_3")
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
    if not isinstance(p, t): raise PSMError("{0} is not of type {1}!",format(p, t))
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
        conn = sqlite3.connect(PATH_TO_DB, detect_types=sqlite3.PARSE_DECLTYPES|sqlite3.PARSE_COLNAMES)
        c = conn.cursor()
        c.execute("drop table if exists pysmarthouse")
        c.execute("create table pysmarthouse (id text, t text, v real, ts timestamp)")
    def __init__(self):
        if not os.path.isfile(PATH_TO_DB): DB.gen_new_db()
        self.conn = sqlite3.connect(PATH_TO_DB, detect_types=sqlite3.PARSE_DECLTYPES|sqlite3.PARSE_COLNAMES)
        self.c = self.conn.cursor()
    def insert(self, id, t, v, ts=datetime.datetime.now()):
        # check parameter
        check(id, str, ALL_IDS)
        check(t, str, ALL_TYPES)
        check(v, float, None)
        check(ts, datetime.datetime, None)
        # insert into database
        self.c.execute("insert into pysmarthouse(id,t,v,ts) values (?,?,?,?)", (id, t, v, ts))
        self.conn.commit()
    def get_records(self, half_delta=datetime.timedelta(hours=12), ts=datetime.datetime.now()):
        '''
        Return all records among |now()-ts| < half_delta
        Result is sorted by timestamp.
        '''
        check(half_delta, datetime.timedelta, None)
        check(ts, datetime.datetime, None)
        result = list()
        self.c.execute("select id, t, v, ts from pysmarthouse order by ts")
        record = self.c.fetchone()
        while record:
            if abs(record[-1] - ts) < half_delta: result.append(record)
            record = self.c.fetchone()
        return tuple(result)

if __name__ == '__main__':
    PATH_TO_DB = "./pysmarthouse.db"
    db = DB()
    for i in range(10):
        db.insert(ALL_IDS[i % len(ALL_IDS)], ALL_TYPES[i % len(ALL_TYPES)], float(i*2))
    for id, t, v, ts in db.get_records():
        print("{:16s}{:16s}{:8.2f}\t{:10s}".format(id, t, v, str(ts)))