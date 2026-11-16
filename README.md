# C-minus Compiler Frontend

Kenneth C. Louden의 TINY compiler skeleton을 바탕으로 C-minus의 scanner, parser와 의미 분석을 구현한 수업 프로젝트입니다.

```text
C-minus → 토큰 → AST → 심볼 테이블 → 타입 검사
```

## 소스

| 파일 | 내용 |
| --- | --- |
| [cminus.l](cminus.l) | Flex 토큰 규칙 |
| [cminus.y](cminus.y) | 구문 규칙과 AST 구성 |
| [symtab.c](symtab.c) | 식별자 등록·탐색 |
| [analyze.c](analyze.c) | 심볼·타입 검사 |
| [globals.h](globals.h), [util.c](util.c) | 공통 자료구조 |

## 빌드

GCC, Flex, Yacc/Bison과 Make를 준비합니다.

```bash
make
./cminus_parser program.cm
```

현재 실행 경로는 parsing과 semantic analysis까지입니다. `NO_CODE=TRUE`로 기계어 생성은 실행하지 않습니다.

[구현 노트](docs/implementation.md) · [원저자 표기](ATTRIBUTION.md)

[전체 문서 안내](docs/README.md)
