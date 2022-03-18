
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "yaccin.y"

	


/* Line 189 of yacc.c  */
#line 78 "yaccin.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */

/* Line 209 of yacc.c  */
#line 4 "yaccin.y"

  //#define YYLTYPE YYLTYPE
  #include "globals.h"
  
	int finalNode;
	int yyerror(const char *s);



/* Line 209 of yacc.c  */
#line 112 "yaccin.tab.c"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     RETURN = 258,
     CLASS = 259,
     ID = 260,
     TYPE = 261,
     END = 262,
     IF = 263,
     ELSE = 264,
     ELSEIF = 265,
     WHILE = 266,
     FOR = 267,
     INTEGER = 268,
     INTEGERX = 269,
     INTEGERB = 270,
     FLOATING = 271,
     CHARACTER = 272,
     STRING = 273,
     EQUAL = 274,
     NOTEQUAL = 275,
     INC = 276,
     DEC = 277,
     PREREF = 278,
     PREDEREF = 279,
     PREDEC = 280,
     PREINC = 281,
     PRENEG = 282,
     POSTDEC = 283,
     POSTINC = 284
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* "%code provides" blocks.  */

/* Line 261 of yacc.c  */
#line 11 "yaccin.y"

char *posToString(YYLTYPE pos);
char *get_source_text2(YYLTYPE pos);



/* Line 261 of yacc.c  */
#line 183 "yaccin.tab.c"

/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 189 "yaccin.tab.c"
/* Unqualified %code blocks.  */

/* Line 265 of yacc.c  */
#line 15 "yaccin.y"

	#include "globals.h"
  #include "AST.h"
  #include "semantic.h"
  #include "codegen.h"
	#include "vector2.h"
	//		B = B C | C
	//beceomes
	//		B = C C C C C
	void ast_unroll_lists_helper(int nodeID2){
		ast_node *N2 = ast_get_node(nodeID2);
		if(!N2->children.size){return;}
		ast_node *N3 = ast_get_child(N2,0);
		if(N2->token.type != N3->token.type){return;}
		ast_unroll_lists_helper(ast_get_child_id(N2, 0));
		m(N2->children,erase,0);
		int i;
		for(i = 0; i < N3->children.size; i++){
			m(N2->children,push_back,ast_get_child_id(N3,i));
		}
	}
	//turns left-recursive lists into non-recursive ones
	//so	A = B | empty
	//		B = B C | C
	//becomes	A = C C C C C C | empty
	
	void ast_unroll_lists(int nodeID){
		ast_node *N1 = ast_get_node(nodeID);
		printf("unroll [%s] ",N1->token.type);
		if(!N1->children.size){return;}
		ast_node *N2 = ast_get_child(N1, 0);
		if(!N2->children.size){return;}
		ast_unroll_lists_helper(ast_get_child_id(N1, 0));
		m(N1->children,erase,0);
		int i;
		for(i = 0; i < N2->children.size; i++){
			m(N1->children,push_front,ast_get_child_id(N2,i));
		}
		printf(" unroll done\n");
	}
	
	void fix_decl_assign(int nodeID){
		//return;
		struct ast_node *N1 = ast_get_node(nodeID);
		//int node_expr = *(int*)vector_get_reference(&N1->children,1);
		int node_expr = m(N1->children,pop_back);
		//vector_pop_back(&N1->children);
		int node_ID = node((struct ast_token){"expr_id",0,N1->token.value},0);
		int N3 = node((struct ast_token){"expr_=",0,0},2,node_ID,node_expr);
		//vector_set(&N1->children,1,&N3);
		m(N1->children,push_back,N3);
	}	
	
	/* //collects all the stars and appends them to the node
	ast_unroll_stars_helper(int nodeID){
		struct ast_node *N1 = ast_get_node(nodeID);
		if(N1->children.size){
			
		}
	}
	ast_unroll_stars(int nodeID){
		struct ast_node *N1 = ast_get_node(nodeID);
		printf("unroll [%s] ",N1->token.type);
		ast_unroll_stars_helper(ast_get_child_id(N1,0));
		printf(" unroll done\n");
	} */
	# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
    if(N) /*if (YYID (N))*/							\
	{								\
								\
		(Current).null = 1;	\
		(Current).first_line   = 0;\
		(Current).first_column = 0;\
		(Current).last_line    = 0;\
		(Current).last_column  = 0;\
		(Current).start        = 0;\
		(Current).end          = 0;\
		(Current).filename = 0;\
		int i = 0;	\
		int hasFirst = 0;	\
		for(i = 1; i <= N; i++){	\
			printf("RHS(%d).null=%d\n",i,YYRHSLOC(Rhs,i).null);	\
			if(!YYRHSLOC(Rhs,i).null){	\
				(Current).null = 0;	\
				if(!hasFirst){	\
					hasFirst = 1;	\
					(Current).first_line = YYRHSLOC(Rhs,i).first_line;	\
					(Current).first_column = YYRHSLOC(Rhs,i).first_column;	\
					(Current).start = YYRHSLOC(Rhs,i).start;	\
					(Current).filename = YYRHSLOC(Rhs,i).filename;\
				}	\
				(Current).last_line = YYRHSLOC(Rhs,i).last_line;	\
				(Current).last_column = YYRHSLOC(Rhs,i).last_column;	\
				(Current).end = YYRHSLOC(Rhs,i).end;	\
			}	\
		}	\
		printf("LHS.null=%d\n",(Current).null);\
	}								\
      else	/* this is what happens when we have an empty production */ \
	{								\
	  /*(Current).null = 0;	\
	  (Current).first_line   = 0;\
	  (Current).first_column = 0;\
	  (Current).last_line    = 0;\
	  (Current).last_column  = 0;\
	  (Current).start        = 0;\
	  (Current).end          = 0;*/\
	}								\
    while(0)/*while (YYID (0))*/
