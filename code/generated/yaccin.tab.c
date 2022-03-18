/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "input/yaccin.y" /* yacc.c:339  */

	

#line 70 "generated/yaccin.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "yaccin.tab.h".  */
#ifndef YY_YY_GENERATED_YACCIN_TAB_H_INCLUDED
# define YY_YY_GENERATED_YACCIN_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 4 "input/yaccin.y" /* yacc.c:355  */

  //#define YYLTYPE YYLTYPE
  #include "globals.h"
  #include "types/ast_node.h"
  
	//ast_node.h not fully included yet due to shenanigans
	typedef struct ast_node ast_node;
	
	ast_node *finalNode;
	int yyerror(const char *s);

#line 112 "generated/yaccin.tab.c" /* yacc.c:355  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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
    PREDEREF = 278,
    PREREF = 279,
    PREINC = 280,
    PREDEC = 281,
    PRENEG = 282
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);
/* "%code provides" blocks.  */
#line 15 "input/yaccin.y" /* yacc.c:355  */

char *posToString(YYLTYPE pos);
char *get_source_text2(YYLTYPE pos);

#line 178 "generated/yaccin.tab.c" /* yacc.c:355  */

#endif /* !YY_YY_GENERATED_YACCIN_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 184 "generated/yaccin.tab.c" /* yacc.c:358  */
/* Unqualified %code blocks.  */
#line 19 "input/yaccin.y" /* yacc.c:359  */

	#include "globals.h"
  #include "AST.h"
  #include "semantic.h"
  #include "codegen.h"
	#include "vector2.h"
	#include "types/ast_node.h"
	//		B = B C | C
	//beceomes
	//		B = C C C C C
	void ast_unroll_lists_helper(ast_node *N2){
		//ast_node *N2 = ast_get_node(nodeID2);
		if(!N2->children.size){return;}
		ast_node *N3 = ast_get_child(N2,0);
		if(N2->token.type != N3->token.type){return;}
		ast_unroll_lists_helper(ast_get_child(N2, 0));
		m(N2->children,erase,0);
		int i;
		for(i = 0; i < N3->children.size; i++){
			//m(N2->children,push_back,ast_get_child_id(N3,i));
			m(N2->children,push_back,ast_get_child(N3,i));
		}
	}
	//turns left-recursive lists into non-recursive ones
	//so	A = B | empty
	//		B = B C | C
	//becomes	A = C C C C C C | empty
	
	void ast_unroll_lists(ast_node *N1){
		//ast_node *N1 = ast_get_node(nodeID);
		printf("unroll [%s] ",N1->token.type);
		if(!N1->children.size){return;}
		ast_node *N2 = ast_get_child(N1, 0);
		if(!N2->children.size){return;}
		//ast_unroll_lists_helper(ast_get_child_id(N1, 0));
		ast_unroll_lists_helper(ast_get_child(N1,0));
		m(N1->children,erase,0);
		int i;
		for(i = 0; i < N2->children.size; i++){
			//m(N1->children,push_front,ast_get_child_id(N2,i));
			m(N1->children,push_front,ast_get_child(N2,i));
		}
		printf(" unroll done\n");
	}
	
	void fix_decl_assign(ast_node *N1){
		//return;
		//struct ast_node *N1 = ast_get_node(nodeID);
		//int node_expr = *(int*)vector_get_reference(&N1->children,1);
		ast_node *node_expr = m(N1->children,pop_back);
		//vector_pop_back(&N1->children);
		//int node_ID = node((struct ast_token){"expr_id",0,N1->token.value,nullPos()},0);
		ast_node *node_ID = ast_node_new(ast_token_here("expr_id",0,N1->token.value,nullPos()),v2pan_here());
		//int N3 = node((struct ast_token){"expr_=",0,0,nullPos()},2,node_ID,node_expr);
		ast_node *N3 = ast_node_new(ast_token_here("expr_=",0,0,nullPos()),
									vector2_ptr_ast_node_here_from_list(2, node_ID, node_expr));
		//vector_set(&N1->children,1,&N3);
		m(N1->children,push_back,N3);
	}	
	
	int checkValidLoc(YYLTYPE loc, const char *name, int num){
		/*
		int null;
		int first_line;
		int first_column;
		int last_line;
		int last_column;
		int start;
		int end;
		char *filename;
		*/
		int valid = 1;
		if(loc.null){printf("checkValidLoc: loc [%s %d] is null\n",name,num); valid = 0;}
		else{
			if(loc.first_line > loc.last_line){printf("checkValidLoc: loc [%s %d] has first_line %d > last_line %d\n",name,num,loc.first_line,loc.last_line); valid = 0;}
			if(loc.first_line == loc.last_line){
				if(loc.first_column > loc.last_column){
					printf("checkValidLoc: loc [%s %d] has first_col %d > last_col %d on same line\n",name,num,loc.first_column,loc.last_column);
					valid = 0;
				}
			}
			if(loc.start > loc.end){printf("checkValidLoc: loc [%s %d] has start %d > end %d\n",name,num,loc.start,loc.end);valid = 0;}
			if(!loc.filename){printf("checkValidLoc: loc [%s %d] has no filename\n",name,num); valid = 0;}
		}
		return valid;
	}
	
	void printParsed(ast_node *node){
		printf("\n\n|parsed node [%s]-----\n", node->token.type);
		int valid = checkValidLoc(node->token.pos,"node",0);
		printf("|node loc check: %d\n",valid);
		printf("|node has %d children\n", node->children.size);
		for(int I = 0; I < node->children.size; I++){
			ast_node *N = ast_get_child(node,I);
			printf("|-child %d/%d: [%s]\n", I, node->children.size,N->token.type);
			int valid2 = checkValidLoc(N->token.pos,"child",I);
			printf("|-child loc valid: %d\n",valid2);
		}
		if(!(node->token.pos.null) && (node->token.pos.filename)){
			printf("|node sourcecode: [%s]\n",get_source_text2(node->token.pos));
		}else{
			printf("|no source\n");
		}
		printf(    "|end node    [%s]-----\n\n",node->token.type);
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
    do{		\
	 printf("YYLOC_DEFAULT(%d)\n",N);		\
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
			/*checkValidLoc(YYRHSLOC(Rhs,i),"Rhs",i);*/ \
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
		/*checkValidLoc((Current), "LHS",0);*/ \
		printf("LHS.null=%d\n",(Current).null);\
	}								\
      else	/* this is what happens when we have an empty production */ \
	{								\
	  (Current).null = 1;	\
	  (Current).first_line   = 0;\
	  (Current).first_column = 0;\
	  (Current).last_line    = 0;\
	  (Current).last_column  = 0;\
	  (Current).start        = 0;\
	  (Current).end          = 0;\
	}}								\
    while(0)/*while (YYID (0))*/
