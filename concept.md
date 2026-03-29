# CXPUI - C Cross-Platform UI - Concept

## A few structs are holding things together

- ```GraphicsContext```
- ```View```
- ```CustomView```
- ```Handle```

## There are two sides to these

- View Path
- User Code

When creating a View / CustomView Handle, the user receives a Handle.

## Ownership is dual

- owned: the view is part of a view graph
- held: the handle is held by user code, i.e., the handle has not been released

## Ownership Flow

The view is freed (and releases ownership of its sub-views) at the moment that neither condition applies anymore.

The default add method, adding a view to another view, will not return a held handle. Access will be possible, but access always checks that either owned, held, or both.

Default add transfers from held to owned.

The e.g. Java representation will set its handle pointer to NULL, so Java itself blocks access after normal add.

Something like addAndHold is necessary to still do stuff with the new view from the user side after adding.

In C, the add method will actually be a macro that sets the handle pointer to NULL after calling the holding add method.

In, e.g., Java, the wrapper just will throw when accessing, as it doesn't have an actual handle pointer anyway.

## Ownership-related methods

- create → returns a held handle
- add → transfers held → owned, invalidates caller access
- addAndHold → owned + still held
- release → drops the user hold
- remove → drops graph ownership

## Rationale

CXPUI uses deterministic native ownership instead of GC-driven resource cleanup. UI objects may be owned by the view graph and/or held by user code, and they are released immediately when neither condition applies. This avoids relying on finalizers or runtime memory pressure to free large native resources such as image buffers, textures, or platform widgets.

In binding languages, wrapper objects only control access; they are not the source of truth for lifetime. This keeps the API predictable, binding-friendly, and safe for long-running applications.

## Plan as of now

Beyond the C core, I'll do UIKit and JVM first, and then let the LLM fill in Pythn, Gtk4...

Once there is already a code base, soo many fewer questions.
