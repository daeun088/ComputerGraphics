# Visual Studio 환경
### 설정

1. freealut 설치
2. VC++ 디렉터리 - 포함 디렉터리 C:\freeglut\include\GL 추가
3. VC++ 디렉터리 - 라이브러리 디렉터리(64비트): C:\freeglut\lib\x64 추가
4. 링커 - 입력 - 추가 종속성 freeglut.lib 등록
5. C:\Windows\SysWOW64 폴더 freeglut.dll, C:\freeglut\bin\x64 복사


### 주의사항
- #include <gl/glut.h> 의 경우 속성 설정에 따라 #inclue<glut.h>로 사용
- C2065, C2660, C2447 등의 에러 -> 주석 삭제로 해결
- 텍스처 매핑 진행 시 이미지 파일 설정

  프로젝트 우클릭 → 속성 → 디버깅 → 작업 디렉터리(Working Directory): $(OutDir)