//	    YYRHSLOC (Rhs, 0).last_column;	
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

#line 366 "generated/yaccin.tab.c" /* yacc.c:359  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   543

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  25
/* YYNRULES -- Number of rules.  */
#define YYNRULES  79
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  148

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   282

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    38,     2,     2,     2,    46,    25,     2,
      35,    43,    32,    28,    45,    29,    33,    31,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    42,
      26,    23,    27,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    34,     2,    44,    30,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    36,    37,
      39,    40,    41
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   216,   216,   229,   239,   250,   261,   274,   283,   294,
     303,   312,   323,   334,   347,   356,   367,   376,   385,   394,
     403,   412,   423,   435,   446,   457,   467,   476,   487,   497,
     509,   521,   531,   541,   553,   564,   574,   583,   594,   603,
     614,   623,   642,   653,   664,   675,   688,   697,   708,   709,
     710,   711,   712,   713,   714,   715,   716,   717,   718,   719,
     720,   721,   722,   723,   724,   725,   726,   727,   728,   729,
     730,   731,   732,   733,   734,   735,   736,   737,   738,   739
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "RETURN", "CLASS", "ID", "TYPE", "END",
  "IF", "ELSE", "ELSEIF", "WHILE", "FOR", "INTEGER", "INTEGERX",
  "INTEGERB", "FLOATING", "CHARACTER", "STRING", "EQUAL", "NOTEQUAL",
  "INC", "DEC", "'='", "'|'", "'&'", "'<'", "'>'", "'+'", "'-'", "'^'",
  "'/'", "'*'", "'.'", "'['", "'('", "PREDEREF", "PREREF", "'!'", "PREINC",
  "PREDEC", "PRENEG", "';'", "')'", "']'", "','", "'%'", "$accept",
  "program", "stmt", "stmt_list", "stmt_list_ne", "decl_stmt",
  "decl_stmt_list", "decl_stmt_list_ne", "imp_stmt", "class_def", "STAR",
  "ptr_stars", "ptr_stars_ne", "typename", "func_def", "var_decl",
  "var_decl_list", "var_decl_list_ne", "if_block", "if_then", "while_loop",
  "for_loop", "expr_list", "expr_list_ne", "expr", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,    61,   124,    38,    60,    62,    43,    45,
      94,    47,    42,    46,    91,    40,   278,   279,    33,   280,
     281,   282,    59,    41,    93,    44,    37
};
# endif

