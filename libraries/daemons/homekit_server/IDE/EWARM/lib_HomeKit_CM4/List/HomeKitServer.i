


















  































 
































 
































 



 
 

  #pragma system_include

 
 

 

  #pragma system_include






 















 












 






 
 


  #pragma system_include

 
 

 

  #pragma system_include






 















 





                 



  

                 













 

   



                 



























 


  #pragma system_include

 
 
 


  #pragma system_include

 

 

 

 

   

 

   #pragma system_include






 




 

 


 


 

 

 

 

 

 

 

 

 

 














 












 




















 










 




















































































































 


 










 





















 















 













 








 












 











 










 









 











 









 









 









 









 














 














 
















 












 








 











 










 









 









 









 









 









 









 









 









 







 




 









 




 





 
















































 













 






 


   

  







 





 





 




 



 





 




 



 












 
   






 
  #pragma language = save 
  #pragma language = extended
  #pragma language = restore






 





 




 





 








                 




















 


                 

 

 
 

 

 

 

 



                 






 
 


  #pragma system_include

 
 

 

  #pragma system_include






 















 




 
 





 

                                 













                 

                 

                 

                 
                 



                 
                                 


  #pragma language=save
  #pragma language=extended
  typedef long long _Longlong;
  typedef unsigned long long _ULonglong;
  #pragma language=restore

  typedef unsigned short int _Wchart;
  typedef unsigned short int _Wintt;



                 

typedef signed int  _Ptrdifft;
typedef unsigned int     _Sizet;

 

                 
  typedef _VA_LIST __Va_list;


__intrinsic __nounwind void __iar_Atexit(void (*)(void));


  typedef struct
  {        
    unsigned int _Wchar;
    unsigned int _State;
  } _Mbstatet;



  typedef struct __FILE _Filet;


