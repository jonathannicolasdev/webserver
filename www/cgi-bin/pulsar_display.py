import os

assert os.environ["REQUEST_METHOD"] == "POST"

content_length = 0
query = "OOPS MISSING CONTENT !"
ret_string = ""
query_map = dict()

def process_query_string(query_str: str):
	split_query = query.split("&")
	query_map = {q.split('=')[0]: q.split('=')[1] for q in split_query}
	return (query_map)


if ("CONTENT_LENGTH" in os.environ):
	if (os.environ["CONTENT_LENGTH"] == ""):
		content_length = 0
	else:
		content_length = int(os.environ["CONTENT_LENGTH"])

	if (content_length > 0):
		query = input()
		query_map = process_query_string(query)
		#split_query = query.split("&")
		#query_map = {q.split('=')[0]: q.split('=')[1] for q in split_query}
elif ("QUERY_STRING" in os.environ):
	query = os.environ["QUERY_STRING"]
	query_map = process_query_string(query)
	#split_query = query.split("&")
	#query_map = {q.split('=')[0]: q.split('=')[1] for q in split_query}

#"            position: relative; /* Make the parent element relative for positioning */" +\
def send_pulsar_html(bg_col,
						puls_color1, puls_color2, puls_color3,
						puls_size1, puls_size2, puls_size3):
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
	f"            background-color: {bg_col};" +\
	"        }" +\
	"        " +\
	"        #circle1 {" +\
	f"            width: {puls_size1};" +\
	f"            height: {puls_size1};" +\
	f"            background-color: {puls_color1};" +\
	"            border-radius: 50%;" +\
	"            opacity: 0.95;" +\
	"            animation: pulse 3s ease-in-out infinite;" +\
	"" +\
	"        }" +\
	"        #circle2 {" +\
	f"            width: {puls_size2};" +\
	f"            height: {puls_size2};" +\
	f"            background-color: {puls_color2};" +\
	"            border-radius: 50%;" +\
	"            opacity: 0.9;" +\
	"            animation: pulse 3s ease-in-out infinite;" +\
	"        }" +\
    "        #circle3 {" +\
	f"            width: {puls_size3};" +\
	f"            height: {puls_size3};" +\
	f"            background-color: {puls_color3};" +\
	"            border-radius: 50%;" +\
	"            opacity: 0.8;" +\
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
	"    <div id=\"circle1\"></div>" +\
	"    <div id=\"circle2\"></div>" +\
	"    <div id=\"circle3\"></div>" +\
	"</body>" +\
	"</html>"

	print(ret_string)
 
 
send_pulsar_html(query_map.get("bg", "navy"),
                 query_map.get("p1color", "yellow"), query_map.get("p2color", "yellow"), query_map.get("p3color", "yellow"),
                 query_map.get("p1size", "50px"), query_map.get("p2size", "50px"), query_map.get("p3size", "50px"))
#print(query_map)