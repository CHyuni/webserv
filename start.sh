#!/bin/bash

# 백업 파일 이름
BACKUP_FILE="myser.conf.bak"
DEFALUT_BACKUP_FILE="default.conf.bak"

# 최종 설정 파일 이름
CONFIG_FILE="myser.conf"
DEFALUT_FILE="default.conf"

# 현재 작업 디렉토리 얻기
CURRENT_PWD=$(pwd)

# 백업 파일을 복사하여 새 설정 파일 생성
cp "$BACKUP_FILE" "$CONFIG_FILE"
cp "$DEFALUT_BACKUP_FILE" "$DEFALUT_FILE"

# sed 명령어를 사용하여 ${PWD}를 현재 디렉토리로 치환
sed -i "s#\${PWD}#${CURRENT_PWD}#g" "$CONFIG_FILE"
sed -i "s#\${PWD}#${CURRENT_PWD}#g" "$DEFALUT_FILE"

echo "설정 파일 ${PWD}가 현재 디렉토리로 변경되었습니다."