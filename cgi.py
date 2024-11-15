#!/usr/bin/python3
# import datetime

# current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

# html = f"""<html>
# <body>
#     <h1>Python CGI Time</h1>
#     <p>Current Time: {current_time}</p>
# </body>
# </html>"""

# print(html)

import sys
import json
import os

print("Current working directory:", os.getcwd())

try:
	with open('data.json', 'r') as file:
		data = json.load(file)

except FileNotFoundError:
	data = []

# 새로운 데이터 추가
new_user = {
	"id": sys.argv[1],
	"password": sys.argv[2]
}

data.append(new_user)

with open('data.json', 'w') as file:
	json.dump(data, file, indent=4)

