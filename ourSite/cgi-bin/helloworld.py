#!/usr/bin/env python3
import cgi
import cgitb

# Enable CGI traceback
cgitb.enable()

# Print necessary HTTP headers

# HTML content
print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Hello - CGI Program</title>
</head>
<body>
    <h2>Hello WORLD</h2>
    <h3>Form Data:</h3>
""")

# Process form data
form = cgi.FieldStorage()
for field in form.keys():
    print(f"    <p>{field}: {form[field].value}</p>")

print("""
</body>
</html>
""")