typedef struct
{        
  _Longlong _Off;     
  _Mbstatet _Wstate;
} _Fpost;




                 














 


  #pragma system_include





 






















































































 

 


  
 

   


  





  #pragma language=save 
  #pragma language=extended
  __intrinsic __nounwind void __iar_dlib_perthread_initialize(void  *);
  __intrinsic __nounwind void  *__iar_dlib_perthread_allocate(void);
  __intrinsic __nounwind void __iar_dlib_perthread_destroy(void);
  __intrinsic __nounwind void __iar_dlib_perthread_deallocate(void  *);



  #pragma segment = "__DLIB_PERTHREAD" 
  #pragma segment = "__DLIB_PERTHREAD_init" 









   
  void  *__iar_dlib_perthread_access(void  *);
  #pragma language=restore








     
  



   
    __intrinsic __nounwind void __iar_Locale_lconv_init(void);

  


  
  typedef void *__iar_Rmtx;
  
  
  __intrinsic __nounwind void __iar_system_Mtxinit(__iar_Rmtx *m);
  __intrinsic __nounwind void __iar_system_Mtxdst(__iar_Rmtx *m);
  __intrinsic __nounwind void __iar_system_Mtxlock(__iar_Rmtx *m);
  __intrinsic __nounwind void __iar_system_Mtxunlock(__iar_Rmtx *m);

  __intrinsic __nounwind void __iar_file_Mtxinit(__iar_Rmtx *m);
  __intrinsic __nounwind void __iar_file_Mtxdst(__iar_Rmtx *m);
  __intrinsic __nounwind void __iar_file_Mtxlock(__iar_Rmtx *m);
  __intrinsic __nounwind void __iar_file_Mtxunlock(__iar_Rmtx *m);

  
 
  __intrinsic __nounwind void __iar_clearlocks(void);



  



  


  typedef unsigned _Once_t;

  








                 


                 
  
  
    
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_Locale(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_Malloc(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_Stream(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_Debug(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_StaticGuard(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock(unsigned int);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_Locale(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_Malloc(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_Stream(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_Debug(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_StaticGuard(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock(unsigned int);

  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Initdynamicfilelock(__iar_Rmtx *);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Dstdynamicfilelock(__iar_Rmtx *);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Lockdynamicfilelock(__iar_Rmtx *);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlockdynamicfilelock(__iar_Rmtx *);
  
  

                 


                 







 


 
  typedef signed char   int8_t;
  typedef unsigned char uint8_t;

  typedef signed short int   int16_t;
  typedef unsigned short int uint16_t;

  typedef signed int   int32_t;
  typedef unsigned int uint32_t;

  #pragma language=save
  #pragma language=extended
  typedef signed long long int   int64_t;
  typedef unsigned long long int uint64_t;
  #pragma language=restore


 
typedef signed char   int_least8_t;
typedef unsigned char uint_least8_t;

typedef signed short int   int_least16_t;
typedef unsigned short int uint_least16_t;

typedef signed int   int_least32_t;
typedef unsigned int uint_least32_t;

 
  #pragma language=save
  #pragma language=extended
  typedef signed long long int int_least64_t;
  #pragma language=restore
  #pragma language=save
  #pragma language=extended
  typedef unsigned long long int uint_least64_t;
  #pragma language=restore



 
typedef signed int   int_fast8_t;
typedef unsigned int uint_fast8_t;

typedef signed int   int_fast16_t;
typedef unsigned int uint_fast16_t;

typedef signed int   int_fast32_t;
typedef unsigned int uint_fast32_t;

  #pragma language=save
  #pragma language=extended
  typedef signed long long int int_fast64_t;
  #pragma language=restore
  #pragma language=save
  #pragma language=extended
  typedef unsigned long long int uint_fast64_t;
  #pragma language=restore

 
#pragma language=save
#pragma language=extended
typedef signed long long int   intmax_t;
typedef unsigned long long int uintmax_t;
#pragma language=restore


 
typedef signed long int   intptr_t;
typedef unsigned long int uintptr_t;

 
typedef int __data_intptr_t; typedef unsigned int __data_uintptr_t;

 

























 














 
 
 

 


  #pragma system_include










 
 
 


  #pragma system_include

 
 

 

  #pragma system_include






 















 




 
 


  #pragma system_include

 
 

 

  #pragma system_include






 















 






                 
typedef _Sizet size_t;

typedef unsigned int __data_size_t;





 
#pragma rtmodel="__dlib_full_locale_support",   "1"




extern int __aeabi_MB_CUR_MAX(void);





                 



                 
  typedef _Wchart wchar_t;

typedef struct
{        
  int quot;
  int rem;
} div_t;

typedef struct
{        
  long quot;
  long rem;
} ldiv_t;

    #pragma language=save
    #pragma language=extended
    typedef struct
    {      
      _Longlong quot;
      _Longlong rem;
    } lldiv_t;
    #pragma language=restore

                 
  
__intrinsic __nounwind int atexit(void (*)(void));
  __intrinsic __noreturn __nounwind void _Exit(int) ;
__intrinsic __noreturn __nounwind void exit(int) ;
__intrinsic __nounwind char * getenv(const char *);
__intrinsic __nounwind int system(const char *);



             __intrinsic __noreturn __nounwind void abort(void) ;
_Pragma("function_effects = no_state, no_errno")     __intrinsic __nounwind int abs(int);
             __intrinsic __nounwind void * calloc(size_t, size_t);
_Pragma("function_effects = no_state, no_errno")     __intrinsic __nounwind div_t div(int, int);
             __intrinsic __nounwind void free(void *);
_Pragma("function_effects = no_state, no_errno")     __intrinsic __nounwind long labs(long);
_Pragma("function_effects = no_state, no_errno")     __intrinsic __nounwind ldiv_t ldiv(long, long);
    #pragma language=save
    #pragma language=extended
    _Pragma("function_effects = no_state, no_errno") __intrinsic __nounwind long long llabs(long long);
    _Pragma("function_effects = no_state, no_errno") __intrinsic __nounwind lldiv_t lldiv(long long, long long);
    #pragma language=restore
             __intrinsic __nounwind void * malloc(size_t);
_Pragma("function_effects = no_write(1)")    __intrinsic __nounwind int mblen(const char *, size_t);
_Pragma("function_effects = no_read(1), no_write(2)") __intrinsic __nounwind size_t mbstowcs(wchar_t *, 
                                          const char *, size_t);
_Pragma("function_effects = no_read(1), no_write(2)") __intrinsic __nounwind int mbtowc(wchar_t *, const char *, 
                                     size_t);
             __intrinsic __nounwind int rand(void);
             __intrinsic __nounwind void srand(unsigned int);
             __intrinsic __nounwind void * realloc(void *, size_t);
_Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind long strtol(const char *, 
                                      char **, int);
_Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind unsigned long strtoul(const char *, char **, int);
_Pragma("function_effects = no_read(1), no_write(2)") __intrinsic __nounwind size_t wcstombs(char *, 
                                          const wchar_t *, size_t);
_Pragma("function_effects = no_read(1)")    __intrinsic __nounwind int wctomb(char *, wchar_t);
    #pragma language=save
    #pragma language=extended
    _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind long long strtoll(const char *, char **, int);
    _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind unsigned long long strtoull(const char *, 
                                                          char **, int);
    #pragma language=restore




    _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind unsigned long __iar_Stoul(const char *, char **, 
                                                        int);
    _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind float         __iar_Stof(const char *, char **, long);
    _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind double        __iar_Stod(const char *, char **, long);
    _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind long double   __iar_Stold(const char *, char **, 
                                                          long);
    _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind long          __iar_Stolx(const char *, char **, int, 
                                                        int *);
    _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind unsigned long __iar_Stoulx(const char *, char **,
                                                         int, int *);
    _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind float         __iar_Stofx(const char *, char **, 
                                                        long, int *);
    _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind double        __iar_Stodx(const char *, char **, 
                                                        long, int *);
    _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind long double   __iar_Stoldx(const char *, char **, 
                                                         long, int *);
      #pragma language=save
      #pragma language=extended
      _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind _Longlong   __iar_Stoll(const char *, char **, 
                                                        int);
      _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind _ULonglong   __iar_Stoull(const char *, char **, 
                                                          int);
      _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind _Longlong    __iar_Stollx(const char *, char **, 
                                                          int, int *);
      _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind _ULonglong   __iar_Stoullx(const char *, char **, 
                                                           int, int *);
      #pragma language=restore





typedef int _Cmpfun(const void *, const void *);

_Pragma("function_effects = no_write(1,2)") __intrinsic void * bsearch(const void *, 
                                                   const void *, size_t,
                                                   size_t, _Cmpfun *);
             __intrinsic void qsort(void *, size_t, size_t, 
                                               _Cmpfun *);
             __intrinsic void __qsortbbl(void *, size_t, size_t, 
                                                    _Cmpfun *);
_Pragma("function_effects = no_write(1)")    __intrinsic __nounwind double atof(const char *);
_Pragma("function_effects = no_write(1)")    __intrinsic __nounwind int atoi(const char *);
_Pragma("function_effects = no_write(1)")    __intrinsic __nounwind long atol(const char *);
    #pragma language=save
    #pragma language=extended
    _Pragma("function_effects = no_write(1)") __intrinsic __nounwind long long atoll(const char *);
    #pragma language=restore
  _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind float strtof(const char *, 
                                         char **);
  _Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind long double strtold(const char *, char **);
_Pragma("function_effects = no_write(1), no_read(2)") __intrinsic __nounwind double strtod(const char *, 
                                        char **);
                                        
                                        
               __intrinsic __nounwind size_t __iar_Mbcurmax(void);

  _Pragma("function_effects = no_state, no_errno")     __intrinsic __nounwind int __iar_DLib_library_version(void);
  




  
  typedef void _Atexfun(void);
  

                 
    #pragma inline=no_body
    double atof(const char *_S)
    {       
      return (__iar_Stod(_S, 0, 0));
    }

    #pragma inline=no_body
    int atoi(const char *_S)
    {       
      return ((int)__iar_Stoul(_S, 0, 10));
    }

    #pragma inline=no_body
    long atol(const char *_S)
    {       
      return ((long)__iar_Stoul(_S, 0, 10));
    }

        #pragma language=save
        #pragma language=extended
        #pragma inline=no_body
        long long atoll(const char *_S)
        {       
            return ((long long)__iar_Stoull(_S, 0, 10));
        }
        #pragma language=restore

    #pragma inline=no_body
    double strtod(const char * _S, char ** _Endptr)
    {       
      return (__iar_Stod(_S, _Endptr, 0));
    }

      #pragma inline=no_body
      float strtof(const char * _S, char ** _Endptr)
      {       
        return (__iar_Stof(_S, _Endptr, 0));
      }

      #pragma inline=no_body
      long double strtold(const char * _S, char ** _Endptr)
      {       
        return (__iar_Stold(_S, _Endptr, 0));
      }

    #pragma inline=no_body
    long strtol(const char * _S, char ** _Endptr, 
                int _Base)
    {       
      return (__iar_Stolx(_S, _Endptr, _Base, 0));
    }

    #pragma inline=no_body
    unsigned long strtoul(const char * _S, char ** _Endptr, 
                          int _Base)
    {       
      return (__iar_Stoul(_S, _Endptr, _Base));
    }

        #pragma language=save
        #pragma language=extended
        #pragma inline=no_body
        long long strtoll(const char * _S, char ** _Endptr,
                          int _Base)
        {       
            return (__iar_Stoll(_S, _Endptr, _Base));
        }

        #pragma inline=no_body
        unsigned long long strtoull(const char * _S, 
                                    char ** _Endptr, int _Base)
        {       
            return (__iar_Stoull(_S, _Endptr, _Base));
        }
        #pragma language=restore


  #pragma inline=no_body
  int abs(int i)
  {       
    return (i < 0 ? -i : i);
  }

  #pragma inline=no_body
  long labs(long i)
  {       
    return (i < 0 ? -i : i);
  }

      #pragma language=save
      #pragma language=extended
      #pragma inline=no_body
      long long llabs(long long i)
      {       
        return (i < 0 ? -i : i);
      }
      #pragma language=restore








 
 
 

  #pragma system_include

 
 

 

  #pragma system_include






 















 




 
 



 
 

  #pragma system_include






    struct __FILE
    {        
      unsigned short _Mode;
      unsigned char _Lockno;
      signed char _Handle;

       
       
       
      unsigned char *_Buf, *_Bend, *_Next;
       
       
      
  
      unsigned char *_Rend, *_Wend, *_Rback;

      
  
      _Wchart *_WRback, _WBack[2];

       
       
       
      unsigned char *_Rsave, *_WRend, *_WWend;

      _Mbstatet _Wstate;
      char *_Tmpnam;
      unsigned char _Back[1], _Cbuf;
    };

    
  

 

__intrinsic __nounwind int remove(const char *);
__intrinsic __nounwind int rename(const char *, const char *);









 



 
#pragma rtmodel="__dlib_file_descriptor","1"

                 

  typedef _Filet FILE;


      
      extern  FILE __iar_Stdin;
      extern  FILE __iar_Stdout;
      extern  FILE __iar_Stderr;
      






                 
typedef _Fpost fpos_t;

                 
#pragma language=save
#pragma language=extended


                 
  

  __intrinsic __nounwind void clearerr(FILE *);
  __intrinsic __nounwind int fclose(FILE *);
  __intrinsic __nounwind int feof(FILE *);
  __intrinsic __nounwind int ferror(FILE *);
  __intrinsic __nounwind int fflush(FILE *);
  __intrinsic __nounwind int fgetc(FILE *);
  __intrinsic __nounwind int fgetpos(FILE *, fpos_t *);
  __intrinsic __nounwind char * fgets(char *, int, FILE *);
  __intrinsic __nounwind FILE * fopen(const char *, const char *);
  _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int fprintf(FILE *, const char *, 
                                      ...);
  __intrinsic __nounwind int fputc(int, FILE *);
  __intrinsic __nounwind int fputs(const char *, FILE *);
  __intrinsic __nounwind size_t fread(void *, size_t, size_t, FILE *);
  __intrinsic __nounwind FILE * freopen(const char *, const char *,
                              FILE *);
  _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown") __intrinsic __nounwind int fscanf(FILE *, const char *, 
                                    ...);
  __intrinsic __nounwind int fseek(FILE *, long, int);
  __intrinsic __nounwind int fsetpos(FILE *, const fpos_t *);
  __intrinsic __nounwind long ftell(FILE *);
  __intrinsic __nounwind size_t fwrite(const void *, size_t, size_t, 
                             FILE *);

  __intrinsic __nounwind void rewind(FILE *);
  __intrinsic __nounwind void setbuf(FILE *, char *);
  __intrinsic __nounwind int setvbuf(FILE *, char *, int, size_t);
  __intrinsic __nounwind FILE * tmpfile(void);
  __intrinsic __nounwind int ungetc(int, FILE *);
  _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int vfprintf(FILE *, 
                                       const char *, __Va_list);
    _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown")  __intrinsic __nounwind int vfscanf(FILE *, const char *,
                                        __Va_list);

    __intrinsic __nounwind FILE * fdopen(signed char, const char *);
    __intrinsic __nounwind signed char fileno(FILE *);
    __intrinsic __nounwind int getw(FILE *);
    __intrinsic __nounwind int putw(int, FILE *);

  __intrinsic __nounwind int getc(FILE *);
  __intrinsic __nounwind int putc(int, FILE *);
  


              
_Pragma("function_effects = no_read(1)")    __intrinsic __nounwind char * __gets(char *, int);
_Pragma("function_effects = no_read(1)")    __intrinsic __nounwind char * gets(char *);
_Pragma("function_effects = no_write(1)")    __intrinsic __nounwind void perror(const char *);
_Pragma("function_effects = no_write(1)")    _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int printf(const char *, ...);
_Pragma("function_effects = no_write(1)")    __intrinsic __nounwind int puts(const char *);
_Pragma("function_effects = no_write(1)")    _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown")  __intrinsic __nounwind int scanf(const char *, ...);
_Pragma("function_effects = no_read(1), no_write(2)") _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int sprintf(char *, 
                                                 const char *, ...);
_Pragma("function_effects = no_write(1,2)") _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown")  __intrinsic __nounwind int sscanf(const char *, 
                                                const char *, ...);
             __intrinsic __nounwind char * tmpnam(char *);
              
             __intrinsic __nounwind int __ungetchar(int);
_Pragma("function_effects = no_write(1)")    _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int vprintf(const char *,
                                                 __Va_list);
  _Pragma("function_effects = no_write(1)")    _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown")  __intrinsic __nounwind int vscanf(const char *, 
                                                  __Va_list);
  _Pragma("function_effects = no_write(1,2)") _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown")  __intrinsic __nounwind int vsscanf(const char *, 
                                                   const char *, 
                                                   __Va_list);
_Pragma("function_effects = no_read(1), no_write(2)")  _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int vsprintf(char *, 
                                                   const char *,
                                                   __Va_list);
               
_Pragma("function_effects = no_write(1)")      __intrinsic __nounwind size_t __write_array(const void *, size_t, size_t);
  _Pragma("function_effects = no_read(1), no_write(3)") _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int snprintf(char *, size_t, 
                                                    const char *, ...);
  _Pragma("function_effects = no_read(1), no_write(3)") _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int vsnprintf(char *, size_t,
                                                     const char *, 
                                                     __Va_list);

              __intrinsic __nounwind int getchar(void);
              __intrinsic __nounwind int putchar(int);



#pragma language=restore

             
  #pragma inline
  int (getc)(FILE *_Str)
  {
    return fgetc(_Str);
  }

  #pragma inline
  int (putc)(int _C, FILE *_Str)
  {
    return fputc(_C, _Str);
  }







 
 
 

  #pragma system_include

 
 

 

  #pragma system_include






 















 




 
 












 


  #pragma system_include


  
  

  





 


  




 




  


 

  #pragma inline=forced_no_body
  __intrinsic __nounwind void * memcpy(void * _D, const void * _S, size_t _N)
  {
    __aeabi_memcpy(_D, _S, _N);
    return _D;
  }

  #pragma inline=forced_no_body
  __intrinsic __nounwind void * memmove(void * _D, const void * _S, size_t _N)
  {
    __aeabi_memmove(_D, _S, _N);
    return _D;
  }

  #pragma inline=forced_no_body
  __intrinsic __nounwind void * memset(void * _D, int _C, size_t _N)
  {
    __aeabi_memset(_D, _N, _C);
    return _D;
  }

  
  




                 

                 

_Pragma("function_effects = no_state, no_errno, no_write(1,2)")   __intrinsic __nounwind int        memcmp(const void *, const void *,
                                                size_t);
_Pragma("function_effects = no_state, no_errno, no_read(1), no_write(2), returns 1") __intrinsic __nounwind void *     memcpy(void *, 
                                                const void *, size_t);
_Pragma("function_effects = no_state, no_errno, no_read(1), no_write(2), returns 1") __intrinsic __nounwind void *     memmove(void *, const void *, size_t);
_Pragma("function_effects = no_state, no_errno, no_read(1), returns 1")    __intrinsic __nounwind void *     memset(void *, int, size_t);
_Pragma("function_effects = no_state, no_errno, no_write(2), returns 1")    __intrinsic __nounwind char *     strcat(char *, 
                                                const char *);
_Pragma("function_effects = no_state, no_errno, no_write(1,2)")   __intrinsic __nounwind int        strcmp(const char *, const char *);
_Pragma("function_effects = no_write(1,2)")     __intrinsic __nounwind int        strcoll(const char *, const char *);
_Pragma("function_effects = no_state, no_errno, no_read(1), no_write(2), returns 1") __intrinsic __nounwind char *     strcpy(char *, 
                                                const char *);
_Pragma("function_effects = no_state, no_errno, no_write(1,2)")   __intrinsic __nounwind size_t     strcspn(const char *, const char *);
                 __intrinsic __nounwind char *     strerror(int);
_Pragma("function_effects = no_state, no_errno, no_write(1)")      __intrinsic __nounwind size_t     strlen(const char *);
_Pragma("function_effects = no_state, no_errno, no_write(2), returns 1")    __intrinsic __nounwind char *     strncat(char *, 
                                                 const char *, size_t);
_Pragma("function_effects = no_state, no_errno, no_write(1,2)")   __intrinsic __nounwind int        strncmp(const char *, const char *, 
                                                 size_t);
_Pragma("function_effects = no_state, no_errno, no_read(1), no_write(2), returns 1") __intrinsic __nounwind char *     strncpy(char *, 
                                                 const char *, size_t);
_Pragma("function_effects = no_state, no_errno, no_write(1,2)")   __intrinsic __nounwind size_t     strspn(const char *, const char *);
_Pragma("function_effects = no_write(2)")        __intrinsic __nounwind char *     strtok(char *, 
                                                const char *);
_Pragma("function_effects = no_write(2)")        __intrinsic __nounwind size_t     strxfrm(char *, 
                                                 const char *, size_t);

  _Pragma("function_effects = no_write(1)")      __intrinsic __nounwind char *   strdup(const char *);
  _Pragma("function_effects = no_write(1,2)")   __intrinsic __nounwind int      strcasecmp(const char *, const char *);
  _Pragma("function_effects = no_write(1,2)")   __intrinsic __nounwind int      strncasecmp(const char *, const char *, 
                                                   size_t);
  _Pragma("function_effects = no_state, no_errno, no_write(2)")    __intrinsic __nounwind char *   strtok_r(char *, const char *, char **);
  _Pragma("function_effects = no_state, no_errno, no_write(1)")    __intrinsic __nounwind size_t   strnlen(char const *, size_t);



  _Pragma("function_effects = no_state, no_errno, no_write(1)")    __intrinsic __nounwind void *memchr(const void *_S, int _C, size_t _N);
  _Pragma("function_effects = no_state, no_errno, no_write(1)")    __intrinsic __nounwind char *strchr(const char *_S, int _C);
  _Pragma("function_effects = no_state, no_errno, no_write(1,2)") __intrinsic __nounwind char *strpbrk(const char *_S, const char *_P);
  _Pragma("function_effects = no_state, no_errno, no_write(1)")    __intrinsic __nounwind char *strrchr(const char *_S, int _C);
  _Pragma("function_effects = no_state, no_errno, no_write(1,2)") __intrinsic __nounwind char *strstr(const char *_S, const char *_P);


                 

                 

_Pragma("function_effects = no_state, no_errno, no_write(1)")    __intrinsic __nounwind void *__iar_Memchr(const void *, int, size_t);
_Pragma("function_effects = no_state, no_errno, no_write(1)")    __intrinsic __nounwind char *__iar_Strchr(const char *, int);
               __intrinsic __nounwind char *__iar_Strerror(int, char *);
_Pragma("function_effects = no_state, no_errno, no_write(1,2)") __intrinsic __nounwind char *__iar_Strpbrk(const char *, const char *);
_Pragma("function_effects = no_state, no_errno, no_write(1)")    __intrinsic __nounwind char *__iar_Strrchr(const char *, int);
_Pragma("function_effects = no_state, no_errno, no_write(1,2)") __intrinsic __nounwind char *__iar_Strstr(const char *, const char *);


                 
                 
    #pragma inline
    void *memchr(const void *_S, int _C, size_t _N)
    {
      return (__iar_Memchr(_S, _C, _N));
    }

    #pragma inline
    char *strchr(const char *_S, int _C)
    {
      return (__iar_Strchr(_S, _C));
    }

    #pragma inline
    char *strpbrk(const char *_S, const char *_P)
    {
      return (__iar_Strpbrk(_S, _P));
    }

    #pragma inline
    char *strrchr(const char *_S, int _C)
    {
      return (__iar_Strrchr(_S, _C));
    }

    #pragma inline
    char *strstr(const char *_S, const char *_P)
    {
      return (__iar_Strstr(_S, _P));
    }

  #pragma inline
  char *strerror(int _Err)
  {
    return (__iar_Strerror(_Err, 0));
  }









 
 
 

  #pragma system_include

 
 

 

  #pragma system_include






 















 






 
 


  #pragma system_include

 
 

 

  #pragma system_include






 















 





 
 

  #pragma system_include

 
 

  #pragma system_include

 
 

 

  #pragma system_include






 















 





 
 






 







 

 
#pragma diag_suppress = Pe076


__intrinsic __nounwind int __iar_Atthreadexit(void (*)(void));
__intrinsic __nounwind void __iar_Destroytls(void);














 


 
 

  #pragma system_include

 
 

 

  #pragma system_include






 















 




 
 






 

 
#pragma rtmodel="__dlib_file_descriptor","1"

 


                 



                 
typedef _Mbstatet mbstate_t;
struct tm;


  typedef _Wintt wint_t;


                 
  __intrinsic __nounwind wint_t fgetwc(_Filet *);
  __intrinsic __nounwind wchar_t * fgetws(wchar_t *, int, _Filet *);
  __intrinsic __nounwind wint_t fputwc(wchar_t, _Filet *);
  __intrinsic __nounwind int fputws(const wchar_t *, _Filet *);
  __intrinsic __nounwind int fwide(_Filet *, int);
  __intrinsic __nounwind int fwprintf(_Filet *, const wchar_t *, ...);
  __intrinsic __nounwind int fwscanf(_Filet *, const wchar_t *, ...);
  __intrinsic __nounwind wint_t getwc(_Filet *);
  __intrinsic __nounwind wint_t putwc(wchar_t, _Filet *);
  __intrinsic __nounwind wint_t ungetwc(wint_t, _Filet *);
  __intrinsic __nounwind int vfwprintf(_Filet *, const wchar_t *,
                             __Va_list);
    __intrinsic __nounwind int vfwscanf(_Filet *, const wchar_t *,
                              __Va_list);


__intrinsic __nounwind wint_t getwchar(void);
__intrinsic __nounwind wint_t __ungetwchar(wint_t);
__intrinsic __nounwind wint_t putwchar(wchar_t);
__intrinsic __nounwind int swprintf(wchar_t *, size_t, 
                          const wchar_t *, ...);
__intrinsic __nounwind int swscanf(const wchar_t *,
                         const wchar_t *, ...);
__intrinsic __nounwind int vswprintf(wchar_t *, size_t,
                           const wchar_t *, __Va_list);
__intrinsic __nounwind int vwprintf(const wchar_t *, __Va_list);
  __intrinsic __nounwind int vswscanf(const wchar_t *, const wchar_t *,
                            __Va_list);
  __intrinsic __nounwind int vwscanf(const wchar_t *, __Va_list);
__intrinsic __nounwind int wprintf(const wchar_t *, ...);
__intrinsic __nounwind int wscanf(const wchar_t *, ...);

                 
__intrinsic __nounwind size_t mbrlen(const char *, size_t,
                           mbstate_t *);
__intrinsic __nounwind size_t mbrtowc(wchar_t *, const char *, size_t,
                            mbstate_t *);
__intrinsic __nounwind size_t mbsrtowcs(wchar_t *, const char **,
                              size_t, mbstate_t *);
__intrinsic __nounwind int mbsinit(const mbstate_t *);
__intrinsic __nounwind size_t wcrtomb(char *, wchar_t, mbstate_t *);
__intrinsic __nounwind size_t wcsrtombs(char *, const wchar_t **,
                              size_t, mbstate_t *);
__intrinsic __nounwind long wcstol(const wchar_t *, wchar_t **, int);
__intrinsic __nounwind unsigned long wcstoul(const wchar_t *,
                                   wchar_t **, int);

    #pragma language=save
    #pragma language=extended
    __intrinsic __nounwind _Longlong wcstoll(const wchar_t *, 
                                   wchar_t **, int);
    __intrinsic __nounwind _ULonglong wcstoull(const wchar_t *, 
                                     wchar_t **, int);
    #pragma language=restore

                 
__intrinsic __nounwind wchar_t * wcscat(wchar_t *, const wchar_t *);
__intrinsic __nounwind int wcscmp(const wchar_t *, const wchar_t *);
__intrinsic __nounwind int wcscoll(const wchar_t *, const wchar_t *);
__intrinsic __nounwind wchar_t * wcscpy(wchar_t *, const wchar_t *);
__intrinsic __nounwind size_t wcscspn(const wchar_t *, const wchar_t *);
__intrinsic __nounwind size_t wcslen(const wchar_t *);
__intrinsic __nounwind wchar_t * wcsncat(wchar_t *, const wchar_t *, 
                               size_t);
__intrinsic __nounwind int wcsncmp(const wchar_t *, const wchar_t *, size_t);
__intrinsic __nounwind wchar_t * wcsncpy(wchar_t *, const wchar_t *,
                               size_t);
__intrinsic __nounwind size_t wcsspn(const wchar_t *, const wchar_t *);
__intrinsic __nounwind wchar_t * wcstok(wchar_t *, const wchar_t *,
                              wchar_t **);
__intrinsic __nounwind size_t wcsxfrm(wchar_t *, const wchar_t *, 
                            size_t);
__intrinsic __nounwind int wmemcmp(const wchar_t *, const wchar_t *, size_t);
__intrinsic __nounwind wchar_t * wmemcpy(wchar_t *, const wchar_t *, 
                               size_t);
__intrinsic __nounwind wchar_t * wmemmove(wchar_t *, const wchar_t *, size_t);
__intrinsic __nounwind wchar_t * wmemset(wchar_t *, wchar_t, size_t);

                 
__intrinsic __nounwind size_t wcsftime(wchar_t *, size_t,
                             const wchar_t *, 
                             const struct tm *);


__intrinsic __nounwind wint_t btowc(int);
  __intrinsic __nounwind float wcstof(const wchar_t *, wchar_t **);
  __intrinsic __nounwind long double wcstold(const wchar_t *,
                                   wchar_t **);
__intrinsic __nounwind double wcstod(const wchar_t *, wchar_t **);
__intrinsic __nounwind int wctob(wint_t);


  __intrinsic __nounwind wint_t __iar_Btowc(int);
  __intrinsic __nounwind int __iar_Wctob(wint_t);
  __intrinsic __nounwind double __iar_WStod(const wchar_t *, wchar_t **, long);
  __intrinsic __nounwind float __iar_WStof(const wchar_t *, wchar_t **, long);
  __intrinsic __nounwind long double __iar_WStold(const wchar_t *, wchar_t **, long);
  __intrinsic __nounwind unsigned long __iar_WStoul(const wchar_t *, wchar_t **, int);
  __intrinsic __nounwind _Longlong __iar_WStoll(const wchar_t *, wchar_t **, int);
  __intrinsic __nounwind _ULonglong __iar_WStoull(const wchar_t *, wchar_t **, int);

  __intrinsic __nounwind wchar_t * __iar_Wmemchr(const wchar_t *, wchar_t, size_t);
  __intrinsic __nounwind wchar_t * __iar_Wcschr(const wchar_t *, wchar_t);
  __intrinsic __nounwind wchar_t * __iar_Wcspbrk(const wchar_t *, const wchar_t *);
  __intrinsic __nounwind wchar_t * __iar_Wcsrchr(const wchar_t *, wchar_t);
  __intrinsic __nounwind wchar_t * __iar_Wcsstr(const wchar_t *, const wchar_t *);
  
  


 

  __intrinsic __nounwind wchar_t * wmemchr(const wchar_t *, wchar_t, size_t);
  __intrinsic __nounwind wchar_t * wcschr(const wchar_t *, wchar_t);
  __intrinsic __nounwind wchar_t * wcspbrk(const wchar_t *, const wchar_t *);
  __intrinsic __nounwind wchar_t * wcsrchr(const wchar_t *, wchar_t);
  __intrinsic __nounwind wchar_t * wcsstr(const wchar_t *, const wchar_t *);

    #pragma inline
    wchar_t * wmemchr(const wchar_t *_S, wchar_t _C, size_t _N)
    {
      return (__iar_Wmemchr(_S, _C, _N));
    }

    #pragma inline
    wchar_t * wcschr(const wchar_t *_S, wchar_t _C)
    {
      return (__iar_Wcschr(_S, _C));
    }

    #pragma inline
    wchar_t * wcspbrk(const wchar_t *_S, const wchar_t *_P)
    {
      return (__iar_Wcspbrk(_S, _P));
    }

    #pragma inline
    wchar_t * wcsrchr(const wchar_t *_S, wchar_t _C)
    {
      return (__iar_Wcsrchr(_S, _C));
    }

    #pragma inline
    wchar_t * wcsstr(const wchar_t *_S, const wchar_t *_P)
    {
      return (__iar_Wcsstr(_S, _P));
    }

  #pragma inline
  wint_t btowc(int _C)
  {        
    return (__iar_Btowc(_C));
  }

    #pragma inline
    float wcstof(const wchar_t *_S,
                 wchar_t **_Endptr)
    {        
      return (__iar_WStof(_S, _Endptr, 0));
    }

    #pragma inline
    long double wcstold(const wchar_t *_S,
                        wchar_t **_Endptr)
    {        
      return (__iar_WStold(_S, _Endptr, 0));
    }

      #pragma language=save
      #pragma language=extended
      #pragma inline
       _Longlong wcstoll(const wchar_t * _S, 
                         wchar_t ** _Endptr, int _Base)
       {
	return (__iar_WStoll(_S, _Endptr, _Base));
       }

      #pragma inline
      _ULonglong wcstoull(const wchar_t * _S, 
                          wchar_t ** _Endptr, int _Base)
      {
	return (__iar_WStoull(_S, _Endptr, _Base));
      }
      #pragma language=restore


  #pragma inline
  double wcstod(const wchar_t *_S,
                wchar_t **_Endptr)
  {        
    return (__iar_WStod(_S, _Endptr, 0));
  }


  #pragma inline
  unsigned long wcstoul(const wchar_t *_S,
                        wchar_t **_Endptr, int _Base)
  {        
    return (__iar_WStoul(_S, _Endptr, _Base));
  }

  #pragma inline
  int wctob(wint_t _Wc)
  {        
    return (__iar_Wctob(_Wc));
  }








 
 
 

  #pragma system_include

 
 

 

  #pragma system_include






 















 








 
#pragma rtmodel="__dlib_full_locale_support",   "1"

                 


                 
                 

                 





                 




                 

                 


 
 

  #pragma system_include

struct lconv
{        
   
  char *currency_symbol;
  char *int_curr_symbol;
  char *mon_decimal_point;
  char *mon_grouping;
  char *mon_thousands_sep;
  char *negative_sign;
  char *positive_sign;

  char frac_digits;
  char n_cs_precedes;
  char n_sep_by_space;
  char n_sign_posn;
  char p_cs_precedes;
  char p_sep_by_space;
  char p_sign_posn;

  char int_frac_digits;

   
  char *decimal_point;
  char *grouping;
  char *thousands_sep;
  char *_Frac_grouping;
  char *_Frac_sep;
  char *_False;
  char *_True;

   
  char *_No;
  char *_Yes;

   
  char int_n_cs_precedes;
  char int_n_sep_by_space;
  char int_n_sign_posn;
  char int_p_cs_precedes;
  char int_p_sep_by_space;
  char int_p_sign_posn;
};



                 

__intrinsic __nounwind struct __aeabi_lconv *__aeabi_localeconv(void);
__intrinsic __nounwind struct lconv *localeconv(void);
  __intrinsic __nounwind char *setlocale(int, const char *);








 
  



 


  

  



 

   






 


  #pragma system_include
































































































































































  

 


  enum
  {
    _LocaleC_id,
    _LocaleCount  
  };



  
  extern int _LocaleC_toupper(int);
  extern int _LocaleC_tolower(int);
  extern int _LocaleC_isalpha(int);
  extern int _LocaleC_iscntrl(int);
  extern int _LocaleC_islower(int);
  extern int _LocaleC_ispunct(int);
  extern int _LocaleC_isspace(int);
  extern int _LocaleC_isupper(int);
  extern wint_t _LocaleC_towupper(wint_t);
  extern wint_t _LocaleC_towlower(wint_t);
  extern int _LocaleC_iswalpha(wint_t);
  extern int _LocaleC_iswcntrl(wint_t);
  extern int _LocaleC_iswlower(wint_t);
  extern int _LocaleC_iswpunct(wint_t);
  extern int _LocaleC_iswspace(wint_t);
  extern int _LocaleC_iswupper(wint_t);
  extern int _LocaleC_iswdigit(wint_t);
  extern int _LocaleC_iswxdigit(wint_t);
  

  





 
#pragma rtmodel="__dlib_full_locale_support",   "1"




         __intrinsic __nounwind int isalnum(int);
         __intrinsic __nounwind int isalpha(int);
         __intrinsic __nounwind int isblank(int);
         __intrinsic __nounwind int iscntrl(int);
_Pragma("function_effects = no_state, no_errno") __intrinsic __nounwind int isdigit(int);
         __intrinsic __nounwind int isgraph(int);
         __intrinsic __nounwind int islower(int);
         __intrinsic __nounwind int isprint(int);
         __intrinsic __nounwind int ispunct(int);
         __intrinsic __nounwind int isspace(int);
         __intrinsic __nounwind int isupper(int);
_Pragma("function_effects = no_state, no_errno") __intrinsic __nounwind int isxdigit(int);
         __intrinsic __nounwind int tolower(int);
         __intrinsic __nounwind int toupper(int);


 

      #pragma inline=no_body
      int isblank(int _C)
      {
        return (   _C == ' '
                || _C == '\t'
                || isspace(_C));
      }

    #pragma inline=no_body
    int isdigit(int _C)
    {
      return _C >= '0' && _C <= '9';
    }

    #pragma inline=no_body
    int isxdigit(int _C)
    {
      return (   (_C >= 'a' && _C <= 'f')
              || (_C >= 'A' && _C <= 'F')
              || isdigit(_C));
    }

    #pragma inline=no_body
    int isalnum(int _C)
    {
      return (   isalpha(_C)
              || isdigit(_C));
    }

    #pragma inline=no_body
    int isprint(int _C)
    {
      return (   (_C >= ' ' && _C <= '\x7e')
              || isalpha(_C)
              || ispunct(_C));
    }

    #pragma inline=no_body
    int isgraph(int _C)
    {
      return (   _C != ' '
              && isprint(_C));
    }





      
 



  
  






 









 


 
 

  #pragma system_include

 
 

 

  #pragma system_include






 















 




 
 






                 


                 
  typedef _Ptrdifft ptrdiff_t;








 




 


  




 



 



 



 



 



 

void *memrchr( const void *s, int c, size_t n );
void iar_set_msp(void*);
















 




typedef uint8_t         Boolean;
typedef uint8_t         mico_bool_t;







typedef uint32_t  mico_time_t;         
typedef uint32_t  mico_utc_time_t;     
typedef uint64_t  mico_utc_time_ms_t;  


typedef int         OSStatus;




















 












 




 
























































 




 















 

 

 









 
static inline  char nibble_to_hexchar( uint8_t nibble )
{
    if (nibble > 9)
    {
        return (char)('A' + (nibble - 10));
    }
    else
    {
        return (char) ('0' + nibble);
    }
}









 
static inline  char hexchar_to_nibble( char hexchar, uint8_t* nibble )
{
    if ( ( hexchar >= '0' ) && ( hexchar <= '9' ) )
    {
        *nibble = (uint8_t)( hexchar - '0' );
        return 0;
    }
    else if ( ( hexchar >= 'A' ) && ( hexchar <= 'F' ) )
    {
        *nibble = (uint8_t) ( hexchar - 'A' + 10 );
        return 0;
    }
    else if ( ( hexchar >= 'a' ) && ( hexchar <= 'f' ) )
    {
        *nibble = (uint8_t) ( hexchar - 'a' + 10 );
        return 0;
    }
    return -1;
}







 




 





typedef enum
{
    WAIT_FOR_ANY_EVENT,
    WAIT_FOR_ALL_EVENTS,
} mico_event_flags_wait_option_t;

typedef uint32_t  mico_event_flags_t;
typedef void * mico_semaphore_t;
typedef void * mico_mutex_t;
typedef void * mico_thread_t;
typedef void * mico_queue_t;
typedef void * mico_event_t;
typedef void (*timer_handler_t)( void* arg );
typedef OSStatus (*event_handler_t)( void* arg );

typedef struct
{
    void *          handle;
    timer_handler_t function;
    void *          arg;
}mico_timer_t;

typedef struct
{
    mico_thread_t thread;
    mico_queue_t  event_queue;
} mico_worker_thread_t;

typedef struct
{
    event_handler_t        function;
    void*                  arg;
    mico_timer_t           timer;
    mico_worker_thread_t*  thread;
} mico_timed_event_t;

typedef uint32_t mico_thread_arg_t;
typedef void (*mico_thread_function_t)( mico_thread_arg_t arg );

extern mico_worker_thread_t mico_hardware_io_worker_thread;
extern mico_worker_thread_t mico_worker_thread;

 



 



 





 




 
void mico_rtos_enter_critical( void );




 
void mico_rtos_exit_critical( void );



 























 













 
OSStatus mico_rtos_create_thread( mico_thread_t* thread, uint8_t priority, const char* name, mico_thread_function_t function, uint32_t stack_size, mico_thread_arg_t arg );







 
OSStatus mico_rtos_delete_thread( mico_thread_t* thread );














 
OSStatus mico_rtos_create_worker_thread( mico_worker_thread_t* worker_thread, uint8_t priority, uint32_t stack_size, uint32_t event_queue_size );








 
OSStatus mico_rtos_delete_worker_thread( mico_worker_thread_t* worker_thread );








 
void mico_rtos_suspend_thread(mico_thread_t* thread);








 
void mico_rtos_suspend_all_thread(void);







 
long mico_rtos_resume_all_thread(void);












 
OSStatus mico_rtos_thread_join( mico_thread_t* thread );












 
OSStatus mico_rtos_thread_force_awake( mico_thread_t* thread );











 
_Bool mico_rtos_is_current_thread( mico_thread_t* thread );




 
mico_thread_t* mico_rtos_get_current_thread( void );






 
void mico_rtos_thread_sleep(uint32_t seconds);






 
void mico_rtos_thread_msleep(uint32_t milliseconds);






 
OSStatus mico_rtos_delay_milliseconds( uint32_t num_ms );








 
OSStatus mico_rtos_print_thread_status( char* buffer, int length );



 




 








 
OSStatus mico_rtos_init_semaphore( mico_semaphore_t* semaphore, int count );








 
OSStatus mico_rtos_set_semaphore( mico_semaphore_t* semaphore );













 
OSStatus mico_rtos_get_semaphore( mico_semaphore_t* semaphore, uint32_t timeout_ms );










 
OSStatus mico_rtos_deinit_semaphore( mico_semaphore_t* semaphore );


 




 











 
OSStatus mico_rtos_init_mutex( mico_mutex_t* mutex );












 
OSStatus mico_rtos_lock_mutex( mico_mutex_t* mutex );











 
OSStatus mico_rtos_unlock_mutex( mico_mutex_t* mutex );










 
OSStatus mico_rtos_deinit_mutex( mico_mutex_t* mutex );


 




 










 
OSStatus mico_rtos_init_queue( mico_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages );











 
OSStatus mico_rtos_push_to_queue( mico_queue_t* queue, void* message, uint32_t timeout_ms );














 
OSStatus mico_rtos_pop_from_queue( mico_queue_t* queue, void* message, uint32_t timeout_ms );








 
OSStatus mico_rtos_deinit_queue( mico_queue_t* queue );








 
_Bool mico_rtos_is_queue_empty( mico_queue_t* queue );








 
_Bool mico_rtos_is_queue_full( mico_queue_t* queue );



 




 










 
OSStatus mico_rtos_send_asynchronous_event( mico_worker_thread_t* worker_thread, event_handler_t function, void* arg );

















 
OSStatus mico_rtos_register_timed_event( mico_timed_event_t* event_object, mico_worker_thread_t* worker_thread, event_handler_t function, uint32_t time_ms, void* arg );











 
OSStatus mico_rtos_deregister_timed_event( mico_timed_event_t* event_object );




 




 







 
uint32_t mico_rtos_get_time(void);















 
OSStatus mico_init_timer( mico_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg );










 
OSStatus mico_start_timer( mico_timer_t* timer );










 
OSStatus mico_stop_timer( mico_timer_t* timer );











 
OSStatus mico_reload_timer( mico_timer_t* timer );










 
OSStatus mico_deinit_timer( mico_timer_t* timer );








 
_Bool mico_is_timer_running( mico_timer_t* timer );

int SetTimer(unsigned long ms, void (*psysTimerHandler)(void));
int SetTimer_uniq(unsigned long ms, void (*psysTimerHandler)(void));
int UnSetTimer(void (*psysTimerHandler)(void));








 
int mico_create_event_fd(mico_event_t event_handle);






 
int mico_delete_event_fd(int fd);



 




 



 















 

#pragma once



 



 






 


 



 



 



 



 




    








    










 

              









 









 









 








 









 










 










 








 









 








 









 










 








 




   





 






typedef struct
{
    int num_of_chunks;  
    int total_memory;  
    int allocted_memory;  
    int free_memory;  
} micoMemInfo_t;








 
micoMemInfo_t* mico_memory_info( void );







 


 
































 






























 










extern int errno;










 




 

 
struct in_addr {
    uint32_t s_addr;
};

 
 
 
 

 
struct sockaddr_in {
    uint8_t sin_len;
    uint8_t sin_family;
    uint16_t sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
};

struct sockaddr {
    uint8_t sa_len;
    uint8_t sa_family;
    uint8_t sa_data[14];
};


struct hostent {
    char  *h_name;       
    char **h_aliases;   
 
    int    h_addrtype;   
    int    h_length;     
    char **h_addr_list; 
 
};


struct addrinfo {
    int               ai_flags;       
    int               ai_family;      
    int               ai_socktype;    
    int               ai_protocol;    
    uint32_t         ai_addrlen;     
    struct sockaddr  *ai_addr;        
    char             *ai_canonname;   
    struct addrinfo  *ai_next;        
};

 






 

typedef struct ip_mreq {
    struct in_addr imr_multiaddr;  
    struct in_addr imr_interface;  
} ip_mreq;




 
typedef enum {
    SO_DEBUG              = 0x0001,      
    SO_ACCEPTCONN         = 0x0002,      
    SO_REUSEADDR          = 0x0004,      
    SO_KEEPALIVE          = 0x0008,      
    SO_DONTROUTE          = 0x0010,      
    SO_BROADCAST          = 0x0020,      
    SO_USELOOPBACK        = 0x0040,      
    SO_LINGER             = 0x0080,      
    SO_OOBINLINE          = 0x0100,      
    SO_REUSEPORT          = 0x0200,      
    SO_BLOCKMODE          = 0x1000,     
 
    SO_SNDBUF             = 0x1001,
    SO_SNDTIMEO           = 0x1005,      
    SO_RCVTIMEO           = 0x1006,      
    SO_ERROR              = 0x1007,      
    SO_TYPE               = 0x1008,      
    SO_NO_CHECK           = 0x100a       

} SOCK_OPT_VAL;

struct pollfd {
	int fd;  
	short events;  
	short revents;  
};




 
typedef enum {
    IP_ADD_MEMBERSHIP       = 0x0003,      
    IP_DROP_MEMBERSHIP      = 0x0004,      
    IP_MULTICAST_TTL        = 0x0005,
    IP_MULTICAST_IF         = 0x0006,
    IP_MULTICAST_LOOP       = 0x0007
} IP_OPT_VAL;



 
typedef enum {
	TCP_NODELAY             = 0x0001,
	TCP_KEEPALIVE           = 0x0002,
    TCP_CONN_NUM            = 0x0006,      
    TCP_MAX_CONN_NUM        = 0x0007,      
    TCP_KEEPIDLE            = 0x0003,      
    TCP_KEEPINTVL           = 0x0004,      
    TCP_KEEPCNT             = 0x0005,      
} TCP_OPT_VAL;


                                        
 




typedef void* mico_ssl_t;



 
enum ssl_version_type_e
{
  SSL_V3_MODE   = 1,
  TLS_V1_0_MODE = 2,
  TLS_V1_1_MODE = 3,
  TLS_V1_2_MODE = 4,
};
typedef uint8_t ssl_version_type_t;


struct timeval {
    long      tv_sec;      
    long      tv_usec;     
};




typedef struct fd_set {
  unsigned long   fds_bits[(((64) + (((sizeof(unsigned long) * 8)) - 1)) / ((sizeof(unsigned long) * 8)))];
} fd_set;









 





















 
int socket(int domain, int type, int protocol);











 
int setsockopt (int socket, int level, int optname, void *optval, uint32_t optlen);













 
int getsockopt (int socket, int level, int optname, void *optval, uint32_t *optlen_ptr);










 
int bind (int socket, struct sockaddr *addr, uint32_t length);










 
int connect (int socket, struct sockaddr *addr, uint32_t length);












 
int listen (int socket, int n);















 
int accept (int socket, struct sockaddr *addr, uint32_t *length_ptr);


























 
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

int poll(struct pollfd *fds, int nfds, int timeout);


















 
int send (int socket, const void *buffer, unsigned int size, int flags);





 
int write (int filedes, const void *buffer, unsigned int size);


















 
int sendto (int socket, const void *buffer, unsigned int size, int flags, const struct sockaddr *addr, uint32_t length);




















 
int recv (int socket, void *buffer, unsigned int size, int flags);





 
int read (int filedes, void *buffer, unsigned int size);





























 
int recvfrom (int socket, void *buffer, unsigned int size, int flags, struct sockaddr *addr, uint32_t *length_ptr);
 








 
int close (int filedes);

int shutdown(int s, int how);
int ioctl(int s, long cmd, void *argp);



 





 







 
uint32_t inet_addr (const char *name);











 
extern char *inet_ntoa (struct in_addr addr);


















 
struct hostent* gethostbyname(const char *name);
int getaddrinfo(const char *nodename,
       const char *servname,
       const struct addrinfo *hints,
       struct addrinfo **res);
void freeaddrinfo(struct addrinfo *ai);
int getpeername (int s, struct sockaddr *name, uint32_t *namelen);
int getsockname (int s, struct sockaddr *name, uint32_t *namelen);


















 
void set_tcp_keepalive(int inMaxErrNum, int inSeconds);









 
void get_tcp_keepalive(int *outMaxErrNum, int *outSeconds);


 








 
void ssl_set_client_version( ssl_version_type_t version );









 
int ssl_socket( mico_ssl_t ssl );












 
void ssl_set_cert(const char *_cert_pem, const char *private_key_pem);













 
mico_ssl_t ssl_connect(int fd, int calen, char*ca, int *errno);






 
mico_ssl_t ssl_accept(int fd);










 
int ssl_send(mico_ssl_t ssl, void* data, unsigned int len);









 
int ssl_recv(mico_ssl_t ssl, void* data, unsigned int len);






 
int ssl_close(mico_ssl_t ssl);


int ssl_pending(void*ssl);
int ssl_get_error(void* ssl, int ret);
void ssl_set_using_nonblock(void* ssl, int nonblock);




 






 



 


































 

#pragma once

















 

#pragma once






























 





  





 




 









  
typedef enum { 
  Soft_AP,   
  Station    
} wlanInterfaceTypedef; 



  
enum wlan_sec_type_e{
   SECURITY_TYPE_NONE,         
   SECURITY_TYPE_WEP,          
   SECURITY_TYPE_WPA_TKIP,     
   SECURITY_TYPE_WPA_AES,      
   SECURITY_TYPE_WPA2_TKIP,    
   SECURITY_TYPE_WPA2_AES,     
   SECURITY_TYPE_WPA2_MIXED,   
   SECURITY_TYPE_AUTO,         
};

typedef uint8_t wlan_sec_type_t;



  
typedef struct {
  uint8_t dhcp;        
  char    ip[16];      
  char    gate[16];    
  char    mask[16];    
  char    dns[16];     
  char    mac[16];     
  char    broadcastip[16];
} IPStatusTypedef;
 


   
typedef  struct  _ScanResult_adv 
{ 
  char ApNum;        
  struct { 
    char ssid[32];   
    char ApPower;    
    char bssid[6];   
    char channel;    
    wlan_sec_type_t security;    
  } * ApList;
} ScanResult_adv; 



 
typedef  struct  _ScanResult 
{  
  char ApNum;        
  struct {  
    char ssid[32];   
    char ApPower;    
  } * ApList; 
} ScanResult;  



 
typedef struct _network_InitTypeDef_st 
{ 
  char wifi_mode;                
  char wifi_ssid[32];            
  char wifi_key[64];             
  char local_ip_addr[16];        
  char net_mask[16];             
  char gateway_ip_addr[16];      
  char dnsServer_ip_addr[16];    
  char dhcpMode;                 
  char reserved[32];            
  int  wifi_retry_interval;     
 
} network_InitTypeDef_st; 



 
typedef struct   
{ 
  char    ssid[32];     
  char    bssid[6];     
  uint8_t channel;     

 
  wlan_sec_type_t security;
}   apinfo_adv_t;



 
typedef struct _network_InitTypeDef_adv_st
{
  apinfo_adv_t ap_info;          
  char  key[64];                 
  int   key_len;                 
  char  local_ip_addr[16];       
  char  net_mask[16];            
  char  gateway_ip_addr[16];     
  char  dnsServer_ip_addr[16];   
  char  dhcpMode;                
  char  reserved[32]; 
  int   wifi_retry_interval;    
 
} network_InitTypeDef_adv_st;



 
typedef struct _linkStatus_t{
  int is_connected;        
  int wifi_strength;       
  uint8_t  ssid[32];       
  uint8_t  bssid[6];       
  int      channel;        
} LinkStatusTypeDef;


typedef struct {
    char bssid[6];
    char ssid[33];
    char key[65];
    int  user_data_len;
    char user_data[65];
} easylink_result_t;




 

void wlan_get_mac_address( uint8_t *mac );


















 
OSStatus StartNetwork(network_InitTypeDef_st* inNetworkInitPara);


















 
OSStatus StartAdvNetwork(network_InitTypeDef_adv_st* inNetworkInitParaAdv);










 
OSStatus getNetPara(IPStatusTypedef *outNetpara, wlanInterfaceTypedef inInterface);







 
OSStatus CheckNetLink(LinkStatusTypeDef *outStatus);







 
void mxchipStartScan(void);







 
void mxchipStartAdvScan(void);





 
OSStatus wifi_power_down(void);








 
OSStatus wifi_power_up(void);








 
OSStatus wlan_disconnect(void);








 
OSStatus sta_disconnect(void);





 
OSStatus uap_stop(void);


 




 




















 
OSStatus OpenEasylink2_withdata(int inTimeout);
















 

OSStatus OpenEasylink(int inTimeout);





 
OSStatus CloseEasylink2(void);
OSStatus CloseEasylink(void);



 






 







 
OSStatus OpenConfigmodeWPS(int inTimeout);





 
OSStatus CloseConfigmodeWPS(void);




 





 














 
OSStatus OpenAirkiss(int inTimeout);




 
OSStatus CloseAirkiss(void);


 




 







 

void ps_enable(void);





 
void ps_disable(void); 



 




 







 
void wifimgr_debug_enable(_Bool enable);

 




 
typedef void (*monitor_cb_t)(uint8_t*data, int len);
enum {
	WLAN_RX_BEACON,     
	WLAN_RX_PROBE_REQ,  
	WLAN_RX_PROBE_RES,  
	WLAN_RX_ACTION,     
	WLAN_RX_MANAGEMENT, 
	WLAN_RX_DATA,       
	WLAN_RX_MCAST_DATA, 

	WLAN_RX_ALL,        
};




 
int mico_wlan_monitor_rx_type(int type);





 
int mico_wlan_start_monitor(void);



 
int mico_wlan_stop_monitor(void);





 
int mico_wlan_set_channel(int channel);



 
void mico_wlan_register_monitor_cb(monitor_cb_t fn);



 




 



 











 

typedef enum
{
  eState_Normal,
  eState_Software_Reset,
  eState_Wlan_Powerdown,
  eState_Restore_default,
  eState_Standby,
} system_state_t;

enum  config_state_type_e{
  
 
  unConfigured,
  


                 
  wLanUnConfigured,
  
 
  allConfigured,
  
 
  mfgConfigured,
  
 
  unConfigured2
};
typedef uint8_t config_state_type_t;

 
typedef struct  _boot_table_t {
  uint32_t start_address; 
  uint32_t length; 
  uint8_t version[8];
  uint8_t type; 
  uint8_t upgrade_type; 
  uint16_t crc;
  uint8_t reserved[4];
}boot_table_t;

typedef struct _mico_sys_config_t
{
   
  char            name[32];

   
  char            ssid[32];
  char            user_key[64]; 
  int             user_keyLength;
  char            key[64]; 
  int             keyLength;
  char            bssid[6];
  int             channel;
  wlan_sec_type_t security;

   
  _Bool            rfPowerSaveEnable;
  _Bool            mcuPowerSaveEnable;

   
  _Bool            dhcpEnable;
  char            localIp[16];
  char            netMask[16];
  char            gateWay[16];
  char            dnsServer[16];

   
  config_state_type_t   configured;
  uint8_t               easyLinkByPass;
  uint32_t              reserved;

   
  uint32_t        magic_number;

   
  int32_t         seed;
} mico_sys_config_t;




typedef struct {

   
  boot_table_t             bootTable;

   
  mico_sys_config_t        micoSystemConfig;

} system_config_t;

typedef struct
{
  system_state_t        current_sys_state;
  mico_semaphore_t      sys_state_change_sem;
   
  char                  localIp[16];
  char                  netMask[16];
  char                  gateWay[16];
  char                  dnsServer[16];
  char                  mac[18];
  char                  rf_version[50];
} system_status_wlan_t;


typedef enum {
  NOTIFY_STATION_UP = 1,
  NOTIFY_STATION_DOWN,

  NOTIFY_AP_UP,
  NOTIFY_AP_DOWN,
} notify_wlan_t; 











 


 
struct cli_command {
	 
	const char *name;
	 
	const char *help;
	 
	void (*function) (char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
};












 
int cli_register_command(const struct cli_command *command);








 
int cli_unregister_command(const struct cli_command *command);








 
int cli_stop(void);









 
int cli_register_commands(const struct cli_command *commands, int num_commands);







 
int cli_unregister_commands(const struct cli_command *commands,
			    int num_commands);










 
int cli_getchar(char *inbuf);






 
int cli_printf(const char *buff, ...);



void wifistate_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void wifidebug_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void wifiscan_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void ifconfig_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void arp_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void ping_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void dns_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void socket_show_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void memory_show_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void memory_dump_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void memory_set_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void memp_dump_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void driver_state_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);
void iperf_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char * *argv);












 












 
















 



extern void mc_set_debug(int debug);
extern int mc_get_debug(void);

extern void mc_set_syslog(int syslog);
extern void mc_abort(const char *msg, ...);
extern void mc_debug(const char *msg, ...);
extern void mc_error(const char *msg, ...);
extern void mc_info(const char *msg, ...);













 
 




 



 



 

struct lh_entry;



 
typedef void (lh_entry_free_fn) (struct lh_entry *e);


 
typedef unsigned long (lh_hash_fn) (const void *k);


 
typedef int (lh_equal_fn) (const void *k1, const void *k2);



 
struct lh_entry {
	

 
	void *k;
	

 
	const void *v;
	

 
	struct lh_entry *next;
	

 
	struct lh_entry *prev;
};




 
struct lh_table {
	

 
	unsigned char size;
	

 
	unsigned char count;

	

 
	struct lh_entry *head;

	

 
	struct lh_entry *tail;

	struct lh_entry *table;

	

 
	lh_entry_free_fn *free_fn;
	lh_hash_fn *hash_fn;
	lh_equal_fn *equal_fn;
};




 
extern unsigned long lh_ptr_hash(const void *k);
extern int lh_ptr_equal(const void *k1, const void *k2);

extern unsigned long lh_char_hash(const void *k);
extern int lh_char_equal(const void *k1, const void *k2);




 



 



















 
extern struct lh_table* lh_table_new(int size, const char *name,
				     lh_entry_free_fn *free_fn,
				     lh_hash_fn *hash_fn,
				     lh_equal_fn *equal_fn);








 
extern struct lh_table* lh_kchar_table_new(int size, const char *name,
					   lh_entry_free_fn *free_fn);









 
extern struct lh_table* lh_kptr_table_new(int size, const char *name,
					  lh_entry_free_fn *free_fn);







 
extern void lh_table_free(struct lh_table *t);







 
extern int lh_table_insert(struct lh_table *t, void *k, const void *v);







 
extern struct lh_entry* lh_table_lookup_entry(struct lh_table *t, const void *k);






 
extern const void* lh_table_lookup(struct lh_table *t, const void *k);










 
extern int lh_table_delete_entry(struct lh_table *t, struct lh_entry *e);










 
extern int lh_table_delete(struct lh_table *t, const void *k);


void lh_abort(const char *msg, ...);
void lh_table_resize(struct lh_table *t, int new_size);












 




typedef void (array_list_free_fn) (void *data);

struct array_list
{
  void **array;
  int length;
  int size;
  array_list_free_fn *free_fn;
};

extern struct array_list*
array_list_new(array_list_free_fn *free_fn);

extern void
array_list_free(struct array_list *al);

extern void*
array_list_get_idx(struct array_list *al, int i);

extern int
array_list_put_idx(struct array_list *al, int i, void *data);

extern int
array_list_add(struct array_list *al, void *data);

extern int
array_list_length(struct array_list *al);













 











 















 




struct printbuf {
  char *buf;
  int bpos;
  int size;
};

extern struct printbuf*
printbuf_new(void);







 
extern int
printbuf_memappend(struct printbuf *p, const char *buf, int size);


extern int
sprintbuf(struct printbuf *p, const char *msg, ...);

extern void
printbuf_reset(struct printbuf *p);

extern void
printbuf_free(struct printbuf *p);









extern const char *json_number_chars;
extern const char *json_hex_chars;




 




 


 
struct json_object_iter
{
	char *key;
	struct json_object *val;
	struct lh_entry *entry;
};

 
typedef int boolean;
typedef struct printbuf printbuf;
typedef struct lh_table lh_table;
typedef struct array_list array_list;
typedef struct json_object json_object;
typedef struct json_object_iter json_object_iter;
typedef struct json_tokener json_tokener;

 

typedef enum json_type {
   
  json_type_null,
  json_type_boolean,
  json_type_double,
  json_type_int,
  json_type_object,
  json_type_array,
  json_type_string,
} json_type;

 





 







 
extern struct json_object* json_object_get(struct json_object *obj);







 
extern void json_object_put(struct json_object *obj);












 
extern int json_object_is_type(struct json_object *obj, enum json_type type);













 
extern enum json_type json_object_get_type(struct json_object *obj);








 
extern const char* json_object_to_json_string(struct json_object *obj);


 





 





 
extern struct json_object* json_object_new_object(void);





 
extern struct lh_table* json_object_get_object(struct json_object *obj);













 
extern void json_object_object_add(struct json_object* obj, const char *key,
				   struct json_object *val);






 
extern struct json_object* json_object_object_get(struct json_object* obj,
						  const char *key);








 
extern void json_object_object_del(struct json_object* obj, const char *key);






 







 

 

 





 




 
extern struct json_object* json_object_new_array(void);





 
extern struct array_list* json_object_get_array(struct json_object *obj);





 
extern int json_object_array_length(struct json_object *obj);










 
extern int json_object_array_add(struct json_object *obj,
				 struct json_object *val);
















 
extern int json_object_array_put_idx(struct json_object *obj, int idx,
				     struct json_object *val);





 
extern struct json_object* json_object_array_get_idx(struct json_object *obj,int idx);
 

 





 						     




 
extern struct json_object* json_object_new_boolean(boolean b);











 
extern boolean json_object_get_boolean(struct json_object *obj);

 

 





 	






 
extern struct json_object* json_object_new_int(int32_t i);





 
extern struct json_object* json_object_new_int64(int64_t i);














 
extern int32_t json_object_get_int(struct json_object *obj);









 
extern int64_t json_object_get_int64(struct json_object *obj);

 

 





 	





 
extern struct json_object* json_object_new_double(double d);









 
extern double json_object_get_double(struct json_object *obj);

 

 





 	







 
extern struct json_object* json_object_new_string(const char *s);

extern struct json_object* json_object_new_string_len(const char *s, int len);











 
extern const char* json_object_get_string(struct json_object *obj);
extern struct printbuf * json_object_to_json_string_ex(struct json_object *jso);









 
extern int json_object_get_string_len(struct json_object *obj);

 

 



 




 







 
extern struct json_object* json_object_from_file(const char *filename);
extern int json_object_to_file(char *filename, struct json_object *obj);
extern int json_parse_int64(const char *buf, int64_t *retval);




 
extern const char *json_type_to_name(enum json_type o_type);












 


 
 






 


enum json_tokener_error {
  json_tokener_success,
  json_tokener_continue,
  json_tokener_error_depth,
  json_tokener_error_parse_eof,
  json_tokener_error_parse_unexpected,
  json_tokener_error_parse_null,
  json_tokener_error_parse_boolean,
  json_tokener_error_parse_number,
  json_tokener_error_parse_array,
  json_tokener_error_parse_object_key_name,
  json_tokener_error_parse_object_key_sep,
  json_tokener_error_parse_object_value_sep,
  json_tokener_error_parse_string,
  json_tokener_error_parse_comment
};

enum json_tokener_state {
  json_tokener_state_eatws,
  json_tokener_state_start,
  json_tokener_state_finish,
  json_tokener_state_null,
  json_tokener_state_comment_start,
  json_tokener_state_comment,
  json_tokener_state_comment_eol,
  json_tokener_state_comment_end,
  json_tokener_state_string,
  json_tokener_state_string_escape,
  json_tokener_state_escape_unicode,
  json_tokener_state_boolean,
  json_tokener_state_number,
  json_tokener_state_array,
  json_tokener_state_array_add,
  json_tokener_state_array_sep,
  json_tokener_state_object_field_start,
  json_tokener_state_object_field,
  json_tokener_state_object_field_end,
  json_tokener_state_object_value,
  json_tokener_state_object_value_add,
  json_tokener_state_object_sep
};

struct json_tokener_srec
{
  enum json_tokener_state state, saved_state;
  struct json_object *obj;
  struct json_object *current;
  char *obj_field_name;
};


struct json_tokener
{
  char *str;
  struct printbuf *pb;
  int depth, is_double, st_pos, char_offset;
  enum json_tokener_error err;
  unsigned int ucs_char;
  char quote_char;
  struct json_tokener_srec stack[32];
};

extern const char* json_tokener_errors[];

extern struct json_tokener* json_tokener_new(void);
extern void json_tokener_free(struct json_tokener *tok);
extern void json_tokener_reset(struct json_tokener *tok);
extern struct json_object* json_tokener_parse(const char *str);
extern struct json_object* json_tokener_parse_verbose(const char *str, enum json_tokener_error *error);
extern struct json_object* json_tokener_parse_ex(struct json_tokener *tok,
						 const char *str, int len);









typedef system_state_t          mico_system_state_t;
typedef system_config_t         mico_Context_t;
typedef system_status_wlan_t    mico_status_wlan_t;




 



 




 







 
void mico_sdk_version( uint8_t *major, uint8_t *minor, uint8_t *revision );






 
void mico_app_info(char *p_info, int len_info);

 





 

 
typedef enum
{
    PARA_BOOT_TABLE_SECTION,
    PARA_MICO_DATA_SECTION,
    PARA_SYS_END_SECTION,
    PARA_APP_DATA_SECTION,
    PARA_END_SECTION
} para_section_t;







 
void* mico_system_context_init( uint32_t size_of_user_data );






 
mico_Context_t* mico_system_context_get( void );






 
void* mico_system_context_get_user_data( mico_Context_t* const in_context );







 
OSStatus mico_system_context_restore( mico_Context_t* const in_context );








 
void appRestoreDefault_callback(void * const user_config_data, uint32_t size);





 
OSStatus mico_system_context_update( mico_Context_t* const in_context );

OSStatus mico_system_para_read(void** info_ptr, int section, uint32_t offset, uint32_t size);
OSStatus mico_system_para_write(const void* info_ptr, int section, uint32_t offset, uint32_t size);
OSStatus mico_system_para_read_release( void* info_ptr );

 




 






 
OSStatus mico_ota_switch_to_new_fw(int ota_data_len, uint16_t ota_data_crc);

 




 

  
typedef enum{
  CONFIG_BY_NONE,              
  CONFIG_BY_EASYLINK_V2,       
  CONFIG_BY_EASYLINK_PLUS,         
  CONFIG_BY_EASYLINK_MINUS,           
  CONFIG_BY_AIRKISS,              
  CONFIG_BY_SOFT_AP,           
  CONFIG_BY_WAC,               
  CONFIG_BY_USER,              
} mico_config_source_t;





 
OSStatus mico_system_init( mico_Context_t* const in_context );





 
OSStatus mico_system_status_wlan( mico_status_wlan_t** status );





 
void mico_system_delegate_config_will_start( void );





 
void mico_system_delegate_config_will_stop( void );





 
void mico_system_delegate_soft_ap_will_start( void );








 
void mico_system_delegate_config_recv_ssid ( char *ssid, char *key );







 
OSStatus mico_system_delegate_config_recv_auth_data( char * userInfo );







 
void mico_system_delegate_config_success( mico_config_source_t source );

 








 




 
typedef struct _mico_system_monitor_t
{
    uint32_t last_update;               
    uint32_t longest_permitted_delay;   
} mico_system_monitor_t;






 
OSStatus mico_system_monitor_daemen_start( void );






 
OSStatus mico_system_monitor_register( mico_system_monitor_t* system_monitor, uint32_t initial_permitted_delay );






 
OSStatus mico_system_monitor_update ( mico_system_monitor_t* system_monitor, uint32_t permitted_delay );




 





 







 
OSStatus mico_system_power_perform( mico_Context_t* const in_context, mico_system_state_t new_state );
 

 




 
 

typedef notify_wlan_t WiFiEvent;

  
typedef enum{

  mico_notify_WIFI_SCAN_COMPLETED,           
  mico_notify_WIFI_STATUS_CHANGED,         
  mico_notify_WiFI_PARA_CHANGED,           
  mico_notify_DHCP_COMPLETED,              
  mico_notify_EASYLINK_WPS_COMPLETED,      
  mico_notify_EASYLINK_GET_EXTRA_DATA,     
  mico_notify_TCP_CLIENT_CONNECTED,        
  mico_notify_DNS_RESOLVE_COMPLETED,       
  mico_notify_SYS_WILL_POWER_OFF,          
  mico_notify_WIFI_CONNECT_FAILED,         
  mico_notify_WIFI_SCAN_ADV_COMPLETED,     
  mico_notify_WIFI_Fatal_ERROR,            
  mico_notify_Stack_Overflow_ERROR,        
 
} mico_notify_types_t;







 
OSStatus mico_system_notify_register( mico_notify_types_t notify_type, void* functionAddress, void* arg );






 
OSStatus mico_system_notify_remove( mico_notify_types_t notify_type, void *functionAddress );





 
OSStatus mico_system_notify_remove_all( mico_notify_types_t notify_type);


 





 








 
OSStatus config_server_start ( void );




 
OSStatus config_server_stop ( void );










 
void config_server_delegate_recv( const char *key, json_object *value, _Bool *need_reboot, mico_Context_t *in_context );









 
void config_server_delegate_report( json_object *config_cell_list, mico_Context_t *in_context );










 
OSStatus config_server_create_string_cell   (json_object* config_cell_list, char* const name, char* const content, char* const privilege, json_object* secectionArray);










 
OSStatus config_server_create_number_cell   (json_object* config_cell_list, char* const name, int content, char* const privilege, json_object* secectionArray);










 
OSStatus config_server_create_float_cell    (json_object* config_cell_list, char* const name, float content, char* const privilege, json_object* secectionArray);










 
OSStatus config_server_create_bool_cell     (json_object* config_cell_list, char* const name, boolean content, char* const privilege);







 
OSStatus config_server_create_sub_menu_cell (json_object* config_cell_list, char* const name, json_object* sub_menu_array);







 
OSStatus config_server_create_sector        (json_object* sub_menu_array, char* const name, json_object *config_cell_list);

 






 






 
int cli_init(void);

 







 



 
typedef struct _mdns_init_t
{
  char *service_name;      
  char *host_name;         
  char *instance_name;     
  char *txt_record;       
 
  uint16_t service_port;   
} mdns_init_t;







 
OSStatus mdns_add_record( mdns_init_t record, wlanInterfaceTypedef interface, uint32_t time_to_live );







 
void mdns_suspend_record( char *service_name, wlanInterfaceTypedef interface, _Bool will_remove );






 
void mdns_resume_record( char *service_name, wlanInterfaceTypedef interface );








 
void mdns_update_txt_record( char *service_name, wlanInterfaceTypedef interface, char *txt_record );

 




 




 
#pragma pack(1)

typedef struct
{
    char year[4];         
    char dash1;           
    char month[2];        
    char dash2;           
    char day[2];          
    char T;               
    char hour[2];         
    char colon1;          
    char minute[2];       
    char colon2;          
    char second[2];       
    char decimal;         
    char sub_second[6];   
    char Z;               
} iso8601_time_t;

#pragma pack()








 
OSStatus mico_time_get_time( mico_time_t* time );







 
OSStatus mico_time_set_time( const mico_time_t* time );









 
OSStatus mico_time_get_utc_time( mico_utc_time_t* utc_time );









 
OSStatus mico_time_get_utc_time_ms( mico_utc_time_ms_t* utc_time_ms );







 
OSStatus mico_time_set_utc_time_ms( const mico_utc_time_ms_t* utc_time_ms );










 
OSStatus mico_time_get_iso8601_time( iso8601_time_t* iso8601_time );









 
OSStatus mico_time_convert_utc_ms_to_iso8601( mico_utc_time_ms_t utc_time_ms, iso8601_time_t* iso8601_time );







 
void mico_nanosecond_delay( uint64_t delayus );




 
uint64_t mico_nanosecond_clock_value( void );




 
void mico_nanosecond_reset( void );




 
void mico_nanosecond_init( void );

 






 




 
void tftp_ota(void);

 

 

 































 


#pragma once





 




 



   




 




 

 

 
 










 
OSStatus MicoRandomNumberRead( void *inBuffer, int inByteCount );

 
 



































 


#pragma once





























 

#pragma once





 



 
  

   


 






















































































































































 

typedef enum
{
    MICO_SYS_LED, 
    MICO_RF_LED, 
    BOOT_SEL, 
    MFG_SEL, 
    EasyLink_BUTTON, 
    STDIO_UART_RX,  
    STDIO_UART_TX,  
    FLASH_PIN_SPI_CS,
    FLASH_PIN_SPI_CLK,
    FLASH_PIN_SPI_MOSI,
    FLASH_PIN_SPI_MISO,
    
    MICO_GPIO_2,
    MICO_GPIO_8,
    MICO_GPIO_9,
    MICO_GPIO_12,
    MICO_GPIO_14,
    MICO_GPIO_16,
    MICO_GPIO_17,
    MICO_GPIO_18,
    MICO_GPIO_19,
    MICO_GPIO_27,  
    MICO_GPIO_29,
    MICO_GPIO_30,
    MICO_GPIO_31,
    MICO_GPIO_33,
    MICO_GPIO_34,
    MICO_GPIO_35,
    MICO_GPIO_36,
    MICO_GPIO_37,
    MICO_GPIO_38,
    MICO_GPIO_MAX,  
    MICO_GPIO_NONE,
} mico_gpio_t;

typedef enum
{
  MICO_SPI_1,
  MICO_SPI_MAX,  
  MICO_SPI_NONE,
} mico_spi_t;

typedef enum
{
    MICO_I2C_1,
    MICO_I2C_MAX,  
    MICO_I2C_NONE,
} mico_i2c_t;

typedef enum
{
    MICO_PWM_MAX,  
    MICO_PWM_NONE,
} mico_pwm_t;

typedef enum
{
    MICO_ADC_1,
    MICO_ADC_2,
    MICO_ADC_MAX,  
    MICO_ADC_NONE,
} mico_adc_t;

typedef enum
{
    MICO_UART_1,
    MICO_UART_2,
    MICO_UART_MAX,  
    MICO_UART_NONE,
} mico_uart_t;

typedef enum
{
  MICO_FLASH_EMBEDDED,
  MICO_FLASH_SPI,
  MICO_FLASH_MAX,
  MICO_FLASH_NONE,
} mico_flash_t;

typedef enum
{
  MICO_PARTITION_FILESYS,
  MICO_PARTITION_USER_MAX
} mico_user_partition_t;



 



 




















 

















 





 



   



 



 



 





 











 
OSStatus MicoMFiAuthInitialize( mico_i2c_t i2c );











 
void MicoMFiAuthFinalize( void );















 
OSStatus MicoMFiAuthCreateSignature( const  void      *inDigestPtr,
                                            unsigned int     inDigestLen,
                                            uint8_t  **outSignaturePtr,
                                            unsigned int    *outSignatureLen );













 
OSStatus MicoMFiAuthCopyCertificate( uint8_t **outCertificatePtr, unsigned int *outCertificateLen );

 
 


















 

#pragma once






























  

#pragma once





 



 


 

  
 



 



 



 


 


 


 


 





 
 
  
 



 

 


 

 


 


 






 




 


 
typedef struct _pair_t {
  char             controllerName[64];
  uint8_t          controllerLTPK[32];
  int              permission;
} _pair_t;

typedef struct _pair_list_in_flash_t {
  _pair_t          pairInfo[16];
} pair_list_in_flash_t;

 


uint32_t HKPairInfoCount(void);

OSStatus HKPairInfoClear(void);

OSStatus HKPairInfoInsert(char controllerIdentifier[64], uint8_t controllerLTPK[32], _Bool admin);

OSStatus HKPairInfoFindByName(char controllerIdentifier[64], uint8_t foundControllerLTPK[32], _Bool *isAdmin );

OSStatus HKPairInfoFindByIndex(uint32_t index, char controllerIdentifier[64], uint8_t foundControllerLTPK[32], _Bool *isAdmin );

OSStatus HKPairInfoRemove(char controllerIdentifier[64]);

















 




 

typedef int32_t         HkStatus;



enum hk_ci_e{
  CI_ORHER = 1,  
  CI_BRIDGE,
  CI_FAN,
  CI_GARAGE_DOOR_OPENER,    
  CI_LIGHTBULB,
  CI_DOOR_LOCK,
  CI_OUTLET,
  CI_SWITCH,
  CI_THERMOSTAT,
  CI_RESERVED,
} ;
typedef uint8_t hk_ci_t;

typedef enum _valueType{
  ValueType_bool,
  ValueType_int,
  ValueType_float,
  ValueType_string,
  ValueType_date,
  ValueType_tlv8,
  ValueType_data,
  ValueType_array,
  ValueType_dict,
  ValueType_null,
} valueType;

typedef union {
    _Bool        boolValue;
    int         intValue;
    double      floatValue;
    char        *stringValue;
    char        *dateValue;
    json_object *array;
    json_object *object;
  } value_union;


struct _hapCharacteristic_t {
  char   *type;

  _Bool   hasStaticValue;
  valueType valueType;
  value_union value;

  _Bool   secureRead;
  _Bool   secureWrite;
  _Bool   hasEvents;

  _Bool   hasMinimumValue;
  union {
    int         intValue;
    float       floatValue;
  }      minimumValue;

  _Bool   hasMaximumValue;
  union {
    int         intValue;
    float       floatValue;
  }      maximumValue;

  _Bool   hasMinimumStep;
  union {
    int         intValue;
    float       floatValue;
  }      minimumStep;

  _Bool   hasMaxLength;
  int    maxLength;

  _Bool   hasMaxDataLength;
  int    maxDataLength;

  char   *description;

  char   *format;
  
  char   *unit;
};

struct _hapService_t {
  char    *type;
  uint32_t num_of_characteristics;
  struct  _hapCharacteristic_t *characteristic;
};

struct _hapAccessory_t {
  uint32_t num_of_services;
  struct _hapService_t  *services;
};

struct _hapProduct_t {
  uint32_t num_of_accessories;
  struct _hapAccessory_t  *accessories;
};

typedef struct _hapProduct_t hapProduct_t;

typedef OSStatus (*hk_key_storage_cb)(unsigned char *pk, unsigned char *sk, _Bool write);

 
typedef struct _hk_init_t {
  hk_ci_t             ci;
  char *              model;
  int                 config_number;
  mico_i2c_t          mfi_cp_port;
  hapProduct_t *      hap_product;
  uint8_t *           password;
  int                 password_len;
  uint8_t *           verifier;
  int                 verifier_len;
  uint8_t *           salt;
  int                 salt_len;
  hk_key_storage_cb   key_storage;
} hk_init_t;

void hk_server_lib_version( uint8_t *major, uint8_t *minor, uint8_t *revision );

OSStatus hk_server_start( hk_init_t init );

OSStatus hk_server_update_attri_db( hapProduct_t *hap_product, int config_number );

OSStatus hk_server_notify_by_bonjour( void );

 
HkStatus HKReadCharacteristicValue(int accessoryID, int serviceID, int characteristicID, value_union *value );

void HKWriteCharacteristicValue(int accessoryID, int serviceID, int characteristicID, value_union value, _Bool moreComing );

HkStatus HKReadCharacteristicStatus(int accessoryID, int serviceID, int characteristicID );

HkStatus HKExcuteUnpairedIdentityRoutine( void );























  




 


 

 
typedef struct _hk_server_status_t {
   
  uint32_t          statusNumber;
  _Bool              useMFiAuth;
  uint16_t          server_port;

} hk_server_status_t;

OSStatus hk_bonjour_start( int ci, int config_number, char *model );
OSStatus hk_bonjour_update_state_number( void );
OSStatus hk_bonjour_update_config_number( int config_number );
OSStatus hk_bonjour_update_status_flag( void );


































 





 




 











 

OSStatus SocketSend( int fd, const uint8_t *inBuf, unsigned int inBufLen );








 
void SocketClose(int* fd);








 
void SocketCloseForOSEvent(int* fd);









 
void SocketAccept(int *plocalTcpClientsPool, int maxClientsNum, int newFd);




 



 

































 


#include <stdarg.h>




 




 





 




 


void formatMACAddr(char *destAddr, char *srcAddr);











 
unsigned int str2hex(unsigned char *ibuf, unsigned char *obuf, unsigned int olen);








 
void Int2Str(uint8_t* str, int32_t intnum);










 
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum);








 
char *__strdup(const char *src);









 
char *__strdup_trans_dot(char *src);













 
int TextToHardwareAddress( const void *inText, unsigned int inTextSize, unsigned int inAddrSize, void *outAddr );











 
char* DataToCString( const uint8_t *inBuf, unsigned int inBufLen );









 
char* DataToHexString( const uint8_t *inBuf, unsigned int inBufLen );









 
char* DataToHexStringWithSpaces( const uint8_t *inBuf, unsigned int inBufLen );









 
char* DataToHexStringWithColons( const uint8_t *inBuf, unsigned int inBufLen );










 
int strnicmp_suffix( const void *inStr, unsigned int inMaxLen, const char *inSuffix );











 
int strnicmp( const char *inS1, const char *inS2, unsigned int inMax );










 
int strnicmpx( const void *inS1, unsigned int inN, const char *inS2 );










 
char * strnstr_suffix( const char *inStr, unsigned int inMaxLen, const char *inSuffix);










 
int VSNScanF( const void *inString, unsigned int inSize, const char *inFormat, va_list inArgs );










 
uint8_t unsigned_to_hex_string( uint32_t value, char* output, uint8_t min_length, uint8_t max_length );










 
uint8_t string_to_unsigned( const char* string, uint8_t str_length, uint32_t* value_out, uint8_t is_hex );











 
void *memmem(void *start, unsigned int s_len, void *find, unsigned int f_len);

uint8_t unsigned_to_decimal_string( uint32_t value, char* output, uint8_t min_length, uint8_t max_length );




 



 























  


































 

































 





 




 


typedef struct
{
    const char *    schemePtr;
    unsigned int          schemeLen;
    const char *    userPtr;
    unsigned int          userLen;
    const char *    passwordPtr;
    unsigned int          passwordLen;
    const char *    hostPtr;
    unsigned int          hostLen;
    const char *    pathPtr;
    unsigned int          pathLen;
    const char *    queryPtr;
    unsigned int          queryLen;
    const char *    fragmentPtr;
    unsigned int          fragmentLen;
    
    const char *    segmentPtr; 
    const char *    segmentEnd; 
    
} URLComponents;












 
int URLParseComponents( const char *inSrc, const char *inEnd, URLComponents *outComponents, const char **outSrc );









 
void PrintURL( URLComponents *inURL );




 



 




      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      






typedef struct _HTTPHeader_t
{
    char *              buf;                
    unsigned int              bufLen;             
    unsigned int              len;                
    char *              extraDataPtr;       
    char *              otaDataPtr;         
    unsigned int              extraDataLen;       

    const char *        methodPtr;          
    unsigned int              methodLen;          
    const char *        urlPtr;             
    unsigned int              urlLen;             
    URLComponents       url;                
    const char *        protocolPtr;        
    unsigned int              protocolLen;        
    int                 statusCode;         
    const char *        reasonPhrasePtr;    
    unsigned int              reasonPhraseLen;    

    uint8_t             channelID;          
    uint64_t            contentLength;      
    _Bool                persistent;         

    int                 firstErr;           

    _Bool                dataEndedbyClose;
    _Bool                chunkedData;        
    char *              chunkedDataBufferPtr;     
    unsigned int              chunkedDataBufferLen; 

    void *              userContext;
    _Bool                isCallbackSupported;
    OSStatus            (*onReceivedDataCallback) ( struct _HTTPHeader_t * , uint32_t, uint8_t *, unsigned int, void * ); 
    void                (*onClearCallback) ( struct _HTTPHeader_t * httpHeader, void * userContext );



} HTTPHeader_t;

typedef OSStatus (*onReceivedDataCallback) ( struct _HTTPHeader_t * httpHeader, uint32_t pos, uint8_t * data, unsigned int len, void * userContext );

typedef void (*onClearCallback) ( struct _HTTPHeader_t * httpHeader, void * userContext );

void PrintHTTPHeader( HTTPHeader_t *inHeader );

_Bool findHeader ( HTTPHeader_t *inHeader,  char **  outHeaderEnd);

int HTTPScanFHeaderValue( const char *inHeaderPtr, unsigned int inHeaderLen, const char *inName, const char *inFormat, ... );

int findCRLF( const char *inDataPtr , unsigned int inDataLen, char **  nextDataPtr );

int findChunkedDataLength( const char *inChunkPtr , unsigned int inChunkLen, char **  chunkedDataPtr, const char *inFormat, ... );

int SocketReadHTTPHeader( int inSock, HTTPHeader_t *inHeader );

int SocketReadHTTPBody( int inSock, HTTPHeader_t *inHeader );

int SocketReadHTTPSHeader( mico_ssl_t ssl, HTTPHeader_t *inHeader );

int SocketReadHTTPSBody( mico_ssl_t ssl, HTTPHeader_t *inHeader );

int HTTPHeaderParse( HTTPHeader_t *ioHeader );

int HTTPHeaderMatchMethod( HTTPHeader_t *inHeader, const char *method );

int HTTPHeaderMatchURL( HTTPHeader_t *inHeader, const char *url );

char* HTTPHeaderMatchPartialURL( HTTPHeader_t *inHeader, const char *url );


int HTTPGetHeaderField( const char *inHeaderPtr, 
                             unsigned int     inHeaderLen, 
                             const char *inName, 
                             const char **outNamePtr, 
                             unsigned int     *outNameLen, 
                             const char **outValuePtr, 
                             unsigned int     *outValueLen, 
                             const char **outNext );

HTTPHeader_t * HTTPHeaderCreate( unsigned int bufLen );

HTTPHeader_t * HTTPHeaderCreateWithCallback( unsigned int bufLen, onReceivedDataCallback , onClearCallback , void * context );

void HTTPHeaderClear( HTTPHeader_t *inHeader );

void HTTPHeaderDestory( HTTPHeader_t **inHeader );

int CreateSimpleHTTPOKMessage( uint8_t **outMessage, unsigned int *outMessageSize );

OSStatus CreateSimpleHTTPMessage      ( const char *contentType, uint8_t *inData, unsigned int inDataLen, uint8_t **outMessage, unsigned int *outMessageSize );
OSStatus CreateSimpleHTTPMessageNoCopy( const char *contentType, unsigned int inDataLen, uint8_t **outMessage, unsigned int *outMessageSize );

OSStatus CreateHTTPRespondMessageNoCopy( int status, const char *contentType, unsigned int inDataLen, uint8_t **outMessage, unsigned int *outMessageSize );


OSStatus CreateHTTPMessage( const char *methold, const char *url, const char *contentType, uint8_t *inData, unsigned int inDataLen, uint8_t **outMessage, unsigned int *outMessageSize );

OSStatus CreateHTTPMessageWithHost( const char *methold, const char *url,
                           const char* host, uint16_t port, 
                           const char *contentType, 
                           uint8_t *inData, unsigned int inDataLen, 
                           uint8_t **outMessage, unsigned int *outMessageSize );



typedef struct _security_session_t {
  _Bool          established;
  char          controllerIdentifier[64];
  uint8_t       OutputKey[32];
  uint8_t       InputKey[32];
  uint64_t      recvedDataLen;
  uint8_t*      recvedDataBuffer;
  uint64_t      outputSeqNo;
  uint64_t      inputSeqNo;
} security_session_t;

security_session_t *HKSNewSecuritySession(void);

int HKSecureSocketSend( int sockfd, void *buf, unsigned int len, security_session_t *session);

int HKSecureRead(security_session_t *session, int sockfd, void *buf, unsigned int len);

int HKSocketReadHTTPHeader( int inSock, HTTPHeader_t *inHeader, security_session_t *session );

int HKSocketReadHTTPBody  ( int inSock, HTTPHeader_t *inHeader, security_session_t *session );

OSStatus HKSendResponseMessage(int sockfd, int status, uint8_t *payload, int payloadLen, security_session_t *session );

OSStatus HKSendNotifyMessage( int sockfd, uint8_t *payload, int payloadLen, security_session_t *session );

































 


























 



struct SRPVerifier;


typedef enum
{
    SRP_NG_1024,
    SRP_NG_2048,
    SRP_NG_3072,
    SRP_NG_4096,
    SRP_NG_8192,
    SRP_NG_CUSTOM
} SRP_NGType;

typedef enum 
{
    SRP_SHA1, 
    SRP_SHA224, 
    SRP_SHA256,
    SRP_SHA384, 
    SRP_SHA512
} SRP_HashAlgorithm;

 
typedef struct _srp_server_t {
  SRP_HashAlgorithm alg;
  _Bool              authenticated;
  SRP_NGType        ng_type;
  const char        * n_hex;
  const char        * g_hex;
  char              *username;
  unsigned char     *bytes_s;
  int               len_s;
  unsigned char     *bytes_B;
  int               len_B;
  unsigned char     *bytes_b;
  int               len_b;  
  unsigned char     *bytes_v;
  int               len_v;
  unsigned char     *session_key;
  int               len_session_key;
  unsigned char     *bytes_M;
  int               len_M;
  unsigned char     *bytes_AMK;
  int               len_AMK;
} srp_server_t;









 
void srp_create_salted_verification_key( SRP_HashAlgorithm alg, 
                                         SRP_NGType ng_type, const char * username,
                                         const unsigned char * password, int len_password,
                                         const unsigned char ** bytes_s, int * len_s, 
                                         const unsigned char ** bytes_v, int * len_v,
                                         const unsigned char ** bytes_B, int * len_B,
                                         const unsigned char ** bytes_b, int * len_b,
                                         const char * n_hex, const char * g_hex );

srp_server_t* srp_server_setup( SRP_HashAlgorithm alg, 
                                SRP_NGType ng_type, const char * username,
                                const unsigned char * password, int len_password, 
                                const unsigned char * verifier, int len_verifier,
                                const unsigned char * salt, int len_salt, 
                                const char * n_hex, const char * g_hex );

OSStatus srp_server_generate_session_key( srp_server_t* server, const unsigned char * bytes_A, int len_A );

void srp_server_verify_session( srp_server_t* server, const unsigned char * user_M, const unsigned char ** bytes_HAMK );

void srp_server_delete( srp_server_t** server);








 
struct SRPVerifier *  srp_verifier_new( SRP_HashAlgorithm alg, SRP_NGType ng_type, const char * username,
                                        const unsigned char * bytes_s, int len_s, 
                                        const unsigned char * bytes_v, int len_v,
                                        const unsigned char * bytes_A, int len_A,
                                        const unsigned char * bytes_B, int len_B,
                                        const unsigned char * bytes_b, int len_b,
                                        const char * n_hex, const char * g_hex );


void                  srp_verifier_delete( struct SRPVerifier * ver );


int                   srp_verifier_is_authenticated( struct SRPVerifier * ver );


const char *          srp_verifier_get_username( struct SRPVerifier * ver );

 
const unsigned char * srp_verifier_get_session_key( struct SRPVerifier * ver, int * key_length );


int                   srp_verifier_get_session_key_length( struct SRPVerifier * ver );


 
void                  srp_verifier_verify_session( struct SRPVerifier * ver,
                                                   const unsigned char * user_M, 
                                                   const unsigned char ** bytes_HAMK );




 
typedef struct _pairInfo_t {
  char              *SRPUser;
  srp_server_t      *SRPServer;
  uint8_t           *SRPControllerPublicKey;
  int           SRPControllerPublicKeyLen;
  uint8_t           *SRPControllerProof;
  int           SRPControllerProofLen;
  uint8_t           *HKDF_Key;
  _Bool              pairListFull;
} pairInfo_t;


 
typedef struct _pairVerifyInfo_t {
  _Bool                      verifySuccess;
  int                       haPairVerifyState;
  uint8_t                   *pControllerLTPK;
  char                      *pControllerIdentifier;
  uint8_t                   *pControllerCurve25519PK;
  uint8_t                   *pAccessoryCurve25519PK;
  uint8_t                   *pAccessoryCurve25519SK;
  uint8_t                   *pSharedSecret;
  uint8_t                   *pHKDFKey;
  uint8_t                   *A2CKey;
  uint8_t                   *C2AKey;
} pairVerifyInfo_t;




OSStatus HKPairEngineInit( mico_i2c_t mfi_cp_port, hk_key_storage_cb key_storage_fun );

void HKSetPassword (uint8_t * password, unsigned int passwordLen);

void HKSetVerifier (uint8_t * verifier, unsigned int verifierLen, uint8_t * salt, unsigned int saltLen );

 

void HKCleanPairSetupInfo(pairInfo_t **info);

OSStatus HKPairSetupEngine( int inFd, HTTPHeader_t* inHeader, pairInfo_t** inInfo );

_Bool HKPairSetupEngineIsRunning( void );

 

pairVerifyInfo_t* HKCreatePairVerifyInfo(void);

void HKCleanPairVerifyInfo(pairVerifyInfo_t **verifyInfo);

OSStatus HKPairVerifyEngine( int inFd, HTTPHeader_t* inHeader, pairVerifyInfo_t* inInfo );

 

OSStatus HKPairAddRemoveEngine( int inFd, HTTPHeader_t* inHeader, security_session_t *session );








static struct _hapProduct_t *hapObjects = 0;

OSStatus system_easylink_wac_stop( void );

typedef struct _HK_Context_t {
  pairInfo_t          *pairInfo;
  pairVerifyInfo_t    *pairVerifyInfo;
  security_session_t  *session;
} HK_Context_t;

typedef struct _HK_Char_ID_t {
  uint32_t          aid;
  uint32_t          iid;
  uint32_t          serviceID;
  uint32_t          characteristicID;
} HK_Char_ID_t;

typedef struct _HK_Notify{
  uint32_t aid;
  uint32_t iid;
  value_union value;
  struct _HK_Notify *next;
} HK_Notify_t;

hk_server_status_t hk_status;

static void homeKitClient_thread(uint32_t inFd);
static OSStatus HKhandleIncomeingMessage(int sockfd, HTTPHeader_t *httpHeader, HK_Notify_t** notifyList, HK_Context_t *inHkContext );
static HkStatus HKCreateHAPAttriDataBase( struct _hapProduct_t *inHapObject,  HK_Notify_t* notifyList, json_object **OutHapObjectJson );

void homeKitListener_thread( uint32_t arg );

void hk_server_lib_version( uint8_t *major, uint8_t *minor, uint8_t *revision )
{
  if( major ) *major = (1);
  if( minor ) *minor = (6);
  if( revision ) *revision = (1);
}

OSStatus hk_server_start( hk_init_t init )
{
  OSStatus err = 0;
  uint16_t random;
  hk_status.statusNumber = 0x01;  
  MicoRandomNumberRead( &random, 2 );
  hk_status.server_port = 49000 + random%2000;
  hapObjects = init.hap_product;

     
  if( MicoMFiAuthInitialize( init.mfi_cp_port ) == 0 )
    hk_status.useMFiAuth = 1;
  else
    hk_status.useMFiAuth = 0;

   
  err = hk_bonjour_start( init.ci, init.config_number, init.model );
  do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );
  
   
  do { if( ! !(!(init . password_len||((init . verifier_len)&&(init . salt_len)))) ) { ; { err = -6754; } goto exit; } } while( 1==0 );

  if( init.password_len ){
    HKSetPassword (init.password, init.password_len);
  }else{
    HKSetVerifier(init.verifier, init.verifier_len, init.salt, init.salt_len);
  }
  
   
  err = HKPairEngineInit( init.mfi_cp_port, init.key_storage);
  do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );

  err = mico_rtos_create_thread(0, (7), "HomeKit Server", homeKitListener_thread, 0x500, 0 );
  do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );

exit:
  return err;
}

