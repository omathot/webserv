#!/usr/bin/env python3
import os
import sys
import cgi
import cgitb

cgitb.enable()

def print_env_vars():
    print("Environment Variables:")
    for key, value in os.environ.items():
        print(f"{key}: {value}")

def print_form_data(form):
    print("\nForm Data:")
    for key in form.keys():
        item = form[key]
        if item.filename:
            print(f"File Input: {key}")
            print(f"  Filename: {item.filename}")
            print(f"  Type: {item.type}")
            print(f"  Size: {len(item.value)} bytes")
            print(f"  First 100 bytes: {item.value[:100]}")
        else:
            print(f"{key}: {form.getvalue(key)}")

def main():
    print("Content-Type: text/plain\n")
    
    print_env_vars()
    
    try:
        form = cgi.FieldStorage()
        print_form_data(form)
    except Exception as e:
        print(f"\nError parsing form data: {str(e)}")
        print("\nRaw POST data:")
        sys.stdout.flush()
        sys.stdout.buffer.write(sys.stdin.buffer.read())

if __name__ == "__main__":
    main()