//	    YYRHSLOC (Rhs, 0).last_column;				\
//
/*

	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).start        = YYRHSLOC (Rhs, 1).start;			\
  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).start = (Current).end = (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).end;				\
	  (Current).filename = YYRHSLOC (Rhs, 0).filename;	\
	  */



/* Line 265 of yacc.c  */
#line 321 "yaccin.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   590

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  49
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  25
/* YYNRULES -- Number of rules.  */
#define YYNRULES  79
/* YYNRULES -- Number of states.  */
#define YYNSTATES  148

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   284

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    38,     2,     2,     2,    48,    25,     2,
      34,    35,    32,    28,    47,    29,    33,    31,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    46,
      26,    23,    27,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    36,     2,    37,    30,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    24,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    39,    40,
      41,    42,    43,    44,    45
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    12,    15,    17,
      19,    21,    24,    26,    27,    30,    32,    34,    36,    39,
      42,    46,    48,    53,    55,    57,    58,    61,    63,    66,
      70,    78,    81,    87,    92,    94,    95,    99,   101,   104,
     109,   115,   122,   129,   139,   141,   142,   146,   148,   150,
     152,   154,   156,   158,   160,   162,   167,   171,   176,   180,
     184,   188,   192,   196,   200,   204,   207,   211,   215,   219,
     223,   227,   231,   235,   238,   241,   244,   247,   250,   253
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      50,     0,    -1,    55,    -1,    57,    -1,    54,    -1,    53,
      -1,    -1,    53,    51,    -1,    51,    -1,    58,    -1,    63,
      -1,    64,    46,    -1,    56,    -1,    -1,    56,    54,    -1,
      54,    -1,    67,    -1,    69,    -1,    73,    46,    -1,     3,
      46,    -1,     3,    73,    46,    -1,    70,    -1,     4,     5,
      55,     7,    -1,    32,    -1,    61,    -1,    -1,    61,    59,
      -1,    59,    -1,     6,    60,    -1,     4,     5,    60,    -1,
      62,     5,    34,    65,    35,    52,     7,    -1,    62,     5,
      -1,    62,     5,    36,    73,    37,    -1,    62,     5,    23,
      73,    -1,    66,    -1,    -1,    66,    47,    64,    -1,    64,
      -1,    68,     7,    -1,    68,     9,    52,     7,    -1,     8,
      34,    73,    35,    52,    -1,    68,    10,    34,    73,    35,
      52,    -1,    11,    34,    73,    35,    52,     7,    -1,    12,
      34,    51,    73,    46,    73,    35,    52,     7,    -1,    72,
      -1,    -1,    72,    47,    73,    -1,    73,    -1,     5,    -1,
      13,    -1,    14,    -1,    15,    -1,    16,    -1,    17,    -1,
      18,    -1,    73,    36,    73,    37,    -1,    34,    73,    35,
      -1,    73,    34,    71,    35,    -1,    73,    33,    73,    -1,
      73,    30,    73,    -1,    73,    31,    73,    -1,    73,    32,
      73,    -1,    73,    48,    73,    -1,    73,    29,    73,    -1,
      73,    28,    73,    -1,    38,    73,    -1,    73,    25,    73,
      -1,    73,    24,    73,    -1,    73,    19,    73,    -1,    73,
      20,    73,    -1,    73,    27,    73,    -1,    73,    26,    73,
      -1,    73,    23,    73,    -1,    73,    21,    -1,    21,    73,
      -1,    73,    22,    -1,    22,    73,    -1,    29,    73,    -1,
      32,    73,    -1,    25,    73,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   156,   156,   159,   160,   163,   164,   167,   168,   171,
     172,   173,   176,   177,   180,   181,   184,   185,   186,   187,
     188,   189,   192,   194,   197,   198,   201,   202,   205,   206,
     209,   211,   212,   213,   216,   217,   220,   221,   224,   225,
     228,   229,   240,   243,   246,   247,   250,   251,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "RETURN", "CLASS", "ID", "TYPE", "END",
  "IF", "ELSE", "ELSEIF", "WHILE", "FOR", "INTEGER", "INTEGERX",
  "INTEGERB", "FLOATING", "CHARACTER", "STRING", "EQUAL", "NOTEQUAL",
  "INC", "DEC", "'='", "'|'", "'&'", "'<'", "'>'", "'+'", "'-'", "'^'",
  "'/'", "'*'", "'.'", "'('", "')'", "'['", "']'", "'!'", "PREREF",
  "PREDEREF", "PREDEC", "PREINC", "PRENEG", "POSTDEC", "POSTINC", "';'",
  "','", "'%'", "$accept", "program", "stmt", "stmt_list", "stmt_list_ne",
  "decl_stmt", "decl_stmt_list", "decl_stmt_list_ne", "imp_stmt",
  "class_def", "STAR", "ptr_stars", "ptr_stars_ne", "typename", "func_def",
  "var_decl", "var_decl_list", "var_decl_list_ne", "if_block", "if_then",
  "while_loop", "for_loop", "expr_list", "expr_list_ne", "expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,    61,   124,    38,    60,    62,    43,    45,
      94,    47,    42,    46,    40,    41,    91,    93,    33,   278,
     279,   280,   281,   282,   283,   284,    59,    44,    37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    49,    50,    51,    51,    52,    52,    53,    53,    54,
      54,    54,    55,    55,    56,    56,    57,    57,    57,    57,
      57,    57,    58,    59,    60,    60,    61,    61,    62,    62,
      63,    64,    64,    64,    65,    65,    66,    66,    67,    67,
      68,    68,    69,    70,    71,    71,    72,    72,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     0,     2,     1,     1,
       1,     2,     1,     0,     2,     1,     1,     1,     2,     2,
       3,     1,     4,     1,     1,     0,     2,     1,     2,     3,
       7,     2,     5,     4,     1,     0,     3,     1,     2,     4,
       5,     6,     6,     9,     1,     0,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     3,     4,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     2,     2,     2,     2,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      13,     0,    25,     0,    15,     2,    12,     9,     0,    10,
       0,    13,    23,    27,    28,    24,     1,    14,    31,    11,
       0,    29,    26,     0,    35,     0,    22,    48,    49,    50,
      51,    52,    53,    54,     0,     0,     0,     0,     0,     0,
       0,    33,     0,     0,    37,     0,    34,     0,    74,    76,
      79,    77,    78,     0,    65,     0,     0,    73,    75,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,     0,     0,    25,    31,     6,     0,    32,    56,    68,
      69,    72,    67,    66,    71,    70,    64,    63,    59,    60,
      61,    58,     0,    44,    47,     0,    62,     0,     0,     0,
       0,     8,     0,     5,     4,     3,    16,     0,    17,    21,
       0,    36,    57,     0,    55,    19,     0,     0,     0,     0,
      30,     7,    38,     6,     0,    18,    46,    20,     0,     0,
       0,     0,     0,     6,     6,     0,    39,     0,    40,     0,
       0,     6,    42,     0,    41,     6,     0,    43
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,   101,   102,   103,   104,     5,     6,   105,     7,
      13,    21,    15,     8,     9,    10,    45,    46,   106,   107,
     108,   109,    92,    93,   110
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -115
static const yytype_int16 yypact[] =
{
      24,    -4,   -10,    35,  -115,  -115,    24,  -115,    45,  -115,
       7,     2,  -115,  -115,  -115,   -10,  -115,  -115,    47,  -115,
      50,  -115,  -115,   133,    96,   133,  -115,  -115,  -115,  -115,
    -115,  -115,  -115,  -115,   133,   133,   133,   133,   133,   133,
     133,   453,    49,    53,  -115,    30,    19,   153,     4,     4,
       4,     4,     4,   183,     4,   133,   133,  -115,  -115,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   -10,   -13,   107,    96,  -115,  -115,   522,
     522,   478,   501,   501,   522,   522,    55,    55,   526,   526,
     526,   542,    34,    25,   453,   213,   453,    46,    39,    48,
      59,  -115,    72,   107,  -115,  -115,  -115,    89,  -115,  -115,
     243,  -115,  -115,   133,  -115,  -115,   273,   133,   133,   107,
    -115,  -115,  -115,   107,    63,  -115,   453,  -115,   303,   333,
     133,    94,   133,   107,   107,   363,  -115,   393,  -115,    97,
     133,   107,  -115,   423,  -115,   107,    98,  -115
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -115,  -115,   -98,  -114,  -115,    18,    95,  -115,  -115,  -115,
      93,   112,  -115,   -21,  -115,   -20,  -115,  -115,  -115,  -115,
    -115,  -115,  -115,  -115,   -23
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -26
static const yytype_int16 yytable[] =
{
      41,    11,    47,    43,    44,   121,     1,   -25,     2,   131,
      23,    48,    49,    50,    51,    52,    53,    54,     4,   138,
     139,   130,    12,    25,    17,    57,    58,   144,     1,     4,
       2,   146,    79,    80,    12,    16,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    94,    95,    96,
      18,    27,    72,    19,    73,    43,   111,    26,    74,    28,
      29,    30,    31,    32,    33,    75,    76,    34,    35,   112,
      23,    36,   113,   117,   116,    37,    57,    58,    38,   120,
      39,    24,   118,    25,    40,    66,    67,    68,    69,    70,
     126,    71,   115,   119,   128,   129,   122,   132,   123,   124,
      42,   136,     2,    72,   142,   147,    20,   135,    22,   137,
      97,     1,    27,     2,    14,    98,     0,   143,    99,   100,
      28,    29,    30,    31,    32,    33,     0,     0,    34,    35,
       0,     0,    36,     0,     0,     0,    37,     0,    27,    38,
       0,    39,     0,     0,     0,    40,    28,    29,    30,    31,
      32,    33,     0,     0,    34,    35,     0,     0,    36,     0,
       0,     0,    37,     0,     0,    38,     0,    39,     0,     0,
       0,    40,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,     0,    71,
      77,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    78,    71,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,     0,    71,
     114,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,     0,    71,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   125,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,     0,    71,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   127,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,   133,    71,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,   134,    71,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,     0,    71,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   140,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,   141,    71,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,   145,    71,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,     0,    71,
       0,     0,     0,     0,     0,     0,     0,    55,    56,    57,
      58,    72,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,     0,    71,     0,     0,     0,     0,     0,
      55,    56,    57,    58,     0,     0,    72,    62,    63,    64,
      65,    66,    67,    68,    69,    70,     0,    71,     0,     0,
       0,     0,     0,    57,    58,     0,     0,    57,    58,    72,
      64,    65,    66,    67,    68,    69,    70,     0,    71,    69,
      70,     0,    71,    57,    58,     0,     0,     0,     0,     0,
      72,     0,     0,     0,    72,     0,    70,     0,    71,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      72
};

static const yytype_int16 yycheck[] =
{
      23,     5,    25,    24,    24,   103,     4,     5,     6,   123,
      23,    34,    35,    36,    37,    38,    39,    40,     0,   133,
     134,   119,    32,    36,     6,    21,    22,   141,     4,    11,
       6,   145,    55,    56,    32,     0,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
       5,     5,    48,    46,     5,    76,    76,     7,     5,    13,
      14,    15,    16,    17,    18,    35,    47,    21,    22,    35,
      23,    25,    47,    34,    97,    29,    21,    22,    32,     7,
      34,    34,    34,    36,    38,    30,    31,    32,    33,    34,
     113,    36,    46,    34,   117,   118,     7,    34,     9,    10,
       4,     7,     6,    48,     7,     7,    11,   130,    15,   132,
       3,     4,     5,     6,     2,     8,    -1,   140,    11,    12,
      13,    14,    15,    16,    17,    18,    -1,    -1,    21,    22,
      -1,    -1,    25,    -1,    -1,    -1,    29,    -1,     5,    32,
      -1,    34,    -1,    -1,    -1,    38,    13,    14,    15,    16,
      17,    18,    -1,    -1,    21,    22,    -1,    -1,    25,    -1,
      -1,    -1,    29,    -1,    -1,    32,    -1,    34,    -1,    -1,
      -1,    38,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    48,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    48,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    48,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,    21,
      22,    48,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    -1,    36,    -1,    -1,    -1,    -1,    -1,
      19,    20,    21,    22,    -1,    -1,    48,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    -1,    36,    -1,    -1,
      -1,    -1,    -1,    21,    22,    -1,    -1,    21,    22,    48,
      28,    29,    30,    31,    32,    33,    34,    -1,    36,    33,
      34,    -1,    36,    21,    22,    -1,    -1,    -1,    -1,    -1,
      48,    -1,    -1,    -1,    48,    -1,    34,    -1,    36,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     6,    50,    54,    55,    56,    58,    62,    63,
      64,     5,    32,    59,    60,    61,     0,    54,     5,    46,
      55,    60,    59,    23,    34,    36,     7,     5,    13,    14,
      15,    16,    17,    18,    21,    22,    25,    29,    32,    34,
      38,    73,     4,    62,    64,    65,    66,    73,    73,    73,
      73,    73,    73,    73,    73,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    36,    48,     5,     5,    35,    47,    37,    35,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73,
      73,    73,    71,    72,    73,    73,    73,     3,     8,    11,
      12,    51,    52,    53,    54,    57,    67,    68,    69,    70,
      73,    64,    35,    47,    37,    46,    73,    34,    34,    34,
       7,    51,     7,     9,    10,    46,    73,    46,    73,    73,
      51,    52,    34,    35,    35,    73,     7,    73,    52,    52,
      46,    35,     7,    73,    52,    35,    52,     7
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 156 "yaccin.y"
    {finalNode = node((struct ast_token){"program",0,0,(yyloc)},1,(yyvsp[(1) - (1)])); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 159 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"stmt",0,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 160 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"stmt",1,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 163 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"stmt_list",0,0,(yyloc)},1,(yyvsp[(1) - (1)])); ast_unroll_lists((yyval));;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 164 "yaccin.y"
    {(yyloc).null = 1;(yyval) = (int)node((struct ast_token){"stmt_list",1,"<empty>",(yyloc)},0);;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 167 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"stmt_list_ne",0,0,(yyloc)},2,(yyvsp[(1) - (2)]),(yyvsp[(2) - (2)]));;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 168 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"stmt_list_ne",1,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 171 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"decl_stmt",0,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 172 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"decl_stmt",1,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 173 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"decl_stmt",2,0,(yyloc)},1,(yyvsp[(1) - (2)]));;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 176 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"decl_stmt_list",0,0,(yyloc)},1,(yyvsp[(1) - (1)])); ast_unroll_lists((yyval));;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 177 "yaccin.y"
    {(yyloc).null = 1;(yyval) = (int)node((struct ast_token){"decl_stmt_list",1,"<empty>",(yyloc)},0);;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 180 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"decl_stmt_list_ne",0,0,(yyloc)},2,(yyvsp[(1) - (2)]),(yyvsp[(2) - (2)]));;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 181 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"decl_stmt_list_ne",1,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 184 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"imp_stmt",0,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 185 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"imp_stmt",1,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 186 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"imp_stmt",2,0,(yyloc)},1,(yyvsp[(1) - (2)]));;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 187 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"imp_stmt",3,0,(yyloc)},0);;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 188 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"imp_stmt",4,0,(yyloc)},1,(yyvsp[(2) - (3)]));;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 189 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"imp_stmt",5,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 192 "yaccin.y"
    {printf("FUNC_DEF ID = [%s]\n",(char*)(yyvsp[(2) - (4)]));(yyval) = (int)node((struct ast_token){"class_def",0,(char*)(yyvsp[(2) - (4)]),(yyloc)},1,(yyvsp[(3) - (4)]));;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 194 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"stars",0,"*",(yyloc)},0);;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 197 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"ptr_stars",0,0,(yyloc)},1,(yyvsp[(1) - (1)])); ast_unroll_lists((yyval));;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 198 "yaccin.y"
    {(yyloc).null = 1;(yyval) = (int)node((struct ast_token){"ptr_stars",1,"<empty>",(yyloc)},0);;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 201 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"ptr_stars_ne",0,0,(yyloc)},2,(yyvsp[(1) - (2)]),(yyvsp[(2) - (2)]));;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 202 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"ptr_stars_ne",1,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 205 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"typename",0,(char*)(yyvsp[(1) - (2)]),(yyloc)},1,(yyvsp[(2) - (2)]));;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 206 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"typename",1,(char*)(yyvsp[(2) - (3)]),(yyloc)},1,(yyvsp[(3) - (3)]));;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 209 "yaccin.y"
    {printf("FUNC_DEF ID = [%s]\n",(char*)(yyvsp[(2) - (7)]));(yyval) = (int)node((struct ast_token){"func_def",0,(char*)(yyvsp[(2) - (7)]),(yyloc)},3,(yyvsp[(1) - (7)]),(yyvsp[(4) - (7)]),(yyvsp[(6) - (7)]));;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 211 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"var_decl",0,(char*)(yyvsp[(2) - (2)]),(yyloc)},1,(yyvsp[(1) - (2)]));;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 212 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"var_decl",1,(char*)(yyvsp[(2) - (5)]),(yyloc)},2,(yyvsp[(1) - (5)]),(yyvsp[(4) - (5)]));;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 213 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"var_decl_assign",0,(char*)(yyvsp[(2) - (4)]),(yyloc)},2,(yyvsp[(1) - (4)]),(yyvsp[(4) - (4)]));;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 216 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"var_decl_list",0,0,(yyloc)},1,(yyvsp[(1) - (1)])); ast_unroll_lists((yyval));;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 217 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"var_decl_list",1,"<empty>",(yyloc)},0);;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 220 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"var_decl_list_ne",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 221 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"var_decl_list_ne",1,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 224 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"if_block",0,0,(yyloc)},1,(yyvsp[(1) - (2)]));;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 225 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"if_block",1,0,(yyloc)},2,(yyvsp[(1) - (4)]),(yyvsp[(3) - (4)]));;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 228 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"if_then",0,0,(yyloc)},2,(yyvsp[(3) - (5)]),(yyvsp[(5) - (5)]));;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 229 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"if_then",2,0,(yyloc)},3,(yyvsp[(1) - (6)]),(yyvsp[(4) - (6)]),(yyvsp[(6) - (6)]));;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 240 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"while_loop",0,0,(yyloc)},2,(yyvsp[(3) - (6)]),(yyvsp[(5) - (6)]));;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 243 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"for_loop",0,0,(yyloc)},4,(yyvsp[(3) - (9)]),(yyvsp[(4) - (9)]),(yyvsp[(6) - (9)]),(yyvsp[(8) - (9)]));;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 246 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_list",0,0,(yyloc)},1,(yyvsp[(1) - (1)]));ast_unroll_lists((yyval));;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 247 "yaccin.y"
    {(yyloc).null = 1;(yyval) = (int)node((struct ast_token){"expr_list",1,"<empty>",(yyloc)},0);;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 250 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_list_ne",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 251 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_list_ne",1,0,(yyloc)},1,(yyvsp[(1) - (1)]));;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 254 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_id",0,(char*)(yyvsp[(1) - (1)]),(yyloc)},0);;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 255 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_const",0,(char*)(yyvsp[(1) - (1)]),(yyloc)},0);;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 256 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_const",1,(char*)(yyvsp[(1) - (1)]),(yyloc)},0);;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 257 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_const",2,(char*)(yyvsp[(1) - (1)]),(yyloc)},0);;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 258 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_const",3,(char*)(yyvsp[(1) - (1)]),(yyloc)},0);;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 259 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_const",4,(char*)(yyvsp[(1) - (1)]),(yyloc)},0);;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 260 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_const",5,(char*)(yyvsp[(1) - (1)]),(yyloc)},0);;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 261 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_index",0,0,(yyloc)},2,(yyvsp[(1) - (4)]),(yyvsp[(3) - (4)]));;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 262 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_subexpr",0,0,(yyloc)},1,(yyvsp[(2) - (3)]));;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 263 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_call",0,0,(yyloc)},2,(yyvsp[(1) - (4)]),(yyvsp[(3) - (4)]));;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 264 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_.",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 265 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_^",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 266 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_/",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 267 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_*",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 268 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_%",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 269 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_-",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 270 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_+",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 271 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_not",0,0,(yyloc)},1,(yyvsp[(2) - (2)]));;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 272 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_and",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 273 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_or",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 274 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_==",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 275 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_!=",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 276 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_>",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 277 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_<",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 278 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_=",0,0,(yyloc)},2,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]));;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 279 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_++",0,0,(yyloc)},1,(yyvsp[(1) - (2)]));;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 280 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_++",1,0,(yyloc)},1,(yyvsp[(2) - (2)]));;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 281 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_--",0,0,(yyloc)},1,(yyvsp[(1) - (2)]));;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 282 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_--",1,0,(yyloc)},1,(yyvsp[(2) - (2)]));;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 283 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_neg",0,0,(yyloc)},1,(yyvsp[(2) - (2)]));;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 284 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_deref",0,0,(yyloc)},1,(yyvsp[(2) - (2)]));;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 285 "yaccin.y"
    {(yyval) = (int)node((struct ast_token){"expr_ref",0,0,(yyloc)},1,(yyvsp[(2) - (2)]));;}
    break;



