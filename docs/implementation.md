# 토큰에서 의미 분석까지

`cminus.l`의 토큰 규칙과 `cminus.y`의 문법에서 scanner·parser를 생성합니다. `util.c`와 `globals.h`는 공통 트리를 정의하고, `symtab.c`가 식별자를 찾으며 `analyze.c`가 심볼과 타입을 검사합니다.

`main.c`에서 단계를 선택하고 Makefile이 필요한 object를 연결합니다. `NO_CODE=TRUE`이므로 실행 코드 생성은 꺼져 있습니다. 토큰·문법만 보는 parser와 이름·타입을 검사하는 의미 분석의 역할을 구분한 프런트엔드입니다.