#define YYPACT_NINF -115

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-115)))

#define YYTABLE_NINF -26

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      76,     0,    -9,    28,  -115,  -115,    76,  -115,    49,  -115,
      15,     2,  -115,  -115,  -115,    -9,  -115,  -115,   -13,  -115,
      22,  -115,  -115,   133,   133,    79,  -115,  -115,  -115,  -115,
    -115,  -115,  -115,  -115,   133,   133,   133,   133,   133,   133,
     133,   433,   153,    71,    74,  -115,    43,    42,     4,     4,
       4,     4,     4,   181,     4,   133,   133,  -115,  -115,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,  -115,    -9,    35,   107,    79,  -115,    68,
      68,   456,   477,   477,    68,    68,   494,   494,   497,   497,
     497,    31,   209,    50,    47,   433,   433,    46,    69,    70,
      73,  -115,    99,   107,  -115,  -115,  -115,    63,  -115,  -115,
     237,  -115,  -115,  -115,   133,  -115,   265,   133,   133,   107,
    -115,  -115,  -115,   107,    81,  -115,   433,  -115,   293,   321,
     133,   119,   133,   107,   107,   349,  -115,   377,  -115,   120,
     133,   107,  -115,   405,  -115,   107,   123,  -115
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      13,     0,    25,     0,    15,     2,    12,     9,     0,    10,
       0,    13,    23,    27,    28,    24,     1,    14,    31,    11,
       0,    29,    26,     0,     0,    35,    22,    48,    49,    50,
      51,    52,    53,    54,     0,     0,     0,     0,     0,     0,
       0,    33,     0,     0,     0,    37,     0,    34,    74,    76,
      79,    77,    78,     0,    65,     0,     0,    73,    75,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    45,     0,    32,    25,    31,     6,     0,    56,    68,
      69,    72,    67,    66,    71,    70,    64,    63,    59,    60,
      61,    58,     0,     0,    44,    47,    62,     0,     0,     0,
       0,     8,     0,     5,     4,     3,    16,     0,    17,    21,
       0,    36,    55,    57,     0,    19,     0,     0,     0,     0,
      30,     7,    38,     6,     0,    18,    46,    20,     0,     0,
       0,     0,     0,     6,     6,     0,    39,     0,    40,     0,
       0,     6,    42,     0,    41,     6,     0,    43
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -115,  -115,  -101,  -114,  -115,    24,   122,  -115,  -115,  -115,
     116,   132,  -115,   -22,  -115,   -21,  -115,  -115,  -115,  -115,
    -115,  -115,  -115,  -115,   -23
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,   101,   102,   103,   104,     5,     6,   105,     7,
      13,    21,    15,     8,     9,    10,    46,    47,   106,   107,
     108,   109,    93,    94,   110
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      41,    42,   121,    44,    45,    11,     1,   -25,     2,   131,
      23,    48,    49,    50,    51,    52,    53,    54,   130,   138,
     139,    24,    25,    12,     4,    57,    58,   144,    16,    26,
      17,   146,    79,    80,    12,     4,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    95,    96,
      72,    27,    57,    58,    18,    44,   111,    19,    23,    28,
      29,    30,    31,    32,    33,    70,    71,    34,    35,    24,
     122,    36,   123,   124,   116,    37,    74,    72,    38,    75,
       1,    39,     2,    43,    40,     2,    76,    77,   115,    57,
      58,   126,   114,   113,   128,   129,    64,    65,    66,    67,
      68,    69,    70,    71,   117,   118,   120,   135,   119,   137,
      97,     1,    27,     2,    72,    98,   132,   143,    99,   100,
      28,    29,    30,    31,    32,    33,   136,   142,    34,    35,
     147,    22,    36,    20,    14,     0,    37,     0,    27,    38,
       0,     0,    39,     0,     0,    40,    28,    29,    30,    31,
      32,    33,     0,     0,    34,    35,     0,     0,    36,     0,
       0,     0,    37,     0,     0,    38,     0,     0,    39,     0,
       0,    40,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,     0,
       0,     0,     0,     0,     0,     0,     0,    73,     0,    72,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,     0,     0,     0,
       0,     0,     0,     0,    78,     0,     0,    72,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,     0,     0,     0,     0,     0,
       0,     0,     0,   112,     0,    72,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,     0,     0,     0,     0,     0,     0,   125,
       0,     0,     0,    72,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,     0,     0,     0,     0,     0,     0,   127,     0,     0,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,     0,
       0,     0,     0,     0,     0,     0,   133,     0,     0,    72,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,     0,     0,     0,
       0,     0,     0,     0,   134,     0,     0,    72,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,     0,     0,     0,     0,     0,
       0,   140,     0,     0,     0,    72,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,     0,     0,     0,     0,     0,     0,     0,
     141,     0,     0,    72,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,     0,     0,     0,     0,     0,     0,     0,   145,     0,
       0,    72,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,     0,
       0,     0,     0,     0,     0,    55,    56,    57,    58,    72,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,     0,     0,     0,     0,    55,    56,    57,    58,
       0,     0,    72,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,     0,     0,    57,    58,     0,    57,    58,
       0,     0,     0,    72,    66,    67,    68,    69,    70,    71,
      69,    70,    71,     0,     0,     0,     0,     0,     0,     0,
      72,     0,     0,    72
};