OSStatus hk_server_update_attri_db( hapProduct_t *hap_product, int config_number )
{
  hapObjects = hap_product;
  hk_bonjour_update_config_number( config_number );
  return 0;
}


void homeKitListener_thread( uint32_t arg )
{
  ;
  ( (void)(arg) );
  OSStatus err = -6700;
  int j, opt;
  struct sockaddr_in addr;
  uint32_t sockaddr_t_size;
  fd_set readfds;
  char ip_address[16];
  
  int homeKitlistener_fd = -1;

    
  homeKitlistener_fd = socket( 2, 1, 6 );
  do { if( ! !(!(( ( homeKitlistener_fd ) >= 0 ))) ) { ; { err = -6729; } goto exit; } } while( 1==0 );

  addr.sin_family = 2;
  addr.sin_addr.s_addr = ( (uint32_t)( ( ( ( (uint32_t)(((uint32_t)0x00000000UL)) ) << 24 ) & 0xFF000000U ) | ( ( ( (uint32_t)(((uint32_t)0x00000000UL)) ) << 8 ) & 0x00FF0000U ) | ( ( ( (uint32_t)(((uint32_t)0x00000000UL)) ) >> 8 ) & 0x0000FF00U ) | ( ( ( (uint32_t)(((uint32_t)0x00000000UL)) ) >> 24 ) & 0x000000FFU ) ) );
  addr.sin_port = ( (uint16_t)( ( ( ( (uint16_t)(hk_status . server_port) ) << 8 ) & 0xFF00 ) | ( ( ( (uint16_t)(hk_status . server_port) ) >> 8 ) & 0x00FF ) ) );
  err = bind(homeKitlistener_fd, (struct sockaddr *)&addr, sizeof(addr));
  do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );

  err = listen(homeKitlistener_fd, 8);
  do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );

   
  opt = (5);
  setsockopt(homeKitlistener_fd, 6, TCP_MAX_CONN_NUM, (void *)&opt, sizeof(opt));

  ;
  
  while(1){
    memset(&readfds, 0, sizeof(*(&readfds)));
    ((&readfds)->fds_bits[(homeKitlistener_fd)/(sizeof(unsigned long) * 8)] |= ((unsigned long)1 << ((homeKitlistener_fd) % (sizeof(unsigned long) * 8))));
    select( homeKitlistener_fd + 1, &readfds, 0, 0, 0 );

     
    if(((&readfds)->fds_bits[(homeKitlistener_fd)/(sizeof(unsigned long) * 8)] & ((unsigned long)1 << ((homeKitlistener_fd) % (sizeof(unsigned long) * 8))))){
      sockaddr_t_size = sizeof(struct sockaddr);
      j = accept(homeKitlistener_fd, (struct sockaddr *)&addr, &sockaddr_t_size);
      if ( ( ( j ) >= 0 ) )  {
        strcpy(ip_address,inet_ntoa( addr.sin_addr ));
        ;
        ;
        err = mico_rtos_create_thread(0, (7), "HomeKit Client", homeKitClient_thread, 0x1000, (mico_thread_arg_t)j);
        if(err != 0){
          ;
          SocketClose(&j);
        }
      }
    }
   }

