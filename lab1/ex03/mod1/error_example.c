make -C /lib/modules/4.15.0-29-generic/build M=/home/cms/cms/embedded_soft/lab2/ex03/mod1 modules
make[1]: 디렉터리 '/usr/src/linux-headers-4.15.0-29-generic' 들어감
  CC [M]  /home/cms/cms/embedded_soft/lab2/ex03/mod1/mod1.o
In file included from /home/cms/cms/embedded_soft/lab2/ex03/mod1/mod1.c:1:0:
./include/linux/module.h:129:42: error: redefinition of ‘__inittest’
  static inline initcall_t __maybe_unused __inittest(void)  \
                                          ^
/home/cms/cms/embedded_soft/lab2/ex03/mod1/mod1.c:19:1: note: in expansion of macro ‘module_init’
 module_init(mod1_exit);
 ^~~~~~~~~~~
./include/linux/module.h:129:42: note: previous definition of ‘__inittest’ was here
  static inline initcall_t __maybe_unused __inittest(void)  \
                                          ^
/home/cms/cms/embedded_soft/lab2/ex03/mod1/mod1.c:18:1: note: in expansion of macro ‘module_init’
 module_init(mod1_init);
 ^~~~~~~~~~~
/home/cms/cms/embedded_soft/lab2/ex03/mod1/mod1.c: In function ‘__inittest’:
/home/cms/cms/embedded_soft/lab2/ex03/mod1/mod1.c:19:13: error: return from incompatible pointer type [-Werror=incompatible-pointer-types]
 module_init(mod1_exit);
             ^
./include/linux/module.h:130:11: note: in definition of macro ‘module_init’
  { return initfn; }     \
           ^~~~~~
/home/cms/cms/embedded_soft/lab2/ex03/mod1/mod1.c: At top level:
./include/linux/module.h:131:6: error: redefinition of ‘init_module’
  int init_module(void) __attribute__((alias(#initfn)));
      ^
/home/cms/cms/embedded_soft/lab2/ex03/mod1/mod1.c:19:1: note: in expansion of macro ‘module_init’
 module_init(mod1_exit);
 ^~~~~~~~~~~
./include/linux/module.h:131:6: note: previous definition of ‘init_module’ was here
  int init_module(void) __attribute__((alias(#initfn)));
      ^
/home/cms/cms/embedded_soft/lab2/ex03/mod1/mod1.c:18:1: note: in expansion of macro ‘module_init’
 module_init(mod1_init);
 ^~~~~~~~~~~
cc1: some warnings being treated as errors
scripts/Makefile.build:339: recipe for target '/home/cms/cms/embedded_soft/lab2/ex03/mod1/mod1.o' failed
make[2]: *** [/home/cms/cms/embedded_soft/lab2/ex03/mod1/mod1.o] Error 1
Makefile:1552: recipe for target '_module_/home/cms/cms/embedded_soft/lab2/ex03/mod1' failed
make[1]: *** [_module_/home/cms/cms/embedded_soft/lab2/ex03/mod1] Error 2
make[1]: 디렉터리 '/usr/src/linux-headers-4.15.0-29-generic' 나감
Makefile:7: recipe for target 'default' failed
make: *** [default] Error 2

