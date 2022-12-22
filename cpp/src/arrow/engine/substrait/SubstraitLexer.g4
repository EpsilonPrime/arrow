// $antlr-format alignTrailingComments on, columnLimit 150, maxEmptyLinesToKeep 1, reflowComments off, useTab off
// $antlr-format allowShortRulesOnASingleLine on, alignSemicolons ownLine

lexer grammar SubstraitLexer;

options {
    caseInsensitive = true;
}

EXTENSION_SPACE : 'EXTENSION_SPACE';
FUNCTION : 'FUNCTION';
AS: 'AS';
SCHEMA: 'SCHEMA';
RELATION: 'RELATION';
NULL: 'NULL';

COMMON: 'COMMON';
BASE_SCHEMA: 'BASE_SCHEMA';
FILTER: 'FILTER';
PROJECTION: 'PROJECTION';
ADVANCED_EXTENSION: 'ADVANCED_EXTENSION';

VIRTUAL_TABLE: 'VIRTUAL_TABLE';
LOCAL_FILES: 'LOCAL_FILES';
NAMED_TABLE: 'NAMED_TABLE';
EXTENSION_TABLE: 'EXTENSION_TABLE';

ARROW: '->';
COLON: ':';
SEMICOLON: ';';
LEFTBRACE: '{';
RIGHTBRACE: '}';
LEFTPAREN: '(';
RIGHTPAREN: ')';

IDENTIFIER:
    [A-Z][A-Z_0-9]*
;

URI:
    'http://sample'
    | 'boolean.yaml'
;

SINGLE_LINE_COMMENT: '//' ~[\r\n]* (('\r'? '\n') | EOF) -> channel(HIDDEN);

SPACES: [ \u000B\t\r\n] -> channel(HIDDEN);