static const yytype_int16 yycheck[] =
{
      23,    24,   103,    25,    25,     5,     4,     5,     6,   123,
      23,    34,    35,    36,    37,    38,    39,    40,   119,   133,
     134,    34,    35,    32,     0,    21,    22,   141,     0,     7,
       6,   145,    55,    56,    32,    11,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      46,     5,    21,    22,     5,    77,    77,    42,    23,    13,
      14,    15,    16,    17,    18,    34,    35,    21,    22,    34,
       7,    25,     9,    10,    97,    29,     5,    46,    32,     5,
       4,    35,     6,     4,    38,     6,    43,    45,    42,    21,
      22,   114,    45,    43,   117,   118,    28,    29,    30,    31,
      32,    33,    34,    35,    35,    35,     7,   130,    35,   132,
       3,     4,     5,     6,    46,     8,    35,   140,    11,    12,
      13,    14,    15,    16,    17,    18,     7,     7,    21,    22,
       7,    15,    25,    11,     2,    -1,    29,    -1,     5,    32,
      -1,    -1,    35,    -1,    -1,    38,    13,    14,    15,    16,
      17,    18,    -1,    -1,    21,    22,    -1,    -1,    25,    -1,
      -1,    -1,    29,    -1,    -1,    32,    -1,    -1,    35,    -1,
      -1,    38,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,    46,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    -1,    -1,    46,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    44,    -1,    46,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    -1,    -1,    46,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    46,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,    -1,    46,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    -1,    -1,    46,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    -1,    -1,    46,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    -1,    46,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,
      -1,    46,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    19,    20,    21,    22,    46,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      -1,    -1,    46,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    -1,    21,    22,    -1,    21,    22,
      -1,    -1,    -1,    46,    30,    31,    32,    33,    34,    35,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    -1,    46
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     6,    48,    52,    53,    54,    56,    60,    61,
      62,     5,    32,    57,    58,    59,     0,    52,     5,    42,
      53,    58,    57,    23,    34,    35,     7,     5,    13,    14,
      15,    16,    17,    18,    21,    22,    25,    29,    32,    35,
      38,    71,    71,     4,    60,    62,    63,    64,    71,    71,
      71,    71,    71,    71,    71,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    46,    44,     5,     5,    43,    45,    43,    71,
      71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
      71,    71,    71,    69,    70,    71,    71,     3,     8,    11,
      12,    49,    50,    51,    52,    55,    65,    66,    67,    68,
      71,    62,    44,    43,    45,    42,    71,    35,    35,    35,
       7,    49,     7,     9,    10,    42,    71,    42,    71,    71,
      49,    50,    35,    43,    43,    71,     7,    71,    50,    50,
      42,    43,     7,    71,    50,    43,    50,     7
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    47,    48,    49,    49,    50,    50,    51,    51,    52,
      52,    52,    53,    53,    54,    54,    55,    55,    55,    55,
      55,    55,    56,    57,    58,    58,    59,    59,    60,    60,
      61,    62,    62,    62,    63,    63,    64,    64,    65,    65,
      66,    66,    67,    68,    69,    69,    70,    70,    71,    71,
      71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
      71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
      71,    71,    71,    71,    71,    71,    71,    71,    71,    71
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
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


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
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
     '$$ = $1'.

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
#line 217 "input/yaccin.y" /* yacc.c:1646  */
    {	
								YYLTYPE pos = (yyloc);
								ast_node *child1 = (yyvsp[0]);
								finalNode = ast_node_new(
									ast_token_here("program",0,NULL,pos),
									vector2_ptr_ast_node_here_from_list
									(1, child1)
								);
								printParsed(finalNode);
							}
#line 1726 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 230 "input/yaccin.y" /* yacc.c:1646  */
    {
								ast_node **res = &((yyval));
								*res = ast_node_new(
									ast_token_here("stmt",0,NULL,(yyloc)),
									vector2_ptr_ast_node_here_from_list
									(1, (yyvsp[0]))
								);
								printParsed(*res);
							}
#line 1740 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 240 "input/yaccin.y" /* yacc.c:1646  */
    {
								(yyval) = ast_node_new(
									ast_token_here("stmt",1,NULL,(yyloc)),
									vector2_ptr_ast_node_here_from_list
									(1, (yyvsp[0]))
								);
								printParsed((yyval));
							}
#line 1753 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 251 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("stmt_list",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
									ast_unroll_lists((yyval));
								}