exit:
    ;
    mico_rtos_delete_thread(0);
    return;
}

void FindCharacteristicByIID(struct _hapProduct_t *inHapObject, uint32_t aid, uint32_t iid, uint32_t *serviceID, uint32_t *characteristicID)
{
  uint32_t serviceIndex, characteristicIndex, tmpIID;
  tmpIID = 1;
  *serviceID = 0;
  *characteristicID = 0;

  for(serviceIndex = 0; serviceIndex < inHapObject->accessories[aid-1].num_of_services; serviceIndex++){
    if(inHapObject->accessories[aid-1].services[serviceIndex].type == 0 || tmpIID > iid)
      return;
    if(tmpIID == iid){
      *serviceID = serviceIndex + 1;
      return;
    }
    tmpIID ++;

    for(characteristicIndex = 0; characteristicIndex < inHapObject->accessories[aid-1].services[serviceIndex].num_of_characteristics; characteristicIndex++){
      if(inHapObject->accessories[aid-1].services[serviceIndex].characteristic[characteristicIndex].type == 0 )
        break;
      if(tmpIID > iid)
        return;
      if(tmpIID == iid){
        *serviceID = serviceIndex + 1;
        *characteristicID = characteristicIndex + 1;;
        return;
      }
      tmpIID ++;
    }
  }
}

