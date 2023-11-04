; Symbol mangling macros
; Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
; $Revision: 1.4 $

; This file defines macros for dealing with externally-visible
; symbols that must be mangled for some object file formats.
; For example, PECOFF requires a leading underscore, while
; ELF does not.

; EXPORT defines a symbol as global
; IMPORT references a symbol defined in another module

; Thanks to Christopher Giese for providing the NASM macros
; (thus saving me hours of frustration).

%ifndef SYMBOL_ASM
%define SYMBOL_ASM

%ifdef NEED_UNDERSCORE

%macro EXPORT 1
[GLOBAL _%1]
%define %1 _%1
%endmacro

%macro IMPORT 1
[EXTERN _%1]
%define %1 _%1
%endmacro

%else

%macro EXPORT 1
[GLOBAL %1]
%endmacro

%macro IMPORT 1
[EXTERN %1]
%endmacro

%endif

%endif