#line 1767 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 261 "input/yaccin.y" /* yacc.c:1646  */
    {
									YYLTYPE *pos = &((yyloc));
									pos->null = 1;
									
									(yyval) = ast_node_new(
										ast_token_here("stmt_list",1,"<empty>",nullPos()),//@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed((yyval));
								}
#line 1783 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 275 "input/yaccin.y" /* yacc.c:1646  */
    {
										(yyval) = ast_node_new(
											ast_token_here("stmt_list_ne",0,NULL,(yyloc)),
											vector2_ptr_ast_node_here_from_list
											(2, (yyvsp[-1]), (yyvsp[0]))
										);
										printParsed((yyval));
									}
#line 1796 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 284 "input/yaccin.y" /* yacc.c:1646  */
    {
										(yyval) = ast_node_new(
											ast_token_here("stmt_list_ne",1,NULL,(yyloc)),
											vector2_ptr_ast_node_here_from_list
											(1, (yyvsp[0]))
										);
										printParsed((yyval));
									}
#line 1809 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 295 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("decl_stmt",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 1822 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 304 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("decl_stmt",1,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 1835 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 313 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("decl_stmt",2,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[-1]))
									);
									printParsed((yyval));
								}
#line 1848 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 324 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("decl_stmt_list",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
									ast_unroll_lists((yyval));
								}
#line 1862 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 334 "input/yaccin.y" /* yacc.c:1646  */
    {
									YYLTYPE *pos = &((yyloc));
									pos->null = 1;
									
									(yyval) = ast_node_new(
										ast_token_here("decl_stmt_list",1,"<empty>",nullPos()),//@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed((yyval));
								}
#line 1878 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 348 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("decl_stmt_list_ne",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(2, (yyvsp[-1]), (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 1891 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 357 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("decl_stmt_list_ne",1,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 1904 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 368 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("imp_stmt",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 1917 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 377 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("imp_stmt",1,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 1930 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 386 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("imp_stmt",2,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[-1]))
									);
									printParsed((yyval));
								}
#line 1943 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 395 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("imp_stmt",3,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed((yyval));
								}
#line 1956 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 404 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("imp_stmt",4,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[-1]))
									);
									printParsed((yyval));
								}
#line 1969 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 413 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("imp_stmt",5,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 1982 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 424 "input/yaccin.y" /* yacc.c:1646  */
    {
									const char *id_str = (yyvsp[-2])->token.value;
									printf("FUNC_DEF ID = [%s]\n",id_str);//(char*)$2);
									(yyval) = ast_node_new(
										ast_token_here("class_def",0,id_str, (yyloc)),
										vector2_ptr_ast_node_here_from_list
										(2, (yyvsp[-2]), (yyvsp[-1]))
									);
									printParsed((yyval));
								}
#line 1997 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 436 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("stars",0,"*",(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed((yyval));
								}
#line 2010 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 447 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("ptr_stars",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
									ast_unroll_lists((yyval));
								}
