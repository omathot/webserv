
import cgi
import sys
import os
import cgitb; cgitb.enable()


# args = cgi.parse()
print("<html>")
print("<head>")
print("<title>Hello - Second CGI Program</title>")
print("<html>")
print("<head>")
print("<h2>Hello WORLD</h2>")
print("<h3>")
# Enable error display (useful for debugging)
cgitb.enable()

arguments = cgi.FieldStorage()
for i in arguments.keys():
	print(arguments[i].value)

# Parse query string arguments
# form = cgi.FieldStorage()
# for i in form.keys():
#  print form[i].value
# Get the values of arg1 and arg2
# arg1 = form.getvalue('arg1')
# arg2 = form.getvalue('arg2')

# Prepare and send the HTTP response
# print("Content-Type: text/html\n")
# print(f"<html><body>")
# print(f"<h2>Argument 1: {arg1}</h2>")
# print(f"<h2>Argument 2: {arg2}</h2>")
# print(f"</body></html>")
print("</h3>")
print("</body>")
print("</html>")