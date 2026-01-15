# Cross platform UI

## Foundational elements

- an IDL for defining data types (structs, enums that can use lists, sets, maps)
- a *language* named tiny, a statically typed version of another language named mini
- projection of IDL types and tiny code into target languages

## The three pillars of the actual UI

- super high-level UI written in tiny against a rudimentary API for logical layout, dialogs, &c.
- simple custom UI elements written in tiny against a drawing API and the typical input events
- target-specific native UI elements and code when required

## Tiny code

The code written in tiny will never run except inside unit tests.

It is projected to the target languages.

It assumes reference counting without automatic resource management, so developers will neither create reference cycles nor rely on RAII.
