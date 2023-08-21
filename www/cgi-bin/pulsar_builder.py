import os

ret_string = \
"<!DOCTYPE html>" +\
"<html>" +\
"<head>" +\
"    <title>Pulsar Builder</title>" +\
"</head>" +\
"<body>"

for k, v in os.environ.items():
    ret_string += f"    <h4>{k} : {v}</h4>"

ret_string += \
"</body>" +\
"</html>"

print(ret_string)