/* Line 1455 of yacc.c  */
#line 2314 "yaccin.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 288 "yaccin.y"

extern FILE *yyin;
FILE *yyin2;

int main(int argc, char **argv); //moved to main.c

char *get_source_text2(YYLTYPE pos){ //this is still stupid-broken
	FILE *fp = fopen(pos.filename,"r");
	if(!fp){error("can't open %s ",pos.filename);}
	//fprintf(stderr,"gst2 %s, line %d\n",pos.filename,pos.first_column);
	int C;
	int line = 0;
	int col = 0;
	int first_line = pos.first_line;
	int last_line = pos.last_line;
	int first_column = pos.first_column;
	int last_column = pos.last_column;
	const char *filename = pos.filename;
	//1) seek to start
	C = fgetc(fp);
	while(line+1 < pos.first_line){while((C != '\n')&&(C != EOF)){C = fgetc(fp);} line++; C = fgetc(fp);}
	while(col != pos.first_column){if(C == '\n'){error("gst2: overshot1 ");} C = fgetc(fp);col++;}
	int fpos = ftell(fp);
	
	//2) count chars
	//int len = 0;
	//C = fgetc(fp);
	//while((line != pos.last_line)&&(C!=EOF)){while((C != '\n')&&(C != EOF)){C = fgetc(fp); len++;} line++; len++;}
	//len+= pos.last_column;
	
	
	//2) write the string down
	//char *str = malloc(sizeof(char)*(len+1));
	char str[2000];
	char *strP = str;
	//int err = fseek(fp,fpos,SEEK_SET);
	//if(err){perror("fseek: "); error("gst2: fseek: err = %d",err);}
	line = pos.first_line;
	//C = fgetc(fp);
	while((line != pos.last_line)&&(C!=EOF)){while((C != '\n')&&(C != EOF)){*strP++ = C; C = fgetc(fp);} *strP++ = C; line++; C = fgetc(fp);}
	while((col != pos.last_column)&&(C != EOF)){if(C == '\n'){error("gst2: overshot2 ");} *strP++ = C; C = fgetc(fp); col++;}
	
	//3) finalize string and return
	fclose(fp);
	*strP = 0;
	return stralloc(str);
}