OSStatus HKNotificationAdd( uint32_t aid, uint32_t iid, value_union *value, HK_Notify_t** notifyList )
{
  OSStatus err = 0;
  HK_Notify_t *notify = (HK_Notify_t *)malloc(sizeof(HK_Notify_t));
  HK_Notify_t *tmp = * notifyList;
  do { if( ! !(!(notify)) ) { ; { err = -6728; } goto exit; } } while( 1==0 );
  notify->aid = aid;
  notify->iid = iid;
  memcpy(&notify->value, value, sizeof(value_union));
  notify->next = 0;
  if(* notifyList == 0){
    * notifyList = notify;
  }else{
    if(tmp->aid == aid && tmp->iid == iid){
      free(notify);
      memcpy(&tmp->value, value, sizeof(value_union));
      return 0;   
    }
    while(tmp->next!=0){
      tmp = tmp->next;
      if(tmp->aid == aid && tmp->iid == iid){
        memcpy(&tmp->value, value, sizeof(value_union));
        free(notify);
        return 0;   
      }
    }
    tmp->next = notify;
  }
exit:
  return err;
}

OSStatus HKNotificationRemove( uint32_t aid, uint32_t iid, HK_Notify_t** notifyList )
{
  OSStatus err = 0;
  HK_Notify_t *temp = *notifyList;
  HK_Notify_t *temp2 = temp;
  do { if( ! !(!(temp)) ) { { err = -6726; } goto exit; } } while( 1==0 );
  do{
    if(temp->aid == aid && temp->iid == iid){
      if(temp == *notifyList){  
        * notifyList = temp->next;
        free(temp);
      }else{
        temp2->next = temp->next;
        free(temp);
      }
       break;
    }
    do { if( ! !(!(temp->next!=0)) ) { ; { err = -6727; } goto exit; } } while( 1==0 );
    temp2 = temp;
    temp = temp->next;
  }while(temp!=0);

exit:
  return err;
}

