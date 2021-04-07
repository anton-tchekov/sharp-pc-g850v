/* SHARP PC-G850V C REFERENCE */

/* ### Library Functions ### */

int getc(FILE *stream);
int getchar(void);
int fgetc(FILE *stream);
char *gets(char *s);
char *fgets(char *s, int n, FILE *stream);
int scanf(const char *format [, address, ...]);
int fscanf(FILE *stream, const char *format [, address, ...]);
int sscanf(char *s, const char *format [, address, ...]);
int putc(int c, FILE *stream);
int putchar(int c);
int fputc(int c, FILE *stream);
int puts(const char *s);
int fputs(const char *s, int n, FILE *stream);
int printf(const char *format [, arg, ...]);
int fprintf(FILE *stream, const char *format [, arg, ...]);
int sprintf(char *s, const char *format [, arg, ...]);
int fflush(FILE *stream);
void clearerr(FILE *stream);

/* ### Character Functions ### */

int isalnum(int c);
int isalpha(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);

int tolower(int c);
int toupper(int c);

/* ### String Functions ### */

char *strcat(char *s1, const char *s2);
char *strchr(const char *s, int c);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *s1, const char *s2);
int strlen(const char *string);

/* ### Memory Functions ### */

void *calloc(unsigned n, unsigned size);
void *malloc(unsigned size);
void free(void *ptr);

/* ### Math Functions ### */

int abs(int x);

double asin(double x);
double acos(double x);
double atan(double x);

double asinh(double x);
double acosh(double x);
double atanh(double x);

double exp(double x);

double log(double x);
double log10(double x);

double pow(double x, double y);

double sin(double x);
double cos(double x);
double tan(double x);

double sinh(double x);
double cosh(double x);
double tanh(double x);

double sqrt(double x);

/* ### SIO Serial Interface ### */

/* Open: */
fopen("stdaux1", "a+");

/* ### SIO I/O Port ### */

/* Open: */
fopen("pio", "a+");

int pioget(void);
void pioput(char byte);
void pioset(char byte);

/* ### File Access ### */

int fclose(FILE *stream);
int feof(FILE *stream);
unsigned long flof(FILE *stream);
FILE *fopen(char *path, char *type);

/* ### Graphics ### */

int circle(int x, int y, int r, double start_angle, double end_angle, double ratio, int reverse, unsigned short kind);
int gcursor(int x, int y);
void gprint(char *image);
int line(int x, int y, int x2, int y2, int reverse, unsigned short mask, int rectangle);
int paint(int x, int y, unsigned short kind);
int point(int x, int y);
int preset(int x, int y);
int pset(int x, int y, int reverse);

/* ### Other ### */

void abort(void);
void exit(int status);

void angle(unsigned n);
void breakpt(void);

void clrscr(void);
int getch(void);
void gotoxy(unsigned x, unsigned y);
int kbhit(void);

