#import "AppDelegate.h"
#import <AppKit/AppKit.h>

@implementation AppDelegate

@property (nonatomic, strong) NSWindow *window;
@property (nonatomic, strong) CustomView *customView;

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    NSRect screenRect = [NSScreen mainScreen].frame;

    self.window = [[NSWindow alloc] initWithContentRect:NSMakeRect(
                                                            NSMidX(screenRect) - 200,
                                                            NSMidY(screenRect) - 200,
                                                            400,
                                                            200)
                                              styleMask:(NSWindowStyleMaskTitled |
                                                        NSWindowStyleMaskClosable |
                                                        NSWindowStyleMaskResizable)
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];

    self.window.title = @"cxpui Appkit Window";
    [self.window center];
    [self.window makeKeyAndOrderFront:nil];

    // Add the custom drawing view
    self.customView = [[CustomView alloc] initWithFrame:self.window.contentView.bounds];
    self.customView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    self.customView.drawFunction = ^(GraphicsContext context) {
        draw(context);
    };
    [self.window.contentView addSubview:self.customView];

    // Activate the application
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];

    self.window.delegate = self;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

@end

// Custom NSView for custom paint logic
@implementation CustomView

- (void)setDrawFunction:(void (^)(GraphicsContext))drawFunction {
    _drawFunction = drawFunction;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];

    // Your custom drawing logic here
    CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
    if (!context) return;

    globalContext = context;
    globalBounds = self.bounds;

    // Clear the background with white color
    [[NSColor whiteColor] setFill];
    CGContextFillRect(context, self.bounds);

    if (self.drawFunction) {
        self.drawFunction((GraphicsContext){
            .width = self.bounds.size.width,
            .height = self.bounds.size.height
        });
    }

    globalContext = nil;
    globalBounds = CGRectZero;
}

@end

static AppDelegate *delegate = nil;
CGContextRef globalContext = nil;
CGRect globalBounds;

Handle *start(void) {
    NSApplication *app = [NSApplication sharedApplication];
    delegate = [[AppDelegate alloc] init];
    app.delegate = delegate;
    [app run];
    void *context = (__bridge void *)delegate;
    NativeView *view = NativeView_create(context);
    return view->base.handle;
}

CustomView *CustomView_create(void) {
    CustomView *instance = [[CustomView alloc] init];
    return instance;
}

void line(CGFloat x1, CGFloat y1, CGFloat x2, CGFloat y2) {
    [[NSColor blueColor] setStroke];
    CGContextSetLineWidth(globalContext, 2);
    CGContextMoveToPoint(globalContext, x1, y1);
    CGContextAddLineToPoint(globalContext, x2, y2);
    CGContextStrokePath(globalContext);
}

void rect(CGFloat x, CGFloat y, CGFloat w, CGFloat h) {
    [[NSColor redColor] setFill];
    CGContextFillRect(globalContext, NSMakeRect(x, y, w, h));
}