char *get_source_text(int start, int end,const char *file){
	if(!file){error("get_source_text: file is null ");}
	if((start < 0)||(end < 0)||(end < start)){error("get_source_text: bad args (%d,%d) ",start,end);}
	//fprintf(stderr,"get_source_text(%d,%d,%s)\n",start,end,file);
	//fseek(yyin2, start, SEEK_SET);
	FILE *fp = fopen(file,"r");
	if(!fp){error("can't open %s ",file);}
	int i;
	//fseek(fp, 0, SEEK_SET);
	for(i = 0; i < start; i++){fgetc(fp);}
	//fseek(fp,start,SEEK_SET);
	char *buff = malloc(sizeof(char)*(end-start+2));
	// int i;
	// for(i = 0; i < end-start; i++){
		// buff[i] = fgetc(yyin2);
	// }
	fread(buff,1,end-start,fp);
	buff[end-start] = 0;
	//fprintf(stderr,"got [%s]\n",buff);
	//fread(buff,end-start,1,yyin2);
	fclose(fp);
	return buff;
}

char *get_source_line(int start, const char *file){
	FILE *fp = fopen(file,"r");
	if(!fp){error("can't open %s ",file);}
	fprintf(stderr,"get_source_line(%d,%s)\n",start,file);
	fseek(fp, start, SEEK_SET);
	int C = fgetc(fp);
	int len = 0;
	while((C != '\n') && (C != EOF)){C = fgetc(fp); len++;}
	fclose(fp);
	return get_source_text(start,start+len,file);
}

