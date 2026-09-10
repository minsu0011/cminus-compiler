# C-minus 컴파일러 프런트엔드

컴파일러 수업에서 C-minus의 토큰·구문 분석, 심볼 테이블과 의미 분석을 구현했습니다. Kenneth C. Louden의 TINY compiler skeleton을 바탕으로 분석 단계들을 연결한 프로젝트입니다.

## 사용 기술

C, Flex, Yacc/Bison, Make를 사용합니다. 문법에서 scanner/parser를 생성하고 분석 코드를 함께 컴파일합니다.

## 분석 단계

```text
C-minus 코드 → scanner → parser → AST → symbol table → type check
```

- [cminus.l](cminus.l): 토큰 규칙
- [cminus.y](cminus.y): 구문 규칙과 트리 구성
- [globals.h](globals.h), [util.c](util.c): AST와 공통 정의
- [symtab.c](symtab.c): 식별자 등록·탐색
- [analyze.c](analyze.c): 심볼·타입 검사
- [main.c](main.c): 분석 단계 선택

## 구현 과정

문법을 판별하는 parser에서 이름과 타입의 의미를 검사하는 단계로 확장했습니다. 문법적으로 올바른 식이라도 식별자가 정의되지 않았거나 타입이 다를 수 있어 AST 순회와 심볼 테이블이 필요합니다.

각 단계가 같은 트리 구조를 사용하도록 공통 정의를 두고 오류를 listing으로 확인합니다. 현재 실행 경로는 parsing과 semantic analysis까지입니다. `NO_CODE=TRUE`이므로 기계어 생성까지 완성한 컴파일러는 아닙니다.

## 빌드와 실행

```bash
make
./cminus_parser program.cm
```

GCC, Flex, Yacc 호환 parser generator를 준비합니다. Bison은 Makefile의 Yacc 호출 방식과 맞춥니다. 입력에는 C-minus 문법에 맞는 코드를 작성합니다.

학습의 중심은 토큰·AST·심볼·타입 검사의 연결입니다. 최적화와 실행 코드 생성은 후속 범위로 남아 있습니다.

[구현 노트](docs/implementation.md) · [원저자 표기](ATTRIBUTION.md)
