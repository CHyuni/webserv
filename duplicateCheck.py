import sys
import json
import os

try:
	with open('data.json', 'r') as file:
		data = json.load(file)

except FileNotFoundError:
	print("HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\n")

# 새로운 데이터 추가
new_user = {
	"id": sys.argv[1],
	"password": sys.argv[2]
}

data.append(new_user)

with open('data.json', 'w') as file:
	json.dump(data, file, indent=4)