int line_to_pos(int line, const char *file){
	FILE *fp = fopen(file,"r");
	if(!fp){error("can't open %s ",file);}
	//fseek(yyin2, 0, SEEK_SET);
	int C = fgetc(fp);
	int start = 0;
	line--;
	while(line--){
		//printf("skipping line %d:%d:[%s]\n",14-line,start,get_source_line(start));
		//printf("C = '%c'\n",C);
		while((C != '\n') && (C != EOF)){
			C = fgetc(fp);
			start++;
		}
		C = fgetc(fp); 
		start++;
	}
	int pos = ftell(fp);
	fclose(fp);
	return start;
}

char *get_source_line2(int line,const char *file){
	FILE *fp = fopen(file,"r");
	if(!fp){error("can't open %s ",file);}
	//fseek(yyin2, 0, SEEK_SET);
	int C = fgetc(fp);
	int start = 0;
	line--;
	while(line--){
		//printf("skipping line %d:%d:[%s]\n",14-line,start,get_source_line(start));
		//printf("C = '%c'\n",C);
		while((C != '\n') && (C != EOF)){
			C = fgetc(fp);
			start++;
		}
		C = fgetc(fp); 
		start++;
	}
	int pos = ftell(fp);
	C = fgetc(fp);
	int len = 0;
	while((C != '\n') && (C != EOF)){C = fgetc(fp); len++;}
	fclose(fp);
	return get_source_text(start,start+len,file);
	//int pos = ftell(yyin2);
	//return get_source_line(pos);
}

