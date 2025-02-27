import sys
import json
import os
from datetime import datetime, timedelta, timezone

request_method = os.environ.get("REQUEST_METHOD")
if not request_method or request_method != "GET":
	body = "Method Not Allowed"
	print(f"HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\nContent-Length: {len(body)}\r\n\r\n{body}")
	sys.exit(0)
script_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_dir)
file_path = '../../User/data.json'

def is_expired(time):
	now = datetime.now(timezone.utc)
	expire_time = datetime.strptime(time, "%a, %d %b %Y %H:%M:%S %z")
	return True if now >= expire_time else False

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

def get_img_in_directory(directory_path, path):
	if not os.path.exists(directory_path):
		os.makedirs(directory_path)
	# 디렉토리의 파일 목록 가져오기
	files = os.listdir(directory_path)
	if not files:
		return "./image/default.png"
	return path + files[0]

try:
	with safe_open(file_path, 'r+') as file:
		data = json.load(file)
		session_id = os.environ.get("HTTP_COOKIE", "")
		idx, match = -1, False
		login_id = ""

		for i, item in enumerate(data.get("sessions", [])):
			if item['session_id'] == session_id:
				if is_expired(item['expire_time']):
					del(data["sessions"][i])
					file.seek(0)
					json.dump(data, file, indent=4)
				else:
					login_id = item['id']
					match = True
				break
		if match:
			body = {
				"status": 'login',
				"id": login_id,
				"path": get_img_in_directory(f"../../upload/{login_id}", f"upload/{login_id}/"),
				}
		else:
			body = {
				"status": "fail",
				}
	body = json.dumps(body)
	print(f"HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: {len(body)}\r\n\r\n{body}")

except Exception as e:
	pass