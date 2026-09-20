# Clipo Roadmap

Clipo is being developed as a fast, lightweight and local-first
clipboard manager.

The roadmap is divided into releases so that each version has a clear
scope.

---

## v1.0.0 — Text Clipboard Manager

**Status: Release Candidate**

The first release focuses on a fast and reliable text clipboard
workflow.

### Clipboard

- [x] Automatic clipboard monitoring
- [x] Text clipboard history
- [x] Persistent SQLite storage
- [x] Duplicate content timestamp update
- [x] Clear clipboard history
- [x] Live history refresh

### Search

- [x] Clipboard history search
- [x] Search field
- [x] Clear search control

### Keyboard Workflow

- [x] Global keyboard shortcut
- [x] Configurable global shortcut
- [x] Arrow-key navigation
- [x] Enter-to-paste
- [x] Escape-to-close

### User Interface

- [x] Compact popup
- [x] Smart popup positioning
- [x] Movable popup
- [x] Light theme
- [x] Dark theme
- [x] System theme
- [x] Configurable transparency
- [x] Settings dialog
- [x] Clipboard item expansion
- [x] No horizontal history scrolling

### Linux

- [x] X11 global shortcut
- [x] X11 paste automation
- [x] Ubuntu/Debian installation
- [x] Debian package generation
- [x] `.deb` distribution
- [x] Desktop application entry
- [x] Application icon

### Documentation

- [x] Installation documentation
- [x] Build documentation
- [x] Usage documentation
- [x] Architecture documentation
- [x] Development roadmap

---

# v2.0.0 — Files and Images

**Status: Planned**

The second major version will expand clipboard support beyond text.

## Images

- [ ] Image clipboard detection
- [ ] Image storage
- [ ] Image thumbnails
- [ ] Image preview
- [ ] Image restoration/paste

## Files

- [ ] File clipboard detection
- [ ] File icons
- [ ] File previews
- [ ] Multiple-file clipboard entries
- [ ] File restoration/paste

## Clipboard Formats

- [ ] MIME type handling
- [ ] Improved clipboard format detection
- [ ] Rich text support
- [ ] HTML clipboard support

## Storage

- [ ] Storage usage information
- [ ] Configurable retention
- [ ] Automatic cleanup
- [ ] Improved database storage management
- [ ] Storage limits

---

# Future Releases

The following features are being considered for future versions.

## Platform Support

- [ ] Native Windows support
- [ ] Native Wayland support
- [ ] Platform-specific paste backends
- [ ] Platform-specific global shortcut implementations
- [ ] Additional Linux desktop environment support

## Clipboard Features

- [ ] Clipboard item pinning
- [ ] Favorites
- [ ] Tags
- [ ] Advanced search
- [ ] Clipboard item categories
- [ ] Rich clipboard formats

## History Management

Possible history-management options include:

- Persistent history
- Current-session-only history
- Clear history on application exit
- Clear history on startup
- Automatic retention periods
- Maximum history size

These options will be designed carefully because application shutdown is
not guaranteed during power loss or system crashes.

## User Experience

- [ ] Improved animations
- [ ] More keyboard shortcuts
- [ ] Customizable popup layouts
- [ ] Improved accessibility
- [ ] Better high-DPI support
- [ ] Improved screen-edge positioning

## Performance

- [ ] Additional startup optimizations
- [ ] Memory usage improvements
- [ ] Large-history optimization
- [ ] Database maintenance improvements
- [ ] More efficient storage handling

---

# Development Philosophy

Clipo aims to remain:

- Fast
- Lightweight
- Local-first
- Privacy-friendly
- Keyboard-friendly
- Native
- Open source

New features should avoid unnecessarily increasing background CPU or
memory usage.

Platform-specific functionality should remain isolated so that additional
platform implementations can be introduced without unnecessarily
changing the core application.

---

# Contributing

Contributions are welcome.

Before implementing a large feature:

1. Check existing issues.
2. Discuss the proposed change when appropriate.
3. Keep platform-specific code isolated.
4. Avoid unnecessary dependencies.
5. Add documentation for user-facing features.
6. Test the application before submitting changes.
7. Keep changes focused and reviewable.

For larger changes, contributors should also consider whether the change
affects:

- The database schema
- Platform-specific behavior
- Packaging
- User settings
- Documentation
- Backward compatibility

---

# Versioning

Clipo follows semantic versioning:

```text
MAJOR.MINOR.PATCH
```

For example:

```text
1.0.0
1.1.0
2.0.0
```

Major releases may introduce significant functionality or architectural
changes.

Minor releases add backward-compatible functionality.

Patch releases focus on bug fixes and small improvements.

---

# License

Clipo is released under the MIT License.

See the repository's `LICENSE` file for the complete license text.