OSStatus HKNotifyGetNext( 
        HK_Notify_t *   notifyList, 
        uint32_t *      outAID, 
        uint32_t *      outIID, 
        value_union **   value, 
        HK_Notify_t **  outNextNotifyList )
{
  if(notifyList == 0) return -6727;
  *outAID = notifyList->aid;
  *outIID = notifyList->iid;
  *value = &notifyList->value;
  *outNextNotifyList = notifyList->next;
  return 0;
}

OSStatus HKNotificationFind( uint32_t aid, uint32_t iid, HK_Notify_t* notifyList )
{
  OSStatus err = -6727;
  uint32_t _aid, _iid;
  value_union *_value;
  HK_Notify_t* _notifyList = notifyList;

  if(notifyList==0) return -6745;

  while(HKNotifyGetNext( _notifyList, &_aid, &_iid, &_value, &_notifyList) == 0){
    if(_aid == aid && _iid == iid)
      return 0;
  }
  return err;
}

OSStatus HKNotificationUpdate( uint32_t aid, uint32_t iid, value_union *value, HK_Notify_t* notifyList )
{
  OSStatus err = -6727;
  uint32_t _aid, _iid;
  value_union *_value = 0;
  HK_Notify_t* _notifyList = notifyList;

  if(notifyList==0) return -6745;

  while(HKNotifyGetNext( _notifyList, &_aid, &_iid, &_value, &_notifyList) == 0){
    if(_aid == aid && _iid == iid){
      memcpy(_value, value, sizeof(value_union));
      return 0;
    }
  }
  return err;
}

