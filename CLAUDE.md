## Style

- 2-spaces for indentation
- `PascalCase` class names
- `CamelCase` method names and variable names
- `snake_case` for type aliases
- Split classes into header and implementation files when possible

## Language

- C++23
- Be as const-correct as possible
- Prefer trailing return types and braces on same line for functions `constexpr auto function() -> type {`

## Code Generation

- Templates are found in `/templates`
  - Use them as reference when asked 
  - For example: When asked "Create logging subsystem" then `/templates/subsystem` will be your starting point for class and file structure