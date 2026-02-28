#!/usr/bin/env python

# With help from https://github.com/msaunby/ble-sensor-pi/blob/master/sensortag/sensortag.py
# Requires sudo privileges
# Hcitool v5.20, tested on Raspbian Linux
# Hardware: Requires a Bluetooth 4.0 dongle or with built-in BLE capabilities


# sys - Python module that provides access to the interpreter and variables/functions defined in it
# os - Python module that provides a portable way of using os dependent functionality
# ctypes - library that provides C compatible data types, it also allows calling functions in shared libraries
# pexpect - Python module for spawning child applications
# traceback - Python module that allows you to extract, format, and print stack traces of Python programs
# threading - Python module that provides high-level threading interface over top the lower-level thread module
# Queue - Python module that create multi-produce, multi-consumer queues
# time - Python module for time access and converstions
# socket - Python module that provides access to the BSD socket interface
# select - Python module that provides the select and poll socket methods

# custom source
# bleconstants - External constants for this server
# blescan - Allows user to scan for BLE devices that support UART

import os, sys
from ctypes.util import find_library
import pexpect, traceback, threading, Queue, time, socket, select
import blescan
import sqlanydb


 # geteuid returns the effective user id for the current process -> == 0 if root
if not os.geteuid() == 0:
    sys.exit("script only works as root")

btlib = find_library("bluetooth")  # Load the bluetoot library
if not btlib:
    raise Exception(
        "Can't find required bluetooth libraries"
    )

dbconn = sqlanydb.connect(uid='dba', pwd='sql', eng='tempdb_mcconnellpi')

keepAlive = 1;

#class BLEListenerInterceptor:
    #def write(self, text):
        #if text.find('Notification handle') >= 0:
            #sys.__stdout__.write('BOOYA!');
        #sys.__stdout__.write("Mod: " + text);

class bleBot:  # This desperatly needs to be renamed

    # Initaiizer is passed a list of BLE client addressses
    def __init__( self, ble_adr ):
        #sys.stdout = BLEListenerInterceptor();
        self.ble_adr = ble_adr
        # Gatttool is a tool distributed along with Bluez, the default linux bluetooth stack, to interact with Bluetooth Low Energy (BLE) devices
        self.con = pexpect.spawn('gatttool -b ' + self.ble_adr + ' -I -t random > stdout')   # Open a session for the list of BLE devices
        #self.con.stdout = BLEListenerInterceptor();
        self.con.delaybeforesend = 0 # This is essential
        self.con.expect('\[LE\]', timeout=1)
        self.handle = 'b' # TX service on the nRF8001 adafruit breakout
        self.listenhandle = 'e' # Handle for enabling nRF8001 listening
        self.sensorid = 0

    def connect( self ):
        # Dependent of version 5.2 of hcitool
        print "Preparing to connect. Address: " + self.ble_adr
        self.con.sendline('connect')
        try:
            self.con.read_nonblocking(2048,0) # We must flush the read pipe
        except:
                pass

        # i = self.con.expect(['Attempting blah to connect to', 'Error'], timeout=10) # Need to test the scenarios
        i = 0
        if i == 0:
            #print 'Attempting to connect'
            j = self.con.expect(['Connection successful', 'No route', 'busy', pexpect.TIMEOUT], timeout = 10) # Need to test the scenarios
            if j == 0:
                    print self.ble_adr, ': connected!'
                    #Successful connection, add this sensor to the database if it hasn't been already
                    curs = dbconn.cursor()
                    sql = "SELECT NAME FROM SENSOR WHERE NAME = '%s'" % str(self.ble_adr)
                    curs.execute(sql)
                    rowset = curs.fetchone()
                    if len(rowset) == 0:
                        # We need to inserrt the SENSOR
                        print 'Creating Sensor record in Database for: ' + str(self.ble_adr)
                        insertcurs = dbconn.cursor()
                        insertcurs.execute("INSERT INTO SENSOR ( NAME ) VALUES ('%s')" % str(self.ble_adr))
                        #insertsql = "INSERT INTO SENSOR ( NAME ) VALUES ('Tim')"
                        #insertcurs.execute(sql,(self.ble_adr,))
                        dbconn.commit();
                        insertcurs.close();
                    else:
                        print 'Sensor already exists in database...'
                    curs.close()
                    # Get the sensor id for this sensor from the database
                    curs = dbconn.cursor()
                    sql = "SELECT SENSOR_ID FROM SENSOR WHERE NAME = '%s'" % str(self.ble_adr)
                    curs.execute(sql)
                    row = curs.fetchone()
                    self.sensorid = row[0];
                    print 'Using sensor ' + str(self.ble_adr) + ' with database id: ' + str(self.sensorid) 

            if j == 1:
                    print self.ble_adr, ': No route to host, is USB dongle plugged in?'
                    self.cleanup()
            if j == 2:
                    print self.ble_adr, ': Device busy, is something else already connected to it?'
                    self.cleanup()
            if j == 3:
                    print 'Attempting to connect, is device on and in range?'
                    self.cleanup()
        if i == 1:
            print 'Is USB dongle plugged in?'
            self.cleanup()
        return self


    def char_write_cmd( self, value ):
        cmd = 'char-write-cmd 0x%s %s' % (self.handle, value)
        #print self.ble_adr, cmd
        self.con.sendline( cmd )
        try:
                print self.con.read_nonblocking(2048,0) #flush the read pipe!! SUPER IMPORTANT
        except:
                pass
        #print 'After sending command, before: ', self.con.before, 'after :', self.con.after
        return


    def cleanup( self ):
        print self.ble_adr, ': attempting to disconnect'
        try:
            self.con.sendline('disconnect')
            self.con.sendline('exit')
            try:
                    self.con.read_nonblocking(2048,0) #flush the read pipe!! SUPER IMPORTANT
            except:
                    pass
            isalive = self.con.terminate(force=True)
            print self.ble_adr, ': has been terminated? ', isalive
            self.con.close(force=True)
            #print self.ble_adr, 'is alive: ', self.con.isalive()
        except OSError:
            print self.ble_adr, ': OSError'
            pass
        return


 # Determine if I need this
