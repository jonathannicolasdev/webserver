import os

script_filename = os.environ.get('PATH_INFO', '')
query_string = os.environ.get('QUERY_STRING', '')
directory_path = os.path.dirname(script_filename)

print("Directory Path: {}".format(directory_path))
print("Query string: {}".format(query_string))

print("Welcome to the otherside !")