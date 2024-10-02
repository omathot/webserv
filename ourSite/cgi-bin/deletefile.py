import os
import cgi
import cgitb
import urllib.parse

cgitb.enable()


# Get the filename from QUERY_STRING for DELETE request
query_string = os.environ.get('QUERY_STRING', '')
parsed_query = urllib.parse.parse_qs(query_string)

filename = parsed_query.get('filename', [None])[0]

if filename:
    # Validate and delete the file
    safe_path = os.path.abspath("your_directory/" + filename)
    if os.path.isfile(safe_path):
        os.remove(safe_path)
        print(f"<html><body><h2>File '{filename}' deleted successfully.</h2></body></html>")
    else:
        print(f"<html><body><h2>Error: File '{filename}' not found.</h2></body></html>")
else:
    print("<html><body><h2>Error: No filename provided.</h2></body></html>")