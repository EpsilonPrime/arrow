// $antlr-format alignTrailingComments on, columnLimit 130, minEmptyLines 1, maxEmptyLinesToKeep 1, reflowComments off
// $antlr-format useTab off, allowShortRulesOnASingleLine off, allowShortBlocksOnASingleLine on, alignSemicolons ownLine

parser grammar SubstraitParser;

options {
   tokenVocab = SubstraitLexer;
}

// MEGAHACK -- Flip so top level grammar items are on top.

plan: (function*|extensionspace) schema* relation* (pipeline SEMICOLON)* EOF
;

extensionspace: EXTENSION_SPACE URI LEFTBRACE function* RIGHTBRACE
;

function: FUNCTION name uri? (AS id)? SEMICOLON
;

name: id COLON signature
;

signature: id
;

uri: URI
;

schema: SCHEMA id LEFTBRACE schema_item* RIGHTBRACE
;

schema_item: id NULL? id SEMICOLON
;

relation: RELATION relation_ref LEFTBRACE relation_detail* RIGHTBRACE
;

relation_detail: COMMON SEMICOLON
    | BASE_SCHEMA SEMICOLON
    | FILTER SEMICOLON
    | PROJECTION SEMICOLON
    | ADVANCED_EXTENSION SEMICOLON
    | read_type
;

read_type: LOCAL_FILES LEFTBRACE RIGHTBRACE
;

relation_ref: id (LEFTPAREN SCHEMA id RIGHTPAREN)?
;

pipeline: pipeline ARROW relation_ref
        | relation_ref
;

id: IDENTIFIER
| 'FILTER'
;

