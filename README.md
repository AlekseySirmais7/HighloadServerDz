# Веб-сервер. Highload.
### Language: C++
### Architecture: thread poll + coroutines
### Main lib: boost::asio
### Student: Aleksey Sirmais (@Aleksey_Sir)

# Commands
### Run from root dir:

* Start cpp server: ***docker build -t cppserveralex . && docker run -p 80:8080 -v /etc/httpd.conf:/etc/httpd.conf:ro -ti cppserveralex*** 
(mb Segmentation fault (core dumped) while ab test in docket, need run local no docker)


* Start check: ***./httptest.py***

* Start nginx: ***docker build -t nginxserveralex ./nginx && docker run -p 80:80 -ti nginxserveralex***

* Fast static sum check: ***du httptest  | sha1sum && cd nginx/static && du httptest  | sha1sum && cd ../..***