OSStatus HKNotificationClean( HK_Notify_t** notifyList )
{
  HK_Notify_t* temp = *notifyList;
  HK_Notify_t* temp2;
  if(*notifyList == 0) return 0;
  do{
    temp2 = temp->next;
    free(temp);
    temp = temp2;
  }while(temp!=0);    

  * notifyList = 0;
  return 0;
}

void homeKitClient_thread( uint32_t inFd)
{
  ;
  OSStatus err;
  int clientFd = inFd, opt;
  struct timeval t;
  HTTPHeader_t *httpHeader = 0;
  int selectResult;
  fd_set      readfds;
  HK_Context_t hkContext;
  uint32_t aid, iid, serviceID, characteristicID;
  value_union *value, newValue;
  struct _hapCharacteristic_t pCharacteristic;
  json_object *outEventJsonObject = 0, *outCharacteristics, *outCharacteristic;
  const char *buffer = 0;
  HK_Notify_t *notifyList = 0, *temp;

   
  opt = 1;
  setsockopt(clientFd, 0xfff, SO_KEEPALIVE, (void *)&opt, sizeof(opt));
  opt = 1200;
  setsockopt(clientFd, 6, TCP_KEEPIDLE, (void *)&opt, sizeof(opt)); 

  memset(&hkContext, 0x0, sizeof(HK_Context_t));
  hkContext.session = HKSNewSecuritySession();
  do { if( ! !(!(hkContext . session)) ) { ; { err = -6728; } goto exit; } } while( 1==0 );

  
  httpHeader = HTTPHeaderCreate( 256 );
  do { if( ! !(!(httpHeader)) ) { ; { err = -6728; } goto exit; } } while( 1==0 );

  t.tv_sec = 0;
  t.tv_usec = 500000;  
  ;

  while(1){
    if(hkContext.session->established == 1 && hkContext.session->recvedDataLen > 0){
       err = HKhandleIncomeingMessage( clientFd, httpHeader, &notifyList, &hkContext );
    }else{
      memset(&readfds, 0, sizeof(*(&readfds)));
      ((&readfds)->fds_bits[(clientFd)/(sizeof(unsigned long) * 8)] |= ((unsigned long)1 << ((clientFd) % (sizeof(unsigned long) * 8))));
      selectResult = select(clientFd + 1, &readfds, 0, 0, &t);
      do { if( ! !(!(selectResult >= 0)) ) { ; goto exit; } } while( 1==0 );
      if(((&readfds)->fds_bits[(clientFd)/(sizeof(unsigned long) * 8)] & ((unsigned long)1 << ((clientFd) % (sizeof(unsigned long) * 8))))){
        err = HKhandleIncomeingMessage(clientFd, httpHeader, &notifyList, &hkContext );
        do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );
      }

      if(hkContext.session->established == 0) 
        continue;

      outEventJsonObject = json_object_new_object();
      do { if( ! !(!(outEventJsonObject)) ) { ; { err = -6728; } goto exit; } } while( 1==0 );
      outCharacteristics = json_object_new_array();
      do { if( ! !(!(outCharacteristics)) ) { ; { err = -6728; } goto exit; } } while( 1==0 );
      json_object_object_add( outEventJsonObject, "characteristics", outCharacteristics);
      temp = notifyList;

      while(HKNotifyGetNext( temp, &aid, &iid, &value, &temp) == 0){
        FindCharacteristicByIID(hapObjects, aid, iid, &serviceID, &characteristicID);   
        if(HKReadCharacteristicValue(aid, serviceID, characteristicID, &newValue)==0){
          pCharacteristic = ((hapObjects->accessories[aid-1]).services[serviceID-1]).characteristic[characteristicID-1];

          switch(pCharacteristic.valueType){
            case ValueType_bool:
              if( value->boolValue != newValue.boolValue ){
                outCharacteristic = json_object_new_object();
                json_object_object_add( outCharacteristic, "aid", json_object_new_int(aid));
                json_object_object_add( outCharacteristic, "iid", json_object_new_int(iid));
                json_object_object_add( outCharacteristic, "value", json_object_new_boolean(newValue.boolValue));
                json_object_array_add( outCharacteristics, outCharacteristic );
                HKNotificationAdd( aid, iid, &newValue, &notifyList );
              }
              break;
            case ValueType_int:
              if( value->intValue != newValue.intValue ){
                outCharacteristic = json_object_new_object();
                json_object_object_add( outCharacteristic, "aid", json_object_new_int(aid));
                json_object_object_add( outCharacteristic, "iid", json_object_new_int(iid));
                json_object_object_add( outCharacteristic, "value", json_object_new_int(newValue.intValue));
                json_object_array_add( outCharacteristics, outCharacteristic );
                HKNotificationAdd( aid, iid, &newValue, &notifyList );
              }
              break;
            case ValueType_float:
              if( value->floatValue != newValue.floatValue ){
                outCharacteristic = json_object_new_object();
                json_object_object_add( outCharacteristic, "aid", json_object_new_int(aid));
                json_object_object_add( outCharacteristic, "iid", json_object_new_int(iid));
                json_object_object_add( outCharacteristic, "value", json_object_new_double(newValue.floatValue));
                json_object_array_add( outCharacteristics, outCharacteristic );
                HKNotificationAdd( aid, iid, &newValue, &notifyList );
              }
              break;
            case ValueType_string:
              if( !strcmp(value->stringValue, newValue.stringValue) ){
                outCharacteristic = json_object_new_object();
                json_object_object_add( outCharacteristic, "aid", json_object_new_int(aid));
                json_object_object_add( outCharacteristic, "iid", json_object_new_int(iid));
                json_object_object_add( outCharacteristic, "value", json_object_new_string(newValue.stringValue));
                json_object_array_add( outCharacteristics, outCharacteristic );
                HKNotificationAdd( aid, iid, &newValue, &notifyList );
              }
              break;
            case ValueType_date:
              if( !strcmp(value->dateValue, newValue.dateValue) ){
                outCharacteristic = json_object_new_object();
                json_object_object_add( outCharacteristic, "aid", json_object_new_int(aid));
                json_object_object_add( outCharacteristic, "iid", json_object_new_int(iid));
                json_object_object_add( outCharacteristic, "value", json_object_new_string(newValue.dateValue));
                json_object_array_add( outCharacteristics, outCharacteristic );
                HKNotificationAdd( aid, iid, &newValue, &notifyList );
              }
              break;
            default:
              break;
          }
        }
      }
            
      if(json_object_array_length(outCharacteristics)){
        buffer = json_object_to_json_string(outEventJsonObject);
        ;  
        err = HKSendNotifyMessage( clientFd, (uint8_t *)buffer, strlen(buffer), hkContext.session );
        do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );

      }
      
      json_object_put(outEventJsonObject);
      outEventJsonObject = 0;
          
          
    }
  }

exit:
  SocketClose(&clientFd);
  HTTPHeaderDestory( &httpHeader );
  HKNotificationClean( &notifyList );
  if(outEventJsonObject) json_object_put(outEventJsonObject);
  HKCleanPairSetupInfo(&hkContext.pairInfo);
  HKCleanPairVerifyInfo(&hkContext.pairVerifyInfo);
  free(hkContext.session);
  ;
  mico_rtos_delete_thread(0);
  return;
}





static HkStatus HKCreateHAPAttriDataBase( struct _hapProduct_t *inHapObject, HK_Notify_t* notifyList, json_object **OutHapObjectJson)
{
  HkStatus err = 0;
  uint32_t accessoryIndex, serviceIndex, characteristicIndex;
  uint32_t aid = 1;
  uint32_t iid = 1;
  struct _hapCharacteristic_t             pCharacteristic;
  value_union value;

  json_object *hapJsonObject, *accessories, *accessory, *services, *service, *characteristics, *characteristic, *properties;

  hapJsonObject = json_object_new_object();
  accessories = json_object_new_array();
  json_object_object_add( hapJsonObject, "accessories", accessories ); 

  for(accessoryIndex = 0; accessoryIndex < inHapObject->num_of_accessories; accessoryIndex++){

    accessory = json_object_new_object();
    json_object_array_add (accessories, accessory);

    json_object_object_add( accessory, "aid", json_object_new_int(aid++) );   
    services = json_object_new_array();
    json_object_object_add( accessory, "services", services);

    for(serviceIndex = 0, iid = 1; serviceIndex < inHapObject->accessories[accessoryIndex].num_of_services; serviceIndex++){
      if(inHapObject->accessories[accessoryIndex].services[serviceIndex].type == 0)
        break;
      service = json_object_new_object();

      json_object_object_add( service, "type", json_object_new_string(inHapObject->accessories[accessoryIndex].services[serviceIndex].type));
      json_object_object_add( service, "iid",  json_object_new_int(iid++));

      characteristics = json_object_new_array();

      json_object_object_add( service, "characteristics",  characteristics);

      for(characteristicIndex = 0; characteristicIndex < inHapObject->accessories[accessoryIndex].services[serviceIndex].num_of_characteristics; characteristicIndex++){
        pCharacteristic = inHapObject->accessories[accessoryIndex].services[serviceIndex].characteristic[characteristicIndex];
        if(pCharacteristic.type){
          characteristic = json_object_new_object();
          json_object_array_add( characteristics, characteristic ); 
           
          json_object_object_add( characteristic, "type", json_object_new_string(pCharacteristic.type));

           
          json_object_object_add( characteristic, "iid", json_object_new_int(iid++));

          HKReadCharacteristicValue(accessoryIndex+1, serviceIndex+1, characteristicIndex+1, &value);
           
          if( pCharacteristic.secureRead == 1 ){
            if(pCharacteristic.hasStaticValue)
              value = pCharacteristic.value;

            switch(pCharacteristic.valueType){
              case ValueType_bool:
                json_object_object_add( characteristic, "value", json_object_new_boolean(value.boolValue));
                break;
              case ValueType_int:
                json_object_object_add( characteristic, "value", json_object_new_int(value.intValue));
                break;
              case ValueType_float:
                json_object_object_add( characteristic, "value", json_object_new_double(value.floatValue));
                break;
              case ValueType_string:
                json_object_object_add( characteristic, "value", json_object_new_string(value.stringValue));
                break;
              case ValueType_date:
                json_object_object_add( characteristic, "value", json_object_new_string(value.dateValue));
                break;
              case ValueType_null:
                json_object_object_add( characteristic, "value", 0);
                break;
              default:
                break;
            }
          }

          properties = json_object_new_array();
          if(pCharacteristic.secureRead)
            json_object_array_add( properties, json_object_new_string("pr") );
          if(pCharacteristic.secureWrite)
            json_object_array_add( properties, json_object_new_string("pw") );
          

          if(pCharacteristic.hasEvents){
            json_object_array_add( properties, json_object_new_string("ev") );
            if(HKNotificationFind(aid, iid, notifyList)==0)
              json_object_object_add( characteristic, "ev", json_object_new_boolean(1));
            else
              json_object_object_add( characteristic, "ev", json_object_new_boolean(0));
          }

          json_object_object_add( characteristic, "perms", properties);

          if(pCharacteristic.hasMinimumValue){
            switch(pCharacteristic.valueType){
              case ValueType_int:
                json_object_object_add( characteristic, "minValue",  json_object_new_int(pCharacteristic.minimumValue.intValue) );
                break;
              case ValueType_float:
                json_object_object_add( characteristic, "minValue",  json_object_new_double(pCharacteristic.minimumValue.floatValue) );
                break;
              default:
                break;
            }
          }

          if(pCharacteristic.hasMaximumValue){
            switch(pCharacteristic.valueType){
              case ValueType_int:
                json_object_object_add( characteristic, "maxValue",  json_object_new_int(pCharacteristic.maximumValue.intValue) );
                break;
              case ValueType_float:
                json_object_object_add( characteristic, "maxValue",  json_object_new_double(pCharacteristic.maximumValue.floatValue) );
                break;
              default:
                break;
            }
          }

          if(pCharacteristic.hasMinimumStep){
            switch(pCharacteristic.valueType){
              case ValueType_int:
                json_object_object_add( characteristic, "minStep",  json_object_new_int(pCharacteristic.minimumStep.intValue) );
                break;
              case ValueType_float:
                json_object_object_add( characteristic, "minStep",  json_object_new_double(pCharacteristic.minimumStep.floatValue) );
                break;
              default:
                break;
            }
          }
               
          if(pCharacteristic.hasMaxLength)
            json_object_object_add( characteristic, "maxLen",     json_object_new_int(pCharacteristic.maxLength));

          if(pCharacteristic.hasMaxDataLength)
            json_object_object_add( characteristic, "maxDataLen",     json_object_new_int(pCharacteristic.maxDataLength));

          if(pCharacteristic.description)
            json_object_object_add( characteristic, "description", json_object_new_string(pCharacteristic.description));

          if(pCharacteristic.format)
            json_object_object_add( characteristic, "format", json_object_new_string(pCharacteristic.format));

          if(pCharacteristic.unit)
            json_object_object_add( characteristic, "unit", json_object_new_string(pCharacteristic.unit));
        }
      }
      
      json_object_array_add( services, service ); 
    }    
  }
  
  *OutHapObjectJson = hapJsonObject;

  return err;

}



OSStatus IDGetNext( 
        const uint8_t *     inSrc, 
        const uint8_t *     inEnd, 
        uint32_t *          outAID, 
        uint32_t *          outIID, 
        const uint8_t **    outNext )
{
  char tmp;
  if(inSrc>=inEnd) return -6727;
  uint8_t *endPoint = memchr((const char *)inSrc, ',', inEnd - inSrc);
  if(!endPoint) endPoint = (uint8_t *)inEnd;
  tmp = *endPoint;
  *endPoint = 0x0;
  sscanf((const char *)inSrc, "%ld.%ld", outAID, outIID);
  *endPoint = tmp;
  *outNext = endPoint + 1;
  return 0;
}



HkStatus _HKCreateReadResponsePerCharacteristic(struct _hapProduct_t *inHapObject, HK_Char_ID_t id, 
                                                _Bool needMeta, _Bool needperms, _Bool needType, _Bool needEv, HK_Notify_t* notifyList,
                                                json_object *inHapReadRespondJson)
{
  HkStatus hkErr = 0;
  value_union value;
  static json_object *characteristic;
  struct _hapCharacteristic_t pCharacteristic = ((inHapObject->accessories[id.aid-1]).services[id.serviceID-1]).characteristic[id.characteristicID-1];
  
  characteristic = json_object_new_object();
  json_object_array_add(inHapReadRespondJson, characteristic);
  json_object_object_add( characteristic, "aid", json_object_new_int(id.aid));
  json_object_object_add( characteristic, "iid", json_object_new_int(id.iid));

  if(pCharacteristic.secureRead == 0){
    hkErr = -70405;
  }else if(id.serviceID == 0)
    hkErr = -70409;
  else{
    if(pCharacteristic.hasStaticValue)
      value = pCharacteristic.value;
    else
      hkErr = HKReadCharacteristicValue(id.aid, id.serviceID, id.characteristicID, &value);    
  }

  json_object_object_add( characteristic, "status", json_object_new_int(hkErr)); 

  if(hkErr == 0){
    switch(pCharacteristic.valueType ){
      case ValueType_bool:
        json_object_object_add( characteristic, "value", json_object_new_boolean(value.boolValue));
        break;
      case ValueType_int:
        json_object_object_add( characteristic, "value", json_object_new_int(value.intValue));
        break;
      case ValueType_float:
        json_object_object_add( characteristic, "value", json_object_new_double(value.floatValue));
        break;
      case ValueType_string:
        json_object_object_add( characteristic, "value", json_object_new_string(value.stringValue));
        break;
      case ValueType_date:
        json_object_object_add( characteristic, "value", json_object_new_string(value.stringValue));
        break;
      case ValueType_null:
        break;
      default:
        break;
    }  
  }

  if(needMeta){
     if(pCharacteristic.hasMinimumValue){
      switch(pCharacteristic.valueType){
        case ValueType_int:
          json_object_object_add( characteristic, "minValue",  json_object_new_int(pCharacteristic.minimumValue.intValue) );
          break;
        case ValueType_float:
          json_object_object_add( characteristic, "minValue",  json_object_new_double(pCharacteristic.minimumValue.floatValue) );
          break;
        default:
          break;
      }
    }

    if(pCharacteristic.hasMaximumValue){
      switch(pCharacteristic.valueType){
        case ValueType_int:
          json_object_object_add( characteristic, "maxValue",  json_object_new_int(pCharacteristic.maximumValue.intValue) );
          break;
        case ValueType_float:
          json_object_object_add( characteristic, "maxValue",  json_object_new_double(pCharacteristic.maximumValue.floatValue) );
          break;
        default:
          break;
      }
    }

    if(pCharacteristic.hasMinimumStep){
      switch(pCharacteristic.valueType){
        case ValueType_int:
          json_object_object_add( characteristic, "minStep",  json_object_new_int(pCharacteristic.minimumStep.intValue) );
          break;
        case ValueType_float:
          json_object_object_add( characteristic, "minStep",  json_object_new_double(pCharacteristic.minimumStep.floatValue) );
          break;
        default:
          break;
      }
    }
         
    if(pCharacteristic.hasMaxLength)
      json_object_object_add( characteristic, "maxLen",     json_object_new_int(pCharacteristic.maxLength));

    if(pCharacteristic.hasMaxDataLength)
      json_object_object_add( characteristic, "maxDataLen",     json_object_new_int(pCharacteristic.maxDataLength));

    if(pCharacteristic.description)
      json_object_object_add( characteristic, "description", json_object_new_string(pCharacteristic.description));

    if(pCharacteristic.format)
      json_object_object_add( characteristic, "format", json_object_new_string(pCharacteristic.format));

    if(pCharacteristic.unit)
      json_object_object_add( characteristic, "unit", json_object_new_string(pCharacteristic.unit));   
  }

  if(needperms){
    json_object *properties = json_object_new_array();
    if(pCharacteristic.secureRead)
      json_object_array_add( properties, json_object_new_string("pr") );
    if(pCharacteristic.secureWrite)
      json_object_array_add( properties, json_object_new_string("pw") );
    json_object_object_add( characteristic, "perms", properties);
  }

  if(needType){
    json_object_object_add( characteristic, "type", json_object_new_string(pCharacteristic.type));
  }

  if(needEv){
    if(pCharacteristic.hasEvents){
      if(HKNotificationFind(id.aid, id.iid, notifyList)==0)
        json_object_object_add( characteristic, "ev", json_object_new_boolean(1));
      else
        json_object_object_add( characteristic, "ev", json_object_new_boolean(0));
    }    
  }

  return hkErr;
}

