import os

assert os.environ["REQUEST_METHOD"] == "GET"

with open("html/pulsar_builder_page.html", "r") as file:
    ret_string = file.read()
    
print(ret_string)