# TODO

- Github
  - Set up code scanning
  - Pages for documentation
    - Autogenerate documentation
  - Set up all community guidelines stuff
- Implementation
  - Add `~~~~~^` style markers to diagnostic output
    - Add `cio_glog_column_indicator` to `cionom.c`
  - Add AST and Token list cleanup functions 
    - Ensure they work even after failure to completely tokenize/parse (User calls on failure)
  - Bytecode generation is disgusting
    - Especially with allocations
- Specification
  - Rewrite semantics for new stuff