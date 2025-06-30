import AppKit

class AppDelegate: NSObject, NSApplicationDelegate, NSWindowDelegate {
    var window: NSWindow!
    var customView: CustomView!

    func applicationDidFinishLaunching(_ notification: Notification) {
        let screenRect = NSScreen.main?.frame ?? NSRect()

        window = NSWindow(
            contentRect: NSMakeRect(
                screenRect.midX - 200,
                screenRect.midY - 200,
                400,
                200
            ),
            styleMask: [.titled, .closable, .resizable],
            backing: .buffered,
            defer: false
        )
        window.title = "cxgui Appkit Window"
        window.center()
        window.makeKeyAndOrderFront(nil)

        // Add the custom drawing view
        customView = CustomView(frame: window.contentView?.bounds ?? NSRect.zero)
        customView.autoresizingMask = [.width, .height] // Resize the view with the window
        customView.drawFunction = draw
        window.contentView?.addSubview(customView)

        // Activate the application
        NSApplication.shared.activate(ignoringOtherApps: true)

        window.delegate = self
    }

    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }
}

// Custom NSView for custom paint logic
class CustomView: NSView {
    var drawFunction: ((Context) -> ())!

    func setDrawFunction(drawFunction: @escaping (Context) -> ()) {
        self.drawFunction = drawFunction
    }

    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)

        // Your custom drawing logic here
        guard let context = NSGraphicsContext.current?.cgContext else { return }

        defer {
            globalContext = nil
            globalBounds = CGRect()
        }

        globalContext = context
        globalBounds = bounds

        // Clear the background with white color
        NSColor.white.setFill()
        context.fill(bounds)

//        // Draw a red rectangle
//        NSColor.red.setFill()
//        let rect = NSRect(x: bounds.midX - 50, y: bounds.midY - 25, width: 100, height: 50)
//        context.fill(rect)
//
//        // Draw a black diagonal line
//        NSColor.black.setStroke()
//        context.setLineWidth(2)
//        context.move(to: CGPoint(x: bounds.minX, y: bounds.minY))
//        context.addLine(to: CGPoint(x: bounds.maxX, y: bounds.maxY))
//        context.strokePath()
//
//        NSColor.red.setStroke()
//            globalContext.setLineWidth(2)
//            globalContext.move(to: CGPoint(x: 0, y: 0))
//            globalContext.addLine(to: CGPoint(x: 50, y: 50))
//            globalContext.strokePath()
        self.drawFunction(Context(width: bounds.size.width, height: bounds.size.height))
    }
}

var delegate: AppDelegate! = nil
var globalContext: CGContext! = nil
var globalBounds: CGRect!

@_cdecl("start")
func start() {
    let app = NSApplication.shared
    delegate = AppDelegate()
    app.delegate = delegate
    app.run()
}

@_cdecl("line")
func line(x1: CGFloat, y1: CGFloat, x2: CGFloat, y2: CGFloat) {
    NSColor.blue.setStroke()
    globalContext.setLineWidth(2)
    globalContext.move(to: CGPoint(x: x1, y: y1))
    globalContext.addLine(to: CGPoint(x: x2, y: y2))
    globalContext.strokePath()
}

@_silgen_name("draw")
func draw(context: Context)
