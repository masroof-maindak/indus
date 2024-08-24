### TODO

- [x] Fetching cwd/username
- [x] prompt generation functions
- [x] getcwd()
- [/] Execution loop
    - [x] Input
    - [x] Parse arguments
    - [ ] fork()/exec() calls to run program
- [/] Built-ins
    - [/] ls
        - [x] Print directories first, then files, in sorted order
        - [/] Colour directories & symlinks differently
        - [ ] `-a` flag; should work in any order
    - [/] mkdir
        - [ ] `-p` flag
    - [/] help
        - [ ] Help messages for all other builtins
        - [ ] Better formatting; bold builtin names, coloured heading, etc
    - [/] trash
    - [x] exit
    - [x] pwd
    - [x] clear
    - [x] whoami
    - [x] cd
    - [x] Function Pointer Array
- [ ] Pipes - `|`
- [ ] Conditionals - `&&`, `||`
- [ ] File redirection (output) - `>>`, `>`
- [ ] Asynchronous execution - `&`
- [ ] autocompletion -> Edit Distance algo (Levenshtein/Jaro Wrinkler)
- [-] Abbreviations like Fish
