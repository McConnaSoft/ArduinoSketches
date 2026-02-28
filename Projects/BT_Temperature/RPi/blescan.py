'''
Thank you Nancy Ouyang whoever you are...
Usage:
    $ sudo python blescan.py
Automation of hcitool lescan to return MAC addresses
Requires sudo privileges
Hcitool v5.20, tested on Raspbian Linux
Hardware: Requires a Bluetooth 4.0 dongle or with built-in BLE capabilities

@params None
@returns ble_adrs - A set of MAC addresses of BLE peripherals that communicate with UART
'''
 
# pexpect - Python module for spawning child applications
# time - Python module for time access and converstions
# sys - Python module that provides access to the interpreter and variables/functions defined in it
# signal - Python module used to signal handlers
# os - Python module that provides a portable way of using os dependent functionality
# ctypes - library that provides C compatible data types, it also allows calling functions in shared libraries


import pexpect, time, sys, signal, os
from ctypes.util import find_library  # Imports the native find_library function which attempts to find a library by path/name 
SCAN_TIME = 1 # In seconds -NOTE- We may want to make this value larger if we find we have trouble finding a device
 
 # geteuid returns the effective user id for the current process -> == 0 if root
if not os.geteuid() == 0:   
    sys.exit("hictool lescan only works as root") # Throws a SystemExit exception and exits Python 
 
btlib = find_library("bluetooth") # Find the Bluetooth library
if not btlib:
    raise Exception(
        "Can't find required bluetooth libraries"  # Not sure where this exception is caught
    )
 

 # closeall - Figure out what a 'connection' is
def closeall(connection):
    isalive = connection.terminate(force=True)
    #print 'process was killed: ', isalive     # I did not comment this out
    #hciout.kill(signal.SIGTERM)
    connection.close(force = True)
 
 # gathers all BLE addresss - Figure out what a 'conn' is
def gatheradr(conn):
    start_time = time.time()                         # Initialize the start time
    print 'Scanning for addresses...'           
    ble_adrs = set()                                 # Defines an empty set
    try:                                             # Python exception handling
        for line in conn:                            # Now we are parsing the addressses - Figure out the format
            tokens = line.strip().split(' ')
            address = tokens[0]
            if len(tokens) > 1: 
                comm_type = line.strip().split(' ')[1]
            #print address
            if address != '' and comm_type == 'UART':
                ble_adrs.add(address)                # Add the address to the set
            elapsed_time = time.time() - start_time  # Check elapsed time and quit if it is exceeded
            if elapsed_time > SCAN_TIME: #in seconds
                closeall(conn)
                break
    except (pexpect.TIMEOUT):
        if len(ble_adrs) == 0:
            print 'No BLE addresses found! Are peripherals on and reset?'
            closeall(conn)
    return ble_adrs
 
 # Performs the actual bluetooth scan and returns a list of the blueooth devices
def blescan():
    ble_adrs = set()
    # pexpect.timeout thrown if no inputs, aka no addresses, are being found with hcitool lescan
    hciout = pexpect.spawn('hciconfig hci0 down');
    hciout = pexpect.spawn('hciconfig hci0 up');
    hciout = pexpect.spawn('hcitool lescan', timeout=1)  # Spans a Bluetooth LE Scan from the command line and returns the 
    i = hciout.expect(['LE Scan ...','File descriptor in bad state','failed', pexpect.TIMEOUT], timeout=1)
    if i == 0:  # Found 'LE Scan ...' in hciout -> Gather the Bluetooth LE addresses
        ble_adrs = gatheradr(hciout)
    if i == 1:  # Found 'File descriptor in bad state' in hciout
        c = True
        while c: #code to check if input acceptable
            inp = raw_input('Check if dongle is plugged in. Type "y" to continue, or type "n" to cancel.')
            if inp.lower().startswith('y'):
                hciout.terminate()
                hciout.close()
                print 'Waiting a second for dongle to initialize'
                time.sleep(1)
                hciout = pexpect.spawn('hcitool lescan', timeout=1)
                j = hciout.expect(['LE Scan ...',pexpect.TIMEOUT], timeout=1)
                if j == 0:
                    ble_adrs = gatheradr(hciout)
                    break
                if j == 1:
                    'Dongle not plugged in.'
                    closeall(hciout)
                    break
            elif inp.lower().startswith('n'):
                closeall(hciout)
                break
            else:
                print 'Did not understand command. Try again.'
    if i == 2:
        print 'Is dongle plugged in? Could not scan: ', hciout.before, hciout.after
        closeall(hciout)
    if i == 3:
        print 'Is dongle pluggeded in? Could not scan: ', hciout.before, hciout.after
        closeall(hciout)
    return ble_adrs
 
        
if __name__ == "__main__":   # This is executed if this source file is the file being executed by Python
    ble_adrs = blescan()
    if ble_adrs:
        print 'Addresses found: ', ble_adrs
    else:
        print 'Empty set. No addresses found'
 
 
        #self.handle = 'b' #!! this is the TX service on the nRF8001 adafruit breakout with callbackEcho sketch
 #= int(sys.argv[1])