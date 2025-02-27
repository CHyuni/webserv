import sys
import json
import os
from datetime import datetime

request_method = os.environ.get("REQUEST_METHOD")
if not request_method or request_method != "DELETE":
	body = "Method Not Allowed"
	print(f"HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\nContent-Length: {len(body)}\r\n\r\n{body}")
	sys.exit(0)
script_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_dir)
file_path = '../../User/data.json'

def safe_open(file_path, mode="w"):
	dir_path = os.path.dirname(file_path)
	users = []
	sessions = []
	if not os.path.exists(dir_path):
		os.makedirs(dir_path)
	if not os.path.exists(file_path):
		with open(file_path, 'w') as temp_file:
			json.dump({"users": users, "sessions": sessions}, temp_file)
	return open(file_path, mode)

try:
	with safe_open(file_path, 'r+') as file:
		data = json.load(file)
		user_id = os.environ.get("HTTP_COOKIE", "")
		match = -1
		cookie = "\r\n"

		for idx in range(len(data["sessions"])):
			id = data["sessions"][idx].get("session_id", "")
			if id == user_id:
				match = idx
				break
		if match != -1:
			del(data["sessions"][match])
			body = "success"
			file.seek(0)
			file.truncate()
			json.dump(data, file, indent=4)
			file.flush()
			cookie = f'Cache-Control: no-store, no-cache, must-revalidate\n'
			cookie += f"Set-Cookie: session_id={user_id}; Expires={datetime(1970, 1, 1)}; Path=/; HttpOnly; Secure; SameSite=Strict\r\n\r\n"
		else:
			body = "fail"
	print(f"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: {len(body)}\r\n{cookie}{body}\r\n")

except Exception as e:
	pass
