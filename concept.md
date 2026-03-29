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

### Two things will be especially fascinating for LLM generation

- Built-in file picker (will need to have ifdef includes for posix / win32, of course).
- And the Gtk4 backend...

The latter will need to be done in two versions: C and Vala.

Because the API is clearly designed around Vala / gobject, and doing thet from C can be nasty...

But I'm already used to CPython, so let's see which will be nicer ^^

CPython and JNI are only examples why I'm not afraid of gtk4 in C.

But I've done the beginnings of the UIKit backend in Swift, which interacts nicely with C via Objective-C ^^

### A remark to an LLM

What would be great is a ref-counted, statically typed very nice language, but we don't have one.
I'm currently working on a dynamically-typed one, kinda JS-inspired, much stricter and more explicit (no implicit up values).
But that is for small soft-real-time projects.
Dynamic typing and behemoth-ish SCADA don't mix.
I plan on making a statically typed version, but that's for later, and once *that* is there, the tooling will initially be abysmal, of course 😎
So, yeah, just accept the GC as a fact, I'm isolating it quite well already anyway.
And I'll just prototype it all in Java, Go, and G# with a neat example ^^
I guess now you see why I'm not always enthusiastic about initial LLM suggestions in my projects, there's just soo much context to explain. 
