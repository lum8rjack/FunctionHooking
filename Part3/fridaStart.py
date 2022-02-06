#/usr/bin/env python3

import argparse
import frida
import os
import signal
import sys

def on_message(message, data):
    print("[on_message] message:", message, "data:", data)

# Create the parser and add the arguments
parser = argparse.ArgumentParser(description='Load frida into a specified program with a specified script')
parser.add_argument('--pid', type=int, required=False, help='Attach to the PID')
parser.add_argument('--program', type=str, required=False, help='Spawn the program to inject into')
parser.add_argument('--script', type=str, required=True, help='Load the script to that will be inject')

# Execute the parse_args() method
args = parser.parse_args()

if not args.pid and not args.program:
    print("You must provide either a program name or the PID")
    quit()

if not os.path.exists(args.script):
    print(f"{args.script} does not exist")
    quit()


# Read in the js script
jscode = open(args.script).read()
pid = None
session = None

# Spawn and attach to process
if args.program:
    try:
        pid = frida.spawn([args.program])
        print(f"Started program with PID: {pid}")
    except:
        print(f"Error starting the program: {args.program}")
        quit()

# Attach to the specified PID    
else:
    pid = args.pid

try:
    session = frida.attach(pid)
    print(f"Attached to PID: {pid}")

except:
    print(f"Error attaching to the specified PID: {pid}")
    quit()


try:
    script = session.create_script(jscode)
    script.on('message', on_message)
    script.load()

    # Waiting for input so python doesn't exit
    sys.stdin.read()
except:
    try:
        session.detach()
        # If we started the program we should kill it
        if args.program:
            # Killing the process at the end
            os.kill(pid, signal.SIGTERM)
    except:
        print("Program already exit")

    print("Exiting program")
    quit()
