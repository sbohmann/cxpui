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
        window.title = "Hello World App with Custom Drawing"
        window.center()
        window.makeKeyAndOrderFront(nil)

        // Add the custom drawing view
        customView = CustomView(frame: window.contentView?.bounds ?? NSRect.zero)
        customView.autoresizingMask = [.width, .height] // Resize the view with the window
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
    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)

        // Your custom drawing logic here
        guard let context = NSGraphicsContext.current?.cgContext else { return }

        // Clear the background with white color
        NSColor.white.setFill()
        context.fill(bounds)

        // Draw a red rectangle
        NSColor.red.setFill()
        let rect = NSRect(x: bounds.midX - 50, y: bounds.midY - 25, width: 100, height: 50)
        context.fill(rect)

        // Draw a black diagonal line
        NSColor.black.setStroke()
        context.setLineWidth(2)
        context.move(to: CGPoint(x: bounds.minX, y: bounds.minY))
        context.addLine(to: CGPoint(x: bounds.maxX, y: bounds.maxY))
        context.strokePath()
    }
}

var delegate: AppDelegate! = nil

@_cdecl("start")
func start() {
    let app = NSApplication.shared
    delegate = AppDelegate()
    app.delegate = delegate
    app.run()
}

@_cdecl("line")
func line(x1: Float, y1: Float, x2: Float, y2: Float) {
    // Placeholder implementation (for expanding custom drawing if needed)
}