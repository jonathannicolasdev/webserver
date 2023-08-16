import os

script_filename = os.environ.get('SCRIPT_FILENAME', '')
directory_path = os.path.dirname(script_filename)

print("Directory Path: {}".format(directory_path))