int countTabs(const char *str){
	int t = 0;
	while(*str++ == '\t'){t++;}
	return t;
}

void point_out_error(int line, int pos, const char *file, const char *s){
	//printf("pos: %d:%d:%d:%d\n",y1,x1,y2,x2);
    const char *str = get_source_line2(line,file);
	//fprintf(stderr, "%s\n", get_source_line(lexlinestart));
	fprintf(stderr, "\n\nfile: %s, line %d:",file, line);
	fprintf(stderr, "\n%s\n", str);
	int i;
	//for(i = 0; i < lexnumtabs; i++){fprintf(stderr, "\t");}
	//for(i = 0; i < x1 - lexnumtabs; i++){fprintf(stderr, " ");}
	for(i = 0; i < countTabs(str); i++){fprintf(stderr, "\t");}
	for(i = 0; i < pos - countTabs(str); i++){fprintf(stderr, " ");}
	fprintf(stderr, "^\n");
	if(s && strcmp(s,"")!=0){
		fprintf(stderr,"%s\n",s);
	}
	
}

int yyerror(const char *s)
{	
	printf("\nyyerror()!\n");
	int x1 = yylloc.first_column;
	int y1 = yylloc.first_line;
	int x2 = yylloc.last_column;
	int y2 = yylloc.last_line;
	const char *f = yylloc.filename;
	point_out_error(y1,x1,f,s);
	
	error("syntax error. yytext: [%s]\n",(char*)yylval);
	//exit(1);
}

char *posToString(YYLTYPE pos){
	int start = pos.first_column;
	int end = pos.last_column;
	int len = end-start;
	char *buff = malloc(sizeof(char)*len);
	fseek(yyin2, start, SEEK_SET);
	fgets(buff, len, yyin2);
	return buff;
}