#line 2024 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 457 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("ptr_stars",1,"<empty>",nullPos()),//@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed((yyval));
								}
#line 2037 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 468 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("ptr_stars_ne",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(2, (yyvsp[-1]), (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2050 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 477 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("ptr_stars_ne",1,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2063 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 488 "input/yaccin.y" /* yacc.c:1646  */
    {
									const char *type_str = (yyvsp[-1])->token.value;
									(yyval) = ast_node_new(
										ast_token_here("typename",0,type_str, (yyloc)),
										vector2_ptr_ast_node_here_from_list
										(2, (yyvsp[-1]), (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2077 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 498 "input/yaccin.y" /* yacc.c:1646  */
    {
									const char *type_str = (yyvsp[-1])->token.value;
									(yyval) = ast_node_new(
										ast_token_here("typename",1,type_str, (yyloc)),
										vector2_ptr_ast_node_here_from_list
										(2, (yyvsp[-1]), (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2091 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 510 "input/yaccin.y" /* yacc.c:1646  */
    {
									const char *id_str = (yyvsp[-5])->token.value;
									printf("FUNC_DEF ID = [%s]\n", id_str);
									(yyval) = ast_node_new(
										ast_token_here("func_def", 0, id_str, (yyloc)),
										vector2_ptr_ast_node_here_from_list
										(4, (yyvsp[-6]), (yyvsp[-5]), (yyvsp[-3]), (yyvsp[-1]))
									);
									printParsed((yyval));
								}
#line 2106 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 522 "input/yaccin.y" /* yacc.c:1646  */
    {
									const char *id_str = (yyvsp[0])->token.value;
									(yyval) = ast_node_new(
										ast_token_here("var_decl",0,id_str, (yyloc)),
										vector2_ptr_ast_node_here_from_list
										(2, (yyvsp[-1]), (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2120 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 532 "input/yaccin.y" /* yacc.c:1646  */
    {
									const char *id_str = (yyvsp[-3])->token.value;
									(yyval) = ast_node_new(
										ast_token_here("var_decl",1,id_str, (yyloc)),
										vector2_ptr_ast_node_here_from_list
										(3, (yyvsp[-4]), (yyvsp[-3]), (yyvsp[-1]))
									);
									printParsed((yyval));
								}
#line 2134 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 542 "input/yaccin.y" /* yacc.c:1646  */
    {
									const char *id_str = (yyvsp[-2])->token.value;
									(yyval) = ast_node_new(
										ast_token_here("var_decl_assign",0,id_str,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(3, (yyvsp[-3]), (yyvsp[-2]), (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2148 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 554 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("var_decl_list",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
									ast_unroll_lists((yyval));
								}
#line 2162 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 564 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("var_decl_list",1,"<empty>",nullPos()),//@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed((yyval));
								}
#line 2175 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 575 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("var_decl_list_ne",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(2, (yyvsp[-2]), (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2188 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 584 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("var_decl_list_ne",1,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2201 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 595 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("if_block",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[-1]))
									);
									printParsed((yyval));
								}
#line 2214 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 604 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("if_block",1,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(2, (yyvsp[-3]), (yyvsp[-1]))
									);
									printParsed((yyval));
								}
#line 2227 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 615 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("if_then",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(2, (yyvsp[-2]), (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2240 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 624 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("if_then",2,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(3, (yyvsp[-5]), (yyvsp[-2]), (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2253 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 643 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("while_loop",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(2, (yyvsp[-3]), (yyvsp[-1]))
									);
									printParsed((yyval));
								}
#line 2266 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 654 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("for_loop",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(4, (yyvsp[-6]), (yyvsp[-5]), (yyvsp[-3]), (yyvsp[-1]))
									);
									printParsed((yyval));
								}
#line 2279 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 665 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("expr_list",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
									ast_unroll_lists((yyval));
								}
#line 2293 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 675 "input/yaccin.y" /* yacc.c:1646  */
    {
									YYLTYPE *pos = &((yyloc));
									pos->null = 1;
									
									(yyval) = ast_node_new(
										ast_token_here("expr_list",1,"<empty>",nullPos()),//@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);	
									printParsed((yyval));
								}
#line 2309 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 689 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("expr_list_ne",0,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(2, (yyvsp[-2]), (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2322 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 698 "input/yaccin.y" /* yacc.c:1646  */
    {
									(yyval) = ast_node_new(
										ast_token_here("expr_list_ne",1,NULL,(yyloc)),
										vector2_ptr_ast_node_here_from_list
										(1, (yyvsp[0]))
									);
									printParsed((yyval));
								}
#line 2335 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 708 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_id",	0,(yyvsp[0])->token.value,(yyloc)), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed((yyval));}
#line 2341 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 709 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_const",	0,(yyvsp[0])->token.value,(yyloc)), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed((yyval));}
#line 2347 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 710 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_const",	1,(yyvsp[0])->token.value,(yyloc)), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed((yyval));}
#line 2353 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 711 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_const",	2,(yyvsp[0])->token.value,(yyloc)), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed((yyval));}
#line 2359 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 712 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_const",	3,(yyvsp[0])->token.value,(yyloc)), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed((yyval));}
#line 2365 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 713 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_const",	4,(yyvsp[0])->token.value,(yyloc)), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed((yyval));}
#line 2371 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 714 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_const",	5,(yyvsp[0])->token.value,(yyloc)), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed((yyval));}
#line 2377 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 715 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_index",	0,NULL,(yyloc)),	vector2_ptr_ast_node_here_from_list(2, (yyvsp[-3]),(yyvsp[-1]))); printParsed((yyval));}
#line 2383 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 716 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_subexpr",0,NULL,(yyloc)),vector2_ptr_ast_node_here_from_list(1, (yyvsp[-1]))); printParsed((yyval));}
#line 2389 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 717 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_call",	0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-3]), (yyvsp[-1]))); printParsed((yyval));}
#line 2395 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 718 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_.",		0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2401 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 719 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_^",		0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2407 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 720 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_/",		0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2413 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 721 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_*",		0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2419 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 722 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_%",		0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2425 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 723 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_-",		0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2431 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 724 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_+",		0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2437 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 725 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_not",	0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(1, (yyvsp[0]))); printParsed((yyval));}
#line 2443 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 726 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_and",	0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2449 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 727 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_or",	0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2455 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 728 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_==",	0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2461 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 729 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_!=",	0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2467 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 730 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_>",		0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2473 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 731 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_<",		0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2479 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 732 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_=",		0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(2, (yyvsp[-2]), (yyvsp[0]))); printParsed((yyval));}
#line 2485 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 733 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_++",	0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(1, (yyvsp[-1]))); printParsed((yyval));}
#line 2491 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 734 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_++",	1,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(1, (yyvsp[-1]))); printParsed((yyval));}
#line 2497 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 735 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_--",	0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(1, (yyvsp[-1]))); printParsed((yyval));}
#line 2503 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 736 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_--",	1,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(1, (yyvsp[0]))); printParsed((yyval));}
#line 2509 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 737 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_neg",	0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(1, (yyvsp[0]))); printParsed((yyval));}
#line 2515 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 738 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_deref",	0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(1, (yyvsp[0]))); printParsed((yyval));}
#line 2521 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 739 "input/yaccin.y" /* yacc.c:1646  */
    {(yyval) = ast_node_new(ast_token_here("expr_ref",	0,NULL,(yyloc)), vector2_ptr_ast_node_here_from_list(1, (yyvsp[0]))); printParsed((yyval));}
#line 2527 "generated/yaccin.tab.c" /* yacc.c:1646  */
    break;


#line 2531 "generated/yaccin.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

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

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
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

#if !defined yyoverflow || YYERROR_VERBOSE
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 742 "input/yaccin.y" /* yacc.c:1906  */

extern FILE *yyin;
FILE *yyin2;

/*

|	expr INC	%prec POSTINC	
|	INC	expr    %prec PREINC	
|	expr DEC	%prec POSTDEC	
|	DEC	expr    %prec PREDEC	
|	'-' expr 	%prec PRENEG	
|	'*' expr	%prec PREDEREF	
|	'&' expr	%prec PREREF	

*/

int main(int argc, char **argv); //moved to main.c


int yyerror(const char *s)
{	
	printf("\nyyerror()!\n");
	int x1 = yylloc.first_column;
	int y1 = yylloc.first_line;
	int x2 = yylloc.last_column;
	//int y2 = yylloc.last_line;
	const char *f = yylloc.filename;
	int length = x2-x1;
	if(length == 1){length = 0;}
	point_out_error(y1,x1,f,s,length);
	fflush(stdout);
	
	error("syntax error. yytext: [%s]\n",(char*)yylval);
	//exit(1);
	return 1;
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
