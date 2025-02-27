#!/bin/bash

# 현재 작업 디렉토리(PWD) 가져오기
CURRENT_PWD=$(pwd)

# 대상 파일 리스트
FILES=("myser.conf" "default.conf")

# 각 파일에 대해 ${PWD}를 대체
for FILE in "${FILES[@]}"; do
    if [ -f "$FILE" ]; then
        OUTPUT_FILE="${FILE%.conf}_modified.conf"  # 예: myser_modified.conf
        sed "s|\${PWD}|${CURRENT_PWD}|g" "$FILE" > "$OUTPUT_FILE"
        echo "수정된 파일이 ${OUTPUT_FILE}로 저장되었습니다."
    else
        echo "파일 ${FILE}이 존재하지 않습니다."
    fi
done