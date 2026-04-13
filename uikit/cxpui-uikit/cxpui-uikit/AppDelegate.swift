import AppKit

class AppDelegate: NSObject, NSApplicationDelegate, NSWindowDelegate {
    var window: NSWindow?
    var windowView: WindowView?

    func applicationDidFinishLaunching(_ notification: Notification) {
        // Activate the application
        NSApplication.shared.activate(ignoringOtherApps: true)
    }

    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }
}

class WindowView: NSView {
    var cxpuiWindow: UnsafeMutablePointer<Window>?
    
    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)

        // Your custom drawing logic here
        guard let context = NSGraphicsContext.current?.cgContext else { return }

        let graphicsContext = context
        let bounds = bounds

        // Clear the background with white color
        NSColor.white.setFill()
        context.fill(bounds)
        
//        if let win = self.cxpuiWindow {
////            if win.pointee.base.type == CustomView {
////                
////            }
////            if cxpuiWindow?.pointee.base.type
////            self.drawFunction(
////                GraphicsContext(
////                    width: bounds.size.width,
////                    height: bounds.size.height))
//        }
    }
}

var globalAppdelegate: AppDelegate! = nil
//var globalContext: CGContext = nil
//var globalBounds: CGRect

@_cdecl("start")
func start() -> UnsafeMutablePointer<Window> {
    let app = NSApplication.shared
    globalAppdelegate = AppDelegate()
    app.delegate = globalAppdelegate
    let view = createWindow()
    return view
}

@_cdecl("Application_run")
func run() {
    NSApplication.shared.run()
}

func createWindow() -> UnsafeMutablePointer<Window> {
    let screenRect = NSScreen.main?.frame ?? NSRect()
    
    let window = NSWindow(
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
    window.title = "cxpui Appkit Window"
    window.center()
    window.delegate = globalAppdelegate
    window.makeKeyAndOrderFront(nil)

    let windowView = WindowView(frame: window.contentView?.bounds ?? NSRect.zero)
    windowView.autoresizingMask = [.width, .height]
    window.contentView?.addSubview(windowView)
    var cxpuiWindow = Window()
    cxpuiWindow.mainView = nil
    cxpuiWindow.native_instance = Unmanaged.passRetained(window).toOpaque()
    let cxpuiWindowPointer = UnsafeMutablePointer<Window>.allocate(capacity: 1)
    cxpuiWindowPointer.initialize(to: cxpuiWindow)
    return cxpuiWindowPointer
}

@_cdecl("Window_set_main_view")
func setMainView(window: UnsafeMutablePointer<Window>, view: UnsafeMutablePointer<View>) {
    window.pointee.mainView = view
}

@_cdecl("line")
func line(x1: CGFloat, y1: CGFloat, x2: CGFloat, y2: CGFloat) {
//    NSColor.blue.setStroke()
//    globalContext.setLineWidth(2)
//    globalContext.move(to: CGPoint(x: x1, y: y1))
//    globalContext.addLine(to: CGPoint(x: x2, y: y2))
//    globalContext.strokePath()
}

@_cdecl("rect")
func rect(x: CGFloat, y: CGFloat, w: CGFloat, h: CGFloat) {
//    NSColor.red.setFill()
//    globalContext.fill(NSRect(x: x, y: y, width: w, height: h))
}

@_silgen_name("draw")
func draw(context: GraphicsContext)
