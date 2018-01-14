#!/usr/bin/python3

""" This module listens to the events from Arduino and forwards them
    to the MySQL database. """

dbConfig = {
    'user'    : 'iot_write',
    'password': 'LetMeWrite',
    'host'    : '192.168.1.177',
    'port'    :  3306,
    'database': 'IoT_project'
}

codeMapping = {
    b'I' : 'in',
    b'O' : 'out',
    b'E' : 'error',
    b'R' : 'reset'
}

insertRecord = "INSERT INTO `IoT_project`.`Corridor` (`event`) VALUES (%s)"

import serial
import mysql.connector

def getConnectionToArduino():

    """ Lists serial devices and connects to first Arduino Uno."""

    from serial.tools.list_ports_linux import comports

    for p in comports():
        if p.description == "Arduino Uno":
            return serial.Serial(p.device)

    raise Exception("Arduino Not Found")

def recordEvent(dbConnection, eventName, insertStatement = insertRecord):

    """ Executes the insert statement with event name
        as an argument, then commits.

        Tries to recconect..."""

    if dbConnection.is_connected() or dbConnection.reconnect(4, 10):
        cursor = dbConnection.cursor()
        cursor.execute(insertStatement, (eventName, ))
        dbConnection.commit()

def watchdog(serialConn, dbConn, codeMap=codeMapping, insertStatement=insertRecord):

    """ Keeps reading the serial input and performs
        the database inserts."""

    while True:
        code = serialConn.read()
        if code in codeMap.keys():
            recordEvent(dbConn, codeMap[code], insertStatement)

if __name__=="__main__":

    try:
        serialConn = getConnectionToArduino();
        dbConn = mysql.connector.Connect(**dbConfig)

        watchdog(serialConn, dbConn)
    finally:
        dbConn.close()
        serialConn.close()
