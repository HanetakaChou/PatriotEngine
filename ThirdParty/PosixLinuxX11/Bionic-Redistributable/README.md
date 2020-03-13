### place the linker at cwd  

the Linux kernel use openat(AT_FDCWD, ...) to open the linker  
\-\- load_elf_binary \#\#\# fs/binfmt_elf.c  
\-\-\-\- open_exec(elf_interpreter) \#\#\# fs/binfmt_elf.c  
\-\-\-\- open_exec(name) \#\#\# fs/exec.c  
\-\-\-\-\-\- do_open_execat(AT_FDCWD, filename, ...) \#\#\# fs/exec.c  
and thus we must place the linker at cwd  