void _HKCreateWritePerCharacteristic(struct _hapProduct_t inHapObject[], HK_Char_ID_t id, json_object *value_obj, _Bool moreComing, HK_Notify_t* notifyList)
{
  struct _hapCharacteristic_t pCharacteristic;
  value_union value;

  if(id.serviceID == 0 || id.characteristicID == 0)
    return;
  
  pCharacteristic = ((inHapObject->accessories[id.aid-1]).services[id.serviceID-1]).characteristic[id.characteristicID-1];
  
  if( pCharacteristic.secureWrite == 1 && id.serviceID && id.characteristicID ){
    switch(pCharacteristic.valueType ){
      case ValueType_bool:
        value.boolValue = json_object_get_boolean(value_obj);
        break;
      case ValueType_int:
        value.intValue = json_object_get_int(value_obj);
        break;
      case ValueType_float:
        value.floatValue = json_object_get_double(value_obj);
        break;
      case ValueType_string:
        value.stringValue = (char *)json_object_get_string(value_obj);
        break;
      case ValueType_date:
        value.stringValue = (char *)json_object_get_string(value_obj);
        break;
      case ValueType_null:
        break;
      default:
        break;
    }      
    HKWriteCharacteristicValue(id.aid, id.serviceID, id.characteristicID, value, moreComing); 
    HKNotificationUpdate( id.aid, id.iid, &value, notifyList );
    hk_bonjour_update_state_number( );  
  }
}

void _HKCreateWriteEVPerCharacteristic(struct _hapProduct_t *inHapObject, HK_Char_ID_t id, json_object *value_obj, HK_Notify_t** notifyList)
{
  struct _hapCharacteristic_t pCharacteristic;
  _Bool enableNotify = json_object_get_boolean(value_obj);
  value_union value;

  if(id.serviceID == 0 || id.characteristicID == 0)
    return;

  pCharacteristic = ((inHapObject->accessories[id.aid-1]).services[id.serviceID-1]).characteristic[id.characteristicID-1];
  
  if( pCharacteristic.hasEvents == 1 && id.serviceID && id.characteristicID ){
    if(enableNotify){
      FindCharacteristicByIID(inHapObject, id.aid, id.iid, &id.serviceID, &id.characteristicID);
      HKReadCharacteristicValue(id.aid, id.serviceID, id.characteristicID, &value);
      HKNotificationAdd(id.aid, id.iid, &value, notifyList);
    }else{
      HKNotificationRemove(id.aid, id.iid, notifyList);
    }
  }
}


HkStatus _HKCreateWriteResponsePerCharacteristic(struct _hapProduct_t *inHapObject, HK_Char_ID_t id, json_object *inHapReadRespondJson, _Bool check_value, _Bool check_event)
{
  HkStatus hkErr = 0;
  static json_object *characteristic;
  struct _hapCharacteristic_t pCharacteristic = ((inHapObject->accessories[id.aid-1]).services[id.serviceID-1]).characteristic[id.characteristicID-1];
  
  characteristic = json_object_new_object();
  json_object_array_add(inHapReadRespondJson, characteristic);
  json_object_object_add( characteristic, "aid", json_object_new_int(id.aid));
  json_object_object_add( characteristic, "iid", json_object_new_int(id.iid));

  if(id.serviceID == 0 || id.characteristicID == 0)
    return -70409;

  if(check_value){
    if(pCharacteristic.secureWrite == 0){
      hkErr = -70404;
    }else{
      hkErr = HKReadCharacteristicStatus(id.aid, id.serviceID, id.characteristicID);    
    }    
  }
  
  if(check_event){
    if(!pCharacteristic.hasEvents && hkErr == 0){
      hkErr = -70406;
    }
  }

  json_object_object_add( characteristic, "status", json_object_new_int(hkErr)); 

  return hkErr;
}

OSStatus HKhandleIncomeingMessage(int sockfd, HTTPHeader_t *httpHeader, HK_Notify_t** notifyList, HK_Context_t *inHkContext)
{
  OSStatus err = 0;
  HkStatus hkErr = 0;
  printbuf *buffer = 0;
  uint32_t idx;
  unsigned int arrayLen;
  err = HKSocketReadHTTPHeader( sockfd, httpHeader, inHkContext->session );
  json_object *characteristics, *characteristic, *outCharacteristics, *outCharacteristic, *event_obj;
  json_object *value_obj = 0;
  json_object *inhapJsonObject = 0, *outhapJsonObject = 0;
  int status = 200;
  HK_Char_ID_t id;
  mico_Context_t* mico_context = mico_system_context_get();

  switch ( err )
  {
    case 0:
        err = HKSocketReadHTTPBody( sockfd, httpHeader, inHkContext->session );
        do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );
         
        if(HTTPHeaderMatchURL( httpHeader, "/pair-setup" ) == 0) {
          err = HTTPHeaderMatchMethod( httpHeader, "POST");
          do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; { status = 405; } goto exit; } } while( 1==0 );

          err = HKPairSetupEngine( sockfd, httpHeader, &inHkContext->pairInfo );
          do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );
          if( 0 == HKPairSetupEngineIsRunning( ) ){err = -6753; goto exit;};
        }
          
        else if(HTTPHeaderMatchURL( httpHeader, "/pair-verify" ) == 0){
          err = HTTPHeaderMatchMethod( httpHeader, "POST");
          do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; { status = 405; } goto exit; } } while( 1==0 );

          if(inHkContext->pairVerifyInfo == 0){
            inHkContext->pairVerifyInfo = HKCreatePairVerifyInfo();
            do { if( ! !(!(inHkContext->pairVerifyInfo)) ) { ; { err = -6728; } goto exit; } } while( 1==0 );
          }
          err = HKPairVerifyEngine( sockfd, httpHeader, inHkContext->pairVerifyInfo );
          do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; { HKCleanPairVerifyInfo(&inHkContext->pairVerifyInfo); } goto exit; } } while( 1==0 );
          if(inHkContext->pairVerifyInfo->verifySuccess){
            inHkContext->session->established = 1;
            memcpy(inHkContext->session->InputKey,  inHkContext->pairVerifyInfo->C2AKey, 32);
            memcpy(inHkContext->session->OutputKey, inHkContext->pairVerifyInfo->A2CKey, 32);
            strncpy(inHkContext->session->controllerIdentifier, inHkContext->pairVerifyInfo->pControllerIdentifier, 64);
            HKCleanPairVerifyInfo(&inHkContext->pairVerifyInfo);
          }
        }
        else if(HTTPHeaderMatchURL( httpHeader, "/configured" ) == 0){
          ;
          

          
          

          
          
          
          HKSendResponseMessage(sockfd, 200, 0, 0, inHkContext->session);
          
          
          

          mico_context->micoSystemConfig.configured = allConfigured;
          mico_system_context_update( mico_context );

          mico_system_delegate_config_success( CONFIG_BY_WAC );
          system_easylink_wac_stop( );

        }
          
        else if(HTTPHeaderMatchURL( httpHeader, "/pairings" ) == 0){
          err = HTTPHeaderMatchMethod( httpHeader, "POST");
          do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; { status = 405; } goto exit; } } while( 1==0 );

          do { if( ! !(!(inHkContext->session->established == 1)) ) { ; { err = -6754; status = 470; } goto exit; } } while( 1==0 );

          err = HKPairAddRemoveEngine( sockfd, httpHeader, inHkContext->session );
          do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );
        }
         
        else if(HTTPHeaderMatchURL( httpHeader, "/identify" ) == 0){
          err = HTTPHeaderMatchMethod( httpHeader, "POST");
          do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; { status = 405; } goto exit; } } while( 1==0 );

          do { if( ! !(!(inHkContext->session->established == 0)) ) { ; { err = -6754; status = 400; hkErr = -70401; } goto exit; } } while( 1==0 );

          HKExcuteUnpairedIdentityRoutine( );

          HKSendResponseMessage(sockfd, 204, 0, 0, inHkContext->session);
          do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );
        }
         
        else if(HTTPHeaderMatchURL( httpHeader, "/accessories" ) == 0){
          err = HTTPHeaderMatchMethod( httpHeader, "GET");
          do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; { status = 405; } goto exit; } } while( 1==0 );

          do { if( ! !(!(inHkContext->session->established == 1)) ) { ; { err = -6754; status = 470; } goto exit; } } while( 1==0 );

          err = HKCreateHAPAttriDataBase(hapObjects, *notifyList, &outhapJsonObject);
          do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );
          buffer = json_object_to_json_string_ex(outhapJsonObject);
          ;
          json_object_put(outhapJsonObject);
          outhapJsonObject = 0;
          err = HKSendResponseMessage(sockfd, status, (uint8_t *)buffer->buf, strlen(buffer->buf), inHkContext->session);
          do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );
        }
         
        else if (HTTPHeaderMatchPartialURL( httpHeader, "/characteristics" ) != 0){

          do { if( ! !(!(inHkContext->session->established == 1)) ) { ; { err = -6754; status = 470; } goto exit; } } while( 1==0 );

          if(HTTPHeaderMatchMethod( httpHeader, "GET")!= -6727){ 
            hkErr = 0;
            outhapJsonObject = json_object_new_object();
            outCharacteristics = json_object_new_array();
            json_object_object_add( outhapJsonObject, "characteristics", outCharacteristics);

            void *metaPtr = memmem((void *)httpHeader->url.queryPtr, httpHeader->url.queryLen, "meta=", strlen("meta="));
            void *permsPtr = memmem((void *)httpHeader->url.queryPtr, httpHeader->url.queryLen, "perms=", strlen("perms="));
            void *typePtr = memmem((void *)httpHeader->url.queryPtr, httpHeader->url.queryLen, "type=", strlen("type="));
            void *evPtr = memmem((void *)httpHeader->url.queryPtr, httpHeader->url.queryLen, "ev=", strlen("ev="));
            
            void *idPtr = memmem((void *)httpHeader->url.queryPtr, httpHeader->url.queryLen, "id=", strlen("id="));
            do { if( ! !(!(idPtr)) ) { ; { err = -6727; status = 400; } goto exit; } } while( 1==0 );
            void *idPtrEnd = (void *)(httpHeader->url.queryPtr + httpHeader->url.queryLen);
            if(memchr(idPtrEnd, '&', (uint8_t *)idPtrEnd - (uint8_t *)idPtr)) idPtrEnd = memchr(idPtrEnd, '&', (uint8_t *)idPtrEnd - (uint8_t *)idPtr);

            _Bool needMeta = metaPtr? *(uint8_t *)((uint8_t *)metaPtr+strlen("meta="))-0x30 :0;
            _Bool needPerms = permsPtr? *(uint8_t *)((uint8_t *)permsPtr+strlen("perms="))-0x30 :0;
            _Bool needEv = evPtr? *(uint8_t *)((uint8_t *)evPtr+strlen("ev="))-0x30 :0;
            _Bool needType = typePtr? *(uint8_t *)((uint8_t *)typePtr+strlen("type="))-0x30 :0;


            const uint8_t *         src = (const uint8_t *) idPtr + strlen("id=");
            const uint8_t * const   end = idPtrEnd;

            while( IDGetNext( src, end, &id.aid, &id.iid, &src ) == 0 ) 
            {
              FindCharacteristicByIID(hapObjects, id.aid, id.iid, &id.serviceID, &id.characteristicID);
              if(id.characteristicID == 0){ 
                for(id.characteristicID = 1; id.characteristicID <= hapObjects->accessories[id.aid-1].services[id.serviceID-1].num_of_characteristics; id.characteristicID++){
                  if(hapObjects->accessories[id.aid-1].services[id.serviceID-1].characteristic[id.characteristicID-1].type == 0 )
                    break;

                  if(_HKCreateReadResponsePerCharacteristic(hapObjects, id, 
                                                            needMeta, needPerms, needType, needEv, *notifyList,
                                                            outCharacteristics)!=0)
                    status = 206;
                  id.characteristicID ++;
                }
              }else{ 
                if(_HKCreateReadResponsePerCharacteristic(hapObjects, id, 
                                                          needMeta, needPerms, needType, needEv, *notifyList,
                                                          outCharacteristics)!=0)
                  status = 206;
              }
            }
            
             
            if(status != 206){
              arrayLen = json_object_array_length(outCharacteristics);
              for(idx = 0; idx < arrayLen; idx++){
                outCharacteristic = json_object_array_get_idx(outCharacteristics, idx);
                json_object_object_del(outCharacteristic, "status");
              }
            }

             
            buffer = json_object_to_json_string_ex(outhapJsonObject);
            ;
            json_object_put(outhapJsonObject);
            outhapJsonObject = 0;
            err = HKSendResponseMessage(sockfd, status, (uint8_t *)buffer->buf, strlen(buffer->buf), inHkContext->session);
            do { OSStatus localErr; localErr = (OSStatus)(err); if( ! !(localErr != 0) ) { ; goto exit; } } while( 1==0 );
          }
         
        else if(HTTPHeaderMatchMethod( httpHeader, "PUT")!= -6727){
          inhapJsonObject = json_tokener_parse(httpHeader->extraDataPtr);
          do { if( ! !(!(inhapJsonObject)) ) { ; { err = -6742; } goto exit; } } while( 1==0 );
          characteristics = json_object_object_get(inhapJsonObject, "characteristics");
          do { if( ! !(!(characteristics)) ) { ; { err = -6742; } goto exit; } } while( 1==0 );
          do { if( ! !(!(json_object_is_type(characteristics, json_type_array))) ) { ; { err = -6742; } goto exit; } } while( 1==0 );

          hkErr = 0;
          status = 204;
          outhapJsonObject = json_object_new_object();
          outCharacteristics = json_object_new_array();
          json_object_object_add( outhapJsonObject, "characteristics", outCharacteristics);

           
          arrayLen = json_object_array_length(characteristics);
          for(idx = 0; idx < arrayLen; idx++){

            characteristic = json_object_array_get_idx(characteristics, idx);
            id.aid = json_object_get_int(json_object_object_get(characteristic, "aid"));
            id.iid = json_object_get_int(json_object_object_get(characteristic, "iid"));
            FindCharacteristicByIID(hapObjects, id.aid, id.iid, &id.serviceID, &id.characteristicID);      

            value_obj = json_object_object_get(characteristic, "value");
            event_obj = json_object_object_get(characteristic, "ev");

            if(value_obj){
              _HKCreateWritePerCharacteristic(hapObjects, id, value_obj, idx < arrayLen-1, *notifyList );
          }
            if(event_obj){
              do { if( ! !(!(json_object_is_type(event_obj, json_type_boolean))) ) { ; { err = -6742; } goto exit; } } while( 1==0 );
              _HKCreateWriteEVPerCharacteristic(hapObjects, id, event_obj, notifyList);
            }
          }

           
          for(idx = 0; idx < arrayLen; idx++){
            characteristic = json_object_array_get_idx(characteristics, idx);
            id.aid = json_object_get_int(json_object_object_get(characteristic, "aid"));
            id.iid = json_object_get_int(json_object_object_get(characteristic, "iid"));
            FindCharacteristicByIID(hapObjects, id.aid, id.iid, &id.serviceID, &id.characteristicID);

            value_obj = json_object_object_get(characteristic, "value");
            event_obj = json_object_object_get(characteristic, "ev");

            if(_HKCreateWriteResponsePerCharacteristic(hapObjects, id, outCharacteristics, value_obj, event_obj)!=0)
              status = 206;               
          }

           
          buffer = json_object_to_json_string_ex(outhapJsonObject);
          ;
          json_object_put(outhapJsonObject);
          outhapJsonObject = 0;
          json_object_put(inhapJsonObject);
          inhapJsonObject = 0;

          if(status == 204)
            HKSendResponseMessage(sockfd, status, 0, 0, inHkContext->session);
          else{
            if(arrayLen == 1)
              HKSendResponseMessage(sockfd, 400, (uint8_t *)buffer->buf, strlen(buffer->buf), inHkContext->session);
            else
              HKSendResponseMessage(sockfd, status, (uint8_t *)buffer->buf, strlen(buffer->buf), inHkContext->session);
          }

        }else
         
          err = -1;
          status = 400;
          goto exit;
      }
         
      else{
        err = -6727;
        status = 404;
        goto exit;
      }

    break;

    case 11:
        
    break;

    case -6763:
      status = 400;
      ;
      goto exit;
    break;

    case -6753:
      
      ;
      goto exit;
    break;
    default:
      status = 400;
      ;
      goto exit;
  }
exit:
  if( err != 0 && status != 200 ){
    outhapJsonObject = json_object_new_object();
    json_object_object_add( outhapJsonObject, "status", json_object_new_int(hkErr));
    buffer = json_object_to_json_string_ex(outhapJsonObject);
    ;
    HKSendResponseMessage(sockfd, status, (uint8_t *)buffer->buf, strlen(buffer->buf), inHkContext->session);
    mico_rtos_delay_milliseconds(100);
  }
  
  HTTPHeaderClear( httpHeader );
  if(outhapJsonObject) json_object_put(outhapJsonObject);
  if(inhapJsonObject) json_object_put(inhapJsonObject);
  if(buffer) printbuf_free(buffer);
  return err;

}

