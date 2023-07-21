# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    high_traffic_test.py                               :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: iamongeo <iamongeo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/07/20 16:47:36 by iamongeo          #+#    #+#              #
#    Updated: 2023/07/20 17:37:44 by iamongeo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# importing the requests library
import requests
import time
 
# api-endpoint
URL = "http://localhost:80"
URL2 = "http://localhost:3738"

def webserv_load_test(req_per_sec: int, test_duration: int):
    total_nb_request = test_duration * req_per_sec
    wait_time = 1.0 / req_per_sec
    
    for i in range(total_nb_request):
        print("Sending one request")
        r = requests.get(url=URL)
        if r.ok:
            print(r.text)
        else:
            print("r2 not ok")
        r2 = requests.get(url=URL2)
        if r2.ok:
            print(r2.text)
        else:
            print("r2 not ok")
        time.sleep(wait_time)


if __name__ == '__main__':
    
    webserv_load_test(200, 10)