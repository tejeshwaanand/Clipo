# Contributing to Clipo

Thank you for your interest in contributing to Clipo.

Clipo is an open-source project and welcomes bug fixes, documentation improvements, testing, UI improvements, and new platform support.

## Development Setup

Follow [docs/BUILDING.md](docs/BUILDING.md) to install the required dependencies and build Clipo from source.

Basic build:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

Run the development build:

```bash
./build/clipo
```

## Project Structure

```text
src/
├── clipboard/    Clipboard monitoring and history handling
├── database/     SQLite persistence
├── platform/     Platform-specific functionality
├── settings/     Application settings
└── ui/           Qt user interface
```

Keep platform-specific code inside the `platform/` area whenever possible.

## Making Changes

Before starting a large change, open an issue to discuss the proposed design.

For smaller changes:

1. Create a branch.
2. Make the change.
3. Build the project.
4. Test the affected functionality.
5. Update documentation if required.
6. Commit the changes.
7. Open a pull request.

Example:

```bash
git checkout -b feature/my-change
```

## Code Style

Clipo uses modern C++20 and Qt6.

Prefer:

- Clear and descriptive names
- Small, focused functions
- RAII and Qt's ownership model
- `const` where appropriate
- Explicit error handling
- Comments explaining non-obvious decisions

Avoid:

- Unnecessary global state
- Blocking the UI thread
- Polling when an event-driven solution is available
- Platform-specific code in shared application logic
- Unrelated changes in the same commit

## UI Changes

Clipo is intended to remain lightweight and keyboard-friendly.

When changing the UI:

- Keep the interface compact.
- Preserve keyboard navigation.
- Avoid unnecessary animations.
- Keep dark and light themes consistent.
- Do not introduce horizontal scrolling for normal clipboard history.
- Consider accessibility and readability.

## Clipboard and Privacy

Clipboard contents can contain sensitive information.

Changes involving clipboard data should:

- Avoid unnecessary external communication.
- Keep clipboard data local unless a future feature explicitly requires otherwise.
- Avoid logging clipboard contents.
- Consider the effect of storing sensitive clipboard data.

## Database Changes

SQLite is used for local clipboard history.

When modifying the database:

- Keep schema changes deliberate.
- Preserve existing user data where practical.
- Avoid destructive migrations without a clear migration strategy.
- Test database operations with realistic clipboard history.

## Platform Support

Clipo is intended to support Linux and Windows.

Linux currently contains platform-specific X11 implementations for functionality such as global shortcuts and paste simulation.

When adding platform-specific behavior, keep it isolated so that other platforms are not unnecessarily affected.

## Testing

At minimum, test:

- Application startup
- Clipboard capture
- History insertion
- Duplicate clipboard handling
- Search
- Keyboard navigation
- Paste
- Clear history
- Settings
- Global shortcut
- Application restart

For Linux/X11, also test the behavior in an actual X11 session.

## Commit Messages

Use concise commit messages that describe the change.

Examples:

```text
Add configurable global shortcut
Fix live clipboard history refresh
Improve popup positioning
Add Debian packaging
Update installation documentation
```

## Pull Requests

A pull request should include:

- A clear description of the change
- The reason for the change
- Testing performed
- Screenshots for significant UI changes
- Documentation updates when applicable

Keep pull requests focused on one logical change where possible.

## Reporting Bugs

When reporting a bug, include:

- Operating system and version
- Desktop environment
- Display session type, when relevant
- Clipo version or commit
- Steps to reproduce
- Expected behavior
- Actual behavior
- Relevant terminal output or logs

Do not include passwords, tokens, private clipboard contents, or other sensitive information.

## Feature Requests

Feature requests are welcome.

Please describe:

- The problem the feature solves
- The expected user workflow
- Why the feature fits Clipo
- Any platform-specific considerations

Large features should be discussed before implementation.

## License

By contributing to Clipo, you agree that your contributions may be distributed under the project's MIT License.

See [LICENSE](LICENSE) for the complete license text.