def tupToHex( foolist ):
    hexed = ''
    for i in foolist :
        hexed += format(i, '02x')
    #print hexed
    return hexed

#def worker( address, commands ):
def worker( cmdQueue, connection):
    listencmd = 'char-write-req 0x%s 0100 -listen' % connection.listenhandle
    connection.con.sendline(listencmd)

    while True:
        try:
            pnum = connection.con.expect('Notification handle = .*? \r', timeout=4)
        except pexpect.TIMEOUT:
            continue
        if pnum==0:
            after = connection.con.after
            hxstr = ''.join(after.split()[5:])
            message = hxstr.decode('hex');
            print 'Message received from BLE device: ' + message;
            items = message.split(',',1)
            temperature = items[0].split(':',1)[1]
            pressure = items[1].split(':',1)[1]
            print 'Creating Sensor record in Database for: ' + str(connection.ble_adr)
            insertcurs = dbconn.cursor()
            insertcurs.execute("INSERT INTO TEMP_READING ( SENSOR_ID, TEMPERATURE, PRESSURE ) \
                                VALUES ( %s, %s, %s )" % (connection.sensorid, temperature, pressure))
            dbconn.commit()
            insertcurs.close()
            pass
        else:
            pass
        pass

def main():
        print 'Server running, ready to scan for BTLE peripherals.'
        keepAlive = 1

        connections = []
        print 'Hmmmm'
        addresses = blescan.blescan()

        for address in addresses:
            b = bleBot(address)
            connection = b.connect()
            connections.append(connection)

        threads = []
        queues = []

        for i in range(len(addresses)):
            q = Queue.Queue()
            queues.append(q)

            cmdQueue = queues[i]
            connection = connections[i]
            #t = threading.Thread(target=worker, args=(connection, cmdList))
            t = threading.Thread(target=worker, args=(cmdQueue, connection))
            t.daemon = True
            threads.append(t)

        for t in threads:
            t.start()

        exitvalue = str('')
        #while str(exitvalue) not in ['q']:
        #    exitvalue = raw_input('Enter \'q\' to quit: ')
        
        while True:
            pass

        for connection in connections:
            connection.cleanup()

        dbconn.close()    

if __name__ == "__main__":
        main()