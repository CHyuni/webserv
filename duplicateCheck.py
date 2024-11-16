import sys
import json
import os

try:
	with open('data.json', 'r') as file:
		data = json.load(file)
	user_id = sys.argv[1]
	user_exists = False

	for user in data:
		if user['id'] == user_id:
			user_exists = True

	if user_exists:
		body = "duplicate"
		print(f"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: {len(body)}\r\n\r\n{body}\r\n\r\n")
	else:
		body = "available"
		print(f"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: {len(body)}\r\n\r\n{body}\r\n\r\n")

except FileNotFoundError:
	body = "available"
	print(f"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: {len(body)}\r\n\r\n{body}\r\n\r\n")
