import os

script_filename = os.environ.get('PATH_INFO', '')
query_string = os.environ.get('QUERY_STRING', '')
directory_path = os.path.dirname(script_filename)

#print("Directory Path: {}".format(directory_path))
#print("Query string: {}".format(query_string))
#print("Welcome to the otherside !")

ret_string = \
"<!DOCTYPE html>" +\
"<html lang=\"en\">" +\
"<head>" +\
"    <meta charset=\"UTF-8\">" +\
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" +\
"    <title>Pulsating Circle</title>" +\
"    <style>" +\
"        body {" +\
"            margin: 0;" +\
"            display: flex;" +\
"            justify-content: center;" +\
"            align-items: center;" +\
"            height: 100vh;" +\
"            background-color: navy;" +\
"        }" +\
"        " +\
"        #circle {" +\
"            width: 100px;" +\
"            height: 100px;" +\
"            background-color: yellow;" +\
"            border-radius: 50%;" +\
"            opacity: 0.8;" +\
"            animation: pulse 3s ease-in-out infinite;" +\
"            position: relative; /* Make the parent element relative for positioning */" +\
"" +\
"        }" +\
"        #inner_circle {" +\
"            width: 20px;" +\
"            height: 20px;" +\
"            background-color: red;" +\
"            border-radius: 50%;" +\
"            animation: pulse 3s ease-in-out infinite;" +\
"        }" +\
"        #text {" +\
"            font-size: 48px;" +\
"            font-family: 'Orbitron', sans-serif; /* Apply custom sci-fi font */" +\
"            font-weight: bold;" +\
"            color: white;" +\
"            margin-bottom: 20px; /* Add margin to separate from circle */" +\
"            margin-right: 120px; /* Add margin to separate from circle */" +\
"        }" +\
"        " +\
"        @keyframes pulse {" +\
"            0%, 100% {" +\
"                transform: scale(0.1);" +\
"            }" +\
"            50% {" +\
"                transform: scale(3);" +\
"            }" +\
"        }" +\
"        " +\
"    </style>" +\
"</head>" +\
"<body>" +\
"    <div id=\"text\">PULSAR</div>" +\
"    <div id=\"inner_circle\"></div>" +\
"    <div id=\"circle\"></div>" +\
"    <div id=\"inner_circle\"></div>" +\
"</body>" +\
"</html>"

print(ret_string)