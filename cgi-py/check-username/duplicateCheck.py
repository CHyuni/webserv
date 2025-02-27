import sys
import json
import os
import cgi

request_method = os.environ.get("REQUEST_METHOD")
if not request_method or request_method != "GET":
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
	with safe_open(file_path, "r") as file:
		data = json.load(file)
	param = os.environ.get("QUERY_STRING", "")
	_, value = param.split("=", 1)
	user_exists = any(user['id'] == value for user in data.get("users", []))
	
	if user_exists:
		body = "duplicate"
	else:
		body = "available"

	print(f"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: {len(body)}\r\n\r\n{body}")

except Exception as e:
	pass
