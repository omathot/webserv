#!/usr/bin/env python3
import os
import sys
import urllib.parse

# Ensure that the DELETE method is intended
request_method = os.environ.get('REQUEST_METHOD', '').upper()

# Read and decode the input from sys.stdin (body of the request)
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
if content_length > 0:
    request_body = sys.stdin.read(content_length)
    parsed_body = urllib.parse.parse_qs(request_body)
else:
    # print("Content-Type: text/html\n")
    print("<html><body><h2>Error: No data in the request body.</h2></body></html>")
    exit()

# Extract the _method and filename parameters from the body
method = parsed_body.get('_method', [None])[0]
filename = parsed_body.get('filename', [None])[0]

# Validate the DELETE method and ensure filename is provided
if method and method.upper() == 'DELETE' and filename:
    # Sanitize and trim the filename to avoid directory traversal and extra spaces/newlines
    filename = os.path.basename(filename.strip())

    # Define the path to the directory where files are stored
    file_dir = "/home/omathot/dev/codam/webserv/ourServer/Downloads/"
    safe_path = os.path.join(file_dir, filename)

    # Ensure that the file is within the intended directory
    if not safe_path.startswith(file_dir):
        # print("Content-Type: text/html\n")
        print(f"<html><body><h2>Error: Invalid file path.</h2></body></html>")
        exit()

    # Check if the file exists and delete it
    if os.path.isfile(safe_path):
        try:
            os.remove(safe_path)
            # print("Content-Type: text/html\n")
            print(f"<html><body><h2>File '{filename}' deleted successfully.</h2></body></html>")
        except Exception as e:
            # print("Content-Type: text/html\n")
            print(f"<html><body><h2>Error deleting file: {str(e)}</h2></body></html>")
    else:
        # print("Content-Type: text/html\n")
        print(f"<html><body><h2>Error: File '{filename}' not found.</h2></body></html>")
else:
    # print("Content-Type: text/html\n")
    print("<html><body><h2>Error: Invalid method or filename not provided.</h2></body></html>")
