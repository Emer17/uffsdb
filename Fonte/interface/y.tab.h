/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INSERT = 258,
    INTO = 259,
    VALUES = 260,
    SELECT = 261,
    FROM = 262,
    CREATE = 263,
    TABLE = 264,
    INTEGER = 265,
    VARCHAR = 266,
    DOUBLE = 267,
    CHAR = 268,
    PRIMARY = 269,
    KEY = 270,
    REFERENCES = 271,
    DATABASE = 272,
    DROP = 273,
    OBJECT = 274,
    NUMBER = 275,
    VALUE = 276,
    QUIT = 277,
    LIST_TABLES = 278,
    LIST_TABLE = 279,
    ALPHANUM = 280,
    CONNECT = 281,
    HELP = 282,
    LIST_DBASES = 283,
    CLEAR = 284,
    CONTR = 285,
    RUN_SCRIPT = 286,
    END_OF_FILE = 287,
    EDITOR = 288,
    WHERE = 289,
    EQUAL = 290,
    LESS = 291,
    GREATER = 292,
    LESS_EQUAL = 293,
    GREATER_EQUAL = 294,
    AND = 295,
    OR = 296,
    NOT_EQUAL = 297
  };
#endif
/* Tokens.  */
#define INSERT 258
#define INTO 259
#define VALUES 260
#define SELECT 261
#define FROM 262
#define CREATE 263
#define TABLE 264
#define INTEGER 265
#define VARCHAR 266
#define DOUBLE 267
#define CHAR 268
#define PRIMARY 269
#define KEY 270
#define REFERENCES 271
#define DATABASE 272
#define DROP 273
#define OBJECT 274
#define NUMBER 275
#define VALUE 276
#define QUIT 277
#define LIST_TABLES 278
#define LIST_TABLE 279
#define ALPHANUM 280
#define CONNECT 281
#define HELP 282
#define LIST_DBASES 283
#define CLEAR 284
#define CONTR 285
#define RUN_SCRIPT 286
#define END_OF_FILE 287
#define EDITOR 288
#define WHERE 289
#define EQUAL 290
#define LESS 291
#define GREATER 292
#define LESS_EQUAL 293
#define GREATER_EQUAL 294
#define AND 295
#define OR 296
#define NOT_EQUAL 297

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 39 "yacc.y" /* yacc.c:1909  */

    int intval;
    double floatval;
    int subtok;
    char *strval;

#line 145 "y.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
