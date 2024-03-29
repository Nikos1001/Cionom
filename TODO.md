# TODO

- Github
  - Pages for documentation
  - Start doing semver releases (Fix up a *hecc* tonne of TODOs before doing this!)
    - Move off upstream into dev branch and add branch protection to trunk
    - Place version (+magic?) into bytecode modules so VM can verify version <= runtime version
- Implementation
  - Extension: specify libs to look in for extsyms
  - Rewrite documentation
  - Add `~~~~~^` style markers to diagnostic output
  - Decompose header data structure into usable data
    - Extension data
  - Fuzzing
  - Add VSC grammar for `.cas` files
  - Add all docs from Esolang wiki
    - Routine/Stack indices
    - Document default packaged library
    - Add little opener to `README.md`
  - Expose bundles in lib API
  - Document "Source-Only Extensions"
  - Debugger
    - Breakponts
    - Debug info
      - Cíonom backtraces vs. Genstone backtraces
    - Disassembly
  - Write some tutorials
  - Document debug info format
  - String hashing in VM
  - Update Esolang wiki
  - Add generalized genstone backends system
  - Fix inconsistencies in naming between "length" and "bounds"
  - Do checks for CLI params with no effect/raw args to operations which ignore them
  - Do parameter checking on `warning_settings` and document it as a parameter
  - Review buffer bounding negligence (e.g. ignoring `source_file_length`)
  - "try-catch" esque. mechanism for catching Genstone errors in Cíonom
    - Update the catch divide-by-zero example
  - Signals/SEH for genthreads
    - Apply to extlib exception handlers
