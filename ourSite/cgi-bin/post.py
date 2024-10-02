#!/usr/bin/env python3
import os
import sys
import tempfile
import re

def parse_multipart_form_data(content_type, content_length):
    boundary = content_type.split("boundary=")[1].encode()
    remain = int(content_length)
    line = sys.stdin.buffer.readline()
    remain -= len(line)
    if not boundary in line:
        return None, None
    
    # Extract filename
    filename = None
    while remain > 0:
        line = sys.stdin.buffer.readline()
        remain -= len(line)
        if b'Content-Disposition' in line:
            match = re.search(b'filename="(.+?)"', line)
            if match:
                filename = match.group(1).decode('utf-8', errors='replace')
        if not line.strip():
            break
    
    # Read file content
    content = tempfile.TemporaryFile("wb+")
    prev_line = sys.stdin.buffer.readline()
    remain -= len(prev_line)
    while remain > 0:
        line = sys.stdin.buffer.readline()
        remain -= len(line)
        if boundary in line:
            content.write(prev_line.rstrip(b'\r\n'))
            content.seek(0)
            return content, filename
        content.write(prev_line)
        prev_line = line
    return None, None

def main():
    print("Content-Type: text/html\n")
    
    upload_dir = "/home/sboulain/goinfre/webserv/ourSite/Downloads"
    os.makedirs(upload_dir, exist_ok=True)

    content_type = os.environ.get('CONTENT_TYPE', '')
    content_length = os.environ.get('CONTENT_LENGTH', '-1')

    if not content_type.startswith('multipart/form-data') or int(content_length) <= 0:
        print("<html><body><h2>Error: Invalid request</h2></body></html>")
        return

    file_content, filename = parse_multipart_form_data(content_type, content_length)
    
    if file_content is None:
        print("<html><body><h2>Error: Failed to parse form data</h2></body></html>")
        return

    try:
        if filename is None:
            filename = "uploaded_file"
        
        # Sanitize filename to prevent directory traversal
        filename = os.path.basename(filename)
        
        filepath = os.path.join(upload_dir, filename)
        
        with open(filepath, "wb") as f:
            while True:
                chunk = file_content.read(8192)
                if not chunk:
                    break
                f.write(chunk)
        
        print(f"<html><body><h2>File '{filename}' uploaded successfully.</h2></body></html>")
    except Exception as e:
        print(f"<html><body><h2>Error uploading file: {str(e)}</h2></body></html>")
    finally:
        file_content.close()

if __name__ == "__main__":
    main()