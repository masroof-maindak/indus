# Indus

Little old downright-abhorrent C shell. I don't want to be too hard on it because it was my first C project, but Lord knows this code doesn't deserve to see the light of day.

Apart from just plain-stupid shit like getting ideas/implementation details from the Bash source, or `typdef`-ing an integer to a bool, the project's also quite lacking in terms of functionality because I got caught up with stupid shit like perfecting printing the prompt, and thinking that certain binaries were a part of the shell...

> If the reader, for some reason, attempts to build this project, be aware that the first character of the input is, for some reason, not printed.
>
> I do not know why this happens.
>
> I could have sworn that at least the prompt and shit used to work perfectly, and I am not quite certain as to why they've regressed.
>
> Oh well.

## TODOs

- [ ] ALIASES
- [ ] Parse arguments - `yacc`/`bison`?
    - [ ] Tokenize string-quoted input for mkdir/trash, etc.
- [ ] Built-ins
    - [ ] mkdir (FIXME)
        - [ ] Implied `-p` flag
    - [ ] ls - refactor
- [ ] Pipes - `|`
- [ ] Conditionals - `&&`, `||`
- [ ] File redirection (output) - `>>`, `>`
- [ ] Asynchronous execution - `&`
- [ ] Autocompletion -> Edit Distance algo? (Levenshtein/Jaro-Wrinkler)
