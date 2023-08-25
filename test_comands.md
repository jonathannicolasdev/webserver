# Access correct server based on host:port and server_name
curl --resolve pulsar:80:127.0.0.1 http://pulsar:80

# Limit body size (with max_body_size == 10)

curl -X POST -H "Content-Type: plain/text" --data '0123456789' localhost:80/uploads # // WORKS //
curl -X POST -H "Content-Type: plain/text" --data '0123456789a' localhost:80/uploads # // DOES NOT WORKS //

# Invalid method name
curl -X TOTO http://localhost:80/index.html

# Invalid method for route
curl -X DELETE http://localhost:80/index.html
# Valid method for route
curl -X GET http://localhost:80/index.html

# CGI test valid scripts 
curl -X GET http://localhost:80/cgi/pulsar_builder.py
curl -X POST http://localhost:80/cgi/pulsar_display.py


# CGI test scripts with errors
curl -X GET http://localhost:80/cgi/INFINITE_LOOP.py
curl -X GET http://localhost:80/cgi/mY-AwZom3_sjCriPT.py

# Siege test
siege -c20 -t10S http://localhost:80